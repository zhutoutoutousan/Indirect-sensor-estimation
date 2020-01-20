//###########################################################################
// Description:
//! \addtogroup f2833x_example_list
//! <h1> eCap capture PWM (ecap_capture_pwm)</h1>
//!
//! This example configures
//!
//! eCAP1 is configured to capture the time between rising
//! and falling edge of the ePWM3A output.
//!
//! \b External \b Connections \n
//! - eCap1 is on GPIO24
//! - ePWM3A is on GPIO4
//! - Connect GPIO4 to GPIO24.
//!
//! \b Watch \b Variables \n
//! - ECap1IntCount - Successful captures
//! - ECap1PassCount - Interrupt counts
//
//###########################################################################
// Code author: Owen Shao  Yangzhou University   Email:amazingostian@gmail.com
//
//
// $TI Release: F2833x/F2823x Header Files and Peripheral Examples V140 $
// $Release Date: March  4, 2015 $
// $Copyright: Copyright (C) 2007-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include <string.h>
#include "TFT.h"
#include "tupian.h"
#include <stdlib.h>
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;

//-----------------------------------------Capture------------------------------------------
//	1.ECAP
//  While angle signal proceeds ST_COUNT samples, the ECAP stops sampling for JP_COUNT-ST_COUNT
//  samples

#define ST_COUNT 5
#define JP_COUNT 8
volatile Uint16 ECap_status = 0;

#define CAPTIMER   ECap1Regs.CAP1
volatile Uint16 array_index = 1;
volatile Uint16 n_array_index = 0;
volatile Uint16 timer_index = 0;
__interrupt void ecap1_isr(void);
void InitECapture(void);
void Fail(void);


volatile int16 i = 1; // This indicator counts the desired loop

// 2. ADC
//ADC start parameters
#define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#define ADC_CKPS   0x0   // ADC module clock = HSPCLK/1      = 25.5MHz/(1)   = 25.0 MHz
#define ADC_SHCLK  0x1   // S/H width in ADC module periods                  = 2 ADC cycle
#define AVG        1000  // Average sample limit
#define ZOFFSET    0x00  // Average Zero offset
#define BUF_SIZE   1024  // Sample buffer size
#define NUM_HULL   1    // the number of pulses per revolution
#define POST_SHIFT   0  // Shift results after the entire sample table is full
#define INLINE_SHIFT 1  // Shift results as the data is taken from the results regsiter
#define NO_SHIFT     0  // Do not shift the results


// Global variables used in this example
volatile Uint16  ECap1IntCount;
volatile Uint16  ECap1PassCount;
volatile Uint16 SampleTable[BUF_SIZE];
void ADC_INIT(void);

volatile Uint16 CAPTIMCTRL = 1;  // flag value controlling Timer

// ----------------------------------------Capture--------------------------------------

// ----------------------------------------FFT---------------------------------------------
// define and global variable for FFT

#define SAMPLENUMBER 64//1024
#define PI 3.1415926
#define NR(x) (sizeof(x)/sizeof(x[0]))
// end define

void InitForFFT();
void MakeWave();

volatile float INPUT[SAMPLENUMBER],DATA[SAMPLENUMBER];
volatile float R_FFT[SAMPLENUMBER],V_FFT[SAMPLENUMBER];
volatile float fWaveR[SAMPLENUMBER],fWaveI[SAMPLENUMBER],w_V[SAMPLENUMBER],w_R[SAMPLENUMBER],w[SAMPLENUMBER];
volatile float fWaveR_R[SAMPLENUMBER],fWaveI_R[SAMPLENUMBER],fWaveR_V[SAMPLENUMBER],fWaveI_V[SAMPLENUMBER];
volatile float sin_tab[SAMPLENUMBER],cos_tab[SAMPLENUMBER];


// ----------------------------------------FFT---------------------------------------------

// ----------------------------------------STEPMOTOR---------------------------------------------
#define HULL_NUMBER 68    //
// Global variable for this example

Uint16 DataTable[BUF_SIZE];



// ----------------------------------------STEPMOTOR---------------------------------------------

//-----------------------------M/T method-----------------------------
// This M/T method use both Timer1 and ECap_passcount:M1 to measure rpm
// M/T method
#define f0 75000000  // High pulse frequency
#define Z 68     // The number of pulses of testing step motor
volatile Uint16 M1 = 0; // passcount for encoder,tachometer or hull meter
//resample
volatile int16 JP = JP_COUNT - ST_COUNT; // JP during resample


