/*
 * CmdLineTerminal.h
 *
 *  Created on: 20 янв. 2016 г.
 *      Author: nafta
 */

#ifndef CMDLINETERMINAL_H_
#define CMDLINETERMINAL_H_
#include <stdio.h>

#define CMD_MAX_N_PAR		(10)
#define CMD_MAX					(20)
#define CMD_MAX_LEN			(10)
#define HLP_CMD_MAX_LEN	(200)
// Токены для передачи в качестве параметра разных строк, в частности JSON
#define START_PARAM_TOKEN	("<s>")
#define END_PARAM_TOKEN		("</s>")

typedef enum{
	CMD_PRIVILEGE_LEVEL_0,	// admin
	CMD_PRIVILEGE_LEVEL_1,
	CMD_PRIVILEGE_LEVEL_2,
	CMD_PRIVILEGE_LEVEL_3,	// user
}cpl_t;

#if 0
	extern void semaphoreTrmlCmdWait(void);
	extern void semaphoreTrmlCmdRelease(void);
#else
	#define semaphoreTrmlCmdWait()	;
	#define semaphoreTrmlCmdRelease()	;
#endif

typedef struct{
	char name[CMD_MAX_LEN];
	cpl_t rightLevel;	// требуемый уровень привилегий команды 0 - самый высокий приоритет
	int (*pFunc) (int argc, char**, FILE*);	// , , ID интерфейса, указатель вывода информации (NULL - не выводим)
	char hlp[HLP_CMD_MAX_LEN];
}cmdHandlers_t;


int cmdLine(char* pStr, FILE* from, cpl_t cpl);
void setCmdTbl(cmdHandlers_t *cmdTbl, int cnt);
void setNotFoundCmdHandler(cmdHandlers_t *hdl);	// установка обработчика, если "команда не найдена"
void printHelpCmd(FILE* to);


int parseCmdStr(char *pStr);
int decodeCmdAndRun(void);
cmdHandlers_t* findHandler(char*);


#endif /* CMDLINETERMINAL_H_ */
