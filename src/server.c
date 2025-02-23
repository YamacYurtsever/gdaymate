#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>

#include "server.h"
#include "server_process.h"
#include "gdmp.h"
#include "thread_pool.h"

struct receive_message_arg {
    Server srv;
    int client_sockfd;
};

int setup_server(Server srv);
void free_server(Server srv);
int get_client(Server srv);
int add_client(Server srv, int client_sockfd);
int remove_client(Server srv, int client_sockfd);
int add_poll(Server srv, int client_sockfd);
int remove_poll(Server srv, int client_sockfd);
int check_poll(Server srv);
void *receive_message(void *arg);

////////////////////////////////// FUNCTIONS ///////////////////////////////////

Server ServerNew(void) {
    Server srv = malloc(sizeof(struct server));
    if (srv == NULL) {
        perror("malloc");
        return NULL;
    }

    srv->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv->sockfd == -1) {
        perror("socket");
        free(srv);
        return NULL;
    }

    srv->clients = malloc(sizeof(int) * SERVER_MAX_CLIENT_COUNT);
    if (srv->clients == NULL) {
        perror("malloc");
        close(srv->sockfd);
        free(srv);
        return NULL;
    }

    srv->client_count = 0;

    srv->poll_set = malloc(sizeof(struct pollfd) * SERVER_MAX_POLL_COUNT);
    if (srv->poll_set == NULL) {
        perror("malloc");
        free(srv->clients);
        close(srv->sockfd);
        free(srv);
        return NULL;
    }

    srv->poll_count = 0;

    srv->pool = ThreadPoolNew(SERVER_THREAD_COUNT);
    if (srv->pool == NULL) {
        fprintf(stderr, "ThreadPoolNew: error\n");
        free(srv->poll_set);
        free(srv->clients);
        close(srv->sockfd);
        free(srv);
        return NULL;
    }

    atomic_store(&srv->shutdown, false);

    pthread_mutex_init(&srv->lock, NULL); 

    // Setup the server
    int res = setup_server(srv);
    if (res == -1) {
        fprintf(stderr, "setup_server: error\n");
        free_server(srv);
        return NULL;
    }

    return srv;
}

void ServerFree(Server srv) {
    atomic_store(&srv->shutdown, true);
}

