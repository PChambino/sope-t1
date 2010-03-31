
#include "msh.h"

static pid_t child = 0; ///< PID of child process
static int status = 0; ///< Process status

void sigint_handler(int sig) {
	if (child > 0)
		kill(child, SIGINT);
}

void sigchld_handler(int sig) {
	if (child > 0) // child is not in background
		return;
		
	child = wait(&status);
	if (child > 0) {
		checkStatus(&status);
		printf("PID %d Done\n", child);
	}
	child = 0;
}

void prompt() {
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		fprintf(stderr, "SIGINT handler not installed!");
	}
	if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
		fprintf(stderr, "SIGCHLD handler not installed!");
	}
	
	char line[MAX_PROMPT_LINE];
	Command_Info* cmd_info = newCommand_Info(MAX_PROMPT_LINE);
	
	while (1) {
		printf("msh$ ");	fflush(stdout);
		fgets(line, MAX_PROMPT_LINE, stdin);
		if (feof(stdin) || ferror(stdin))
			break;

		if (parse_cmd(line, cmd_info) != 0) {
			if (!cmd_empty(cmd_info))
				fprintf(stderr, "Syntax error!\n");
			continue;
		}
			
		if (strcmp(cmd_info->arg[0], "exit") == 0) // builtin command exit
			break;
		
		if (cmd_info->background == 1) { // Background execution
			child = exec_simple_back(cmd_info);
			if (child > 0)
				printf("PID %d\n", child);
			child = 0;
		}
		else if ((child = exec_simple(cmd_info)) > 0) { // Foreground execution
			waitpid(child, &status, 0);
			checkStatus(&status);
			child = 0; // reset child to 0 so it knows when to send the SIGINT signal
		}
	}

	deleteCommand_Info(cmd_info);
}

void checkStatus(const int *status) {
	if (WIFEXITED(*status))
		switch (WEXITSTATUS(*status)) {
			case 1:
				fprintf(stderr, "Command Not Found!\n");
				break;
			case 2:
				fprintf(stderr, "Input File Not Found!\n");
				break;
			default:
				break;
		}
}
