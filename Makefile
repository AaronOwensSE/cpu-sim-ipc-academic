system: System.o CPU.o Memory.o
	g++ -o system System.o CPU.o Memory.o

System.o: System.cpp CPU.h Memory.h SystemExceptions.h
	g++ -c System.cpp

CPU.o: CPU.cpp CPU.h SystemExceptions.h
	g++ -c CPU.cpp

Memory.o: Memory.cpp Memory.h SystemExceptions.h
	g++ -c Memory.cpp

clean:
	rm -f system System.o CPU.o Memory.o
