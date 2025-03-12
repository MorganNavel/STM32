#include "fifo.h"
#include "sd_uart.h"
#include "io.h"
void ME_USART_putc(void *pCtx, uint8_t c)
{
    me_sd_uart_ctx_t *ctx = (me_sd_uart_ctx_t *)pCtx;

    ctx->UARTx->CR1 &= ~USART_CR1_TXEIE;

    if (ctx->is_transmitting)
    {
        if (ME_FifoCanWrite(ctx->tx, 1))
        {
            ME_FifoWrite(ctx->tx, c);
        }
    }
    else
    {
        ctx->is_transmitting = true;
        ctx->UARTx->TDR = c;
    }

    ctx->UARTx->CR1 |= USART_CR1_TXEIE;
}
bool ME_SD_FifoCanRead(void *pCtx, int n)
{
    me_sd_uart_ctx_t *ctx = (me_sd_uart_ctx_t *)pCtx;
    return ME_FifoCanRead(ctx->rx, n);
}
bool ME_SD_FifoCanWrite(void *pCtx, int n)
{
    me_sd_uart_ctx_t *ctx = (me_sd_uart_ctx_t *)pCtx;
    return ME_FifoCanWrite(ctx->tx, n);
}
uint8_t ME_SD_FifoRead(void *pCtx)
{
    me_sd_uart_ctx_t *ctx = (me_sd_uart_ctx_t *)pCtx;
    return ME_FifoRead(ctx->rx);
}

void SD_USART_Init(me_sd_t *sd, me_sd_uart_ctx_t *ctx, ME_Fifo *rx, ME_Fifo *tx)
{
    static me_sd_ops_t ops = {
        .canRead = ME_SD_FifoCanRead,
        .canWrite = ME_SD_FifoCanWrite,
        .getc = ME_SD_FifoRead,
        .putc = ME_USART_putc};

    ctx->is_transmitting = false;
    ctx->tx = tx;
    ctx->rx = rx;
    ctx->UARTx = USART1;
    sd->ops = &ops;
    sd->ctx = ctx;
}
void FD_ME_Fifo_Init(ME_Fifo *fifo, uint8_t *buf, int size)
{
    fifo->buf = buf;
    fifo->mask = size - 1;
    fifo->rd = 0;
    fifo->wr = 0;
}