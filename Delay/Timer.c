#include "stm32f10x.h"                  // Device header
#include "CountSensor.h"

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //使能引脚时钟
	
	TIM_InternalClockConfig(TIM2);  //选择时基单元的时钟
	
	//接下来配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //为滤波检测频率指定时钟分频，与时基单元本身关系并不大
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数器模式，这里选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;  //ARR自动重装器的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;  //预分频器的值，这里设置可以让定时器定时1s
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;  //重复计数器，只有高级计数器才用，这里直接给0
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);  //防止刚初始化完就进中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  //使能时钟中断，这里是选择更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //NVIC优先级分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //选择定时器2在NVIC中的通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);  //启动定时器2
}

/*
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
*/
