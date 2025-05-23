#include "stm32f10x.h"
#include "Delay.h"

void KeyInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1)  //读取外部输入的端口值，返回1/0
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1);
		Delay_ms(20);
		KeyNum = 1;
	}
	else
	{
		Delay_ms(20);
		KeyNum = 0;
	}
	return KeyNum;
}
