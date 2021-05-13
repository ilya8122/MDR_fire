#ifndef __ADC_H
#define __ADC_H

#include "MDR32F9Qx_dma.h"
#include "MDR32F9Qx_adc.h"
#include <stdbool.h>

#define FACTORY_ADC_TEMP25		1700		// ADC value = 1700 @ 25C = 1.36996V - from milandr demo project
#define FACTORY_VTEMP25			1.36996	// 1.38393 @ 26C. 1.34-1.52, 1.43 V typical @ factory delta_calib
#define FACTORY_ADC_AVG_SLOPE		6		// ADC delta value @ 1C, from milandr demo project
#define FACTORY_AVG_SLOPE		0.004835	// 4.0-4.6, 4.3 mV/C typical @ factory delta_calib

#define FACTORY_TEMP25		(0)

#define VREF_MV	(3300.0)	// Voltage for max ADC value
#define VREF_INT	(1230.0)	// Voltage for internal reference
#define	ADC_MAX	(4095.0)


void adc_init(void);
uint16_t adc_getVal(uint32_t channel);
float adc_getTemp(void);

#endif /* __ADC_H */
