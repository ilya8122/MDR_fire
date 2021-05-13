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
#include "fire_sensor_module.h"
#include <stdint.h>
#include <stdio.h>
#include "testMelody.h"
#include "dictionary.h"
#include "adc.h"
#include "led.h"

enum status 
{
    off,
    ok_connect,
		fall_connect,
		pozhar
};


float R_PREV;
float R;
uint8_t sensor_status = 0;
uint8_t first_iteration_flag = 1;
uint32_t SENSOR_ADC_CH;//

/**
* @brief  actions that will be performed after the sensor status is determined(after reaction(float R))
* @sensor_status  sensor_status
*/
void post_reaction_choose(uint8_t sensor_status)
{
	uint8_t voice[3];
switch ( sensor_status )
	{
case off:
  printf("\r\n SENSOR UNCONNECT , %f", R);
       
        uint8_t voice[1] = { sensor_fall };
        testAudioStart(voice, 1);
  break;

case ok_connect:
#if (DEBUG_fire == 1)
    printf("\r\n SENSOR CONNECT");
		printf("\r\n SENSOR OK R=%f",R);    
#endif
 voice[0] =  sensor_ok ;
testAudioStart(voice, 1);
  break;

case fall_connect:
#if (DEBUG_fire == 1)
				printf("\r\n SENSOR CONNECT");
        printf("\r\n SENSOR BAD R=%f",R);
#endif

 voice[0] =  sensor_bad ;
testAudioStart(voice, 1);
	break;

case pozhar:
	#if (DEBUG_fire == 1)
        printf("\r\nR=%f POZZZZZZHHHHHHHHHHHAAAAAAAAAAAAAAAAAAAAAAARRRRRRRRRRRRRR", R);
	#endif

 uint8_t b[3] = { fire, in_sector, a1 };
 testAudioStart(b, 3);
	break;

default:
  
  break;
}
}



/**
* @brief  calculate resistance in OM
* @U  U in volts
* @retval resistance in OM
*/
float calculate_resistance(float U)
{
    /*((core_U*SERIAL_Resistor2)/U)-SERIAL_Resistor2*/ //formule2
    /* ((core_U*SERIAL_Resistor2)/U)-SERIAL_Resistor2*/ //formule1
    R_PREV = R;
    R = SERIAL_Resistor2 * (U / (core_U - U));
    return R;
}

/**
* @brief  sensor_diagnostic  
* @retval sensor_status 
*/
uint8_t sensor_diagnostic()
{
    if (R < top_good && R > bot_good )
		{       
        sensor_status = ok_connect;
				post_reaction_choose(sensor_status);
        return sensor_status;
    }		
    else
		{        
        sensor_status = fall_connect;
				post_reaction_choose(sensor_status);
        return sensor_status;
    }
}

/**
* @brief  reaction on change resistanse 
* @retval  R in om
*/
uint8_t reaction(float R )
{
    if (R - R_PREV > dopusk_value && (sensor_status == ok_connect || sensor_status == fall_connect) && sensor_status != pozhar) 
		{
        sensor_status = off;
				post_reaction_choose(sensor_status);
        return sensor_status;
    }

    if (R > top_good && sensor_status != pozhar) {
#if (DEBUG_fire == 1)
        printf("\r\n SENSOR UNACTIVE , %f", R);
#endif
      //  sensor_status = off;
        return sensor_status;
    }

    if (R < top_good && R > bot_good && sensor_status == ok_connect && sensor_status != pozhar) {
#if (DEBUG_fire == 1)
        printf("\r\n SENSOR ACTIVE , %f", R);
#endif
        return sensor_status;
    }

    if (R_PREV - R > dopusk_value && sensor_status == off && sensor_status != pozhar&& R>0&& R_PREV>0) {
        sensor_diagnostic();
        return sensor_status;
    }

    if (sensor_status == ok_connect && R < bot_good) {
			
        sensor_status = pozhar;
				post_reaction_choose(sensor_status);
        return sensor_status;
    }

    if (first_iteration_flag == 1 && sensor_status != pozhar) {
        first_iteration_flag = 0;
        sensor_diagnostic();
        return sensor_status;
    }
}

/**
* @brief  calculate voltage in volts
* @adc_channl  the ADC channel from which the value is read(sensor)
* @retval voltage in volts
*/
float calculate_U(uint32_t adc_channl)
{
    uint32_t adc;
    uint32_t adc_vref;

    adc_vref = adc_getVal(ADC_CH_INT_VREF);
    adc = adc_getVal(adc_channl);
    float inV = ((VREF_INT * adc) / adc_vref) / 1000; /*0.0008056640625*(float)adc;*/

    return inV;
}
/**
* @brief  complete fire inspection cycle 
* @adc_channl  the ADC channel from which the value is read(sensor)
*/
void fire_chek_module(uint32_t adc_channl)
{
	float R;//Om
	float inV;		
	
	SENSOR_ADC_CH=adc_channl;
	inV=calculate_U(adc_channl);	
	R=calculate_resistance(inV);	
	reaction(R);
}