#include <stdlib.h>

struct Node
{
    char *data;
    struct Node *next;
};

extern void push(char *x);
extern char **take_list(int length);
extern void delete_list();
extern void print_list();
extern int length();