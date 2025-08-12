#pragma once

/* 
 * class CPU
 *
 * A class simulating an operating system's CPU.
 *
 * Communicates on given file descriptors to fetch and execute instructions.
 * Supports several registers and a robust instruction set for basic data processing, output, and
 * memory operations.
 * Supports stack operations for nonlinear execution.
 * Enters system mode on system call or interrupt from given timer to execute instructions in
 * privileged memory space.
 * Throws exceptions to calling code on error conditions.
 */
class CPU {
	private:
		// File descriptors
		int readFD;
		int writeFD;

		// Timer counter
		int timer;
		int count;

		// CPU registers
		int PC;
		int SP;
		int IR;
		int AC;
		int X;
		int Y;

		// System mode flags
		bool kernel;
		bool timerElapsed;

		// Internal memory operations
		int readFromAddress(int p_address);
		void writeToAddress(int p_address, int p_value);

		// Internal execution operation
		int getArgument();

		// Internal stack operations
		void push(int p_value);
		int pop();

		// Internal interrupt operation
		void interrupt(bool p_manual = false);

	public:
		// Constructor
		CPU(int p_readFD, int p_writeFD, int p_timer);

		// Fetch/execute cycle operations
		void fetch();
		bool execute();
};
