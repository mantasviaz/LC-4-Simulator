/************************************************************************/
/* File Name : lc4_loader.c		 										*/
/* Purpose   : This file implements the loader (ld) from PennSim		*/
/*             It will be called by main()								*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"
#include "lc4_hash.h"

//endianness helper function
#define endian_switch(num) (((num & 0xFF00) >> 8) | ((num & 0x00FF) << 8))

/* declarations of functions that must defined in lc4_loader.c */
//open file function
FILE* open_file(char* file_name)
{
	FILE* file = fopen(file_name, "r+");
	if (file == NULL) {
		printf("Error opening file: %s\n", file_name);
		return NULL;
	}
	return file;
}

//function to parse through file and add lines to hash table
int parse_file (FILE* my_obj_file, lc4_memory_segmented* memory)
{
	if (my_obj_file == NULL || memory == NULL) {
		return -1; //Returning and error if either pointer is NULL
	}

	//start from beginning of obj file
	rewind(my_obj_file);

	//run while not at end of obj file
	while (!feof(my_obj_file)) {
		char* label = NULL;
		unsigned short int header, address, n, tempHeader, tempAddress, temp_n;
		int num_read = fread(&tempHeader, sizeof(header), 1, my_obj_file);
		header = endian_switch(tempHeader); //switch endianness

		if (num_read != 1) {
			if (feof(my_obj_file)) {
				break; //Reached end of file, exit loop
			} else {
				printf("Error reading header from object file.\n");
				return -2; // Error occured while reading
			}
		}

		//read in address
		num_read = fread(&tempAddress, sizeof(address), 1, my_obj_file);
		address = endian_switch(tempAddress); //switch endianness
		if (num_read != 1) {
			printf("Error reading address from object file.\n");
			return -2; // Error occured while reading
		} 

		//read in number of elements
		num_read = fread(&temp_n, sizeof(n), 1, my_obj_file);
		n = endian_switch(temp_n); //switch endianness
		if (num_read != 1) {
			printf("Error reading n from object file.\n");
			return -2; // Error occured while reading			
		}

		switch(header) {
			case 0xCADE:  // CODE header
			case 0xDADA:  // DATA header
				for (int i = 0; i < n; i++) {
					unsigned short int content;
					unsigned short int temp_content;
					num_read = fread(&temp_content, sizeof(content), 1, my_obj_file);
					content = endian_switch(temp_content); //switch endianness
					if (num_read != 1) {
						printf("Error reading content from object file.\n");
						return -2; // Error occured while reading	
					}		

					//create node with the address and content
					if (add_entry_to_tbl(memory, address + i, content) == -1) {
						printf("Error adding entry to memory.\n");
						return -3;  //Error occurred while adding entry to memory
					}
				}
				break;
			case 0xC3B7:  //SYMBOL header
				label = malloc(n+1); //Allocate memory for the label string
				if (label == NULL) {
					printf("Error allocating memory for label.\n");
					return -4;  //Error occurred while allocating memory
				}

				//read in the label
				num_read = fread(label, sizeof(char), n, my_obj_file);
				if (num_read != n) {
                    printf("Error reading label from object file.\n");
                    free(label);

                    return -2;  // Error occurred while reading
				}
				label[n] = '\0'; //Null-terminating the label string

				//search for node with the address
				row_of_memory* node = search_tbl_by_address(memory, address);
				if (node != NULL) {
					if(node->label != NULL) {
						free(node->label); //Free the old label if it exists
					}
					node->label = label; //Assign the new label to the node
				} else {
					free(label); //Free the label if the corresponding node doesn't exist
				}
				break;
			default:
				printf("Invalid header type: 0x%04X\n", header);
				return -5;	//Invalid header type encountered
		}
	}
	//close file and free table
	if (fclose(my_obj_file) == EOF) {
    	printf("Error closing obj file: %s\n", (char*)my_obj_file);
    	delete_table(memory);
	    return 2;
	}
	return 0 ; //Parsing completed successfully
}
