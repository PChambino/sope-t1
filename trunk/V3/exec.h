
#ifndef _EXEC_H_
#define _EXEC_H_

#include <unistd.h>
#include <sys/wait.h>
#include "cmd.h"

/** Executa o comando.
	Cria um processo usando fork e exec para executar o comando.
	
	@return a PID do processo, ou -1 em caso de erro no fork
*/
pid_t exec_simple(Command_Info *cmd_info);

#endif // _EXEC_H_

