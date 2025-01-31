#ifndef SERVER_PROCESS_H
#define SERVER_PROCESS_H

#include "server.h"
#include "gdmp.h"

void process_message(Server srv, GDMPMessage msg);
void process_text_message(Server srv, GDMPMessage msg);
void process_join_message(Server srv, GDMPMessage msg);

#endif
