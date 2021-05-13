/**
  ******************************************************************************
  * File Name          : detect.h
  * Date               : 10/03/2021
  * Description        : 
  ******************************************************************************
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DETECT_H
#define __DETECT_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include "channel.h"

typedef enum{
	THR_DETECT_GT,	// >
	THR_DETECT_LT,	// <
} thr_detectType_s;

/*
Пороговый детектор с гистерезисом	(threshold detector)
*/
typedef struct{
	float sp;						//set point
	float hyst;					// гистерезис
	thr_detectType_s type;
	uint8_t state;		// состояние детектора (1 - событие активно)
	uint8_t event;		// индикатор изменения состояния события (устанавливается при обнаружении изменения state 0 <-> 1)
}thr_detectParam_s;

int thrDetectorFunc (float val, thr_detectParam_s* c);

typedef struct{
	thr_detectParam_s data;
	int (*pDetectorFunc) (float, thr_detectParam_s*);		// обработчик
}thr_detect_s;

	 
#ifdef __cplusplus
}
#endif
#endif /*__DETECT_H */

