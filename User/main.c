#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include "Delay.h"
#include "LED.h"
#include "KEY.h"
#include "Buzzer.h"
#include "OLED.h"
#include "CountSensor.h"
#include "Timer.h"
#include "IC.h"
#include "Serial.h"


uint8_t KeyNumber;  //按键判断
uint16_t Runtime_s;  //运行时间
uint16_t Runtime_min;
uint16_t Runtime_h;
//int8_t Speed_CCR;
float Speed;  //速度
uint8_t AlarmValue_Num;  //报警速度值
char AlarmValue_Str[4];  //报警速度值字符串

void Warning(void)   //声光报警函数
{
	Buzzer_ON();
	LED1_ON();
	Delay_ms(250);
	Buzzer_OFF();
	LED1_OFF();
	Delay_ms(250);
}

void ShowTitle(void)  //OLED标题显示
{
	OLED_Init();
	OLED_ShowString(1,1,"AlarmVal:");
	OLED_ShowString(2,1,"Speed:");
	OLED_ShowString(2,13,"mpm");
	OLED_ShowString(3,1,"Distance:");
	OLED_ShowString(3,16,"m");
	OLED_ShowString(4,1,"Runtime:");
	OLED_ShowChar(4,11,':');
	OLED_ShowChar(4,14,':');
}

float Distance_Cal(void)  //行驶距离计算
{
	float distance;
	distance = 0.1*CountSensor_Get_Dis();  //这里调用对射式红外传感器计次信号，实际上是外部中断EXTI
	return distance;
}

float Speed_Cal(void)  //速度计算
{
	float T = 1.0/IC_GetFreq();
	Speed = (0.1/T)*59.9;  //测频率函数存在误差，会把频率测高大约0.2%，因此这里作出修正
	if(Speed < 0.1)
	{
		Speed = 0;
	}
	return Speed;
}

void Show_Info_TTL(void)  //串口通信上位机显示
{
	Serial_SendString("Distance:");  //均为以文本模式发送
	Serial_SendFNumber(Distance_Cal());
	Serial_SendString("m\r\n");
	Serial_SendString("Speed:");
	Serial_SendFNumber(Speed_Cal());
	Serial_SendString("m/min\r\n");
}

void Hex_Dec_Convert(uint8_t HexNum)  //16进制数字转换为十进制字符串
{
	uint8_t i = 0;
	char Data_Hex[] = " ";
	sprintf(Data_Hex,"%3d",HexNum);
	for(i=0;i<4;i++)
	{
		AlarmValue_Str[i] = Data_Hex[i];
	}
}

int main(void)
{
	LED_Init();
	KeyInit();
	Buzzer_Init();
	ShowTitle();
	CountSensor_Init();
	Timer_Init();
	//Motor_Init();  //包含了PWM初始化程序，实物演示不需要驱动电机
	IC_Init();
	Serial_Init();
	//Serial_SendByte(0x41);
	//Serial_SendByte('A');  //串口检测
	//Serial_SendFNumber(Speed_Cal());
	
	while(1)
	{
		if(Serial_GetRxFlag() == 1)  //报警值修改及显示, 目前功能有限，仅能设置0~255
		{
			AlarmValue_Num = Serial_GetRxData();
			Hex_Dec_Convert(AlarmValue_Num);
			//Serial_SendByte(AlarmValue);  //串口回传数据检测
			OLED_ShowString(1,11,AlarmValue_Str);
		}
		OLED_ShowFNum(3,10,Distance_Cal());
		OLED_ShowFNum(2,7,Speed_Cal());
		KeyNumber = Key_GetNum();  //更改电机转速
		if(KeyNumber == 1)
		{
			Show_Info_TTL();  //向上位机发送数据，按键按一下单片机向上位机发送一次数据
		}
		//OLED_ShowNum(2,7,IC_GetFreq(),2);
		//OLED_ShowNum(2,7,TIM_GetCapture1(TIM3),6);  //定时器CCR数据检测
		//OLED_ShowFNum(2,7,CountSensor_Get_Speed(),6); //中断嵌套检测
		OLED_ShowNum(4,15,Runtime_s,2);
		OLED_ShowNum(4,12,Runtime_min,2);
		OLED_ShowNum(4,9,Runtime_h,2);
		
		if(Speed_Cal() > AlarmValue_Num)
		{
			Warning();
		}
		
/*		KeyNumber = Key_GetNum();  //更改电机转速
		if(KeyNumber == 1)
		{
			Speed_CCR += 20;
			if (Speed_CCR > 100)
			{
				Speed_CCR = 0;  //这里为了安全起见不设置speed为负数了，不使电机反转
			}
			Motor_SetSpeed(Speed_CCR);
		}
*/	
	}
}

void TIM2_IRQHandler(void)  //定时器2中断函数，此中断用于运行时间计时
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Runtime_s++;
		if (Runtime_s>59)
		{
			Runtime_s = 0;
			Runtime_min++;
			if (Runtime_min>59)
			{
				Runtime_min = 0;
				Runtime_h++;
			}
		}
		//Clear_Speed();  //每次触发1s定时中断，就清除速度计次，属于测频法测速度，1s定时下精度太差
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
