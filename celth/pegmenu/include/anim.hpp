/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// anim.hpp - bitmap tables definition for animation in TVDecorated buttons
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifndef _ANIM_HPP
#define _ANIM_HPP

#include "peg.hpp"

extern UCHAR PegCustomPalette[256*3];
extern PegBitmap gbarrangeBitmap;
extern PegBitmap gbautoBitmap;
extern PegBitmap gbautooffBitmap;
extern PegBitmap gbavmodeBitmap;
extern PegBitmap gbbackgroundBitmap;
extern PegBitmap gbbalanceBitmap;
extern PegBitmap gbbassBitmap;
extern PegBitmap gbchineseBitmap;
extern PegBitmap gbenglishBitmap;
extern PegBitmap gbbrightnessBitmap;
extern PegBitmap gbcolorBitmap;
extern PegBitmap gbcolortempBitmap;
extern PegBitmap gbcontrastBitmap;
extern PegBitmap gbCRTBitmap;
extern PegBitmap gbdefinitionBitmap;
extern PegBitmap gbdesignerselectBitmap;
extern PegBitmap gbstarttuning1Bitmap;
extern PegBitmap gbstarttuning2Bitmap;
extern PegBitmap gbstarttuning3Bitmap;
extern PegBitmap gbendtuning1Bitmap;
extern PegBitmap gbendtuning2Bitmap;
extern PegBitmap gbepgBitmap;
extern PegBitmap gbfactoryBitmap;
extern PegBitmap gbwhitebalanceBitmap;
extern PegBitmap gbfinetuneBitmap;
extern PegBitmap gbgeomagnetismBitmap;
extern PegBitmap gbgeometryBitmap;
extern PegBitmap gbhelpBitmap;
extern PegBitmap gbhueBitmap;
extern PegBitmap gblanguageBitmap;
extern PegBitmap gbmanualBitmap;
extern PegBitmap gboutputformatBitmap;
extern PegBitmap gbparentcontrolBitmap;
extern PegBitmap gbpictureBitmap;
extern PegBitmap gbprogramBitmap;
extern PegBitmap gbprogrameditBitmap;
extern PegBitmap gbreducenoiseBitmap;
extern PegBitmap gbreminderBitmap;
extern PegBitmap gbsettimeBitmap;
extern PegBitmap gbsharpnessBitmap;
extern PegBitmap gbsoundBitmap;
extern PegBitmap gbsoundchannelBitmap;
extern PegBitmap gbsoundeffectBitmap;
extern PegBitmap gbstandardBitmap;
extern PegBitmap gbsuperfunctionBitmap;
extern PegBitmap gbsupersettingBitmap;
extern PegBitmap gbSVCBitmap;
extern PegBitmap gbtimeBitmap;
extern PegBitmap gbtimepreserveBitmap;
extern PegBitmap gbtransparanceBitmap;
extern PegBitmap gbtrebleBitmap;
extern PegBitmap gbvolumeBitmap;
extern PegBitmap gbhelpbigBitmap;

extern PegFont Arial14;
extern PegFont Arial12;
extern PegFont heiti12;
extern PegFont heiti14;

extern UCHAR gbCurrentLanguage;

#undef EXTERNAL_DEFINITION
#ifdef __MAINMENUMASTER /* this should be defined in MainMenu.cpp only */
	#define EXTERNAL_DEFINITION
#else
	#define EXTERNAL_DEFINITION extern
#endif

EXTERNAL_DEFINITION PegBitmap* ppBmpAutoFineStart[]
#ifdef __MAINMENUMASTER
={
	&gbstarttuning1Bitmap,
	&gbstarttuning2Bitmap,
	&gbstarttuning3Bitmap
}
#endif
;

EXTERNAL_DEFINITION PegBitmap* ppBmpAutoFineEnd[]
#ifdef __MAINMENUMASTER
={
	&gbendtuning1Bitmap,
	&gbendtuning2Bitmap
}
#endif
;

/* end of file */
#endif /*_ANIM_HPP*/