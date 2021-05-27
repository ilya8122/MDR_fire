/**
  ******************************************************************************
  * @file    fire_sensor_module.c
  * @author  FIA
  * @version V1.0.0
  * @date    5/02/2021
  * @brief   work with fire sensor 
  ******************************************************************************
  *
  *
  */
#define core_U 3.2
#define SERIAL_Resistor2 10000

#define dopusk_value 10000 //the interval of changing the resistance value to understand that the sensor is connected

#define sensor_off 500000 // unactive
#define top_good 10000 //top resistanse level of good fire sensor
#define bot_good 1000 //bot resistanse level of good fire sensor

#define DEBUG_fire 0

#include <stdint.h>

void fire_chek_module(uint32_t adc_channl);

float calculate_resistance(float U);

uint8_t reaction(float r); //reaction on change resistance

float calculate_U(uint32_t adc_channl);

