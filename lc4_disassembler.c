#include <stdio.h>
#include <stdlib.h>
#include "lc4_hash.h"

//helper function to perform SEXT
short int signExtend(unsigned short int value, int numBits) {
    short int extendedValue;
    int signBit = (value >> (numBits - 1)) & 1;
    
    if (signBit == 1) {
        // Negative number
        unsigned short int mask = ~((1 << numBits) - 1);
        extendedValue = value | mask;
    } else {
        // Positive number
        unsigned short int mask = (1 << numBits) - 1;
        extendedValue = value & mask;
    }
    
    return extendedValue;
}

//function to reverse_assemble the instruction into string form
int reverse_assemble (lc4_memory_segmented* memory) 
{
	for (int i = 0; i < memory->num_of_buckets; i++) {
		row_of_memory* node = memory->buckets[i];

		while (node != NULL) {
			unsigned short int instruction = node->contents;
			unsigned short int opcode = (instruction >> 12) & 0xF;

			//check for the arith or logic opcodes and that assembly is empty
			if((opcode == 0x1 || opcode == 0x5) && node->assembly == NULL) {
				char* assembly = NULL;
				switch (opcode) {
					//ARITH
					case 0x1:
						//ADD IMM5
						if (((instruction >> 5) & 0x1) == 1) {
								assembly = malloc(14);
								sprintf(assembly, "ADD R%d R%d #%d", (instruction >> 9) & 0x7,
                                        (instruction >> 6) & 0x7, signExtend(instruction & 0x1F, 5));
								break;
						}
						switch((instruction >> 3) & 0x7) {
							//ADD
							case 0x0:
								assembly = malloc(13);
								sprintf(assembly, "ADD R%d R%d R%d", (instruction >> 9) & 0x7,
                                        (instruction >> 6) & 0x7, instruction & 0x7);
								break;
							//MUL
							case 0x1:
								assembly = malloc(13);
								sprintf(assembly, "MUL R%d R%d R%d", (instruction >> 9) & 0x7,
                                        (instruction >> 6) & 0x7, instruction & 0x7);
								break;	
							//SUB						
							case 0x2:
								assembly = malloc(13);
								sprintf(assembly, "SUB R%d R%d R%d", (instruction >> 9) & 0x7,
                                        (instruction >> 6) & 0x7, instruction & 0x7);
								break;	
							//DIV	
							case 0x3:
								assembly = malloc(13);
								sprintf(assembly, "DIV R%d R%d R%d", (instruction >> 9) & 0x7,
                                        (instruction >> 6) & 0x7, instruction & 0x7);
								break;																			
						}
						break;
					//logic instr
					case 0x5:
						//AND IMM5
						if (((instruction >> 5) & 0x1) == 1) {
							assembly = malloc(14);
							sprintf(assembly, "AND R%d R%d #%d", (instruction >> 9) & 0x7,
                                        (instruction >> 6) & 0x7, signExtend(instruction & 0x1F, 5));
							break;
						}		
						switch((instruction >> 3) & 0x7) {
							//AND
							case 0x0:
								assembly = malloc(13);
								sprintf(assembly, "AND R%d R%d R%d", (instruction >> 9) & 0x7,
                                        (instruction >> 6) & 0x7, instruction & 0x7);
								break;
							//NOT
							case 0x1:
								assembly = malloc(10);
								sprintf(assembly, "NOT R%d R%d", (instruction >> 9) & 0x7,
                                        (instruction >> 6) & 0x7);
								break;
							//OR							
							case 0x2:
								assembly = malloc(12);
								sprintf(assembly, "OR R%d R%d R%d", (instruction >> 9) & 0x7,
                                        (instruction >> 6) & 0x7, instruction & 0x7);
								break;		
							//XOR
							case 0x3:
								assembly = malloc(13);
								sprintf(assembly, "XOR R%d R%d R%d", (instruction >> 9) & 0x7,
                                        (instruction >> 6) & 0x7, instruction & 0x7);
								break;																			
						}
						break;										
				}
				if (assembly != NULL) {
					free(node->assembly); //free memory
					node->assembly = assembly;
				}	
			}
			node = node->next;
		}
	}
	return 0 ;
}
