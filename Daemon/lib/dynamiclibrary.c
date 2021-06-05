#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamiclibrary.h"

struct Node *head = NULL;
struct Node *current = NULL;

void push(char *x)
{
	struct Node *temp;
	
	temp = (struct Node*)malloc(sizeof(struct Node));
	
    temp->data = x;
	temp->next = NULL;
	
	if (head != NULL)
		temp->next = head;
	
    head = temp;
}

void printList() 
{
    struct Node *ptr = head;
    printf("Print list elements:\n");
    while(ptr != NULL) {
        printf("%s\n",ptr->data);
        ptr = ptr->next;
    }
}

char **takeList(int length) 
{
    char ** sub_str = malloc(100 * sizeof(char**));
    int i = 0;
    for (int i =0 ; i < length; i++) {
        sub_str[i] = malloc(500 * sizeof(char));
    }
    
    struct Node *ptr = head;
    i = 0;
    while(ptr != NULL) {
        strcpy(sub_str[i++], ptr->data);
        ptr = ptr->next;
    }
    return sub_str;
}

void deleteList()
{
	struct Node *temp;
	
	while(head != NULL){
	temp = head;
	head=head->next;
	free(temp);
	}
}

int length() {
    int count = 0;
    struct Node *current = head;

    while(current != NULL) {
        count++;
		current = current->next;
    }

    return count;
}
