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
#include <fcntl.h>

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

		//fprintf(stdout, "DEBUG: %s\n", cmdline);

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
	char cmd[MAXLINE];
	char *path = getenv("PATH");
	//printf("Possible paths: %s\n", path);
	char exec_name[MAXLINE] = "";
	strcpy(exec_name, argv[0]);
	char temp[MAXLINE];
	strcpy(temp, path);	
	char *token = strtok(temp, ":");
	int access_flag = -1;
	while(token != NULL) {
		//concatinate token and cmd here
		strcpy(cmd, token);
		strcat(cmd, "/");
		strcat(cmd, exec_name);
		//printf("Test cmd: %s\n", cmd);
		access_flag = access(cmd, X_OK);
		fflush(stdout);
		if(access_flag == 0) {
			//printf("Break\n"); 
			break;
		}
		token = strtok(NULL, ":");
		fflush(stdout);
	}
	if(access_flag == -1) {
		//command doesn't exist
		//printf("DEBUG: access flag is -1\n");
		return 0;
	}
	argv[0] = strdup(cmd);
//	strncpy(argv[0], cmd, (MAXLINE-1));
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
//                else {
//                       printf("current working directory is: %s\n", cwd);
//                }

                int ch_dir = chdir(args[1]);
                if (ch_dir == -1){
                        fprintf(stderr, "ERROR: directory not found\n");
                }
//		else {
//			getcwd(cwd, sizeof(cwd));
//			printf("new directory is: %s\n", cwd);
//		}
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
		runExternalCommand(args, bg);
        	return 0;
	}

}

void runExternalCommand(char **args, int bg){

	int i = 0;
	int index = 0;
	int pipe_cmd = 0;
	int si = 0;
	int siIndex = 0;
	int so = 0;
	int soIndex = 0;
	int se = 0;
	int seIndex = 0;
	while(args[i] != NULL){
		if(strcmp(args[i], "|") == 0){
			index = i;
			pipe_cmd = 1;
		}
		if(strcmp(args[i], "<") == 0){
			siIndex = i;
			si = 1;
		}
		if(strcmp(args[i], "1>") == 0){
			soIndex = i;
			so = 1;
		}
		if(strcmp(args[i], "2>") == 0){
			seIndex = i;
			se = 1;
		}
		i++;
	}	
	
	char *args2[MAXARGS];	

	// TODO: add statement to handle to I/O redirect_cmd

	pid_t cpid1, cpid2;
	if(pipe_cmd ==1){
		
		memcpy(args2, args + index + 1, 20 *sizeof(*args));
		args[index] = NULL;

		int pipe_id[2];
		if(pipe(pipe_id) != 0){
			printf("Pipe failed\n");
			return;
		}
		/*
		if (pipe(pipe_id)){
			fprintf(stderr, "Pipe failed.\n");
			exit(1);
		}
		*/
		cpid1 = fork();
		if (cpid1 == 0) { // child prcoess
			
			close(pipe_id[0]);
			dup2(pipe_id[1], STDOUT_FILENO);
			close(pipe_id[1]);
			
			//printf("COMAND PATH FOR EXECV: %s\n", args[0]);
			//fprintf(stderr,"child executes!");
			//fflush(stdout);
			execv(args[0], args);

			//fprintf(stderr, "ERROR: Command not found\n");
			exit(63);
		}
		else { // parent process 
			cpid2 = fork();
			if(cpid2 == 0){		
				close(pipe_id[1]);
				dup2(pipe_id[0], STDIN_FILENO);
				close(pipe_id[0]);
				//printf("parent with child executes!\n");
				execv(args2[0], args2);
				//printf("_______________\n");
				// prints if execv fails
				//fprintf(stdout, "ERROR: Command not found.\n");
			} 
			else {
				//wait here until the children finish
				//printf("about to wait for the children to finish\n");
				waitpid(cpid2, NULL, 0);
				waitpid(cpid1, NULL, 0);
			}
			
		}
	}

	else {
		
		cpid1 = fork();
		if (cpid1 == 0){ // child process
			//new code for IO Redirection
			

			if(si == 1){
				int fid = open(args[siIndex + 1], O_RDONLY, 0666);
				dup2(fid, 0);
				close(fid);	
				args[siIndex] = NULL;
			}	
			if(so == 1){
				int fid = open(args[soIndex + 1], O_WRONLY | O_CREAT, 0666);
				dup2(fid, 1);
				close(fid);
				args[soIndex] = NULL;
			}
			if(se == 1){
				int fid = open(args[seIndex + 1], O_APPEND | O_WRONLY | O_CREAT, 0666);
				dup2(fid, 2);
				close(fid);
				args[seIndex] = NULL;
			}
			
			//commented out following code for clarity
			char full_cmd[MAXLINE] = "";
			strcat(full_cmd, args[0]);
			//printf("full_cmd: %s\n", full_cmd);	
			execv(full_cmd, args);
			//prints if execv fails
			//fprintf(stderr, "ERROR: Commnand not found.\n");
			exit(63);
		}
		else if (cpid1 > 0){// parent process and background process

			if (bg){	
				// check to see if the child has returned
				// do not block if the child is still running
				waitpid(cpid1, NULL, WNOHANG);
			}
			else {
				// wait here until the child finishes
				waitpid(cpid1, NULL, 0);
				//printf("done waiting for child\n");
				
			}

		}
		else { // fork failed
			perror("FORK:  n");//prints "FORK: " followed by an error message
			exit(1);
		}


	}

}
	
