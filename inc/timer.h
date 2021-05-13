#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>

extern volatile uint8_t sys_tick_evt;
extern volatile uint8_t period_cycle;

extern volatile uint8_t check_sensor_0;
extern volatile uint8_t check_sensor_1;
extern volatile uint8_t check_sensor_2;
extern volatile uint8_t check_sensor_3;
extern volatile uint8_t check_sensor_4;
extern volatile uint8_t check_sensor_5;
extern volatile uint8_t check_sensor_6;
extern volatile uint8_t check_sensor_7;
extern volatile uint8_t check_sensor_8;
extern volatile uint8_t check_sensor_9;
extern volatile uint8_t check_sensor_10;
extern volatile uint8_t check_sensor_11;
extern volatile uint8_t check_sensor_12;
extern volatile uint8_t check_sensor_13;
extern volatile uint8_t check_sensor_14;
extern volatile uint8_t check_sensor_15;



void HAL_SYSTICK_Callback(void); //1ms from standart HAL
uint32_t HAL_GetTick(void);

void syncPeriod (void);
void delay_ms(uint16_t delay);

void event_sys_tick_evt(void);
void event_period_cycle(void);
void check_sensor_0_handler(void);
void check_sensor_1_handler(void);

#endif /* TIMER_H */
