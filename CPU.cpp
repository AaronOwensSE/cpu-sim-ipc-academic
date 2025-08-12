#include "CPU.h"
#include "SystemExceptions.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>

using namespace std;

// Initializes with program counter at 0 and kernel mode disabled.
// Sets given file descriptors and timer.
CPU::CPU(int p_readFD, int p_writeFD, int p_timer) {
	readFD = p_readFD;
	writeFD = p_writeFD;

	timer = count = p_timer;

	PC = 0;
	SP = 1000;

	kernel = false;
	timerElapsed = false;
}

// Throws ACCESS_VIOLATION_ERROR, PIPE_READ_ERROR, PIPE_WRITE_ERROR
// Retrieves next instruction.
void CPU::fetch() {
	// If timer has expired, reset and flag for timer interrupt.
	if (count <= 0) {
		timerElapsed = true;
		count = timer;
	}

	// If interrupt flag has not yet been handled, call interrupt.
	// Distinct from count <= 0 above, since flag can be set during interrupt or system call to be
	// handled after. Infinite timer interrupt loop occurs if interrupt instruction count <= timer.
	// Per professor's instructions, this is expected.
	if (timerElapsed) {
		interrupt();
	}

	// Fetch instruction at PC to IR.
	IR = readFromAddress(PC++);
}

// Throws ACCESS_VIOLATION_ERROR, PIPE_READ_ERROR, PIPE_WRITE_ERROR
// Evaluates current instruction, executes, and updates timer.
bool CPU::execute() {
	bool cont = true;

	int operand;

	// Each IR value represents a different instruction.
	switch (IR) {
		case 1:	// Load value
			AC = getArgument();

			break;
		case 2:	// Load addr
			AC = readFromAddress(getArgument());

			break;
		case 3:	// LoadInd addr
			AC = readFromAddress(readFromAddress(getArgument()));

			break;
		case 4:	// LoadIdxX addr
			AC = readFromAddress(getArgument() + X);

			break;
		case 5:	// LoadIdxY addr
			AC = readFromAddress(getArgument() + Y);

			break;
		case 6:	// LoadSpX
			AC = readFromAddress(SP + X);

			break;
		case 7:	// Store addr
			writeToAddress(getArgument(), AC);

			break;
		case 8:	// Get
			AC = rand() % 100 + 1;

			break;
		case 9:	// Put port
			operand = getArgument();

			if (operand == 1) {
				cout << AC;	// Output int
			} else if (operand == 2) {
				cout << static_cast<char>(AC);	// Output char
			} else {
				cout << "Invalid operand." << endl;
			}

			break;
		case 10:	// AddX
			AC += X;

			break;
		case 11:	// AddY
			AC += Y;

			break;
		case 12:	// SubX
			AC -= X;

			break;
		case 13:	// SubY
			AC -= Y;

			break;
		case 14:	// CopyToX
			X = AC;

			break;
		case 15:	// CopyFromX
			AC = X;

			break;
		case 16:	// CopyToY
			Y = AC;

			break;
		case 17:	//CopyFromY
			AC = Y;

			break;
		case 18:	// CopyToSp
			SP = AC;

			break;
		case 19:	// CopyFromSp
			AC = SP;

			break;
		case 20:	// Jump addr
			PC = getArgument();

			break;
		case 21:	// JumpIfEqual addr
			if (AC == 0) {
				PC = getArgument();
			} else {
				PC++;
			}

			break;
		case 22:	// JumpIfNotEqual addr
			if (AC != 0) {
				PC = getArgument();
			} else {
				PC++;
			}

			break;
		case 23:	// Call addr
			push(PC + 1);	// PC currently on call argument. +1 will return to instruction after
							// call+arg

			PC = getArgument();

			break;
		case 24:	// Ret
			PC = pop();

			break;
		case 25:	// IncX
			X++;

			break;
		case 26:	// DecX
			X--;

			break;
		case 27:	// Push
			push(AC);

			break;
		case 28:	// Pop
			AC = pop();

			break;
		case 29:	// Int
			interrupt(true);	// Call interrupt with manual flag.

			break;
		case 30:	// IRet
			PC = pop();
			SP = pop();

			kernel = false;	// Leave system mode.

			break;
		case 50:	// End
			// Send a request to read from nonexistent address -1 as a signal to memory to
			// terminate.
			AC = -1;

			if (write(writeFD, &AC, sizeof(int)) == -1) {
				throw PIPE_WRITE_ERROR;
			}

			cont = false;

			break;
	}

	// Update timer.
	count--;

	// Return false after end instruction or true.
	return cont;
}

// Throws ACCESS_VIOLATION_ERROR, PIPE_READ_ERROR, PIPE_WRITE_ERROR
// Requests to read given address from file descriptor representing memory unit and returns
// response.
int CPU::readFromAddress(int p_address) {
	// Disallow access to addresses >= 1000 if not in kernel mode.
	if (p_address >= 1000 && !kernel) {
		cout << "Memory violation: accessing system address " << p_address << " in user mode"
			<< endl;

		throw ACCESS_VIOLATION_ERROR;
	}

	int value;

	// Request read of given address.
	if (write(writeFD, &p_address, sizeof(int)) == -1) {
		throw PIPE_WRITE_ERROR;
	}

	// Receive and return resulting value.
	if (read(readFD, &value, sizeof(int)) == -1) {
		throw PIPE_READ_ERROR;
	}

	return value;
}

// Throws ACCESS_VIOLATION_ERROR, PIPE_WRITE_ERROR
// Requests to write given value to given address.
void CPU::writeToAddress(int p_address, int p_value) {
	// Disallow access to addresses >= 1000 if not in kernel mode.
	if (p_address >= 1000 && !kernel) {
		cout << "Memory violation: accessing system address " << p_address << " in user mode"
			<< endl;

		throw ACCESS_VIOLATION_ERROR;
	// Request write of given value to given address.
	} else {
		int buffer[2] = { p_address, p_value };

		if (write(writeFD, &buffer, sizeof(buffer)) == -1) {
			throw PIPE_WRITE_ERROR;
		}
	}
}

// Throws ACCESS_VIOLATION_ERROR, PIPE_READ_ERROR, PIPE_WRITE_ERROR
// Retrieves argument from next address and advances program counter.
// For use in instructions followed by arguments.
int CPU::getArgument() {
	return readFromAddress(PC++);
}

// Throws ACCESS_VIOLATION_ERROR, PIPE_WRITE_ERROR
// Pushes given value to the stack and updates stack pointer.
void CPU::push(int p_value) {
	writeToAddress(--SP, p_value);
}

// Throws ACCESS_VIOLATION_ERROR, PIPE_READ_ERROR, PIPE_WRITE_ERROR
// Returns popped value from the stack and updates stack pointer.
int CPU::pop() {
	return readFromAddress(SP++);
}

// Throws ACCESS_VIOLATION_ERROR, PIPE_WRITE_ERROR
// Processes timer interrupts or system calls, depending on given flag.
void CPU::interrupt(bool p_manual) {
	// Disallow nested interrupts.
	if (!kernel) {
		// Enter system mode and save stack pointer and program counter to system stack.
		kernel = true;

		int temp = SP;

		SP = 2000;

		push(temp);
		push(PC);

		// Set program counter to interrupt handler, depending on given flag.
		if (p_manual) {
			PC = 1500;
		} else {
			// On timer interrupt, clear timerElapsed flag.
			timerElapsed = false;

			PC = 1000;
		}
	}
}
