/********************************************************************************
COPYRIGHT (C) STMicroelectronics 2000.

File name       : sv_peg.h 
Author          : Jacky Burban
Description     : 
Creation        : 26/10/01
Modification    :
	  Date                  DDTS Number       		Modification       	     Name 
	  ===========       		============          ===========              =====
	  
********************************************************************************/

/* ----------------------------------------------------------------------------*/
/* Define to prevent recursive inclusion --------------------------------------*/
/* ----------------------------------------------------------------------------*/

#ifndef __SV_PEG_H
#define __SV_PEG_H

/* ----------------------------------------------------------------------------*/
/* C++ compatibility ----------------------------------------------------------*/
/* ----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stddefs.h"
#include "handle.h"
#include "stosd.h"

/* Define the transparent color for PEG. This color must be initialized as transparent
   when initializing STOSD region */
#define PegTransparentColor 0xFF

/* error code */
typedef enum
{
	SVPEG_kNoError       , /* No error detected */
	SVPEG_kExternalError , /* Error happened in called component */
	SVPEG_kBadParameter  , /* one or more parameter is invalid */
	SVPEG_kNotInitiated  , /* component is not initialized */
	SVPEG_kInvalidHandle , /* given handle is invalid */
	SVPEG_kInvalidCall     /* initialize already done => invalid call to the function */
} SVPEG_ErrorCode_t;

/* define Init parameters structure*/
typedef struct{
	STOSD_RegionHandle_t RegionHandle;
	int xSize;
	int ySize;
}SVPEG_InitParams_t;

/* function prototypes definition */
SVPEG_ErrorCode_t SVPEG_TimerTick(HDL_Handle_t CnxHandle, const int iTimerRef); /* function to be called for timer update. to be called each 200ms */
SVPEG_ErrorCode_t SVPEG_Init(SVPEG_InitParams_t stInitParams); /* function to initialialize PEG */
SVPEG_ErrorCode_t SVPEG_Open(HDL_Handle_t* pCnxHandle); /* open a connection to peg */
SVPEG_ErrorCode_t SVPEG_Close( HDL_Handle_t CnxHandle ); /* close connection to peg */
SVPEG_ErrorCode_t SVPEG_Terminate(void); /* terminate peg module */
SVPEG_ErrorCode_t SVPEG_KeyUp(HDL_Handle_t CnxHandle); /* Key up pressed on user interface */
SVPEG_ErrorCode_t SVPEG_KeyDown(HDL_Handle_t CnxHandle);/* Key down pressed on user interface */
SVPEG_ErrorCode_t SVPEG_KeyLeft(HDL_Handle_t CnxHandle);/* Key left pressed on user interface */
SVPEG_ErrorCode_t SVPEG_KeyRight(HDL_Handle_t CnxHandle);/* Key right pressed on user interface */
SVPEG_ErrorCode_t SVPEG_KeySelect(HDL_Handle_t CnxHandle);/* OK Key pressed on user interface */
SVPEG_ErrorCode_t SVPEG_KeyAlt(HDL_Handle_t CnxHandle);/* go to the menu object */
SVPEG_ErrorCode_t SVPEG_KeyCtrlTab(HDL_Handle_t CnxHandle);/* go to the next window */
SVPEG_ErrorCode_t SVPEG_KeyShiftCtrlTab(HDL_Handle_t CnxHandle);/* go to the previous window */
SVPEG_ErrorCode_t SVPEG_KeyTab(HDL_Handle_t CnxHandle);/* go to the next objet in the window */
SVPEG_ErrorCode_t SVPEG_KeyShiftTab(HDL_Handle_t CnxHandle); /* go to the previous object in the window */
SVPEG_ErrorCode_t SVPEG_KeyScrollUp(HDL_Handle_t CnxHandle); /* scroll window content up */
SVPEG_ErrorCode_t SVPEG_KeyScrollDown(HDL_Handle_t CnxHandle);/* scroll window content down */
SVPEG_ErrorCode_t SVPEG_CloseWindow(HDL_Handle_t CnxHandle);/* close current window */
SVPEG_ErrorCode_t SVPEG_AddWindow(HDL_Handle_t CnxHandle, void *pChild); /* add a new window on the screen */
SVPEG_ErrorCode_t SVPEG_KeyCharValue(HDL_Handle_t CnxHandle, U8 ucKeyValue); /* transfer this key ascii code to the current object */
SVPEG_ErrorCode_t SVPEG_KeyValueInc(HDL_Handle_t CnxHandle); /* increase value contained in the current object */
SVPEG_ErrorCode_t SVPEG_KeyValueDec(HDL_Handle_t CnxHandle); /* decrease value contained in the current object */
SVPEG_ErrorCode_t SVPEG_KeyMenu(HDL_Handle_t CnxHandle); /* decrease value contained in the current object */


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
