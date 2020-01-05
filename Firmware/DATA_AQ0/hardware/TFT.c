/*
 * TFT.c
 *
 *  Created on: 2016年5月7日
 *      Author: Administrator
 */
#include "dsp28x_project.h"
#include "TFT.h"
#include "Ascii_8x16.h"
#include "hanzi.h"
_lcd_dev lcddev;
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色
void Delay_ms(ms)
{
	while(ms--)
	{
		DSP28x_usDelay(1000);
	}
}
void initLcdXint(void)
{
#if 0
  	// All Zones---------------------------------
    // Timing for all zones based on XTIMCLK = 1/2 SYSCLKOUT
    EALLOW;
    XintfRegs.XINTCNF2.bit.XTIMCLK = 1;
    // No write buffering
    XintfRegs.XINTCNF2.bit.WRBUFF = 0;
    // XCLKOUT is enabled
    XintfRegs.XINTCNF2.bit.CLKOFF = 1;
    // XCLKOUT = XTIMCLK/2
    XintfRegs.XINTCNF2.bit.CLKMODE = 1;

  	/* Zone 0 ---------------------------------------------------------------*/
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING0.bit.XWRLEAD = 3;
    XintfRegs.XTIMING0.bit.XWRACTIVE = 7;
    XintfRegs.XTIMING0.bit.XWRTRAIL = 3;
    // Zone read timing
    XintfRegs.XTIMING0.bit.XRDLEAD = 3;
    XintfRegs.XTIMING0.bit.XRDACTIVE = 7;
    XintfRegs.XTIMING0.bit.XRDTRAIL = 3;

    // double all Zone read/write lead/active/trail timing
    XintfRegs.XTIMING0.bit.X2TIMING = 1;

    // Zone will NOT sample XREADY signal
    XintfRegs.XTIMING0.bit.USEREADY = 0;
   	XintfRegs.XTIMING0.bit.READYMODE = 0;  // sample asynchronous

    // Size must be either:
    // 0,1 = x32 or
    // 1,1 = x16 other values are reserved
    XintfRegs.XTIMING0.bit.XSIZE = 3;

    EDIS;

    asm(" RPT #7 || NOP");
#endif

    EALLOW;
        // All Zones---------------------------------
        // Timing for all zones based on XTIMCLK = SYSCLKOUT
        XintfRegs.XINTCNF2.bit.XTIMCLK =1;
        // Buffer up to 3 writes
        XintfRegs.XINTCNF2.bit.WRBUFF = 0;
        // XCLKOUT is enabled
        XintfRegs.XINTCNF2.bit.CLKOFF = 1;
        // XCLKOUT = XTIMCLK
        XintfRegs.XINTCNF2.bit.CLKMODE = 1;

        // Zone 7------------------------------------
        // When using ready, ACTIVE must be 1 or greater
        // Lead must always be 1 or greater
        // Zone write timing
        XintfRegs.XTIMING7.bit.XWRLEAD = 3;
        XintfRegs.XTIMING7.bit.XWRACTIVE = 7;
        XintfRegs.XTIMING7.bit.XWRTRAIL = 3;
        // Zone read timing
        XintfRegs.XTIMING7.bit.XRDLEAD = 3;
        XintfRegs.XTIMING7.bit.XRDACTIVE = 7;
        XintfRegs.XTIMING7.bit.XRDTRAIL = 3;

        // don't double all Zone read/write lead/active/trail timing
        XintfRegs.XTIMING7.bit.X2TIMING = 1;

        // Zone will not sample XREADY signal
        XintfRegs.XTIMING7.bit.USEREADY = 0;
        XintfRegs.XTIMING7.bit.READYMODE = 0;

        // 1,1 = x16 data bus
        // 0,1 = x32 data bus
        // other values are reserved
        XintfRegs.XTIMING7.bit.XSIZE = 3;

        XintfRegs.XBANK.bit.BANK = 0;
        XintfRegs.XBANK.bit.BCYC = 3;
        EDIS;

       //Force a pipeline flush to ensure that the write to
       //the last register configured occurs before returning.
       asm(" RPT #7 || NOP");
}/* end of lcdFSMCConfig() */

