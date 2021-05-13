#include <stdint.h>
#include <string.h>
#include "config.h"
#include "tools.h"
#include "timer.h"


/*=======================================================================*/
/*
Конвертация числа из BCD кода в BINARY.
*/
uint8_t bcdToBin(uint8_t bcd)
{
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/*
Конвертация числа BINARY -> BCD.
*/
uint8_t binToBCD(uint8_t bin)
{
	bin=bin%100; // 0 to 99 only!
	return (uint8_t)((bin / 10) << 4) + (bin % 10);
}

/*
Функция удаления ведущих пробелов
*/
char* ltrim (char * str) 
{
	while(*str == ' '){
		str++;
	}
	return (str);
}

/*
Функция удаления концевых пробелов
*/
void rtrim (char * str) 
{
	uint32_t len;
	len = strlen(str);
	if(len == 0){
		return;
	}
	len--;
	str += len;	// укажем на последний символ
	while((*str == ' ') && (len > 0)){
		str--;
		len--;
	}
	*(str+1) = 0;
	return;
}

// *********************************************************
/*
Преобразование из милисекунд в тики
*/
uint32_t msToTick(uint32_t ms)
{
	return (uint16_t)(ms / (1000* SYS_TICK));
}

/*
Преобразование из тиков в милисекунды
*/
uint32_t tickToMs(uint32_t ticks)
{
	return (uint32_t)(ticks * 1000 * SYS_TICK);
}

/*
Преобразование из секунд в количество периодов опроса
*/
uint32_t secondsToPeriod(uint32_t seconds)
{
	return (uint16_t)(seconds / (PERIOD_CYCLE));
}

/*
Преобразование из количества периодов опроса в секунды
*/
uint32_t PeriodToSeconds(uint32_t period)
{
	return (uint32_t)(period * PERIOD_CYCLE);
}

/*
Пауза в милисекундах
*/
void delayMs(uint32_t ms)
{
	uint32_t start, end;
	start = HAL_GetTick();
	end=start+ms;
	while(end!=HAL_GetTick());
}

/*
Функция перевода строки в верхний регистр включая русские в CP1251 (аналог strupr())
*/
char* strupr1251 (char * str1251) 
{
	uint8_t i=0;
	while(str1251[i]){
		if((str1251[i] >= 'a') && (str1251[i] <= 'z')){
			str1251[i] -=0x20;// ('a' - 'A'); 
		}
		else{
			if((str1251[i] >= 'а') && (str1251[i] <= 'я')){ //
				str1251[i] -= ('д' - 'Д'); 
			}
			else{
				if((str1251[i] == 'ё')){
					str1251[i] = 'Ё';
				}
			}
		}		
		i++;
	}
	return (str1251);
}

