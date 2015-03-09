RSHELL
===============

Program Overview
-------------------
This project is a simple command shell that reads in user-input commands on one line and work similarly to the bash shell.
It is a project for **CS100: Software Construction** at [UC Riverside](http://ucr.edu), taught by Mike Izbicki.

Installation Guide / How to Use
-------------------------------
Clone this project at: [https://github.com/shawnjzlee/rshell.git](https://github.com/shawnjzlee/rshell.git).

Once cloned, enter a shell and type `cd rshell` to enter the project directory.

1. Type `make` to compile `rshell.cpp` and `ls.cpp` and place the executables in the `./bin` folder, which will be created by Makefile.
    1. Type `make rshell` to compile and place `rshell` in the `./bin` folder
    2. Type `make ls` to compile and place `ls` in the `./bin` folder

2. Running the executable using your existing shell will prompt you with `username@hostname$ ` and wait for an input

3. Press enter after typing in a command, its flags, etc.
    - Typing `exit` will close the executable.


- Type `debug-rshell` to enable the `-g` flag for debugging `rshell`
- Type `debug-ls` to enable the `-g` flag for debugging `ls`

Bugs/Known Issues/Limitations
-----------------------------
###rshell
* `;` connector does not work as intended. Even though multiple commands linked together should execute, only the first command before the `;` executes.
	> **[FIX 0.01 (02/02/15)]**  All connectors are working as intended
* `cd` produces an error because it is not built into the bash command
* When scripting on Ubuntu 14.04, rshell will not be able to get username. It will instead throw an error and use default `guest` as its username.
* Multiple `&&` and `||` connectors does not work as intended because connectors and their logic (as well as precedence) are not implemented correctly; however, single uses of these connectors work as intended.
	> **[FIX 0.01 (02/02/15)]** See FIX 0.01
* `exit` does not work on a line with multiple commands. `exit` alone will work; however, `ls; exit` will not.
	> **[FIX 0.01 (02/02/15)]** See FIX 0.01
* rshell does not work with infinitely large commands
* `mkdir test` creates a folder as intended; however, `mkdir test#folder` will treat `folder` as a comment and create a folder named `test`.
* `touch test` creates a file as intended; however, `touch test#file` will treat `file` as a comment and create a file named `test`.

###ls
* `-R` is incomplete. It acts like `-a` and can be combined with other flags for similar results (except total number of blocks allocated).
* Unless in a directory with all files of hard links `1`, total number of allocated blocks displayed is incorrect.
* `ls` will not allow directories to be passed in to output its files; however, if files are passed in, it will output those files if they exist.
* `ls -a` outputs up to 64 characters per line, for better readability in console.
* `ls` cannot access directories before and after it because it cannot recognize the location.

###io redirection
* `>`, `>>` and `<` work as long as there are spaces on the between arguments.
* `|` outputs what is passed in through redirection, but does not work with `grep` and similar commands

###signals and changing directory
* calling a shell within a shell, as well as `g++`, does not work
* `cd`,`ls`, `cat`, `pwd`, `echo` work within rshell