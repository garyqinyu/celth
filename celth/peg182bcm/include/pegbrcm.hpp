#ifndef _PEGBRCM_H_
#define _PEGBRCM_H_


#define LOCK_TIMER_LIST
#define UNLOCK_TIMER_LIST
#define LOCK_PEG
#define UNLOCK_PEG




/* Defines the functionnal client key code for the RC */
typedef enum 
{
  BRCMPEG_kKeyUp,               /*to go to the nearest control up or change the value up when available*/
  BRCMPEG_kKeyDown,             /*to go to the nearest control down or change the value down when available*/
  BRCMPEG_kKeyRight,            /*to go to the nearest control right or change the value when available*/
  BRCMPEG_kKeyLeft,             /*to go to the nearest control left or change the value when available*/
  BRCMPEG_kKeySelect,           /*to press the return button*/
  BRCMPEG_kKeyCtrlTab,          /*to go to through the window*/
  BRCMPEG_kKeyShiftCtrlTab,     /*to go to the previous window*/
  BRCMPEG_kKeyTab,              /*to go to the next widget*/
  BRCMPEG_kKeyShiftTab,         /*to go to the previous widget*/
  BRCMPEG_kKeyAlt,              /*to go to the menu*/
  BRCMPEG_kKeyScrollUp,         /*to Scroll windows up*/
  BRCMPEG_kKeyScrollDown,       /*to Scroll windows down*/
  BRCMPEG_kMenu,
  BRCMPEG_kExit,								/*to close the service Peg*/
  BRCMPEG_kCloseWindow,	        /*to close last opened window */
  BRCMPEG_kAddWindow,						/*to add one child in presentation */
  BRCMPEG_kKeyCharValue,				/*to press 0 to 9 digital button */
  BRCMPEG_kKeyValueInc,	        /*to press VOL+ to increase value */
  BRCMPEG_kKeyValueDec,	        /*to press VOL- to decrease value */
  BRCMPEG_kTimerTick						/*to press VOL- to decrease value */
} PegKeyCode_t ;



/* define system parameters transmitted in message*/
typedef struct 
{
  PegKeyCode_t	SystemMessage;
	void*					pChildObject;
} BRCMPEG_SystemParam_t ;

/* define RC Key parameters transmitted in message*/
typedef struct{
  PegKeyCode_t  iKeyPressed;
  unsigned char		ucKeyValueParam;
}BRCMPEG_RcParam_t;

/* define message type*/
typedef enum {
  BRCMPEG_kRcKeyPressed=1,
  BRCMPEG_kSystem 
}BRCMPEG_MessageType_t;

/* define message structure*/
typedef struct{
  BRCMPEG_MessageType_t tMessageType;
  union 
        {BRCMPEG_RcParam_t tRcParam;
         BRCMPEG_SystemParam_t tSystem;
  }tMessageParam;
}BRCMPEG_ChassisMessage_t;



#define PegTransparentColor 0xFF

/* error code */
typedef enum
{
	BRCMPEG_kNoError       , /* No error detected */
	BRCMPEG_kExternalError , /* Error happened in called component */
	BRCMPEG_kBadParameter  , /* one or more parameter is invalid */
	BRCMPEG_kNotInitiated  , /* component is not initialized */
	BRCMPEG_kInvalidHandle , /* given handle is invalid */
	BRCMPEG_kInvalidCall     /* initialize already done => invalid call to the function */
} BRCMPEG_ErrorCode_t;

/* define Init parameters structure*/

/*
typedef struct{
	NEXUS_SurfaceHandle RegionOneHandle;
	NEXUS_SurfaceMemory RegionOneMem;
	
	NEXUS_SurfaceHandle RegionTwoHandle;
	NEXUS_SurfaceMemory RegionTwoMem;
	NEXUS_DisplayHandle DisplayHandle;
	int xSize;
	int ySize;
}BRCMPEG_InitParams_t;
*/

/*Celth_Compositor Celth_Osd;*/


typedef struct{
	void* Celth_OsdMemoryAddr;
	unsigned int Celth_OsdMemoryPitch;

	int xSize;
	int ySize;
}CELTHPEG_InitParams_t;


typedef void* HDL_Handle_t;





extern "C"{

int BrcmPeg_Init();



void* PegMain(void* pparam);



BRCMPEG_ErrorCode_t BrcmPeg_TimerTick(HDL_Handle_t CnxHandle, const int iTimerRef); /* function to be called for timer update. to be called each 200ms */

BRCMPEG_ErrorCode_t BrcmPeg_Open(HDL_Handle_t* pCnxHandle);/* open a connection to peg */

BRCMPEG_ErrorCode_t BrcmPeg_Close( HDL_Handle_t CnxHandle );/*close a connection to peg*/

BRCMPEG_ErrorCode_t BrcmPeg_Terminate(void); /* terminate peg module */


BRCMPEG_ErrorCode_t BrcmPeg_KeyUp(HDL_Handle_t CnxHandle); /* Key up pressed on user interface */
BRCMPEG_ErrorCode_t BrcmPeg_KeyDown(HDL_Handle_t CnxHandle);/* Key down pressed on user interface */
BRCMPEG_ErrorCode_t BrcmPeg_KeyLeft(HDL_Handle_t CnxHandle);/* Key left pressed on user interface */
BRCMPEG_ErrorCode_t BrcmPeg_KeyRight(HDL_Handle_t CnxHandle);/* Key right pressed on user interface */
BRCMPEG_ErrorCode_t BrcmPeg_KeySelect(HDL_Handle_t CnxHandle);/* OK Key pressed on user interface */
BRCMPEG_ErrorCode_t BrcmPeg_KeyExit(HDL_Handle_t CnxHandle);/* go to the menu object */



BRCMPEG_ErrorCode_t BrcmPeg_CloseWindow(HDL_Handle_t CnxHandle);/* close current window */
BRCMPEG_ErrorCode_t BrcmPeg_AddWindow(HDL_Handle_t CnxHandle, void *pChild); /* add a new window on the screen */
BRCMPEG_ErrorCode_t BrcmPeg_KeyCharValue(HDL_Handle_t CnxHandle, unsigned char ucKeyValue); /* transfer this key ascii code to the current object */
BRCMPEG_ErrorCode_t BrcmPeg_KeyValueInc(HDL_Handle_t CnxHandle); /* increase value contained in the current object */
BRCMPEG_ErrorCode_t BrcmPeg_KeyValueDec(HDL_Handle_t CnxHandle); /* decrease value contained in the current object */
BRCMPEG_ErrorCode_t BrcmPeg_KeyMenu(HDL_Handle_t CnxHandle); /* decrease value contained in the current object */






}

#endif


