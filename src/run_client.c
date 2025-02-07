#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "client.h"

Client cli;

void handle_sigint(int signal);

int main(void) {
    cli = ClientNew();
    if (cli == NULL) {
        fprintf(stderr, "ClientNew: error\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);

    int res = ClientStart(cli);
    if (res == -1) {
        fprintf(stderr, "ClientStart: error\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void handle_sigint(int signal) {
    ClientFree(cli);
}
