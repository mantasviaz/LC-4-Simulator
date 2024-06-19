/************************************************************************/
/* File Name : lc4.c 													*/
/* Purpose   : This file contains the main() for this project			*/
/*             main() will call the loader and disassembler functions	*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"
#include "lc4_hash.h"
#include "lc4_loader.h"
#include "lc4_disassembler.h"

/* hash function to determine which bucket to insert/search/delete in hash table */
int hash_function(void* table, void* key) {
    lc4_memory_segmented* hash_memory = (lc4_memory_segmented*)table;
    unsigned short int address = *((unsigned short int*)key);

    if (address >= 0x0000 && address <= 0x1FFF) {
        return 0;  // User Program Memory
    } else if (address >= 0x2000 && address <= 0x7FFF) {
        return 1;  // User Data Memory
    } else if (address >= 0x8000 && address <= 0x9FFF) {
        return 2;  // OS Program Memory
    } else if (address >= 0xA000 && address <= 0xFFFF) {
        return 3;  // OS Data Memory
    } else {
        return -1;  // Invalid address
    }
}

/* program to mimic pennsim loader and disassemble object files */

int main (int argc, char** argv) {

	/**
	 * main() holds the hashtable &
	 * only calls functions in other files
	 */

	//check if obj file is given
    if (argc < 3) {
        printf("error1: usage: ./lc4 <object_file.obj> \n");
        return -1;
    }

	/* step 1: create a pointer to the hashtable: memory 	*/
	lc4_memory_segmented* memory = NULL ;

	/* step 2: call create_hash_table() and create 4 buckets 	*/
	memory = create_hash_table(4, hash_function);
	if (memory == NULL) {
		printf("Error creating hash table.\n");
		return -1;
	}

	/* step 3: determine filename, then open it		*/
	/*   TODO: extract filename from argv, pass it to open_file() */
	char* output_filename = argv[1];
	FILE* output_file = open_file(output_filename);
	if (output_file == NULL) {
        printf("Error opening output file: %s\n", output_filename);
        delete_table(memory);
        return 1;
    }

	//run for each obj file
    for (int i = 2; i < argc; i++) {
        char* obj_filename = argv[i];
        FILE* obj_file = open_file(obj_filename);

        if (obj_file == NULL) {
            printf("Error opening object file: %s\n", obj_filename);
            delete_table(memory);
            fclose(output_file);
            return 1;
        }
	/* step 4: call function: parse_file() in lc4_loader.c 	*/
	/*   TODO: call function & check for errors		*/
        if (parse_file(obj_file, memory) != 0) {
            printf("Error parsing object file: %s\n", obj_filename);
            fclose(obj_file);
            delete_table(memory);
            fclose(output_file);
            return 1;
        }
    }	
	/* step 5: repeat steps 3 and 4 for each .OBJ file in argv[] 	*/


	/* step 6: call function: reverse_assemble() in lc4_disassembler.c */
	/*   TODO: call function & check for errors		*/
	if (reverse_assemble(memory) != 0) {
		printf("Error during reverse assembly.\n");
		delete_table(memory);
		fclose(output_file);
		return 1;
	}

	/* step 7: call function: print out the hashtable to output file */
	/*   TODO: call function 				*/
	print_table(memory, output_file);

	/* step 8: call function: delete_table() in lc4_hash.c */
	/*   TODO: call function & check for errors		*/
	delete_table(memory);
	fclose(output_file);

	/* only return 0 if everything works properly */
	return 0 ;
}
