#include "main.h"
#include "io.h"

typedef struct
{
	GPIO_TypeDef *port;
	uint8_t n;
	uint8_t mode;
	uint8_t type;
	uint8_t pupd;
	uint8_t af;
} iodesc_t;

const iodesc_t ios[] = {
	IODESC(PIN_LED0, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP),
	IODESCAF(PIN_UART1_RX, GPIO_OType_PP, GPIO_PuPd_UP, 1),
	IODESCAF(PIN_UART1_TX, GPIO_OType_PP, GPIO_PuPd_UP, 1),
	IODESC_END()};

void IO_ApplyConf(const iodesc_t *pCnf)
{
	const iodesc_t *p;
	GPIO_InitTypeDef gpio;

	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;

	for (p = pCnf; p->port; ++p)
	{
		gpio.GPIO_Pin = 1 << (p->n);
		gpio.GPIO_Mode = p->mode;
		gpio.GPIO_OType = p->type;
		gpio.GPIO_PuPd = p->pupd;
		GPIO_Init(p->port, &gpio);

		if (p->mode == GPIO_Mode_AF)
		{
			GPIO_PinAFConfig(p->port, p->n, p->af);
		}
	}
}

void IO_Init()
{
	IO_ApplyConf(ios);
}