int ServerStart(Server srv) {
    // Start listening for incoming connections
    int res = listen(srv->sockfd, SERVER_MAX_BACKLOG);
    if (res == -1) {
        perror("listen");
        return -1;
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    while (!atomic_load(&srv->shutdown)) {
        // Wait until a socket is ready or timeout runs out
        pthread_mutex_lock(&srv->lock);
        int res = poll(srv->poll_set, srv->poll_count, SERVER_POLL_TIMEOUT);
        if (res == -1 && errno != EINTR) {
            perror("poll");
            return -1;
        }
        pthread_mutex_unlock(&srv->lock);

        if (res > 0) {
            // Check all sockets in poll set
            res = check_poll(srv);
            if (res == -1) {
                fprintf(stderr, "check_poll: error\n");
                return -1;
            }
        }
    }

    printf("Server shutting down...\n");

    free_server(srv);
    return 0;
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/**
 * Defines server socket address, binds server socket to server socket address,
 * and adds server socket to poll set. Returns -1 on error.
 */
int setup_server(Server srv) {
    // Enable server socket reuse (to prevent already in use error)
    int reuse_addr = 1;
    int res = setsockopt(srv->sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
    if (res == -1) {
        perror("setsockopt");
        return -1;
    }

    // Define server socket address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;     

    // Bind server socket to server socket address
    res = bind(srv->sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (res == -1) {
        perror("bind");
        return -1;
    }

    // Add server socket to poll set
    add_poll(srv, srv->sockfd);

    return 0;
}

/**
 * Frees server.
 */
void free_server(Server srv) {
    pthread_mutex_lock(&srv->lock);

    for (int i = 0; i < srv->poll_count; i++) {
        close(srv->poll_set[i].fd);
    }

    free(srv->poll_set);
    free(srv->clients);
    ThreadPoolFree(srv->pool);

    pthread_mutex_unlock(&srv->lock);
    pthread_mutex_destroy(&srv->lock);

    free(srv);
}

/**
 * Accepts a new client connection on the given server socket,
 * and returns the client's socket file descriptor, or -1 on error.
 */
int get_client(Server srv) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_sockfd = accept(
        srv->sockfd, (struct sockaddr *)&client_addr, &client_len
    );
    if (client_sockfd == -1) {
        perror("accept");
        return -1;
    }

    if (SERVER_DEBUG_MODE) {
        printf("Connecting client: %d\n", client_sockfd);
    }

    return client_sockfd;
}

/**
 * Adds a client to the clients array. Returns -1 on error.
 */
int add_client(Server srv, int client_sockfd) {
    pthread_mutex_lock(&srv->lock);

    if (srv->client_count >= SERVER_MAX_CLIENT_COUNT) {
        pthread_mutex_unlock(&srv->lock);
        close(client_sockfd);
        return -1;
    }

    srv->clients[srv->client_count] = client_sockfd;
    srv->client_count++;

    pthread_mutex_unlock(&srv->lock);

    return 0;
}

/**
 * Removes a client from the clients array. Returns -1 on error.
 */
int remove_client(Server srv, int client_sockfd) {
    pthread_mutex_lock(&srv->lock);

    int idx = -1;
    for (int i = 0; i < srv->client_count; i++) {
        if (srv->clients[i] == client_sockfd) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        pthread_mutex_unlock(&srv->lock);
        return -1;
    }

    srv->clients[idx] = srv->clients[srv->client_count - 1];
    srv->client_count--;

    pthread_mutex_unlock(&srv->lock);

    return 0;
}

/**
 * Adds a client to the poll set. Returns -1 on error.
 */
int add_poll(Server srv, int client_sockfd) {
    pthread_mutex_lock(&srv->lock);

    if (srv->poll_count >= SERVER_MAX_POLL_COUNT) {
        pthread_mutex_unlock(&srv->lock);
        close(client_sockfd);
        return -1;
    }

    srv->poll_set[srv->poll_count].fd = client_sockfd;
    srv->poll_set[srv->poll_count].events = POLLIN;
    srv->poll_count++;

    pthread_mutex_unlock(&srv->lock);

    return 0;
}

/**
 * Removes a client from the poll set. Returns -1 on error.
 */
int remove_poll(Server srv, int client_sockfd) {
    pthread_mutex_lock(&srv->lock);

    int poll_idx = -1;
    for (int i = 0; i < srv->poll_count; i++) {
        if (srv->poll_set[i].fd == client_sockfd) {
            poll_idx = i;
            break;
        }
    }

    if (poll_idx == -1) {
        pthread_mutex_unlock(&srv->lock);
        return -1;
    }

    srv->poll_set[poll_idx] = srv->poll_set[srv->poll_count - 1];
    srv->poll_count--;

    pthread_mutex_unlock(&srv->lock);

    return 0;
}

/**
 * Checks the poll set for ready sockets. Returns -1 on error.
 * If the socket is a server, accepts a connection.
 * If the socket is a client, creates a task to receive message. 
 */
int check_poll(Server srv) {
    pthread_mutex_lock(&srv->lock);

    for (int i = 0; i < srv->poll_count; i++) {
        if (srv->poll_set[i].revents & POLLIN) {
            int poll_sockfd = srv->poll_set[i].fd;
            pthread_mutex_unlock(&srv->lock);

            if (poll_sockfd == srv->sockfd) {
                // Accept a connection (get a client)
                int client_sockfd = get_client(srv);
                if (client_sockfd == -1) {
                    fprintf(stderr, "get_client: error\n");
                    return -1;
                }

                // Add client into client array and poll set
                int res = add_client(srv, client_sockfd);
                if (res == -1) {
                    fprintf(stderr, "add_client: error\n");
                    return -1;
                }

                res = add_poll(srv, client_sockfd);
                if (res == -1) {
                    fprintf(stderr, "add_poll: error\n");
                    return -1;
                }
            } else {
                // Remove client from poll set
                int res = remove_poll(srv, poll_sockfd);
                if (res == -1) {
                    fprintf(stderr, "remove_client: error\n");
                    return - 1;
                }

                // Create a task to receive message
                struct receive_message_arg *arg = malloc(sizeof(struct receive_message_arg));
                arg->srv = srv;
                arg->client_sockfd = poll_sockfd;
                Task task = TaskNew(receive_message, arg);

                // Add task to task queue
                ThreadPoolAddTask(srv->pool, task);
            }

            pthread_mutex_lock(&srv->lock);
        }
    }

    pthread_mutex_unlock(&srv->lock);
    return 0;
}

/**
 * Receives a GDMP message from the client, parses it, validates it, 
 * and sends it to processing. Executed by treads in the thread pool.
 */
void *receive_message(void *arg) {
    struct receive_message_arg *msg_arg = (struct receive_message_arg *)arg;
    Server srv = msg_arg->srv;
    int client_sockfd = msg_arg->client_sockfd;

    // Receive string
    char msg_str[GDMP_MESSAGE_MAX_LEN];
    ssize_t bytes_read = recv(client_sockfd, msg_str, GDMP_MESSAGE_MAX_LEN - 1, MSG_DONTWAIT);

    if (bytes_read < 0) {
        if (!(errno == EWOULDBLOCK || errno == EAGAIN)) {
            perror("recv");
        }

        add_poll(srv, client_sockfd);
        return NULL;
    }

    if (bytes_read == 0) {
        if (SERVER_DEBUG_MODE) {
            printf("Disconnecting client: %d\n", client_sockfd);
        }

        remove_client(srv, client_sockfd);
        close(client_sockfd);
        return NULL;
    }

    msg_str[bytes_read] = '\0';

    // Parse string
    GDMPMessage msg = GDMPParse(msg_str);

    // Validate message
    if (!GDMPValidate(msg, GDMPGetType(msg))) return NULL;;

    // Process message
    process_message(srv, msg, client_sockfd);

    // Add client back into poll set
    add_poll(srv, client_sockfd);

    GDMPFree(msg);
    free(msg_arg);
    return NULL;
}