//-----------------------------M/T method-----------------------------

//-----------------------------Timer0-----------------------------
__interrupt void cpu_timer0_isr(void);
volatile float TimeStamp[BUF_SIZE]; // in ms mode
volatile Uint16 TimeStamp_ref[BUF_SIZE];
volatile Uint16 ZPO = 0;
volatile Uint16 n[BUF_SIZE];
//-----------------------------Timer0-----------------------------
#define Tc 15000
volatile Uint16 Sampletime = 0; // used to control sample time Tc
volatile float absolutime = 0.0f; // used to represent time stamp
//-----------------------------Timer1-----------------------------
__interrupt void cpu_timer1_isr(void);
volatile Uint16 M2 = 0;
//-----------------------------Timer1-----------------------------

//-----------------------------SPI--------------------------------

//-----------------------------SPI--------------------------------

//-----------------------------I2C--------------------------------

//-----------------------------I2C--------------------------------
void main(void)
{
	   // Initialize
	   // 1.Memory

	   MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);



	   // 2.System Interrupt and GPIO

	   InitSysCtrl();

	   // ADC
	   EALLOW;
	   SysCtrlRegs.HISPCP.all = ADC_MODCLK;
	   EDIS;

	   // GPIO60-SWSTEPMOTORCTRL, GPIO0-EPWMOUT,GPIO34
	   EALLOW;
	   GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
	   GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
	   GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
	   GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
	   GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;
	   GpioCtrlRegs.GPBDIR.bit.GPIO60 = 0;
	   EDIS;

	   InitECap1Gpio();

	   InitXintf16Gpio();

	   DINT;

	   InitPieCtrl();

	   IER = 0x0000;
	   IFR = 0x0000;

	   InitPieVectTable();

	   // 3. TFT screen

	   LCD_Init();
	   DSP28x_usDelay(1000);	/* delay 1000 us */
	   POINT_COLOR=RED;
	   BACK_COLOR=WHITE;
	   LCD_Clear(WHITE);
	   LCD_ShowString(70,25,192,16,16,"NVH analysis");
	   LCD_ShowString(100,250,96,16,16,"Rpm:36");
	   LCD_ShowString(30,230,144,16,16,"Vibration(dB):1425");
	   LCD_ShowString(66,125,240,16,16,"Author:OwenShao");
	   InitAdc();

	   EALLOW;
	   PieVectTable.ECAP1_INT = &ecap1_isr;
	   PieVectTable.TINT0 = &cpu_timer0_isr;
	   EDIS;



	   // 4. Initialize MODULE

	      InitCpuTimers();

		  InitECapture();

		  InitForFFT();

		  ADC_INIT();

      // 5. Initialize data and variable
	  // data
		  static Uint16 i;
		  for(i=0; i<BUF_SIZE; i++)
		{
			SampleTable[i] = 0;
			TimeStamp[i] = 0;
		}

	  // variable
		  //ECap
		   ECap1IntCount = 0;
		   ECap1PassCount = 0;




		// 6. Configure timer
		// Timer0
		InitCpuTimers();
		ConfigCpuTimer(&CpuTimer0,150,10,1);
		ConfigCpuTimer(&CpuTimer1,150,10,1);




		// 7. Enable interrupt
		IER |= M_INT4;
		IER |= M_INT1;
//		IER |= M_INT13;
		PieCtrlRegs.PIEIER4.bit.INTx1 = 1;
		PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

		// 8. START
		CpuTimer0Regs.TCR.bit.TSS = 1;
		CpuTimer0Regs.TCR.all = 0x4000;
		CpuTimer0Regs.TCR.bit.TSS = 0;

		CpuTimer1Regs.TCR.bit.TSS = 1;
		CpuTimer1Regs.TCR.all = 0x4000;
		CpuTimer1Regs.TCR.bit.TSS = 0;


		EINT;
		ERTM;




