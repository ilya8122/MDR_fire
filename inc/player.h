#ifndef __PLAYER_H
#define __PLAYER_H

#include "MDR32F9Qx_dma.h"
#include <stdbool.h>

#define DMA_BUFFER_UPDATE_IN_ISR	// включает режим обновления буфера (вызов play_nextBlockCallback()) в обработчике прерывания

void DMA_IRQCallback(void);

void play_init(void);
void play_start(void);
void play_stop(void);
void dmaReqProcessing(void); // нужно вызывать не реже 1 раза в мс, если не используется режим DMA_BUFFER_UPDATE_IN_ISR

#endif /* __PLAYER_H */
