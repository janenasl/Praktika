#include <stdlib.h>
#include <string.h>
#include "helpers.h"
#include <stdio.h>

static void split_into_parts(char *string, int client_number, struct Clients **clients);
static void logs_string_parse(char *string, struct Log_messages **logs);
static void status_string_parse(char *string, struct Clients **clients);
static int count_lines(char *string, int _case);

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

/**
 * parse received message and remove unnecessary characters
 * @return 0 - success; 1 - client count is zero; 2 - allocation problems
 */
int parse_status(char *message, int *clients_count, struct Clients **clients)
{
    char *message_start = NULL;
    char *message_end = NULL;
    char *mystring = NULL;

    *clients_count = count_lines(message, 0);

    if (*clients_count == 0)
            return 1;

    message_start = strstr(message, "Since\r\n")+7;
    message_end = strstr(message, "ROUTING TABLE\r\n");

    mystring = (char *) malloc((sizeof(char) * 100) * (*clients_count));

    if(mystring == NULL) return 1;

    memmove(mystring, message_start, message_end - message_start-1);
    mystring[message_end - message_start-1] = '\0';

    *clients = (struct Clients *) calloc ((*clients_count), sizeof(struct Clients));

    if (*clients == NULL)
            return 2;

    status_string_parse(mystring, clients);

    free(mystring);

    return 0;
}

/**
 * split message when \n occur and split tokens into parts
 */
static void status_string_parse(char *string, struct Clients **clients)
{
    char *token;
    int client_number = 0;

    while((token = strtok_r(string, "\n", &string))) {
            split_into_parts(token, client_number, clients);
            client_number++;
    }
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
 * split line into parts
 */
static void split_into_parts(char *string, int client_number, struct Clients **clients)
{
    char temp_string[80];
    char *token;
    char *rest = NULL;
    int counter = 0;
    
    strcpy(temp_string, string);

    for (token = strtok_r(temp_string, ",", &rest);
            token != NULL;
            token = strtok_r(NULL, ",", &rest)) {
            remove_char(token);
            if (counter == 0) 
                    strcpy((*clients)[client_number].name, token);
            if (counter == 1)
                    strcpy((*clients)[client_number].address, token);
            if (counter == 2)
                    strcpy((*clients)[client_number].bytes_received, token);
            if (counter == 3)
                    strcpy((*clients)[client_number].bytes_sent, token);
            if (counter == 4)
                    strcpy((*clients)[client_number].connected, token); 
            counter++;
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