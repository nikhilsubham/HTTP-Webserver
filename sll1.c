#include "sll1.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

/* Public Function Implementation to create and return new empty singly linked list*/
dll_t * get_new_dll(){

    dll_t *dll = calloc(1, sizeof(dll_t));
    dll->head = NULL;
    return dll;
}


/* Given a reference (pointer to pointer) to the head of a list inserts a new node on the front of the list. */
void At_front(struct Node** head_ref, void*app_data) 
{ 
	/* 1. allocate node */
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node)); 

	/* 2. put in the data */
	new_node->data = app_data; 

	/* 3. Make next of new node as head */
	new_node->next = (*head_ref); 

	/* 4. move the head to point to the new node */
	(*head_ref) = new_node; 
} 


/* Given a node prev_node, insert a new node after the given prev_node */
void insertAfter(struct Node* prev_node, void* new_data) 
{ 
	/*1. check if the given prev_node is NULL */
	if (prev_node == NULL) 
	{ 
	printf("the given previous node cannot be NULL");	 
	return; 
	} 
		
	/* 2. allocate new node */
	struct Node* new_node =(struct Node*) malloc(sizeof(struct Node)); 

	/* 3. put in the data */
	new_node->data = new_data; 

	/* 4. Make next of new node as next of prev_node */
	new_node->next = prev_node->next; 

	/* 5. move the next of prev_node as new_node */
	prev_node->next = new_node; 
} 


/* Given a reference (pointer to pointer) to the head of a list and a data, appends a new node at the end */
void At_end(struct Node** head_ref, void* new_data) 
{
	/* 1. allocate node */
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node)); 

	struct Node *last = *head_ref; /* used in step 5*/

	/* 2. put in the data */
	new_node->data = new_data; 

	/* 3. This new node is going to be the last node, so make next 
		of it as NULL*/
	new_node->next = NULL; 

	/* 4. If the Linked List is empty, then make the new node as head */
	if (*head_ref == NULL) 
	{ 
	*head_ref = new_node; 
	return; 
	} 
	
	/* 5. Else traverse till the last node */
	while (last->next != NULL) 
		last = last->next; 

	/* 6. Change the next of last node */
	last->next = new_node; 
	return;	 
} 


void deleteNode(struct Node **head_ref, struct Node *del)
{ 
    struct Node* temp = (*head_ref); 
    struct Node* prev = NULL;

    if(*head_ref == NULL || del == NULL) 
        return; 

    if(temp == del) 
     {
        *head_ref = del->next;
        free(del); 
        return; 
     } 
    
    while(temp != del && temp != NULL)
    {
        prev = temp;
        temp= temp ->next;
    }

    if(temp == NULL)
       return;

    prev->next = temp ->next;
    free(temp); 
} 


void deleteList(struct Node** head_ref) 
{ 
   struct Node* current = *head_ref; 
   struct Node* next; 
  
   while (current != NULL)  
   { 
       next = current->next; 
       free(current); 
       current = next; 
   } 
   *head_ref = NULL; 
} 











