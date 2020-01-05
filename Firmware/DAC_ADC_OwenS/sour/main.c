#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define SetLOAD GpioDataRegs.GPBDAT.bit.GPIO55=1;
#define ClrLOAD GpioDataRegs.GPBDAT.bit.GPIO55=0;

#define ADC_MODCLK 0x5
#define ADC_CKPS   0x1
#define AVG 	   100
#define ZOFFSET    0x00
#define BUF_SIZE   2048

Uint16 SampleTable[BUF_SIZE];
float Vin=0;

void main(void)
{

	InitSysCtrl();

	EALLOW;
	SysCtrlRegs.HISPCP.all = ADC_MODCLK;	// HSPCLK = SYSCLKOUT/（2*ADC_MODCLK）=15MHZ
	EDIS;

	InitSpiaGpio();

	EALLOW;
	GpioCtrlRegs.GPBMUX2.bit.GPIO55= 0; // 配置GPIO55为GPIO口
	GpioCtrlRegs.GPBDIR.bit.GPIO55 = 1;      // 定义GPIO55输出引脚
	GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;      // 禁止上啦 GPIO55引脚
	EDIS;

	DINT;
	IER = 0x0000;
	IFR = 0x0000;

	InitPieCtrl();

	InitPieVectTable();

	InitAdc();  // For this example, init the ADC

	spi_init();		  // 初始化SPI

	EINT;   	// Enable Global interrupt INTM
	ERTM;		// Enable Global realtime interrupt DBGM

    // Specific ADC setup for this example:
	AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;//设置采样窗口时间：（15+1）*ADCCLK
	AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;//ADC内核时钟分频：HSPCLK/2=6.25MHZ
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1选择级联模式
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;  //通道选择ADCAIN0
	AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       // 设置为连续运行
	
	   for (i=0; i<BUF_SIZE; i++)
	   {
	     SampleTable[i] = 0;
	   }

	   SetLOAD;

	   temp = 47; //


}

void WriteDAC()
{
	unsigned short int data;
	data = 0x0000;
	// add->00,01,10,11   RNG->x*Vout,can be 0 or 0~256
	data = ((add<<14))|(rng<<13)|(vol<<5);


}
