/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 3500peg.h - Definition file for 3500peg.cpp
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if !defined(__3500peg_h)              // Sentry, use file only if it's not already included.
#define __3500peg_h

#include "winpeg.rh"            // Definition of all resources.
#include "3500peg.hpp"            // Definition of all resources.
#include "sv_peg.h"
#include "stosd.h"


/* Add user_defined messages for PEG	*/
enum PegUserMessage {
    KEY_VALUE_INCREASE = FIRST_USER_MESSAGE,
    KEY_VALUE_DECREASE
};

/* Defines the functionnal client key code for the RC */
typedef enum 
{
  SVPEG_kKeyUp,               /*to go to the nearest control up or change the value up when available*/
  SVPEG_kKeyDown,             /*to go to the nearest control down or change the value down when available*/
  SVPEG_kKeyRight,            /*to go to the nearest control right or change the value when available*/
  SVPEG_kKeyLeft,             /*to go to the nearest control left or change the value when available*/
  SVPEG_kKeySelect,           /*to press the return button*/
  SVPEG_kKeyCtrlTab,          /*to go to through the window*/
  SVPEG_kKeyShiftCtrlTab,     /*to go to the previous window*/
  SVPEG_kKeyTab,              /*to go to the next widget*/
  SVPEG_kKeyShiftTab,         /*to go to the previous widget*/
  SVPEG_kKeyAlt,              /*to go to the menu*/
  SVPEG_kKeyScrollUp,         /*to Scroll windows up*/
  SVPEG_kKeyScrollDown,       /*to Scroll windows down*/
  SVPEG_kMenu,
  SVPEG_kExit,								/*to close the service Peg*/
  SVPEG_kCloseWindow,	        /*to close last opened window */
  SVPEG_kAddWindow,						/*to add one child in presentation */
  SVPEG_kKeyCharValue,				/*to press 0 to 9 digital button */
  SVPEG_kKeyValueInc,	        /*to press VOL+ to increase value */
  SVPEG_kKeyValueDec,	        /*to press VOL- to decrease value */
  SVPEG_kTimerTick						/*to press VOL- to decrease value */
} PegKeyCode_t ;

// define system parameters transmitted in message
typedef struct 
{
  PegKeyCode_t	SystemMessage;
	void*					pChildObject;
} SVPEG_SystemParam_t ;

// define RC Key parameters transmitted in message
typedef struct{
  PegKeyCode_t  iKeyPressed;
  U8			ucKeyValueParam;
}SVPEG_RcParam_t;

// define message type
typedef enum {
  SVPEG_kRcKeyPressed=1,
  SVPEG_kSystem 
}SVPEG_MessageType_t;

// define message structure
typedef struct{
  SVPEG_MessageType_t tMessageType;
  union 
        {SVPEG_RcParam_t tRcParam;
         SVPEG_SystemParam_t tSystem;
  }tMessageParam;
}SVPEG_ChassisMessage_t;


typedef struct
{
	SIGNED iScan;
	SIGNED iKey;
} KEYXLATE;

// scan codes for extended keys:

#define SCAN_DEL 	0x0053
#define SCAN_INS 	0x0052
#define SCAN_HOME	0x0047
#define SCAN_END	0x004f
#define SCAN_PGUP	0x0049
#define SCAN_PGDN	0x0051
#define SCAN_LNUP	0x0048
#define SCAN_LNDN	0x0050
#define SCAN_LEFT	0x004b
#define SCAN_RIGHT	0x004d

int PegMain(SVPEG_InitParams_t * pstInitParams);

/* C++ compatibility ----------------------------------------------------------*/

#endif                                      // __winpeg_h sentry.


