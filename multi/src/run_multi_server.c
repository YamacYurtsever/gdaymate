#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "multi_server.h"

MultiServer srv;

void handle_sigint(int signal);

int main(void) {
    srv = MultiServerNew();
    if (srv == NULL) {
        fprintf(stderr, "MultiServerNew: error\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);

    int res = MultiServerStart(srv);
    if (res == -1) {
        fprintf(stderr, "MultiServerStart: error\n");
        MultiServerFree(srv);
        exit(EXIT_FAILURE);
    }

    return 0;
}

void handle_sigint(int signal) {
    MultiServerFree(srv);
}
