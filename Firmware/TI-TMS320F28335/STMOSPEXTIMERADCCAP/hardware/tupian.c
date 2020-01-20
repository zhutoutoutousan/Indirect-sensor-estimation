/*
 * tupian.c
 *
 *  Created on: 2016年7月7日
 *      Author: Administrator
 */
#include "tupian.h"
#include "TFT.h"
#include "img.h"


u16 Write_Data(u16 LCD_Data_HD, u16 LCD_Data_LD)
{
	return (LCD_Data_HD<<8)|LCD_Data_LD;
}
void Show_Image1(unsigned int x0,unsigned int y0,unsigned int width,unsigned int height)
{
	unsigned int i,j,index_data;
	for (i=0; i<height; i++)         //起始位置应该从0开始，便于提取数据
	{
    for (j=0; j<width;j++)      //起始位置应该从0开始，便于提取数据
		{
			index_data=Write_Data(gImage_256[i*width*2+j*2+1],gImage_256[i*width*2+j*2]);
			LCD_Fast_DrawPoint(x0+j,y0+i,index_data);
		}
	}
}
void Show_Image2(unsigned int x0,unsigned int y0,unsigned int width,unsigned int height)
{
	unsigned int i,j,index_data;
	for (i=0; i<height; i++)         //起始位置应该从0开始，便于提取数据
	{
    for (j=0; j<width;j++)      //起始位置应该从0开始，便于提取数据
		{
			index_data=Write_Data(gImage_256[i*width*2+j*2+1],gImage_256[i*width*2+j*2]);
			LCD_Fast_DrawPoint(x0+j,y0+i,index_data);
		}
	}
}
void Show_Image3(unsigned int x0,unsigned int y0,unsigned int width,unsigned int height)
{
	unsigned int i,j,index_data;
	for (i=0; i<height; i++)         //起始位置应该从0开始，便于提取数据
	{
    for (j=0; j<width;j++)      //起始位置应该从0开始，便于提取数据
		{
			index_data=Write_Data(gImage_256[i*width*2+j*2+1],gImage_256[i*width*2+j*2]);
			LCD_Fast_DrawPoint(x0+j,y0+i,index_data);
		}
	}
}





