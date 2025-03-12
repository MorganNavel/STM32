#ifndef __IO_H
#define __IO_H

#include "stm32f0xx_gpio.h"

void IO_Init();

#define IODESC(p, m, t, pp) {p, m, t, pp, 0}
#define IODESCAF(p, t, pp, af) {p, GPIO_Mode_AF, t, pp, af}
#define IODESC_END() {NULL, 0, 0, 0, 0}

#define MEPIN(_port, _pin) _port, _pin

#define MEPIN_SET(_p_) MEPIN_SET_(_p_)
#define MEPIN_SET_(_port, _pin) _port->BSRR = (1 << _pin)

#define MEPIN_CLR(_p_) MEPIN_CLR_(_p_)
#define MEPIN_CLR_(_port, _pin) _port->BRR = (1 << _pin)

#define MEPIN_SWAP(_p_) MEPIN_SWAP_(_p_)
#define MEPIN_SWAP_(_port, _pin) _port->BSRR = MEPIN_GET_(_port, _pin) ? (1 << (_pin + 16)) : 1 << (_pin)

#define MEPIN_GET(_p_) MEPIN_GET_(_p_)
#define MEPIN_GET_(_port, _pin) (_port->IDR & (1 << _pin))

#define MEPIN_GET_PIN_MASK(_p_) MEPIN_GET_PIN_MASK_(_p_)
#define MEPIN_GET_PIN_MASK_(_port, _pin) (1 << _pin)

#define MEPIN_GET_PIN_GPIO(_p_) MEPIN_GET_PIN_GPIO_(_p_)
#define MEPIN_GET_PIN_GPIO_(_port, _pin) (_port)

// /////////////////////////////////////////////////////////////////////

#define PIN_LED0 MEPIN(GPIOB, 12)
#define PIN_UART1_TX MEPIN(GPIOA, 9)
#define PIN_UART1_RX MEPIN(GPIOA, 10)

#endif
