#rshell

## Program Overview
This project is a simple command shell that reads in user-input commands on one line and work similarly to the bash shell.
It is a project for **CS100: Software Construction** at [UC Riverside](http://ucr.edu), taught by Mike Izbicki.

## Installation Guide / How to Use
Clone this project at: [https://github.com/slee208/rshell.git](https://github.com/slee208/rshell.git).

Once cloned, enter a shell and type `cd rshell` to enter the project directory.

1. Type `make` to compile `rshell.cpp` and place the executable in the `./bin` folder, which will be created by Makefile.

2. Running the executable using your existing shell will prompt you with `username@hostname$ ` and wait for an input

3. Press enter after typing in a command, its flags, etc.
    - Typing `exit` will close the executable.

## Bug/Known Issues/Limitations
* `;` connector does not work as intended. Even though multiple commands linked together should execute, only the first command before the `;` executes.
* `cd` produces an error because it is not built into the bash command
* When scripting on Ubuntu 14.04, rshell will not be able to get username. It will instead throw an error and use default `guest` as its username.
* Multiple `&&` and `||` connectors does not work as intended because connectors and their logic (as well as precedence) are not implemented correctly; however, single uses of these connectors work as intended.
* `exit` does not work on a line with multiple commands. `exit` alone will work; however, `ls; exit` will not.
* rshell does not work with infinitely large commands
* `mkdir test` creates a folder as intended; however, `mkdir test#folder` will treat `folder` as a comment and create a folder named `test`.
* `touch test` creates a file as intended; however, `touch test#file` will treat `file` as a comment and create a file named `test`.