/*
 * touch.h
 *
 *  Created on: 2016年5月8日
 *      Author: Administrator
 */

#ifndef TOUCH_H_
#define TOUCH_H_
#include "TFT.h"
//Xmin、max和Ymin、Ymax分别是触摸屏横纵坐标的最小/最大值,如果偏差大时，用户可重新标定
#define Xmin           0x0133
#define Xmax           0x0EDA
#define Ymin           0x00D0
#define Ymax           0x0E2F

/*********************液晶触摸管脚定义 Start************************************/
#define PEN         GpioDataRegs.GPADAT.bit.GPIO13            //输入的值

#define TPDOUT      GpioDataRegs.GPBDAT.bit.GPIO55            //输入的值MISO

#define BUSY_CLR	GpioDataRegs.GPACLEAR.bit.GPIO17= 1          //BUSY置低
#define BUSY_SET	GpioDataRegs.GPASET.bit.GPIO17 = 1            //BUSY置高

#define TPDI_CLR	GpioDataRegs.GPBCLEAR.bit.GPIO54= 1  //TPDI置低  --> MOSI
#define TPDI_SET	GpioDataRegs.GPBSET.bit.GPIO54 = 1   //TPDI置高

#define TPCS_CLR	GpioDataRegs.GPBCLEAR.bit.GPIO49= 1          //TPCS置低
#define TPCS_SET	GpioDataRegs.GPBSET.bit.GPIO49 = 1          //TPCS置高

#define TPCLK_CLR	GpioDataRegs.GPBCLEAR.bit.GPIO56= 1           //TPCLK置低
#define TPCLK_SET	GpioDataRegs.GPBSET.bit.GPIO56 = 1           //TPCLK置高
/*********************液晶触摸管脚定义 END************************************/

struct struct1                          //定义一个类型名为struct1的结构,包含两个成员x和y
{
  int x;                         //成员x
  int y;                         //成员y
};
extern uint  lx,ly;                      //根据采集到的参数转换为TFT上的实际坐标值
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
