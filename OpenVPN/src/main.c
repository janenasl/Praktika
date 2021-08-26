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
	char *message = NULL;
    int len = 0;
	
	network_socket = socket(AF_INET , SOCK_STREAM , 0);
	if (network_socket == -1) {
            printf("Could not create socket");
            return 1;
    }
	
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;

	if (connect(network_socket , (struct sockaddr *)&server , sizeof(server)) < 0) {
            fprintf(stderr, "connection to network failed\n");
            goto cleanup;
	}
    
    recv_all(); //!< receive first message that you get always when you connect
    process_ubus();
	
    cleanup:
            close(network_socket);
	return 0;
}

/**
 * Receive data in multiple chunks by checking a non-blocking socket
 * 
 */
char *recv_all()
{
	int size_recv = 0, total_size= 0;
	char *chunk;
    chunk = (char *) malloc(sizeof(char) * CHUNK_SIZE);
    int count = 0;
	
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
                    //fprintf(stderr, "no message is sent\n");
                    break;
            }
	}
	
	return chunk;
}
/**
 * Send data to server, handle partial send
 */
int send_all(char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(network_socket, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
} 