/**
  ******************************************************************************
  * @file    player.c
  * @author  BPV
  * @version V1.0.0
  * @date    12/02/2021
  * @brief   В Миландр описание DMA просто ужасное! 
  ******************************************************************************
  *
  * Нюансы работы трудноуловимы. Документация тяжелая... Пример DAC+DMA составлен неверно 
	* (обработчик прерывания переинициализирует DMA, а в описании указано по другому) и сильно упрощенно. 
	* Ограничения: 
	* максимальный размер буфера DMA = 1024
	* Буфер должен быть только в ОЗУ
	* В режиме пинг-понг в прерывании или по флагу нужно грузить альтернативную конфигурацию, если флаг DMA_FLAG_CHNL_ALT == RESET! (вероятно из-за того, что DMA уже переключился на второй буфер!)
	* при неверной инициализации канала в прерывании процессор сваливается в постоянные запросы DMA, убивая систему.
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_dac.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_dma.h"
#include "MDR32F9Qx_it.h"

#include "config.h"
#include "player.h"
#include "led.h"

extern uint16_t play_nextBlockCallback(uint16_t* pBuf, uint16_t maxLen);

/* Private typedef -----------------------------------------------------------*/
typedef enum {
	DMA_NO_REQ,
	DMA_PRI_REQ,
	DMA_ALT_REQ,
} dmaReq_e;

/* Private define ------------------------------------------------------------*/
#define AUDIOBUF_LEN	(1024)

/* Private macro -------------------------------------------------------------*/
#define DMA_SELECT(CHANNEL)	(0x00000001<<(CHANNEL))

/* Private variables ---------------------------------------------------------*/
static PORT_InitTypeDef PORT_InitStructure;
static TIMER_CntInitTypeDef sTIM_CntInit;
static DMA_ChannelInitTypeDef DMA_InitStr;
static DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;
static DMA_CtrlDataInitTypeDef DMA_AltCtrlStr;
/*static uint16_t Sine12bit[32] = {
                     2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                     3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                     599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};
*/
// DMA work with RAM ONLY!
static uint16_t audioBuf1[AUDIOBUF_LEN];
static uint16_t audioBuf2[AUDIOBUF_LEN];

static volatile dmaReq_e dmaReq=DMA_NO_REQ;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  HW_init. Подготовка оборудования к работе
  * @param  None
  * @retval None
  */
static void HW_init(void)
{
//  RST_CLK_DeInit();
//  RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);
  /* Enable peripheral clocks --------------------------------------------------*/
  RST_CLK_PCLKcmd((RST_CLK_PCLK_DMA | RST_CLK_PCLK_PORTE),ENABLE);
  RST_CLK_PCLKcmd((RST_CLK_PCLK_TIMER1 | RST_CLK_PCLK_DAC),ENABLE);
  RST_CLK_PCLKcmd((RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_SSP2),ENABLE);

  /* Disable all interrupt */
//  NVIC->ICPR[0] = 0xFFFFFFFF;
//  NVIC->ICER[0] = 0xFFFFFFFF;
  NVIC_DisableIRQ(DMA_IRQn);

  /* Reset PORTE settings */
//  PORT_DeInit(MDR_PORTE);

  /* Configure DAC pin: DAC2_OUT */
  /* Configure PORTE pin 0 */
  PORT_InitStructure.PORT_Pin   = PORT_Pin_0;
  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
  PORT_Init(MDR_PORTE, &PORT_InitStructure);

  /* DAC Configuration */
  /* Reset all DAC settings */
//  DAC_DeInit();
  /* DAC channel2 Configuration */
  DAC2_Init(DAC2_AVCC);
}

/**
  * @brief  HW_stop. Останов оборудования
  * @param  None
  * @retval None
  */
static void HW_stop(void)
{
  /* TIMER1 disable*/
  TIMER_Cmd(MDR_TIMER1,DISABLE);
  
  /* Disable interrupt */
  NVIC_DisableIRQ(DMA_IRQn);

  /* Reset all DAC settings */
//  DAC2_Cmd(DISABLE);	//comment for prevent DAC out set to 0!!!
  /* Disable peripheral clocks --------------------------------------------------*/
//  RST_CLK_PCLKcmd((RST_CLK_PCLK_DMA ),DISABLE);
  RST_CLK_PCLKcmd((RST_CLK_PCLK_TIMER1 | RST_CLK_PCLK_DAC),DISABLE);
}

