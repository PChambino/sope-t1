
#include "msh.h"

static pid_t* children = NULL; ///< PID of child processes
static pid_t* childrenBG = 0; ///< PID of first child process on background
static int status = 0; ///< Process status
static pid_t child = 0; ///< PID of a child process, variavel auxiliar

static int childIn(pid_t id, pid_t *arr) {
	int i = 0;
	for (i = 0; arr[i] != 0; i++) {
		if (arr[i] == id)
			return i;
	}
	return -1;
}

void sigint_handler(int sig) {
	if(children == NULL)
		return;
		
	// exists children running in foreground
	int i = 0;	
	for (i = 0; children[i] != 0; i++) {
		if (children[i] > 0)
			kill(children[i], SIGKILL);
	}
	printf("\n");
}

void sigchld_handler(int sig) {
	child = wait(&status);

	if (child <= 0) // Filho invalido!
		return;

	checkStatus(&status);	
	
	/*if (childIn(child, children) != -1) { // Filho que terminou estava em foreground!
		child = 0; // Reset child para 0, indicando que nao ha nenhum processo em foreground
	} else { // Filho que terminou estava em background
		printf("PID %d Done\n", childBG);
	}*/
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

		cmd_clear(cmd_info); // limpa o comando antes de fazer parse
		if (parse_cmd(line, cmd_info) != 0) {
			if (!cmd_empty(cmd_info))
				fprintf(stderr, "Syntax error!\n");
			continue;
		}
			
		if (strcmp(cmd_info->arg[0], "exit") == 0) // builtin command exit
			break;
		
		if (cmd_info->background == 1) { // Background execution
			childrenBG = exec_pipe(cmd_info);
			if (childrenBG != NULL) {
				printf("PID %d\n", childrenBG[0]);
				free(childrenBG);
				childrenBG = NULL;
			}
		}
		else if ((children = exec_pipe(cmd_info)) > 0) { // Foreground execution
			//while (children != NULL) {} // espera enquanto processo em foreground ainda esta a correr
			if (children != NULL) {
				free(children);
				children = NULL;
			}
		}
	}

	deleteCommand_Info(cmd_info);
}

void checkStatus(const int *status) {
	if (WIFEXITED(*status))
		switch (WEXITSTATUS(*status)) {
			case 0:
				break;
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
