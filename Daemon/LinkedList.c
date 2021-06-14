#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"

struct Node *head = NULL;
struct Node *current = NULL;

/* Add element to Linked List */
void push(char *x)
{
	struct Node *temp;
	
	temp = (struct Node*)malloc(sizeof(struct Node));
	temp->data = (char *) malloc (sizeof(char) * strlen(x)+1);
    strcpy(temp->data, x);
	temp->next = NULL;
	
	if (head != NULL)
		    temp->next = head;
	
    head = temp;
}
/* Print linked list elements */
void print_list() 
{
    struct Node *ptr = head;
    printf("List elements:\n");
    while(ptr != NULL) {
            printf("%s\n",ptr->data);
            ptr = ptr->next;
    }
}
/* return linked list as string array */
char **take_list(int length) 
{
     char **sub_str = (char **) malloc(length * sizeof(char**));

    struct Node *ptr = head;
    int i = 0;

    while(ptr != NULL) {
        sub_str[i] = (char *) malloc(sizeof(char) * strlen(ptr->data)+1);
        strcpy(sub_str[i], ptr->data);
        //sub_str[i] = ptr->data;
        ptr = ptr->next;
        i++;
    }
    return sub_str;
}

/* delete node*/
void delete_list()
{
	struct Node *temp;
	
	while(head != NULL){
            temp = head;
            head=head->next;
            free(temp->data);
            free(temp);
	}
}
/* return size of linked list */
int length()
{
    struct Node *current = head;
    int count = 0;

    while(current != NULL) {
            count++;
            current = current->next;
    }

    return count;
}
