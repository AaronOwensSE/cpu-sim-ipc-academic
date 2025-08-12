# cpu-sim-ipc-academic

An academic project that utilizes interprocess communication to simulate the low-level functionality of a computer system, including a CPU with a robust instruction set and a memory module. Capable of executing real, user-created programs.

This project was part of an operating systems class completed in the fall of 2023. Written in C++ for a Linux environment.

## Design

### An Important Note on Credit

The *what* of the system being simulated by this project was provided as part of the assignment specification. That is, descriptions of the CPU and its registers, the layout of memory, the instruction set, and the format of a valid input program were given. Students were tasked with the *how* of implementation.

### CPU

#### Registers

- **AC** - Main register. Source or destination for many operations.
- **X** - Auxiliary register with instructions supporting its use as a counter.
- **Y** - Auxiliary register.
- **PC** - Program counter. Points to the next instruction to be fetched.
- **IR** - Instruction register. Receives the next instruction to be executed.
- **SP** - Stack pointer. Points to the top of the stack.

#### Instruction Set

Programs are read into the memory module from an input file, where instructions are represented as numeric codes.

|Code|Instruction|Argument|Description|
|---|---|---|---|
|1|Load|value|Loads value into AC.|
|2|Load|addr|Loads the value pointed to by addr into AC.|
|3|LoadInd|addr|Loads the value at the address pointed to by the value at addr into AC.|
|4|LoadIdxX|addr|Loads the value at (addr + X) into AC.|
|5|LoadIdxY|addr|Loads the value at (addr + Y) into AC.|
|6|LoadSpX||Loads the value at (SP + X) into AC.|
|7|Store|addr|Stores the value in AC at addr.|
|8|Get||Gets a random integer from 1 to 100 and loads it into AC.|
|9|Put|port|Writes AC to the screen as an integer (port = 1) or character (port = 2).|
|10|AddX||Adds the value in X to AC.|
|11|AddY||Adds the value in Y to AC.|
|12|SubX||Subtracts the value in X from AC.|
|13|SubY||Subtracts the value in Y from AC.|
|14|CopyToX||Copies the value in AC to X.|
|15|CopyFromX||Copies the value in X to AC.|
|16|CopyToY||Copies the value in AC to Y.|
|17|CopyFromY||Copies the value in Y to AC.|
|18|CopyToSp||Copies the value in AC to SP.|
|19|CopyFromSp||Copies the value in SP to AC.|
|20|Jump|addr|Jumps to addr.|
|21|JumpIfEqual|addr|Jumps to addr if AC = 0.|
|22|JumpIfNotEqual|addr|Jumps to addr if AC != 0.|
|23|Call|addr|Pushes a return address onto the stack and jumps to addr.|
|24|Ret||Pops a return address from the stack and jumps to it.|
|25|IncX||Increments the value in X.|
|26|DecX||Decrements the value in X.|
|27|Push||Pushes the value in AC onto the stack.|
|28|Pop||Pops a value from the stack and loads it into AC.|
|29|Int||Performs a system call.|
|30|IRet||Returns from a system call.|
|50|End||Ends the program.|

### Memory

- Memory is represented by a block of 2000 integers.
- Addresses 0-999 are for use by normal programs.
- The program counter begins at address 0.
- The program stack grows down from address 999.
- Addresses 1000-1999 are for use by the system.
- The system stack grows down from address 1999.

### Interrupts

A 29 (Int) instruction manually interrupts program execution to process a system call, switching to kernel mode and executing code at memory address 1000.

Additionally, the system will process an automatic interrupt every time a certain number of instructions have been executed. Automatic interrupts are processed by switching to kernel mode and executing code at memory address 1500. The number of instructions to elapse between each automatic interrupt is supplied as a command-line argument.

It is a *known issue* that the number of instructions between automatic interrupts must be greater than the number of instructions comprising the automatic interrupt subroutine to avoid an infinite loop. Students were directed to disregard this issue and assume a sufficiently large number would be provided by the user.

### Program File Format

A program is loaded from a text file containing a list of memory loading instructions, CPU instructions, whitespace, and comments.

- A memory loading instruction is represented by a line beginning with a period (.), followed by an address. Subsequent CPU instructions are loaded into the memory module beginning at the specified address.
- A CPU instruction is represented by a line beginning with an integer corresponding to an instruction code.
- A CPU instruction argument is taken to be the integer value on the next line after the CPU instruction.
- Empty lines or lines not beginning with integers or memory loading instructions are ignored by the loader.
- Text comments following memory loading instructions or CPU instructions are ignored by the loader.

## Sample Program

The included sample5.txt file contains a student-created program that will output the first 20 numbers of the Fibonacci sequence, interrupting periodically to output the sum of three random integers.

## Compile and Link

A Makefile is provided for a Linux environment with make and g++.

From the project root directory:

```
make
```

To remove the binary and .o files for cleanup or recompilation:

```
make clean
```

## Run

From the project root directory:

```
./system sample5.txt 50
```

Or supply your own program file and interrupt timer.

## [Original Notes to Grader](./OriginalNotesToGrader.md)

## Disclaimer

As an academic exercise, this project's scope is limited to a highly specific set of requirements elicited from assignment documentation and classroom discussions. It is presented as a successful fulfillment of learning objectives and a snapshot of my development over time as a programmer.
