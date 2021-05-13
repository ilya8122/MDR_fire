#ifndef __TOOLS_H
#define __TOOLS_H

#include <stdint.h>

#define C_TO_D(c) (c-'0')
#define D_TO_C(d) (d+'0')

const char* getMonthStr(uint8_t mo);

char* ltrim (char * str);
void rtrim (char * str);

uint32_t msToTick(uint32_t ms);
uint32_t tickToMs(uint32_t ticks);
void delayMs(uint32_t ms);

uint32_t secondsToPeriod(uint32_t seconds);
unsigned int PeriodToSeconds(uint32_t period);

uint8_t bcdToBin(uint8_t bcd);
uint8_t binToBCD(uint8_t bin);
char* strupr1251 (char * str1251) ;


#endif /* TOOLS_H */
