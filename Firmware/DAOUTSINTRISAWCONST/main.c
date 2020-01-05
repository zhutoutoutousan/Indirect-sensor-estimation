/*********************************************************************
**	Module Name:		DAC     				    				**
**	Revision History:	2014-3-10
**	Author:sxdembed
**	store:http://sxdembed.taobao.com/ 					**
**********************************************************************/

/*********************************************************************
**	实验目的:通过学习此程序可以了解DSP的SPI与DA芯片(TLV5620)的连接  **
**           和驱动，这是DSP的一个重要特性,大家要达到掌握的程度     **
**	实验说明:主要硬件由DSP和TLV5620组成,DSP提供了SIMO和SCLK、C3TRIP **
**           信号给TLV5620，且C3TRIP信号是用来做片DAC信号的更新和锁 **
**           存的，这里大家要参照我们所提供的TLV5602的手册，4个信道 **
**           的DAC输出分别引了出来，方便大家来检测.这里还要说明的一 **
**           点的是我们用了其中的两个信道DACA和DACB作为ADC的输入，  **
**           大家看电路便知。	                                    **
**	实验结果:用万用表测试TLV5620的4个输出电压值，可看到他们是成比例 **
**           关系，分别为0.4V,0.8V,1.2V,1.6V.                       **
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

/*这里使用了宏定义来控制更新锁存信号的功能,重点就是在时序上*/
#define SetLOAD GpioDataRegs.GPBDAT.bit.GPIO55=1;	//将LOAD置高
#define ClrLOAD GpioDataRegs.GPBDAT.bit.GPIO55=0;	//将LOAD置低

void WriteDAC(unsigned char add,unsigned char rng,unsigned char vol);
void delay(unsigned int t);
void spi_xmit(Uint16 a);
void spi_fifo_init(void);
void spi_init(void);




void main(void)
{   
	int temp;
    
	/*初始化系统*/
	InitSysCtrl();

	/*初始化GPIO;*/
	InitSpiaGpio();

	///初始化IO口
	
	EALLOW;
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0; // 配置GPIO55为GPIO口
	GpioCtrlRegs.GPBDIR.bit.GPIO55 = 1;      // 定义GPIO55输出引脚
	GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;      // 禁止上啦 GPIO55引脚
	EDIS;
	 GpioDataRegs.GPBDAT.bit.GPIO57=1;
	/* 关中断 */
	DINT;
	IER = 0x0000;
	IFR = 0x0000;
	
	/* 初始化PIE控制寄存器 */
	InitPieCtrl();
	
	/* 初始化PIE参数表 */
	InitPieVectTable();	
	


	///初始化SPI
 
    spi_init();		  // 初始化SPI

	EINT;   	// Enable Global interrupt INTM
	ERTM;		// Enable Global realtime interrupt DBGM

    SetLOAD;	//把刷新锁存控制信号拉高
            
    temp=47;//REF=2.2V;VO(DACA|B|C|D) =REF* CODE/256
 
    while(1)
    {
	        
	        WriteDAC(0,0,temp);		//0.4V

	        WriteDAC(1,0,temp*2);	//0.8V

	        WriteDAC(2,0,temp*3);	//1.2V

	        WriteDAC(3,0,temp*4);	//1.6V

	        delay(1500);	//在此设断点,观察变量DAC0和DAC1的值,另外加三用表直接测量四路DA的输出电压值
	}

} 	

	
void WriteDAC(unsigned char add,unsigned char rng,unsigned char vol)
{   
	unsigned short int data;
    data=0x0000;
    ///大家要知道这里所定义的各个变量的含义,add是4个通道的地址（00，01，10，11）
    ///                                     RNG是输出范围的倍数，可以是0或1。
    ///                                     VOL是0~256数据
    
     data = ((add<<14) | (rng<<13) | (vol<<5));
     //注意这里的有效数据是11位，SPI初始化中也进行了定义
 
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG ==1);			//判断SPI的发送缓冲区是否是空的,等于0可写数据
     
       SpiaRegs.SPITXBUF = data;	//把发送的数据写如SPI发送缓冲区

    while( SpiaRegs.SPISTS.bit.BUFFULL_FLAG==1);		//当发送缓冲区出现满标志位时,开始琐存数据
    	 
	delay(1500);//同通过一负跳变琐存要发送的数据,看TLV5620数据手册即可得知
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
//初始化SPI函数
void spi_init()
{    
	SpiaRegs.SPICCR.all =0x0a;///进入初始状态，数据在上升沿输出，自测禁止，11位数据模式
	                            
	SpiaRegs.SPICTL.all =0x0006; // 使能主机模式，正常相位，使能主机发送，禁止接收
	                            //溢出中断，禁止SPI中断；
                                   
	SpiaRegs.SPIBRR =0x0031;	//SPI波特率=37.5M/50	=0.75MHZ；							
    SpiaRegs.SPICCR.all =0x8a; //退出初始状态； 
    SpiaRegs.SPIPRI.bit.FREE = 1;  // 自由运行     
}






//===========================================================================
// No more.
//===========================================================================
