//###########################################################################
//
// FILE:    DSP2833x_CpuTimers.c
//
// TITLE:   CPU 32-bit Timers Initialization & Support Functions.
//
// NOTES:   CpuTimer2 is reserved for use with DSP BIOS and
//          other realtime operating systems.
//
//          Do not use these this timer in your application if you ever plan
//          on integrating DSP-BIOS or another realtime OS.
//
//###########################################################################
// $TI Release: F2833x/F2823x Header Files and Peripheral Examples V140 $
// $Release Date: March  4, 2015 $
// $Copyright: Copyright (C) 2007-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "DSP2833x_Device.h"     // Headerfile Include File
#include "DSP2833x_Examples.h"   // Examples Include File

struct CPUTIMER_VARS CpuTimer0;

// When using DSP BIOS & other RTOS, comment out CPU Timer 2 code.
struct CPUTIMER_VARS CpuTimer1;
struct CPUTIMER_VARS CpuTimer2;

//---------------------------------------------------------------------------
// InitCpuTimers:
//---------------------------------------------------------------------------
// This function initializes all three CPU timers to a known state.
//
void InitCpuTimers(void)
{
    // CPU Timer 0
    // Initialize address pointers to respective timer registers:
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    // Initialize timer period to maximum:
    CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    CpuTimer0Regs.TPR.all  = 0;
    CpuTimer0Regs.TPRH.all = 0;
    // Make sure timer is stopped:
    CpuTimer0Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    CpuTimer0Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer0.InterruptCount = 0;


// CpuTimer2 is reserved for DSP BIOS & other RTOS
// Do not use this timer if you ever plan on integrating
// DSP-BIOS or another realtime OS.

    // Initialize address pointers to respective timer registers:
    CpuTimer1.RegsAddr = &CpuTimer1Regs;
    CpuTimer2.RegsAddr = &CpuTimer2Regs;
    // Initialize timer period to maximum:
    CpuTimer1Regs.PRD.all  = 0xFFFFFFFF;
    CpuTimer2Regs.PRD.all  = 0xFFFFFFFF;
    // Make sure timers are stopped:
    CpuTimer1Regs.TCR.bit.TSS = 1;
    CpuTimer2Regs.TCR.bit.TSS = 1;
    // Reload all counter register with period value:
    CpuTimer1Regs.TCR.bit.TRB = 1;
    CpuTimer2Regs.TCR.bit.TRB = 1;
    // Reset interrupt counters:
    CpuTimer1.InterruptCount = 0;
    CpuTimer2.InterruptCount = 0;

}

//---------------------------------------------------------------------------
// ConfigCpuTimer:
//---------------------------------------------------------------------------
// This function initializes the selected timer to the period specified
// by the "Freq" and "Period" parameters. The "Freq" is entered as "MHz"
// and the period in "uSeconds". The timer is held in the stopped state
// after configuration.
//
void ConfigCpuTimer(struct CPUTIMER_VARS *Timer, Uint32 Freq, Uint32 Period, Uint32 Prescale)
{
    Uint32  temp;
    Uint16  SETPSC;
    Uint16  SETPSCH;

    // Initialize timer period:

    SETPSCH = (Uint16)Prescale >> 8;
    // Prescale is set to be 1,2,3,4,5,6,7... but better not too much, only compensating for
    // Clock trip is enough.
    if((Uint16)Prescale >> 8 > 0)
    {
     SETPSC  = (Prescale - ((Uint16)Prescale&0xFF00)) ;
    }
    else SETPSC = Prescale;
    Timer->CPUFreqInMHz = Freq;
    Timer->PeriodInUSec = Period;
    temp = (long) (Period*Freq/Prescale);
    Timer->RegsAddr->PRD.all = temp;


    Timer->RegsAddr->TPR.all  = 0;
    Timer->RegsAddr->TPRH.all  = 0;

    // Initialize timer control register:
    Timer->RegsAddr->TCR.bit.TSS = 1;      // 1 = Stop timer, 0 = Start/Restart Timer
    Timer->RegsAddr->TCR.bit.TRB = 1;      // 1 = reload timer
    Timer->RegsAddr->TCR.bit.SOFT = 1;
    Timer->RegsAddr->TCR.bit.FREE = 0;     // Timer Free Run
    Timer->RegsAddr->TCR.bit.TIE = 1;      // 0 = Disable/ 1 = Enable Timer Interrupt
    Timer->RegsAddr->TPRH.bit.TDDRH =SETPSCH ;
    Timer->RegsAddr->TPR.bit.TDDR  = SETPSC;
    Timer->RegsAddr->TPR.bit.PSC  = 0x00;
    Timer->RegsAddr->TPRH.bit.PSCH = 0x00;
//    Timer->RegsAddr->PRD.all = 0x08F0D180;
    Timer->RegsAddr->TIM.all = 0x00000000;
    // Reset interrupt counter:
    Timer->InterruptCount = 0;
    Timer->RegsAddr->TCR.bit.TSS = 0;
}

//===========================================================================
// End of file.
//===========================================================================
