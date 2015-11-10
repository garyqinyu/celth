/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// userfct.cpp - user functions
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "wbstring.hpp"
#include "wbstrnga.hpp"
#include "anim.hpp"

#undef __USERFCTMASTER /* for mastership */
#define __USERFCTMASTER /* for mastership */
#include "userfct.hpp"

HorizontalScrollText_t tstMin_Med_High[5];
HorizontalScrollTable_t Ifp_Table;

HorizontalScrollText_t tstCVBS_SVHS_RGB[3];
HorizontalScrollTable_t CVBS_SVHS_RGB_Table;

HorizontalScrollText_t tstCVBS_SVHS[2];
HorizontalScrollTable_t CVBS_SVHS_Table;

HorizontalScrollText_t tstTUNER_AV2_FRONT[3];
HorizontalScrollTable_t TUNER_AV2_FRONT_Table;

HorizontalScrollText_t tstTUNER_AV1_FRONT[3];
HorizontalScrollTable_t TUNER_AV1_FRONT_Table;

HorizontalScrollText_t tstON_OFF[2];
HorizontalScrollTable_t ON_OFF_Table;

HorizontalScrollText_t tstCRT43_CRT169[2];
HorizontalScrollTable_t CRT43_CRT169_Table;

HorizontalScrollText_t tstAUTO_AABB[2];
HorizontalScrollTable_t AUTO_AABB_Table;

HorizontalScrollText_t tstOFF_BS_WS_GS_BWS[5];
HorizontalScrollTable_t OFF_BS_WS_GS_BWS_Table;

HorizontalScrollText_t tstPRG[9];
HorizontalScrollTable_t PRG_Table;

HorizontalScrollText_t tstCountry[2];
HorizontalScrollTable_t Country_Table;

HorizontalScrollText_t tstVideo_Std[3];
HorizontalScrollTable_t Video_Std_Table;

HorizontalScrollText_t tstRF_Std[4];
HorizontalScrollTable_t RF_Std_Table;

HorizontalScrollText_t tstAUTO_OFF_MIN_MED_MAX[5];
HorizontalScrollTable_t AUTO_OFF_MIN_MED_MAX_Table;

HorizontalScrollText_t tstRC_TXT_TWB[3];
HorizontalScrollTable_t RC_TXT_TWB_Table;

HorizontalScrollText_t tstAudioType[2];
HorizontalScrollTable_t AudioType_Table;

HorizontalScrollText_t tstAudioLanguage[4];
HorizontalScrollTable_t AudioLanguage_Table;

HorizontalScrollText_t tstSRSStereoSpEffect[4];
HorizontalScrollTable_t SRSStereoSpEffect_Table;

HorizontalScrollText_t tstSRSMonoSpEffect[3];
HorizontalScrollTable_t SRSMonoSpEffect_Table;

HorizontalScrollText_t tstSTStereoSpEffect[3];
HorizontalScrollTable_t STStereoSpEffect_Table;

HorizontalScrollText_t tstSTMonoSpEffect[2];
HorizontalScrollTable_t STMonoSpEffect_Table;

HorizontalScrollText_t tstEqSoundPreset[5];
HorizontalScrollTable_t EqSoundPreset_Table;


