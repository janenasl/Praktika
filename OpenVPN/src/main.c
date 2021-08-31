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
 * check if server is still up
 * @return 0 - success, 1 - failed (server is down)
 */
int is_socket_alive(void)
{
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt (network_socket, SOL_SOCKET, SO_ERROR, &error, &len);

    if (retval != 0) {
            fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
            return 1;
    }

    if (error != 0) {
            fprintf(stderr, "socket error: %s\n", strerror(error));
            return 1;
    }

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
	int count = 0;
	char *chunk;

    if (fcntl(network_socket, F_SETFL, O_NONBLOCK) == -1) return NULL;

    chunk = (char *) malloc(sizeof(char) * CHUNK_SIZE);
    if (chunk == NULL) return NULL;
	
	while(1) {
            memset(chunk, 0, CHUNK_SIZE);
            if(recv(network_socket, chunk, CHUNK_SIZE, 0) < 0) {
                    usleep(100000); //!< wait some time, data might not be received.
                    count++;
            } else {
                    break;
            }

            if(count > 3) {
                    //chunk = NULL;
                    break;
            }
	}
	
	return chunk;
}