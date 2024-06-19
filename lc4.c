/*
 * LC4.c: Defines simulator functions for executing instructions
 */

#include "LC4.h"
#include <stdio.h>
unsigned short int instr;
unsigned short int bin1;
unsigned short int bin2;
unsigned short int bin3;
unsigned short int bin4;
unsigned short int currentPC;
/*
 * Reset the machine state as Pennsim would do
 */
void Reset(MachineState* CPU)
{
	CPU->PC = 0x8200;
	CPU->PSR = 0x8002;
	memset(CPU->R, 0, sizeof(CPU->R));
	ClearSignals(CPU);

}


/*
 * Clear all of the control signals (set to 0)
 */
void ClearSignals(MachineState* CPU)
{
	CPU->rsMux_CTL = 0;
	CPU->rtMux_CTL = 0;
	CPU->rdMux_CTL = 0;
	CPU->regFile_WE = 0;
	CPU->NZP_WE = 0;
	CPU->DATA_WE = 0;
	CPU->regInputVal = 0;
	CPU->NZPVal = 0;
	CPU->dmemAddr = 0;
	CPU->dmemValue = 0;
}
//helper function to convert ints to binary
char* breakBinary(unsigned short int inst) {
    static char binary[17];
    int i;
    
    for (i = 0; i < 16; i++) {
        binary[15 - i] = (inst & (1 << i)) ? '1' : '0';
    }
    binary[16] = '\0';
    
    return binary;
}
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

/*
 * This function should write out the current state of the CPU to the file output.
 */
void WriteOut(MachineState* CPU, FILE* output)
{		
	char* instruction = breakBinary(CPU->memory[currentPC]);
    fprintf(output, "%04X %s %1X %1X %04X %1X %1X %1X %04X %04X\n",
          	currentPC, instruction, CPU->regFile_WE, CPU->rdMux_CTL,
            CPU->regInputVal, CPU->NZP_WE, CPU->NZPVal, CPU->DATA_WE,
            CPU->dmemAddr, CPU->dmemValue);
}



/*
 * This function should execute one LC4 datapath cycle.
 */
