/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// userfct.hpp - user functions
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef __USERFCT
#define __USERFCT


#include "peg.hpp"
/*  #include "ap_cmd.h"  */

/* the following is used to define horizontal scrolling string in menus */
enum
{ PR1=1,
	PR2,
	PR3,
	PR4,
	PR5,
	PR6,
	PR7,
	PR8,
	PR9
};

#define LANGUAGE_NUMBER 4 /* 4 languages handled */

typedef struct {
	int iStringID;
	const PEGCHAR* StringToDisplay[LANGUAGE_NUMBER]; /* string to display for a given language */
} HorizontalScrollText_t;

typedef struct {
	HorizontalScrollText_t* ptstTable;
	int iTableSize;
} HorizontalScrollTable_t;

#ifndef __USERFCTMASTER /* this should be defined in Userfct.cpp only */
extern HorizontalScrollTable_t CVBS_SVHS_RGB_Table;
extern HorizontalScrollTable_t CVBS_SVHS_Table;
extern HorizontalScrollTable_t TUNER_AV2_FRONT_Table;
extern HorizontalScrollTable_t TUNER_AV1_FRONT_Table;
extern HorizontalScrollTable_t ON_OFF_Table;
extern HorizontalScrollTable_t CRT43_CRT169_Table;
extern HorizontalScrollTable_t AUTO_AABB_Table;
extern HorizontalScrollTable_t OFF_BS_WS_GS_BWS_Table;
extern HorizontalScrollTable_t PRG_Table;
extern HorizontalScrollTable_t Country_Table;
extern HorizontalScrollTable_t Video_Std_Table;
extern HorizontalScrollTable_t RF_Std_Table;
extern HorizontalScrollTable_t AUTO_OFF_MIN_MED_MAX_Table;
extern HorizontalScrollTable_t RC_TXT_TWB_Table;
extern HorizontalScrollTable_t AudioType_Table;
extern HorizontalScrollTable_t AudioLanguage_Table;
extern HorizontalScrollTable_t SRSStereoSpEffect_Table;
extern HorizontalScrollTable_t SRSMonoSpEffect_Table;
extern HorizontalScrollTable_t STStereoSpEffect_Table;
extern HorizontalScrollTable_t STMonoSpEffect_Table;
extern HorizontalScrollTable_t EqSoundPreset_Table;
extern HorizontalScrollTable_t Ifp_Table;
#endif

#endif





