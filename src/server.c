#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "gdmp.h"
#include "thread_pool.h"

#define PORT 8080
#define THREAD_COUNT 5
#define BACKLOG 5

int create_server(void);
void start_server(int server_sockfd);
int get_client(int server_sockfd);
void handle_client(int client_sockfd);

void process_text_message(GDMPMessage msg);
void process_join_message(GDMPMessage msg);

int main(void) {
    // Create a TCP server
    int server_sockfd = create_server();

    // Create a thread pool
    ThreadPool pool = ThreadPoolNew(THREAD_COUNT);

    // Start the server (listen for connections)
    start_server(server_sockfd);

    // Server loop
    while (1) {
        // Accept a connection (get a client)
        int client_sockfd = get_client(server_sockfd);

        // Create a task to handle the client
        Task task = TaskNew(handle_client, client_sockfd);

        // Add the task to the thread pool's task queue
        ThreadPoolAddTask(pool, task);
    }

    ThreadPoolFree(pool);
    close(server_sockfd);
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

    return server_sockfd;
}

/**
 * Starts a server, making it listen for incoming connections.
 */
void start_server(int server_sockfd) {
    int res = listen(server_sockfd, BACKLOG);
    if (res == -1) {
        perror("listen");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);
}

/**
 * Accepts a new client connection on the given server socket,
 * and returns the client's socket file descriptor.
 */
int get_client(int server_sockfd) {
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
 * Handles communication with the client.
 * Receives GDMP messages, and sends them to processing.
 */
void handle_client(int client_sockfd) {
    char msg_str[GDMP_MESSAGE_MAX_LEN];
    ssize_t res;

    // Receive string
    while ((res = recv(client_sockfd, msg_str, sizeof(msg_str) - 1, 0)) > 0) {
        msg_str[res] = '\0';

        // Parse string
        GDMPMessage msg = GDMPParse(msg_str);

        // Get message type
        MessageType type = GDMPGetType(msg);

        // Send message to processing (according to type)
        switch (type) {
            case GDMP_TEXT_MESSAGE:
                // validate_text_message(msg)
                process_text_message(msg);
                break;
            case GDMP_JOIN_MESSAGE:
                // validate_join_message(msg)
                process_join_message(msg);
                break; 
            case GDMP_ERROR_MESSAGE:
                printf("Error: Can't parse message\n");
                break;
        }
    }

    close(client_sockfd);
}

/**
 * Processes a GDMP text message.
 */
void process_text_message(GDMPMessage msg) {
    // Access headers
    char *username = GDMPGetValue(msg, "Username");
    char *content = GDMPGetValue(msg, "Content");

    // Log content
    printf("%s: %s\n", username, content);

    // TODO: Broadcast to other clients
}

/**
 * Processes a GDMP join message.
 */
void process_join_message(GDMPMessage msg) {
    
}
