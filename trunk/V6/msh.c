
#include "msh.h"

static pid_t* childrenFG = NULL; ///< PIDs of foreground children processes
static int childrenFGcount = 0;
static pid_t childBG = 0; ///< PID of background child process

static int status = 0; ///< a Process status
static pid_t child = 0; ///< a PID of a child process, variavel auxiliar

static int waitForChildren = 1; ///< indica se deve esperar pelos processos filhos

/** Procura por um PID num array com a forma do devolvido pela funcao exec_pipe().

	@return o index no array
*/
static int childIn(pid_t id, pid_t *arr) {
	if (arr == NULL)
		return -1;
	int i = 0;
	for (i = 0; arr[i] > 0; i++) {
		if (arr[i] == id)
			return i;
	}
	return -1;
}

/** Conta o numero de elementos num array com a forma do devolvido pela funcao exec_pipe().

	@return o numero de elementos do array
*/
static int childrenCount(pid_t *arr) {
	if (arr == NULL)
		return 0;
	int i = 0;
	for (i = 0; arr[i] > 0; i++) {}
	return i;
}

/** Verifica o status do processo.
	Imprime mensagens de erro se necessario.
*/
static void checkStatus(const int *status) {
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

/** SIGINT handler.
	Sends the SIGKILL to the children processes in foreground.
*/
void sigint_handler(int sig) {
	if(childrenFG == NULL)
		return;
	// exists children running in foreground
	int i = 0;	
	for (i = 0; childrenFG[i] > 0; i++) {
		kill(childrenFG[i], SIGKILL);
	}
	printf("\n");
}

/** SIGCHLD handler.
	Waits for the child, and processes it's status.
	Diferenciates a background process from a foreground one.
*/
void sigchld_handler(int sig) {
	if (!waitForChildren) // se estiver a executar nao faz wait pelos filhos
		return;
		
	while ((child = waitpid(-1, &status, WNOHANG)) > 0) {

		checkStatus(&status);	
		
		if (childIn(child, childrenFG) != -1) { // Filho que terminou estava em foreground!
			childrenFGcount--;
		} else if (childBG == child) { // Filho que terminou estava em background
			printf("PID %d Done\n", child);
			childBG = 0;
		}
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

		cmd_clear(cmd_info); // limpa o comando antes de fazer parse
		if (parse_cmd(line, cmd_info) != 0) {
			if (!cmd_empty(cmd_info))
				fprintf(stderr, "Syntax error!\n");
			continue;
		}
			
		if (strcmp(cmd_info->arg[0], "exit") == 0) // builtin command exit
			break;
		
		waitForChildren = 0;
		if (cmd_info->background == 1) { // Background execution
			childrenFG = exec_pipe(cmd_info);
			if (childrenFG != NULL) {
				childBG = childrenFG[0];
				free(childrenFG);
				childrenFG = NULL;
				printf("PID %d\n", childBG);
			}
		}
		else if ((childrenFG = exec_pipe(cmd_info)) > 0) { // Foreground execution
			if (childrenFG != NULL) {
				childrenFGcount = childrenCount(childrenFG);
				waitForChildren = 1;
				while (childrenFGcount > 0) {} // espera enquanto processo em foreground ainda esta a correr
				free(childrenFG);
				childrenFG = NULL;
			}
		}
		waitForChildren = 1;
	}

	deleteCommand_Info(cmd_info);
}
