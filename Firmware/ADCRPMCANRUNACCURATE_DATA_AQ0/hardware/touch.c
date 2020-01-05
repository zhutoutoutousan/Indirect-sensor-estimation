/*
 * touch.c
 *
 *  Created on: 2016��5��8��
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

//Ĭ��Ϊtouchtype=0������.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;

struct struct1 coordinate;        //����һ����ΪTp_pix�Ľṹ����,���ڴ�Ŵ�����������

uint  lx,ly;                      //���ݲɼ����Ĳ���ת��ΪTFT�ϵ�ʵ������ֵ

void Init_Touch_Port()
{
	EALLOW;
	GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;	/* output */
	EDIS;   //TFT��ʾ������
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
//	ʵ�ֹ��ܣ���ȡ������X���Y���ѹֵ
//	���ز�����pix ��ȡ���Ĵ������ѹֵ
//================================================================================================
struct struct1 AD_Touch()
{
  struct struct1 pix;
  TPCS_CLR;

  Write_Touch(0x90);                 //�Ϳ����� 10010000 ���ò�ַ�ʽ��X���꣬��ϸ����й�����
  TPCLK_SET;
  DSP28x_usDelay(1);
  TPCLK_CLR;
  DSP28x_usDelay(1);
  pix.y=Read_Touch();

  Write_Touch(0xD0);                 //�Ϳ����� 11010000 ���ò�ַ�ʽ��Y���� ��ϸ����й�����
  TPCLK_SET;
  DSP28x_usDelay(1);
  TPCLK_CLR;
  DSP28x_usDelay(1);
  pix.x=Read_Touch();

  TPCS_SET;
  return pix;
}

//================================================================================================
//	ʵ�ֹ��ܣ�	д8λ�����������IC
//	���������  temp ��Ҫд���8λ��������
//================================================================================================
void Write_Touch(uchar temp)                         //SPIд8λ�����������IC
{
  uchar i=0;

  for(i=0;i<8;i++)                                    //ѭ��8��д��һ�ֽ�
  {
    if(temp&0x80)
      TPDI_SET;
    else
      TPDI_CLR;    //�ж����λ�Ƿ�Ϊ1,Ϊ1��������λд1
    TPCLK_CLR;
    DSP28x_usDelay(1);                //��һ�����壬��������Ч����DINλ�������뵽IC
    TPCLK_SET;
    DSP28x_usDelay(1);
    temp<<=1;                           //��д��������1λ��׼����д��һλ����
  }
}

//================================================================================================
//	ʵ�ֹ��ܣ�	�Ӵ�������IC��8λ���ݵ�������
//	���ز�����  temp ��Ҫд���8λ��������
//================================================================================================
uint Read_Touch()                          //SPI ������
{
  uchar i=0;
  uint temp=0;

  for(i=0;i<12;i++)                         //ѭ��12�ζ�ȡ12λ���
  {
    temp<<=1;                                  //temp����һλ��׼����ȡ��һλ
    TPCLK_SET;
    DSP28x_usDelay(1);                   //�½�����Ч
    TPCLK_CLR;
    DSP28x_usDelay(1);
    if(TPDOUT)
      temp++;                     //�жϿ���IC�ͳ���һλ�����Ƿ�Ϊ1,���Ϊ1,����temp�����λ
  }
  return(temp);                                 //���ؽ��
}

//================================================================================================
//	ʵ�ֹ��ܣ�����˲����˵���������Ĳ�����
//	���ز�����flag �ɼ������Ƿ���Ч��־,flag=1;��������Ч
//================================================================================================
uchar pix_filter(struct struct1 pix1,struct struct1 pix2)
{
  uchar flag=0;
  int x=pix1.x>pix2.x?pix1.x-pix2.x:pix2.x-pix1.x;  //X�����β�������ֵ
  int y=pix1.y>pix2.y?pix1.y-pix2.y:pix2.y-pix1.y;  //Y�����β�������ֵ
  if(x<10&&y<10)	                                  //����˲���2��ȡ����ֵ���̫�����Ϊ����
  {
    flag=1;
    coordinate.x=(pix1.x+pix2.x)/2;           //�����β���ƽ��ֵ
    coordinate.y=(pix1.y+pix2.y)/2;
  }
  return flag;
}


//////////////////////////////////////////////////////////////////////////////////
//��LCD�����йصĺ���
//��һ��������
//����У׼�õ�
//x,y:����
//color:��ɫ
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);//����
	LCD_DrawLine(x,y-12,x,y+13);//����
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6);//������Ȧ
}
//��һ�����(2*2�ĵ�)
//x,y:����
//color:��ɫ
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//���ĵ�
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);
}

void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//����
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//��ʾ��������
  	POINT_COLOR=RED;//���û�����ɫ
}
/****************************************************************************
* ��    �ƣ�void LCD_Adjustd(void)
* ��    �ܣ�У��������ϵ��
* ��ڲ�����	null
* ���ڲ�������
* ˵    ����null
* ���÷�����LCD_Adjustd();
****************************************************************************/
u8 TP_Adjustd(void)
{
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	LCD_ShowString(80,140,200,16,16,"Adjustd Begin");
	Delay_ms(5000);
	Delay_ms(5000);
	//��һ����
	TP_Drow_Touch_Point(20, 20, RED);
	while(Adjustd_index(0));
	Delay_ms(5000);
	Delay_ms(5000);
	Delay_ms(5000);
	// ���ڶ�����
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
	// ����������
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
	// �����ĸ���
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
	// ���������
	TP_Drow_Touch_Point(120, 160, RED);
	TP_Drow_Touch_Point(220, 300, WHITE);
	while(Adjustd_index(4));
	//����У��ϵ��
	Delay_ms(5000);
	Delay_ms(5000);
	Delay_ms(5000);
	KY  = (((float)(y[0]-y[2])/280+(float)(y[1]-y[3])/280)/2);
	KX  = (((float)(x[0]-x[1])/200+(float)(x[2]-x[3])/200)/2);
	YLC  = y[4];
	XLC  = x[4];
	// �������
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
	if(PEN==0)			//������������
	{
	    pix1=AD_Touch();
	    pix2=AD_Touch();
	    if(pix_filter(pix1,pix2)==1) //�õ���ǰTP��ȡ��ֵ���˴�ʹ������˲���2��ȡ����ֵ���̫�����Ϊ����
	    {
			lx=(int)((int)((coordinate.x-XLC)/KX)+XC);
			lx=240-lx;
			ly=(int)((int)((coordinate.y-YLC)/KY)+YC);
			ly=320-ly;
	    }
	 	if(lx < lcddev.width && ly < lcddev.height)
		{
			if( lx >(lcddev.width-24) && ly <16)Load_Drow_Dialog();//���
			else TP_Draw_Big_Point(lx,ly,RED);		//��ͼ
		}
	}else Delay_ms(10);	//û�а������µ�ʱ��

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
	    if(pix_filter(pix1,pix2)==1) //�õ���ǰTP��ȡ��ֵ���˴�ʹ������˲���2��ȡ����ֵ���̫�����Ϊ����
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
