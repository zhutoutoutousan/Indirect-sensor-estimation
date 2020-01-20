#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include  "DSP2833x_EPWMUPAQ.h"

extern EPWM_INFO epwm1_info;
extern EPWM_INFO epwm2_info;
extern EPWM_INFO epwm3_info;

#define EPWM1_TIMER_TBPRD  0x0345  // Period register
#define EPWM1_MAX_CMPA     1950
#define EPWM1_MIN_CMPA       50
#define EPWM1_MAX_CMPB     1950
#define EPWM1_MIN_CMPB       50

#define EPWM2_TIMER_TBPRD  2000  // Period register
#define EPWM2_MAX_CMPA     1950
#define EPWM2_MIN_CMPA       50
#define EPWM2_MAX_CMPB     1950
#define EPWM2_MIN_CMPB       50

#define EPWM3_TIMER_TBPRD  2000  // Period register
#define EPWM3_MAX_CMPA      950
#define EPWM3_MIN_CMPA       50
#define EPWM3_MAX_CMPB     1950
#define EPWM3_MIN_CMPB     1050

// To keep track of which way the compare value is moving
#define EPWM_CMP_UP   1
#define EPWM_CMP_DOWN 0

void InitEPwm1Example(void);
void InitEPwm2Example(void);
void InitEPwm3Example(void);
void update_compare(EPWM_INFO*);

void InitEPwm1Example()
{
   // Setup TBCLK
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
   EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x7;   // Clock ratio to SYSCLKOUT
   EPwm1Regs.TBCTL.bit.CLKDIV = 0x7;
   EPwm1Regs.TZSEL.all = 0x0000;
   EPwm1Regs.TZCTL.bit.TZB = 3;
   EPwm1Regs.TZCTL.bit.TZA = 3;
   EPwm1Regs.TZEINT.bit.OST = 0;
   EPwm1Regs.TZEINT.bit.CBC = 0;


   // Setup shadow register load on ZERO
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set Compare values
   EPwm1Regs.CMPA.half.CMPA = EPWM1_MIN_CMPA;    // Set compare A value
   EPwm1Regs.CMPB = EPWM1_MIN_CMPB;              // Set Compare B value

   // Set actions
   EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM1A on Zero
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A, up count

   EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;            // Set PWM1B on Zero
   EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Clear PWM1B on event B, up count

   // Interrupt where we will change the Compare Values
   EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
   EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
   EPwm1Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event

   // Information this example uses to keep track
   // of the direction the CMPA/CMPB values are
   // moving, the min and max allowed values and
   // a pointer to the correct ePWM registers
   epwm1_info.EPwm_CMPA_Direction = EPWM_CMP_UP; // Start by increasing CMPA & CMPB
   epwm1_info.EPwm_CMPB_Direction = EPWM_CMP_UP;
   epwm1_info.EPwmTimerIntCount = 0;             // Zero the interrupt counter
   epwm1_info.EPwmRegHandle = &EPwm1Regs;        // Set the pointer to the ePWM module
   epwm1_info.EPwmMaxCMPA = EPWM1_MAX_CMPA;      // Setup min/max CMPA/CMPB values
   epwm1_info.EPwmMinCMPA = EPWM1_MIN_CMPA;
   epwm1_info.EPwmMaxCMPB = EPWM1_MAX_CMPB;
   epwm1_info.EPwmMinCMPB = EPWM1_MIN_CMPB;
}

void InitEPwm2Example()
{
   // Setup TBCLK
   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm2Regs.TBPRD = EPWM2_TIMER_TBPRD;       // Set timer period
   EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm2Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
   EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
   EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV2;

   // Setup shadow register load on ZERO
   EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set Compare values
   EPwm2Regs.CMPA.half.CMPA = EPWM2_MIN_CMPA;       // Set compare A value
   EPwm2Regs.CMPB = EPWM2_MAX_CMPB;                 // Set Compare B value

   // Set actions
   EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR;             // Clear PWM2A on Period
   EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;               // Set PWM2A on event A, up count

   EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;             // Clear PWM2B on Period
   EPwm2Regs.AQCTLB.bit.CBU = AQ_SET;               // Set PWM2B on event B, up count

   // Interrupt where we will change the Compare Values
   EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;        // Select INT on Zero event
   EPwm2Regs.ETSEL.bit.INTEN = 1;                   // Enable INT
   EPwm2Regs.ETPS.bit.INTPRD = ET_3RD;              // Generate INT on 3rd event

   // Information this example uses to keep track
   // of the direction the CMPA/CMPB values are
   // moving, the min and max allowed values and
   // a pointer to the correct ePWM registers
   epwm2_info.EPwm_CMPA_Direction = EPWM_CMP_UP;    // Start by increasing CMPA
   epwm2_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN;  // and decreasing CMPB
   epwm2_info.EPwmTimerIntCount = 0;                // Zero the interrupt counter
   epwm2_info.EPwmRegHandle = &EPwm2Regs;           // Set the pointer to the ePWM module
   epwm2_info.EPwmMaxCMPA = EPWM2_MAX_CMPA;         // Setup min/max CMPA/CMPB values
   epwm2_info.EPwmMinCMPA = EPWM2_MIN_CMPA;
   epwm2_info.EPwmMaxCMPB = EPWM2_MAX_CMPB;
   epwm2_info.EPwmMinCMPB = EPWM2_MIN_CMPB;
}

