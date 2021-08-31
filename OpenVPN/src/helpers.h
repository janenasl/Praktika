#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include "ubus.h"

int parse_logs(char *string, int *messages_count, struct Log_messages **logs);
char *parse_message(char *message, char symbol);
char *malloc_message(char *text, int *len);
int parse_status(char *message);
void remove_char(char *s);
int gather_status();
int set_pkcs();

#endif