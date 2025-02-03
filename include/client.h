#ifndef CLIENT_H
#define CLIENT_H

#define CLIENT_SERVER_IP "127.0.0.1"
#define CLIENT_SERVER_PORT 8080
#define CLIENT_TIMESTAMP_FORMAT "%H:%M"
#define CLIENT_COMMAND_CHAR '/'

typedef struct client *Client;

/**
 * Creates a client. Returns NULL on error.
 */
Client ClientNew(void);

/**
 * Frees a client.
 */
void ClientFree(Client cli);

/**
 * Starts a client. Returns -1 on error.
 */
int ClientStart(Client cli);

#endif
