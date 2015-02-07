// closh.c - CS 377, Spring 2015
// Nathan Fuller

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

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

// main method - program entry point
int main() {
	char cmd[81]; // array of chars (a string)
	char* cmdTokens[20]; // array of strings
	int count; // number of times to execute command
	char parallelInput; // input to parse and determine if parallel
	int parallel; // whether to run in parallel or sequentially
	int timeout; // max seconds to run set of commands (parallel) or each command (sequentially)

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

		int DEBUG = 1;

		if (DEBUG) { // My code

			if (parallel) { // handle concurrent execution
				int i;
				pid_t child_pid = fork(); // create first child
				for (i = 0; i < count - 1; i++) {
					if (child_pid < 0){ // err
						fprintf(stderr, "ERR: Something went wrong while forking children.\n");
						exit(1);
					}
					else if (child_pid == 0)
						break; // get child out of the loop so it can immediate move onto doing work
					else { // make parent fork the rest of the children
						child_pid = fork();
					}
				}
				
				if (child_pid == 0) { // do child pid work concurrently
					printf("CHILD: Process ID %d executing line 1.\n", getpid());
					printf("CHILD: Process ID %d executing line 2.\n", getpid());
					exit(0);
				} else {
					printf("PARENT: %d Waiting for all children to terminate.\n", child_pid);
					waitpid(-1, NULL, 0);
					printf("PARENT: %d Terminating.\n", child_pid);
					exit(0);
				}
				
			} else { // handle sequential execution
				int i;
				pid_t child_pid;

				for (i = 0; i < count; i++) {
					if ((child_pid = fork()) < 0) {
						fprintf(stderr, "ERR: Something went wrong when forking.");
						exit(1);
					}
					else if (child_pid == 0) {
						printf("CHILD: Process number %d with pid %d executing line 1.\n", i + 1, getpid());
						printf("CHILD: Process number %d with pid %d executing line 2.\n", i + 1, getpid());
						exit(0);
					} else {
						printf("PARENT: Waiting for children to finish execution.\n");
						waitpid(-1, NULL, 0);
					}

				}
				
				printf("PARENT: Terminating.\n");
				exit(0);
			}
		} else { // Original code
			execvp(cmdTokens[0], cmdTokens); // replaces the current process with the given program
			printf("Can't execute %s\n", cmdTokens[0]); // only reached if running the program failed
			exit(1);
		}
	}
}

