/********************************************************************************
COPYRIGHT (C) STMicroelectronics 2000.

File name       : init_val.h 
Author          : Pascal VOYER
Creation        : 24/07/2002
Modification    :
Date					DDTS Number       	Modification       	     Name 
===========				============        ===========              =====
	  
********************************************************************************/


#ifndef __INITVAL
#define __INITVAL

/* ----------------------------------------------------------------------------*/
/* C++ compatibility ----------------------------------------------------------*/
/* ----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Local Includes -------------------------------------------------------------*/

/* Private constant -----------------------------------------------------------*/

/* Private Variables ----------------------------------------------------------*/

// Picture Menu
//**************
#define ucBrightnessMin	0
#define ucBrightnessMax	63
#define ucContrastMin	0
#define ucContrastMax	63
#define ucColorMin	0
#define ucColorMax	63
#define ucSharpnessMin	0
#define ucSharpnessMax	7

// Picture+ Menu
//**************
#define ucTintMin	0
#define ucTintMax	63
#define ucRotationMin	0
#define ucRotationMax	63

// Geometry Menu
//**************
#define ucVSlopeMin	0
#define ucVSlopeMax	63
#define ucVAmplitudeMin	0
#define ucVAmplitudeMax	63
#define ucVPositionMin	0
#define ucVPositionMax	63
#define ucVSCorrectionMin	0
#define ucVSCorrectionMax	63
#define ucHBlankingMin	0
#define ucHBlankingMax	15
#define ucHShiftMin	0
#define ucHShiftMax	63
#define ucHParallelogramMin	0
#define ucHParallelogramMax	15
#define ucHBowMin	0
#define ucHBowMax	15
#define ucEWWidthMin	0
#define ucEWWidthMax	63
#define ucEWPinCushionMin	0
#define ucEWPinCushionMax	63
#define ucEWTrapeziumMin	0
#define ucEWTrapeziumMax	63
#define ucEWUpperCornerMin	0
#define ucEWUpperCornerMax	63
#define ucEWLowerCornerMin	0
#define ucEWLowerCornerMax	63
#define ucEWEHTCompensationMin	0
#define ucEWEHTCompensationMax	63

// Video Menu
//************
#define ucWhitePointRMin	0
#define ucWhitePointRMax	63
#define ucWhitePointGMin	0
#define ucWhitePointGMax	63
#define ucWhitePointBMin	0
#define ucWhitePointBMax	63
#define ucPeakWhiteLimiterMin	0
#define ucPeakWhiteLimiterMax	15
#define ucBlackLevelOffsetRMin	0
#define ucBlackLevelOffsetRMax	15
#define ucBlackLevelOffsetGMin	0
#define ucBlackLevelOffsetGMax	15
#define scGainPrescaleRGBMin -32
#define scGainPrescaleRGBMax	32
#define scFBDelayMin -8
#define scFBDelayMax	7
#define scLumaChromaDelayMin -8
#define scLumaChromaDelayMax	7

// Setup Menu
//************
#define ucTranslucencyMin	20
#define ucTranslucencyMax	127

// Sound Menu
//************
#define ucLSVolumeMin	0
#define ucLSVolumeMax	100
#define scLSBalMin	-50
#define scLSBalMax	50
#define ucLSBalTicWidth	50 
#define ucSTWideGainMin	1
#define ucSTWideGainMax	128
#define ucSRS3DSpaceMin	1
#define ucSRS3DSpaceMax	128
#define ucSRS3DCenterMin	1
#define ucSRS3DCenterMax	128
#define ucHpVolumeMin	0
#define ucHpVolumeMax	100
#define scHpBalMin	-50
#define scHpBalMax	50
#define ucHpBalTicWidth	50 
#define scHpBassMin	-12
#define scHpBassMax	12
#define ucHpBassTicWidth	12 
#define scHpTrebleMin	-12
#define scHpTrebleMax	12
#define ucHpTrebleTicWidth	12 
#define scEqBandMin	-12
#define scEqBandMax	12
#define ucEqBandTicWidth	4


// Service Sound Menu
//********************
#define ucThrSwNICAMMin	0	
#define ucThrSwNICAMMax	3	
#define scPrescNICAMMin	-6	
#define scPrescNICAMMax	24	
#define scPrescFMMin	-6	
#define scPrescFMMax	24	
#define scPrescSCARTMin	-6	
#define scPrescSCARTMax	6	
#define ucAGCMin	0		
#define ucAGCMax	30	

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

