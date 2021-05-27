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

struct sensor 
{
	  uint32_t chanel;
    uint8_t type;
		uint8_t sensor_status;
		float R_PREV;
		float R;
		uint8_t first_iteration_flag;
};

   struct sensor sensor_list[]=
{
0x00 /*!< Specifies the ADC channel 0.  */,0,0,0,0,1,
0x01 /*!< Specifies the ADC channel 1.  */,1,0,0,0,1,
0x02 /*!< Specifies the ADC channel 2.  */,0,0,0,0,1,
0x03 /*!< Specifies the ADC channel 3.  */,0,0,0,0,1,
0x04 /*!< Specifies the ADC channel 4.  */,0,0,0,0,1,
0x05 /*!< Specifies the ADC channel 5.  */,0,0,0,0,1,
0x06 /*!< Specifies the ADC channel 6.  */,0,0,0,0,1,
0x07 /*!< Specifies the ADC channel 7.  */,0,0,0,0,1,
0x08 /*!< Specifies the ADC channel 8.  */,0,0,0,0,1,
0x09 /*!< Specifies the ADC channel 9.  */,0,0,0,0,1,
0x0A /*!< Specifies the ADC channel 10. */,0,0,0,0,1,
0x0B /*!< Specifies the ADC channel 11. */,0,0,0,0,1,
0x0C /*!< Specifies the ADC channel 12. */,0,0,0,0,1,
0x0D /*!< Specifies the ADC channel 13. */,0,0,0,0,1,
0x0E /*!< Specifies the ADC channel 14. */,0,0,0,0,1,
0x0F /*!< Specifies the ADC channel 15. */,0,0,0,0,1,
} ;
//uint8_t sensor_status = 0;

uint32_t SENSOR_ADC_CH;//