int UpdateMachineState(MachineState* CPU, FILE* output)
{
    instr = CPU->memory[CPU->PC];
		unsigned short int opcode = INSN_OP(instr);

		ClearSignals(CPU);
		currentPC = CPU->PC;
		//printf("Current PC is: %04X\n", currentPC);
		getchar();
		switch(opcode) {
			case 0b0000:
				//branch operations
				//printf("Starting Branch operation\n");
				if (INSN_RD(instr) != 0b000) {
						BranchOp(CPU, output);
				} else {
					//do nothing
					CPU->PC++;
				}
				break;
			case 0b0001:
				//arithmetic operations
				//printf("Starting arithmetic\n");
				ArithmeticOp(CPU, output);
				CPU->PC++;
				break;
			case 0b0010:
				//comparative operations
				//printf("Starting comp\n");
				ComparativeOp(CPU, output);
				CPU->PC++;
				break;
			case 0b0100:
				//JSR operations
				//printf("Starting jsr\n");
				JSROp(CPU, output);
				break;
			case 0b0101:
				//logical operations
				//printf("Starting logical\n");
				LogicalOp(CPU, output);
				CPU->PC++;
				break;
			case 0b0110:
				//LDR operation
			case 0b0111:
				//printf("Starting ldr/str\n");
				//STR operation
				CPU->regFile_WE = (opcode == 0b0110);
				CPU->DATA_WE = (opcode == 0b0111);
				CPU->dmemAddr = CPU->R[INSN_RS(instr)] + signExtend(INSN_IMM6(instr), 6);
				if (CPU->PC < 0x2000 && CPU->dmemAddr > 0x8000){
						printf("Error: Attempting to read or write code section address as data\n");
        		return 1;  // Terminate the program
					}
				if (opcode == 0b0110) {
					//if LDR
					CPU->dmemValue = CPU->memory[CPU->dmemAddr];
					CPU->regInputVal = CPU->dmemValue;
					CPU->rdMux_CTL = INSN_RD(instr);
					CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
					SetNZP(CPU, (CPU->regInputVal));
				} else {
					//if STR
					//printf("This is the address: %d\n", CPU->dmemAddr);
					CPU->rtMux_CTL = INSN_RD(instr);					
					CPU->dmemValue = CPU->R[CPU->rtMux_CTL];
					CPU->memory[CPU->dmemAddr] = CPU->dmemValue;
				}
				CPU->PC++;
				break;
			case 0b1000:
				//RTI operation
				//printf("Starting RTI\n");
          CPU->PC = CPU->R[7];
          CPU->PSR = (CPU->PSR & 0x7FFF);
					//printf("%d\n", CPU->PC);
					break;
			case 0b1001:
				//const operation
				//printf("Starting const\n");
				CPU->regFile_WE = 1;
				CPU->regInputVal = signExtend(INSN_IMM9(instr), 9);
				CPU->rdMux_CTL = INSN_RD(instr);
				CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
				SetNZP(CPU, (CPU->regInputVal));
				CPU->PC++;
				//SetNZP(CPU, CPU->regInputVal);
				break;
			case 0b1010:
				//printf("Starting shift\n");
				//shift operations
				ShiftModOp(CPU, output);
				CPU->PC++;
				break;
			case 0b1100:
				//jump operations
				//printf("Starting jump\n");
				JumpOp(CPU, output);
				break;
			case 0b1101:
				//hiconst operation
				//printf("Starting hiconst\n");
				CPU->regFile_WE = 1;
				CPU->rdMux_CTL = INSN_RD(instr);
				CPU->regInputVal = (((CPU->R[CPU->rdMux_CTL]) & 0xFF) | (INSN_UIMM8(instr) << 8));
				CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
				SetNZP(CPU, (CPU->regInputVal));
				CPU->PC++;
				break;
			case 0b1111:
				//trap operation
				//printf("Starting trap\n");
				CPU->regFile_WE = 1;
				CPU->regInputVal = CPU->PC + 1;
				CPU->rdMux_CTL = 7;
				CPU->R[7] = CPU->regInputVal;
				CPU->PC = (0x8000 | INSN_UIMM8(instr));
				CPU->PSR = CPU->PSR | 0x8000;
				SetNZP(CPU, (CPU->regInputVal));
				break;
			default:
				printf("Error: unkown opcode");
				return 1;
		}
		//printf("PC after operation: %04X\n", CPU->PC);
		//getchar();
		//printf("R5 is: %d\n", CPU->R[5]);
		//write out results
		WriteOut(CPU, output);
		//printf("WriteOut complete\n");

		//if past allotted memory then end
		if (CPU->PC == 0x80FF) {
			return 1;
		}

    return 0;
}



//////////////// PARSING HELPER FUNCTIONS ///////////////////////////



/*
 * Parses rest of branch operation and updates state of machine.
 */
void BranchOp(MachineState* CPU, FILE* output)
{
		int16_t offset = signExtend(INSN_IMM9(instr), 9);
		unsigned short int br_type = INSN_RD(instr);

		switch (br_type){
				//BRp
        case 0b001:
						//printf("entered first\n");
						if ((CPU->PSR & 0x0007) == 0b001) {
							CPU->PC =  CPU->PC + 1 + offset;
									//printf("PC after branch: %d\n", CPU->PC);
						} else {
							CPU->PC =  CPU->PC + 1;
						}
            break;
				//BRz
        case 0b010:
						if ((CPU->PSR & 0x0007) == 0b010) {
							CPU->PC =  CPU->PC + 1 + offset;
						} else {
							CPU->PC =  CPU->PC + 1;
						}
            break;
				//BRzp
        case 0b011:
						if (((CPU->PSR & 0x0007) == 0b010) || ((CPU->PSR & 0x0007) == 0b001)) {
							CPU->PC =  CPU->PC + 1 + offset;
						} else {
							CPU->PC =  CPU->PC + 1;
						}
            break;
				//BRn
        case 0b100:
						if ((CPU->PSR & 0x0007) == 0b100) {
							CPU->PC =  CPU->PC + 1 + offset;
						} else {
							CPU->PC =  CPU->PC + 1;
						}
            break;
				//BRnp
        case 0b101:
						if (((CPU->PSR & 0x0007) == 0b100) || ((CPU->PSR & 0x0007) == 0b001)) {
							CPU->PC =  CPU->PC + 1 + offset;
						} else {
							CPU->PC =  CPU->PC + 1;
						}
            break;
				//BRnz
        case 0b110:
						if (((CPU->PSR & 0x0007) == 0b100) || ((CPU->PSR & 0x0007) == 0b010)) {
							CPU->PC =  CPU->PC + 1 + offset;
						} else {
							CPU->PC =  CPU->PC + 1;
						}
            break;
				//BRnzp
        case 0b111:
            CPU->PC = (CPU->PC + 1 + offset);
            break;
		}
}
/*
 * Parses rest of arithmetic operation and prints out.
 */
