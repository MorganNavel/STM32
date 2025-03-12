#include <stdbool.h>
#include <stm32f0xx_usart.h>
#include "fifo.h"
#include "sd_uart.h"
#include "io.h"
extern me_sd_t USART1_sd;

void usartInit()
{
    USART_ClockInitTypeDef USART_ClockInitStruct;
    USART_InitTypeDef USART_InitStruct;

    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStruct);

    USART_ClockStructInit(&USART_ClockInitStruct);
    USART_ClockInit(USART1, &USART_ClockInitStruct);

    USART1->CR1 |= USART_CR1_RXNEIE; // Enable Receive Buffer Not Empty Interrupt
    USART1->CR1 |= USART_CR1_TXEIE;  // Enable Transmit Buffer Empty Interrupt
    USART1->CR1 |= USART_CR1_UE;

    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 3);
}

void sendString(USART_TypeDef *UARTx, char *str)
{
    while (*str != '\0')
    {
        while (!(UARTx->ISR & USART_ISR_TXE))
            ;

        USART_SendData(UARTx, (uint16_t)(*str));
        str++;
    }
}
char recvChar(USART_TypeDef *UARTx)
{
    while (!(UARTx->ISR & USART_ISR_RXNE))
        ;
    return UARTx->RDR;
}

bool recvCharNB(USART_TypeDef *UARTx, char *pC)
{
    bool ret;
    if (UARTx->ISR & USART_ISR_RXNE)
    {
        *pC = UARTx->RDR;
        ret = true;
    }
    else
        ret = false;

    return ret;
}

void ME_USART1_onRcv()
{
    uint8_t c = USART1->RDR;
    me_sd_uart_ctx_t *ctx = (me_sd_uart_ctx_t *)USART1_sd.ctx;

    if (ME_FifoCanWrite(ctx->rx, 1))
    {
        ME_FifoWrite(ctx->rx, c);
    }
}
void ME_USART1_onTXE()
{
    me_sd_uart_ctx_t *ctx = (me_sd_uart_ctx_t *)USART1_sd.ctx;
    if (ME_FifoCanRead(ctx->tx, 1))
    {
        ctx->UARTx->TDR = ME_FifoRead(ctx->tx);
    }
    else
    {
        ctx->is_transmitting = false;
        ctx->UARTx->CR1 &= ~USART_CR1_TXEIE;
    }
}

void USART1_IRQHandler()
{
    if (USART1->ISR & USART_ISR_RXNE)
    {
        ME_USART1_onRcv();
    }
    if (USART1->ISR & USART_ISR_TXE)
    {
        ME_USART1_onTXE();
    }
}