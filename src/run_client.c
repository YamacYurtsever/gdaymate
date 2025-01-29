#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "client.h"

Client cli;

void handle_sigint(int signal);

int main(void) {
    cli = ClientNew();
    signal(SIGINT, handle_sigint);
    ClientStart(cli);

    return 0;
}

void handle_sigint(int signal) {
    ClientFree(cli);
    exit(EXIT_SUCCESS);
}
