#ifndef DSP2833x_EPWMUPAQ_H
#define DSP2833x_EPWMUPAQ_H
#endif


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
   volatile struct EPWM_REGS *EPwmRegHandle;
   Uint16 EPwm_CMPA_Direction;
   Uint16 EPwm_CMPB_Direction;
   Uint16 EPwmTimerIntCount;
   Uint16 EPwmMaxCMPA;
   Uint16 EPwmMinCMPA;
   Uint16 EPwmMaxCMPB;
   Uint16 EPwmMinCMPB;
}EPWM_INFO;
