/*
Работа с таймером
 */ 

#include <stdint.h>
#include <stdio.h>
#include "timer.h" 	
#include "config.h" 	
#include "uart_app.h"
#include "terminal.h"
#include "player.h"
#include "sensors.h"
#include "led.h"
#include "MDR32F9Qx_adc.h"
#include "fire_sensor_module.h"

volatile uint8_t sys_tick_evt = 0;
volatile uint8_t period_cycle = 0;

volatile uint8_t check_sensor_0 = 0;
volatile uint8_t check_sensor_1 = 0;
volatile uint8_t check_sensor_2 = 0;
volatile uint8_t check_sensor_3 = 0;
volatile uint8_t check_sensor_4 = 0;
volatile uint8_t check_sensor_5 = 0;
volatile uint8_t check_sensor_6 = 0;
volatile uint8_t check_sensor_7 = 0;
volatile uint8_t check_sensor_8 = 0;
volatile uint8_t check_sensor_9 = 0;
volatile uint8_t check_sensor_10= 0;
volatile uint8_t check_sensor_11= 0;
volatile uint8_t check_sensor_12= 0;
volatile uint8_t check_sensor_13= 0;
volatile uint8_t check_sensor_14= 0;
volatile uint8_t check_sensor_15= 0;

static uint16_t period = (uint16_t)(PERIOD_CYCLE / SYS_TICK);
static volatile uint16_t delayMs=0;
static uint32_t uwTick=0;

void delay_ms(uint16_t delay)
{
	delayMs = delay;
	while(delayMs > 0);
}

/*
Обработчик прерывания таймера периода опроса
Timer/Counter0 Compare Match 
*/
void HAL_SYSTICK_Callback(void) //1ms from standart HAL
{
	static uint16_t tic;
	uwTick++;
	tic = (uint16_t)(SYS_TICK / 0.001);
  tic--;
  if(!tic){
		period--;
		if(!period){
			period_cycle = 1;	// установка признака события  
			
			check_sensor_0=1;// установка признака события  
			check_sensor_1=1;// установка признака события  
/*
			check_sensor_2=1;// установка признака события  
			check_sensor_3=1;// установка признака события  
			check_sensor_4=1;// установка признака события  
			check_sensor_5=1;// установка признака события  
			check_sensor_6=1;// установка признака события  
			check_sensor_7=1;// установка признака события  
			check_sensor_8=1;// установка признака события  
			check_sensor_9=1;// установка признака события  
			check_sensor_10=1;// установка признака события  
			check_sensor_11=1;// установка признака события  
			check_sensor_12=1;// установка признака события  
			check_sensor_13=1;// установка признака события  
			check_sensor_14=1;// установка признака события  
			check_sensor_15=1;// установка признака события  
*/			
			period = (uint16_t)(PERIOD_CYCLE / SYS_TICK);
		}
		sys_tick_evt = 1;
		tic = (uint16_t)(SYS_TICK / 0.001);
		if(delayMs>0){
			delayMs--;
		}
	}
}

void syncPeriod (void)
{
	period = (uint16_t)(PERIOD_CYCLE / SYS_TICK);
}

uint32_t HAL_GetTick(void)
{
	return uwTick;
}


void event_sys_tick_evt(void)
{
static int ReciveByte=0x00;
	sys_tick_evt=0;
	dmaReqProcessing();
	ReciveByte = getchar();
	if(ReciveByte >=0){
//				putchar(ReciveByte);
			terminal_rx(&uart1RxBuf, ReciveByte);
	}else{
		clearerr(stdin);
	}
}

void event_period_cycle(void)
{
			period_cycle=0;
/*			SPI_SS_EXT_FLASH_SELECT();
			HAL_SPI_TransmitReceive(MDR_SSP1,&dataTx,&dataRx,1,100);
			SPI_SS_EXT_FLASH_DESELECT();*/
/*			printf("=%d, %d, %d\n", (uint16_t)adc_getVal(ADC_CH_TEMP_SENSOR), (uint16_t)adc_getVal(ADC_CH_ADC2), (uint16_t)adc_getVal(ADC_CH_INT_VREF));
			waitUart1TxComplete();*/
			//getCh0Value();
/*			printf("=%.2f, %.2f, %.2f[V]\n", chGetOut(&ch0), chGetOutLpf(&ch0), ch0.in);
			waitUart1TxComplete();*/
}

void check_sensor_0_handler(void)
{
check_sensor_0=0;
//BlinkLED(2, 10000000, LED3);
	fire_chek_module( ADC_CH_ADC2);
}

void check_sensor_1_handler(void)
{
check_sensor_1=0;
//BlinkLED(2, 10000000, LED5);
	fire_chek_module( ADC_CH_ADC3);
}

void check_sensor_2_handler(void)
{
check_sensor_2=0;
	fire_chek_module( ADC_CH_ADC2);
}


