#include <stdio.h>
#include "lc4_memory.h"


int main ()
{

    /* This main() is simply to test the basic functionalty of lc4_memory.c for part 1.
       It must not be used for part 2 of the assignment
    */

    /* Step 1: create a local linked list head pointer and allocate memory to it using 'add_to_list' */
	row_of_memory* test_list = NULL ;

    /* Step 2: test your linked list by adding some 'dummy' data: 
               add_to_list(..., 0x0000, 0x1234), add_to_list(..., 0x0001, 0x5678), ... */
    add_to_list(&test_list, 0x0000, 0x1234);
    add_to_list(&test_list, 0x0001, 0x5678);
    
	/* Step 3: print your list, see it works! */
    printf("Initial List:\n");
    print_list(test_list, stdout);

    /* Step 4: try adding data to the list out of order: 
             add_to_list(..., 0x0003, 0x9ABC), add_to_list(..., 0x0002, 0xDEF0) */
    add_to_list(&test_list, 0x0003, 0x9ABC);
    add_to_list(&test_list, 0x0002, 0xDEF0);

    /* Step 5: print your list, ensure it is in ascedning order by address */
    printf("\nList After Adding Out of Order:\n");
    print_list(test_list, stdout);
    
	/* Step 6: delete a single node, say the one with address: 0x0002 */
    delete_from_list(&test_list, 0x0002);

    /* Step 7: print your list (to FILE:stdout), ensure that the list is still intact */
    printf("\nList After Deleting Node with Address 0x0002:\n");
    print_list(test_list, stdout);

    /* Step 8: try deleting the first node in the list and then 
               print your list, ensure that the list is still intact */
	delete_from_list(&test_list, 0x0000);
    printf("\nList After Deleting First Node:\n");
    print_list(test_list, stdout);

    /* Step 9: when finished, run valgrind on your program to see if there are any leaks
               points will be lost even if your program is working but leaking memory */
	delete_list(&test_list);

	return 0;
}