u16 LCD_RD_DATA(void)
{
	u16 t;
	DSP28x_usDelay(10);	/* delay 10 us */
	t= LCD_Data_Port->LCD_RAM;
	DSP28x_usDelay(10);	/* delay 10 us */
    return t;
}
//写寄存器
//LCD_Reg:寄存器编号
//LCD_RegValue:要写入的值
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{

	LCD_WR_REG(LCD_Reg);
	LCD_Data_Port->LCD_RAM =LCD_RegValue;
	DSP28x_usDelay(10);	/* delay 10 us */
}

void LCD_WR_REG(u16 LCD_Reg)
{
	LCD_Data_Port->LCD_REG=LCD_Reg;
	DSP28x_usDelay(10);	/* delay 10 us */
}

//读寄存器
//LCD_Reg:寄存器编号
//返回值:读到的值
u16 LCD_ReadReg(u16 LCD_Reg)
{
	LCD_Data_Port->LCD_REG=LCD_Reg;//写入要读的寄存器号
	DSP28x_usDelay(1);
	return LCD_RD_DATA();
}
//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
	DSP28x_usDelay(1);	/* delay 1 us */
}
//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{
	LCD_WR_DATA(RGB_Code);//写十六位GRAM
	DSP28x_usDelay(1);	/* delay 1 us */
}

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;
	rgb=(b<<11)+(g<<5)+(r<<0);
	return(rgb);
}

void LCD_WR_DATA(data)
{
	LCD_Data_Port->LCD_RAM =data;
	DSP28x_usDelay(1);	/* delay 1 us */
}

void LCD_WR_DATAX(u16 data)
{
	LCD_WR_DATA(data);
	DSP28x_usDelay(1);	/* delay 1 us */
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏其实就是调转x,y坐标
	LCD_WriteReg(lcddev.setxcmd, Xpos);
	LCD_WriteReg(lcddev.setycmd, Ypos);
}


void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
//	u16 temp;
	switch(dir)
	{
		case L2R_U2D://从左到右,从上到下
			regval|=(1<<5)|(1<<4)|(0<<3);
			break;
		case L2R_D2U://从左到右,从下到上
			regval|=(0<<5)|(1<<4)|(0<<3);
			break;
		case R2L_U2D://从右到左,从上到下
			regval|=(1<<5)|(0<<4)|(0<<3);
			break;
		case R2L_D2U://从右到左,从下到上
			regval|=(0<<5)|(0<<4)|(0<<3);
			break;
		case U2D_L2R://从上到下,从左到右
			regval|=(1<<5)|(1<<4)|(1<<3);
			break;
		case U2D_R2L://从上到下,从右到左
			regval|=(1<<5)|(0<<4)|(1<<3);
			break;
		case D2U_L2R://从下到上,从左到右
			regval|=(0<<5)|(1<<4)|(1<<3);
			break;
		case D2U_R2L://从下到上,从右到左
			regval|=(0<<5)|(0<<4)|(1<<3);
			break;
	}
	if(lcddev.id==0x8989)//8989 IC
	{
		dirreg=0X11;
		regval|=0X6040;	//65K
	}
	LCD_WriteReg(dirreg,regval);
}



//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//设置光标位置
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	LCD_WR_DATA(POINT_COLOR);
}

//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{
	if(lcddev.dir==1)x=lcddev.width-1-x;//横屏其实就是调转x,y坐标
	LCD_WriteReg(lcddev.setxcmd,x);
	LCD_WriteReg(lcddev.setycmd,y);
	LCD_WR_REG(lcddev.wramcmd);
	LCD_WR_DATA(color);		//写数据
}


//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
		lcddev.wramcmd=R34;
		lcddev.setxcmd=0X4E;
		lcddev.setycmd=0X4F;
	}
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}



