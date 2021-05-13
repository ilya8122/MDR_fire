/**
  ******************************************************************************
  * File Name          : USART_app.h
  * Date               : 31/07/2015 10:26:00
  * Description        : 
  ******************************************************************************
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_app_H
#define __usart_app_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>

//extern FILE *uartBLE;
	 
extern uint8_t chRxU1;
extern uint32_t uart1_IT_TX_flag;
extern uint32_t uart1_IT_RX_flag;


void UART1_Init(void);
	
int stdout_putchar (int ch);
int stdin_getchar (void);
void terminal_putc(char ch);

void clrUart1Rx(void);
void waitUart1TxComplete(void);
void waitUartTxEnd (FILE* to);

void UART1_RxCpltCallback(uint16_t chRx);
void UART1_TxCpltCallback(void);
void UART1_ErrorCallback(void);

#ifdef __cplusplus
}
#endif
#endif /*__ usart_app_H */

