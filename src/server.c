#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <errno.h>

#include "server.h"
#include "server_process.h"
#include "gdmp.h"

struct server {
    int sockfd;
    struct pollfd *poll_set;
    int poll_count;
};

int setup_server(Server srv);
int start_server(Server srv);
int check_poll_set(Server srv);
int get_client(Server srv);
int add_client(Server srv, int client_sockfd);
int remove_client(Server srv, int client_sockfd);
void receive_message(Server srv, int client_sockfd);

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

    srv->poll_set = malloc(sizeof(struct pollfd) * SERVER_MAX_POLL_COUNT);
    if (srv->poll_set == NULL) {
        perror("malloc");
        close(srv->sockfd);
        free(srv);
        return NULL;
    }

    srv->poll_count = 0;

    return srv;
}

void ServerFree(Server srv) {
    printf("Server shutting down...\n");

    for (int i = 1; i < srv->poll_count; i++) {
        close(srv->poll_set[i].fd);
    }

    free(srv->poll_set);
    close(srv->sockfd);
    free(srv);
}

int ServerStart(Server srv) {
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
    srv->poll_set[0].fd = srv->sockfd;
    srv->poll_set[0].events = POLLIN;
    srv->poll_count++;

    return 0;
}

/**
 * Starts server loop. Returns -1 on error.
 */
int start_server(Server srv) {
    printf("Server listening on port %d...\n", SERVER_PORT);

    // Start listening for incoming connections
    int res = listen(srv->sockfd, SERVER_MAX_BACKLOG);
    if (res == -1) {
        perror("listen");
        return -1;
    }

    while (1) {
        // Wait until a socket is ready
        int res = poll(srv->poll_set, srv->poll_count, SERVER_POLL_TIMEOUT);
        if (res == -1) {
            perror("poll");
            return -1;
        }

        // Check all sockets in poll set
        res = check_poll_set(srv);
        if (res == -1) {
            fprintf(stderr, "check_poll_set: error\n");
            return -1;
        }
    }
}

/**
 * Checks the poll set for ready sockets.
 * If the socket is a server, accepts a connection.
 * If the socket is a client, creates a task to receive message. 
 * Returns -1 on error.
 */
int check_poll_set(Server srv) {
    for (int i = 0; i < srv->poll_count; i++) {
        if (srv->poll_set[i].revents & POLLIN) {
            // If a client, recieve the message
            if (srv->poll_set[i].fd != srv->sockfd) {
                receive_message(srv, srv->poll_set[i].fd);
                continue;
            }

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
        }
    }

    return 0;
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
 * Adds a client to the poll set. Returns -1 on error.
 */
int add_client(Server srv, int client_sockfd) {
    if (srv->poll_count >= SERVER_MAX_POLL_COUNT) {
        close(client_sockfd);
        return -1;
    }

    srv->poll_set[srv->poll_count].fd = client_sockfd;
    srv->poll_set[srv->poll_count].events = POLLIN;
    srv->poll_count++;

    return 0;
}

/**
 * Removes a client from the poll set. Returns -1 on error.
 */
int remove_client(Server srv, int client_sockfd) {
    int poll_idx = -1;
    for (int i = 0; i < srv->poll_count; i++) {
        if (srv->poll_set[i].fd == client_sockfd) {
            poll_idx = i;
            break;
        }
    }

    if (poll_idx == -1) {
        return -1;
    }

    srv->poll_set[poll_idx] = srv->poll_set[srv->poll_count - 1];
    srv->poll_count--;

    return 0;
}

/**
 * Receives a GDMP message from the client, and sends it to processing.
 */
void receive_message(Server srv, int client_sockfd) {
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

        remove_client(srv, client_sockfd);
        close(client_sockfd);
        return;
    }

    msg_str[bytes_read] = '\0';

    // Parse string (get message)
    GDMPMessage msg = GDMPParse(msg_str);

    // TODO: Validate message (GDMPValidate)

    // Process message
    process_message(srv, msg);

    // Free message
    GDMPFree(msg);
}
