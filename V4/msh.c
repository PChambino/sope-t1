
#include "msh.h"

static pid_t child = 0; ///< PID of child process

void sigint_handler(int sig) {
	if (child != 0)
		kill(child, SIGINT);
}

void prompt() {
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
		fprintf(stderr, "SIGINT handler not installed!");
	}
	
	char line[MAX_PROMPT_LINE];
	Command_Info* cmd_info = newCommand_Info(MAX_PROMPT_LINE);
	
	int status = 0;

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
			
		if ((child = exec_simple(cmd_info)) > 0) {
			waitpid(child, &status, 0);
			child = 0; // reset child to 0 so it knows when to send the signal
			
			if (WIFEXITED(status))
				switch (WEXITSTATUS(status)) {
					case 1:
						fprintf(stderr, "Command Not Found!\n");
						break;
					case 2:
						fprintf(stderr, "File Not Found! (%s)\n", cmd_info->infile);
						break;
					default:
						break;
				}
		}
	}

	deleteCommand_Info(cmd_info);
}

