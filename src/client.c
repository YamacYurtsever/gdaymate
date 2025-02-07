#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdatomic.h>
#include <pthread.h>

#include "client.h"
#include "gdmp.h"
#include "ui.h"

struct client {
    int sockfd;
    UI ui;
    pthread_t receive_message_thread;
    atomic_bool shutdown;
};

int setup_client(Client cli);
void free_client(Client cli);
void *receive_message(void *arg);
void handle_command(Client cli, char *command);
char *get_timestamp(void);

int send_text_message(Client cli, char *username, char *content, char *timestamp);
int send_join_message(Client cli);

////////////////////////////////// FUNCTIONS ///////////////////////////////////

Client ClientNew(void) {
    Client cli = malloc(sizeof(struct client));
    if (cli == NULL) {
        perror("malloc");
        return NULL;
    }

    cli->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cli->sockfd == -1) {
        perror("socket");
        free(cli);
        return NULL;
    }

    atomic_store(&cli->shutdown, false);

    // Setup the client
    int res = setup_client(cli);
    if (res == -1) {
        fprintf(stderr, "setup_client: error\n");
        close(cli->sockfd);
        free(cli);
        return NULL;
    }

    cli->ui = UINew();

    return cli;
}

int ClientStart(Client cli) {
    // Start receive message thread;
    pthread_create(&cli->receive_message_thread, NULL, receive_message, cli);

    // Get username
    char username[GDMP_USERNAME_MAX_LEN];
    UIDisplayInput(cli->ui, "Username: ", username, GDMP_USERNAME_MAX_LEN);

    while (!atomic_load(&cli->shutdown)) {
        // Get content
        char content[GDMP_CONTENT_MAX_LEN];
        UIDisplayInput(cli->ui, "Content: ", content, GDMP_CONTENT_MAX_LEN);

        // Handle if command
        if (content[0] == CLIENT_COMMAND_CHAR) {
            handle_command(cli, content);
            continue;
        }

        // Get timestamp
        char *timestamp = get_timestamp();

        // Send text message
        if (strlen(content) > 0) {
            int res = send_text_message(cli, username, content, timestamp);
            if (res == -1) {
                fprintf(stderr, "send_text_message: error\n");
                return -1;
            }
        }
    }

    free_client(cli);
    return 0;
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

/**
 * Defines server socket address, connects client to server.
 * Returns -1 on error.
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
 * Frees client.
 */
void free_client(Client cli) {
    close(cli->sockfd);
    pthread_join(cli->receive_message_thread, NULL);
    UIFree(cli->ui);
    free(cli);
}

/**
 * Receives GDMP messages from the server, parses them, validates them, 
 * and displays them. Executed by a seperate thread.
 */
 void *receive_message(void *arg) {
    Client cli = (Client)arg;

    while (!atomic_load(&cli->shutdown)) {
        // Receive string
        char msg_str[GDMP_MESSAGE_MAX_LEN];
        ssize_t bytes_read = recv(cli->sockfd, msg_str, GDMP_MESSAGE_MAX_LEN - 1, 0);

        if (bytes_read < 0) {
            perror("recv");
            return NULL;
        }

        if (bytes_read == 0) {
            UIDisplayMessage(cli->ui, "Server disconnected");
            atomic_store(&cli->shutdown, true);
            return NULL;
        }

        msg_str[bytes_read] = '\0';

        // Parse string
        GDMPMessage msg = GDMPParse(msg_str);

        // Validate message
        if (!GDMPValidate(msg)) return NULL;

        // Access headers
        char *username = GDMPGetValue(msg, "Username");
        char *content = GDMPGetValue(msg, "Content");
        char *timestamp = GDMPGetValue(msg, "Timestamp");

        // Display message
        char message[GDMP_MESSAGE_MAX_LEN];
        snprintf(
            message, GDMP_MESSAGE_MAX_LEN, 
            "[%s] %s: %s", timestamp, username, content
        );
        UIDisplayMessage(cli->ui, message);

        GDMPFree(msg);
    }

    return NULL;
 }

/**
 * Handles the given command string.
 */
void handle_command(Client cli, char *command) {
    if (strcmp(command, "/exit") == 0) {
        atomic_store(&cli->shutdown, true);
    } else if (strcmp(command, "/clear") == 0) {
        UIClearMessages(cli->ui);
    } else {
        UIDisplayMessage(cli->ui, "Invalid command");
    }
}

/**
 * Returns current timestamp.
 */
char *get_timestamp(void) {
    char *timestamp = malloc(GDMP_TIMESTAMP_MAX_LEN);
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, GDMP_TIMESTAMP_MAX_LEN, CLIENT_TIMESTAMP_FORMAT, tm_info);
    return timestamp;
}

/////////////////////////////////// SENDING ////////////////////////////////////

/**
 * Sends a GDMP text message to the server. Returns -1 on error.
 */
int send_text_message(Client cli, char *username, char *content, char *timestamp) {
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
        free(timestamp);
        free(msg_str);
        return -1;
    }

    // Display message
    char message[GDMP_MESSAGE_MAX_LEN];
    snprintf(
        message, GDMP_MESSAGE_MAX_LEN, 
        "[%s] %s: %s", timestamp, username, content
    );
    UIDisplayMessage(cli->ui, message);

    free(timestamp);
    free(msg_str);
    GDMPFree(msg);
    return 0;
}

/**
 * Sends a GDMP join message to the server. Returns -1 on error.
 */
int send_join_message(Client cli) {
    // TODO
    return 0;
}
