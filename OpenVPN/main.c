#include <stdlib.h>
#include <stdio.h>
#include <string.h>
	
#include <sys/socket.h>	
#include <arpa/inet.h>	
#include <unistd.h>
#include <fcntl.h>

#define CHUNK_SIZE 4000
#define PORT 7505
#define IP "127.0.0.1"

int recv_timeout(int s, int timeout);
int sendall(int s, char *buf, int *len);

int main(int argc , char *argv[])
{
    struct sockaddr_in server;
	int network_socket, len = 0;
	char *message = NULL;
	
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
	
	message = (char *) malloc(sizeof(char) * 8);
    if (message == NULL) return 1;
	strcpy(message, "status\n");
    len = strlen(message);
 
	if(sendall(network_socket , message , &len) < 0) //!< send some data, not critical
            fprintf(stderr, "sending failed\n");

	if (recv_timeout(network_socket, 4) < 0)
            fprintf(stderr, "receiving data failed\n"); //!< print received data, not critical

    free(message);
    cleanup:
            close(network_socket);
	return 0;
}

/**
 * Receive data in multiple chunks by checking a non-blocking socket
 * 
 */
int recv_timeout(int s, int timeout)
{
	int size_recv = 0, total_size= 0;
	char chunk[CHUNK_SIZE];
	
	fcntl(s, F_SETFL, O_NONBLOCK); 	//!< make socket non blocking
	
	while(1) {
            memset(chunk, 0, CHUNK_SIZE);
            if((size_recv = recv(s, chunk, CHUNK_SIZE, 0)) < 0) {
                    usleep(100000); //!< wait some time, data might not be received.
            } else {
                    total_size += size_recv;
                    printf("%s" , chunk);
                    break;
            }
	}
	
	return total_size;
}
/**
 * Sending data to server, handling partial send
 */
int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
} 