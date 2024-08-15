/*
 * @Description:AD9834驱动实现文件
 * @Author: 小登王 
 * @Date: 2022-07-03 15:41:32
 * @LastEditTime: 2024-05-30 02:54:35
 * @LastEditors: 小登王 
 * @Reference:康威电子
 */
 /**********************************************************
 功能：75MHz时钟， AD9834正弦波点频输出，范围0-30M(可编辑0-100M)，
            方波：50k-5M，三角波：0-10M，扫频默认正弦波 。
 接口：控制接口请参照AD9834.h
 时间：2022/10/08
 版本：0.9
 作者：康威电子（原作者），MALossov（修改），小登王（修改） 

//用的HAL库软件SPI，写的比较规范，转成HAL库硬件的SPI也方便的很。
//软件SPI的针脚定义和控制针脚定义在 .h头文件中自行修改就行。
//具体可看init函数来看引脚应该如何配置 
//但是其实功能发掘的还不算特别好，比如：初相设置直接置为0：AD9834_Write_16Bits(0XC000); 
//更多的直接看芯片的datasheet吧，一起放一个中文版的到仓库里了。



////使用这个库的启动代码如下：
//
//HAL_Delay(1000); //等待电平恢复稳定
//AD9834_Init();
//AD9834_Select_Wave(Sine_Wave);//Sine_Wave Square_Wave Triangle_Wave
//AD9834_Set_Freq(FREQ_0, 1000);
// //后续直接选波即可，这里使用的是频率寄存器0



//进阶使用
//使用AD9834时，一般情况下用于混频出波。按照以上配置即可。
//
//如果需要出调制波。尽量选择20M以下的信号，超过20M之后，极容易出现问题。
//
//在调制时，可以通过以下两种方式改变调制信号，当然首先需要设置好频率寄存器：
//
//使用控制位Sine_Wave和Sine_Wave_Reg1来切换两种正弦波实现FSK。
//
//写入控制位：SelectByPin之后，使用相对应的引脚控制实现波形切换。
//
//如果需要对DAC进行关断，而碰巧使用了康威的板子。
//
//直接使用控制位写入：0x2040，之后使用控制位开启。
//
//在其他情况下可以使用SLEEP引脚控制
//
//确保熟悉芯片手册。



////进行FSK/ASK进行调制的示例代码
////注意：在输出20Mhz以上的波形时，Vpp下降的幅度趋势极快。
//
////---初始化---//
////首先进行初始化
//HAL_Delay(1000);    //电平初始化
//AD9834_Init();    //拉低相关引脚
////设置基本的波形
//AD9834_Set_Freq(FREQ_0, 20000000);
//AD9834_Set_Freq(FREQ_1, 23000000);
////开启通过外部引脚控制波形寄存器
//AD9834_Select_Wave(SelectByPin);
//
////---循环体---//
////使用引脚切换两个频率寄存器，进行实现FSK2的实现
//AD9834_FS_SET();    //使用频率寄存器1
//AD9834_FS_CLR();    //使用频率寄存器0
//
////使用引脚切换DAC开关断状态，实现ASK
////因为使用康威的板子，无法关断DAC，直接使能RST引脚
//AD9834_RST_SET();    //关断波形输出
//AD9834_RST_CLR();    //开启波形输出
////在开关波形时，输出电平为结束波形输出时电平，可能需要滤直流
 **********************************************************/
#include "AD9834.h" 

 /****************************************************************
 函数名称: AD9834_Write_16Bits
 功    能: 向AD9834写入16为数据
 参    数: data --  要写入的16位数据
 返回值  : 无
 *****************************************************************/
//这就是软件模拟spi的函数，其他配置都是调用这个函数 
//往从机发送数据单向过程，没有往主机发送数据的过程 
void AD9834_Write_16Bits(unsigned int data)
{
    unsigned char i = 0;

    AD9834_SCLK_SET;
    AD9834_FSYNC_CLR;

    for (i = 0;i < 16;i++)
    {
        if (data & 0x8000)
            AD9834_SDATA_SET;
        else
            AD9834_SDATA_CLR;

        AD9834_SCLK_CLR;
        data <<= 1;
        AD9834_SCLK_SET;
    }
    AD9834_SDATA_SET;
    AD9834_FSYNC_SET;
}

