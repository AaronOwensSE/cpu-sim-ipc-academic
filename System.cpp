#include "CPU.h"
#include "Memory.h"
#include "SystemExceptions.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

using namespace std;

void handleInvalidArgs();

// Establishes pipes, forks process, instantiates classes, calls methods to simulate CPU/memory
// interactions, and handles errors and exceptions.
// Input file name and timer are given as command line arguments.
int main(int argc, char** argv) {
	// Discard invalid arguments and terminate.
	if (argc != 3) {
		handleInvalidArgs();
	}

	string fileName = argv[1];
	int timer = atoi(argv[2]);

	if (timer <= 0) {
		handleInvalidArgs();
	}

	// Create pipes.
	int cpuToMem[2];
	int memToCPU[2];

	if (pipe(cpuToMem) == -1) {
		cout << "CPU to memory pipe error." << endl;

		exit(0);
	}

	if (pipe(memToCPU) == -1) {
		cout << "Memory to CPU pipe error." << endl;

		exit(0);
	}

	// Fork process.
	pid_t childPID = fork();

	// Child memory process
	if (childPID == 0) {
		// Close unused pipe ends.
		close(cpuToMem[1]);
		close(memToCPU[0]);
		
		try {
			// Create memory object, and pass pipe file descriptors and file name.
			Memory memory(cpuToMem[0], memToCPU[1], fileName);
		
			// Loop on request handler until exit.
			while(memory.handleRequest());
		} catch (SystemException exception) {
			switch (exception) {
				case FILE_ERROR:
					cout << "File error in memory module." << endl;

					break;
				case PIPE_READ_ERROR:
					cout << "Pipe read error in memory module." << endl;

					break;
				case PIPE_WRITE_ERROR:
					cout << "Pipe write error in memory module." << endl;
					
					break;
			}
		}

		// Close remaining pipe ends.
		close(cpuToMem[0]);
		close(memToCPU[1]);
	// Parent CPU process
	} else if (childPID > 0) {
		// Close unused pipe ends.
		close(cpuToMem[0]);
		close(memToCPU[1]);

		// Create CPU object, and pass pipe file descriptors and timer value.
		CPU cpu(memToCPU[0], cpuToMem[1], timer);
		
		try {
			// Loop on fetch/execute cycle until exit.
			do {
				cpu.fetch();
			} while (cpu.execute());
		// On exception, CPU is unable to signal termination to memory via pipe, so kill memory.
		} catch (SystemException exception) {
			switch (exception) {
				case ACCESS_VIOLATION_ERROR:
					cout << "Access violation error in CPU module." << endl;

					break;
				case PIPE_READ_ERROR:
					cout << "Pipe read error in CPU module." << endl;

					break;
				case PIPE_WRITE_ERROR:
					cout << "Pipe write error in CPU module." << endl;

					break;
			}

			kill(childPID, SIGINT);
		}
		
		// Close remaining pipe ends.
		close(cpuToMem[1]);
		close(memToCPU[0]);

		// Wait for child process to terminate to prevent zombie process.
		waitpid(childPID, 0, 0);
	} else {
		cout << "Fork error in system module." << endl;
	}

	// Both processes: terminate.
	return 0;
}

// Outputs and terminates, for use after invalid arguments detected.
void handleInvalidArgs() {
	cout << "Invalid arguments.\n";
	cout << "arg1: string filepath\n";
	cout << "arg2: int timer (positive)" << endl;

	exit(0);
}
