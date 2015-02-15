// closh.c - CS 377, Spring 2015
/******************************************************************
 * Project : Closh                                                        
 * Author :  Nathan Fuller (UMASS ID: 26506886)
 * Author :  Daniel Laizer (UMASS ID: 27616321)         
*******************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

pid_t child_pid = -1;

// tokenize the command string into arguments - do not modify
void readCmdTokens(char* cmd, char** cmdTokens) {
	cmd[strlen(cmd) - 1] = '\0'; // drop trailing newline
	int i = 0;
	cmdTokens[i] = strtok(cmd, " "); // tokenize on spaces
	while (cmdTokens[i++] && i < sizeof(cmdTokens)) {
		cmdTokens[i] = strtok(NULL, " ");
	}
}

// read one character of input, then discard up to the newline - do not modify
char readChar() {
	char c = getchar();
	while (getchar() != '\n');
	return c;
}

static void timeout_kill(int signo) { //signal handler for alarm, kills calling process
	//printf("Signal called! Killing child: %d\n",child_pid);
    kill(child_pid,SIGKILL);
}

// main method - program entry point
int main() {
	char cmd[81]; // array of chars (a string)
	char* cmdTokens[20]; // array of strings
	int count; // number of times to execute command
	char parallelInput; // input to parse and determine if parallel
	int parallel; // whether to run in parallel or sequentially
	int timeout; // max seconds to run set of commands (parallel) or each command (sequentially)


	signal(SIGALRM, (void (*)(int))timeout_kill); //sets up signal handler for when alarm goes off

	while (TRUE) { // main shell input loop

		// begin parsing code - do not modify
		printf("closh> ");
		fgets(cmd, sizeof(cmd), stdin);
		if (cmd[0] == '\n') continue;
		readCmdTokens(cmd, cmdTokens);
		do {
			printf("  count> ");
			count = readChar() - '0';
		} while (count <= 0 || count > 9);
		do {
			printf("  [p]arallel or [s]equential> ");
			parallelInput = readChar();
		} while (parallelInput != 'p' && parallelInput != 's');
		parallel = (parallelInput == 'p') ? TRUE : FALSE;
		do {
			printf("  timeout> ");
			timeout = readChar() - '0';
		} while (timeout < 0 || timeout > 9);
		// end parsing code

		////////////////////////////////////////////////////////
		//                                                    //
		// TODO: use cmdTokens, count, parallel, and timeout  //
		// to implement the rest of closh                     //
		//                                                    //
		// /////////////////////////////////////////////////////

		// just executes the given command once - REPLACE THIS CODE WITH YOUR OWN

		if (parallel) { // handle concurrent execution
			int i, j;
			pid_t pids[count]; //array that stores child pids
			
			for (i = 0; i < count; i++) {
				pids[i] = fork();
				if (pids[i] == 0) { //child process
					//if its a child process set up alarm and try to execute the cmd
					//if alarm goes off the child is terminated by the signal handler
					printf("CHILD: %d forked with PARENT ID: %d\n", getpid(), getppid());
					alarm(timeout);
					execvp(cmdTokens[0], cmdTokens); // replaces the current process with the given program
					printf("CHILD: %d can't execute %s\n",getpid(), cmdTokens[0]); // only reached if running the program failed
					exit(1);
				} else if (pids[i] < 0){ // error when forking
					fprintf(stderr, "ERR: Something went wrong while forking children.\n");
					exit(1);
				}
			}
			for (j = 0; j < count; j++) {
				if (pids[j] > 0) { //parent process
					waitpid(pids[j], NULL, 0);//waits for each child process to terminate, then reaps them
				}
			}
		} else { // handle sequential execution
			int i;
			
			for (i = 0; i < count; i++) {
				if ((child_pid = fork()) < 0) { // error when forking
					fprintf(stderr, "ERR: Something went wrong when forking.");
					exit(1);
				}
				else if (child_pid == 0) { // have child process do work then exit immediately
					printf("CHILD: %d forked with PARENT ID: %d\n", getpid(), getppid());
					execvp(cmdTokens[0], cmdTokens); // replaces the current process with the given program
					printf("CHILD: %d can't execute %s\n",getpid(), cmdTokens[0]); // only reached if running the program failed
					exit(1);
				} else { // wait for child to finish
					alarm(timeout); //if the current child process exceeds the timeout, the signal handler is called to kill the process
					waitpid(-1, NULL, 0); //waits for all child processes to terminate 
				}
			}
			alarm(0); //resets the alarm 
		}
	} 
	
}

