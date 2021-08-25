#include <stdlib.h>
#include <string.h>
#include "helpers.h"

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
    char *parsed_messaged;
    parsed_messaged = strchr(message, '=');

    if (parsed_messaged == NULL) return parsed_messaged;

    parsed_messaged++;
    remove_char(parsed_messaged);

    return parsed_messaged;
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