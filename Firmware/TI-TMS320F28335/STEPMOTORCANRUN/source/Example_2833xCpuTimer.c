//###########################################################################
// Description
//! \addtogroup f2833x_example_list
//! <h1>Cpu Timer (cpu_timer)</h1>
//!
//! This example configures CPU Timer0, 1, and 2 and increments
//! a counter each time the timers asserts an interrupt.
//!
//! \b Watch \b Variables \n
//! - CputTimer0.InterruptCount
//! - CpuTimer1.InterruptCount
//! - CpuTimer2.InterruptCount
//
//###########################################################################
// $TI Release: F2833x/F2823x Header Files and Peripheral Examples V140 $
// $Release Date: March  4, 2015 $
// $Copyright: Copyright (C) 2007-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include  "DSP2833x_EPWMUPAQ.h"

extern volatile struct CPUTIMER_REGS CpuTimer0Regs;

EPWM_INFO epwm1_info;
EPWM_INFO epwm2_info;
EPWM_INFO epwm3_info;

#define RCP_MAX_SMTSPEED 0.016129f;    // The specified value of maxium step motor speed,
#define RCP_STR_SMTSPEED 1.000000f;
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


volatile float CHZ;
volatile float RCPSTRSP = RCP_STR_SMTSPEED;
volatile float CPHLM = CRCP_Hull_PRDNUM;
volatile float CSU = CRCP_STEP_uANG;
volatile Uint16 PS = PWM_PRESCALE ;
volatile Uint16 CD = CCALMD;


// Prototype statements for functions found within this file.
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void xint3_isr(void);
__interrupt void xint4_isr(void);

volatile Uint32 Xint3Count;
volatile Uint32 Xint4Count;
Uint32 LoopCount;


float RPM2HZ(float RCP_rpm, float RCP_Hull_PRDNUM,float RCP_STEP_uANG, Uint16 CALMD);
float HZ2RPM(float hz, float Hull_PRDNUM,float STEP_uANG, Uint16 CALMD);
float RPM2EPR(float RCP_rpm, float RCP_Hull_PRDNUM,float RCP_STEP_uANG, Uint16 CALMD);
void InitEPwm1Example(void);
__interrupt void epwm1_isr(void);
void update_compare(EPWM_INFO*);


// Configure the period for each timer
#define EPWM1_TIMER_TBPRD  0x0345  // Period register
#define EPWM1_MAX_CMPA     1950
#define EPWM1_MIN_CMPA       50
#define EPWM1_MAX_CMPB     1950
#define EPWM1_MIN_CMPB       50



// To keep track of which way the compare value is moving
#define EPWM_CMP_UP   1
#define EPWM_CMP_DOWN 0



