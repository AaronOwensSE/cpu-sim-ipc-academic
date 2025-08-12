#include "Memory.h"
#include "SystemExceptions.h"
#include <unistd.h>
#include <cstdlib>
#include <fstream>

using namespace std;

// Throws FILE_ERROR
Memory::Memory(int p_readFD, int p_writeFD, const string& p_fileName) {
	// Set file descriptors.
	readFD = p_readFD;
	writeFD = p_writeFD;

	// Attempt to open input file.
	ifstream file;
	file.open(p_fileName.c_str(), ios::in);

	if (!file.is_open()) {
		throw FILE_ERROR;
	}

	// Read input file
	string line;
	
	int i = 0;

	// While file has unread contents, read next line.
	while (!file.eof()) {
		getline(file, line);

		// Skip empty lines.
		if (!line.empty()) {
			// Handle . directives by setting correct memory index.
			if (line[0] == '.') {
				line.erase(0, 1);

				i = atoi(line.c_str());
			// Write integer instructions to memory, and ignore all other text.
			} else if (line[0] >= 48 && line[0] <= 57) {
				memory[i++] = atoi(line.c_str());
			}
		}
	}

	file.close();
}

// Throws PIPE_READ_ERROR, PIPE_WRITE_ERROR
// Return true on successful request completion, false on exit.
bool Memory::handleRequest() {
	int buffer[2];

	// Determine type of request by analyzing request size.
	int requestSize = ::read(readFD, &buffer, sizeof(buffer));

	// One integer is a read request containing an address.
	if (requestSize == sizeof(int)) {
		// If unaddressable value -1 is passed, interpret as exit request.
		if (buffer[0] == -1) {
			return false;
		// Otherwise, attempt to read from address and transmit value.
		} else {
			buffer[0] = read(buffer[0]);

			if (::write(writeFD, &buffer, sizeof(int)) == -1) {
				throw PIPE_WRITE_ERROR;
			} else {
				return true;
			}
		}
	// Two integers is a write request containing an address and a value. Write to memory.
	} else if (requestSize == sizeof(buffer)) {
		write(buffer[0], buffer[1]);

		return true;
	} else if (requestSize == -1) {
		throw PIPE_READ_ERROR;
	} else {
		return false;
	}
}

int Memory::read(int p_address) {
	return memory[p_address];
}

void Memory::write(int p_address, int p_value) {
	memory[p_address] = p_value;
}
