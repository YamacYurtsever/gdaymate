#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "server.h"

Server srv;

void handle_sigint(int signal);

int main(void) {
    srv = ServerNew();
    if (srv == NULL) {
        perror("ServerNew");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);
    ServerStart(srv);

    return 0;
}

void handle_sigint(int signal) {
    ServerFree(srv);
    exit(EXIT_SUCCESS);
}
