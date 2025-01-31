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
 * Frees a server.
 */
void ServerFree(Server srv);

/**
 * Starts a server. Returns -1 on error.
 */
int ServerStart(Server srv);

#endif
