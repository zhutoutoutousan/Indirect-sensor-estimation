#include "DSP28x_Project.h"
#include "DSP2833x_Examples.h"
#include "math.h"
#include <stdio.h>
#define omega 4
#define fai 25/180
volatile Uint16 i;
volatile Uint16 j;
volatile Uint16 k;
double SIN[128];

void DELAY(int x)
{
	GpioDataRegs.GPADAT.bit.GPIO0 = ~(GpioDataRegs.GPADAT.bit.GPIO0);
	DELAY_US(x);

}


void main(void)
{

   InitSysCtrl();

   DINT;

   InitPieCtrl();

   IER = 0x0000;
   IFR = 0x0000;

   InitPieVectTable();

   EALLOW;
   PieVectTable.TINT0 = &cpu_timer0_isr;
   EDIS;

   InitCpuTimers();

   ConfigCpuTimer(&CpuTimer0, 150, 10000);
   CpuTimer1Regs.TCR.bit.TSS = 1;
   CpuTimer0Regs.TCR.all = 0x4000;

   IER |= M_INT1;


   EALLOW;
   GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0; //GPIO
   GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;  //Output
   EDIS;




   for(;;)
   {
	   for(int i=0;;i++)
	   {
	   	dedelay(A*sin(omega*i));
	   }


}
}
