#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>

#include "gdmp.h"
#include "thread_pool.h"

#define PORT 8080
#define BACKLOG 5
#define THREAD_COUNT 5
#define CLIENTS_MAX_COUNT 1024

int server_sockfd;
ThreadPool pool;
struct pollfd poll_fds[CLIENTS_MAX_COUNT];
int client_count = 1;

int create_server(void);
void start_server(void);
void stop_server(int signal);
void loop_server(void);

int get_client(void);
void add_client(int client_sockfd);
void recv_client(int client_sockfd);

void process_message(GDMPMessage msg);
void process_text_message(GDMPMessage msg);
void process_join_message(GDMPMessage msg);

//////////////////////////////// SERVER LOGIC //////////////////////////////////

int main(void) {
    // Create a TCP server
    server_sockfd = create_server();

    // Create a thread pool
    pool = ThreadPoolNew(THREAD_COUNT);

    // Setup SIGINT handler
    signal(SIGINT, stop_server);

    // Start server (listen for connections)
    start_server();

    // Server loop
    loop_server();

    return 0;
}

/**
 * Creates a new TCP server socket, defines server socket address,
 * binds the socket to the socket address, and returns the file descriptor.
 */
int create_server(void) {
    // Create socket
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set reuse address (to prevent already in use error)
    int reuse_addr = 1;
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) == -1) {
        perror("setsockopt");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    // Define server socket address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;     

    // Bind socket to socket address
    int res = bind(
        server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)
    );
    if (res == -1) {
        perror("bind");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    // Add socket to poll set
    poll_fds[0].fd = server_sockfd;
    poll_fds[0].events = POLLIN;

    return server_sockfd;
}

/**
 * Starts a server, making it listen for incoming connections.
 */
void start_server(void) {
    int res = listen(server_sockfd, BACKLOG);
    if (res == -1) {
        perror("listen");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);
}

/**
 * Stops a server.
 */
void stop_server(int signal) {
    printf("Server shutting down...\n");

    for (int i = 0; i < client_count; i++) {
        close(poll_fds[i].fd);
    }

    ThreadPoolFree(pool);
    exit(EXIT_SUCCESS);
}

void loop_server(void) {
    while (1) {
        // Wait until at least one socket is ready
        int res = poll(poll_fds, client_count, -1);
        if (res == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < client_count; i++) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == server_sockfd) {
                    // Accept a connection (get a client)
                    int client_sockfd = get_client();
                    add_client(client_sockfd);
                } else {
                    // Create a task to handle client and add it to task queue
                    int client_sockfd = poll_fds[i].fd;
                    Task task = TaskNew(recv_client, client_sockfd);
                    ThreadPoolAddTask(pool, task);
                }
            }
        }
    }
}

/**
 * Accepts a new client connection on the given server socket,
 * and returns the client's socket file descriptor.
 */
int get_client(void) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_sockfd = accept(
        server_sockfd, (struct sockaddr *)&client_addr, &client_len
    );
    if (client_sockfd == -1) {
        perror("accept");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    return client_sockfd;
}

/**
 * Adds a client to the poll set.
 */
void add_client(int client_sockfd) {
    if (client_count >= CLIENTS_MAX_COUNT) {
        fprintf(stderr, "Error: CLIENTS_MAX_COUNT\n");
        close(client_sockfd);
        return;
    }

    poll_fds[client_count].fd = client_sockfd;
    poll_fds[client_count].events = POLLIN;
    client_count++;
}

/**
 * Receives GDMP messages from a client, and sends them to processing.
 */
void recv_client(int client_sockfd) {
    char msg_str[GDMP_MESSAGE_MAX_LEN];
    ssize_t res;

    // Receive string
    while ((res = recv(client_sockfd, msg_str, sizeof(msg_str) - 1, MSG_DONTWAIT)) > 0) {
        msg_str[res] = '\0';

        // Parse string (get message)
        GDMPMessage msg = GDMPParse(msg_str);

        // TODO: Validate message (GDMPValidate)

        // Process message
        process_message(msg);
    }

    if (res < 0 && !(errno == EWOULDBLOCK || errno == EAGAIN)) {
        perror("recv");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
}

///////////////////////////////// PROCESSING ///////////////////////////////////

/**
 * Gets the type of the GDMP message, and sends it to the appropriate processor.
 */
void process_message(GDMPMessage msg) {
    MessageType type = GDMPGetType(msg);
    switch (type) {
        case GDMP_TEXT_MESSAGE:
            process_text_message(msg);
            break;
        case GDMP_JOIN_MESSAGE:
            process_join_message(msg);
            break; 
        case GDMP_ERROR_MESSAGE:
            printf("Error: GDMP_ERROR_MESSAGE\n");
            break;
    }
}

/**
 * Processes a GDMP text message.
 */
void process_text_message(GDMPMessage msg) {
    // Access headers
    char *username = GDMPGetValue(msg, "Username");
    char *content = GDMPGetValue(msg, "Content");
    char *timestamp = GDMPGetValue(msg, "Timestamp");

    // Log content
    printf("[%s] %s: %s\n", timestamp, username, content);

    // TODO: Broadcast to other clients
}

/**
 * Processes a GDMP join message.
 */
void process_join_message(GDMPMessage msg) {
    
}