/**
	* @brief  DMA_IRQCallback. Обработчик прерывания DMA. Подготовка следующего блока данных
  * @param  None
  * @retval None
  */
void DMA_IRQCallback(void)
{
	uint16_t cnt=AUDIOBUF_LEN;
  /* Reconfigure the inactive DMA data structure*/
  if(DMA_GetFlagStatus(DMA_Channel_TIM1, DMA_FLAG_CHNL_ALT) == SET)	//primary? Не укладывается в голове! установлен Флаг использования альтернативной конфигурации! См. заголовок.
  {
		LEDOn(LED1);
#ifndef DMA_BUFFER_UPDATE_IN_ISR
		dmaReq = DMA_PRI_REQ;
#else
		cnt = play_nextBlockCallback(audioBuf1, AUDIOBUF_LEN);
		LEDOff(LED1);
#endif
		if(cnt){
			DMA_PriCtrlStr.DMA_CycleSize = cnt;
			DMA_CtrlInit(DMA_Channel_TIM1, DMA_CTRL_DATA_PRIMARY, &DMA_PriCtrlStr);
		}
  }
  else // alternate
  {
		LEDOn(LED2);
#ifndef DMA_BUFFER_UPDATE_IN_ISR
		dmaReq = DMA_ALT_REQ;
#else
		cnt = play_nextBlockCallback(audioBuf2, AUDIOBUF_LEN);
		LEDOff(LED2);
#endif
		if(cnt){
			DMA_AltCtrlStr.DMA_CycleSize = cnt;
			DMA_CtrlInit(DMA_Channel_TIM1, DMA_CTRL_DATA_ALTERNATE, &DMA_AltCtrlStr);	//need to update registers for DMA to start automatically
		}
  }
	if(!cnt){
		HW_stop();
	}
}

/**
	* @brief  DMA_IRQCallback. Обработчик прерывания DMA. Подготовка следующего блока данных
  * @param  None
  * @retval None
  */
static void dmaBufRefresh(bool primary)
{
	uint16_t cnt;
	if(primary){
		cnt = play_nextBlockCallback(audioBuf1, AUDIOBUF_LEN);
		if(cnt){
			DMA_PriCtrlStr.DMA_CycleSize = cnt;
			DMA_CtrlInit(DMA_Channel_TIM1, DMA_CTRL_DATA_PRIMARY, &DMA_PriCtrlStr);
		}
	}else{
		cnt = play_nextBlockCallback(audioBuf2, AUDIOBUF_LEN);
		if(cnt){
			DMA_AltCtrlStr.DMA_CycleSize = cnt;
			DMA_CtrlInit(DMA_Channel_TIM1, DMA_CTRL_DATA_ALTERNATE, &DMA_AltCtrlStr);	//need to update registers for DMA to start automatically
		}
	}
	if(!cnt){
		HW_stop();
	}
}

void dmaReqProcessing(void)
{
	if(dmaReq == DMA_PRI_REQ){
		dmaBufRefresh(true);
		LEDOff(LED1);
		dmaReq=DMA_NO_REQ;
	}
	if(dmaReq == DMA_ALT_REQ){
		dmaBufRefresh(false);
		LEDOff(LED2);
		dmaReq=DMA_NO_REQ;
	}
}

/**
  * @brief  play_init.
  * @param  None
  * @retval None
  */
void play_init(void)
{
	HW_init(); 
}

/**
  * @brief  play_start.
  * @param  None
  * @retval None
  */