/**
* @brief  actions that will be performed after the sensor status is determined(after reaction(float R))
* @sensor_status  sensor_status
*/
void post_reaction_choose(uint8_t sensor_status_loc)
{
	uint8_t voice[3];
switch ( sensor_status_loc )
	{
case off:
  printf("\r\n SENSOR UNCONNECT!!!!!!!!!!!!!!!!!! %d , %f",SENSOR_ADC_CH, sensor_list[SENSOR_ADC_CH].R);
       
        uint8_t voice[1] = { sensor_fall };
        testAudioStart(voice, 1);
  break;

case ok_connect:
#if (DEBUG_fire == 1)
    printf("\r\n SENSOR CONNECT %d,",SENSOR_ADC_CH);
		printf("\r\n SENSOR OK R=%f",sensor_list[SENSOR_ADC_CH].R);    
#endif
 voice[0] =  sensor_ok ;
testAudioStart(voice, 1);
  break;

case fall_connect:
#if (DEBUG_fire == 1)
				printf("\r\n SENSOR CONNECT %d",SENSOR_ADC_CH);
        printf("\r\n SENSOR BAD R=%f",sensor_list[SENSOR_ADC_CH].R);
#endif

 voice[0] =  sensor_bad ;
testAudioStart(voice, 1);
	break;

case pozhar:
	#if (DEBUG_fire == 1)
        printf("\r\nR=%f POZZZZZZHHHHHHHHHHHAAAAAAAAAAAAAAAAAAAAAAARRRRRRRRRRRRRR sensor %d", sensor_list[SENSOR_ADC_CH].R,SENSOR_ADC_CH);
	#endif

 uint8_t l[3] = { fire, in_sector, a1 };
 testAudioStart(l, 3);
	break;

default:
   printf("\r\nERROR post_reaction_choose!!!!!!!!!!! %d", SENSOR_ADC_CH);
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
   sensor_list[SENSOR_ADC_CH].R_PREV = sensor_list[SENSOR_ADC_CH].R;
    sensor_list[SENSOR_ADC_CH].R = SERIAL_Resistor2 * (U / (core_U - U));
    return sensor_list[SENSOR_ADC_CH].R;
}

/**
* @brief  sensor_diagnostic  
* @retval sensor_status 
*/
uint8_t sensor_diagnostic()
{
	
    if (sensor_list[SENSOR_ADC_CH].R < top_good && sensor_list[SENSOR_ADC_CH].R > bot_good )
		{       
        sensor_list[SENSOR_ADC_CH].sensor_status = ok_connect;
				post_reaction_choose(sensor_list[SENSOR_ADC_CH].sensor_status);
        return sensor_list[SENSOR_ADC_CH].sensor_status;
    }		
    else
		{        
        sensor_list[SENSOR_ADC_CH].sensor_status = fall_connect;
				post_reaction_choose(sensor_list[SENSOR_ADC_CH].sensor_status);
        return sensor_list[SENSOR_ADC_CH].sensor_status;
    }
}

/**
* @brief  reaction on change resistanse 
* @retval  R in om
*/
uint8_t reaction(float   R)
{
	
    if (sensor_list[SENSOR_ADC_CH].R  - sensor_list[SENSOR_ADC_CH].R_PREV > dopusk_value
			&& (sensor_list[SENSOR_ADC_CH].sensor_status == ok_connect || sensor_list[SENSOR_ADC_CH].sensor_status == fall_connect)
			&& sensor_list[SENSOR_ADC_CH].sensor_status != pozhar) 
		{
        sensor_list[SENSOR_ADC_CH].sensor_status = off;
				post_reaction_choose(sensor_list[SENSOR_ADC_CH].sensor_status);
        return sensor_list[SENSOR_ADC_CH].sensor_status;
    }

    if (sensor_list[SENSOR_ADC_CH].R  > top_good && sensor_list[SENSOR_ADC_CH].sensor_status != pozhar) 
			{
#if (DEBUG_fire == 1)
        printf("\r\n SENSOR %d UNACTIVE , %f",SENSOR_ADC_CH, sensor_list[SENSOR_ADC_CH].R );
/*				 printf("\r\n SENSOR %d delta , %f",SENSOR_ADC_CH, sensor_list[SENSOR_ADC_CH].R  - sensor_list[SENSOR_ADC_CH].R_PREV );
				 printf("\r\n SENSOR %d status , %f",SENSOR_ADC_CH, sensor_list[SENSOR_ADC_CH].sensor_status );*/
#endif
      //  sensor_status = off;
        return sensor_list[SENSOR_ADC_CH].sensor_status;
    }

    if (sensor_list[SENSOR_ADC_CH].R  < top_good && sensor_list[SENSOR_ADC_CH].R  > bot_good && sensor_list[SENSOR_ADC_CH].sensor_status == ok_connect && sensor_list[SENSOR_ADC_CH].sensor_status != pozhar) {
#if (DEBUG_fire == 1)
        printf("\r\n SENSOR %d  ACTIVE , %f",SENSOR_ADC_CH, sensor_list[SENSOR_ADC_CH].R );
#endif
        return sensor_list[SENSOR_ADC_CH].sensor_status;
    }

    if (sensor_list[SENSOR_ADC_CH].R_PREV - sensor_list[SENSOR_ADC_CH].R  > dopusk_value && sensor_list[SENSOR_ADC_CH].sensor_status == off && sensor_list[SENSOR_ADC_CH].sensor_status != pozhar&& sensor_list[SENSOR_ADC_CH].R >0&& sensor_list[SENSOR_ADC_CH].R_PREV>0) {
		sensor_list[SENSOR_ADC_CH].sensor_status=      sensor_diagnostic();
        return sensor_list[SENSOR_ADC_CH].sensor_status;
    }

    if (sensor_list[SENSOR_ADC_CH].sensor_status == ok_connect && sensor_list[SENSOR_ADC_CH].R  < bot_good) {
			
        sensor_list[SENSOR_ADC_CH].sensor_status = pozhar;
				post_reaction_choose(sensor_list[SENSOR_ADC_CH].sensor_status);
        return sensor_list[SENSOR_ADC_CH].sensor_status;
    }

    if (sensor_list[SENSOR_ADC_CH].first_iteration_flag == 1 && sensor_list[SENSOR_ADC_CH].sensor_status != pozhar) {
        sensor_list[SENSOR_ADC_CH].first_iteration_flag = 0;
   sensor_list[SENSOR_ADC_CH].sensor_status=     sensor_diagnostic();
        return sensor_list[SENSOR_ADC_CH].sensor_status;
    }
		
		  if (sensor_list[SENSOR_ADC_CH].sensor_status != pozhar) 
		{
        printf("\r\nR=%f POZAR sensor %d", sensor_list[SENSOR_ADC_CH].R,SENSOR_ADC_CH);
        return sensor_list[SENSOR_ADC_CH].sensor_status;
    }
		printf("\r\nERROR  reaction		%d		!!!!!!!!!!! ", SENSOR_ADC_CH);
		printf("\r\nsensor %d  	R_PREV=	%f	!!!!!!!!!!! ", SENSOR_ADC_CH,sensor_list[SENSOR_ADC_CH].R);
		printf("\r\nsensor %d  	R =		%f		!!!!!!!!!!! ", SENSOR_ADC_CH,sensor_list[SENSOR_ADC_CH].R_PREV);
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