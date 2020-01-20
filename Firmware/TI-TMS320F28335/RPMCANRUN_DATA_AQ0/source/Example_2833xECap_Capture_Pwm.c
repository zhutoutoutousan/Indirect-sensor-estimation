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

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;

//---------------------------- ADC define ------------------------------------------
//	define for ADC
#define POST_SHIFT   0  // Shift results after the entire sample table is full
#define INLINE_SHIFT 1  // Shift results as the data is taken from the results regsiter
#define NO_SHIFT     0  // Do not shift the results

// ADC start parameters

#define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#define ADC_CKPS   0x0   // ADC module clock = HSPCLK/1      = 25.5MHz/(1)   = 25.0 MHz
#define ADC_SHCLK  0x1   // S/H width in ADC module periods                  = 2 ADC cycle
#define AVG        1000  // Average sample limit
#define ZOFFSET    0x00  // Average Zero offset
#define BUF_SIZE   512  // Sample buffer size
#define NUM_HULL   1    // the number of pulses per revolution
#define CAPTIMER   ECap1Regs.CAP1

// ----------------------------------------ADC define--------------------------------------

// define and global variable for FFT
#define SAMPLENUMBER 64//1024
#define PI 3.1415926
#define NR(x) (sizeof(x)/sizeof(x[0]))
// end define


void InitForFFT();
void MakeWave();

#define HULL_NUMBER 68    //
// Global variable for this example
Uint16 SampleTable[BUF_SIZE];
Uint16 DataTable[BUF_SIZE];
Uint16 TimeStamp[BUF_SIZE];
volatile Uint16 av = 0;
volatile float avtmp = 0;
volatile Uint32 Count[2];
volatile float R_RPM[BUF_SIZE] ;
volatile float R_CRPM;
volatile Uint32 R_CT = 0;
volatile Uint32 R_PT = 0;
volatile Uint16 INITIALIZED1 = 0;
volatile Uint16 INITIALIZED2 = 0;
volatile Uint16 INITIALIZED3 = 0;
volatile Uint16 INITIALIZED4 = 0;
volatile Uint16 CTINITED = 0;
volatile Uint16 PTINITED = 0;
volatile Uint16 flag = 0;
volatile Uint16 PRE_COUNT = 0;
volatile Uint16 counti = 0;
volatile Uint32 tmp = 0;
Uint16 j;
volatile float INPUT[SAMPLENUMBER],DATA[SAMPLENUMBER];
volatile float R_FFT[SAMPLENUMBER],V_FFT[SAMPLENUMBER];
volatile float fWaveR[SAMPLENUMBER],fWaveI[SAMPLENUMBER],w_V[SAMPLENUMBER],w_R[SAMPLENUMBER],w[SAMPLENUMBER];
volatile float fWaveR_R[SAMPLENUMBER],fWaveI_R[SAMPLENUMBER],fWaveR_V[SAMPLENUMBER],fWaveI_V[SAMPLENUMBER];
volatile float sin_tab[SAMPLENUMBER],cos_tab[SAMPLENUMBER];
Uint16 SampleTable[BUF_SIZE];


volatile Uint16 countindex = 0;
volatile Uint16 array_index = 0;
volatile Uint16 count = 0;
volatile Uint16 timer_index = 0;
__interrupt void ecap1_isr(void);
//__interrupt void cpu_timer0_isr(void);
void InitECapture(void);
void Fail(void);
void ADC_INIT(void);

// Global variables used in this example
volatile Uint32  ECap1IntCount;
volatile Uint32  ECap1PassCount;
// To keep track of which way the timer value is moving

// Define Cputimer parameters
//__interrupt void cpu_timer0_isr(void);

