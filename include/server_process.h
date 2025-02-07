#ifndef SERVER_PROCESS_H
#define SERVER_PROCESS_H

#include "server.h"
#include "gdmp.h"

/**
 * Gets the type of the GDMP message, 
 * and sends it to the appropriate function for processing.
 */
void process_message(Server srv, GDMPMessage msg, int client_sockfd);

/**
 * Processes a GDMP text message.
 */
void process_text_message(Server srv, GDMPMessage msg, int client_sockfd);

/**
 * Processes a GDMP join message.
 */
void process_join_message(Server srv, GDMPMessage msg, int client_sockfd);

#endif
