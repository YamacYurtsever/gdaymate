#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "server.h"

Server srv;

void handle_sigint(int signal);

int main(void) {
    srv = ServerNew();
    if (srv == NULL) {
        fprintf(stderr, "ServerNew: error\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);

    int res = ServerStart(srv);
    if (res == -1) {
        fprintf(stderr, "ServerStart: error\n");
        ServerFree(srv);
        exit(EXIT_FAILURE);
    }

    return 0;
}

void handle_sigint(int signal) {
    ServerStop(srv);
    exit(EXIT_SUCCESS);
}