/* ----------------------------------------------------------------------- */
/* definition of tables used for horizontal text scrolling in menu banners */
/* ----------------------------------------------------------------------- */
void InitializeMenuBanners(void)
{
		UCHAR SavedLanguageValue;
		char iLoop;

		SavedLanguageValue = gbCurrentLanguage;

		tstCVBS_SVHS_RGB[0].iStringID = Ssb_kCvbs;
		tstCVBS_SVHS_RGB[1].iStringID = Ssb_kSVideo;
		tstCVBS_SVHS_RGB[2].iStringID = Ssb_kRGB;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstCVBS_SVHS_RGB[0].StringToDisplay[iLoop] = LS(SID_CVBS);
			tstCVBS_SVHS_RGB[1].StringToDisplay[iLoop] = LS(SID_SVHS);
			tstCVBS_SVHS_RGB[2].StringToDisplay[iLoop] = LS(SID_RGB);
		}
		CVBS_SVHS_RGB_Table.ptstTable = tstCVBS_SVHS_RGB;
		CVBS_SVHS_RGB_Table.iTableSize = sizeof(tstCVBS_SVHS_RGB)/sizeof(HorizontalScrollText_t);


		tstCVBS_SVHS[0].iStringID = Ssb_kFrontCvbs;
		tstCVBS_SVHS[1].iStringID = Ssb_kSVideoonAV3;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstCVBS_SVHS[0].StringToDisplay[iLoop] = LS(SID_CVBS);
			tstCVBS_SVHS[1].StringToDisplay[iLoop] = LS(SID_SVHS);
		}
		CVBS_SVHS_Table.ptstTable = tstCVBS_SVHS;
		CVBS_SVHS_Table.iTableSize = sizeof(tstCVBS_SVHS)/sizeof(HorizontalScrollText_t);


		tstTUNER_AV2_FRONT[0].iStringID = Ssb_kTuneronAV1;
		tstTUNER_AV2_FRONT[1].iStringID = Ssb_kAV2onAV1;
		tstTUNER_AV2_FRONT[2].iStringID = Ssb_kFrontonAV1;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstTUNER_AV2_FRONT[0].StringToDisplay[iLoop] = LS(SID_Tuner);
			tstTUNER_AV2_FRONT[1].StringToDisplay[iLoop] = LS(SID_AV2);
			tstTUNER_AV2_FRONT[2].StringToDisplay[iLoop] = LS(SID_Front);
		}
		TUNER_AV2_FRONT_Table.ptstTable = tstTUNER_AV2_FRONT;
		TUNER_AV2_FRONT_Table.iTableSize = sizeof(tstTUNER_AV2_FRONT)/sizeof(HorizontalScrollText_t);


		tstTUNER_AV1_FRONT[0].iStringID = Ssb_kTuneronAV2;
		tstTUNER_AV1_FRONT[1].iStringID = Ssb_kAV1onAV2;
		tstTUNER_AV1_FRONT[2].iStringID = Ssb_kFrontonAV2;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstTUNER_AV1_FRONT[0].StringToDisplay[iLoop] = LS(SID_Tuner);
			tstTUNER_AV1_FRONT[1].StringToDisplay[iLoop] = LS(SID_AV1);
			tstTUNER_AV1_FRONT[2].StringToDisplay[iLoop] = LS(SID_Front);
		}
		TUNER_AV1_FRONT_Table.ptstTable = tstTUNER_AV1_FRONT;
		TUNER_AV1_FRONT_Table.iTableSize = sizeof(tstTUNER_AV1_FRONT)/sizeof(HorizontalScrollText_t);


		tstON_OFF[0].iStringID = Ssb_kOn;
		tstON_OFF[1].iStringID = Ssb_kOff;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstON_OFF[0].StringToDisplay[iLoop] = LS(SID_ON);
			tstON_OFF[1].StringToDisplay[iLoop] = LS(SID_OFF);
		}
		ON_OFF_Table.ptstTable = tstON_OFF;
		ON_OFF_Table.iTableSize = sizeof(tstON_OFF)/sizeof(HorizontalScrollText_t);


		tstCRT43_CRT169[0].iStringID = Ssb_kCrt43;
		tstCRT43_CRT169[1].iStringID = Ssb_kCrt169;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstCRT43_CRT169[0].StringToDisplay[iLoop] = LS(SID_CRT43);
			tstCRT43_CRT169[1].StringToDisplay[iLoop] = LS(SID_CRT169);
		}
		CRT43_CRT169_Table.ptstTable = tstCRT43_CRT169;
		CRT43_CRT169_Table.iTableSize = sizeof(tstCRT43_CRT169)/sizeof(HorizontalScrollText_t);


		tstAUTO_AABB[0].iStringID = Ssb_kAuto;
		tstAUTO_AABB[1].iStringID = Ssb_kAABB;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstAUTO_AABB[0].StringToDisplay[iLoop] = LS(SID_AUTO);
			tstAUTO_AABB[1].StringToDisplay[iLoop] = LS(SID_AABB);
		}
		AUTO_AABB_Table.ptstTable = tstAUTO_AABB;
		AUTO_AABB_Table.iTableSize = sizeof(tstAUTO_AABB)/sizeof(HorizontalScrollText_t);


		tstOFF_BS_WS_GS_BWS[0].iStringID = Ssb_kContrastOff;
		tstOFF_BS_WS_GS_BWS[1].iStringID = Ssb_kBlackStretch;
		tstOFF_BS_WS_GS_BWS[2].iStringID = Ssb_kWhiteStretch;
		tstOFF_BS_WS_GS_BWS[3].iStringID = Ssb_kGreyStretch;
		tstOFF_BS_WS_GS_BWS[4].iStringID = Ssb_kBWShrink;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstOFF_BS_WS_GS_BWS[0].StringToDisplay[iLoop] = LS(SID_OFF);
			tstOFF_BS_WS_GS_BWS[1].StringToDisplay[iLoop] = LS(SID_BlackStretch);
			tstOFF_BS_WS_GS_BWS[2].StringToDisplay[iLoop] = LS(SID_WhiteStretch);
			tstOFF_BS_WS_GS_BWS[3].StringToDisplay[iLoop] = LS(SID_GreyStretch);
			tstOFF_BS_WS_GS_BWS[4].StringToDisplay[iLoop] = LS(SID_BWShrink);
		}
		OFF_BS_WS_GS_BWS_Table.ptstTable = tstOFF_BS_WS_GS_BWS;
		OFF_BS_WS_GS_BWS_Table.iTableSize = sizeof(tstOFF_BS_WS_GS_BWS)/sizeof(HorizontalScrollText_t);


		tstPRG[0].iStringID = PR1;
		tstPRG[1].iStringID = PR2;
		tstPRG[2].iStringID = PR3;
		tstPRG[3].iStringID = PR4;
		tstPRG[4].iStringID = PR5;
		tstPRG[5].iStringID = PR6;
		tstPRG[6].iStringID = PR7;
		tstPRG[7].iStringID = PR8;
		tstPRG[8].iStringID = PR9;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstPRG[0].StringToDisplay[iLoop] = LS(SID_PR1);
			tstPRG[1].StringToDisplay[iLoop] = LS(SID_PR2);
			tstPRG[2].StringToDisplay[iLoop] = LS(SID_PR3);
			tstPRG[3].StringToDisplay[iLoop] = LS(SID_PR4);
			tstPRG[4].StringToDisplay[iLoop] = LS(SID_PR5);
			tstPRG[5].StringToDisplay[iLoop] = LS(SID_PR6);
			tstPRG[6].StringToDisplay[iLoop] = LS(SID_PR7);
			tstPRG[7].StringToDisplay[iLoop] = LS(SID_PR8);
			tstPRG[8].StringToDisplay[iLoop] = LS(SID_PR9);
		}
		PRG_Table.ptstTable = tstPRG;
		PRG_Table.iTableSize = sizeof(tstPRG)/sizeof(HorizontalScrollText_t);


		tstCountry[0].iStringID = Ssb_kFRANCE;
		tstCountry[1].iStringID = Ssb_kOthers;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstCountry[0].StringToDisplay[iLoop] = LS(SID_FRANCE);
			tstCountry[1].StringToDisplay[iLoop] = LS(SID_OTHERS);
		}
		Country_Table.ptstTable = tstCountry;
		Country_Table.iTableSize = sizeof(tstCountry)/sizeof(HorizontalScrollText_t);


		tstVideo_Std[0].iStringID = Ssb_kSECAM;
		tstVideo_Std[1].iStringID = Ssb_kPAL;
		tstVideo_Std[2].iStringID = Ssb_kAUTO;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstVideo_Std[0].StringToDisplay[iLoop] = LS(SID_SECAM);
			tstVideo_Std[1].StringToDisplay[iLoop] = LS(SID_PAL);
			tstVideo_Std[2].StringToDisplay[iLoop] = LS(SID_AUTO);
		}
		Video_Std_Table.ptstTable = tstVideo_Std;
		Video_Std_Table.iTableSize = sizeof(tstVideo_Std)/sizeof(HorizontalScrollText_t);


		tstRF_Std[0].iStringID = Ssb_kRFL;
		tstRF_Std[1].iStringID = Ssb_kRFBG;
		tstRF_Std[2].iStringID = Ssb_kRFDK;
		tstRF_Std[3].iStringID = Ssb_kRFI;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstRF_Std[0].StringToDisplay[iLoop] = LS(SID_LLPRIM);
			tstRF_Std[1].StringToDisplay[iLoop] = LS(SID_BG);
			tstRF_Std[2].StringToDisplay[iLoop] = LS(SID_DK);
			tstRF_Std[3].StringToDisplay[iLoop] = LS(SID_I);
		}
		RF_Std_Table.ptstTable = tstRF_Std;
		RF_Std_Table.iTableSize = sizeof(tstRF_Std)/sizeof(HorizontalScrollText_t);


		tstAUTO_OFF_MIN_MED_MAX[0].iStringID = Ssb_Tnr_Auto;
		tstAUTO_OFF_MIN_MED_MAX[1].iStringID = Ssb_Tnr_Off;
		tstAUTO_OFF_MIN_MED_MAX[2].iStringID = Ssb_Tnr_Min;
		tstAUTO_OFF_MIN_MED_MAX[3].iStringID = Ssb_Tnr_Med;
		tstAUTO_OFF_MIN_MED_MAX[4].iStringID = Ssb_Tnr_High;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstAUTO_OFF_MIN_MED_MAX[0].StringToDisplay[iLoop] = LS(SID_AUTO);
			tstAUTO_OFF_MIN_MED_MAX[1].StringToDisplay[iLoop] = LS(SID_OFF);
			tstAUTO_OFF_MIN_MED_MAX[2].StringToDisplay[iLoop] = LS(SID_MIN);
			tstAUTO_OFF_MIN_MED_MAX[3].StringToDisplay[iLoop] = LS(SID_MED);
			tstAUTO_OFF_MIN_MED_MAX[4].StringToDisplay[iLoop] = LS(SID_MAX);
		}
		AUTO_OFF_MIN_MED_MAX_Table.ptstTable = tstAUTO_OFF_MIN_MED_MAX;
		AUTO_OFF_MIN_MED_MAX_Table.iTableSize = sizeof(tstAUTO_OFF_MIN_MED_MAX)/sizeof(HorizontalScrollText_t);


		tstRC_TXT_TWB[0].iStringID = 0;
		tstRC_TXT_TWB[1].iStringID = 1;
		tstRC_TXT_TWB[2].iStringID = 2;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstRC_TXT_TWB[0].StringToDisplay[iLoop] = LS(SID_General);
			tstRC_TXT_TWB[1].StringToDisplay[iLoop] = LS(SID_TeletextOnOff);
			tstRC_TXT_TWB[2].StringToDisplay[iLoop] = LS(SID_Teleweb);
		}
		RC_TXT_TWB_Table.ptstTable = tstRC_TXT_TWB;
		RC_TXT_TWB_Table.iTableSize = sizeof(tstRC_TXT_TWB)/sizeof(HorizontalScrollText_t);

		
		tstAudioType[0].iStringID = Ssb_kSoundTypeAuto;
		tstAudioType[1].iStringID = Ssb_kSoundTypeMono;
		
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstAudioType[0].StringToDisplay[iLoop] = LSA(SID_StatusAuto);
			tstAudioType[1].StringToDisplay[iLoop] = LSA(SID_StatusMono);
		}
		AudioType_Table.ptstTable = tstAudioType;
		AudioType_Table.iTableSize = sizeof(tstAudioType)/sizeof(HorizontalScrollText_t);


		tstAudioLanguage[0].iStringID = Ssb_kL1;
		tstAudioLanguage[1].iStringID = Ssb_kL2;
		tstAudioLanguage[2].iStringID = Ssb_kL1plusL2;
		tstAudioLanguage[3].iStringID = Ssb_kL1andL2;
		
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstAudioLanguage[0].StringToDisplay[iLoop] = LSA(SID_LeftOnBoth);
			tstAudioLanguage[1].StringToDisplay[iLoop] = LSA(SID_RightOnBoth);
			tstAudioLanguage[2].StringToDisplay[iLoop] = LSA(SID_MixOnBoth);
			tstAudioLanguage[3].StringToDisplay[iLoop] = LSA(SID_StereoLang);
		}
		AudioLanguage_Table.ptstTable = tstAudioLanguage;
		AudioLanguage_Table.iTableSize = sizeof(tstAudioLanguage)/sizeof(HorizontalScrollText_t);


		tstSRSStereoSpEffect[0].iStringID = Ssb_kSTWideSurroundMusic;
		tstSRSStereoSpEffect[1].iStringID = Ssb_kSTWideSurroundMovie;
		tstSRSStereoSpEffect[2].iStringID = Ssb_kSRS3D;
		tstSRSStereoSpEffect[3].iStringID = Ssb_kSpatialEffectOff;
		
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstSRSStereoSpEffect[0].StringToDisplay[iLoop] = LSA(SID_STWideMusic);
			tstSRSStereoSpEffect[1].StringToDisplay[iLoop] = LSA(SID_STWideMovie);
			tstSRSStereoSpEffect[2].StringToDisplay[iLoop] = LSA(SID_SRS3D);
			tstSRSStereoSpEffect[3].StringToDisplay[iLoop] = LS(SID_OFF);
		}
		SRSStereoSpEffect_Table.ptstTable = tstSRSStereoSpEffect;
		SRSStereoSpEffect_Table.iTableSize = sizeof(tstSRSStereoSpEffect)/sizeof(HorizontalScrollText_t);


		tstSRSMonoSpEffect[0].iStringID = Ssb_kSTWideSuroundSimulated;
		tstSRSMonoSpEffect[1].iStringID = Ssb_kSRS3DMono;
		tstSRSMonoSpEffect[2].iStringID = Ssb_kSpatialEffectMonoOff;
		
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstSRSMonoSpEffect[0].StringToDisplay[iLoop] = LSA(SID_STWideSimulated);
			tstSRSMonoSpEffect[1].StringToDisplay[iLoop] = LSA(SID_SRS3DMono);
			tstSRSMonoSpEffect[2].StringToDisplay[iLoop] = LS(SID_OFF);
		}
		SRSMonoSpEffect_Table.ptstTable = tstSRSMonoSpEffect;
		SRSMonoSpEffect_Table.iTableSize = sizeof(tstSRSMonoSpEffect)/sizeof(HorizontalScrollText_t);


		tstSTStereoSpEffect[0].iStringID = Ssb_kSTWideSurroundMusic;
		tstSTStereoSpEffect[1].iStringID = Ssb_kSTWideSurroundMovie;
		tstSTStereoSpEffect[2].iStringID = Ssb_kSpatialEffectOff;
		
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstSTStereoSpEffect[0].StringToDisplay[iLoop] = LSA(SID_STWideMusic);
			tstSTStereoSpEffect[1].StringToDisplay[iLoop] = LSA(SID_STWideMovie);
			tstSTStereoSpEffect[2].StringToDisplay[iLoop] = LS(SID_OFF);
		}
		STStereoSpEffect_Table.ptstTable = tstSTStereoSpEffect;
		STStereoSpEffect_Table.iTableSize = sizeof(tstSTStereoSpEffect)/sizeof(HorizontalScrollText_t);


		tstSTMonoSpEffect[0].iStringID = Ssb_kSTWideSuroundSimulated;
		tstSTMonoSpEffect[1].iStringID = Ssb_kSpatialEffectMonoOff;
		
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstSTMonoSpEffect[0].StringToDisplay[iLoop] = LSA(SID_STWideSimulated);
			tstSTMonoSpEffect[1].StringToDisplay[iLoop] = LS(SID_OFF);
		}
		STMonoSpEffect_Table.ptstTable = tstSTMonoSpEffect;
		STMonoSpEffect_Table.iTableSize = sizeof(tstSTMonoSpEffect)/sizeof(HorizontalScrollText_t);


		tstEqSoundPreset[0].iStringID = Ssb_kSoundMovie;
		tstEqSoundPreset[1].iStringID = Ssb_kSoundMusic;
		tstEqSoundPreset[2].iStringID = Ssb_kSoundHall;
		tstEqSoundPreset[3].iStringID = Ssb_kSoundFlat;
		tstEqSoundPreset[4].iStringID = Ssb_kSoundPersonnel;
		
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstEqSoundPreset[0].StringToDisplay[iLoop] = LSA(SID_PresetMovie);
			tstEqSoundPreset[1].StringToDisplay[iLoop] = LSA(SID_PresetMusic);
			tstEqSoundPreset[2].StringToDisplay[iLoop] = LSA(SID_PresetHall);
			tstEqSoundPreset[3].StringToDisplay[iLoop] = LSA(SID_PresetFlat);
			tstEqSoundPreset[4].StringToDisplay[iLoop] = LSA(SID_PresetPersonal);
		}
		EqSoundPreset_Table.ptstTable = tstEqSoundPreset;
		EqSoundPreset_Table.iTableSize = sizeof(tstEqSoundPreset)/sizeof(HorizontalScrollText_t);

		tstMin_Med_High[0].iStringID = DVSDIN_kVeryLow;
		tstMin_Med_High[1].iStringID = DVSDIN_kLow;
		tstMin_Med_High[2].iStringID = DVSDIN_kMedium;
		tstMin_Med_High[3].iStringID = DVSDIN_kHigh;
		tstMin_Med_High[4].iStringID = DVSDIN_kVeryHigh;
		for (iLoop = 0 ; iLoop<LANGUAGE_NUMBER ; iLoop++)
		{
			gbCurrentLanguage = iLoop; /* chose language */
			tstMin_Med_High[0].StringToDisplay[iLoop] = LS(SID_Video);
			tstMin_Med_High[1].StringToDisplay[iLoop] = LS(SID_MIN);
			tstMin_Med_High[2].StringToDisplay[iLoop] = LS(SID_MED);
			tstMin_Med_High[3].StringToDisplay[iLoop] = LS(SID_MAX);
			tstMin_Med_High[4].StringToDisplay[iLoop] = LS(SID_Movie);
		}
		Ifp_Table.ptstTable = tstMin_Med_High;
		Ifp_Table.iTableSize = sizeof(tstMin_Med_High)/sizeof(HorizontalScrollText_t);

		gbCurrentLanguage = SavedLanguageValue; /* reset language to initial value */
}

