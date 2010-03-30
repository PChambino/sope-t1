
#include "msh.h"

void prompt() {
	char line[MAX_PROMPT_LINE];
	Command_Info* cmd_info = newCommand_Info(MAX_PROMPT_LINE);
	
	while (1) {
		printf("msh$ ");	fflush(stdin);
		fgets(line, MAX_PROMPT_LINE, stdin);
		if (feof(stdin) || ferror(stdin))
			break;
		if (parse_cmd(line, cmd_info) == 0)
			printf("OK\n");
		else
			printf("Err\n");
		print_cmd(cmd_info);
	}
	
	deleteCommand_Info(cmd_info);
}

