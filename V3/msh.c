
#include "msh.h"

void prompt() {
	char line[MAX_PROMPT_LINE];
	Command_Info* cmd_info = newCommand_Info(MAX_PROMPT_LINE);
	
	pid_t child = 0;
	int status = 0;

	while (1) {
		printf("msh$ ");	fflush(stdin);
		fgets(line, MAX_PROMPT_LINE, stdin);
		if (feof(stdin) || ferror(stdin))
			break;

		if (parse_cmd(line, cmd_info) != 0) {
			if (!cmd_empty(cmd_info))
				printf("Syntax error!\n");
			continue;
		}
			
		if (strcmp(cmd_info->arg[0], "exit") == 0) // builtin command exit
			break;
			
		if ((child = exec_simple(cmd_info)) > 0) {
			waitpid(child, &status, 0);
			
			if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
				printf("Command Not Found!\n");
		}
	}

	deleteCommand_Info(cmd_info);
}

