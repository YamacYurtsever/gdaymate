#ifndef MULTI_SERVER_H
#define MULTI_SERVER_H

#define MULTI_SERVER_PORT 8080
#define MULTI_SERVER_THREAD_COUNT 5
#define MULTI_SERVER_MAX_BACKLOG 5
#define MULTI_SERVER_MAX_POLL_COUNT 1024
#define MULTI_SERVER_POLL_TIMEOUT 0
#define MULTI_SERVER_DEBUG_MODE 1

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
