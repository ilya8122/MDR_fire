/*
Работа с консолью
*/

#ifndef _INCL_TERMINAL_H
	#define _INCL_TERMINAL_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

//	#define RX_BUFSIZE 1000		// длина буфера сбора команды терминала
	#define RX_BUFSIZE COMMAND_MAX_LEN		// длина буфера сбора команды терминала

typedef struct{
	char RxBuf[RX_BUFSIZE];
	char *cp; // cp - указатель на место куда класть символы
	bool complete;
	bool echo;
} rxBuf_s;

extern rxBuf_s uart1RxBuf;

// объявления функций для работы с терминалом

// основная функция терминала. Параметр - символ
// возврат:
//	- 0 - ошибок нет, команда не собрана
// < 0 - код ошибки (для анализа необходимо взять с обратным знаком)
// 1 - командная строка готова
void TerminalRxBufInit (rxBuf_s*);
int16_t terminal_rx (rxBuf_s*, unsigned char);
void TerminalMsgDone(rxBuf_s*);	// подтверждение обработки принятой строки - можно собирать следующую

/* По завершению приема командной строки, модуль вызывает функцию 
 __weak void TRML_strComplete(char *pStr)
 Для использования ее, нужно в проекте обявить функцию вида: void TRML_strComplete(char *pStr)
 После обработки строки, не забудь вызвать TerminalMsgDone()!
*/

extern void TRML_strComplete(rxBuf_s*);
extern void TRML_putc(rxBuf_s*, char);	// терминал вызывает для выдачи "эхо" принятых символов (в проекте использовать void TRML_putc(char ch) для переопределения

const char* TRML_getErrStr(int16_t err); // считывание текста ошибки
uint8_t TRML_getBusy(rxBuf_s*);
#endif

