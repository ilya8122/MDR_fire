/**
  ******************************************************************************
  * File Name          : detect.c
  * Date               : 10/03/2021
  * Description        : 
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "sensors.h"
#include "detect.h"

/**
  * @brief  threshold detector function.
  * @param  channel value
  * @param  detector parameters (c - context)
  * @retval 0 - no event detect
						1 - event detect
  */
int thrDetectorFunc (float val, thr_detectParam_s* c)
{
	uint8_t statePrev = c->state;
	if(c->type == THR_DETECT_GT){
		if(val>(c->sp + c->hyst)){
			c->state=1;
		}
		if(val<(c->sp - c->hyst)){
			c->state=0;
		}
	}
	if(c->type == THR_DETECT_LT){
		if(val<(c->sp + c->hyst)){
			c->state=1;
		}
		if(val>(c->sp - c->hyst)){
			c->state=0;
		}
	}
	if(statePrev != c->state){
		c->event=1;
	}
	return c->event;
}

