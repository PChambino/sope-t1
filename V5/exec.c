
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