void initLcdGpio(void)
{
		/* config extend interface -------------------------------------------*/
  		EALLOW;
    	GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 3;  // XD15
    	GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 3;  // XD14
    	GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 3;  // XD13
    	GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 3;  // XD12
    	GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 3;  // XD11
    	GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 3;  // XD10
    	GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 3;  // XD19
    	GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 3;  // XD8
    	GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 3;  // XD7
    	GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 3;  // XD6
    	GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 3;  // XD5
    	GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 3;  // XD4
    	GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 3;  // XD3
    	GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 3;  // XD2
    	GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 3;  // XD1
    	GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 3;  // XD0

    	GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 3;  // XA0/XWE1n
    	GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 3;  // XA1
    	GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 3;  // XA2
    	GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 3;  // XA3
    	GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 3;  // XA4
    	GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 3;  // XA5
    	GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 3;  // XA6
    	GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 3;  // XA7

    	GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 3;  // XA8
   	 	GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 3;  // XA9
    	GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 3;  // XA10
    	GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 3;  // XA11
    	GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 3;  // XA12
    	GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 3;  // XA13

        GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 3;  // XA4
        GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 3;  // XA5
        GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 3;  // XA16
        GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 3;  // XA17
        GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 3;  // XA18
        GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 3;  // XA19

     	GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 3;  // XWE0

     	GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 3;  // XZCS7

     	EDIS;

     	/* lcd reset pin -> GPIO63 as output pin  ----------------------------*/
     	EALLOW;
     	GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;
     	GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;  // gpio
     	GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;	/* output */
     	EDIS;

    	/* Power Pin -> GPIO50 ----------------------------------------------*/
    	EALLOW;
    	GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;
		GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;
		GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;	/* output */
		EDIS;
//
//		LCD_POWER_OFF;
}/* end of initLcdGpio() */
void Init_LCD_PORT()
{
	initLcdXint();
	initLcdGpio();
	LCD_BL_SET;       //LCD背光打开
}


//初始化lcd
//该初始化函数可以初始化各种ALIENTEK出品的LCD液晶屏
//本函数占用较大flash,用户可以根据自己的实际情况,删掉未用到的LCD初始化代码.以节省空间.
void LCD_Init(void)
{
		Init_LCD_PORT();
		Delay_ms(1);
		LCD_RST_SET;
	    Delay_ms(1);                    // Delay 1ms
	    LCD_RST_CLR;
	    Delay_ms(10);                   // Delay 10ms
	    LCD_RST_SET;
	    Delay_ms(50);                   // Delay 50 ms
		LCD_WriteReg(0x0000,0x0001);
		Delay_ms(50);                   // Delay 50 ms
		Delay_ms(50);                   // Delay 50 ms
	  	lcddev.id = LCD_ReadReg(0x0000);
	  	DSP28x_usDelay(1000);
	  	DSP28x_usDelay(1000);
	  	Delay_ms(500);                   // Delay 50 ms
  	lcddev.id = LCD_ReadReg(0x0000);
  	Delay_ms(500);
	if(lcddev.id==0x8989)//OK |/|/|
		{
			LCD_WriteReg(0x0000,0x0001);//打开晶振
	    	LCD_WriteReg(0x0003,0xA8A4);//0xA8A4
	    	LCD_WriteReg(0x000C,0x0000);
	    	LCD_WriteReg(0x000D,0x080C);
	    	LCD_WriteReg(0x000E,0x2B00);
	    	LCD_WriteReg(0x001E,0x00B0);
	    	LCD_WriteReg(0x0001,0x2B3F);//驱动输出控制320*240  0x6B3F
	    	LCD_WriteReg(0x0002,0x0600);
	    	LCD_WriteReg(0x0010,0x0000);
	    	LCD_WriteReg(0x0011,0x6078); //定义数据格式  16位色 		横屏 0x6058
	    	LCD_WriteReg(0x0005,0x0000);
	    	LCD_WriteReg(0x0006,0x0000);
	    	LCD_WriteReg(0x0016,0xEF1C);
	    	LCD_WriteReg(0x0017,0x0003);
	    	LCD_WriteReg(0x0007,0x0233); //0x0233
	    	LCD_WriteReg(0x000B,0x0000);
	    	LCD_WriteReg(0x000F,0x0000); //扫描开始地址
	    	LCD_WriteReg(0x0041,0x0000);
	    	LCD_WriteReg(0x0042,0x0000);
	    	LCD_WriteReg(0x0048,0x0000);
	    	LCD_WriteReg(0x0049,0x013F);
	    	LCD_WriteReg(0x004A,0x0000);
	    	LCD_WriteReg(0x004B,0x0000);
	    	LCD_WriteReg(0x0044,0xEF00);
	    	LCD_WriteReg(0x0045,0x0000);
	    	LCD_WriteReg(0x0046,0x013F);
	    	LCD_WriteReg(0x0030,0x0707);
	    	LCD_WriteReg(0x0031,0x0204);
	    	LCD_WriteReg(0x0032,0x0204);
	    	LCD_WriteReg(0x0033,0x0502);
	    	LCD_WriteReg(0x0034,0x0507);
	    	LCD_WriteReg(0x0035,0x0204);
	    	LCD_WriteReg(0x0036,0x0204);
	    	LCD_WriteReg(0x0037,0x0502);
	    	LCD_WriteReg(0x003A,0x0302);
	    	LCD_WriteReg(0x003B,0x0302);
	    	LCD_WriteReg(0x0023,0x0000);
	    	LCD_WriteReg(0x0024,0x0000);
	    	LCD_WriteReg(0x0025,0x8000);
	    	LCD_WriteReg(0x004f,0);        //行首址0
	    	LCD_WriteReg(0x004e,0);        //列首址0
		}
		LCD_Display_Dir(DIR);		 	//默认为竖屏
		DSP28x_usDelay(10);	/* delay 1 us */
		LCD_BL_SET;       //LCD背光打开
		DSP28x_usDelay(10);	/* delay 1 us */
		LCD_Clear(WHITE);
}


