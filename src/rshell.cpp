#include <errno.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

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

int main(){
	//initialize defaults (size, char arrays, strings); does not reset
	const unsigned size = 128, max_size = 1024;
	char semicolon[] = ";", space[] = " ", operators[] = ";&|", pound[] = "#";
	string _exit = "exit";

	//get login and host name for prompt '$'
    char * username = getlogin();
    if(username == NULL){
		perror("getlogin(): no information received");
		char guest[] = "guest";
		username = guest;
	}
    char * hostname = new char[max_size];
    int checkHost = gethostname(hostname, max_size);
    if(checkHost == -1) perror("gethostname(): no information received");

	//input string and tokenization
    while(true){
		cout << username << "@" << hostname << "$ ";
		string cmdStr;

		//initialize defaults (resets within the scope of the while loop)
		int i = 0, j = 0, k = 0, argc = -1;
		bool falseFlag = false;

		getline(cin, cmdStr);
		int cmd_size = cmdStr.size();

		//if comparision with "exit" is true, exit with status 1
		if(cmdStr.compare(0, 4, _exit) == 0) exit(1);

		char * cmdChar = new char[cmd_size+1];
		char * commands[size];
		int operatorCheck[size] = { }, _operatorCheck = 0;

		//convert string to c-string
		while((i < cmd_size) && (cmdStr[i] != pound[0])){
			cmdChar[i] = cmdStr[i];
			
			if(cmdStr[i] == operators[0])
				operatorCheck[0]+=1;
			if((cmdStr[i] == operators[1]) && (cmdStr[i+1] == operators[1]))
				operatorCheck[1]+=1;
			if(cmdStr[i] == operators[2] && cmdStr[i+1] == operators[2])
				operatorCheck[2]+=1;
				
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
				goto outerloop;
			}

			i++;
		}
		cmdChar[i] = '\0';
		outerloop:
		if(falseFlag == false){
			//removes semicolons - TODO separate commands by semicolons
			char * token = new char[size];
			token = strtok(cmdChar, semicolon);
			commands[++argc] = token;
			tokenization(token, commands, argc, semicolon);
			commands[++argc] = '\0';
	
			//Initialize variables
			char * _commands[size], * _commands_[size], * _token = new char[size], * _token_;
			int numOfOps = 0, falseCount = 0;
	
			//removes "&&" and "||" operators
			_token = strtok(commands[j], operators);
			
			//checks for boolean values within comparison operators
			if(operatorCheck[1] != 0)
				falseFlag = comparatorCheck(1, falseCount, _token, operatorCheck);
			if(operatorCheck[2] != 0)
				falseFlag = comparatorCheck(2, falseCount, _token, operatorCheck);
	
			if(falseFlag == false){	
				_commands[0] = _token;
				tokenization(_token, _commands, numOfOps, operators);
				for(k = 0; k < numOfOps; ++k){
					argc = -1;
					_token_ = strtok(_commands[k], space);
					_commands_[++argc] = _token_;
					tokenization(_token_, _commands_, argc, space);
					_commands_[++argc] = '\0';
					
					//PID, fork(), wait(), execvp()
					int PID = fork();
					if(PID == -1) perror("fork() failed");
					else if(PID == 0){
						if(execvp(_commands_[0], _commands_) == -1) 
							perror("execvp(*file, *const argv[]): command not found");
						exit(1);
					}
					else{
						if(wait(0) == -1) perror("wait()");
					}
				}
			}
		}
	}
	return 0;
}