// Step 6. IDLE loop. Just sit and loop forever (optional):
   for(;;)
   {
/*
	   if(ECap1Regs.ECFLG.bit.CTROVF == 1){
		   count++;
		   ECap1Regs.ECCLR.bit.CTROVF = 1;

		   for (i=0;i<SAMPLENUMBER;i++)
		   	          {
		   	       	   fWaveR_R[i]=R_RPM[i];
		   	       	   fWaveI_R[i]=0.0f;
		   	       	   w_V[i]=0.0f;
		   	          }
		   	          FFT(fWaveR_R,fWaveI_R);
		   	          for (i=0;i<SAMPLENUMBER;i++)
		   	          {
		   	       	   R_FFT[i]=-w_R[i];
		   	          }

		   	          InitForFFT();
		   	          for (i=0;i<SAMPLENUMBER;i++)
		   	          {
		   	       	   fWaveR_V[i]=SampleTable[i];
		   	       	   fWaveI_V[i]=0.0f;
		   	       	   w_V[i]=0.0f;
		   	          }
		   	          FFT(fWaveR_V,fWaveI_V);
		   	          for (i=0;i<SAMPLENUMBER;i++)
		   	          {
		   	       	   V_FFT[i]=w_V[i];
		   	          }
	   }


*/
	   ;
   }

}



void InitECapture()
{
   ECap1Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
   ECap1Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
   ECap1Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads
   ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped
   ECap1Regs.ECCTL1.bit.PRESCALE = 0x00;
   // Configure peripheral registers
   ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;      // Continuous
   ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;        // Stop at 4 events
   ECap1Regs.ECCTL1.bit.CAP1POL = 0;          // Falling edge
   ECap1Regs.ECCTL1.bit.CAP2POL = 0;          // Rising edge
   ECap1Regs.ECCTL1.bit.CAP3POL = 1;          // Falling edge
   ECap1Regs.ECCTL1.bit.CAP4POL = 0;          // Rising edge
   ECap1Regs.ECCTL1.bit.CTRRST1 = 0;          // stm operation
   ECap1Regs.ECCTL1.bit.CTRRST2 = 0;          // stm operation
   ECap1Regs.ECCTL1.bit.CTRRST3 = 0;          // stm operation
   ECap1Regs.ECCTL1.bit.CTRRST4 = 0;          // stm operation
   ECap1Regs.ECCTL2.bit.SYNCI_EN = 1;         // Enable sync in
   ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0;        // Pass through
   ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable capture units


   ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
   ECap1Regs.ECCTL2.bit.REARM = 0;            // arm one-shot
   ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
   ECap1Regs.ECEINT.bit.CEVT4 = 1;            // 4 events = interrupt




}
/*
__interrupt void cpu_timer0_isr(void)
{
	GpioDataRegs.GPADAT.bit.GPIO0 = ~(GpioDataRegs.GPADAT.bit.GPIO0);
	;
	CpuTimer0.InterruptCount++;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
*/
__interrupt void cpu_timer0_isr(void)
{
	CpuTimer0.InterruptCount++;
	Sampletime++;
	absolutime++;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;


}

__interrupt void cpu_timer1_isr(void)
{

	CpuTimer1.InterruptCount++;
	if(CAPTIMCTRL == 1)
	M2++;
}

__interrupt void ecap1_isr(void)
{

	CpuTimer0Regs.TCR.bit.TSS = 0;
	CpuTimer1Regs.TCR.bit.TSS = 0;

	CAPTIMCTRL = 1;
	M1++;

//---------------------------Resample------------------------------
   static int16 ST = ST_COUNT;

   volatile Uint16 ADCcheck = 0;

   if(ECap_status<=ST-1)
   {
	   AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
	   GpioDataRegs.GPBSET.bit.GPIO34 = 1;
	   AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;

		   TimeStamp[array_index-1] = absolutime*10.0f;
		   if(array_index>=2)TimeStamp_ref[array_index-1] = (Uint16)(TimeStamp[array_index-1]-TimeStamp[array_index-2]);
		   SampleTable[array_index-1]= (AdcRegs.ADCRESULT0)>>4;
		   GpioDataRegs.GPBCLEAR.bit.GPIO34 = 0;
		   ECap_status++;
		   i++;
		   array_index++;
   }
   else
   {
	   if(JP <= 0)
	   {
		   ECap_status = 0;
		   JP = JP_COUNT - ST_COUNT;
	   }
	   else
	   {
		   JP--;
	   }
   }



//---------------------------Resample------------------------------



//---------------------------M/T method----------------------------

if(Sampletime>=Tc)
{
	n[n_array_index++] = 60*M1*100000/(Z*Sampletime)/270;
	M1 = 0;
	M2 = 0;
	Sampletime = 0;
}
else
{
	;
}
//---------------------------M/T method----------------------------


    ECap1Regs.ECCLR.bit.CEVT4 = 1;
    ECap1Regs.ECCLR.bit.INT = 1;
    ECap1Regs.ECCTL2.bit.REARM = 0;


    if(array_index >= BUF_SIZE)
    {
    	array_index = 0;
    }




   // Acknowledge this interrupt to receive more interrupts from group 4
    PieCtrlRegs.PIEACK.all = 0x0009;
}

