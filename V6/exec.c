
#include "exec.h"

pid_t exec_simple(Command_Info *cmd_info) {
	pid_t child = fork();

	if (child < 0) // child process not created!
		return -1;

	if (child > 0) // parent returns child pid
		return child;
		
	// only child gets here:
	
	// input file
	if (cmd_info->infile != NULL) {
		int file = open(cmd_info->infile, O_RDONLY);
		if (file == -1) // File not found!
			exit(2);
		dup2(file, STDIN_FILENO);
	}
	
	// output file
	if (cmd_info->outfile != NULL) {
		int file = open(cmd_info->outfile, O_WRONLY | O_CREAT, 0644);
		dup2(file, STDOUT_FILENO);
	}
	
	// execute command
	execvp(cmd_info->arg[0], cmd_info->arg);
	
	// if it gets here execution failed!
	exit(1);
}

pid_t exec_simple_back(Command_Info *cmd_info) {
	pid_t child = fork();

	if (child < 0) // child process not created!
		return -1;

	if (child > 0) // parent returns child pid
		return child;
		
	// only child gets here:
	
	// input file
	if (cmd_info->infile != NULL) {
		int file = open(cmd_info->infile, O_RDONLY);
		if (file == -1) // File not found!
			exit(2);
		dup2(file, STDIN_FILENO);
	} else {
		int file = open("/dev/null", O_RDONLY);
		if (file == -1) // File not found!
			exit(2);
		dup2(file, STDIN_FILENO);
	}
	
	// output file
	if (cmd_info->outfile != NULL) {
		int file = open(cmd_info->outfile, O_WRONLY | O_CREAT, 0644);
		dup2(file, STDOUT_FILENO);
	}
	
	// set process group, so it won't receive signals sent to the msh
	setpgid(child, child);
	
	// execute command
	execvp(cmd_info->arg[0], cmd_info->arg);
	
	// if it gets here execution failed!
	exit(1);
}
/** Funcao auxiliar para executar o primeiro comando com pipes.
*/
static pid_t exec_pipe_first_cmd(Command_Info *cmd_info, int* fildes) {
	pid_t child = fork();

	if (child < 0) // child process not created!
		return -1;

	if (child > 0) // parent returns child pid
		return child;
		
	// only child gets here:
	
	// set process group, so it won't receive signals sent to the msh while in background
	if (cmd_info->background == 1)
		setpgid(child, child);
	
	// input file
	if (cmd_info->infile != NULL) {
		int file = open(cmd_info->infile, O_RDONLY);
		if (file == -1) // File not found!
			exit(2);
		dup2(file, STDIN_FILENO);
	} else if (cmd_info->background == 1) {
		int file = open("/dev/null", O_RDONLY);
		if (file == -1) // File not found!
			exit(2);
		dup2(file, STDIN_FILENO);
	}
	
	// se liga ao proximo pipe
	if (fildes != NULL) {
		// nao le deste pipe apenas escreve
		close(fildes[0]);

		// output file
		if (cmd_info->outfile != NULL) {
			int file = open(cmd_info->outfile, O_WRONLY | O_CREAT, 0644);
			dup2(file, STDOUT_FILENO);
	
			// vai escrever no output file e nao no pipe
			close(fildes[1]);
		} else {
			// o stdout vai para o pipe
			dup2(fildes[1], STDOUT_FILENO);
		}
		
	// se nao tem um proximo comando, mas tem um outfile
	} else if (cmd_info->outfile != NULL) {
		int file = open(cmd_info->outfile, O_WRONLY | O_CREAT, 0644);
		dup2(file, STDOUT_FILENO);
	}
	
	// execute command
	execvp(cmd_info->arg[0], cmd_info->arg);
	
	// if it gets here execution failed!
	exit(1);
}

/** Funcao auxiliar para executar comandos ligados por pipes apos a chamada de exec_pipe_first_cmd().
*/
static pid_t exec_pipe_next_cmd(Command_Info *cmd_info, int* fildes, int* fildes_next) {
	pid_t child = fork();

	if (child < 0) // child process not created!
		return -1;

	if (child > 0) // parent returns child pid
		return child;
		
	// only child gets here:
	
	// fecha pipe de escrita
	close(fildes[1]);

	// input file
	if (cmd_info->infile != NULL) {
		int file = open(cmd_info->infile, O_RDONLY);
		if (file == -1) // File not found!
			exit(2);
		dup2(file, STDIN_FILENO);
				
		// vai ler do input file e nao do pipe
		close(fildes[0]);
	} else {
		// o stdin vem do pipe
		dup2(fildes[0], STDIN_FILENO);
	}
	
	// se liga ao proximo pipe
	if (fildes_next != NULL) {
		// nao le deste pipe apenas escreve
		close(fildes_next[0]);

		// output file
		if (cmd_info->outfile != NULL) {
			int file = open(cmd_info->outfile, O_WRONLY | O_CREAT, 0644);
			dup2(file, STDOUT_FILENO);
	
			// vai escrever no output file e nao no pipe
			close(fildes_next[1]);
		} else {
			// o stdout vai para o pipe
			dup2(fildes_next[1], STDOUT_FILENO);
		}
		
	// se nao tem um proximo comando, mas tem um outfile
	} else if (cmd_info->outfile != NULL) {
		int file = open(cmd_info->outfile, O_WRONLY | O_CREAT, 0644);
		dup2(file, STDOUT_FILENO);
	}

	// execute command
	execvp(cmd_info->arg[0], cmd_info->arg);
	
	// if it gets here execution failed!
	exit(1);
}

/** Liberta a memoria dos pipes.
*/
static void free_pipes(int** pipes, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		close(pipes[i][0]);
		close(pipes[i][1]);
		free(pipes[i]);
	}
	free(pipes);
}

pid_t* exec_pipe(Command_Info *cmd_info) {
	if (cmd_info == NULL)
		return NULL;
	
	// conta os comandos a executar
	Command_Info *cmd;	
	int cmd_count = 0;
	for (cmd = cmd_info; cmd != NULL; cmd = cmd->next) {
		cmd_count++;
	}
	
	// inicia o array para as PIDs do processos filhos
	pid_t* children = calloc( cmd_count + 1, sizeof(pid_t));
	children[cmd_count] = 0; // o array termina em 0 (zero)
	
	// inicia o array para os pipes
	int** pipes = calloc( cmd_count, sizeof(int*));
	pipes[cmd_count - 1] = NULL; // nao associa um pipe ao ultimo comando
	int i;
	for (i = 0; i < cmd_count - 1; i++) {
		pipes[i] = calloc( 2, sizeof(int));
		pipe(pipes[i]);
	}
	
	// executa o primeiro comando
	if ((children[0] = exec_pipe_first_cmd(cmd_info, pipes[0])) == -1) {
		free(children);
		free_pipes(pipes, cmd_count - 1);
		return NULL;
	}
	
	// executa os restantes comandos
	i = 0;
	for (cmd = cmd_info->next; cmd != NULL; cmd = cmd->next) {
		if ((children[i+1] = exec_pipe_next_cmd(cmd, pipes[i], pipes[i+1])) == -1) {
			free(children);
			free_pipes(pipes, cmd_count - 1);
			return NULL;
		}
		
		i++;
	}
	
	// liberta a memoria dos pipes
	free_pipes(pipes, cmd_count - 1);
	
	return children;
}
