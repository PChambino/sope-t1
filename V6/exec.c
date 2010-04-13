
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
	
	// set process group, so it won't receive signals sent to the msh
	setpgid(child, child);

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

/** Funcao auxiliar para executar comandos ligados por pipes.
*/
static pid_t exec_pipe_aux(Command_Info *cmd_info, int fildes[2]) {
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
	
	// se existe proximo comando
	if (cmd_info->next != NULL) {
		pipe(fildes); // cria um novo pipe 
	
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
	
		// chamada recursiva para executar os proximos comandos ligados por pipes
		exec_pipe_aux(cmd_info->next, fildes);

		// nao le deste pipe apenas escreve
		close(fildes[0]);
		
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

pid_t exec_pipe(Command_Info *cmd_info) {
	pid_t child = fork();

	if (child < 0) // child process not created!
		return -1;

	if (child > 0) // parent returns child pid
		return child;
		
	// only child gets here:
	
	// set process group, so it won't receive signals sent to the msh
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
	
	// se existe proximo comando
	if (cmd_info->next != NULL) {
		int fildes[2];
		pipe(fildes); // cria um novo pipe 
	
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
	
		// chamada recursiva para executar os proximos comandos ligados por pipes
		exec_pipe_aux(cmd_info->next, fildes);

		// nao le deste pipe apenas escreve
		close(fildes[0]);
		
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
