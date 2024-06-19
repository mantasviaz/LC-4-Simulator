#include <stdio.h>
#include <stdlib.h>
#include "lc4_hash.h"

/*
 * creates a new hash table with num_of_buckets requested
 */
lc4_memory_segmented* create_hash_table (int num_of_buckets, 
             					         int (*hash_function)(void* table, void *key) ) 
{
	// allocate a single hash table struct
	lc4_memory_segmented* table = (lc4_memory_segmented*)malloc(sizeof(lc4_memory_segmented));
	if (table == NULL) {
		printf("Error allocating memory for hash table.\n");
		return NULL;
	}
	// allocate memory for the buckets (head pointers)
	table->num_of_buckets = num_of_buckets;
	table->buckets = malloc(num_of_buckets * sizeof(row_of_memory*));
	if(table->buckets == NULL) {
		printf("Error allocating memory for hash table buckets.\n");
		free(table);
		printf("Test 1");
		return NULL;
	}
	// assign function pointer to call back hashing function
	table->hash_function = hash_function;

	// return 0 for success, -1 for failure
	return table;
}


/*
 * adds a new entry to the table
 */
int add_entry_to_tbl (lc4_memory_segmented* table, 
					  unsigned short int address,
			    	  unsigned short int contents) 
{
	// apply hashing function to determine proper bucket #
	if (table == NULL || table->hash_function == NULL) {
		return -1; //Return an error if table or hash function is NULL
	}

	int bucket_index = table->hash_function(table, (void*)&address);
	if (bucket_index < 0 || bucket_index >= table->num_of_buckets) {
		return -1; //Return an error if bucket index is out of range
	}
	// add to bucket's linked list using linked list add_to_list() helper function
	return add_to_list(&(table->buckets[bucket_index]), address, contents);
}

/*
 * search for an address in the hash table
 */
row_of_memory* search_tbl_by_address 	(lc4_memory_segmented* table,
			                   			 unsigned short int address ) 
{
	// apply hashing function to determine bucket # item must be located in
	if (table == NULL || table->hash_function == NULL) {
		return NULL; //Return NULL if table or hash function is NULL
	}

	int bucket_index = table->hash_function(table, (void*)&address);
	if (bucket_index < 0 || bucket_index >= table->num_of_buckets) {
		return NULL; //Return NULL if bucket index is out of range
	}

	// invoked linked_lists helper function, search_by_address() to return return proper node
	return search_address(table->buckets[bucket_index], address);
}

/*
 * prints the linked list in a particular bucket
 */

void print_bucket (lc4_memory_segmented* table, 
				   int bucket_number,
				   FILE* output_file ) 
{
	// call the linked list helper function to print linked list
	if (table == NULL || bucket_number < 0 || bucket_number >= table->num_of_buckets) {
		return; //Return if table is NULL or bucket number is out of range
	}
	print_list(table->buckets[bucket_number], output_file);
}

/*
 * print the entire table (all buckets)
 */
void print_table (lc4_memory_segmented* table, 
				  FILE* output_file ) 
{
	// call the linked list helper function to print linked list to output file for each bucket
	if (table == NULL) {
		return; //Return if table is NULL
	}
	/* print out a header to output_file */
    fprintf(output_file, "<label>\t\t<address>\t\t<contents>\t\t<assembly>\n");
	printf("<label>\t\t<address>\t\t<contents>\t\t<assembly>\n");

	for (int i = 0; i < table->num_of_buckets; i++) {
		print_bucket(table, i, output_file);
	}
}

/*
 * delete the entire table and underlying linked lists
 */

void delete_table (lc4_memory_segmented* table ) 
{
	if (table == NULL) {
		return; //Return if table is NULL
	}	
	// call linked list delete_list() on each bucket in hash table

	for (int i = 0; i < table->num_of_buckets; i++) {
		delete_list(&(table->buckets[i]));
	}
	// then delete the table itself
	free(table->buckets);
	free(table);
}
