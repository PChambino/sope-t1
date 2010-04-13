
#include "cmd.h"
#include "msh.h"

Command_Info* newCommand_Info(int max_prompt_line) {
	Command_Info* cmd = (Command_Info*) malloc(sizeof(Command_Info));
	// Maximo de tokens possiveis a partir do tamanho maximo da linha
	cmd->arg = (char **) calloc( max_prompt_line / 2 + 1, sizeof(char *));
	
	return cmd;
}

void deleteCommand_Info(Command_Info *cmd_info) {
	if (cmd_info == NULL)
		return;
		
	deleteCommand_Info(cmd_info->next);
		
	free(cmd_info->arg);
	free(cmd_info);
}

int parse_cmd(char *cmd_line, Command_Info *cmd_info) {
	if (cmd_info == NULL)
		return -1;
	
	Command_Info *cmd = cmd_info;
	
	char *token;
	char *sep = " \n\t";
	int count = 0;
	
	for (token = strtok(cmd_line, sep); token != NULL; token = strtok(NULL, sep)) {
		printf("token: %s\n", token);
		if (strcmp(token, "<") == 0) { // infile
			token = strtok(NULL, sep);
			if (token == NULL)
				return -1;
			cmd->infile = token;
		} else if (strcmp(token, ">") == 0) { // outfile
			token = strtok(NULL, sep);
			if (token == NULL)
				return -1;
			cmd->outfile = token;
		} else if (strcmp(token, "&") == 0) { // background
			token = strtok(NULL, sep);
			if (token != NULL) // apenas pode ser o ultimo token
				return -1;
			cmd->background = 1;
		} else if (strcmp(token, "|") == 0) { // pipe
			cmd->next = newCommand_Info(MAX_PROMPT_LINE);
			cmd_clear(cmd->next);
			cmd = cmd->next;
		} else { // arg
			cmd->arg[count++] = token;
		}
	}
	cmd->arg[count] = NULL;
	
	if (count == 0) // string vazia!
		return -1;
	
	return 0;
}

void print_cmd(Command_Info *cmd_info) {
	Command_Info *cmd = cmd_info;
	for (cmd = cmd_info; cmd != NULL; cmd = cmd->next) {
		printf("Command:\n");
		printf("Args:");
		int i;
		for (i = 0; cmd->arg[i] != NULL; ++i)
			printf(" %s", cmd->arg[i]);
		printf("\n");
		
		printf("InFile: %s\n", cmd->infile);
		printf("OutFile: %s\n", cmd->outfile);
		printf("Background: %d\n", cmd->background);
	}
}

int cmd_empty(Command_Info *cmd_info) {
	if(cmd_info->arg[0] == NULL 
		&& cmd_info->infile == NULL
		&& cmd_info->outfile == NULL
		&& cmd_info->background == 0
		&& cmd_info->next == NULL)
		return 1;

	return 0;
}

void cmd_clear(Command_Info *cmd_info) {
	if (cmd_info == NULL)
		return;
		
	cmd_info->arg[0] = NULL;
	cmd_info->infile = NULL;
	cmd_info->outfile = NULL;
	cmd_info->background = 0;
	deleteCommand_Info(cmd_info->next);
	cmd_info->next = NULL;
}