/* ------------------------------------------------------------------------------ */
/* fonction used to get next item to be displayed in the horizontal scroll banner */
/* ------------------------------------------------------------------------------ */
int GetNextHorizontalScrollText(int iCurrentValue, HorizontalScrollTable_t* ptstScrollTable)
{
	int iLoop;

	for (iLoop = 0 ; iLoop < ptstScrollTable->iTableSize ; iLoop++)
	{
		if (ptstScrollTable->ptstTable[iLoop].iStringID == iCurrentValue)
		{
			if (iLoop == ptstScrollTable->iTableSize - 1)
				return (ptstScrollTable->ptstTable[0].iStringID);
			else
				return (ptstScrollTable->ptstTable[iLoop+1].iStringID);
		}
	}
	return(0);
}

/* ---------------------------------------------------------------------------------- */
/* fonction used to get previous item to be displayed in the horizontal scroll banner */
/* ---------------------------------------------------------------------------------- */
int GetPreviousHorizontalScrollText(int iCurrentValue, HorizontalScrollTable_t* ptstScrollTable)
{
	int iLoop;

	for (iLoop = 0 ; iLoop < ptstScrollTable->iTableSize ; iLoop++)
	{
		if (ptstScrollTable->ptstTable[iLoop].iStringID == iCurrentValue)
		{
			if (iLoop == 0)
				return (ptstScrollTable->ptstTable[ptstScrollTable->iTableSize-1].iStringID);
			else
				return (ptstScrollTable->ptstTable[iLoop-1].iStringID);
		}
	}
	return(0);
}

