/*---------------------------------------------------------------------
输入状态扫描
效率：非常高，核心部分非常简单
所具备功能：单击，长按，长按连续响应，长按响应一次
注意：双击，三击功能只是一段时间内判断是否再次单击即可
作者：往事随风
----------------------------------------------------------------------*/
#include "KeyScan.h"
#include "main.h"
#include "cot_menu.h"

#define TimerGap		1		//定时调用间隔 可以是1ms,可以是10ms


//按键接口，我是51单片机可以直接这么写，32只要Read_GPIO函数返回即可
#define Key_One			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)		//按键1
#define Key_Two			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)		//按键2
#define Key_Thr			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)		//按键3
#define Key_Fou			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)		//按键4

#define DownTime_Init   (1000/TimerGap)		//初始长按时间，1s
#define Enter_Gap		(100/TimerGap)		//长按进入间隔,0.1s


uint16_t DownTime_Set[Key_Num] = 		//按下设定时间
{
	DownTime_Init,DownTime_Init,DownTime_Init,DownTime_Init,
};

uint16_t DownTime_Now[Key_Num] = 		//按下实时时间
{
	0,0,0,0,
};

/*---------------------最终标志----------------------*/
uint8_t Press_Short[Key_Num] =			//短按标志
{
	Key_Up,Key_Up,Key_Up,Key_Up,
};

uint8_t Press_Long[Key_Num]  = 			//长按标志
{
	Key_Up,Key_Up,Key_Up,Key_Up,
};


/*是否允许长按连续进入*/
uint8_t Continue_Enter[Key_Num] =
{
	Enter_Yes,Enter_Yes,Enter_Yes,Enter_Yes,
};




/*---------------------------------------------------------------------------
函数：Key_Scan
功能：按键扫描
使用方法：定时器调用，调用频率和TimerGap有关，要么1ms,要么10ms
----------------------------------------------------------------------------*/
void Key_Scan(void)
{
	uint8_t i=0;
	static uint8_t Press_Long_In[Key_Num] =		//内部记录长按
	{
		Key_Up,Key_Up,Key_Up,Key_Up,
	};
	uint8_t Key_State[Key_Num] = 		//记录按键状态
	{
		Key_Up,Key_Up,Key_Up,Key_Up,
	};
	/*--------------------扫描n个按键------------------------*/
	Key_State[0] = Key_One;			//注意按下为0，弹起为1
	Key_State[1] = Key_Two;
	Key_State[2] = Key_Thr;
	Key_State[3] = Key_Fou;

	/*-----------------------长短按算法------------------------*/
	for(i=0;i<4;i++)
	{
		if(Key_State[i] == Key_Down)		//按下
		{
			DownTime_Now[i]++;

			if(DownTime_Now[i]>(2500/TimerGap))			//防止溢出为，为什么是2500这个值和8位最大255有关，当设置成了8位计时变量，最大250
			{
				DownTime_Now[i] = DownTime_Init+10;		//每次溢出要设置成比目标时间大一些的值，不然会判定为短按
			}

			/*连续进入*/
			if(Continue_Enter[i] == Enter_Yes)			//是否允许连续进入
			{
				if(DownTime_Now[i] > DownTime_Set[i])		//长按
				{
					DownTime_Now[i] -= Enter_Gap;				//设置进入间隔，20x10ms=200ms,一直按着会每间隔200ms执行一次
													//因为每次进来减去20，又需要一定时间去填充
					//填入长按需要执行的操作
					Press_Long[i]    = Key_Down;			//长按按下标志
					Press_Long_In[i] = Key_Down;			//长按按下内部标志
				}

			}
		}
		else						//释放
		{
			//先判断是不是长按
			if(DownTime_Now[i] > DownTime_Set[i] || Press_Long_In[i] == Key_Down)		//时间达到或者有标志位
			{
				Press_Long_In[i] = Key_Up;		//Press_Long_In用于消除松手DownTime_Now<DownTime_Set后判断为短按
			}
			else if(DownTime_Now[i]>(20/TimerGap))		//短按,大于抖动时间，20ms
			{
				Press_Short[i] = Key_Down;		//短按标志按下
			}
			DownTime_Now[i] = 0;		//释放后时间清零
			Continue_Enter[i] = Enter_Yes;
		}
	}
}



uint8_t test = 0;

/*-------------------------------------------------------------------
函数：Key_Dispose
功能：按键处理
使用方法：主循环调用
-------------------------------------------------------------------*/
void Key_Dispose(void)
{
//	if(Press_Short[0] == Key_Down)
//	{
//		Press_Short[0] = Key_Up;	
//		test++;	
//		cotMenu_SelectPrevious(true);
//		
//	}
//	if(Press_Short[1] == Key_Down)
//	{
//		Press_Short[1] = Key_Up;		
//		test--;		
//		cotMenu_SelectNext(true);
//	}
//	if(Press_Long[0] == Key_Down)
//	{
//		Press_Long[0] = Key_Up;
//		test++;	
//		cotMenu_SelectPrevious(true);
//	}
//	if(Press_Long[1] == Key_Down)
//	{
//		Press_Long[1] = Key_Up;
//		test--;	
//		cotMenu_SelectNext(true);
//	}	
	/*
	判断最终标志即可
	Press_Short
	Press_Long

	例if(Press_Short[0] == Key_Down)
	{
		Press_Short[0] = Key_Up;

		//执行你要的操作
	}

	组合键怎么办？
	新建一个KeyStatus
	把状态保存进这个变量，最后统一判断


	KeyStatus |= 0x01;		//按键1长按
	KeyStatus |= 0x02;		//按键2长按
	KeyStatus |= 0x04;		//按键3长按
	KeyStatus |= 0x08;		//按键4长按

	//判断1和2键同时按下
	if(KeyStatus & 0x03 == 0x03)
	*/

}


