#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "helpers.h"
#include "main.h"
#include "linked_list.h"

static void logs_string_parse(char *string, struct Log_messages **logs);
static void status_string_parse(char *string);
static void split_into_parts(char *string);
static int count_lines(char *string, int _case);

/**
 * get status information from server
 * parse message and fill structure with information
 * @return 0 - success; 1 - client count is 0; 2 - allocation problems
 */
int gather_status()
{
    char *send_message;
    char *received_message;
    int len = 0;
    int rc = 0; //return code

    recv_all(); //!< receive unnecessary messages (example - new client connect)

    send_message = malloc_message("status\n", &len);
    if (send_message == NULL) return 2;

    send_all(send_message, &len);
    received_message = recv_all();

    if (received_message == NULL) goto cleanup_1;

    rc = parse_status(received_message);

    free(received_message);
    cleanup_1:
            free(send_message);
    return rc;
}

/**
 * parse received message and remove unnecessary characters
 * @return 0 - success; 1 - client count is zero; 2 - allocation problems
 */
int parse_status(char *message)
{
    char *message_start = NULL;
    char *message_end = NULL;
    char *mystring = NULL;
    int clients_count;

    clients_count = count_lines(message, 0);

    if (clients_count == 0)
            return 1;

    message_start = strstr(message, "Since\r\n")+7;
    message_end = strstr(message, "ROUTING TABLE\r\n");

    mystring = (char *) malloc((sizeof(char) * 100) * (clients_count));

    if(mystring == NULL) return 1;

    memmove(mystring, message_start, message_end - message_start-1);
    mystring[message_end - message_start-1] = '\0';

    if (clients != NULL) {
            delete_list(clients);
            clients = (struct Clients *) calloc(1,sizeof(struct Clients));
    }

    if (clients_count > 0)
            status_string_parse(mystring);

    free(mystring);

    return 0;
}

/**
 * split message when \n occur and split tokens into parts
 */
static void status_string_parse(char *string)
{
    char *token;

    while((token = strtok_r(string, "\n", &string))) {
            split_into_parts(token);
    }
}

/**
 * split line into parts
 */
static void split_into_parts(char *string)
{
    char temp_string[80];
    char *token;
    char *rest = NULL;
    int counter = 0;

    struct Clients *new_client;
    struct Clients temp_client;
    strcpy(temp_string, string);

    for (token = strtok_r(temp_string, ",", &rest);
            token != NULL;
            token = strtok_r(NULL, ",", &rest)) {
            remove_char(token);
            if (counter == 0) 
                    strcpy(temp_client.name, token);
            else if (counter == 1)
                    strcpy(temp_client.address, token);
            else if (counter == 2)
                    strcpy(temp_client.bytes_received, token);
            else if (counter == 3)
                    strcpy(temp_client.bytes_sent, token);
            else if (counter == 4)
                    strcpy(temp_client.connected, token); 
            counter++;  
    }
    new_client = create_client(temp_client);
    push_client(&clients, new_client);
}
/**
 * receive version from server
 * parse received message
 * @return version information on success; NULL - allocation problems
 */
char *set_version()
{
    char *received_message = NULL;
    char *send_message = NULL;
    int len = 0;

    recv_all(); //!< receive unnecessary messages (example - new client connect)

    send_message = malloc_message("version\n", &len);
    if (send_message == NULL) return NULL;

    send_all(send_message, &len);
    received_message = recv_all();      

    if (received_message == NULL) goto cleanup;

    received_message = parse_message(received_message, ':');

    cleanup:
            free(send_message);

    return received_message;
}

/**
 * receive pid from server
 * parse received message
 * @return pid number on success; 1 - allocation problems
 */
int set_pid()
{
    char *received_message = NULL;
    char *send_message = NULL;
    int pid = 0;
    int len = 0;

    recv_all(); //!< receive unnecessary messages (example - new client connect)

    send_message = malloc_message("pid\n", &len);
    if (send_message == NULL) return 1;

    send_all(send_message, &len);
    received_message = recv_all();      
    received_message = parse_message(received_message, '=');
    
    if (received_message == NULL) goto cleanup_1;

    pid = atoi(received_message);


    cleanup_1:
            free(send_message);
    return pid;
}

/**
 * parse logs received from server
 * @return 0 - success; 1 - allocation problems
 */
int parse_logs(char *string, int *messages_count, struct Log_messages **logs)
{
    if (*messages_count == 0)
            *messages_count = count_lines(string, 1);

    *logs = (struct Log_messages *) malloc(sizeof(struct Log_messages) * (*messages_count));

    if (logs == NULL)
            return 1;

    logs_string_parse(string, logs);

    return 0;
}

/**
 * split message when \n occur
 */
static void logs_string_parse(char *string, struct Log_messages **logs)
{
    char *token;
    int log_number = 0;

    while((token = strtok_r(string, "\n", &string))) {
            remove_char(token);
            strcpy((*logs)[log_number].message, token);
            log_number++;
    }
}

/**
 * count lines of given string
 * @return:
 * _case 0:
 * 8 is default number of lines if zero clients are connected.
 * If client connects +2 lines are added to message. Formula: (count-8)/2
 * other:
 * return normal count number
 */
static int count_lines(char *string, int _case)
{
    int count = 0;
    string = strchr(string, '\n');

    while (string != NULL) {
            string = strchr(string+1, '\n');
            count++;
    }

    if (_case == 0)
            count = (count-8)/2;

    return count;
}

/**
 * malloc and copy given text to return_message 
 * set len pointer the value
 * @return malloced message or NULL if failed
 */
char *malloc_message(char *text, int *len)
{
    char *return_message;
    return_message = (char *) malloc(sizeof(text));
    if (return_message == NULL) return return_message;

	strcpy(return_message, text);
    *len = strlen(return_message);

    return return_message;
}

/**
 * remove \n and \r from string for ubus printing
 */
void remove_char(char *s)
{
    int writer = 0, reader = 0;

    while (s[reader])
    {
            if (s[reader]!= '\n' && s[reader] != '\r') 
            {   
                    s[writer++] = s[reader];
            }

            reader++;    
    }
    s[writer]=0;
}

/**
 * if parsing is only needed for removing unnecessary symbol from received message, we use this method
 * @return parsed message on success, NULL if something failed 
 */
char *parse_message(char *message, char symbol)
{
    char *parsed_message;
    parsed_message = strchr(message, symbol);

    if (parsed_message == NULL) return parsed_message;

    parsed_message++;
    remove_char(parsed_message);

    return parsed_message;
}