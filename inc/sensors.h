/**
  ******************************************************************************
  * File Name          : battery.h
  * Date               : 28/08/2019
  * Description        : 
  ******************************************************************************
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __battery_H
#define __battery_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm32l1xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "channel.h"

#define R4			(100000.0)	// devider
#define R3			(24000.0)	// devider
#define K_VOLTAGE	(((float)(R4+R3))/R3)

#define W_LAG_BATT		(0.95)		
#define KA			(1.00)	//(1.03)	
#define KB			(0.0)

extern channelRd_s ch0;

void ch0Init(void);
float getCh0Value(void);
//float getChargePercent(float voltage);

	 
#ifdef __cplusplus
}
#endif
#endif /*__ __battery_H */

