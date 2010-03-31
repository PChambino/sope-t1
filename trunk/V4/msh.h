
#ifndef _MSH_H_
#define _MSH_H_

#include <signal.h>
#include "cmd.h"
#include "exec.h"

#define MAX_PROMPT_LINE 1024 ///< maximo tamanho da linha recebida no prompt

/** SIGINT handler.
	Sends the SIGINT to the child process.
*/
void sigint_handler(int sig);

/** Inicia o prompt da mini shell.
	Le os comandos e executa-os se parse_cmd devolver 0.
	Faz depois um waitpid pelo processo filho.
	Imprime mensagens de erro relativas 'a syntax ou se nao encontrar um comando.
*/
void prompt();

#endif // _MSH_H_
