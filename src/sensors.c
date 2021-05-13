/**
  ******************************************************************************
  * File Name          : battery.c
  * Date               : 28/08/2019
  * Description        : 
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//#include "cfg_info.h"
#include "adc.h"
#include "channel.h"
#include "sensors.h"
#include "math.h"
#include "fire_sensor_module.h"
float ch0Voltage;
channelRd_s ch0;

typedef struct {
	channelRd_s ch;
	void (*pInputValueFunc) (void);		// обработчик
	void (*pChProcessingFunc) (channelRd_s*);		// обработчик
	int (*pDetectorFunc) (channelRd_s*, int param);		// обработчик
} sensor_t;

void ch0Init()
{
	calibrKoef_s ch0Calibr={0.0, 100.0, 0.0, 4.0};
	chSetLPF(&ch0, 0.95);
	chLpfReset(&ch0);
	chSetCalibr(&ch0, &ch0Calibr);
	chSetSimulType(&ch0, SIMUL_NONE);
}

 float getCh0Value(void)
	{
	float R;//Om
	float inV;
		
	inV=calculate_U(ADC_CH_ADC2);	

	R=calculate_resistance(inV);	

	reaction(R);

	return inV;
}

/*float getChargePercent(float voltage){
	float out = CH_lineScale(voltage, &ch0.calibr);
	if (out < BATT_SCALE_MIN) return BATT_SCALE_MIN;
	if (out > BATT_SCALE_MAX) return BATT_SCALE_MAX;
	return out;
}*/