/***********************************************************************************
函数名称：AD9834_Select_Wave
功    能：写控制位控制，
    --------------------------------------------------
    IOUT正弦波 ，SIGNBITOUT方波 ，写FREQREG0 ，写PHASE0
    ad9834_write_16bit(0x2028)   一次性写FREQREG0
    ad9834_write_16bit(0x0038)   单独改写FREQREG0的LSB
    ad9834_write_16bit(0x1038)   单独改写FREQREG0的MSB
    --------------------------------------------------
    IOUT三角波 ，写PHASE0
    ad9834_write_16bit(0x2002)   一次性写FREQREG0
    ad9834_write_16bit(0x0002)   单独改写FREQREG0的LSB
    ad9834_write_16bit(0x1008)   单独改写FREQREG0的MSB
参    数：initdata -- 要输入的命令
返回值  ：无
注    意： 大部分情况下，只需要写入一次，除非要改变波形类型。
          同样可以通过改变控制位来改变波形类型，但是这样会导致频率和相位的改变。
************************************************************************************/
void AD9834_Select_Wave(unsigned int initdata)

{

    AD9834_FSYNC_SET;
    AD9834_SCLK_SET;
    AD9834_RESET_SET;
    AD9834_RESET_SET;
    AD9834_RESET_CLR;

    AD9834_Write_16Bits(initdata);

}

/****************************************************************

函数名称: Init_AD9834

功    能: 初始化后将FS和PS引脚设置为低。
参    数: 无

返回值  : 无

*****************************************************************/
// B6 FS  B5 PS    B7 FSY   B8 SCK   B9 SDA B10 RST 

void AD9834_Init()
{
    GPIO_ResetBits(AD9834_Control_Port, AD9834_FS);
    GPIO_ResetBits(AD9834_Control_Port, AD9834_PS);
}

/****************************************************************

函数名称: AD9834_Set_Freq

功    能: 设置频率值

参    数: freq_number -- 要写入的平率寄存器(FREQ_0或FREQ_1)
          freq -- 频率值，直接填写赫兹，换算公式为： (Freq_value(value)=Freq_data(data)*FCLK/2^28)
                                                //此时FCLK=75MHz，所以Freq_value=Freq_data*75/2^28

返回值  : 无

*****************************************************************/
void AD9834_Set_Freq(unsigned char freq_number, unsigned long freq)
{
    unsigned long FREQREG = (unsigned long)(268435456.0 / AD9834_SYSTEM_COLCK * freq);
    unsigned int FREQREG_LSB_14BIT = (unsigned int)FREQREG;
    unsigned int FREQREG_MSB_14BIT = (unsigned int)(FREQREG >> 14);

    if (freq_number == FREQ_0)
    {
        FREQREG_LSB_14BIT &= ~(1U << 15);
        FREQREG_LSB_14BIT |= 1 << 14;
        FREQREG_MSB_14BIT &= ~(1U << 15);
        FREQREG_MSB_14BIT |= 1 << 14;
    }
    else
    {
        FREQREG_LSB_14BIT &= ~(1 << 14);
        FREQREG_LSB_14BIT |= 1U << 15;
        FREQREG_MSB_14BIT &= ~(1 << 14);
        FREQREG_MSB_14BIT |= 1U << 15;
    }
    AD9834_Write_16Bits(FREQREG_LSB_14BIT);
    AD9834_Write_16Bits(FREQREG_MSB_14BIT);
}
//============================================================// 
/*   注：使用时先调用初始化函数初始化IO配置，然后调用AD9834_Select_Wave()选择（基本的三种波形为频率寄存器0）
波形，最后选择频率值即可输出。 */
