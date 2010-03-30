
#ifndef _MSH_H_
#define _MSH_H_

#include "cmd.h"

#define MAX_PROMPT_LINE 1024 ///< maximo tamanho da linha recebida no prompt

/** Inicia o prompt da mini shell.
	Le os comandos e imprime-os indicando se parse_cmd deu erro ou nao.
*/
void prompt();

#endif // _MSH_H_
