/*********************************************************************
**	Module Name:		DAC     				    				**
**	Revision History:	2014-3-10
**	Author:sxdembed
**	store:http://sxdembed.taobao.com/ 					**
**********************************************************************/

/*********************************************************************
**	ʵ��Ŀ��:ͨ��ѧϰ�˳�������˽�DSP��SPI��DAоƬ(TLV5620)������  **
**           ������������DSP��һ����Ҫ����,���Ҫ�ﵽ���յĳ̶�     **
**	ʵ��˵��:��ҪӲ����DSP��TLV5620���,DSP�ṩ��SIMO��SCLK��C3TRIP **
**           �źŸ�TLV5620����C3TRIP�ź���������ƬDAC�źŵĸ��º��� **
**           ��ģ�������Ҫ�����������ṩ��TLV5602���ֲᣬ4���ŵ� **
**           ��DAC����ֱ����˳����������������.���ﻹҪ˵����һ **
**           ����������������е������ŵ�DACA��DACB��ΪADC�����룬  **
**           ��ҿ���·��֪��	                                    **
**	ʵ����:�����ñ����TLV5620��4�������ѹֵ���ɿ��������ǳɱ��� **
**           ��ϵ���ֱ�Ϊ0.4V,0.8V,1.2V,1.6V.                       **
**********************************************************************/

//    As supplied, this project is configured for "boot to SARAM"
//    operation.  The 2833x Boot Mode table is shown below.
//
//       $Boot_Table:
//
//         GPIO87   GPIO86     GPIO85   GPIO84
//          XA15     XA14       XA13     XA12
//           PU       PU         PU       PU
//        ==========================================
//            1        1          1        1    Jump to Flash
//            1        1          1        0    SCI-A boot
//            1        1          0        1    SPI-A boot
//            1        1          0        0    I2C-A boot
//            1        0          1        1    eCAN-A boot
//            1        0          1        0    McBSP-A boot
//            1        0          0        1    Jump to XINTF x16
//            1        0          0        0    Jump to XINTF x32
//            0        1          1        1    Jump to OTP
//            0        1          1        0    Parallel GPIO I/O boot
//            0        1          0        1    Parallel XINTF boot
//            0        1          0        0    Jump to SARAM	    <- "boot to SARAM"
//            0        0          1        1    Branch to check boot mode
//            0        0          1        0    Boot to flash, bypass ADC cal
//            0        0          0        1    Boot to SARAM, bypass ADC cal
//            0        0          0        0    Boot to SCI-A, bypass ADC cal
//                                              Boot_Table_End$


#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

/*����ʹ���˺궨�������Ƹ��������źŵĹ���,�ص������ʱ����*/
#define SetLOAD GpioDataRegs.GPBDAT.bit.GPIO55=1;	//��LOAD�ø�
#define ClrLOAD GpioDataRegs.GPBDAT.bit.GPIO55=0;	//��LOAD�õ�

void WriteDAC(unsigned char add,unsigned char rng,unsigned char vol);
void delay(unsigned int t);
void spi_xmit(Uint16 a);
void spi_fifo_init(void);
void spi_init(void);




void main(void)
{   
	int temp;
    
	/*��ʼ��ϵͳ*/
	InitSysCtrl();

	/*��ʼ��GPIO;*/
	InitSpiaGpio();

	///��ʼ��IO��
	
	EALLOW;
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0; // ����GPIO55ΪGPIO��
	GpioCtrlRegs.GPBDIR.bit.GPIO55 = 1;      // ����GPIO55�������
	GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;      // ��ֹ���� GPIO55����
	EDIS;
	 GpioDataRegs.GPBDAT.bit.GPIO57=1;
	/* ���ж� */
	DINT;
	IER = 0x0000;
	IFR = 0x0000;
	
	/* ��ʼ��PIE���ƼĴ��� */
	InitPieCtrl();
	
	/* ��ʼ��PIE������ */
	InitPieVectTable();	
	


	///��ʼ��SPI
 
    spi_init();		  // ��ʼ��SPI

	EINT;   	// Enable Global interrupt INTM
	ERTM;		// Enable Global realtime interrupt DBGM

    SetLOAD;	//��ˢ����������ź�����
            
    temp=47;//REF=2.2V;VO(DACA|B|C|D) =REF* CODE/256
 
    while(1)
    {
	        
	        WriteDAC(0,0,temp);		//0.4V

	        WriteDAC(1,0,temp*2);	//0.8V

	        WriteDAC(2,0,temp*3);	//1.2V

	        WriteDAC(3,0,temp*4);	//1.6V

	        delay(1500);	//�ڴ���ϵ�,�۲����DAC0��DAC1��ֵ,��������ñ�ֱ�Ӳ�����·DA�������ѹֵ
	}

} 	

	
void WriteDAC(unsigned char add,unsigned char rng,unsigned char vol)
{   
	unsigned short int data;
    data=0x0000;
    ///���Ҫ֪������������ĸ��������ĺ���,add��4��ͨ���ĵ�ַ��00��01��10��11��
    ///                                     RNG�������Χ�ı�����������0��1��
    ///                                     VOL��0~256����
    
     data = ((add<<14) | (rng<<13) | (vol<<5));
     //ע���������Ч������11λ��SPI��ʼ����Ҳ�����˶���
 
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG ==1);			//�ж�SPI�ķ��ͻ������Ƿ��ǿյ�,����0��д����
     
       SpiaRegs.SPITXBUF = data;	//�ѷ��͵�����д��SPI���ͻ�����

    while( SpiaRegs.SPISTS.bit.BUFFULL_FLAG==1);		//�����ͻ�������������־λʱ,��ʼ��������
    	 
	delay(1500);//ͬͨ��һ����������Ҫ���͵�����,��TLV5620�����ֲἴ�ɵ�֪
    ClrLOAD;
    
	delay(150);
    SetLOAD;
    
    delay(1500);	
}

void delay(unsigned int t)
{
 
 	while(t>0)
    	t--;
}
//��ʼ��SPI����
void spi_init()
{    
	SpiaRegs.SPICCR.all =0x0a;///�����ʼ״̬��������������������Բ��ֹ��11λ����ģʽ
	                            
	SpiaRegs.SPICTL.all =0x0006; // ʹ������ģʽ��������λ��ʹ���������ͣ���ֹ����
	                            //����жϣ���ֹSPI�жϣ�
                                   
	SpiaRegs.SPIBRR =0x0031;	//SPI������=37.5M/50	=0.75MHZ��							
    SpiaRegs.SPICCR.all =0x8a; //�˳���ʼ״̬�� 
    SpiaRegs.SPIPRI.bit.FREE = 1;  // ��������     
}






//===========================================================================
// No more.
//===========================================================================
