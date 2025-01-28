#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#include "gdmp.h"
#include "ui.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

// TODO: Client context

int create_client(void);
void connect_server(int client_sockfd);

void send_text_message(UI ui, int client_sockfd, char *username, char *content);
void send_join_message(UI ui, int client_sockfd);

char *get_timestamp(void);

//////////////////////////////// CLIENT LOGIC //////////////////////////////////

int main(int argc, char *argv[]) {
    UI ui = UINew();

    // Get username
    char username[GDMP_USERNAME_MAX_LEN];
    UIDisplayInputBox(ui, "Username: ", username, GDMP_USERNAME_MAX_LEN);

    // Create a TCP client
    int client_sockfd = create_client();

    // Connect to server
    connect_server(client_sockfd);

    // Send text messages
    char content[GDMP_CONTENT_MAX_LEN];
    while (1) {
        UIDisplayInputBox(ui, "Content: ", content, GDMP_CONTENT_MAX_LEN);
        if (strlen(content) > 0) {
            send_text_message(ui, client_sockfd, username, content);
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
 * Defines server socket address, connects the client to the server.
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

/////////////////////////////////// SENDING ////////////////////////////////////

/**
 * Sends a GDMP text message to the server.
 */
void send_text_message(
    UI ui, int client_sockfd, char *username, char *content
) {
    // Create message
    GDMPMessage msg = GDMPNew(GDMP_TEXT_MESSAGE);
    char *timestamp = get_timestamp();

    // Add headers to message
    GDMPAddHeader(msg, "Username", username);
    GDMPAddHeader(msg, "Content", content);
    GDMPAddHeader(msg, "Timestamp", timestamp);

    // Serialize message
    char *msg_str = GDMPStringify(msg);

    // Send string
    ssize_t bytes_sent = send(client_sockfd, msg_str, strlen(msg_str), 0);
    if (bytes_sent == -1) {
        perror("send");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }

    // Log text message
    char message[GDMP_MESSAGE_MAX_LEN];
    snprintf(
        message, GDMP_MESSAGE_MAX_LEN, 
        "[%s] %s: %s", timestamp, username, content
    );
    UIDisplayMessage(ui, message);

    free(msg_str);
    free(timestamp);
}

/**
 * Sends a GDMP join message to the server.
 */
void send_join_message(UI ui, int client_sockfd) {

}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/**
 * Returns current timestamp.
 */
char *get_timestamp(void) {
    char *timestamp = malloc(GDMP_TIMESTAMP_MAX_LEN);
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%H:%M", tm_info);
    return timestamp;
}
