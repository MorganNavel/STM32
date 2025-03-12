#ifndef __USART_H
#define __USART_H

#include <stm32f0xx_usart.h>
#include <stdbool.h>
void usartInit();
void sendString(USART_TypeDef *UARTx, char *str);
char recvChar(USART_TypeDef *UARTx);
bool recvCharNB(USART_TypeDef *UARTx, char *pC);
void USART1_IRQHandler();

#endif //__USART_H