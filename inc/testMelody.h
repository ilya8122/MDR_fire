/**
  ******************************************************************************
  * File Name          : testMelody.h
  * Date               : 15/02/2021 
  * Description        : 
  ******************************************************************************
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __testMelody_H
#define __testMelody_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#define DEBUG_audio 0

void testAudioStart(uint8_t* needless, uint8_t num); //start play
void testAudioStop(void); //stop play

uint16_t play_nextBlockCallback(uint16_t* pBuf, uint16_t maxLen);

#ifdef __cplusplus
}
#endif
#endif /*__testMelody_H */
