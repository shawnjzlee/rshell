#include <errno.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <iterator>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <vector>
#include <unistd.h>
using namespace std;

#define PID(x) 	if(wait(0) == -1) perror("wait() failed"); \
				if(close(fd) == -1) perror("Failed to close file"); \
				prev = i; command[curr] = x;
				
#define CHECK(x) 	bool check = true; fd = open(command[i+1], O_WRONLY | O_CREAT | x); \
					if(fd < 0){ perror("Open file failed"); check = false; } \
					pd = chmod(command[i+1], S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); \
					if(pd < 0){ perror("Failed to edit permissions"); check = false; } \
					return check;

char semicolon[] = ";", space[] = " ", operators[] = ";&|", pound[] = "#",
	 rightarrow[] = ">", leftarrow[] = "<";


void tokenization(char * &token, char * command[], int &iterator, char operators[]){
	while(token != NULL){
		token = strtok(NULL, operators);
		command[++iterator] = token;
	}
}

//checks true or false boolean value in the first token
bool comparatorCheck(const int type, int &falseCount, char * token, int operatorCheck[]){
	int size = 4;
	char f[6] = "false", _bool[6] = "true";
	
	if(type == 1){
		size++;
		copy(f, f+5, _bool);
	}

	for(int i = 0; i < size+1; i++){
		if(token[i] == _bool[i]) falseCount++;
	}
	if(falseCount >= size) return true;
	return false;
}

bool checkFileTrunc(int i, char * command[], int &fd, int &pd){
	CHECK(O_TRUNC);
}

bool checkFileApp(int i, char * command[], int &fd, int &pd){
	CHECK(O_APPEND);
}

void inputRedirection(int i, char * command[], int &curr, int &prev, int &fd){
	fd = open(command[i+1], O_RDONLY);
	if(fd < 0) perror("Open file failed");
	else{
		curr = i;
		command[i] = '\0';    
		int PID;
		if((PID = fork()) < 0) perror("fork() failed");
		else if(PID == 0){
			if(dup2(fd, 0) == -1) perror("dup2() failed");
			if(execvp(command[prev], command) == -1)
			perror("execvp(*file, *const _commands_[]): command not found");
		}
		else{
			PID(leftarrow);
		}
	}
}

void outRedirection(int i, char * command[], int &curr, int &prev, int &fd, int &pd){
	if(checkFileTrunc(i, command, fd, pd)){
		curr = i;
		command[i] = '\0';
		int PID;
		if( (PID = fork()) == -1) perror("fork() failed");
		else if(PID == 0){
		if(dup2(fd, 1) == -1) perror("dup2() failed");
		if(execvp(command[prev], command) == -1)
		perror("execvp(*file, *const _commands_[]): command not found");
		exit(1);
		}
		else{
			PID(rightarrow);
		}
	}
	else cerr << "Output redirection failed";
}

void piping(int j, char * command[]){
	int pipeIndex = 0, _pipeIndex = 0, i = 0;
	bool pipeCheck = false;
	char * pipe_[BUFSIZ], * _pipe[BUFSIZ];
	
	for(; command[j] != '\0'; ++i){
		if(strcmp(command[j], "|") == 0){
			pipeCheck = true;
			pipeIndex = i;
			break;
		}
	}

	if(pipeCheck){
		for(i = 0; i < pipeIndex; ++i) pipe_[i] = command[j];
		pipe_[i] = NULL;
		
		for(i = pipeIndex + 1; command[j] != '\0'; i++){
			_pipe[_pipeIndex] = command[j];
			++pipeIndex;
		}
	}
	else return;
	
	_pipe[pipeIndex] = NULL;

	int fd[2];
	if(pipe(fd) == -1) perror("pipe() failed");
	int PID;
	if((PID = fork()) == -1) perror("fork() failed");
	
    else if(PID == 0){
        if(-1 == dup2(fd[1],1)) perror("dup2() failed");

        if(-1 == close(fd[0])) perror("Failed to close file");

        if(-1 == execvp(pipe_[0], pipe_)) perror("execvp(*file, *const _commands_[]): command not found");

        exit(1);
    }
    
    int in = 0;
    if(-1 == (in = dup(0))) perror("dup() failed");
    if(-1 == dup2(fd[0], 0)) perror("dup2() failed");
    if(-1 == close(fd[1])) perror("Close file failed");
    if(-1 == wait(0)) perror("wait() failed");

    piping(j, _pipe);
}

