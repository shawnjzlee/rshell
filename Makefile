#variables
COMPILE = g++
FLAGS = --std=c++11
DEBUG = $(FLAGS) -g

#targets
all: clean
	mkdir ./bin
	$(COMPILE) $(FLAGS) ./src/rshell.cpp -o ./bin/rshell
	$(COMPILE) $(FLAGS) ./src/ls.cpp -o ./bin/ls

rshell: clean
	mkdir ./bin
	$(COMPILE) $(FLAGS) ./src/rshell.cpp -o ./bin/rshell
	
ls: clean
	mkdir ./bin
	$(COMPILE) $(FLAGS) ./src/ls.cpp -o ./bin/ls
	
clean:
	rm -rf ./bin

debug-rshell: clean
	mkdir ./bin
	$(COMPILE) $(DEBUG) ./src/rshell.cpp -o ./bin/rshell
	gdb ./bin/rshell

debug-ls: clean
	mkdir ./bin
	$(COMPILE) $(DEBUG) ./src/ls.cpp -o ./bin/ls
	gdb ./bin/ls
