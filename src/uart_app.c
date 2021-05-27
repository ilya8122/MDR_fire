/**
  ******************************************************************************
  * File Name          : USART_app.c
  * Date               : 31/07/2015 10:26:00
  * Description        : 
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "fifo.h"
#include "uart_app.h"

//#include "MDR32F9Qx_board.h"
//#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_it.h"
#include "timer.h"

//FILE *uartBLE;		// Bluetooth console
//FILE *uart485PC;	// Computer RS485 (console)


volatile FIFO(512) u1_rx_fifo;
volatile FIFO(512) u1_tx_fifo;

uint32_t uart1_IT_TX_flag = RESET;
uint32_t uart1_IT_RX_flag = RESET;

uint8_t chRxU1;

//static volatile char chTxU1;
static volatile uint8_t dataInUart1RcvBuf = 0;	// 

/**
  * @brief  Initializes the UART1
  * @param  None
  * @retval None
  */
void UART1_Init(void)
{
 PORT_InitTypeDef PortInit;
 UART_InitTypeDef UART_InitStructure;
	
  /* Enables the HSI clock on PORTB,PORTD */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB,ENABLE);
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD,ENABLE);

  /* Fill PortInit structure */
  PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
  PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
  PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
  PortInit.PORT_PD = PORT_PD_DRIVER;
  PortInit.PORT_GFEN = PORT_GFEN_OFF;
  PortInit.PORT_FUNC = PORT_FUNC_ALTER;
  PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;

  /* Configure PORTB pins 5 (UART1_TX) as output */
  PortInit.PORT_OE = PORT_OE_OUT;
  PortInit.PORT_Pin = PORT_Pin_5;
  PORT_Init(MDR_PORTB, &PortInit);

  /* Configure PORTB pins 6 (UART1_RX) as input */
  PortInit.PORT_OE = PORT_OE_IN;
  PortInit.PORT_Pin = PORT_Pin_6;
  PORT_Init(MDR_PORTB, &PortInit);

  /* Configure PORTD pins 1 (UART2_TX) as output */
/*  PortInit.PORT_OE = PORT_OE_OUT;
  PortInit.PORT_Pin = PORT_Pin_1;
  PORT_Init(MDR_PORTD, &PortInit);
*/
  /* Configure PORTD pins 0 (UART1_RX) as input */
/*  PortInit.PORT_OE = PORT_OE_IN;
  PortInit.PORT_Pin = PORT_Pin_0;
  PORT_Init(MDR_PORTD, &PortInit);
*/
  /* Select HSI/2 as CPU_CLK source */
//  RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);

  /* Enables the CPU_CLK clock on UART1,UART2 */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
//  RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);

  /* Set the HCLK division factor = 8 for UART1,UART2 */
  UART_BRGInit(MDR_UART1, UART_HCLKdiv8);
//  UART_BRGInit(MDR_UART2, UART_HCLKdiv1);

  NVIC_EnableIRQ(UART1_IRQn);
//  NVIC_EnableIRQ(UART2_IRQn);

  /* Initialize UART_InitStructure */
  UART_InitStructure.UART_BaudRate                = 4800;
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;
  UART_InitStructure.UART_StopBits                = UART_StopBits1;
  UART_InitStructure.UART_Parity                  = UART_Parity_No;
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

  /* Configure UART1 parameters */
  UART_Init (MDR_UART1,&UART_InitStructure);

  /* Enable interrupt */
  UART_ITConfig (MDR_UART1, UART_IT_TX | UART_IT_RX, ENABLE);

  /* Enables UART1 peripheral */
  UART_Cmd(MDR_UART1,ENABLE);

  /* Configure UART2 parameters */
//  UART_Init (MDR_UART2,&UART_InitStructure);

  /* Enable Receiver interrupt */
//  UART_ITConfig (MDR_UART2, UART_IT_RX, ENABLE);

  /* Enables UART2 peripheral */
//  UART_Cmd(MDR_UART2,ENABLE);
}


/**
  * @brief  Processing received bytes from UART1 (callback from receive ISR). Place it to RxFIFO
  * @param  receive byte + status bits in high byte in WORD
  * @retval None
  */
void UART1_RxCpltCallback(uint16_t chRx) 
{ 
	if( !FIFO_IS_FULL( u1_rx_fifo ) ) {
		FIFO_PUSH( u1_rx_fifo, (uint8_t)chRx );
		dataInUart1RcvBuf = 1;
	}
}
/**
  * @brief  Handling the transfer complete event UART1 (callback from ISR)
  * @param  None
  * @retval None
  */
void UART1_TxCpltCallback(void)
{
	uint16_t ch;
  NVIC_DisableIRQ(UART1_IRQn);
	if( !FIFO_IS_EMPTY( u1_tx_fifo ) ){ // ???? ??? ?????????
		ch = FIFO_FRONT( u1_tx_fifo );
		FIFO_POP( u1_tx_fifo );
		UART_SendData (MDR_UART1, (uint16_t)ch);
	}
	else{
	}
  NVIC_EnableIRQ(UART1_IRQn);
}

/**
  * @brief  Handling the error event UART1 (callback from ISR)
  * @param  None
  * @retval None
  */
void UART1_ErrorCallback(void)
{
//	HAL_UART_Receive_IT(&huart1, &chRxU1, 1);
}

/**
  * @brief  put byte to stdout (redirect from stdio) with use FIFO
  * @param  byte to sent
  * @retval byte to sent
  */
int stdout_putchar (int ch)
{
	uint32_t	startTime=HAL_GetTick();
	if(UART_GetFlagStatus(MDR_UART1, UART_FLAG_TXFE) == SET){
    UART_SendData (MDR_UART1, (uint16_t)ch);
	}
	else{
		while(FIFO_IS_FULL( u1_tx_fifo )){	// wait, if need
			if(HAL_GetTick()-startTime > 2){	// no more than 2ms!
				return ch;
			}
		}
//		if( !FIFO_IS_FULL( u1_tx_fifo ) ) {
			FIFO_PUSH( u1_tx_fifo, (uint16_t)ch );
//		}
	}
  return ch;
}

/**
  * @brief  get data from stdin (redirect from stdio) with use FIFO
  * @param  None
  * @retval byte from RxFIFO, if fifo not empty. -1, if fifo empty
  */
int stdin_getchar (void)
{
  int ret=-1;

  NVIC_DisableIRQ(UART1_IRQn);
	if( !FIFO_IS_EMPTY( u1_rx_fifo ) ){
		ret = FIFO_FRONT( u1_rx_fifo );
		FIFO_POP( u1_rx_fifo );
	}
  NVIC_EnableIRQ(UART1_IRQn);
  return ret;
}


// 
void terminal_putc(char ch){
	putchar(ch);
}

/**
  * @brief  FLUSH the Received FIFO from UART1
  * @param  None
  * @retval None
  */
void clrUart1Rx(void){
	FIFO_FLUSH(u1_rx_fifo);
}
/**
  * @brief  Waiting for completion of sending data TxFIFO UART1
  * @param  None
  * @retval None
  */
void waitUart1TxComplete(void){
	while(!FIFO_IS_EMPTY( u1_tx_fifo ));
}

void waitUartTxEnd (FILE* to)
{
	if(to == stdout){
		waitUart1TxComplete();
	}
}