void ArithmeticOp(MachineState* CPU, FILE* output)
{
		CPU->regFile_WE = 1;
    unsigned short int subcode = INSN_SUB(instr);
		unsigned short int subcodeIMM = INSN_SUBIMM(instr);

		//if IMM
		if (subcodeIMM == 0x1) {
			CPU->rdMux_CTL = INSN_RD(instr);
			CPU->rsMux_CTL = INSN_RS(instr);
			CPU->regInputVal = CPU->R[CPU->rsMux_CTL] + signExtend(INSN_IMM5(instr), 5);
			CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
			SetNZP(CPU, (CPU->regInputVal));
		} else {
			  CPU->rdMux_CTL = INSN_RD(instr);
				CPU->rsMux_CTL = INSN_RS(instr);
				CPU->rtMux_CTL = INSN_RT(instr);
			switch (subcode){
				//ADD
        case 0b000:
						CPU->regInputVal = CPU->R[CPU->rsMux_CTL] + CPU->R[CPU->rtMux_CTL];
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));
            break;
				//MUL
        case 0b001:
						CPU->regInputVal = CPU->R[CPU->rsMux_CTL] * CPU->R[CPU->rtMux_CTL];
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));
            break;
				//SUB
        case 0b010:
						CPU->regInputVal = CPU->R[CPU->rsMux_CTL] - CPU->R[CPU->rtMux_CTL];
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));
						break;				
				//DIV
        case 0b011:
						if (CPU->R[CPU->rtMux_CTL] == 0) {
							break;
						} else{
							CPU->regInputVal = CPU->R[CPU->rsMux_CTL] / CPU->R[CPU->rtMux_CTL];
							CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
							SetNZP(CPU, (CPU->regInputVal));
							break;
						}
						
		}
		}
}

/*
 * Parses rest of comparative operation and prints out.
 */
void ComparativeOp(MachineState* CPU, FILE* output)
{
    CPU->rsMux_CTL = INSN_RD(instr);
		CPU->rtMux_CTL = INSN_RT(instr);
		unsigned short int subCOMP = INSN_SUBCOMP(instr);
		int16_t IMM7 = INSN_IMM7(instr);

		switch (subCOMP){
        case 0b00:
				//CMP
						SetNZP(CPU, CPU->R[CPU->rsMux_CTL] - CPU->R[CPU->rtMux_CTL]);
            break;
        case 0b01:
				//CMPU
						SetNZP(CPU, (unsigned short int)(CPU->R[CPU->rsMux_CTL] - CPU->R[CPU->rtMux_CTL]));
            break;
        case 0b10:
				//CMPI
						SetNZP(CPU, (CPU->R[CPU->rsMux_CTL] - signExtend(IMM7, 7)));
						break;
        case 0b11:
				//CMPIU
						SetNZP(CPU, (unsigned short int)(CPU->R[CPU->rsMux_CTL] - IMM7));
            break;
		}
}

/*
 * Parses rest of logical operation and prints out.
 */
void LogicalOp(MachineState* CPU, FILE* output)
{
    CPU->regFile_WE = 1;
    unsigned short int subcode = INSN_SUB(instr);
		unsigned short int subcodeIMM = INSN_SUBIMM(instr);
		
		//if IMM
		if (subcodeIMM == 0x1) {
			CPU->rdMux_CTL = INSN_RD(instr);
			CPU->rsMux_CTL = INSN_RS(instr);
			CPU->regInputVal = CPU->R[CPU->rsMux_CTL] & signExtend(INSN_IMM5(instr), 5);
			CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
			SetNZP(CPU, (CPU->regInputVal));
		} else {
			  CPU->rdMux_CTL = INSN_RD(instr);
				CPU->rsMux_CTL = INSN_RS(instr);
				CPU->rtMux_CTL = INSN_RT(instr);
			switch (subcode){
				//AND
        case 0b000:
						CPU->regInputVal = CPU->R[CPU->rsMux_CTL] & CPU->R[CPU->rtMux_CTL];
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));
            break;
				//NOT
        case 0b001:
						CPU->regInputVal = ~CPU->R[CPU->rsMux_CTL];
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));
            break;
				//OR
        case 0b010:
						CPU->regInputVal = CPU->R[CPU->rsMux_CTL] | CPU->R[CPU->rtMux_CTL];
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));				
				//XOR
        case 0b011:
						CPU->regInputVal = CPU->R[CPU->rsMux_CTL] ^ CPU->R[CPU->rtMux_CTL];
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));
            break;
			 }
		}
}

