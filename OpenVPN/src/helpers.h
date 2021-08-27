#ifndef HELPERS_H
#define HELPERS_H

#include "ubus.h"

int parse_status(char *message, int *clients_count, struct Clients **clients);
int parse_logs(char *string, int *messages_count, struct Log_messages **logs);
char *malloc_message(char *text, int *len);
char *parse_pid(char *message);
char *parse_certs(char *message);
void remove_char(char *s);

#endif