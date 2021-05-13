/*
Терминал.
	Обработка вводимых символов и сборка строки до символа '\n'
	Принятые байты функцией terminal_rx() помещаются в буфер RxBuf[]
	По символу '\n' формируется флаг готовности командной строки.
	Подтверждение обработки и разрешение приема следующего ввода - TerminalMsgDone().
*/

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "terminal.h"
#include "CmdLineTerminal.h"

//****************************************************************************
// именованные константы (могут быть заданы сверху)

#ifndef CallBackTrmlFunc
	#define CallBackTrmlFunc()		;		
#endif

enum{
	TERMINAL_BUSY=1,
};

static const char errMsg0[]="";
static const char errMsg1[]="terminal is busy. Command string exist!\n";
static const char* pErrMsg[]={
	errMsg0,
	errMsg1,
};
#define msgCnt (sizeof(pErrMsg)/sizeof(pErrMsg[0]))
//****************************************************************************
// внешние переменные
// функция для вывода символов на терминальное устройство
//extern void TRML_putc(char ch);

//****************************************************************************
// глобальные переменные доступные извне
rxBuf_s uart1RxBuf;

//****************************************************************************
// глобальные переменные внутреннего пользования
/*static char RxBuf[RX_BUFSIZE];
static char *cp = RxBuf; // cp - указатель на место куда класть символы
static uint8_t complete = 0;
*/
//****************************************************************************
// используемые только модулем функции
#if 0
__weak void TRML_strComplete(rxBuf_s* rxBuf)
{
  /* NOTE: This function should not be modified, when the callback is needed,
           the TRML_strComplete can be implemented in the user file
   */ 
}

 __weak void TRML_putc(rxBuf_s* rxBuf, char ch)
{
  /* NOTE: This function should not be modified, when the callback is needed,
           the TRML_putc can be implemented in the user file
   */ 
}
#else
// 
 void TRML_strComplete(rxBuf_s* rxBuf)
{
	if(rxBuf==&uart1RxBuf){
		cmdLine(rxBuf->RxBuf, stdout, CMD_PRIVILEGE_LEVEL_0);
	}
/*	if(rxBuf==&uart485PCTrmlRxBuf){
		cmdLine(rxBuf->RxBuf, uart485PC, CMD_PRIVILEGE_LEVEL_0);
	}*/
	TerminalMsgDone(rxBuf); //
}

// 
void TRML_putc(rxBuf_s* rxBuf, char ch)
{
	if(rxBuf==&uart1RxBuf && uart1RxBuf.echo){
		fputc(ch, stdout);
	}
/*	if(rxBuf==&uart485PCTrmlRxBuf && uart485PCTrmlRxBuf.echo){
		fputc(ch, uart485PC);
	}*/
}
#endif

//****************************************************************************
// пока не введен ENTER, процедура помещает поступающие символы в буфер
// после ввода ENTER функция устанавливает флаг приема командной строки
// и пока командная строка не будет обработана, дальнейшая информация не принимается
// Также устанавливается флаг complete, что командная строка собрана (событие)
// возврат:
//	- 0 - ошибок нет, командная строка не собрана
// < 0 - код ошибки
// 1 - командная строка готова

int16_t terminal_rx (rxBuf_s* rxBuf, unsigned char c) 
{
  char *cp2;	

// защита от вызова в период обработки ранее принятого буфера
	if(rxBuf->complete){		
		return(-TERMINAL_BUSY);
	}

	// обрабатываем спец символы
	// behaviour similar to Unix stty ICRNL 
	if (c == '\r')
		c = '\n';
	if (c == '\n'){	// если код = 13
		*rxBuf->cp = c;	// если это ENTER то записываем в буфер без инкремента указателя
		TRML_putc(rxBuf, c);	// отсылаем обратно
		rxBuf->cp++;
		*rxBuf->cp = 0;	// на всякий случай добавим конец строки
		// инициализируем указатели для приема следующей команды
		rxBuf->cp = rxBuf->RxBuf;		// указатель для складывания символов в буфер в прерывании UART
		// и здесь вызовем функцию декодирования и разборки команды
		rxBuf->complete = 1;
		TRML_strComplete(rxBuf);
		return (1);
	}
	else{ 	// если это рядовой символ
		if (c == '\t')
			c = ' ';
		// проверка диапазона кода символа
	  if ((c >= (unsigned char)' ' && c <= (unsigned char)'\x7e') || c >= (unsigned char)'\xa0') {
			if (rxBuf->cp == rxBuf->RxBuf + RX_BUFSIZE - 2)	// если буфер полностью занят, то BEEP
				TRML_putc(rxBuf, '\a');
			else{	// иначе инкремент указателя и помещаем в буфер символ 
				*rxBuf->cp++ = c;
				TRML_putc(rxBuf, c);	// эхо
			}
//		continue;	// на начало цикла
	  }
	  else{ // обработка специальных символов
			switch (c){
				case 'c' & 0x1f:	// CTRL-C
				break;

				// * . \RxBuf (BS) or \177 (DEL) delete the previous character
				case '\b':	// BACKSPACE
				case '\x7f':
					if (rxBuf->cp > rxBuf->RxBuf)	{
						TRML_putc(rxBuf, '\b');
						TRML_putc(rxBuf, ' ');
						TRML_putc(rxBuf, '\b');
						rxBuf->cp--;	
					}
				break;

				// * . ^r sends a CR, and then reprints the buffer
				case 'r' & 0x1f:
					TRML_putc(rxBuf, '\r');
					for (cp2 = rxBuf->RxBuf; cp2 < rxBuf->cp; cp2++)
						TRML_putc(rxBuf, *cp2);
				break;

				// * . ^u kills the entire input buffer
				case 'u' & 0x1f:
					while (rxBuf->cp > rxBuf->RxBuf){
						TRML_putc(rxBuf, '\b');
						TRML_putc(rxBuf, ' ');
						TRML_putc(rxBuf, '\b');
						rxBuf->cp--;
					}
				break;

				// * . ^w deletes the previous word
				case 'w' & 0x1f:
					while (rxBuf->cp > rxBuf->RxBuf && rxBuf->cp[-1] != ' ')	{
						TRML_putc(rxBuf, '\b');
						TRML_putc(rxBuf, ' ');
						TRML_putc(rxBuf, '\b');
						rxBuf->cp--;
					}
				break;
			}	// switch
	 	}	//else обработка специальных символов
	}	// else ENTER
	return (0);
}

//****************************************************************************
// функция подтверждения обработки сообщения
void TerminalMsgDone(rxBuf_s* rxBuf)
{
	rxBuf->complete = 0;
}
// считывание текста ошибки
const char* TRML_getErrStr(int16_t err)
{
	if(err<0) err=-err;
	if(err < msgCnt){
		return pErrMsg[err];
	}
	return pErrMsg[0];
}

uint8_t TRML_getBusy(rxBuf_s* rxBuf)
{
	return rxBuf->complete;
}

void TerminalRxBufInit (rxBuf_s* rxBuf)
{
	rxBuf->complete=false;
	rxBuf->cp=rxBuf->RxBuf;
	rxBuf->echo=false;
}

