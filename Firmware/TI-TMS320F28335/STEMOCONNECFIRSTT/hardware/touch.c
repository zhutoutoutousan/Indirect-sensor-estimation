/*
 * touch.c
 *
 *  Created on: 2016年5月8日
 *      Author: Administrator
 */
#include"touch.h"
#include "dsp28x_project.h"

#define YC 160
#define XC 120
int x[5]={0,0,0,0,0};
int y[5]={0,0,0,0,0};
int XLC,YLC;
float KY,KX;

//默认为touchtype=0的数据.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;

struct struct1 coordinate;        //定义一个名为Tp_pix的结构变量,用于存放触摸点采样结果

uint  lx,ly;                      //根据采集到的参数转换为TFT上的实际坐标值

void Init_Touch_Port()
{
	EALLOW;
	GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;	/* output */
	EDIS;   //TFT显示控制线
	EALLOW;
	GpioCtrlRegs.GPBPUD.bit.GPIO49 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;	/* output */
	EDIS;
	EALLOW;
	GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO54= 1;	/* output */
	EDIS;
	EALLOW;
	GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;
	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO17 = 1;	/* output */
	EDIS;
	EALLOW;
	GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;//input
	GpioDataRegs.GPADAT.bit.GPIO13= 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0;//input
	GpioDataRegs.GPBDAT.bit.GPIO55= 0;
	EDIS;

}

void Init_Touch()
{
  Init_Touch_Port();
  DSP28x_usDelay(1);
  TPCLK_CLR;
  TPCS_SET;
  TPDI_SET;
  TPCLK_SET;
  TPCS_CLR;
}

//================================================================================================
//	实现功能：读取触摸点X轴和Y轴电压值
//	返回参数：pix 读取到的触摸点电压值
//================================================================================================
struct struct1 AD_Touch()
{
  struct struct1 pix;
  TPCS_CLR;

  Write_Touch(0x90);                 //送控制字 10010000 即用差分方式读X坐标，详细请见有关资料
  TPCLK_SET;
  DSP28x_usDelay(1);
  TPCLK_CLR;
  DSP28x_usDelay(1);
  pix.y=Read_Touch();

  Write_Touch(0xD0);                 //送控制字 11010000 即用差分方式读Y坐标 详细请见有关资料
  TPCLK_SET;
  DSP28x_usDelay(1);
  TPCLK_CLR;
  DSP28x_usDelay(1);
  pix.x=Read_Touch();

  TPCS_SET;
  return pix;
}

//================================================================================================
//	实现功能：	写8位命令到触摸控制IC
//	输入参数：  temp 需要写入的8位控制命令
//================================================================================================
void Write_Touch(uchar temp)                         //SPI写8位命令到触摸控制IC
{
  uchar i=0;

  for(i=0;i<8;i++)                                    //循环8次写入一字节
  {
    if(temp&0x80)
      TPDI_SET;
    else
      TPDI_CLR;    //判断最高位是否为1,为1则向数据位写1
    TPCLK_CLR;
    DSP28x_usDelay(1);                //送一个脉冲，上升沿有效，将DIN位数据送入到IC
    TPCLK_SET;
    DSP28x_usDelay(1);
    temp<<=1;                           //待写数据左移1位，准备好写下一位数据
  }
}

//================================================================================================
//	实现功能：	从触摸控制IC读8位数据到控制器
//	返回参数：  temp 需要写入的8位控制命令
//================================================================================================
uint Read_Touch()                          //SPI 读数据
{
  uchar i=0;
  uint temp=0;

  for(i=0;i<12;i++)                         //循环12次读取12位结果
  {
    temp<<=1;                                  //temp左移一位，准备读取下一位
    TPCLK_SET;
    DSP28x_usDelay(1);                   //下降沿有效
    TPCLK_CLR;
    DSP28x_usDelay(1);
    if(TPDOUT)
      temp++;                     //判断控制IC送出的一位数据是否为1,如果为1,赋给temp的最低位
  }
  return(temp);                                 //返回结果
}

//================================================================================================
//	实现功能：软件滤波，滤掉波动过大的采样点
//	返回参数：flag 采集数据是否有效标志,flag=1;则数据有效
//================================================================================================
uchar pix_filter(struct struct1 pix1,struct struct1 pix2)
{
  uchar flag=0;
  int x=pix1.x>pix2.x?pix1.x-pix2.x:pix2.x-pix1.x;  //X轴两次采样绝对值
  int y=pix1.y>pix2.y?pix1.y-pix2.y:pix2.y-pix1.y;  //Y轴两次采样绝对值
  if(x<10&&y<10)	                                  //软件滤波，2次取样的值相差太大的视为噪声
  {
    flag=1;
    coordinate.x=(pix1.x+pix2.x)/2;           //求两次采样平均值
    coordinate.y=(pix1.y+pix2.y)/2;
  }
  return flag;
}


//////////////////////////////////////////////////////////////////////////////////
//与LCD部分有关的函数
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);//横线
	LCD_DrawLine(x,y-12,x,y+13);//竖线
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6);//画中心圈
}
//画一个大点(2*2的点)
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//中心点
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);
}