void play_start(void)
{
	uint16_t cnt;
	uint32_t cpuclock;
	RST_CLK_FreqTypeDef RST_CLK_Clocks;

  DMA_DeInit();
  DMA_StructInit(&DMA_InitStr);
  RST_CLK_PCLKcmd((RST_CLK_PCLK_TIMER1 | RST_CLK_PCLK_DAC),ENABLE);

	cnt = play_nextBlockCallback(audioBuf1, AUDIOBUF_LEN);

	/* Set Primary Control Data */
	DMA_PriCtrlStr.DMA_SourceBaseAddr = (uint32_t)audioBuf1;
  DMA_PriCtrlStr.DMA_DestBaseAddr = (uint32_t)(&(MDR_DAC->DAC2_DATA));
  DMA_PriCtrlStr.DMA_SourceIncSize = DMA_SourceIncHalfword;
  DMA_PriCtrlStr.DMA_DestIncSize = DMA_DestIncNo;
  DMA_PriCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_PriCtrlStr.DMA_Mode = DMA_Mode_PingPong;
  DMA_PriCtrlStr.DMA_CycleSize = cnt;
  DMA_PriCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
  DMA_PriCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;
  DMA_PriCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;

	cnt = play_nextBlockCallback(audioBuf2, AUDIOBUF_LEN);
  /* Set Alternate Control Data */
  DMA_AltCtrlStr.DMA_SourceBaseAddr = (uint32_t)audioBuf2;
  DMA_AltCtrlStr.DMA_DestBaseAddr   = (uint32_t)(&(MDR_DAC->DAC2_DATA));
  DMA_AltCtrlStr.DMA_SourceIncSize = DMA_SourceIncHalfword;
  DMA_AltCtrlStr.DMA_DestIncSize = DMA_DestIncNo;
  DMA_AltCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_AltCtrlStr.DMA_Mode = DMA_Mode_PingPong;
  DMA_AltCtrlStr.DMA_CycleSize = cnt;
  DMA_AltCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
  DMA_AltCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;
  DMA_AltCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;
  /* Set Channel Structure */
  DMA_InitStr.DMA_PriCtrlData = &DMA_PriCtrlStr;
  DMA_InitStr.DMA_AltCtrlData = &DMA_AltCtrlStr;
  DMA_InitStr.DMA_Priority = DMA_Priority_Default;
  DMA_InitStr.DMA_UseBurst = DMA_BurstClear;
  DMA_InitStr.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;

  /* Init DMA channel TIM1*/
  DMA_Init(DMA_Channel_TIM1, &DMA_InitStr);

  /* Enable dma_req or dma_sreq to generate DMA request */
  MDR_DMA->CHNL_REQ_MASK_CLR = DMA_SELECT(DMA_Channel_TIM1);
  MDR_DMA->CHNL_USEBURST_CLR = DMA_SELECT(DMA_Channel_TIM1);

  /* Enable DMA_Channel_TIM1 */
  DMA_Cmd(DMA_Channel_TIM1, ENABLE);

  /* DAC channel2 Configuration */
//  DAC2_Init(DAC2_AVCC);
  /* DAC channel2 enable */
  DAC2_Cmd(ENABLE);

  /* TIMER1 Configuration */
	RST_CLK_GetClocksFreq(&RST_CLK_Clocks);
	cpuclock = RST_CLK_Clocks.CPU_CLK_Frequency;

  /* Time base configuration */
  TIMER_DeInit(MDR_TIMER1);
  TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);
  sTIM_CntInit.TIMER_Prescaler                = 0;
  sTIM_CntInit.TIMER_Period                   = cpuclock/DAC_SAMPLE_RATE;
  sTIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
  sTIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
  sTIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;
  sTIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;
  sTIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
  sTIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
  sTIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
  sTIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
  sTIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
  TIMER_CntInit (MDR_TIMER1,&sTIM_CntInit);

  /* Enable DMA for TIMER1 */
  TIMER_DMACmd(MDR_TIMER1,(TIMER_STATUS_CNT_ARR), ENABLE);

  /* TIMER1 enable counter */
  TIMER_Cmd(MDR_TIMER1,ENABLE);

  /* Enable DMA IRQ */
  NVIC_EnableIRQ(DMA_IRQn);
}

/**
  * @brief  play_stop.
  * @param  None
  * @retval None
  */
void play_stop(void)
{
	HW_stop();
}


