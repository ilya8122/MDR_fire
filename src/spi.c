/**
  ******************************************************************************
  * @file    spi.c
  * @author  BPV
  * @version V1.0.0
  * @date    05/03/2021
  * @brief    
  ******************************************************************************
  *
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_it.h"
#include "MDR32F9Qx_ssp.h"

#include "config.h"
#include "led.h"
#include "timer.h"
#include "spi.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  SPI_init. Подготовка оборудования к работе
  * @param  None
  * @retval None
  */
void SPI_init(void)
{
	SSP_InitTypeDef sSSP;
	PORT_InitTypeDef PORT_InitStructure;
	RST_CLK_FreqTypeDef RST_CLK_Clocks;
	uint32_t cpuclock;
	uint32_t SSP_BRG_divider = SSP_HCLKdiv2;
	uint32_t f_sspclk;
	uint32_t scrVal;

  /* Enable peripheral clocks --------------------------------------------------*/
  RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_SSP1),ENABLE);
  RST_CLK_PCLKcmd((RST_CLK_PCLK_PORTF), ENABLE);

  /* Reset PORTF settings */
//  PORT_DeInit(MDR_PORTF);

  /* Configure SSP1 pins: FSS, CLK, RXD, TXD */

  /* Configure PORTF pins 0, 1, 2, 3 */
  PORT_InitStructure.PORT_Pin   = (PORT_Pin_3);
  PORT_InitStructure.PORT_OE    = PORT_OE_IN;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
  PORT_Init(MDR_PORTF, &PORT_InitStructure);
  PORT_InitStructure.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1 /*| PORT_Pin_2*/);
  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_Init(MDR_PORTF, &PORT_InitStructure);

  /* Configure SPI_SS output*/
  RST_CLK_PCLKcmd((SPI_SS_EXT_FLASH_CLK_PORT), ENABLE);
  PORT_InitStructure.PORT_Pin   = (SPI_SS_EXT_FLASH_PIN);
  PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
  PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
  PORT_Init(SPI_SS_EXT_FLASH_PORT, &PORT_InitStructure);

  /* Reset all SSP settings */
  SSP_DeInit(MDR_SSP1);

	RST_CLK_GetClocksFreq(&RST_CLK_Clocks);
	cpuclock = RST_CLK_Clocks.CPU_CLK_Frequency;

	SSP_BRGInit(MDR_SSP1,SSP_BRG_divider); //F_SSPCLK = CPU_CLK / divider

  /* SSP1 MASTER configuration ------------------------------------------------*/
  SSP_StructInit (&sSSP);

  sSSP.SSP_CPSDVSR = 2;

	f_sspclk = cpuclock / (1<<SSP_BRG_divider);
	scrVal = ((f_sspclk / SPI_CLK_FREQ) / sSSP.SSP_CPSDVSR) - 1;
  sSSP.SSP_SCR  = scrVal;	//SPI_SPEED = F_SSPCLK / (CPSDVR * (1 + SCR))

	sSSP.SSP_Mode = SSP_ModeMaster;
  sSSP.SSP_WordLength = SSP_WordLength8b; 
  sSSP.SSP_SPH = SSP_SPH_1Edge;
  sSSP.SSP_SPO = SSP_SPO_Low;
  sSSP.SSP_FRF = SSP_FRF_SPI_Motorola;
  sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_None;	//SSP_HardwareFlowControl_SSE;
  SSP_Init (MDR_SSP1,&sSSP);

  /* Enable SSP1 DMA Rx and Tx request */
//  SSP_DMACmd(MDR_SSP1,(SSP_DMA_RXE | SSP_DMA_TXE), ENABLE);

  /* Enable SSP1 */
  SSP_Cmd(MDR_SSP1, ENABLE);

}

/*
HAL_StatusTypeDef HAL_SPI_TransmitReceive (SPI_HandleTypeDef * hspi, uint8_t * pTxData, uint8_t * pRxData, uint16_t Size, uint32_t Timeout);
*/
HAL_StatusTypeDef HAL_SPI_TransmitReceive(void* hspi, uint8_t * pTxData, uint8_t * pRxData, uint16_t Size, uint32_t Timeout)
{
	uint32_t	startTime=HAL_GetTick();
	for(int i=0; i<Size; i++){
    /* Wait for SPI1 Tx buffer empty */
    while (SSP_GetFlagStatus((MDR_SSP_TypeDef *)hspi, SSP_FLAG_TFE) == RESET)
    {
			if(HAL_GetTick()-startTime > Timeout){
				return HAL_TIMEOUT;
			}
    }
    /* Send SPI1 data */
    SSP_SendData((MDR_SSP_TypeDef *)hspi, pTxData[i]);
    /* Wait for SPI1 data reception */
    while (SSP_GetFlagStatus((MDR_SSP_TypeDef *)hspi, SSP_FLAG_RNE) == RESET)
    {
			if(HAL_GetTick()-startTime > Timeout){
				return HAL_TIMEOUT;
			}
    }
    /* Read SPI1 received data */
    pRxData[i] = SSP_ReceiveData((MDR_SSP_TypeDef *)hspi);
	}
	return HAL_OK;
}

/*
HAL_StatusTypeDef HAL_SPI_Receive (SPI_HandleTypeDef * hspi, uint8_t * pData, uint16_t Size, uint32_t Timeout);
*/
HAL_StatusTypeDef HAL_SPI_Receive(void* hspi, uint8_t * pRxData, uint16_t Size, uint32_t Timeout)
{
	uint32_t	startTime=HAL_GetTick();
	for(int i=0; i<Size; i++){
    /* Wait for SPI1 Tx buffer empty */
    while (SSP_GetFlagStatus((MDR_SSP_TypeDef *)hspi, SSP_FLAG_TFE) == RESET)
    {
			if(HAL_GetTick()-startTime > Timeout){
				return HAL_TIMEOUT;
			}
    }
    /* Send SPI1 data */
    SSP_SendData((MDR_SSP_TypeDef *)hspi, 0);
    /* Wait for SPI1 data reception */
    while (SSP_GetFlagStatus((MDR_SSP_TypeDef *)hspi, SSP_FLAG_RNE) == RESET)
    {
			if(HAL_GetTick()-startTime > Timeout){
				return HAL_TIMEOUT;
			}
    }
    /* Read SPI1 received data */
    pRxData[i] = SSP_ReceiveData((MDR_SSP_TypeDef *)hspi);
	}
	return HAL_OK;
}

/*
HAL_StatusTypeDef HAL_SPI_Transmit (SPI_HandleTypeDef * hspi, uint8_t * pData, uint16_t Size, uint32_t Timeout);
*/
HAL_StatusTypeDef HAL_SPI_Transmit(void* hspi, uint8_t * pTxData, uint16_t Size, uint32_t Timeout)
{
	uint32_t	startTime=HAL_GetTick();
	for(int i=0; i<Size; i++){
    /* Wait for SPI1 Tx buffer empty */
    while (SSP_GetFlagStatus((MDR_SSP_TypeDef *)hspi, SSP_FLAG_TFE) == RESET)
    {
			if(HAL_GetTick()-startTime > Timeout){
				return HAL_TIMEOUT;
			}
    }
    /* Send SPI1 data */
    SSP_SendData((MDR_SSP_TypeDef *)hspi, pTxData[i]);
    /* Wait for SPI1 data reception */
    while (SSP_GetFlagStatus((MDR_SSP_TypeDef *)hspi, SSP_FLAG_RNE) == RESET)
    {
			if(HAL_GetTick()-startTime > Timeout){
				return HAL_TIMEOUT;
			}
    }
	}
	return HAL_OK;
}