/* -------------------------------------------------------------------------------------- */
/* fonction used to get pointer to string to be displayed in the horizontal scroll banner */
/* -------------------------------------------------------------------------------------- */
const PEGCHAR* GetString(int iCurrentValue, HorizontalScrollTable_t* ptstScrollTable)
{
	int iLoop;
	PEGCHAR* pString = NULL;

	for (iLoop = 0 ; iLoop < ptstScrollTable->iTableSize ; iLoop++)
	{
		if (ptstScrollTable->ptstTable[iLoop].iStringID == iCurrentValue)
		{
			pString = (PEGCHAR*) ptstScrollTable->ptstTable[iLoop].StringToDisplay[gbCurrentLanguage];
		}
	}
	
	return ((const PEGCHAR*) pString);
}

/* ----------------------------------------------------------- */
/* function used to update a PegPrompt with a given int value.
   inputs are pointer to the prompt and value
   return nothing */
/* ----------------------------------------------------------- */
void ValueUpdate(PegPrompt* pObject, int iValue)
{
	PEGCHAR tStringToDisplay[5];
	long lValue = 0;

	lValue = iValue;

	_ltoa(lValue,tStringToDisplay,10);

	pObject->DataSet(tStringToDisplay);
	pObject->Invalidate();
	pObject->Parent()->Draw();
}