//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	u32 index=0;
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
	LCD_SetCursor(0x00,0x0000);	//设置光标位置
	LCD_WriteRAM_Prepare();     		//开始写入GRAM
	for(index=0;index<totalpoint;index++)LCD_WR_DATA(color);
}

//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{
	u16 i,j;
	u16 xlen=0;
//	u16 temp;
	xlen=ex-sx+1;
	for(i=sy;i<=ey;i++)
	{
		LCD_SetCursor(sx,i);      				//设置光标位置
		LCD_WriteRAM_Prepare();     			//开始写入GRAM
		for(j=0;j<xlen;j++)LCD_WR_DATA(color);	//设置光标位置
	}
}

//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)
			LCD_WR_DATA(color[i*width+j]);//写入数据
	}
}
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向
	else if(delta_x==0)incx=0;//垂直线
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//水平线
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//画线输出
	{
		LCD_DrawPoint(uRow,uCol);//画点
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
    u8 temp,t1,t;
	u16 y0=y;
	//设置窗口
	num=num-' ';//得到偏移后的值
	for(t=0;t<16;t++)
	{
		temp=asc2_1608[num][t];	//调用1608字体
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(x>=lcddev.width)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}
	}
}
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,1);
        x+=size/2;
        p++;
    }
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3
		LCD_DrawPoint(x0+b,y0-a);             //0
		LCD_DrawPoint(x0-a,y0+b);             //1
		LCD_DrawPoint(x0-b,y0-a);             //7
		LCD_DrawPoint(x0-a,y0-b);             //2
		LCD_DrawPoint(x0+b,y0+a);             //4
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6
		LCD_DrawPoint(x0-b,y0+a);
		a++;
		//使用Bresenham算法画圆
		if(di<0)di +=4*a+6;
		else
		{
			di+=10+4*(a-b);
			b--;
		}
		LCD_DrawPoint(x0+a,y0+b);
	}
}
//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1;

		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0);
	}
}
//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;
	return result;
}


void showhanzi(unsigned int x,unsigned int y,unsigned char index,u8 mode)
{
    u8 temp,t1,t;
	u16 y0=y;
	for(t=0;t<128;t++)
	{
		temp=shaotianchupin[index][t];
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(x>=lcddev.width)return;		//超区域了
			if((y-y0)==31)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}
	}
}

