#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "thread_pool.h"
#include "task_queue.h"
#include "task.h"
#include "gdmp.h"

#define PORT 8080
#define THREAD_COUNT 5
#define BACKLOG 5
#define BUFFER_SIZE 1024

int create_server(void);
void start_server(int server_sockfd);
int get_client(int server_sockfd);
void handle_client(int client_sockfd);

void process_message(GDMPMessage msg);
void process_ack(GDMPMessage msg);
void process_auth(GDMPMessage msg)

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
 * binds the socket to the socket address, and returns the socket file descriptor.
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
 * Reads data from the client's socket, and logs the message.
 */
void handle_client(int client_sockfd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        // Receive and parse string
        buffer[bytes_received] = '\0';
        GDMPMessage msg = GDMPParse(buffer);

        // Get message type and send to processing
        MessageType type = GDMPGetType(msg);
        switch (type) {
            case GDMP_MESSAGE:
                process_message(msg);
                continue;
            case GDMP_ACK:
                process_ack(msg);
                continue; 
            case GDMP_ACK:
                process_auth(msg);
                continue; 
            default:
                continue;
        }
    }

    close(client_sockfd);
}

void process_message(GDMPMessage msg) {
    // Access headers
    char *username = GDMPGetValue(msg, "Username");
    char *content = GDMPGetValue(msg, "Content");

    // Log content
    printf("%s: %s\n", username, content);

    // TODO: Broadcast to other clients
}

void process_ack(GDMPMessage msg) {
    
}

void process_auth(GDMPMessage msg) {
    
}
