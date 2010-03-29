
#include "cmd.h"

int parse_cmd(char *cmd_line, Command_Info *cmd_info) {
	if (cmd_info == NULL)
		return -1;
	
	cmd_info->arg[0] = NULL;
	cmd_info->infile = NULL;
	cmd_info->outfile = NULL;
	cmd_info->background = 0;
	
	char *token;
	char *sep = " \n\t";
	int count = 0;
	for (token = strtok(cmd_line, sep); token != NULL; token = strtok(NULL, sep)) {
		if (strcmp(token, "<") == 0) { // infile
			token = strtok(NULL, sep);
			if (token == NULL)
				return -1;
			cmd_info->infile = token;
		} else if (strcmp(token, ">") == 0) { // outfile
			token = strtok(NULL, sep);
			if (token == NULL)
				return -1;
			cmd_info->outfile = token;
		} else if (strcmp(token, "&") == 0) { // background
			cmd_info->background = 1;
		} else { // arg
			cmd_info->arg[count++] = token;
		}
	}
	cmd_info->arg[count] = NULL;
	
	if (count == 0) // string vazia!
		return -1;
	
	return 0;
}

void print_cmd(Command_Info *cmd_info) {
	printf("Command:\n");
	printf("Args:");
	int i;
	for (i = 0; cmd_info->arg[i] != NULL; ++i)
		printf(" %s", cmd_info->arg[i]);
	printf("\n");
	
	printf("InFile: %s\n", cmd_info->infile);
	printf("OutFile: %s\n", cmd_info->outfile);
	printf("Background: %d\n", cmd_info->background);	
}

void prompt() {
	char line[MAX_PROMPT_LINE];
	Command_Info cmd;
	// Maximo de tokens possiveis a partir do tamanhdo maximo da linha
	cmd.arg = (char **) calloc( MAX_PROMPT_LINE / 2 + 1, sizeof(char *));
	
	pid_t child = 0;
	int status = 0;
	
	while (1) {
		printf("msh$ ");	fflush(stdin);
		fgets(line, MAX_PROMPT_LINE, stdin);
		if (feof(stdin) || ferror(stdin))
			break;
		if (parse_cmd(line, &cmd) == 0)
			if ((child = exec_simple(&cmd)) > 0)
				waitpid(child, &status, 0);
	}
	
	free(cmd.arg);
}

pid_t exec_simple(Command_Info *cmd_info) {
	pid_t child = fork();

	if (child < 0) // child process not created!
		return -1;

	if (child > 0) // parent returns child pid
		return child;
		
	// only child gets here:
	// execute command
	execvp(cmd_info->arg[0], cmd_info->arg);
	
	// if it gets here execution failed!
	exit(1);
}
