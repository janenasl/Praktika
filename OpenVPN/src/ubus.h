#ifndef UBUS_H
#define UBUS_H

struct Clients {
        char name[40];
        char address[40];
        char bytes_received[25];
        char bytes_sent[25];
        char connected[40];
};

int process_ubus();
char *parse_pid(char *message);

#endif