/*
 * Parses rest of jump operation and prints out.
 */
void JumpOp(MachineState* CPU, FILE* output)
{
		int16_t IMM11 = signExtend(INSN_IMM11(instr), 11);
		CPU->rsMux_CTL = INSN_RS(instr);
		//if JUMP with IMM
    if (INSN_SUBJUMP(instr) == 1) {
				CPU->PC = CPU->PC + 1 + IMM11;
		} else {			
				CPU->PC = CPU->R[CPU->rsMux_CTL];
		}
}

/*
 * Parses rest of JSR operation and prints out.
 */
void JSROp(MachineState* CPU, FILE* output)
{
    CPU->regFile_WE = 1;
		int16_t IMM11 = INSN_IMM11(instr);
		CPU->rsMux_CTL = INSN_RS(instr);
		//IF JSR to IMM
    if (INSN_SUBJUMP(instr) == 1) {
				CPU->regInputVal = CPU->PC + 1;
				CPU->R[7] = CPU->regInputVal;
				SetNZP(CPU, (CPU->regInputVal));
				CPU->rdMux_CTL = 7;
				CPU->PC = ((CPU->PC & 0x8000) | (IMM11 << 4));
		} else {
				
				CPU->regInputVal = CPU->PC + 1;
				CPU->PC = CPU->R[CPU->rsMux_CTL];
				CPU->R[7] = CPU->regInputVal;		
				SetNZP(CPU, (CPU->regInputVal));
				CPU->rdMux_CTL = 7;		

		}
}

/*
 * Parses rest of shift/mod operations and prints out.
 */
void ShiftModOp(MachineState* CPU, FILE* output)
{
    CPU->regFile_WE = 1;
    unsigned short int shiftOP = INSN_SHIFT(instr);
		unsigned short int shiftIMM = INSN_IMM4(instr);
		CPU->rdMux_CTL = INSN_RD(instr);
		CPU->rsMux_CTL = INSN_RS(instr);
		CPU->rtMux_CTL = INSN_RT(instr);

		switch (shiftOP){
        case 0b00:
				//shift <<
						CPU->regInputVal = CPU->R[CPU->rsMux_CTL] << shiftIMM;
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));
            break;
        case 0b01:
				//arithmetic shift >>
						if (CPU->R[CPU->rsMux_CTL] & 0x8000) {
							// If the most significant bit is 1 (negative value)
							CPU->regInputVal = (CPU->R[CPU->rsMux_CTL] >> shiftIMM) | (0xFFFF << (16 - shiftIMM));
						} else {
							// If the most significant bit is 0 (non-negative value)
							CPU->regInputVal = CPU->R[CPU->rsMux_CTL] >> shiftIMM;
						}
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));
						break;
        case 0b10:
				//shift >>
						CPU->regInputVal = CPU->R[CPU->rsMux_CTL] >> shiftIMM;
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));				
        case 0b11:
				//MOD
						CPU->regInputVal = CPU->R[CPU->rsMux_CTL] % CPU->R[CPU->rtMux_CTL];
						CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
						SetNZP(CPU, (CPU->regInputVal));
            break;
			 }
}

/*
 * Set the NZP bits in the PSR.
 */
void SetNZP(MachineState* CPU, short result)
{
	CPU->NZP_WE = 1;
	CPU->NZPVal = (result < 0) ? 0b100 : ((result == 0) ? 0b010 : 0b001);
  CPU->PSR = (CPU->PSR & 0x8000) | CPU->NZPVal;
	//printf("nzp is: %d\n", CPU->PSR & 0x0007);
}
