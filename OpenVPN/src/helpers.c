#include <stdlib.h>
#include <string.h>
#include "helpers.h"
#include <stdio.h>

static int string_parse(char *string, struct Clients **clients);
static int split_into_parts(char *string, int client_number, struct Clients **clients);

/*
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

/*
 * parse received message and remove unnecessary characters
 */
char *parse_pid(char *message)
{
    char *parsed_message;
    parsed_message = strchr(message, '=');

    if (parsed_message == NULL) return parsed_message;

    parsed_message++;
    remove_char(parsed_message);

    return parsed_message;
}

/*
 * parse received message and remove unnecessary characters
 * default lines if no clients is connected = 8
 */
int parse_status(char *message, int *clients_count, struct Clients **clients)
{
    char *message_start = NULL;
    char *message_end = NULL;
    char *mystring = NULL;

    *clients_count = count_lines(message);

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
            return 1;

    string_parse(mystring, clients);

    free(mystring);

    return 0;
}

/*
 * split message when \n occur
 */
static int string_parse(char *string, struct Clients **clients)
{
    char *token;
    int client_number = 0;

    while((token = strtok_r(string, "\n", &string))) {
            split_into_parts(token, client_number, clients);
            client_number++;
    }
    return 0;
}

/*
 * split line into parts
 */
static int split_into_parts(char *string, int client_number, struct Clients **clients)
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
    return 0;
}
/*
 * count lines and return clients count
 * 8 - default number of lines if zero clients are connected.
 * If client connects +2 lines are added to message.
 */
int count_lines(char *string)
{
    int count = 0;
    string = strchr(string, '\n');

    while (string != NULL) {
            string = strchr(string+1, '\n');
            count++;
    }

    return (count-8)/2;
}

/*
 * malloc and copy given text to return_message 
 * set len pointer the value
 */
char *malloc_message(char *text, int size, int *len)
{
    char *return_message;
    return_message = (char *) malloc(sizeof(char) * size);
    if (return_message == NULL) return return_message;

	strcpy(return_message, text);
    *len = strlen(return_message);

    return return_message;
}