#ifndef HELPERS_H
#define HELPERS_H

#include "ubus.h"

void remove_char(char *s);
int count_lines(char *string);
char *parse_pid(char *message);
int parse_status(char *message, int *clients_count, struct Clients **clients);
static int split_into_parts(char *string, int client_number, struct Clients **clients);
static int string_parse(char *string, struct Clients **clients);
char *malloc_message(char *text, int size, int *len);

#endif