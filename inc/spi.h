#ifndef __SPI_H
#define __SPI_H

#include "MDR32F9Qx_ssp.h"
#include "MDR32F9Qx_port.h"
#include <stdint.h>

/**
  * @brief  HAL Status structures definition
  */
typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;


#define SPI_CLK_FREQ								(2000000)	 // bitrate

#define SPI_SS_EXT_FLASH_PORT     	MDR_PORTF
#define SPI_SS_EXT_FLASH_CLK_PORT		RST_CLK_PCLK_PORTF
#define SPI_SS_EXT_FLASH_PIN        PORT_Pin_2

#define SPI_SS_EXT_FLASH_SELECT()  PORT_ResetBits(SPI_SS_EXT_FLASH_PORT, SPI_SS_EXT_FLASH_PIN)
#define SPI_SS_EXT_FLASH_DESELECT()  PORT_SetBits(SPI_SS_EXT_FLASH_PORT, SPI_SS_EXT_FLASH_PIN)

void SPI_init(void);
HAL_StatusTypeDef HAL_SPI_TransmitReceive(void* hspi, uint8_t * pTxData, uint8_t * pRxData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Transmit(void* hspi, uint8_t * pTxData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Receive(void* hspi, uint8_t * pRxData, uint16_t Size, uint32_t Timeout);
#endif /* __SPI_H */
