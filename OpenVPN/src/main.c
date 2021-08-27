#include <stdlib.h>
#include <stdio.h>
#include <string.h>
	
#include <sys/socket.h>	
#include <arpa/inet.h>	
#include <unistd.h>
#include <fcntl.h>

#include "ubus.h"
#include "main.h"

int network_socket;

int main(int argc , char *argv[])
{
    struct sockaddr_in server;
	
	network_socket = socket(AF_INET , SOCK_STREAM , 0);
	if (network_socket == -1) {
            fprintf(stderr, "Could not create socket\n");
            return 1;
    }
	
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;

	if (connect(network_socket , (struct sockaddr *)&server , sizeof(server)) < 0) {
            fprintf(stderr, "connection to network failed\n");
            goto cleanup;
	}

    if (process_ubus() != 0)
            goto cleanup;
	
    cleanup:
            close(network_socket);
	return 0;
}

/**
 * Send data to server, handle partial send
 * @return 0 - success; -1 - failure
 */
int send_all(char *buf, int *len)
{
    int total = 0;        //!< how many bytes we've sent
    int bytes_left = *len; //!< how many we have left to send
    int n;

    while(total < *len) {
        n = send(network_socket, buf+total, bytes_left, 0);
        if (n == -1) { break; }
        total += n;
        bytes_left -= n;
    }

    *len = total; //!< return number actually sent here

    return n == -1 ? -1 : 0;
} 

/**
 * Receive data in multiple chunks by checking a non-blocking socket
 * @return received data on success, NULL incase of failure
 */
char *recv_all()
{
	int size_recv = 0, total_size= 0, count = 0;
	char *chunk;
    chunk = (char *) malloc(sizeof(char) * CHUNK_SIZE);
    if (chunk == NULL) return NULL;
	
	fcntl(network_socket, F_SETFL, O_NONBLOCK); 	//!< make socket non blocking
	
	while(1) {
            memset(chunk, 0, CHUNK_SIZE);
            if((size_recv = recv(network_socket, chunk, CHUNK_SIZE, 0)) < 0) {
                    usleep(100000); //!< wait some time, data might not be received.
                    count++;
            } else {
                    total_size += size_recv;
                    break;
            }

            if(count > 2) {
                    break;
            }
	}
	
	return chunk;
}