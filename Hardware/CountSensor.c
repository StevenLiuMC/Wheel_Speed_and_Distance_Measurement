#include "stm32f10x.h"

float CountSensor_Count_Dis;
uint16_t CountSensor_Count_Speed;

void CountSensor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);  //配置AFIO外部中断引脚选择
	
	EXTI_InitTypeDef EXTI_InitStructure;  //配置外部中断，第14线路开中断，下降沿触发
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	//这里不得不采用双边触发，因为单边触发会因硬件自身问题造成错误触发
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //中断通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //响应优先级
	NVIC_Init(&NVIC_InitStructure);  //中断函数根据不同通道有不同的函数，详细参考startup文件	
}

uint16_t CountSensor_Get_Dis(void)
{
	return CountSensor_Count_Dis;
}

float CountSensor_Get_Speed(void)  //获取单位时间1s内下降沿的次数，计算平均频率
{
	return CountSensor_Count_Speed;
}
/*
void Clear_Speed(void)  //清除速度频率计次
{
	CountSensor_Count_Speed = 0;
}
*/
void EXTI15_10_IRQHandler(void)  //中断函数不需要调用，自动执行，因此无需声明
{
	if (EXTI_GetITStatus(EXTI_Line14) == SET)  //判断到底是不是14路进来的中断
	{
		//中断程序结束后一定要调用清除中断函数标志位的函数，否则就会一直触发中断
		CountSensor_Count_Dis = CountSensor_Count_Dis + 0.5;
		//CountSensor_Count_Speed++;  //用于测频法测速
		EXTI_ClearITPendingBit(EXTI_Line14);   //清除中断标志位
	}
}
