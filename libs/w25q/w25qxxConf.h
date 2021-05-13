#ifndef _W25QXXCONFIG_H
#define _W25QXXCONFIG_H

#include "spi.h"
#include "timer.h"

#define GPIO_PIN_RESET		0
#define GPIO_PIN_SET			1

#define _W25QXX_SPI                   MDR_SSP1
#define _W25QXX_CS_GPIO               SPI_SS_EXT_FLASH_PORT
#define _W25QXX_CS_PIN                SPI_SS_EXT_FLASH_PIN
#define _W25QXX_USE_FREERTOS          0
#define _W25QXX_DEBUG                 0

//#define HAL_GPIO_WritePin(port, pin, val)		if (val){ PORT_SetBits(port, pin);} else {PORT_ResetBits(port, pin);}
#define HAL_GPIO_WritePin(port, pin, val)		if (val){ SPI_SS_EXT_FLASH_DESELECT();} else {SPI_SS_EXT_FLASH_SELECT();}
#define HAL_Delay(delay)						delay_ms(delay)

#endif