/* ------------------------------------------------------------------------------ */
/* function used to convert a "string" value contained in a PegPrompt into an int
	 input is the pointer to the prompt
	 returns the corresponding value  */
/* ------------------------------------------------------------------------------ */
int GetValue(PegPrompt* pObject)
{
	unsigned short tString[5];
	unsigned short* pString;
	PEGCHAR* pPegString;
	int iTmp;


	pPegString = pObject->DataGet(); /* get object */
	pString = tString;	/* now, translate PEGCHAR to "clasic" string */
	for (iTmp = 0 ; iTmp<4 ; iTmp++)
	{
		*pString++ = *pPegString++;
	}

	/* the value contained in tString is located on the left of the string */
	if (tString[0]	== '-')
	{
		if (tString[1] == 0)
			iTmp = 0;
		else if (tString[2] ==0)
			iTmp = -(tString[1]-48);
		else if (tString[3] == 0)
			iTmp = - (((tString[1]-48)*10) + ((tString[2]-48)));
		else if (tString[4] == 0)
			iTmp = - (((tString[1]-48)*100) + ((tString[2]-48)*10) + ((tString[3]-48)));
	}
	else
	{
		if (tString[0] == 0)
			iTmp = 0;
		else if (tString[1] ==0)
			iTmp = (tString[0]-48);
		else if (tString[2] == 0)
			iTmp =  (((tString[0]-48)*10) + ((tString[1]-48)));
		else if (tString[3] == 0)
			iTmp = (((tString[0]-48)*100) + ((tString[1]-48)*10) + ((tString[2]-48)));
		else if (tString[4] == 0)
			iTmp = (((tString[0]-48)*1000) + ((tString[1]-48)*100) + ((tString[2]-48)*10) + ((tString[3]-48)));
	}

	return (iTmp);
}


