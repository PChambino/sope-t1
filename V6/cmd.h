
#ifndef _CMD_H_
#define _CMD_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Declaracao previa da estrutura Command_Info para ser possivel incluir
// um campo 'recursivo' que aponta para o proximo commando
typedef struct Command_Info Command_Info;

struct Command_Info {
	char **arg; ///< lista de argumentos (terminada com NULL)
	char *infile; ///< fich. p/onde redireccionar stdin ou NULL
	char *outfile; ///< fich. p/onde redirecc. stdout ou NULL
	int background;	///< 0 ou 1(=proc.o a executar em background)
	Command_Info *next; ///< proximo comando a executar com pipes
};

/** Cria um Command_Info.
	Usando o tamanho maximo da linha prompt, alocando espaco na
	memoria para (max_prompt_line / 2 + 1) tokens.
*/
Command_Info* newCommand_Info(int max_prompt_line);

/** Liberta a memória usada pelo Command_Info.
*/
void deleteCommand_Info(Command_Info *cmd_info);

/** Recebe uma string que representa um commando
	e constroi a estrutura Command_Info.
	
	@return 0 se cmd_line estiver correcta e -1 em caso de erro
*/
int parse_cmd(char *cmd_line, Command_Info *cmd_info);

/** Imprime o Command_Info para o stdout.
	Função para teste.
*/
void print_cmd(Command_Info *cmd_info);

/** Verifica se o Command_Info nao tem nenhum comando.
	@return 1 se Command_Info nao tem nenhum comando e 0 caso
	contrario.
*/
int cmd_empty(Command_Info *cmd_info);

/** Limpa o commando tornando-o vazio.
*/
void cmd_clear(Command_Info *cmd_info);

#endif // _CMD_H_

