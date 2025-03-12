#ifndef __MESD_UART_H
#define __MESD_UART_H
#include <stm32f0xx.h>
#include "fifo.h"
#include "sd.h"

typedef struct
{
    bool is_transmitting;
    USART_TypeDef *UARTx;
    ME_Fifo *rx;
    ME_Fifo *tx;
} me_sd_uart_ctx_t;
void SD_USART_Init(me_sd_t *me_sd, me_sd_uart_ctx_t *ctx, ME_Fifo *rx, ME_Fifo *tx);

#endif // __MESD_UART_H