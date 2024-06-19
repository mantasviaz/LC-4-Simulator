# LC4-Simulator


## Overview of the system
An assembly-level simulator for a 16-bit LC-4 processor implementing a full instruction set with an OS, assembler, parser, disassembler, and a simple game in C utilizing the simulator. While similar to an LC-3, and LC-4 is a more advanced version with more functionality. The functionality of this simulator includes loading and processing machine code files (binary files produced by the LC4 assembler) and executing them just by keeping track of internal state (PC, PSR, registers, control signals, etc.). As an output, trace textfiles are generated that contain information from each LC4 “cycle” as the program executes the loaded machine code. 

## Instructions for Running Project
1. **Run the Makefile**:
   - Run with `make all`.
2. **Run the Trace**:
   - Invoke from the command line: `./trace output_filename.txt first.obj second.obj third.obj`.


