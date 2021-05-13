#include <stdint.h>
#include <string.h>
#include "config.h"
#include "tools.h"
#include "timer.h"


/*=======================================================================*/
/*
����������� ����� �� BCD ���� � BINARY.
*/
uint8_t bcdToBin(uint8_t bcd)
{
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/*
����������� ����� BINARY -> BCD.
*/
uint8_t binToBCD(uint8_t bin)
{
	bin=bin%100; // 0 to 99 only!
	return (uint8_t)((bin / 10) << 4) + (bin % 10);
}

/*
������� �������� ������� ��������
*/
char* ltrim (char * str) 
{
	while(*str == ' '){
		str++;
	}
	return (str);
}

/*
������� �������� �������� ��������
*/
void rtrim (char * str) 
{
	uint32_t len;
	len = strlen(str);
	if(len == 0){
		return;
	}
	len--;
	str += len;	// ������ �� ��������� ������
	while((*str == ' ') && (len > 0)){
		str--;
		len--;
	}
	*(str+1) = 0;
	return;
}

// *********************************************************
/*
�������������� �� ���������� � ����
*/
uint32_t msToTick(uint32_t ms)
{
	return (uint16_t)(ms / (1000* SYS_TICK));
}

/*
�������������� �� ����� � �����������
*/
uint32_t tickToMs(uint32_t ticks)
{
	return (uint32_t)(ticks * 1000 * SYS_TICK);
}

/*
�������������� �� ������ � ���������� �������� ������
*/
uint32_t secondsToPeriod(uint32_t seconds)
{
	return (uint16_t)(seconds / (PERIOD_CYCLE));
}

/*
�������������� �� ���������� �������� ������ � �������
*/
uint32_t PeriodToSeconds(uint32_t period)
{
	return (uint32_t)(period * PERIOD_CYCLE);
}

/*
����� � ������������
*/
void delayMs(uint32_t ms)
{
	uint32_t start, end;
	start = HAL_GetTick();
	end=start+ms;
	while(end!=HAL_GetTick());
}

/*
������� �������� ������ � ������� ������� ������� ������� � CP1251 (������ strupr())
*/
char* strupr1251 (char * str1251) 
{
	uint8_t i=0;
	while(str1251[i]){
		if((str1251[i] >= 'a') && (str1251[i] <= 'z')){
			str1251[i] -=0x20;// ('a' - 'A'); 
		}
		else{
			if((str1251[i] >= '�') && (str1251[i] <= '�')){ //
				str1251[i] -= ('�' - '�'); 
			}
			else{
				if((str1251[i] == '�')){
					str1251[i] = '�';
				}
			}
		}		
		i++;
	}
	return (str1251);
}

