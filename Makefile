all:	program1

program1:	program1.o
	g++ -std=c++17 -Wall -Wextra program1.o -o program1

program1.o: program1.cpp
	g++ -std=c++17 -Wall -Wextra -c program1.cpp -o program1.o

clean:
	rm -f *.o program1