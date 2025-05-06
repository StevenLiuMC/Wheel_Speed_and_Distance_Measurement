#include "stm32f10x.h"                  // Device header

void IC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  //GPIOA_6读入方波信号
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1; //ARR，这里设置为上限，尽量防止计数溢出，到达65535重装为0
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36000 - 1;		//PSC，确定PSC后可以确定内部标准时钟，
	//标准时钟每经过一个单位时间，便会使CNT中的值+1，72MHz/PSC，便是计数器CNT自增的频率。之后定时器每次
	//捕获边沿信号触发中断便可以向CCR送出CNT的值，从而确定一个方波的频率或者说周期。现在PSC设置为35999，
	//可以实现0.0005s的标准时钟，CNT*0.0005S便是脉冲波的周期.目前可以做到保持32s无脉冲不重置
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;  //指定配置哪个通道，这里选择TIM3的CH1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;   //滤波器参数0x0~0xF，针对目前的低频信号，越大滤波效果越好
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  //A口6号引脚输入的方波信号下降沿触发
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  //触发信号分频器，目前选择不分频，每次触发均有效
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  //选择触发信号从哪个引脚输入，这里选择直连
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);  //触发源选择
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);  //从模式设置，这里设置为自动清零CNT
	
	TIM_Cmd(TIM3, ENABLE);
}

float IC_GetFreq(void)
{
	//Freq = Fc / N(CCR的值)
	return 2000 / TIM_GetCapture1(TIM3);  //输入捕获模式下CCR只读，需调用函数读出，目前Fc为2000Hz
}
