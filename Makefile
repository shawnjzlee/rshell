#variables
COMPILE = g++
FLAGS = -Wall -Werror -ansi -pedantic
DEBUG = $(FLAGS) -g

#targets
all: clean
	mkdir ./bin
	$(COMPILE) $(FLAGS) ./src/rshell.cpp -o ./bin/rshell

clean:
	rm -rf ./bin

debug: clean
	mkdir ./bin
	$(COMPILE) $(DEBUG) ./src/rshell.cpp -o ./bin/rshell
	gdb ./bin/rshell
	gdb run /bin/rshell
