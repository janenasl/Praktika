#ifndef UBUS_H
#define UBUS_H

struct Clients {
        char name[40];
        char address[40];
        char bytes_received[25];
        char bytes_sent[25];
        char connected[40];
};

struct Log_messages {
        char message[150]; //!< one line of log information
};

int process_ubus();

#endif