
#include "msh.h"

static pid_t child = 0; ///< PID of child process, maior que 0 se ha um processo a correr em foreground
static int status = 0; ///< Process status
static pid_t childBG = 0; ///< PID of a child process, variavel auxiliar

void sigint_handler(int sig) {
	if (child > 0) { // exists a child running in foreground
		kill(child, SIGINT);
		printf("\n");
	}
}

void sigchld_handler(int sig) {
	childBG = wait(&status);

	if (childBG <= 0) // Filho invalido!
		return;

	checkStatus(&status);	
	
	if (childBG == child) { // Filho que terminou estava em foreground!
		child = 0; // Reset child para 0, indicando que nao ha nenhum processo em foreground
	} else { // Filho que terminou estava em background
		printf("PID %d Done\n", childBG);
	}
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
			childBG = exec_simple_back(cmd_info);
			if (childBG > 0)
				printf("PID %d\n", childBG);
		}
		else if ((child = exec_simple(cmd_info)) > 0) { // Foreground execution
			while (child > 0) {} // espera enquanto processo em foreground ainda esta a correr
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
