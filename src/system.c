#include <stm32f0xx.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_gpio.h>
#include "stm32f0xx.h"
#include "stm32f0xx_flash.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_misc.h"
#include "system_stm32f0xx.h"
#include "stm32f0xx_iwdg.h"
#include <stm32f0xx_usart.h>
#include "system.h"
#include "io.h"
#include "usart.h"

volatile uint64_t meTime = 0;

//-----------------------------------------------------------------------------------------
void System_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	IO_Init();
	usartInit();

	SysTick_Config(SystemCoreClock / 1000);

	meTime = 0;
}

//-----------------------------------------------------------------------------------------
void SysTick_Handler(void) // 1 kHz
{
	meTime++;
}

uint32_t GiveTime()
{
	return meTime;
}
uint32_t GiveTimeSec()
{
	return meTime / 1000;
}
