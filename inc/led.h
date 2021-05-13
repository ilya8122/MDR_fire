#ifndef __LED_H
#define __LED_H

#include "MDR32F9Qx_port.h"
#include <stdint.h>

#define LED1_2_PORT     MDR_PORTF
#define LED1_2_CLK_PORT	RST_CLK_PCLK_PORTF
#define LED1            PORT_Pin_0
#define LED2            PORT_Pin_1

#define LED3            PORT_Pin_2
#define LED4            PORT_Pin_3
#define LED5            PORT_Pin_4
#define LED6            PORT_Pin_5
#define BLINK_DELAY 100000

//void Delay(__IO uint32_t nCount);
void LEDConfig(void);
void LEDOn(uint32_t LED_Num);
void LEDOff(uint32_t LED_Num);
void BlinkLED(uint32_t num, uint32_t del,uint32_t LED_Num);

#endif /* __LED_H */
