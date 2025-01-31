#include <stdio.h>

#include "server_process.h"
#include "server.h"
#include "gdmp.h"

/**
 * Gets the type of the GDMP message, 
 * and sends it to the appropriate function for processing.
 */
void process_message(Server srv, GDMPMessage msg) {
    MessageType type = GDMPGetType(msg);
    switch (type) {
        case GDMP_TEXT_MESSAGE:
            process_text_message(srv, msg);
            break;
        case GDMP_JOIN_MESSAGE:
            process_join_message(srv, msg);
            break; 
        case GDMP_ERROR_MESSAGE:
            fprintf(stderr, "GDMP_ERROR_MESSAGE\n");
            break;
    }
}

/**
 * Processes a GDMP text message.
 */
void process_text_message(Server srv, GDMPMessage msg) {
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
void process_join_message(Server srv, GDMPMessage msg) {
    
}
