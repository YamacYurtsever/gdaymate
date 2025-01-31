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

#include "multi_server.h"
#include "gdmp.h"
#include "thread_pool.h"

struct multi_server {
    ThreadPool pool;
    int sockfd;
    struct pollfd *poll_set;
    int poll_count;
    atomic_bool shutdown;
    pthread_mutex_t lock;
};

struct receive_message_arg {
    MultiServer srv;
    int client_sockfd;
};

void free_server(MultiServer srv);
int setup_server(MultiServer srv);
int start_server(MultiServer srv);
int check_poll_set(MultiServer srv);
int get_client(MultiServer srv);
int add_client(MultiServer srv, int client_sockfd);
int remove_client(MultiServer srv, int client_sockfd);
void receive_message(void *arg);

////////////////////////////////// FUNCTIONS ///////////////////////////////////

MultiServer MultiServerNew(void) {
    MultiServer srv = malloc(sizeof(struct multi_server));
    if (srv == NULL) {
        perror("malloc");
        return NULL;
    }

    srv->pool = ThreadPoolNew(MULTI_SERVER_THREAD_COUNT);
    if (srv->pool == NULL) {
        fprintf(stderr, "ThreadPoolNew: error\n");
        free(srv);
        return NULL;
    }

    srv->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv->sockfd == -1) {
        perror("socket");
        ThreadPoolFree(srv->pool);
        free(srv);
        return NULL;
    }

    srv->poll_set = malloc(sizeof(struct pollfd) * MULTI_SERVER_MAX_POLL_COUNT);
    if (srv->poll_set == NULL) {
        perror("malloc");
        close(srv->sockfd);
        ThreadPoolFree(srv->pool);
        free(srv);
        return NULL;
    }

    srv->poll_count = 0;

    atomic_store(&srv->shutdown, false);

    pthread_mutex_init(&srv->lock, NULL); 

    return srv;
}

void MultiServerFree(MultiServer srv) {
    atomic_store(&srv->shutdown, true);
}

