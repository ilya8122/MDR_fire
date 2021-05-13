/**
  ******************************************************************************
  * @file    adc.c
  * @author  BPV
  * @version V1.0.0
  * @date    25/02/2021
  * @brief    
  ******************************************************************************
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_adc.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_dma.h"
#include "MDR32F9Qx_it.h"

#include "config.h"
#include "adc.h"
#include "led.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
PORT_InitTypeDef PORT_InitStructure;
ADC_InitTypeDef sADC;
ADCx_InitTypeDef sADCx;
uint16_t ADCConvertedValue[10];
DMA_ChannelInitTypeDef DMA_InitStr;
DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;
DMA_CtrlDataInitTypeDef DMA_AltCtrlStr;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  adc_init. Подготовка оборудования к работе
  * @param  None
  * @retval None
  */
void adc_init(void)
{
  /* Enable peripheral clocks --------------------------------------------------*/
  RST_CLK_PCLKcmd((RST_CLK_PCLK_DMA | RST_CLK_PCLK_ADC),ENABLE);
#if 0
	/* DMA Configuration */
  /* Reset all settings */
  DMA_DeInit();
  DMA_StructInit(&DMA_InitStr);
  /* Set Primary Control Data */
  DMA_PriCtrlStr.DMA_SourceBaseAddr = (uint32_t)(&(MDR_ADC->ADC1_RESULT));
  DMA_PriCtrlStr.DMA_DestBaseAddr = (uint32_t)ADCConvertedValue;
  DMA_PriCtrlStr.DMA_SourceIncSize = DMA_SourceIncNo;
  DMA_PriCtrlStr.DMA_DestIncSize = DMA_DestIncHalfword;
  DMA_PriCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_PriCtrlStr.DMA_Mode = DMA_Mode_PingPong;
  DMA_PriCtrlStr.DMA_CycleSize = 10;
  DMA_PriCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
  DMA_PriCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;
  DMA_PriCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;

  /* Set Alternate Control Data */
  DMA_AltCtrlStr.DMA_SourceBaseAddr = (uint32_t)(&(MDR_ADC->ADC1_RESULT));
  DMA_AltCtrlStr.DMA_DestBaseAddr   = (uint32_t)ADCConvertedValue;
  DMA_AltCtrlStr.DMA_SourceIncSize = DMA_SourceIncNo;
  DMA_AltCtrlStr.DMA_DestIncSize = DMA_DestIncHalfword;
  DMA_AltCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_AltCtrlStr.DMA_Mode = DMA_Mode_PingPong;
  DMA_AltCtrlStr.DMA_CycleSize = 10;
  DMA_AltCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
  DMA_AltCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;
  DMA_AltCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;

  /* Set Channel Structure */
  DMA_InitStr.DMA_PriCtrlData = &DMA_PriCtrlStr;
  DMA_InitStr.DMA_AltCtrlData = &DMA_AltCtrlStr;
  DMA_InitStr.DMA_Priority = DMA_Priority_Default;
  DMA_InitStr.DMA_UseBurst = DMA_BurstClear;
  DMA_InitStr.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;

  /* Init DMA channel ADC1 */
  DMA_Init(DMA_Channel_ADC1, &DMA_InitStr);

  /* Enable dma_req or dma_sreq to generate DMA request */
  MDR_DMA->CHNL_REQ_MASK_CLR = (1<<DMA_Channel_ADC1);
  MDR_DMA->CHNL_USEBURST_CLR = (1<<DMA_Channel_ADC1);

  /* Enable DMA channel ADC1 */
  DMA_Cmd(DMA_Channel_ADC1, ENABLE);
#endif

  /* ADC Configuration */
  /* Reset all ADC settings */
  ADC_DeInit();
  ADC_StructInit(&sADC);
/*
  ADC_InitStruct->ADC_StartDelay        	= 0; //позволяет задать задержку между запусками АЦП1 и АЦП2
  ADC_InitStruct->ADC_TempSensor        	= ADC_TEMP_SENSOR_Disable; //запрет работы температурного датчика и источника опорного напряжения (бит TS_EN)
  ADC_InitStruct->ADC_TempSensorAmplifier   = ADC_TEMP_SENSOR_AMPLIFIER_Disable; //запрет работы выходного усилителя (бит TS_BUF)
  ADC_InitStruct->ADC_TempSensorConversion  = ADC_TEMP_SENSOR_CONVERSION_Disable; //запрет выбора датчика температуры (бит SEL_TS)
  ADC_InitStruct->ADC_IntVRefConversion     = ADC_VREF_CONVERSION_Disable; //запрет выбора источника опорного напряжения для оцифровки (бит SEL_VREF для 1986ВЕ9x значение 1.23 В)
  ADC_InitStruct->ADC_IntVRefTrimming     	= 0; //задавая значения от 0 до 7, можно в небольших пределах подстроить 
*/
  sADC.ADC_SynchronousMode      = ADC_SyncMode_Independent;
  sADC.ADC_StartDelay           = 1; //Задержка между началом преобразования ADC1 и ADC2 при последовательном переборе либо работе на один канал
  sADC.ADC_TempSensor           = ADC_TEMP_SENSOR_Enable;
  sADC.ADC_TempSensorAmplifier  = ADC_TEMP_SENSOR_AMPLIFIER_Enable;
  sADC.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Enable;
  sADC.ADC_IntVRefConversion    = ADC_VREF_CONVERSION_Enable;//ADC_VREF_CONVERSION_Disable;
  sADC.ADC_IntVRefTrimming      = 1;
  ADC_Init (&sADC);

  /* ADC1 Configuration */
/*
	sADCx.ADC_ClockSource      = ADC_CLOCK_SOURCE_CPU; //выбор источника тактирования, частота ядра
  sADCx.ADC_SamplingMode     = ADC_SAMPLING_MODE_CICLIC_CONV; //режим многократного преобразования
  sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable; // автоматическое переключение каналов
  sADCx.ADC_ChannelNumber    = ADC_CH_ADC7; //выбор номера канала
  sADCx.ADC_Channels         = 0; //количество используемых каналов, если включен перебор ADC_CH_SWITCHING_Enable
  sADCx.ADC_LevelControl     = ADC_LEVEL_CONTROL_Enable; // контроль уровня входного сигнала
  sADCx.ADC_LowLevel         = L_Level;//нижний уровень контроля
  sADCx.ADC_HighLevel        = H_Level; // верхний уровень
  sADCx.ADC_VRefSource       = ADC_VREF_SOURCE_INTERNAL; //выбор внутреннего источника опорного напряжения (от AUсс и AGND)
  sADCx.ADC_IntVRefSource    = ADC_INT_VREF_SOURCE_INEXACT; //вид источника для датчика опорного напряжения
  sADCx.ADC_Prescaler        = ADC_CLK_div_32768; //выбор делителя тактовой частоты
  sADCx.ADC_DelayGo          = 0xF; //значение задержки перед началом следующего преобразования
*/
	ADCx_StructInit (&sADCx);
  sADCx.ADC_ClockSource      = ADC_CLOCK_SOURCE_CPU;
  sADCx.ADC_SamplingMode     = ADC_SAMPLING_MODE_SINGLE_CONV;
  sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
  sADCx.ADC_ChannelNumber    = ADC_CH_TEMP_SENSOR;
  sADCx.ADC_Channels         = 0;
  sADCx.ADC_LevelControl     = ADC_LEVEL_CONTROL_Disable;
  sADCx.ADC_LowLevel         = 0;
  sADCx.ADC_HighLevel        = 0;
  sADCx.ADC_VRefSource       = ADC_VREF_SOURCE_INTERNAL;
  sADCx.ADC_IntVRefSource    = ADC_INT_VREF_SOURCE_EXACT;//ADC_INT_VREF_SOURCE_INEXACT;
  sADCx.ADC_Prescaler        = ADC_CLK_div_8;
  sADCx.ADC_DelayGo          = 7;
  ADC1_Init (&sADCx);

  /* Enable ADC1 EOCIF and AWOIFEN interupts */
  ADC1_ITConfig((ADCx_IT_END_OF_CONVERSION  | ADCx_IT_OUT_OF_RANGE), DISABLE);

  /* ADC1 enable */
  ADC1_Cmd (ENABLE);

  /* Enable DMA IRQ */
//  NVIC_EnableIRQ(DMA_IRQn);
}

/**
  * @brief  Get value from analog channel (with start and wait end of ADC conversion).
	* @param  channel number (example: ADC_CH_ADC2)
  * @retval 12 bit value of ADC
  */
uint16_t adc_getVal(uint32_t channel)
{
  ADC1_SetChannel(channel);
	ADC1_Start();
	while(ADC_GetFlagStatus(ADC1_FLAG_END_OF_CONVERSION) != SET);
	return (uint16_t) ADC1_GetResult();
}

float adc_getTemp(void)
{
	uint16_t adc_value;
	float temperature_C;
  ADC1_SetChannel(ADC_CH_TEMP_SENSOR);
	ADC1_Start();
	while(ADC_GetFlagStatus(ADC1_FLAG_END_OF_CONVERSION) != SET);
	adc_value=(uint16_t)ADC1_GetResult();
// расчёт в int
	temperature_C = (adc_value - FACTORY_ADC_TEMP25)/FACTORY_ADC_AVG_SLOPE + FACTORY_TEMP25;
// я предпочитаю вот так (расчёты во float):
//temperature_C = ((Vtemp - Vtemp25) / Avg_Slope) + FACTORY_TEMP25;
	return temperature_C;
}
