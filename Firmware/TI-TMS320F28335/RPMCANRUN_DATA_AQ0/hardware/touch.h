/*
 * touch.h
 *
 *  Created on: 2016��5��8��
 *      Author: Administrator
 */

#ifndef TOUCH_H_
#define TOUCH_H_
#include "TFT.h"
//Xmin��max��Ymin��Ymax�ֱ��Ǵ����������������С/���ֵ,���ƫ���ʱ���û������±궨
#define Xmin           0x0133
#define Xmax           0x0EDA
#define Ymin           0x00D0
#define Ymax           0x0E2F

/*********************Һ�������ܽŶ��� Start************************************/
#define PEN         GpioDataRegs.GPADAT.bit.GPIO13            //�����ֵ

#define TPDOUT      GpioDataRegs.GPBDAT.bit.GPIO55            //�����ֵMISO

#define BUSY_CLR	GpioDataRegs.GPACLEAR.bit.GPIO17= 1          //BUSY�õ�
#define BUSY_SET	GpioDataRegs.GPASET.bit.GPIO17 = 1            //BUSY�ø�

#define TPDI_CLR	GpioDataRegs.GPBCLEAR.bit.GPIO54= 1  //TPDI�õ�  --> MOSI
#define TPDI_SET	GpioDataRegs.GPBSET.bit.GPIO54 = 1   //TPDI�ø�

#define TPCS_CLR	GpioDataRegs.GPBCLEAR.bit.GPIO49= 1          //TPCS�õ�
#define TPCS_SET	GpioDataRegs.GPBSET.bit.GPIO49 = 1          //TPCS�ø�

#define TPCLK_CLR	GpioDataRegs.GPBCLEAR.bit.GPIO56= 1           //TPCLK�õ�
#define TPCLK_SET	GpioDataRegs.GPBSET.bit.GPIO56 = 1           //TPCLK�ø�
/*********************Һ�������ܽŶ��� END************************************/

struct struct1                          //����һ��������Ϊstruct1�Ľṹ,����������Աx��y
{
  int x;                         //��Աx
  int y;                         //��Աy
};
extern uint  lx,ly;                      //���ݲɼ����Ĳ���ת��ΪTFT�ϵ�ʵ������ֵ
void Init_Touch_Port();
void Init_Touch();
struct struct1 AD_Touch();
void Write_Touch(unsigned char temp);
unsigned int Read_Touch();
unsigned char pix_filter(struct struct1 pix1, struct struct1 pix2);
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color);
void TP_Draw_Big_Point(u16 x,u16 y,u16 color);
void Load_Drow_Dialog(void);
u8 TP_Adjustd(void);
void test_touch();
unsigned char Adjustd_index(unsigned char j);




#endif /* TOUCH_H_ */
