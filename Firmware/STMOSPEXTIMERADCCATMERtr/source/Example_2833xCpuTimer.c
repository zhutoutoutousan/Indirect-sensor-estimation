//###########################################################################
// Description

//###########################################################################

//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include  "DSP2833x_EPWMUPAQ.h"
#include <string.h>

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern volatile struct CPUTIMER_REGS CpuTimer0Regs;


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

EPWM_INFO epwm1_info;
EPWM_INFO epwm2_info;
EPWM_INFO epwm3_info;
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
volatile float R_CRPM = 0;
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



#define RCP_MAX_SMTSPEED 0.016129f;    // The specified value of maxium step motor speed,
#define RCP_STR_SMTSPEED 0.000000f;
#define CCALMD           0
#define CRCP_Hull_PRDNUM 0.014705f;
#define CRCP_STEP_uANG   0.625000f;
#define PWM_PRESCALE     14;
#define DELAY 35.700L
// for our test model motor, Hull sensor returns a mean of 240 in Hz, the unit
// rolation angle is 1.6 degrees. Then MAX_SMTSPEED = 240*1.6/360*60 = 64 rpm.
// shoule be the nominal rpm. Start speed is 1 rpm, hullPRDNUM is 68. RCP rep
// -resents THE RECIPRICAL NUMBER OF.

// The two basic formula for step motor control is:
// f_Hz = RPM*360/(StepUANG*60) = 1/TTPRD = 1/(TPRDC/(CPUCLK/Prescale))
// 		= CPUCLK/(TPRDC*Prescale)
// TPRDC = CPUCLK/(f_Hz*Prescale)
// RPM = (f_Hz*StepUANG/360)*60
//     = f_Hz*StepUANG*60/360
// 	   = CPUCLK/(TPRDC*Prescale)*StepUANG*60/360
//     = CPUCLK*StepUANG*60/(TPRDC*360*Prescale)
// TPRDC = CPUCLK*StepUANG*60/(RPM*360*Prescale)

// For our motor, assume that the maximum RPM is 64,then
// f_Hz = 64*360/(1.6*60) = 240 Hz
// TPRDC = 150e6/(240*14*128) = 348 = 0x015c
// TPRDC = 150e6*1.6*60/(64*360*128*14) = 348 = 0x015c

// Following defines the controlling state of our motor
#define START_STRAIGHT 1
#define RPM_INC 2
#define STOP_STRAIGHT 3
#define RPM_DEC 4
volatile Uint16 SYSCTRLF = 0;   // This variable is where the external interrupt controls the
// Start and stop of the motor


volatile float CHZ = 0.0f;
volatile float RCPSTRSP = RCP_STR_SMTSPEED;
volatile float CPHLM = CRCP_Hull_PRDNUM;
volatile float CSU = CRCP_STEP_uANG;
volatile Uint16 PS = PWM_PRESCALE ;
volatile Uint16 CD = CCALMD;


#define STABLE 0
#define INCREASE 1
#define DECREASE 2
#define STOP 3
#define START 5
volatile Uint16 START_CON = STABLE;

// Prototype statements for functions found within this file.
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);

__interrupt void ecap1_isr(void);

/*
 * for future use
float RPM2HZ(float RCP_rpm, float RCP_Hull_PRDNUM,float RCP_STEP_uANG, Uint16 CALMD);
float HZ2RPM(float hz, float Hull_PRDNUM,float STEP_uANG, Uint16 CALMD);
float RPM2EPR(float RCP_rpm, float RCP_Hull_PRDNUM,float RCP_STEP_uANG, Uint16 CALMD);
*/

void InitECapture(void);
void Fail(void);
void ADC_INIT(void);
volatile Uint32  ECap1IntCount;
volatile Uint32  ECap1PassCount;


