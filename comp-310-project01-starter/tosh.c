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

		// (3) determine how to execute it, and then execute it

	}

	return 0;
}
