/**
  ******************************************************************************
  * @file    led.c
  * @author  BPV
  * @version V1.0.0
  * @date    12/02/2021
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#include "led.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define _DELAY 400000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Executes delay loop.
  * @param  nCount: specifies the delay.
  * @retval None
  */
static void Delay(__IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}

void LEDConfig(void)
{
	PORT_InitTypeDef PORT_InitStructure;
  /* Enables the RTCHSE clock on PORTE and PORTF */
  RST_CLK_PCLKcmd(LED1_2_CLK_PORT, ENABLE);

  /* Configure PORTF pins 0,1 for output to switch LEDs on/off */

  PORT_InitStructure.PORT_Pin   = (LED1 | LED2| LED3 | LED4 | LED5 | LED6 );
  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

  PORT_Init(LED1_2_PORT, &PORT_InitStructure);
}


void LEDOn(uint32_t LED_Num)
{
  PORT_SetBits(LED1_2_PORT, LED_Num);
}

void LEDOff(uint32_t LED_Num)
{
  PORT_ResetBits(LED1_2_PORT, LED_Num);
}

void BlinkLED(uint32_t num, uint32_t del,uint32_t LED_Num)
{
  uint32_t cnt;
  for ( cnt = 0; cnt < num; cnt++)
  {
    LEDOn(LED_Num);
    Delay(del);
    LEDOff(LED_Num);
    Delay(del);
  }
}
