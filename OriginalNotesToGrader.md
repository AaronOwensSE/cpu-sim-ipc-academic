# Original Notes to Grader

## Instructions

Place all program files into the same working directory on a Linux machine.

Utilize the Makefile command "make" to compile a binary named "system".

Run the binary "system" with two required arguments in the following order:

1. String containing a correct path to the desired input file.
2. Integer representing the duration of the interrupt timer.

Program will reject invalid or missing arguments and terminate.

Object files and binary may be cleared using command "make clean".

## File Descriptions

**Makefile** - Makefile for compiling and linking the program.

**SystemExceptions.h** - Brief exception definition for use by the program.

**CPU.h** - Declarations for the CPU class instantiated by the CPU process.

**CPU.cpp** - Definitions for the CPU class instantiated by the CPU process.

**Memory.h** - Declarations for the Memory class instantiated by the memory
process.

**Memory.cpp** - Definitions for the Memory class instantiated by the memory
process.

**System.cpp** - Program file containing main method.

**sample5.txt** - Sample input file containing student-written program.

---

[Back to README](./README.md)
