/**
  ******************************************************************************
  * @file    main.c
  * @author  BPV
  * @version V1.2.0
  * @date    04/02/2021
  * @brief   Main program body.
  ******************************************************************************
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_it.h"

#include "uart_app.h"
#include "timer.h"
#include "terminal.h"
#include "command.h"
#include "player.h"
#include "adc.h"
#include "led.h"
#include "spi.h"
#include "w25qxx.h"
#include "sensors.h"

#include <stdio.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define FCPU_HSI_8MHz
#define FCPU_HSE_64MHz
//#define FCPU_HSI_64MHz


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


uint8_t debugLevel=1;	// 
//rxBuf_s uart485PCTrmlRxBuf;

FILE*	uart485PC = (FILE*)15;


/*
Структура для обработки событий
*/
struct Event {
	volatile uint8_t *sys_event;	// событие
	void (*pEventFunc) (void);		// обработчик
};

static const struct Event eventTbl[] = {	
//	{&dataInTerminalRcvBuf,		event_dataInTerminalRcvBuf},//Вызов из таймера(по прерыванию), чтоб не зависнуть
//	{&TerminalCmdMessageExist,	event_TerminalCmdMessageExist},	//cmd_engine
//	{&cmdExist,					event_cmdExist},												//cmd_engine
//	{&endParseCmdMessage,		event_endParseCmdMessage},					//cmd_engine
//	{&SIM900.events_1.Byte,			event_SIM900_event1},	
	{&sys_tick_evt,				event_sys_tick_evt},	//sysEvent
	{&period_cycle,				event_period_cycle},	//sysEvent
	
//		{&check_sensor_0,				check_sensor_0_handler},
//		{&check_sensor_1,				check_sensor_1_handler},
		/*{&check_sensor_2,				check_sensor_2_handler},
		{&check_sensor_3,				check_sensor_3_handler},
		{&check_sensor_4,				check_sensor_4_handler},
		{&check_sensor_5,				check_sensor_5_handler},
		{&check_sensor_6,				check_sensor_6_handler},
		{&check_sensor_7,				check_sensor_7_handler},
		{&check_sensor_8,				check_sensor_8_handler},
		{&check_sensor_9,				check_sensor_9_handler},
		{&check_sensor_10,			check_sensor_10_handler},
		{&check_sensor_11,			check_sensor_11_handler},
		{&check_sensor_12,			check_sensor_12_handler},
		{&check_sensor_13,			check_sensor_13_handler},
		{&check_sensor_14,			check_sensor_14_handler},
		{&check_sensor_15,			check_sensor_15_handler},*/
//	{&,			},	
};
#define EVENT_COUNT	( sizeof (eventTbl) ) / ( sizeof (eventTbl[0]) )


/**
  * @brief  Configures the CPU_PLL clock (HSE clock 8 * 8 = 64MHz).
  * @param  None
  * @retval None
  */
static void ClockConfigure(void)
{
	uint32_t cpuclock;
	RST_CLK_FreqTypeDef RST_CLK_Clocks;
  /* Configure CPU_PLL clock */
//  RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv1,0);
#ifdef FCPU_HSE_64MHz
	/* Enable HSE clock oscillator (8 MHz)*/
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
  while (RST_CLK_HSEstatus() != SUCCESS){};
	/* Select HSE clock as CPU_PLL input clock source */
	/* Set PLL multiplier to 8                        */
	RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul8);
	/* Enable CPU_PLL */
	RST_CLK_CPU_PLLcmd(ENABLE);
	while (RST_CLK_HSEstatus() != SUCCESS){};                     /* Good CPU PLL */
	/* Set CPU_C3_prescaler to 1 */
	RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
	/* Set CPU_C2_SEL to CPU_PLL output instead of CPU_C1 clock */
	RST_CLK_CPU_PLLuse(ENABLE);
	while(RST_CLK_CPU_PLLstatus() != SUCCESS){};
	/* Select CPU_C3 clock on the CPU clock MUX */
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
#endif

#ifdef FCPU_HSI_8MHz
	/* Enable HSI clock source */
	RST_CLK_HSIcmd(ENABLE);
	while (RST_CLK_HSIstatus() != SUCCESS){}                     /* Good HSI clock */
	/* Select HSI clock on the CPU clock MUX */
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkHSI);
	/* LED1 blinking with HSI clock as input clock source */
#endif

#ifdef FCPU_HSI_64MHz
	RST_CLK_HSIcmd(ENABLE);
	RST_CLK_CPU_PLLcmd(DISABLE);
	RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSIdiv1, 8);
	while (RST_CLK_HSIstatus() != SUCCESS){};                     /* Good HSI clock */
	/* Set CPU_C3_prescaler to 1 */
	RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
	/* Switch CPU_C2_SEL to CPU_C1 clock instead of CPU_PLL output */
	RST_CLK_CPU_PLLcmd(ENABLE);
	RST_CLK_CPU_PLLuse(ENABLE);
	while(RST_CLK_CPU_PLLstatus() != SUCCESS){};
	/* Select CPU_C3 clock on the CPU clock MUX */
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
#endif

	// init 1ms systick timer 
	RST_CLK_GetClocksFreq(&RST_CLK_Clocks);
	cpuclock = RST_CLK_Clocks.CPU_CLK_Frequency;
	SysTick_Config(cpuclock / 1000);	// ticks for timeot 0.001c
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main (void)
{
	uint8_t evt, eventExist;

	LEDConfig();
  BlinkLED(2, BLINK_DELAY, LED1);
  BlinkLED(2, BLINK_DELAY, LED2);
	
	ClockConfigure();
	LEDOn(LED1);
	UART1_Init();
	play_init();
	adc_init();
	SPI_init();
	NVIC_EnableIRQ(SysTick_IRQn);
	// link command table
	setCmdTbl(cmdList, cmdListCnt);

	printf("%s %s\n", VERSION_STRING, VERSION_HW);
//	fprintf(uart485PC, "12345\r\n");
	W25qxx_Init();
	ch0Init();

  while (1)
  {
		eventExist=0;
		// цикл по событиям и вызов обработчиков
		for(evt=0; evt < EVENT_COUNT; evt++){
			if(*(eventTbl[evt].sys_event) != 0)	{
				eventTbl[evt].pEventFunc();
				eventExist=1;
			}
		}
		
//sleep		if(!eventExist){HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFE);}
  }
}

/**
  * @brief  Reports the source file ID, the source line number
  *         and expression text (if USE_ASSERT_INFO == 2) where
  *         the assert_param error has occurred.
  * @param  file_id: pointer to the source file name
  * @param  line: assert_param error line source number
  * @param  expr:
  * @retval None
  */
#if (USE_ASSERT_INFO == 1)
void assert_failed(uint32_t file_id, uint32_t line)
{
  /* User can add his own implementation to report the source file ID and line number.
     Ex: printf("Wrong parameters value: file Id %d on line %d\r\n", file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#elif (USE_ASSERT_INFO == 2)
void assert_failed(uint32_t file_id, uint32_t line, const uint8_t* expr);
{
  /* User can add his own implementation to report the source file ID, line number and
     expression text.
     Ex: printf("Wrong parameters value (%s): file Id %d on line %d\r\n", expr, file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_ASSERT_INFO */

/* END OF FILE main.c */

