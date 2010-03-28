
#ifndef _CMD_H_
#define _CMD_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PROMPT_LINE 1024 ///< maximo tamanho da linha recebida no prompt

typedef struct {
	char **arg; ///< lista de argumentos (terminada com NULL)
	char *infile; ///< fich. p/onde redireccionar stdin ou NULL	char *outfile; ///< fich. p/onde redirecc. stdout ou NULL
	int background;	///< 0 ou 1(=proc.o a executar em background)
} Command_Info ;

/** Recebe uma string que representa um commando
	e constroi a estrutura Command_Info.
	
	@return 0 se cmd_line estiver correcta e -1 em caso de erro
*/
int parse_cmd(char *cmd_line, Command_Info *cmd_info);

/**	Imprime o Command_Info para o stdout.
	Função para teste.
*/
void print_cmd(Command_Info *cmd_info);

/** Inicia o prompt da mini shell.
	Le os comandos e imprime-os indicando se parse_cmd deu erro ou nao.
*/
void prompt();

#endif // _CMD_H_