int main(){
	//initialize defaults (size, char arrays, strings); does not reset
	const unsigned size = BUFSIZ, max_size = 1024;
//	char semicolon[] = ";", space[] = " ", operators[] = ";&|", pound[] = "#",
//		 leftarrow[] = ">", rightarrow[] = "<";
	string _exit = "exit";

	//get login and host name for prompt '$'
    char * username = getlogin();
    if(username == NULL){
		perror("getlogin(): no information received");
		char guest[] = "guest";
		username = guest;
	}
    char hostname[size]; //= new char[max_size];
    int checkHost = gethostname(hostname, max_size);
    if(checkHost == -1) perror("gethostname(): no information received");

	//input string and tokenization
    while(true){
		cout << username << "@" << hostname << "$ ";
		string cmdStr;

		//initialize defaults (resets within the scope of the while loop)
		int i = 0, j = 0, k = 0, l = 0, argc = -1, zero_op = 0, one_op = 0, two_op = 0;
		bool falseFlag = false, pipeFlag = false;

		getline(cin, cmdStr);
		int cmd_size = cmdStr.size();

		//if comparision with "exit" is true, exit with status 1
		if(cmdStr.compare(0, 4, _exit) == 0) exit(1);

		char * cmdChar = new char[cmd_size+1];
		char * commands[BUFSIZ];
		vector<char> cmdSym;
		int operatorCheck[size] = { }, _operatorCheck = 0;

		//convert string to c-string
		while((i < cmd_size) && (cmdStr[i] != pound[0])){
			cmdChar[i] = cmdStr[i];
			
			if(cmdStr[i] == operators[0]){
				operatorCheck[0]+=1;
				zero_op++;
				cmdSym.push_back(operators[0]);
			}
			if((cmdStr[i] == operators[1]) && (cmdStr[i+1] == operators[1])){
				operatorCheck[1]+=1;
				one_op++;
				cmdSym.push_back(operators[1]);
			}
			if(cmdStr[i] == operators[2] && cmdStr[i+1] == operators[2]){
				operatorCheck[2]+=1;
				two_op++;
				cmdSym.push_back(operators[2]);
			}
				
			if(cmdStr[i] == operators[0]){
				if(cmdStr[i+1] == operators[0])
					if(cmdStr[i+2] == operators[0])
						_operatorCheck+=1;
			}
			else if(cmdStr[i] == operators[1]){
				if(cmdStr[i+1] == operators[1])
					if(cmdStr[i+2] == operators[1])
						_operatorCheck+=1;
			}
			else if(cmdStr[i] == operators[2]){
				if(cmdStr[i+1] == operators[2])
					if(cmdStr[i+2] == operators[2])
						_operatorCheck+=1;
			}
			
			if(_operatorCheck >= 1){
				perror("error: syntax error near unexpected token");
				falseFlag = true;
				break;
			}
			i++;
		}
		cmdChar[i] = '\0';
		if(falseFlag == false){
			
			char * token;
			token = strtok(cmdChar, semicolon);
			commands[++argc] = token;
			tokenization(token, commands, argc, semicolon);
			commands[++argc] = '\0';
			
			zero_op = argc;
			for(int i = 0; i < zero_op - 1; i++){
				//Initialize variables
				char * _commands[BUFSIZ], * _commands_[BUFSIZ], * _token, * _token_;
				int numOfOps = 0, falseCount = 0, curr = 0, prev = 0, fd = 0, pd = 0;
		
				//removes "&&" and "||" operators
				_token = strtok(commands[j], operators);
				
				//checks for boolean values within comparison operators
				if(operatorCheck[1] != 0)
					falseFlag = comparatorCheck(1, falseCount, _token, operatorCheck);
				if(operatorCheck[2] != 0)
					falseFlag = comparatorCheck(2, falseCount, _token, operatorCheck);
		

				_commands[0] = _token;
				tokenization(_token, _commands, numOfOps, operators);
				for(k = 0; k < numOfOps; ++k){
					argc = -1;
					_token_ = strtok(_commands[k], space);
					_commands_[++argc] = _token_;
					tokenization(_token_, _commands_, argc, space);
					_commands_[++argc] = '\0';
					
					//IO redirection and piping
					for(l = 0; _commands_[l] != '\0'; ++l){
						if(strcmp(_commands_[l], "<") == 0){
							pipeFlag = true;
							inputRedirection(l, _commands_, curr, prev, fd);
						}

						else if(strcmp(_commands_[l], ">") == 0){
							pipeFlag = true;
							outRedirection(l, _commands_, curr, prev, fd, pd);
						}
						
						else if(strcmp(_commands_[l], ">>") == 0){
							pipeFlag = true;
							if(checkFileApp(l, _commands_, fd, pd)){
								curr = l;
								_commands_[l] = '\0';
								int PID;
								if( (PID = fork()) == -1) perror("fork() failed");
								else if(PID == 0){
									if(dup2(fd, 1) == -1) perror("dup2() failed");
									if(execvp(_commands_[prev], _commands_) == -1)
									perror("execvp(*file, *const _commands_[]): command not found");
									exit(0);
								}
								else{
									if(wait(0) == -1) perror("wait() failed");
									if(close(fd) == -1) perror("Failed to close file");
									_commands_[curr] = rightarrow;
									prev = l;    
								}
							}
							else break;
						}
						
						else if(strcmp(_commands_[l], "|") == 0){
							piping(l, _commands_);
						}
					}
					
					if(pipeFlag);	
					else{
						//PID, fork(), wait(), execvp()
						int PID = fork(), _status = 0;
						if(PID == -1) perror("fork() failed");
						else if(PID == 0){
							if(execvp(_commands_[0], _commands_) == -1) 
								perror("execvp(*file, *const _commands_[]): command not found");
							exit(1);
						}
						else{
							int _waitpid = wait(&_status);
							if(-1 == _waitpid) perror("wait() failed");
						}
						
						//connector '&|' logic using vector of symbols and wait status pointer
						if(cmdSym.size() == 0) break;
						cout << cmdSym.at(0);
				        if(cmdSym.at(0) == operators[1]){
				            if(_status != 0) break;
				        }
				        else if (cmdSym.at(0) == operators[2]){
				            if(_status == 0) break;
				        }
				        cmdSym.erase(cmdSym.begin());
					}
				}
			}
		}
		delete [] cmdChar;
	}
	return 0;
}