void main(void)
{

   MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

   static Uint16 i;

   InitSysCtrl();

   InitECap1Gpio();

   EALLOW;
   SysCtrlRegs.HISPCP.all = ADC_MODCLK;
   EDIS;



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
   PieVectTable.TINT0 = &cpu_timer0_isr;
   PieVectTable.ECAP1_INT = &ecap1_isr;
 //  PieVectTable.TINT2 = &cpu_timer2_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers



// Step 4. Initialize the Device Peripheral. This function can be
//         found in DSP2833x_CpuTimers.c
   InitCpuTimers();   // For this example, only initialize the Cpu Timers
   InitECapture();
   InitForFFT();
   ADC_INIT();
// Configure CPU-Timer 0, 1, and 2 to interrupt every second:
// 150MHz CPU Freq, 1 second Period (in uSeconds)

// To ensure precise timing, use write-only instructions to write to the entire register. Therefore, if any
// of the configuration bits are changed in ConfigCpuTimer and InitCpuTimers (in DSP2833x_CpuTimers.h), the
// below settings must also be updated.

   CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
   CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
//   CpuTimer2Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0 */

   float STARTSP = 0.0f;
   STARTSP  = RPM2EPR(RCPSTRSP, CPHLM, CSU, CD);
   EPwm1Regs.TBPRD = STARTSP;
   CHZ = RCP_STR_SMTSPEED;

   for(i=0; i<BUF_SIZE; i++)
 {
 	SampleTable[i] = 0;
 	R_RPM[i] = 0;
 }

   ECap1IntCount = 0;
   ECap1PassCount = 0;


// Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
// which is connected to CPU-Timer 1, and CPU int 14, which is connected
// to CPU-Timer 2:
   IER |= M_INT1;
   IER |= M_INT4;

// Enable TINT0 in the PIE: Group 1 interrupt 7
   PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
   PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
   PieCtrlRegs.PIEIER4.bit.INTx1 = 1;




   ConfigCpuTimer(&CpuTimer0, 150,25000,20000);

   ConfigCpuTimer(&CpuTimer1, 150, 100000,25000);

//   ConfigCpuTimer(&CpuTimer2, 150, 1000000);
   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
   EDIS;
   CpuTimer0Regs.TCR.bit.TSS = 0;
   CpuTimer1Regs.TCR.bit.TSS = 0;













// Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global interrupt INTM
   ERTM;   // Enable Global realtime interrupt DBGM




// Step 6. IDLE loop. Just sit and loop forever (optional):
   for(;;)
   {
	   ;


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
__interrupt void cpu_timer0_isr(void)
{


	   CpuTimer0.InterruptCount++;

	   // Acknowledge this interrupt to receive more interrupts from group 1
	   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer1_isr(void)
{
   CpuTimer1Regs.TCR.bit.TIF = 1;
   CpuTimer1.InterruptCount++;

   // Acknowledge this interrupt to receive more interrupts from group 1
   EDIS;
}



float RPM2HZ(float RCP_rpm, float RCP_Hull_PRDNUM,float RCP_STEP_uANG, Uint16 CALMD)
{

	// This function is waiting for furthur improvement by adding a choosing mode between
	// internal PWM output control and external Hull sensor closed loop control which req
	// -uires a Ecap module to process speed in the other program pending for merging.
	float TMP;

    if(CALMD == 1)
    {
    	TMP = RCP_Hull_PRDNUM * RCP_rpm;
    }
    else
    {
    	if(CALMD == 0)
    	{
    		//TMP = (60.0f*150000000.0f*RCP_rpm)/(RCP_STEP_uANG*360.0f*(float)PS);
    		TMP = (RCP_STEP_uANG*360.0f)/(RCP_rpm*60.0f);
    	}
    	else CALMD = 0;
    }

    return TMP;

}
float RPM2EPR(float RCP_rpm, float RCP_Hull_PRDNUM,float RCP_STEP_uANG, Uint16 CALMD)
{

	// This function is waiting for furthur improvement by adding a choosing mode between
	// internal PWM output control and external Hull sensor closed loop control which req
	// -uires a Ecap module to process speed in the other program pending for merging.
	float TMP;
/*
    CALMD = 0;
    RCP_STEP_uANG = 0.625000f;
    RCP_rpm = 1.000000f;
*/
    if(CALMD == 1)
    {
    	TMP = RCP_Hull_PRDNUM * RCP_rpm;
    }
    else
    {
    	if(CALMD == 0)
    	{
    		TMP = (60.0f*150000000.0f*RCP_rpm)/(RCP_STEP_uANG*360.0f*128.0f*14.0f);
    		//TMP = (RCP_STEP_uANG*360.0f)/(RCP_rpm*60.0f);
    	}
    	else CALMD = 0;
    }

    return TMP;

}

float HZ2RPM(float hz, float Hull_PRDNUM,float STEP_uANG, Uint16 CALMD)
{
	// This function is waiting for furthur improvement by adding a choosing mode between
	// internal PWM output control and external Hull sensor closed loop control which req
	// -uires a Ecap module to process speed in the other program pending for merging.
    Uint16 OUTR;
    if(CALMD == 1)
    {
    	OUTR = hz/Hull_PRDNUM;
    }
    else
    {
    	if(CALMD == 0)
    	{
    		OUTR = (hz*STEP_uANG)/360;
    	}
    	else CALMD = 0;
    }
    return OUTR;
}
__interrupt void ecap1_isr(void)
{
	volatile Uint16 q;
	volatile float RPM = 0.0f;
	volatile float DELTAT = 0.0f;
	if(q>=512)q = 0;
    //  volatile Uint16 array_index;

   TimeStamp[q++] = CpuTimer1.InterruptCount;
   while(TimeStamp[1]!=0)
   {
	   DELTAT = 0.1*(TimeStamp[q]-TimeStamp[q-1]);
	   R_CRPM = (1.6f/360.0f)/DELTAT;
   }
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
/*
   	   if(flag == 0)
   	   {
	   R_CT = ECap1Regs.CAP1;
	   INITIALIZED1 = 1;
	   DELAY_US(150);


	   R_PT = ECap1Regs.CAP1;
	   R_CT = tmp;
	   INITIALIZED2 = 1;
	   DELAY_US(150);


	   R_PT = tmp;
	   R_CT = ECap1Regs.CAP1;
	   INITIALIZED3 = 1;
	   DELAY_US(150);

	   flag = 1;
   	   }

	   R_PT = R_CT;
	   R_CT = ECap1Regs.CAP1;
	   R_RPM[array_index] = (150000000.0f*60.0f)/((float)((R_CT-R_PT)*68.0f))*24.5f;
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
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
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