void main(void)
{
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

   static Uint16 i;


   InitSysCtrl();

   EALLOW;
   SysCtrlRegs.HISPCP.all = ADC_MODCLK;
   EDIS;

   EALLOW;
   GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0; // GPIO
   GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;  // Output
   GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
   GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
   EDIS;


   InitECap1Gpio();

   InitXintf16Gpio();
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
   DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2833x_PieCtrl.c file.
   InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
// This function is found in DSP2833x_PieVect.c.
   InitPieVectTable();

   InitAdc();


// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
   EALLOW;  // This is needed to write to EALLOW protected registers
   PieVectTable.ECAP1_INT = &ecap1_isr;
 //  PieVectTable.TINT0 = &cpu_timer0_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers

// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP2833x_InitPeripherals.c
// InitPeripherals();  // Not required for this example

   //initialize GPIO I don't understand why this can enable interrupt.
      EALLOW;
	  GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;
	  GpioCtrlRegs.GPBDIR.bit.GPIO60 = 0;
	  EDIS;

//	  InitCpuTimers();

      InitECapture();

      InitForFFT();

// Step 5. User specific code, enable interrupts:

// Specific ADC setup for this example:
      ADC_INIT();


      for(i=0; i<BUF_SIZE; i++)
    {
    	SampleTable[i] = 0;
    	R_RPM[i] = 0;
    }

    /*
    ConfigCpuTimer(&CpuTimer0,150,100000);
    CpuTimer1Regs.TCR.bit.TSS = 1;
    CpuTimer0Regs.TCR.all = 0x4000;
	*/


// Initialize counters:
   ECap1IntCount = 0;
   ECap1PassCount = 0;

// Enable CPU INT4 which is connected to ECAP1-4 INT:
   IER |= M_INT4;
//   IER |= M_INT1;
// Enable eCAP INTn in the PIE: Group 3 interrupt 1-6
   PieCtrlRegs.PIEIER4.bit.INTx1 = 1;
   PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

   InitCpuTimers();
   ConfigCpuTimer(&CpuTimer0, 150, 1000000);
   CpuTimer1Regs.TCR.bit.TSS = 0;

// Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global interrupt INTM
   ERTM;



// Step 6. IDLE loop. Just sit and loop forever (optional):
   for(;;)
   {


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
__interrupt void ecap1_isr(void)
{
    //  volatile Uint16 array_index;
   volatile Uint16 i;

   //Variable to monitor whether the rotation signal is triggering interrupt.
/*
   PRE_COUNT = counti;

   if(ECap1Regs.ECFLG.bit.INT == 1)
   {
	   counti++;
   }

   if(counti == 1)
   {
	   R_PT = ECap1Regs.CAP1;
   }
   if(counti > 1 )
   {
	   R_CT = ECap1Regs.CAP1;
	   R_RPM[array_index] = (150000000*60)/((R_CT-R_PT)*68);
	   R_CRPM = R_RPM[array_index];
	   while(counti-PRE_COUNT >3)
	   {
		   R_PT = R_CT;
		   R_CT = ECap1Regs.CAP1;
	   }

   }
   if(counti > 128)
   {
	   counti = 3;
   }

   while(PRE_COUNT <= counti -5)
   {
	   PRE_COUNT = counti;
   }


   ECap1IntCount++;
       if(ECap1IntCount > 1023)ECap1IntCount = 0;
       ECap1IntCount++;
*/

   	   if(flag == 0)
   	   {
	   R_CT = ECap1Regs.CAP1;
	   INITIALIZED1 = 1;
	   DELAY_US(15000);


	   R_PT = ECap1Regs.CAP1;
	   R_CT = tmp;
	   INITIALIZED2 = 1;
	   DELAY_US(15000);


	   R_PT = tmp;
	   R_CT = ECap1Regs.CAP1;
	   INITIALIZED3 = 1;
	   DELAY_US(15000);

	   flag = 1;
   	   }

	   R_PT = R_CT;
	   R_CT = ECap1Regs.CAP1;
	   R_RPM[array_index] = (float)(150000000*60)/((float)((R_CT-R_PT)*68)/(float)232);
	   tmp = 0;
	   for(av=0;av<10;av++)
	   {
	      tmp = tmp + R_RPM[av];
	   }
	   if(tmp<3000*10)R_CRPM = (float)tmp/(float)(10);



   if(ECap1IntCount > 1023)ECap1IntCount = 0;   // Question remains why it pre-overflows before reaching 65535?


   PRE_COUNT=ECap1IntCount;

   ECap1IntCount++;
       if(ECap1PassCount > 1023)ECap1PassCount = 0;
       ECap1PassCount++;

       counti++;
   /*
   if((R_PT>R_CT)&(PASSED==0))
   {

	   R_CT = ECap1Regs.CAP1;
	   R_RPM[array_index] = (150000000*60)/((R_PT-R_CT)*HULL_NUMBER);
   	   R_CRPM = R_RPM[array_index];
   	   R_PT = R_CT;


   }
   else
   {
	   if(PASSED==0)
	   {
			 R_PT = ECap1Regs.CAP1;
			 PASSED = 1;
	   }
	   else
	   {
		   if(R_PT<R_CT)
		   {
			   if(R_PT = R_CT)
				   {
				   PASSED = 1;
				   }
			   else
			   	 {

				   R_CT = ECap1Regs.CAP1;
				   R_RPM[array_index] = (150000000*60)/((R_CT-R_PT)*HULL_NUMBER);
				   R_CRPM = R_RPM[array_index];
			   	   }
			}
		   else
		   {
			   R_CT = ECap1Regs.CAP1;
			   PASSED = 1;
		   }
	   }
   }
	*/


	   // Wait for int1
           AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
		   GpioDataRegs.GPBSET.bit.GPIO34 = 1;
		   AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;



#if INLINE_SHIFT
/* using CpuTimer0 as counter
           TimeStamp[array_index] = CpuTimer0.InterruptCount;
           //Every past loop of Count is a time of 0.1s.
	       R_RPM[array_index] = (TimeStamp[array_index] - TimeStamp[array_index-1])*60*NUM_HULL;
	       R_CRPM = R_RPM[array_index];
*/
	       SampleTable[array_index++]= ( (AdcRegs.ADCRESULT0)>>4);


#endif


    GpioDataRegs.GPBCLEAR.bit.GPIO34 = 0;  // Clear GPIO34 for monitoring  -optional

    ECap1Regs.ECCLR.bit.CEVT4 = 1;
    ECap1Regs.ECCLR.bit.INT = 1;
    ECap1Regs.ECCTL2.bit.REARM = 0;


    if(array_index >= 127)
    {
    	array_index = 0;
    }




   // Acknowledge this interrupt to receive more interrupts from group 4
    PieCtrlRegs.PIEACK.all = 0x0009;
}

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
