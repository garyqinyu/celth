#ifndef __CELTH_DEBUG_H
#define __CELTH_DEBUG_H

#ifdef __cplusplus

extern "C" {

#endif



typedef enum {
  CELTHDBG_eTrace=0, 	 /* Trace level */
  CELTHDBG_eMsg=1,     /* Message level */
  CELTHDBG_eWrn=2,     /* Warning level */
  CELTHDBG_eErr=3     /* Error level */
  
} CELTHDBG_Level;


void CelthApi_Debug_Init();

void CelthApi_Debug_SetDebugLevel(CELTHDBG_Level level);


CELTHDBG_Level CelthApi_Debug_GetDebugLevel();



void CelthApi_Debug_EnterFunction(const char *function);


void CelthApi_Debug_LeaveFunction(const char *function);


void CelthApi_Debug_Msg(const char* fmt,...);

void CelthApi_Debug_Wrn(const char* fmt,...);

void CelthAPi_Debug_Err(const char* fmt,...);

#ifdef __cplusplus

}

#endif






#endif
