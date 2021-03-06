/*
 * hanzi.h
 *
 *  Created on: 2016年6月11日
 *      Author: Administrator
 */

#ifndef HANZI_H_
#define HANZI_H_
/////////////////////////////////////////////////////////////////////////
// 汉字取模步骤:
//      1、在文字输入区输入汉字
//      2、在输入区鼠标右键选择字体，字体选：新宋体,大小选：小四，即16x16，后点确定
//      3、键盘输入CTRL+ENTER组合键
//      4、点击左边菜单"取模方式-》C51格式"，即会在点阵生成区中显示结果。
//  注意:软件默认为横向取模,如果需要修改取模方式,点击左边菜单"参数设置-》其他选项"即可修改.
/////////////////////////////////////////////////////////////////////////


//逐列式，顺向
u8 shaotianchupin[4][128]={



		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBF,0xFF,0xFF,0xFF,0x3F,0xFF},
		{0xFF,0xFE,0x7F,0xFF,0xFF,0xFC,0xBF,0xFF,0xFD,0xF1,0x87,0xFF,0xFD,0xE3,0x80,0x7F},
		{0xFD,0x0F,0xBD,0xFF,0xF8,0x3F,0xBD,0xFF,0xFA,0xF7,0xBD,0xFF,0xFB,0xF3,0x7D,0xFF},
		{0xFB,0xF3,0x79,0xFF,0xFB,0xC7,0x01,0xFF,0xF0,0x0F,0x0B,0xFF,0xF0,0x7F,0x3F,0xFF},
		{0xF9,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0x00,0x00,0x01},
		{0xFE,0x00,0x00,0x03,0xFE,0x7F,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFE,0xF9,0xF7,0xFF},
		{0xFE,0xE5,0xF7,0xFF,0xFC,0x9E,0xF3,0xFF,0xFC,0x3E,0x33,0xFF,0xFC,0x7F,0x01,0xFF},
		{0xFE,0xFF,0xC0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},/*"邵",0*/
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xEF},
		{0xFF,0xFF,0xFF,0xEF,0xFF,0xFF,0xFF,0xDF,0xFF,0xFF,0xFF,0xDF,0xFF,0xFE,0xFF,0x9F},
		{0xFF,0xFE,0xFF,0xBF,0xFE,0xFE,0xFF,0x3F,0xFE,0xFE,0xFE,0x7F,0xFE,0xFC,0xFC,0xFF},
		{0xFE,0xFC,0xF1,0xFF,0xFE,0xFC,0x03,0xFF,0xFC,0x00,0x0F,0xFF,0xFC,0x01,0xDF,0xFF},
		{0xFC,0xFD,0xEF,0xFF,0xFD,0xF9,0xF3,0xFF,0xFD,0xF9,0xF9,0xFF,0xF9,0xF9,0xFC,0xFF},
		{0xF9,0xFB,0xFE,0x7F,0xF9,0xF3,0xFE,0x3F,0xFF,0xF3,0xFF,0x0F,0xFF,0xF3,0xFF,0x8F},
		{0xFF,0xFB,0xFF,0xC7,0xFF,0xFF,0xFF,0xC7,0xFF,0xFF,0xFF,0xC7,0xFF,0xFF,0xFF,0xE7},
		{0xFF,0xFF,0xFF,0xEF,0xFF,0xFF,0xFF,0xEF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},/*"天",1*/
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0x87,0xFF,0xC0,0x30,0x0F},
		{0xFF,0xE0,0x38,0x4F,0xFF,0xFF,0x7F,0xDF,0xFF,0xFE,0x7F,0x9F,0xFF,0xFE,0x7F,0x9F},
		{0xFF,0xFE,0x7F,0x9F,0xFF,0xFE,0xFF,0xBF,0xE3,0xFE,0xFF,0xBF,0xE0,0x00,0x00,0x3F},
		{0xF0,0x1E,0xFF,0xBF,0xFF,0xFC,0xFF,0x3F,0xFF,0xFD,0xFF,0x3F,0xFF,0xFD,0xFF,0x7F},
		{0xFF,0xFD,0xFF,0x7F,0xFF,0xFD,0xFF,0x7F,0xFF,0xFD,0xFF,0x7F,0xFF,0x80,0x20,0x07},
		{0xFF,0x80,0xF0,0x0F,0xFF,0xCF,0xF3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},/*"出",2*/
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF7,0xFF},
		{0xFF,0xFF,0xF1,0xFF,0xFF,0xFF,0xF0,0x1F,0xFF,0xFF,0xF7,0x9F,0xFF,0xFF,0xEF,0xBF},
		{0xFF,0xFF,0xEF,0xBF,0xFE,0xFF,0xEF,0xBF,0xFE,0x1F,0xEF,0xBF,0xFE,0x03,0xCF,0x3F},
		{0xFE,0xF1,0xC0,0x3F,0xFE,0xFB,0xE1,0x3F,0xFE,0xFB,0xE7,0xFF,0xFE,0xFB,0xFF,0xFF},
		{0xFD,0xF3,0xDF,0xFF,0xFD,0xF7,0xC1,0xFF,0xFC,0xE7,0xC0,0x1F,0xF8,0x07,0xDF,0xBF},
		{0xFC,0x37,0xDF,0x3F,0xFC,0x7F,0xDF,0x3F,0xFE,0xFF,0xDF,0x3F,0xFF,0xFF,0x9F,0x3F},
		{0xFF,0xFF,0x9F,0x3F,0xFF,0xFF,0x80,0x7F,0xFF,0xFF,0x81,0x7F,0xFF,0xFF,0x87,0xFF},
		{0xFF,0xFF,0xCF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},/*"品",3*/


};/*"字",1*/





#endif /* HANZI_H_ */
