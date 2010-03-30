
#include "exec.h"

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

