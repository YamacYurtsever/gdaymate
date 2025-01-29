#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#include "client.h"
#include "gdmp.h"
#include "ui.h"

struct client {
    UI ui;
    int sockfd;
};

int setup_client(Client cli);
char *get_timestamp(void);

void send_text_message(Client cli, char *username, char *content, char *timestamp);
void send_join_message(Client cli);


////////////////////////////////// FUNCTIONS ///////////////////////////////////

Client ClientNew(void) {
    Client cli = malloc(sizeof(struct client));
    if (cli == NULL) {
        perror("malloc");
        free(cli);
        return NULL;
    }

    cli->ui = UINew();

    cli->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cli->sockfd == -1) {
        perror("socket");
        UIFree(cli->ui);
        free(cli);
        exit(EXIT_FAILURE);
    }

    int res = setup_client(cli);
    if (res == -1) {
        perror("setup_client");
        close(cli->sockfd);
        UIFree(cli->ui);
        free(cli);
        return NULL;
    }

    return cli;
}

void ClientFree(Client cli) {
    close(cli->sockfd);
    UIFree(cli->ui);
    free(cli);
}

void ClientStart(Client cli) {
    // Get username
    char username[GDMP_USERNAME_MAX_LEN];
    UIDisplayInputBox(cli->ui, "Username: ", username, GDMP_USERNAME_MAX_LEN);

    while (1) {
        // Get content
        char content[GDMP_CONTENT_MAX_LEN];
        UIDisplayInputBox(cli->ui, "Content: ", content, GDMP_CONTENT_MAX_LEN);

        // Get timestamp
        char *timestamp = get_timestamp();

        // Send text message
        if (strlen(content) > 0) {
            send_text_message(cli, username, content, timestamp);
        }
    }
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/**
 * Defines server socket address, 
 * connects client socket to server socket address.
 * Returns 0 on success, and -1 on error.
 */
int setup_client(Client cli) {
    // Define server socket address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(CLIENT_SERVER_PORT);
    inet_pton(AF_INET, CLIENT_SERVER_IP, &server_addr.sin_addr);

    // Connect client socket to server socket address
    int res = connect(
        cli->sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)
    );
    if (res == -1) {
        perror("connect");
        return -1;
    }

    return 0;
}

/**
 * Gets current timestamp.
 */
char *get_timestamp(void) {
    char *timestamp = malloc(GDMP_TIMESTAMP_MAX_LEN);
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), CLIENT_TIMESTAMP_FORMAT, tm_info);
    return timestamp;
}

/////////////////////////////////// SENDING ////////////////////////////////////

/**
 * Sends a GDMP text message to the server.
 */
void send_text_message(Client cli, char *username, char *content, char *timestamp) {
    // Create message
    GDMPMessage msg = GDMPNew(GDMP_TEXT_MESSAGE);

    // Add headers to message
    GDMPAddHeader(msg, "Username", username);
    GDMPAddHeader(msg, "Content", content);
    GDMPAddHeader(msg, "Timestamp", timestamp);

    // Serialize message
    char *msg_str = GDMPStringify(msg);

    // Send string
    ssize_t bytes_sent = send(cli->sockfd, msg_str, strlen(msg_str), 0);
    if (bytes_sent == -1) {
        perror("send");
        return;
    }

    // Display message
    char message[GDMP_MESSAGE_MAX_LEN];
    snprintf(
        message, GDMP_MESSAGE_MAX_LEN, 
        "[%s] %s: %s", timestamp, username, content
    );
    UIDisplayMessage(cli->ui, message);

    free(msg_str);
    free(timestamp);
}

/**
 * Sends a GDMP join message to the server.
 */
void send_join_message(Client cli) {

}
