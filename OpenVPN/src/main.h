#ifndef MAIN_H
#define MAIN_H

#define CHUNK_SIZE 4000
#define PORT 7505
#define IP "127.0.0.1"

char *recv_all();
int send_all(char *buf, int *len);

#endif