//----------------------- Filter function part------------

void median(volatile Uint16 input[BUF_SIZE], Uint16 WS)
{
	Uint16 offset = 0;
	if(offset < BUF_SIZE)
	{
	// WS is set to be an odd number in order to simplify variations
	// Choose medium value of WS can achieve a better result.
	Uint16 windowed_array[WS];
	Uint16 offset = 0;
	Uint16 pointer = 0;
	for (pointer=0;pointer<WS;pointer++)
	{
		windowed_array[pointer] =input[pointer];
	}
	quickSort(windowed_array,0,WS-1);
	input[offset + (WS+1)<<1] = windowed_array[(WS+1)<<1];
	offset++;
	}
}


void swap(Uint16* a,Uint16* b)
{
	Uint16 t = *a;
	*a = *b;
	*b = t;
}

Uint16 partition(Uint16 arr[], Uint16 low, Uint16 high)
{
	Uint16 j;
	Uint16 pivot = arr[high];  // pivot
	Uint16 i = (low -1); 	   // Index of smaller element

	for(j = low;j <= high - 1;j++)
	{
		// If current element is smaller than or
		// equal to pivot
		if(arr[j] <= pivot)
		{
			i++;  // increment index of smaller element
			swap(&arr[i],&arr[j]);
		}
	}
	swap(&arr[i+1],&arr[high]);
	return (i+1);
}

void quickSort(Uint16 arr[], Uint16 low, Uint16 high)
{
	if(low < high)
	{
		/* pi is partitioning index, arr[p] is now
		   at right place
		 */
		Uint16 pi = partition(arr,low,high);
		quickSort(arr,low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}

//----------------------- Filter function part------------


void FFT(float dataR[SAMPLENUMBER],float dataI[SAMPLENUMBER])
{
	int x0,x1,x2,x3,x4,x5,x6,xx;
	int i,j,k,b,p,L;
	float TR,TI,temp;
    // why should I invert the binary sequence of each indice of the array?
    for (i=0;i<SAMPLENUMBER;i++)
    {
    	x0=x1=x2=x3=x4=x5=x6=0;
    	x0=i&0x01; x1=(i/2)&0x01; x2=(i/4)&0x01; x3=(i/8)&0x01; x4=(i/16)&0x01; x5=(i/32)&0x01; x6=(i/64)&0x01;

    	dataI[xx]=dataR[i];
    }
    for (i=0;i<SAMPLENUMBER;i++)
    {
    	dataR[i]=dataI[i];  dataI[i]=0;
    }
    //*******FFT*******//
    for ( L=1;L<=8;L++ )
    {
    	b=1; i=L-1;
    	while ( i>0 )
    	{
    		b=b*2; i--;

    	}
    	p=p*j;
    	for ( k=j; k<64;k=k+2*b)
    	{
    		TR=dataR[k]; TI=dataI[k]; temp=dataR[k+b];
			dataR[k]=dataR[k]+dataR[k+b]*cos_tab[p]+dataI[k+b]*sin_tab[p];
			dataI[k]=dataI[k]-dataR[k+b]*sin_tab[p]+dataI[k+b]*cos_tab[p];
			dataR[k+b]=TR-dataR[k+b]*cos_tab[p]-dataI[k+b]*sin_tab[p];
			dataI[k+b]=TI+temp*sin_tab[p]-dataI[k+b]*cos_tab[p];
		} /* END for (3) */
	} /* END for (2) */
 /* END for (1) */
    for ( i=0;i<SAMPLENUMBER/2;i++ )
    {
    	w[i]=sqrt(dataR[i]*dataR[i]+dataI[i]*dataI[i]);
    }
} /* END FFT */




void InitForFFT()
{
int i;

for ( i=0;i<SAMPLENUMBER;i++ )
{
	sin_tab[i]=sin(PI*2*i/SAMPLENUMBER);
	cos_tab[i]=cos(PI*2*i/SAMPLENUMBER);
}
}


void Fail()
{
    __asm("   ESTOP0");
}




//===========================================================================
// No more.
//===========================================================================