void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//清屏
 	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
  	POINT_COLOR=RED;//设置画笔蓝色
}
/****************************************************************************
* 名    称：void LCD_Adjustd(void)
* 功    能：校正电阻屏系数
* 入口参数：	null
* 出口参数：无
* 说    明：null
* 调用方法：LCD_Adjustd();
****************************************************************************/
u8 TP_Adjustd(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	LCD_ShowString(80,140,200,16,16,"Adjustd Begin");
	Delay_ms(5000);
	Delay_ms(5000);
	//第一个点
	TP_Drow_Touch_Point(20, 20, RED);
	while(Adjustd_index(0));
	Delay_ms(5000);
	Delay_ms(5000);
	Delay_ms(5000);
	// 定第二个点
	TP_Drow_Touch_Point(220, 20, RED);
	TP_Drow_Touch_Point(20, 20, WHITE);
	while(Adjustd_index(1));
	Delay_ms(5000);
	Delay_ms(5000);
	Delay_ms(5000);
	if(abs(y[1]-y[0]) >300)
	{
		LCD_Clear(WHITE);
		POINT_COLOR=RED;
		LCD_ShowString(80,140,200,16,16,"Adjustd Fail");
		Delay_ms(5000);
		LCD_Clear(WHITE);
		return 1;
	}
	Delay_ms(5000);
	Delay_ms(5000);
	Delay_ms(5000);
	// 定第三个点
	TP_Drow_Touch_Point(20, 300, RED);
	TP_Drow_Touch_Point(220, 20, WHITE);
	while(Adjustd_index(2));
	Delay_ms(5000);
	Delay_ms(5000);
	Delay_ms(5000);
	if(abs(x[2]-x[0]) >200)
	{
		LCD_Clear(WHITE);
		LCD_ShowString(80,140,200,16,16,"Adjustd Fail");
		Delay_ms(5000);
		Delay_ms(5000);
		Delay_ms(5000);
		Delay_ms(5000);
		LCD_Clear(WHITE);
		return 1;
	}
	// 定第四个点
	TP_Drow_Touch_Point(220, 300, RED);
	TP_Drow_Touch_Point(20, 300, WHITE);
	while(Adjustd_index(3));
	Delay_ms(5000);
	Delay_ms(5000);
	Delay_ms(5000);
	if((abs(y[2]-y[3]) >200) || (abs(x[1]-x[3]) >200))
	{
		LCD_Clear(WHITE);
		LCD_ShowString(80,140,200,16,16,"Adjustd Fail");
		Delay_ms(5000);
		Delay_ms(5000);
		Delay_ms(5000);
		Delay_ms(5000);
		LCD_Clear(WHITE);
		return 1;
	}
	// 定第五个点
	TP_Drow_Touch_Point(120, 160, RED);
	TP_Drow_Touch_Point(220, 300, WHITE);
	while(Adjustd_index(4));
	//计算校正系数
	Delay_ms(5000);
	Delay_ms(5000);
	Delay_ms(5000);
	KY  = (((float)(y[0]-y[2])/280+(float)(y[1]-y[3])/280)/2);
	KX  = (((float)(x[0]-x[1])/200+(float)(x[2]-x[3])/200)/2);
	YLC  = y[4];
	XLC  = x[4];
	// 定点完成
	LCD_Clear(WHITE);
	LCD_ShowString(80,140,200,16,16,"Adjustd Done");
	Delay_ms(5000);
	LCD_Fill(110,20,200,35, WHITE);
	LCD_ShowString(80,160,200,16,16,"Testing");
	return 0;
}
void test_touch()
{
	struct struct1 pix1;
	struct struct1 pix2;
	int lx,ly;
	if(PEN==0)			//触摸屏被按下
	{
	    pix1=AD_Touch();
	    pix2=AD_Touch();
	    if(pix_filter(pix1,pix2)==1) //得到当前TP的取样值，此处使用软件滤波，2次取样的值相差太大的视为噪声
	    {
			lx=(int)((int)((coordinate.x-XLC)/KX)+XC);
			lx=240-lx;
			ly=(int)((int)((coordinate.y-YLC)/KY)+YC);
			ly=320-ly;
	    }
	 	if(lx < lcddev.width && ly < lcddev.height)
		{
			if( lx >(lcddev.width-24) && ly <16)Load_Drow_Dialog();//清除
			else TP_Draw_Big_Point(lx,ly,RED);		//画图
		}
	}else Delay_ms(10);	//没有按键按下的时候

}

unsigned char Adjustd_index(unsigned char j)
{
	struct struct1 pix1;
	struct struct1 pix2;
	while(PEN);
	if(PEN==0)
	{
	    pix1=AD_Touch();
	    pix2=AD_Touch();
	    if(pix_filter(pix1,pix2)==1) //得到当前TP的取样值，此处使用软件滤波，2次取样的值相差太大的视为噪声
	    {
			x[j] = coordinate.x;
			y[j] = coordinate.y;
			LCD_ShowNum(150,80,x[j],4,16);
			LCD_ShowNum(150,110,y[j],4,16);
			Delay_ms(1000);
			LCD_Fill(150,80,200,130, WHITE);
			return 0;
	    }
	    else
	    	return 1;
	}
	else
		return 1;
}
