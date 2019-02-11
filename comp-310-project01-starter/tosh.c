/*
 * The Torero Shell (TOSH)
 *
 * Add your top-level comments here.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <signal.h>
#include <readline/readline.h>

#include "parse_args.h"
#include "history_queue.h"

// TODO: add your function prototypes here as necessary
static void handleCommand(char **args, int bg);
void parseAndExecute(char *cmdline, char **args);
void runExternalCommand(char **args, int bg);

void child_reaper(__attribute__ ((unused)) int sig_num) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(){ 
	char cmdline[MAXLINE];
	char *argv[MAXARGS];
	 
	//register a signal handler for SIGCHLD here
	struct sigaction sa;
	sa.sa_handler = child_reaper;
	sa.sa_flags = 0;
	sigaction(SIGCHILD, &sa, NULL);
	

	while(1) {

		// (0) print the shell prompt
		fprintf(stdout, "tosh> ");
		fflush(stdout);

		// (1) read in the next command entered by the user
		char *cmdline = readline("tosh$ ");

		if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)){
			clearerr(stdin);
			continue;
		}
		// NULL indicates EOF was reached, which in this case means someone
		// probably typed in CTRL-d
		if (cmdline == NULL) {
			fflush(stdout);
			exit(0);
		}

		fprintf(stdout, "DEBUG: %s\n", cmdline);

		// TODO: complete the following top-level steps
		// (2) parse the cmdline
	//need: cmd (just the command and arguements), arg_list (with [0] just the command, no arguements)
		int arg_i = 0;
		char arg_list[MAXARGS][MAXLINE];
		for(int i = 0; i < (int)strlen(cmdline); i++) {
			if(strcmp(cmdline[i]," ")
		}		

		char *path = getenv("PATH");
		const char s[2] = ":";
		char *token = strtok(path, s);
		int access_flag = -1;
		while( token != NULL):
			//concatinate token and cmd here
			access_flag = access(cmd_path, X_OK);
			if(access_flag == 0) { 
				break;
			}
			token = strtok(path, s);
		}
		if( access_flag == -1) {
			//command doesn't exist
			if(!
			continue;
		}
		// (3) determine how to execute it, and then execute it

	}

	return 0;
}

void handleCommand(char **args, int bg){
	// handle the built in commands directly
	if (strcmp(args[0], "exit") == 0) {
		printf("Goodbye! Thank you for using the Torero Shell!\n");
		exit(0);
	}	
	else if (strcmp(args[0], "history") == 0){
		print_history();
	}

}

void runExternalCommand(char **args, int bg){
	int cpid = fork();
	if (cpid == -1) { // ford failed, handling the error
		perror("fork");
		exit(1);
	}
	else if (cpid == 0) { // child process
		
	}
	else { // parent process
		pid = waitpid(cpid, &status, 0);
	}
	
}
