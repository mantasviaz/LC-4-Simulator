#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"


/*
 * adds a new node to linked list pointed to by head
 */
int add_to_list (row_of_memory** head,
		 		 unsigned short int address,
		 		 unsigned short int contents)
{

	/* allocate memory for a single node */
	row_of_memory* new_row = malloc(sizeof(row_of_memory));
	if (new_row == NULL) {
		return -1; //malloc failed
	}
	/* populate fields in newly allocated node with arguments: address/contents */
	new_row->address = address;
	new_row->contents = contents;
    /* make certain to set other fields in structure to NULL */
	new_row->label = NULL;
	new_row->assembly = NULL;
	new_row->next = NULL;
	/* if head==NULL, node created is the new head of the list! */
	if (*head == NULL) {
		*head = new_row; // new node is the head
	} else {
        row_of_memory* current = *head;
        row_of_memory* prev = NULL;

        while (current != NULL && current->address < address) {
            prev = current;
            current = current->next;
        }

        if (prev == NULL) {
            new_row->next = *head;
            *head = new_row;  // new node becomes the head
        } else {
            prev->next = new_row;
            new_row->next = current;  // insert in the middle or end
        }
    }

	/* return 0 for success, -1 if malloc fails */

	return 0 ;
}


/*
 * search linked list by address field, returns node if found
 */
row_of_memory* search_address (row_of_memory* head,
			        		   unsigned short int address )
{
	/* traverse linked list, searching each node for "address"  */
	row_of_memory* current = head;

	while (current != NULL) {
		if (current->address == address) {
			return current; //Found node with the matching address
		}

		current = current->next;
	}

	/* return NULL if list is empty or if "address" isn't found */
	return NULL ;
}

/*
 * search linked list by opcode field, returns node if found
 */
row_of_memory* search_opcode (row_of_memory* head,
				      		  unsigned short int opcode  )
{
	/* traverse linked list until node is found with matching opcode
	   AND "assembly" field of node is empty */
	row_of_memory* current = head;

	while (current != NULL) {
		unsigned short int instruction = current->contents;
		unsigned short int curr_opcode = (instruction >> 12) & 0xF; //Extract opcode

		if (curr_opcode == opcode && current->assembly == NULL) {
			return current; //Found node with the matching opcode and null assembly
		}
		current = current->next;
	}

	/* return NULL if list is empty or if no matching nodes */

	return NULL ;
}



/*
 * delete the node from the linked list with matching address
 */
int delete_from_list (row_of_memory** head,
			          unsigned short int address ) 
{
	/* if head isn't NULL, traverse linked list until node is found with matching address */
	if (*head == NULL) {
		return -1;
	} 

	row_of_memory* current = *head;
	row_of_memory* prev = NULL;
	
	/* delete the matching node, re-link the list */
	while (current != NULL && current->address != address) {
		prev = current;
		current = current->next;

	}

	if (current == NULL) {
		return -1;
	}

	
	/* make certain to update the head pointer - if original was deleted */
	if (prev == NULL) {
		*head = current->next;
	} else {
		prev->next = current->next;

	}

	free(current);	//free the memory of deleted node
	/* return 0 if successfully deleted the node from list, -1 if node wasn't found */

	return 0 ;

}

void print_list (row_of_memory* head, 
				 FILE* output_file )
{
	/* make sure head isn't NULL */
	if (head == NULL) {
		return;
	}
	/* traverse linked list, print contents of each node to output_file */

	row_of_memory* current = head;

    while (current != NULL) {

        if (current->label != NULL) {
            fprintf(output_file, "%s\t\t", current->label);
			printf("%s\t\t", current->label);
        } else {
            fprintf(output_file, "\t\t\t");
			printf("\t\t\t");
        }

		fprintf(output_file, "0x%04X\t\t\t", current->address);
		printf("0x%04X\t\t\t", current->address);

        fprintf(output_file, "0x%04X\t\t\t", current->contents);
		printf("0x%04X\t\t\t", current->contents);

        if (current->assembly != NULL) {
            fprintf(output_file, "%s", current->assembly);
			printf("%s", current->assembly);
        }

        fprintf(output_file, "\n");
		printf("\n");
        current = current->next;
    }
}

/*
 * delete entire linked list
 */
void delete_list (row_of_memory** head)
{
    row_of_memory* current = *head;
    row_of_memory* next;

    // Traverse the list and delete each node
    while (current != NULL) {
        next = current->next;
		free(current->label);
		free(current->assembly);
        free(current);  // Free the node itself
        current = next;
    }

    // Set the head pointer to NULL after deleting the entire list
    *head = NULL;
	return ;
}
