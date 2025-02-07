#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include "server_process.h"
#include "server.h"
#include "gdmp.h"
#include "task.h"
#include "thread_pool.h"

struct send_text_message_arg {
    GDMPMessage msg;
    int client_sockfd;
};

void *send_text_message(void *arg);

////////////////////////////////// FUNCTIONS ///////////////////////////////////

void process_message(Server srv, GDMPMessage msg, int client_sockfd) {
    MessageType type = GDMPGetType(msg);
    switch (type) {
        case GDMP_TEXT_MESSAGE:
            process_text_message(srv, msg, client_sockfd);
            break;
        case GDMP_JOIN_MESSAGE:
            process_join_message(srv, msg, client_sockfd);
            break; 
        case GDMP_ERROR_MESSAGE:
            fprintf(stderr, "GDMP_ERROR_MESSAGE\n");
            break;
    }
}

void process_text_message(Server srv, GDMPMessage msg, int client_sockfd) {
    // Access headers
    char *username = GDMPGetValue(msg, "Username");
    char *content = GDMPGetValue(msg, "Content");
    char *timestamp = GDMPGetValue(msg, "Timestamp");

    // Log message
    printf("[%s] %s: %s\n", timestamp, username, content);

    // Broadcast to other clients
    pthread_mutex_lock(&srv->lock);
    for (int i = 0; i < srv->poll_count; i++) {
        if (srv->poll_set[i].fd == srv->sockfd || 
            srv->poll_set[i].fd == client_sockfd) {
            continue;
        }

        // Create a task to send text message
        struct send_text_message_arg *arg = malloc(sizeof(struct send_text_message_arg));
        arg->msg = GDMPCopy(msg);
        arg->client_sockfd = srv->poll_set[i].fd;
        Task task = TaskNew(send_text_message, arg);

        // Add task to task queue
        ThreadPoolAddTask(srv->pool, task);
    }
    pthread_mutex_unlock(&srv->lock);
}

void process_join_message(Server srv, GDMPMessage msg, int client_sockfd) {
    // TODO
}

////////////////////////////// HELPER FUNCTIONS ////////////////////////////////

void *send_text_message(void *arg) {
    struct send_text_message_arg *msg_arg = (struct send_text_message_arg *)arg;
    GDMPMessage msg = msg_arg->msg;
    int client_sockfd = msg_arg->client_sockfd;

    // Serialize message
    char *msg_str = GDMPStringify(msg);

    // Send string
    ssize_t bytes_sent = send(client_sockfd, msg_str, strlen(msg_str), MSG_DONTWAIT);
    if (bytes_sent == -1 && !(errno == EWOULDBLOCK || errno == EAGAIN)) {
        perror("send");
    }

    GDMPFree(msg);
    free(msg_str);
    free(msg_arg);
    return NULL;
}
