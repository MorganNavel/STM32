#include "stm32f0xx.h"
#include "system.h"
#include "io.h"
#include "me_timer.h"
#include "system.h"
#include "me_timer.h"
#include "tiny_printf.h"
#include "fifo.h"
#include "usart.h"
#include "sd_uart.h"
#include "console.h"
DEFINE_FIFO(rx, 256)
DEFINE_FIFO(tx, 512)
DEFINE_BUF_CONSOLE(c1)
DEFINE_ESC_SEQ(c1)
me_sd_t USART1_sd;
me_sd_uart_ctx_t USART1_ctx;
console_ctx_t console_ctx;
int main(void)
{
	ME_Timer esc_timer;
	console_ctx.esc_timer = &esc_timer;
	SD_USART_Init(&USART1_sd, &USART1_ctx, &rx, &tx);
	ME_Console_Init(&USART1_sd, &console_ctx, c1_console_buff, c1_seq_buff);
	System_Init();
	MEPIN_SET(PIN_LED0);
	ME_Timer myTimer;
	ME_timerInit(&myTimer, 1000);
	while (1)
	{
		if (ME_isTimedOut(&myTimer))
		{
			ME_timerInit(&myTimer, 1000);
			MEPIN_SWAP(PIN_LED0);
		}
		ME_Console_Poll(&console_ctx);
	}
}

void HardFault_Handler()
{
	__ASM volatile("BKPT #01	\n");
}