int MultiServerStart(MultiServer srv) {
    int res = setup_server(srv);
    if (res == -1) {
        fprintf(stderr, "setup_server: error\n");
        return -1;
    }

    res = start_server(srv);
    if (res == -1) {
        fprintf(stderr, "start_server: error\n");
        return -1;
    }

    return 0;
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/**
 * Frees server.
 */
void free_server(MultiServer srv) {
    pthread_mutex_lock(&srv->lock);

    for (int i = 0; i < srv->poll_count; i++) {
        close(srv->poll_set[i].fd);
    }

    free(srv->poll_set);
    ThreadPoolFree(srv->pool);

    pthread_mutex_unlock(&srv->lock);
    pthread_mutex_destroy(&srv->lock);

    free(srv);
}

/**
 * Defines server socket address, binds server socket to server socket address,
 * and adds server socket to poll set. Returns -1 on error.
 */
int setup_server(MultiServer srv) {
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
    server_addr.sin_port = htons(MULTI_SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;     

    // Bind server socket to server socket address
    res = bind(srv->sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (res == -1) {
        perror("bind");
        return -1;
    }

    // Add server socket to poll set
    srv->poll_set[0].fd = srv->sockfd;
    srv->poll_set[0].events = POLLIN;
    srv->poll_count++;

    return 0;
}

/**
 * Listens for connections and starts server loop. Returns -1 on error.
 */
int start_server(MultiServer srv) {
    printf("Server listening on port %d...\n", MULTI_SERVER_PORT);

    // Start listening for incoming connections
    int res = listen(srv->sockfd, MULTI_SERVER_MAX_BACKLOG);
    if (res == -1) {
        perror("listen");
        return -1;
    }

    while (1) {
        // Check shutdown flag
        if (atomic_load(&srv->shutdown)) {
            printf("Server shutting down...\n");
            free_server(srv);
            return 0;
        }

        // Wait until a socket is ready or timeout runs out
        pthread_mutex_lock(&srv->lock);
        int res = poll(srv->poll_set, srv->poll_count, MULTI_SERVER_POLL_TIMEOUT);
        if (res == -1) {
            perror("poll");
            return -1;
        }
        pthread_mutex_unlock(&srv->lock);

        // Check all sockets in poll set
        res = check_poll_set(srv);
        if (res == -1) {
            fprintf(stderr, "check_poll_set: error\n");
            return -1;
        }
    }
}

/**
 * Checks the poll set for ready sockets. Returns -1 on error.
 * If the socket is a server, accepts a connection.
 * If the socket is a client, creates a task to receive message. 
 */
int check_poll_set(MultiServer srv) {
    pthread_mutex_lock(&srv->lock);

    for (int i = 0; i < srv->poll_count; i++) {
        if (srv->poll_set[i].revents & POLLIN) {
            pthread_mutex_unlock(&srv->lock);

            if (srv->poll_set[i].fd == srv->sockfd) {
                // Accept a connection (get a client)
                int client_sockfd = get_client(srv);
                if (client_sockfd == -1) {
                    fprintf(stderr, "get_client: error\n");
                    return -1;
                }

                // Add client into poll set
                int res = add_client(srv, client_sockfd);
                if (res == -1) {
                    fprintf(stderr, "add_client: error\n");
                    return -1;
                }
            } else {
                // Remove client from poll set
                int res = remove_client(srv, srv->poll_set[i].fd);
                if (res == -1) {
                    fprintf(stderr, "remove_client: error\n");
                    return - 1;
                }

                // Create a task to receive message
                struct receive_message_arg *arg = malloc(sizeof(struct receive_message_arg));
                arg->srv = srv;
                arg->client_sockfd = srv->poll_set[i].fd;

                Task task = TaskNew(receive_message, arg);
                if (task == NULL) {
                    fprintf(stderr, "TaskNew: error\n");
                    return -1;
                }

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
 * Accepts a new client connection on the given server socket,
 * and returns the client's socket file descriptor, or -1 on error.
 */
int get_client(MultiServer srv) {
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
 * Adds a client to the poll set. Returns -1 on error.
 */
int add_client(MultiServer srv, int client_sockfd) {
    pthread_mutex_lock(&srv->lock);

    if (srv->poll_count >= MULTI_SERVER_MAX_POLL_COUNT) {
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
int remove_client(MultiServer srv, int client_sockfd) {
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
 * Receives a GDMP message from the client, parses it, validates it, 
 * and sends it to processing. Executed by treads in the thread pool.
 */
void receive_message(void *arg) {
    struct receive_message_arg *msg_arg = (struct receive_message_arg *)arg;
    MultiServer srv = msg_arg->srv;
    int client_sockfd = msg_arg->client_sockfd;

    // Receive string
    char msg_str[GDMP_MESSAGE_MAX_LEN];
    ssize_t bytes_read = recv(client_sockfd, msg_str, GDMP_MESSAGE_MAX_LEN - 1, MSG_DONTWAIT);

    if (bytes_read < 0 && !(errno == EWOULDBLOCK || errno == EAGAIN)) {
        perror("recv");
        return;
    }

    if (bytes_read == 0) {
        if (SERVER_DEBUG_MODE) {
            printf("Disconnecting client: %d\n", client_sockfd);
        }

        close(client_sockfd);
        return;
    }

    msg_str[bytes_read] = '\0';

    // Parse string
    GDMPMessage msg = GDMPParse(msg_str);

    // TODO: Validate message

    // TODO: Process message
    if (GDMPGetType(msg) == GDMP_TEXT_MESSAGE) {
        char *username = GDMPGetValue(msg, "Username");
        char *content = GDMPGetValue(msg, "Content");
        char *timestamp = GDMPGetValue(msg, "Timestamp");
        printf("[%s] %s: %s\n", timestamp, username, content);
    }

    // Free message
    GDMPFree(msg);

    // Add client back into poll set
    int res = add_client(srv, client_sockfd);
    if (res == -1) {
        fprintf(stderr, "add_client: error\n");
        return;
    }
}
