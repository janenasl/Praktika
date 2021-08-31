#ifndef MAIN_H
#define MAIN_H

#define CHUNK_SIZE 4000
#define PORT 7505
#define IP "127.0.0.1"
#define ENABLE_PKCS 0
#define WORKING 0

char *recv_all();
int is_socket_alive();
int send_all(char *buf, int *len);

#endif