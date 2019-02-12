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
		char *argv[MAXARGS];
		int bg = parseArguments(cmdline, argv);
		if(argv[0] == NULL) {
			continue;
		}
		char *path = getenv("PATH");
		char *token = strtok(path, ":");
		char *cmd_path = strcat(token, arg_list[0])
		int access_flag = -1;
		while(token != NULL):
			//concatinate token and cmd here
			access_flag = access(cmd_path, X_OK);
			if(access_flag == 0) { 
				break;
			}
			token = strtok(NULL, s);
		}
		if(access_flag == -1) {
			//command doesn't exist
			printf("%s does not exist", argv[0]);
			continue;
		}
		strcpy(arg_list[0], cmd_path);
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

        else if (strcmp(args[0], "cd") == 0){
                // 1 - get the value of its current working directory
                // call getcwd on startup
                // 2 - when user enters the cd command, change the current directory
                //  by calling chdir
                // 3 - subsequent calls to pwd should reflect the change in the cwd 

                char cwd[256];
                if (getcwd(cwd, sizeof(cwd)) == NULL) {
                        perror("getcwd() error");
                }
                else {
                        fprintf("curremnt working directory is: %s\n", cwd);
                }

                int ch_dir = chdir(args[1]);
                if (ch_dir == -1){
                        fprintf(stderr, "ERROR: directory not found\n");
                }
        }

        else if (strcmp(args[0], "history") == 0){
                print_history();
        }

        else if (args[0][0] == '!'){
                unsigned in cmd_num = strtoul(&args[0][1], NULL, 10);
                char *cmd = get_command(cmd_num);
                if (cmd == NULL){
                        fprintf(stderr, "ERROR: %d is not in history\n", cmd_num);
                }
                else {
                        pareseAndExecute(cmd, args);
                }

        }
        else
        {
                runExternalCommand(args, bg);
        }

}

void runExternalCommand(char **args, int bg){
	pid_t cpid = fork();
	if (cpid == 0) { // child prcoess
		//execv(full_path_to_command_executable, command_argv_list);
		// need to make sure that the full path is held in args[0]
		
		
		// if execv fails, pritn out the error statement, otherwise we should not get here
		fprintf(stderr, "ERROR: Command not found\n");
		exit(63);
	}
	else if (cpid > 0) { // parent process and background process
		if (bg){	
			// check to see if the child has returned
			// do not block if the child is still running
			waitpid(cpid, NULL, WNOHANG);
		}
		else {
			// wait here until the child finishes
			waitpid(cpid, NULL, 0);
		}
	}
	else { // fork failed
		perror("FORK:  n");//prints "FORK: " followed by an error message
		exit(1);
	}
	
}
