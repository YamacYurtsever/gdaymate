#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "client.h"

int main(void) {
    Client cli = ClientNew();
    if (cli == NULL) {
        fprintf(stderr, "ClientNew: error\n");
        exit(EXIT_FAILURE);
    }

    int res = ClientStart(cli);
    if (res == -1) {
        fprintf(stderr, "ClientStart: error\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
