#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "ThreadPool.h"
#include "TaskQueue.h"
#include "Task.h"

#define PORT 8080
#define BACKLOG 5
#define BUFFER_SIZE 1024

int create_server(void);
int get_client(int server_sockfd);
void handle_client(int client_sockfd);

int main(void) {
    // Create a TCP server
    int server_sockfd = create_server();

    // Create a thread pool
    ThreadPool pool = ThreadPoolNew();

    // Listen for incoming connections
    int res = listen(server_sockfd, BACKLOG);
    if (res == -1) {
        perror("listen");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Server loop
    while (1) {
        // Accept a connection (get a client)
        int client_sockfd = get_client(server_sockfd);

        printf("Accepted connection from client %d\n", client_sockfd);

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
 * Creates a new TCP server socket, binds it to the specified address, 
 * and returns the socket file descriptor.
 */

int create_server(void) {
    // Create socket
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Define socket address
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
 * Reads data from the client's socket, logs the message, 
 * and closes the socket when the client disconnects.
 */

void handle_client(int client_sockfd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        // Log the message
        buffer[bytes_received] = '\0';
        printf("Client %d: %s\n", client_sockfd, buffer);
    }

    printf("Client %d disconnected\n", client_sockfd);
    close(client_sockfd);
}
