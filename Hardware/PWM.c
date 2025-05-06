#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出，让片上外设（TIM3的CH1通道）来控制引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  //查表TIM3_CH1对应PA6
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;		//ARR. 分辨率=1/(ARR+1)，到达99重置为0
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;		//PSC，这里为了规避电机发出蜂鸣器响声，加大PWM
	//波频率，为了不影响占空比，因此只改变预分频器的值. Freq=CK_PSC/(PSC+1)/(ARR+1)
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);  //为结构体赋默认初始值，防止因结构体成员未赋初值导致错误
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  //设置输出比较模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //设置输出比较极性，有效电平为高电平，输出电平不翻转
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //设置输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;		//设置CCR(捕获比较寄存器),Duty=CCR/(ARR+1)
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据不同的GPIO通道选择不同的OCx
	
	TIM_Cmd(TIM3, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)  //对应CH1更改CCR的值，从而间接调节PWM波的占空比
{										//因为这里(ARR+1)的上限我们设定的是100，因此CCR取值范围0~100
	//如果(ARR+1)上限为1000甚至10000，那么CCR取值范围便也可以跟着变化，调节精度也会更高。目前是1%精度
	TIM_SetCompare1(TIM3, Compare);
}

void PWM_SetPrescaler(uint16_t Prescaler)  //设定PSC子函数,可以改变PWM波的频率，而不影响占空比
{
	TIM_PrescalerConfig(TIM3, Prescaler, TIM_PSCReloadMode_Update);
}
