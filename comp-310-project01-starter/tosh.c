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
static int handleCommand(char **args, int bg);
//void parseAndExecute(char *cmdline, char **args);
void runExternalCommand(char **args, int bg);
int parseArguments(char * const line, char **argv);
void add_to_history(char * const cmd);
void print_history();
char* get_command(unsigned int cmd_num);
int parseCommandLine(char **arg, char *cmd_path);



void child_reaper(__attribute__ ((unused)) int sig_num) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(){ 
	//char cmdline[MAXLINE];
	//char *argv[MAXARGS];
	 
	//register a signal handler for SIGCHLD here
	struct sigaction sa;
	sa.sa_handler = child_reaper;
	sa.sa_flags = 0;
	sigaction(SIGCHLD, &sa, NULL);
	

	while(1) {

		// (0) print the shell prompt
		//fprintf(stdout, "tosh> ");

		//fflush(stdout);
		// (1) read in the next command entered by the user
		char *cmdline = readline("tosh$ ");
		//printf("%s", cmdline);
		//if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)){
		//	clearerr(stdin);
		//	continue;
		//}
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
		if (argv[0] == NULL) {
			continue;
		}
		// (3) determine how to execute it, and then execute it
		if (argv[0] != NULL){
			if (argv[0][0] != '!'){
				add_to_history(cmdline);
			}
			if(handleCommand(argv, bg)){
				continue;
			}
		}		
		char cmd_path[MAXLINE];
		int cmd_exists = parseCommandLine(argv, cmd_path);
		
		if(cmd_exists == 0) {
			printf("%s does not exist\n", argv[0]);
			continue;
		}

                runExternalCommand(argv, bg);
	}

	return 0;
}

int parseCommandLine(char **argv, char *cmd_path) {
	char *path = getenv("PATH");
	printf("Possible paths: %s\n", path);
	char exec_name[MAXLINE] = "";
	strcpy(exec_name, argv[0]);
	
	char *token = strtok(path, ":");
	int access_flag = -1;
	while(token != NULL) {
		//concatinate token and cmd here
		strcpy(cmd_path, token);
		strcat(cmd_path, "/");
		strcat(cmd_path, exec_name);
		printf("%s\n", cmd_path);
		access_flag = access(cmd_path, X_OK);
		fflush(stdout);
		if(access_flag == 0) { 
			break;
		}
		token = strtok(NULL, ":");
		printf("DEBUG: Checkpoint\n");
		fflush(stdout);
		printf("Next token: %s\n", cmd_path);
	}
	printf("DEBUG: access flag is -1\n");
	if(access_flag == -1) {
		//command doesn't exist
		return 0;
	}
	strcpy(argv[0], cmd_path);
	return 1;
}

int handleCommand(char **args, int bg){
	 // handle the built in commands directly
        if (strcmp(args[0], "exit") == 0) {
                printf("Goodbye! Thank you for using the Torero Shell!\n");
                exit(0);
		return 1;
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
                        printf("current working directory is: %s\n", cwd);
                }

                int ch_dir = chdir(args[1]);
                if (ch_dir == -1){
                        fprintf(stderr, "ERROR: directory not found\n");
                }
		else {
			getcwd(cwd, sizeof(cwd));
			printf("new directory is: %s\n", cwd);
		}
		return 1;
        }

        else if (strcmp(args[0], "history") == 0){
                print_history();
		return 1;
        }

        else if (args[0][0] == '!'){
                unsigned int cmd_num = strtoul(&args[0][1], NULL, 10);
                char *cmd = get_command(cmd_num);
                if (cmd == NULL){
                        fprintf(stderr, "ERROR: %d is not in history\n", cmd_num);
                }
                else {
			printf("command that should be executed: %s\n", cmd);
			char *argv[MAXARGS];
			int bg = parseArguments(cmd, argv);
			if (argv[0] == NULL) {
				return 0;
			}
			// (3) determine how to execute it, and then execute it
			if (argv[0] != NULL){
				if (argv[0][0] != '!'){
					add_to_history(cmd);
				}
				if(handleCommand(argv, bg)){
					return 1;
				}
			}		
			char cmd_path[MAXLINE];
			int cmd_exists = parseCommandLine(argv, cmd_path);
			
			if(cmd_exists == 0) {
				printf("%s does not exist\n", argv[0]);
				return 0;
			}

			runExternalCommand(argv, bg);
                }
		return 1;

        }
        else
        {
		return 0;
		//runExternalCommand(args, bg);
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
