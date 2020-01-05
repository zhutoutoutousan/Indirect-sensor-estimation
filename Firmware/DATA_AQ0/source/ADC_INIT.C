#include "DSP28x_Project.h"

#define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#define ADC_CKPS   0x0   // ADC module clock = HSPCLK/1      = 25.5MHz/(1)   = 25.0 MHz
#define ADC_SHCLK  0x1   // S/H width in ADC module periods                  = 2 ADC cycle
void ADC_INIT(void);
// Specific ADC setup for this example:
void ADC_INIT(void)
{
   AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;  // Sequential mode: Sample rate   = 1/[(2+ACQ_PS)*ADC clock in ns]
                        //                     = 1/(3*40ns) =8.3MHz (for 150 MHz SYSCLKOUT)
					    //                     = 1/(3*80ns) =4.17MHz (for 100 MHz SYSCLKOUT)
					    // If Simultaneous mode enabled: Sample rate = 1/[(3+ACQ_PS)*ADC clock in ns]
   AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
   AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1  Cascaded mode
  // AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x1;
   AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       // Setup continuous run

   AdcRegs.ADCTRL1.bit.SEQ_OVRD = 1;       // Enable Sequencer override feature

   AdcRegs.ADCCHSELSEQ1.all = 0x0001;         // Initialize all ADC channel selects to A0
   AdcRegs.ADCCHSELSEQ2.all = 0x0001;
   AdcRegs.ADCCHSELSEQ3.all = 0x0001;
   AdcRegs.ADCCHSELSEQ4.all = 0x0001;
   AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x7;  // convert and store in 8 results registers
}
