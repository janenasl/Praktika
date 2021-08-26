#include <stdlib.h>
#include <string.h>
#include "helpers.h"
#include <stdio.h>


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
char *parse_status(char *message, int *count)
{
    char *parsed_message;

    *count = count_lines(message)-8;

    if (*count == 0 || parsed_message == NULL) {
            strcpy(parsed_message, "no clients are connected");
            return parsed_message;
    }

    remove_char(parsed_message);

    return parsed_message;
}
/*
 * count lines of given string
 */
int count_lines(char *string)
{
    int count = 0;
    string = strchr(string, '\n');

    while (string != NULL) {
            string = strchr(string+1, '\n');
            count++;
    }

    return count;
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