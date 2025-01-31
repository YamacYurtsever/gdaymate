#ifndef SERVER_H
#define SERVER_H

#define SERVER_PORT 8080
#define SERVER_THREAD_COUNT 5
#define SERVER_MAX_BACKLOG 5
#define SERVER_MAX_POLL_COUNT 1024
#define SERVER_POLL_TIMEOUT 0
#define SERVER_DEBUG_MODE 1

typedef struct server *Server;

/**
 * Creates a server. Returns NULL on error.
 */
Server ServerNew(void);

/**
 * Starts a server. Returns -1 on error.
 */
int ServerStart(Server srv);

/**
 * Stops and frees a server. Only use if the server has been started.
 */
void ServerStop(Server srv);

#endif