void main(void)
{
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2833x_SysCtrl.c file.
   InitSysCtrl();

// Step 2. Initialize GPIO:
// This example function is found in the DSP2833x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
   InitEPwm1Gpio();



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

// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
   EALLOW;  // This is needed to write to EALLOW protected registers
   PieVectTable.TINT0 = &cpu_timer0_isr;
   PieVectTable.EPWM1_INT = &epwm1_isr;
   PieVectTable.XINT3 = &xint3_isr;
   PieVectTable.XINT4 = &xint4_isr;
   PieVectTable.XINT13 = &cpu_timer1_isr;
 //  PieVectTable.TINT2 = &cpu_timer2_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers




// Step 4. Initialize the Device Peripheral. This function can be
//         found in DSP2833x_CpuTimers.c
   InitCpuTimers();   // For this example, only initialize the Cpu Timers
   InitEPwm1Example();

// Configure CPU-Timer 0, 1, and 2 to interrupt every second:
// 150MHz CPU Freq, 1 second Period (in uSeconds)

   ConfigCpuTimer(&CpuTimer0, 150,100000,2);
      CpuTimer1Regs.TCR.bit.TSS = 1;
   ConfigCpuTimer(&CpuTimer1, 150, 10000,2);
//   ConfigCpuTimer(&CpuTimer2, 150, 1000000);




// To ensure precise timing, use write-only instructions to write to the entire register. Therefore, if any
// of the configuration bits are changed in ConfigCpuTimer and InitCpuTimers (in DSP2833x_CpuTimers.h), the
// below settings must also be updated.

   CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
/*   CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
   CpuTimer2Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0 */

   float STARTSP = 0.0f;
   STARTSP  = RPM2EPR(RCPSTRSP, CPHLM, CSU, CD);
   EPwm1Regs.TBPRD = STARTSP;
   CHZ = RCP_STR_SMTSPEED;

   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
   EDIS;

// Step 5. User specific code, enable interrupts:

   Xint3Count = 0; // Count Xint1 interrupts
   Xint4Count = 0; // Count XINT2 interrupts
   LoopCount = 0;  // Count times through idle loop

// Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
// which is connected to CPU-Timer 1, and CPU int 14, which is connected
// to CPU-Timer 2:
   IER |= M_INT1;
   IER |= M_INT3;
   IER |= M_INT12;
// Enable TINT0 in the PIE: Group 1 interrupt 7
   PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
   PieCtrlRegs.PIEIER12.bit.INTx1= 1;          // Enable PIE Gropu 12 INT1
   PieCtrlRegs.PIEIER12.bit.INTx2 = 1;          // Enable PIE Gropu 12 INT2
   PieCtrlRegs.PIEIER3.bit.INTx1 = 0;
   PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

   EALLOW;
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;         // GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO60 = 0;          // input
    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 0;        // Xint1 Synch to SYSCLKOUT only
    GpioCtrlRegs.GPBCTRL.bit.QUALPRD0 = 0xFF;   // Each sampling window is 510*SYSCLKOUT
   EDIS;

   EALLOW;
   GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 0x1c;   //select GPIO28 pin as XINT3 source
   GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 0x1d;   //as above
   EDIS;


// Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global interrupt INTM
   ERTM;   // Enable Global realtime interrupt DBGM

   EALLOW;
   GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;         // GPIO
   GpioCtrlRegs.GPBDIR.bit.GPIO60 = 0;          // input
   GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 0;        // Xint1 Synch to SYSCLKOUT only

   GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;         // GPIO
   GpioCtrlRegs.GPBDIR.bit.GPIO61 = 0;          // input
   GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 2;        // XINT2 Qual using 6 samples
   GpioCtrlRegs.GPBCTRL.bit.QUALPRD0 = 0xFF;   // Each sampling window is 510*SYSCLKOUT
   EDIS;



   EALLOW;
   GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 0x1c;   //select GPIO28 pin as XINT3 source
   GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 0x1d;   //as above
   EDIS;

// Configure XINT3 and XINT4
   XIntruptRegs.XINT3CR.bit.POLARITY = 0;      // Falling edge interrupt
   XIntruptRegs.XINT4CR.bit.POLARITY = 1;      // Rising edge interrupt

// Enable XINT1 and XINT2
   XIntruptRegs.XINT3CR.bit.ENABLE = 1;        // 使能外部中断3
   XIntruptRegs.XINT4CR.bit.ENABLE = 1;        // 使能外部中断4


// 用来指示发生了中断，在中断函数中进行翻转

   LoopCount=0;

// Step 6. IDLE loop. Just sit and loop forever (optional):
   for(;;)
   {
	   LoopCount++;
   }
}

__interrupt void cpu_timer0_isr(void)
{
//   GpioDataRegs.GPADAT.bit.GPIO0 = ~(GpioDataRegs.GPADAT.bit.GPIO0);

   if(CHZ<=400)
   {
	   CHZ+=1;

	   EPwm1Regs.TBPRD = RPM2EPR(1.0f/CHZ, CPHLM, CSU, CD);

   }
   else
   {
	   EPwm1Regs.TBPRD = RPM2EPR(1.0f/400, CPHLM, CSU, CD);
   }


   CpuTimer0.InterruptCount++;

   // Acknowledge this interrupt to receive more interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer1_isr(void)
{


   CpuTimer1.InterruptCount++;

   // Acknowledge this interrupt to receive more interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void epwm1_isr(void)
{
   // Update the CMPA and CMPB values
   update_compare(&epwm1_info);

   // Clear INT flag for this timer
   EPwm1Regs.ETCLR.bit.INT = 1;

   // Acknowledge this interrupt to receive more interrupts from group 3
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void xint3_isr(void)
{

	Xint3Count++;

	// Acknowledge this interrupt to get more from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

__interrupt void xint4_isr(void)
{

	Xint4Count++;

	// Acknowledge this interrupt to get more from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
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
