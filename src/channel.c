/*
*/

#include <stdint.h>
#include <string.h> 
#include "channel.h" 


/*=======================================================================*/
/*
преобразование значения канала в соответствие с калибровкой
*/
static float CH_lineScale(float in, calibrKoef_s* koef)
{
	float a = (koef->scaledMax - koef->scaledMin) / (koef->inputMax-koef->inputMin);
	float b = koef->scaledMin - (koef->inputMin * a);
	return in * a + b;
}

static float CH_LPF(float in, LPF_s* lpf)
{
	// При сбросе инициализируем выход по входному значению
	if(lpf->reset){
		lpf->out = in;
		lpf->reset = false;
	}else{
		lpf->out = in * (1-lpf->WLag) + lpf->WLag * lpf->out;
	}
	return lpf->out;
}
		
void chSetLPF(channelRd_s* ch, float wlag)
{
	ch->lpf.WLag = wlag;
}

/*
Установка  канала (обычно в мВ - с АЦП)
*/
void chSetCalibr(channelRd_s* ch, calibrKoef_s* calibr)
{
	ch->calibr=*calibr;
}

/*
Установка рабочего входа канала (обычно в мВ - с АЦП)
*/
void chSetInputRAW(channelRd_s* ch, float val)
{
	ch->in=val;
}

/*
Установка входа 1 симуляции канала (обычно в мВ)
*/
void chSetSimulInRAW(channelRd_s* ch, float val)
{
	ch->in_simul=val;
}

/*
Установка входа 2 симуляции канала (в измеряемых единицах канала - сопротивление, температура и т.д.)
*/
void chSetSimulIN(channelRd_s* ch, float val)
{
	ch->out_simul=val;
}

/*
Установка типа симуляции канала
*/
void chSetSimulType(channelRd_s* ch, simulType_s simulType)
{
	if(ch->simulType!=simulType){	// при смене типа симуляции сбрасываем фильтр
		ch->lpf.reset=1;
	}
	ch->simulType=simulType;
}

/*
Пересчет канала
*/
void chProcessing(channelRd_s* ch)
{
	// определим значение выхода канала без фильтрации и калибровкой с учетом типа симуляции
	switch(ch->simulType){
		case (SIMUL_NONE):
			ch->out=CH_lineScale(ch->in, &ch->calibr);
			break;
		case (SIMUL_INPUT):
			ch->out=CH_lineScale(ch->in_simul, &ch->calibr);
			break;
		case (SIMUL_OUTPUT):
			ch->out=ch->out_simul;
			break;
	}
	
	// рассчитаем сглаженное (фильтрованное) значение канала 
	ch->lpf.out=CH_LPF(ch->out, &ch->lpf);
}

/*
Чтение выходного значения канала без фильтрации (в измеряемых единицах канала - сопротивление, температура и т.д.)
*/
float chGetOut(channelRd_s* ch)
{
	return ch->out;
}

/*
Чтение выходного значения канала после фильтрации (в измеряемых единицах канала - сопротивление, температура и т.д.)
*/
float chGetOutLpf(channelRd_s* ch)
{
	return ch->lpf.out;
}

/*
Сброс фильтра выходного значения канала (позволяет сразу присвоить на выход значение входа без сглаживания. Например при детектировании подключения датчика)
*/
void chLpfReset(channelRd_s* ch)
{
	ch->lpf.reset=1;
}
