#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int create_client(void);
void send_server(int client_sockfd, char *message);

int main(void) {
    // Create a TCP client
    int client_sockfd = create_client();

    // Send a message to the server
    char *message = "G'day mate!";
    send_server(client_sockfd, message);

    close(client_sockfd);
    return 0;
}

/**
 * Creates a new TCP client socket, and returns the file descriptor.
 */

int create_client(void) {
    // Create socket
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Define server socket address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Connect client to server
    int res = connect(
        client_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)
    );
    if (res == -1) {
        perror("connect");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }

    return client_sockfd;
}

/**
 * Sends a message to the server a client is connected to.
 */

void send_server(int client_sockfd, char *message) {
    ssize_t bytes_sent = send(client_sockfd, message, strlen(message), 0);
    if (bytes_sent == -1) {
        perror("send");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Message sent to server: %s\n", message);
}
