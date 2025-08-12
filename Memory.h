#pragma once

#include <string>

/* 
 * class Memory
 *
 * A class simulating an operating system's memory unit.
 *
 * Loads given input file into memory and communicates on given file descriptors.
 * Handles three types of requests, read, write, and exit, by analyzing and passing incoming data.
 * Throws exceptions to calling code on error conditions.
 */
class Memory {
	private:
		// File descriptors
		int readFD;
		int writeFD;

		// Simulated system memory
		int memory[2000];

		// Internal operations
		int read(int p_address);
		void write(int p_address, int p_value);

	public:
		// Constructor
		Memory(int p_readFD, int p_writeFD, const std::string& p_fileName);
		
		// Request handler operation
		bool handleRequest();
};