void InitEPwm3Example(void)
{
   // Setup TBCLK
   EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm3Regs.TBPRD = EPWM3_TIMER_TBPRD;       // Set timer period
   EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm3Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
   EPwm3Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
   EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

   // Setup shadow register load on ZERO
   EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

  // Set Compare values
   EPwm3Regs.CMPA.half.CMPA = EPWM3_MIN_CMPA; // Set compare A value
   EPwm3Regs.CMPB = EPWM3_MAX_CMPB;           // Set Compare B value

   // Set Actions
   EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;         // Set PWM3A on event B, up count
   EPwm3Regs.AQCTLA.bit.CBU = AQ_CLEAR;       // Clear PWM3A on event B, up count

   EPwm3Regs.AQCTLB.bit.ZRO = AQ_TOGGLE;      // Toggle EPWM3B on Zero

   // Interrupt where we will change the Compare Values
   EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
   EPwm3Regs.ETSEL.bit.INTEN = 1;                // Enable INT
   EPwm3Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event

   // Start by increasing the compare A and decreasing compare B
   epwm3_info.EPwm_CMPA_Direction = EPWM_CMP_UP;
   epwm3_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN;
   // Start the count at 0
   epwm3_info.EPwmTimerIntCount = 0;
   epwm3_info.EPwmRegHandle = &EPwm3Regs;
   epwm3_info.EPwmMaxCMPA = EPWM3_MAX_CMPA;
   epwm3_info.EPwmMinCMPA = EPWM3_MIN_CMPA;
   epwm3_info.EPwmMaxCMPB = EPWM3_MAX_CMPB;
   epwm3_info.EPwmMinCMPB = EPWM3_MIN_CMPB;
}

void update_compare(EPWM_INFO *epwm_info)
{
   // Every 10'th interrupt, change the CMPA/CMPB values
   if(epwm_info->EPwmTimerIntCount == 10)
   {
       epwm_info->EPwmTimerIntCount = 0;

       // If we were increasing CMPA, check to see if
       // we reached the max value.  If not, increase CMPA
       // else, change directions and decrease CMPA
	   if(epwm_info->EPwm_CMPA_Direction == EPWM_CMP_UP)
	   {
	       if(epwm_info->EPwmRegHandle->CMPA.half.CMPA < epwm_info->EPwmMaxCMPA)
	       {
	          epwm_info->EPwmRegHandle->CMPA.half.CMPA++;
	       }
	       else
	       {
	          epwm_info->EPwm_CMPA_Direction = EPWM_CMP_DOWN;
              epwm_info->EPwmRegHandle->CMPA.half.CMPA--;
	       }
	   }

	   // If we were decreasing CMPA, check to see if
       // we reached the min value.  If not, decrease CMPA
       // else, change directions and increase CMPA
	   else
	   {
	       if(epwm_info->EPwmRegHandle->CMPA.half.CMPA == epwm_info->EPwmMinCMPA)
	       {
	          epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
	          epwm_info->EPwmRegHandle->CMPA.half.CMPA++;
	       }
	       else
	       {
	          epwm_info->EPwmRegHandle->CMPA.half.CMPA--;
	       }
	   }

	   // If we were increasing CMPB, check to see if
       // we reached the max value.  If not, increase CMPB
       // else, change directions and decrease CMPB
	   if(epwm_info->EPwm_CMPB_Direction == EPWM_CMP_UP)
	   {
	       if(epwm_info->EPwmRegHandle->CMPB < epwm_info->EPwmMaxCMPB)
	       {
	          epwm_info->EPwmRegHandle->CMPB++;
	       }
	       else
	       {
	          epwm_info->EPwm_CMPB_Direction = EPWM_CMP_DOWN;
	          epwm_info->EPwmRegHandle->CMPB--;
	       }
	   }

	   // If we were decreasing CMPB, check to see if
       // we reached the min value.  If not, decrease CMPB
       // else, change directions and increase CMPB

	   else
	   {
	       if(epwm_info->EPwmRegHandle->CMPB == epwm_info->EPwmMinCMPB)
	       {
	          epwm_info->EPwm_CMPB_Direction = EPWM_CMP_UP;
	          epwm_info->EPwmRegHandle->CMPB++;
	       }
	       else
	       {
	          epwm_info->EPwmRegHandle->CMPB--;
	       }
	   }
   }
   else
   {
      epwm_info->EPwmTimerIntCount++;
   }

   return;
}
