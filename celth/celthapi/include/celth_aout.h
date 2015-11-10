#ifndef _CELTH_AUDIOOUT_H_
#define _CELTH_AUDIOOUT_H_


#ifdef __cplusplus

extern "C" {

#endif

#include "celth_stddef.h"

typedef enum  CELTH_AOUT_DEVICE_e
{
    CELTH_AOUT_DEVICE_ANALOG_STERO = 0x01, 
    CELTH_AOUT_DEVICE_ANALOG_MUTI  = 0x02, 
    CELTH_AOUT_DEVICE_SPDIF        = 0x04, 
    CELTH_AOUT_DEVICE_HDMI         = 0x08, 
    CELTH_AOUT_DEVICE_I2S          = 0x10, 
    CELTH_AOUT_DEVICE_SPEAKER      = 0x20, 
    CELTH_AOUT_DEVICE_ALL          = 0xff  
} CELTH_AOUT_DEVICE;







typedef enum CELTH_AOUT_DEVICE_SPDIF_e{
    CELTH_AOUT_DEVICE_SPDIF_OFF = 0,     
    CELTH_AOUT_DEVICE_SPDIF_RAW = 1,      
    CELTH_AOUT_DEVICE_SPDIF_PCM = 2       
}CELTH_AOUT_DEVICE_SPDIF;     

typedef struct Celth_EquOutput_Param_t
{
    CELTH_UINT     af;        
    CELTH_UINT     value;    
} Celth_EquOutput_Param;
/* audio output */

CELTH_VOID  CelthApi_AudioOut_Init ();
CELTH_VOID  CelthApi_AudioOut_Exit   ();

CELTH_INT  CelthApi_AudioOut_Get_Num();

CELTH_HANDLE CelthApi_AudioOut_Open   (CELTH_INT index);
CELTH_Error_Code  CelthApi_AudioOut_Close  (CELTH_HANDLE aout);

CELTH_Error_Code CelthApi_AudioOut_SetValume(CELTH_HANDLE aout, CELTH_INT valume);

CELTH_INT  CelthApi_AudioOut_GetValume(CELTH_HANDLE aout);


CELTH_VOID CelthApi_AudioOut_SetOutputDev(CELTH_HANDLE aout, CELTH_AOUT_DEVICE outdev);

CELTH_VOID CelthApi_AudioOut_SetSPDIFOutput(CELTH_HANDLE aout, CELTH_AOUT_DEVICE_SPDIF spdif);


#ifdef __cplusplus

}

#endif


#endif /*_CELTH_AUDIOOUT_H_*/

