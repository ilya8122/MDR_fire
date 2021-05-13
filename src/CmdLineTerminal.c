/*
 * CmdLineTerminal.cpp
 *
 *  Created on: 20 янв. 2016 г.
 *      Author: nafta
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "CmdLineTerminal.h"
#include "tools.h"
#include "timer.h" 	

extern void waitUartTxEnd(FILE *f);

static 	int argc;	// количество считанных параметров
static	char* argv[CMD_MAX_N_PAR + 1];	// указатель на параметры (строки)
static FILE* to=stdin;
static cmdHandlers_t *pCmdList=NULL;
static cmdHandlers_t *pNotFoundCmd=NULL;	
static int cmdCnt=0;
static cpl_t cplUser=CMD_PRIVILEGE_LEVEL_3;

void setCmdTbl(cmdHandlers_t *cmdTbl, int cnt)
{
	pCmdList=cmdTbl;
	cmdCnt=cnt;
}
void setNotFoundCmdHandler(cmdHandlers_t *hdl)
{
	pNotFoundCmd=hdl;
}
/*
Проверка наличия токена START_PARAM_TOKEN в начале параметра. Если он найден, то весь текст до токена END_PARAM_TOKEN считается одним параметром.
Задача - выдать указатель на конец завершающего токена в таком случае.
Если завершающий токен не найден при наличии открывающего, возвращаем исходный указатель
*/
char* checkStringParam(const char* pStr)
{
	char* pNext = (char*)pStr;
	pNext = ltrim(pNext);
	if(strncmp(pNext, START_PARAM_TOKEN, strlen(START_PARAM_TOKEN)) == 0){
		if((pNext = strstr(pNext, END_PARAM_TOKEN)) != NULL){
			return pNext;
		}
	}
	return (char*)pStr;
}

/*
Удаление экранирующих токенов из строки
Return:
указатель на начало строки без стартового токена START_PARAM_TOKEN
*/
char* deleteTokens(const char* pStartStr)
{
	char* pStart = (char*)pStartStr;
	char* pEnd;
	pStart = ltrim(pStart);
	if(strncmp(pStart, START_PARAM_TOKEN, strlen(START_PARAM_TOKEN)) == 0){
		pStart += strlen(START_PARAM_TOKEN);
		if((pEnd = strstr(pStart, END_PARAM_TOKEN)) != NULL){
			*pEnd = '\0';
		}
	}
	return (char*)pStart;	
}

int cmdLine(char* pStr, FILE* from, cpl_t cpl) {
	int retCode;
	char * pch;
	char *saveptr;
	to = from; // сохраим, с какого интерфейса пришла командная строка
	cplUser=cpl;	// и уровень привилегий
	// в одной строке может быть несколько команд, разделенных ";"
	pch = strtok_r (pStr,";", &saveptr);
	while (pch != NULL)	  {
		parseCmdStr(pch);
		retCode = decodeCmdAndRun();
		if(retCode < 0){	// если команда успешно выполнена, то сбрасываем признак ошибки
			fprintf(from, "E: Unknown command: %s\n", argv[0]);
		}
		pch = strtok_r (NULL, ";", &saveptr);
	}
/*	pconsole->printf("Param:\n");
	for(int i=0; i<argc; i++){
		pconsole->printf("%s\n", argv[i]);
	}*/
	return 0;
}

/*!
функция разбора командной строки на отдельные команды и разбор команды на лексемы
Выход
 argc
*/
int parseCmdStr(char* pStr) {
	char * pch;
	char *saveptr;
	char *tmpPtr;
	argc=0;
	pch = strtok_r (pStr," ,=\n\r", &saveptr);
	while (pch != NULL && argc < CMD_MAX_N_PAR)	  {
		argv[argc]=pch;
		argv[argc] = deleteTokens(argv[argc]);	// удалим экранирующие токены, если имеются
		argc++;
		// Необходимо проверить на наличие строки в качестве параметра (экранирование символами START_PARAM_TOKEN, END_PARAM_TOKEN), и если такая строка найдена, пропустить ее
		// saveptr сохраняет следующую стартовую для поиска позицию
		// если мы нашли заэкранированный параметр, нам надо эту позицию скорректировать, но при этом правильно сформировать pch
		tmpPtr = checkStringParam(saveptr);
		if(tmpPtr != saveptr){	// найден заэкранированный параметр, корректируем указатели и запоминаем начальную позицию
			pch = saveptr;
			saveptr = tmpPtr;
			strtok_r (NULL, " ,=\n\r", &saveptr);
		}
		else{
			pch = strtok_r (NULL, " ,=\n\r", &saveptr);
		}
	}
	return argc;
}

/*!
декодирование команды и вызов обработчика
В argv[0] лежит либо команда, либо имя канала
Сначала поиск в таблице команд,
а уж затем обрабатываем как имя канала.
Return:
-1 - не найдена команда argv[0]
0 - команда найдена (ошибки параметров обрабатываются в команде)
*/
int decodeCmdAndRun(void)
{
	cmdHandlers_t* handler;
	if(argc <= 0){	// да нет же ничего!
		return (0);
	}
	// приведем к верхнему регистру
	argv[0] = strupr1251(argv[0]);
	// здесь непосредственно перекодируем псевдокод команды в ее номер среди разрешенных для интерфейса
	handler=findHandler(argv[0]);
	if(handler == NULL){
		handler=pNotFoundCmd;
	}
	if(handler && handler->pFunc){
		if(cplUser < handler->rightLevel){
			int res=0;
			semaphoreTrmlCmdWait();
//			if(semaphoreCmdChk(to))	// проверка и ограничение совместного доступа
			{
				res = handler->pFunc(argc-1, &argv[1], to); // в функцию передаем параметры без имени команды!
//				pwDnTimReset();	//сброс таймера выключения
			}
			semaphoreTrmlCmdRelease();
/*			if(res <0  && handler == pNotFoundCmd){
				printHelpCmd();
			}*/
			return res;
		}
		else{
			return -2;
		}
	}
	return -1;
	// если не нашли команду с таким псевдокодом, то попробуем как имя канала
/*	if((i == n_command)){
		semaphoreTrmlCmdWait();
		int res = cmd_chName(argc, argv, pConsole);
		semaphoreTrmlCmdRelease();
		return res;
	}
	// если все нормально, выполняем команду с защитой от нескольких потоков
	semaphoreTrmlCmdWait();
	CmdTable[i].pFunc(argc, argv, pConsole);
	semaphoreTrmlCmdRelease();
	return 0;*/
}


cmdHandlers_t* findHandler(char* name) {
int i;
	for (i = 0; i < cmdCnt; i++) {
	   if (strcmp(pCmdList[i].name, name) == 0)
	       return &pCmdList[i];
	}
	/*    for (int i = 0; i < cmdList.size(); i++) {
	        if (strcmp(cmdList[i].name, name) == 0)
	            return &cmdList[i];
	    }*/
    return 0;
}

void printHelpCmd(FILE* to) {
	int i;
	fprintf(to, "Command list:\n");
	for (i = 0; i < cmdCnt; i++) {
		fprintf(to,"%s %s\n", pCmdList[i].name, pCmdList[i].hlp);
		waitUartTxEnd(to);
	}
}
