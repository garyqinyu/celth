/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// PegIdle.hpp - Definition file for PegIdle.cpp
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if !defined(__PegIdle_h)              // Sentry, use file only if it's not already included.
#define __PegIdle_h

#include "sv_peg.h"

/* ----------------------------------------------------------------------------*/
/* C++ compatibility ----------------------------------------------------------*/
/* ----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Defines the functionnal client key code for the RC */
typedef enum 
{
	ap_mmi_kExit,								/*to close the service Peg*/
	ap_mmi_kCloseWindow,	        /*to close last opened window */
} PegKeyCode_t ;

// define system parameters transmitted in message
typedef struct 
{
  PegKeyCode_t	SystemMessage;
	void*					pChildObject;
} ap_mmi_SystemParam_t ;

// define RC Key parameters transmitted in message
typedef struct{
  int  iKeyPressed;
  U8			ucKeyValueParam;
}ap_mmi_RcParam_t;

// define message type
typedef enum {
  ap_mmi_kRcKeyPressed=1,
  ap_mmi_kSystem 
}ap_mmi_MessageType_t;

// define message structure
typedef struct{
  ap_mmi_MessageType_t tMessageType;
  union 
        {ap_mmi_RcParam_t tRcParam;
         ap_mmi_SystemParam_t tSystem;
  }tMessageParam;
}ap_mmi_ChassisMessage_t;

typedef enum {
			ap_mmi_kKeyUp,
			ap_mmi_kKeyDown,
			ap_mmi_kKeyRight,
			ap_mmi_kKeyLeft,
			ap_mmi_kKeySelect,
			ap_mmi_kKeyCharValueD,
			ap_mmi_kKeyValueInc,
			ap_mmi_kKeyValueDec,
			ap_mmi_kMainMenu,
			ap_mmi_kZoomMode,
			ap_mmi_kGeneralInfo,
			ap_mmi_kNoSignal,
			ap_mmi_kSignal,
			ap_mmi_kSoundStandard,
			ap_mmi_kAvName,
			ap_mmi_kNoTeletext,
			ap_mmi_kKeyCharValue,
			ap_mmi_kKeyVolUp,
			ap_mmi_kKeyVolDown,
			ap_mmi_kTimerTick,
			ap_mmi_kKeyMuteUnMute,
			ap_mmi_kKeyVolUpTxt,
			ap_mmi_kKeyVolDownTxt,
			ap_mmi_kKeyMuteUnMuteTxt,
			ap_mmi_kRefreshWindow,
			ap_mmi_kRefreshSoundVolume,
			ap_mmi_kGoToStandby,
			ap_mmi_kValueRestored,
			ap_mmi_kValueStored,
			ap_mmi_kTuningFrequencyFound,
			ap_mmi_kProgramStored,
			ap_mmi_kNoTeleweb,
			ap_mmi_kCloseAllForStandbyFinished,
			ap_mmi_kSetColorOsdTranslucency,
			ap_mmi_kKeyAudioLanguage,
			ap_mmi_kKeyEqSoundPreset,
			ap_mmi_kKeyRed,
			ap_mmi_kKeyYellow,
			ap_mmi_kDoubleDigitChannelChange,
			ap_mmi_kDoubleDigitChannelClose,
			ap_mmi_kCloseAllWindows,
			ap_mmi_kCloseAllWindowsFinished
}ap_mmi_KeyStroke_t;

#define kMainMenuWidth 400
#define kMainMenuHeight 600

#define kPictureMenuWidth 276
#define kPictureMenuHeight 261


/* C++ compatibility ----------------------------------------------------------*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif                                      // __PegIdle_h sentry.



