#ifndef UBUS_H
#define UBUS_H

struct Clients {
        char name[40];
        char address[40];
        int bytes_received;
        int bytes_sent;
        char connected[40];
};

int process_ubus();
char *parse_pid(char *message);

#endif