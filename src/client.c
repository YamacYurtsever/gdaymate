#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#include <gdmp.h>
#include <ui.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int create_client(void);
void connect_server(int client_sockfd);
void send_server(int client_sockfd, char *username, char *content, UI ui);

int main(int argc, char *argv[]) {
    UI ui = UINew();

    // Get username
    char username[GDMP_USERNAME_MAX_LEN];
    UIDisplayInputBox(ui, "Username: ", username, GDMP_USERNAME_MAX_LEN);

    // Create a TCP client
    int client_sockfd = create_client();

    // Connect to server
    connect_server(client_sockfd);

    // Send a message to the server
    char content[GDMP_CONTENT_MAX_LEN];
    while (1) {
        UIDisplayInputBox(ui, "Content: ", content, GDMP_CONTENT_MAX_LEN);
        if (strlen(content) > 0) {
            send_server(client_sockfd, username, content, ui);
            memset(content, 0, GDMP_CONTENT_MAX_LEN);
        }
    }

    close(client_sockfd);
    UIFree(ui);
    return 0;
}

/**
 * Creates a new TCP client socket, and returns the socket file descriptor.
 */
int create_client(void) {
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return client_sockfd;
}

/**
 * Defines server socket address, connects the client to the server
 */
void connect_server(int client_sockfd) {
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
}

/**
 * Sends a message to the server a client is connected to.
 */
void send_server(int client_sockfd, char *username, char *content, UI ui) {
    // Create message
    GDMPMessage msg = GDMPNew(GDMP_MESSAGE);

    // Add headers to message
    GDMPAddHeader(msg, "Username", username);
    GDMPAddHeader(msg, "Content", content);

    // Serialize GDMP message
    char *msg_str = GDMPStringify(msg);

    // Send string
    ssize_t bytes_sent = send(client_sockfd, msg_str, strlen(msg_str), 0);
    if (bytes_sent == -1) {
        perror("send");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }

    // Log message
    UIDisplayMessage(ui, username, content);
}
