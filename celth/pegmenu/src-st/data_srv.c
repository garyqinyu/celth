/********************************************************************************
COPYRIGHT (C) STMicroelectronics 2000.

File name       : data_srv.h 
Author          : David Meunier/ P Voyer
Creation        : 24/07/2002
Modification    :
Date					DDTS Number       	Modification       	     Name 
===========				============        ===========              =====
	  
********************************************************************************/


/* Local Includes -------------------------------------------------------------*/
#include "ap_mmi.h"

#define __DATASRVMASTER /* only for mastership */
#include "data_srv.h"
#include "osal.h"

/* Private constant -----------------------------------------------------------*/

/* Private Variables ----------------------------------------------------------*/
Ssb_Picture_t  stPictureMmi;
OSAL_Sem_t		ap_mmi_PictureConfSem;
OSAL_SemId_t	ap_mmi_PictureConfSemID;
Ssb_Tuning_t	stTuningMmi;
OSAL_Sem_t		ap_mmi_TuningConfSem;
OSAL_SemId_t	ap_mmi_TuningConfSemID;
Ssb_AVConf_t	stAVUserMmi;
OSAL_Sem_t		ap_mmi_AvConfSem;
OSAL_SemId_t	ap_mmi_AvConfSemID;
Ssb_CrtType_t	eCrtTypeMmi;
OSAL_Sem_t		ap_mmi_CrtConfSem;
OSAL_SemId_t	ap_mmi_CrtConfSemID;
Ssb_Language_t	eLanguageMmi;
unsigned char ucTranslucencyMmi;
OSAL_Sem_t		ap_mmi_OsdConfSem;
OSAL_SemId_t	ap_mmi_OsdConfSemID;
Ssb_Geometry_t stGeometryMmi;
OSAL_Sem_t		ap_mmi_GeometryConfSem;
OSAL_SemId_t	ap_mmi_GeometryConfSemID;
Ssb_VideoConf_t	stVideoUserMmi;
OSAL_Sem_t		ap_mmi_VideoConfSem;
OSAL_SemId_t	ap_mmi_VideoConfSemID;
Ssb_Sound_t stSoundMmi;
OSAL_Sem_t		ap_mmi_SoundConfSem;
OSAL_SemId_t	ap_mmi_SoundConfSemID;
Ssb_SoundService_t stSoundsystemMmi;
OSAL_Sem_t		ap_mmi_SoundSysConfSem;
OSAL_SemId_t	ap_mmi_SoundSysConfSemID;
Ssb_SoundStatusInfo_t stSoundInfoMmi;
int iRestoreBlueLink = 0;
int iVideoNotGeometry;


/******************************************************************************
Function name	: ap_mmi_PictureGet
Description		: Function returning the data of picture & picture+ menu
Input param		: Ssb_Picture_t* pstPictureSettings
Output param	: Ssb_Picture_t* pstPictureSettings
******************************************************************************/
void	ap_mmi_PictureGet(Ssb_Picture_t* pstPictureSettings)
{
	OSAL_SemWait(ap_mmi_PictureConfSemID, OSAL_kWaitForEver);
	*pstPictureSettings=stPictureMmi;
	OSAL_SemRelease(ap_mmi_PictureConfSemID);
}

/******************************************************************************
Function name	: ap_mmi_RestoreBlueLinkGet
Description		: Function returning the RestoreBlueLink state
Input param		: none
Output param	: int
******************************************************************************/
int	ap_mmi_RestoreBlueLinkGet(void)
{
	return iRestoreBlueLink;
}

/******************************************************************************
Function name	: ap_mmi_VideoNotGeometryGet
Description		: Function returning the iVideoNotGeometry state
Input param		: none
Output param	: int
******************************************************************************/
int	ap_mmi_VideoNotGeometryGet(void)
{
	return iVideoNotGeometry;
}
/******************************************************************************
Function name	: ap_mmi_TuningGet
Description		: Function returning the data of tuning menu
Input param		: Ssb_Tuning_t* pstTuningPreference
Output param	: Ssb_Tuning_t* pstTuningPreference
******************************************************************************/
void	ap_mmi_TuningGet(Ssb_Tuning_t* pstTuningPreference)
{
	OSAL_SemWait(ap_mmi_TuningConfSemID, OSAL_kWaitForEver);
	*pstTuningPreference=stTuningMmi;
	OSAL_SemRelease(ap_mmi_TuningConfSemID);
}
/******************************************************************************
Function name	: ap_mmi_InOutAVManagementGet
Description		: Function returning the data of In Out management menu
Input param		: Ssb_AVConf_t* pstInOutManagement
Output param	: Ssb_AVConf_t* pstInOutManagement
******************************************************************************/
void	ap_mmi_InOutAVManagementGet(Ssb_AVConf_t* pstInOutManagement)
{
	OSAL_SemWait(ap_mmi_AvConfSemID, OSAL_kWaitForEver);
	*pstInOutManagement=stAVUserMmi;
	OSAL_SemRelease(ap_mmi_AvConfSemID);
}
/******************************************************************************
Function name	: ap_mmi_CrtTypeGet
Description		: Function returning the data crt type choose by the user
Input param		: none
Output param	: return Ssb_CrtType_t
******************************************************************************/
Ssb_CrtType_t	ap_mmi_CrtTypeGet(void)
{
	Ssb_CrtType_t CrtTypeReturned;
	OSAL_SemWait(ap_mmi_CrtConfSemID, OSAL_kWaitForEver);
	CrtTypeReturned = eCrtTypeMmi;
	OSAL_SemRelease(ap_mmi_CrtConfSemID);
	return (CrtTypeReturned);

}
/******************************************************************************
Function name	: ap_mmi_TranslucencyGet
Description		: Function returning the translucency set by the user
Input param		: none
Output param	: return unsigned char
******************************************************************************/
unsigned char	ap_mmi_TranslucencyGet(void)
{
	unsigned char ucTranslucencyReturned;
	OSAL_SemWait(ap_mmi_OsdConfSemID, OSAL_kWaitForEver);
	ucTranslucencyReturned = ucTranslucencyMmi;
	OSAL_SemRelease(ap_mmi_OsdConfSemID);
	return (ucTranslucencyReturned);
	
}
/******************************************************************************
Function name	: ap_mmi_MenuLanguageValueGet
Description		: Function returning the language of the menu set by the user
Input param		: none
Output param	: return Ssb_Language_t
******************************************************************************/
Ssb_Language_t	ap_mmi_MenuLanguageValueGet(void)
{
	return (eLanguageMmi);
}
/******************************************************************************
Function name	: ap_mmi_GeometryGet
Description		: Function returning the data of the geometry menu window
Input param		: Ssb_Geometry_t* pstGeometrySettings
Output param	: Ssb_Geometry_t* pstGeometrySettings
******************************************************************************/
void	ap_mmi_GeometryGet(Ssb_Geometry_t* pstGeometrySettings)
{
	OSAL_SemWait(ap_mmi_GeometryConfSemID, OSAL_kWaitForEver);
	*pstGeometrySettings = stGeometryMmi;
    OSAL_SemRelease(ap_mmi_GeometryConfSemID);
}
/******************************************************************************
Function name	: ap_mmi_VideoGet
Description		: Function returning the data of video menu window
Input param		: Ssb_VideoConf_t* pstVideoSettings
Output param	: Ssb_VideoConf_t* pstVideoSettings
******************************************************************************/
void	ap_mmi_VideoGet(Ssb_VideoConf_t* pstVideoSettings)
{
	OSAL_SemWait(ap_mmi_VideoConfSemID, OSAL_kWaitForEver);
	*pstVideoSettings = stVideoUserMmi;
	OSAL_SemRelease(ap_mmi_VideoConfSemID);
}
/******************************************************************************
Function name	: ap_mmi_SoundGet
Description		: Function returning the data of the sound menu window
Input param		: Ssb_Sound_t* pstSoundSettings
Output param	: Ssb_Sound_t* pstSoundSettings
******************************************************************************/
void	ap_mmi_SoundGet(Ssb_Sound_t* pstSoundSettings)
{
	OSAL_SemWait(ap_mmi_SoundConfSemID, OSAL_kWaitForEver);
	*pstSoundSettings = stSoundMmi;
	OSAL_SemRelease(ap_mmi_SoundConfSemID);
}
/******************************************************************************
Function name	: ap_mmi_SoundServiceGet
Description		: Function returning the data of the sound service menu window
Input param		: Ssb_SoundService_t* pstSoundServiceSettings
Output param	: Ssb_SoundService_t* pstSoundServiceSettings
******************************************************************************/
void	ap_mmi_SoundServiceGet(Ssb_SoundService_t* pstSoundServiceSettings)
{
	OSAL_SemWait(ap_mmi_SoundSysConfSemID, OSAL_kWaitForEver);
	*pstSoundServiceSettings = stSoundsystemMmi;
	OSAL_SemRelease(ap_mmi_SoundSysConfSemID);
}


/******************************************************************************
Function name	: PictureDataSet
Description		: Function modifying a value of the structure
Input param		: Which value, and the value
Output param	: none
******************************************************************************/
void	PictureDataSet(APCTRL_LinkId_t eEvent, uPictureData_t uValue)
{
	OSAL_SemWait(ap_mmi_PictureConfSemID, OSAL_kWaitForEver);
	switch (eEvent)
	{
		case Ssb_BrightnessChange:
			stPictureMmi.ucBrightness = uValue.ucValue;
			break;

		case Ssb_ContrastChange:
			stPictureMmi.ucContrast = uValue.ucValue;
			break;

		case Ssb_ColorChange:
			stPictureMmi.ucColor = uValue.ucValue;
			break;

		case Ssb_SharpnessChange:
			stPictureMmi.ucSharpness = uValue.ucValue;
			break;

		case Ssb_TintChange:
			stPictureMmi.ucTint = uValue.ucValue;
			break;

		case Ssb_EFCorrectionChange:
			stPictureMmi.ucRotation = uValue.ucValue;
			break;
	}
	OSAL_SemRelease(ap_mmi_PictureConfSemID);
}

/******************************************************************************
Function name	: GeometryDataSet
Description		: Function modifying a value of the structure
Input param		: Which value, and the value
Output param	: none
******************************************************************************/
void	GeometryDataSet(APCTRL_LinkId_t eEvent, uGeometryData_t uValue)
{

	OSAL_SemWait(ap_mmi_GeometryConfSemID, OSAL_kWaitForEver);
	switch (eEvent)
	{
		case Ssb_VslopeChange:
			stGeometryMmi.ucVslope = uValue.ucValue;
			break;

		case Ssb_VAmplitudeChange:
			stGeometryMmi.ucVAmplitude = uValue.ucValue;
			break;

		case Ssb_VPositionChange:
			stGeometryMmi.ucVPosition = uValue.ucValue;
			break;

		case Ssb_VSCorrectionChange:
			stGeometryMmi.ucVSCorrection = uValue.ucValue;
			break;

		case Ssb_HblankingChange:
			stGeometryMmi.ucHBlanking = uValue.ucValue;
			break;

		case Ssb_HShiftChange:
			stGeometryMmi.ucHShift = uValue.ucValue;
			break;

		case Ssb_HParallelogramChange:
			stGeometryMmi.ucHParallelogram = uValue.ucValue;
			break;

		case Ssb_HBowChange:
			stGeometryMmi.ucHbow = uValue.ucValue;
			break;

		case Ssb_EWWidthChange:
			stGeometryMmi.ucEWWidth = uValue.ucValue;
			break;

		case Ssb_EWPinCushionChange:
			stGeometryMmi.ucEWPinCushion = uValue.ucValue;
			break;

		case Ssb_EWTrapeziumChange:
			stGeometryMmi.ucEWTrapezium = uValue.ucValue;
			break;

		case Ssb_EWUpperCornerChange:
			stGeometryMmi.ucEwUpperCorner = uValue.ucValue;
			break;

		case Ssb_EWLowerCornerChange:
			stGeometryMmi.ucEwLowerCorner = uValue.ucValue;
			break;

		case Ssb_EWEHTCompChange:
			stGeometryMmi.ucEwEHTCompensation = uValue.ucValue;
			break;
	}
	OSAL_SemRelease(ap_mmi_GeometryConfSemID);
}
/******************************************************************************
Function name	: VideoDataSet
Description		: Function modifying a value of the structure
Input param		: Which value, and the value
Output param	: none
******************************************************************************/
void	VideoDataSet(APCTRL_LinkId_t eEvent, uVideoData_t uValue)
{
	OSAL_SemWait(ap_mmi_VideoConfSemID, OSAL_kWaitForEver);
	switch (eEvent)
	{
		case Ssb_WhitePointRChange:
			stVideoUserMmi.ucWhitePointR = uValue.ucValue;
			break;

		case Ssb_WhitePointGChange:
			stVideoUserMmi.ucWhitePointG = uValue.ucValue;
			break;

		case Ssb_WhitePointBChange:
			stVideoUserMmi.ucWhitePointB = uValue.ucValue;
			break;

		case Ssb_PeakWhiteLimChange:
			stVideoUserMmi.ucPeakWhiteLimiter = uValue.ucValue;
			break;

		case Ssb_BlackOffsetRChange:
			stVideoUserMmi.ucBlackLevelOffsetR = uValue.ucValue;
			break;

		case Ssb_BlackOffsetGChange:
			stVideoUserMmi.ucBlackLevelOffsetG = uValue.ucValue;
			break;

		case Ssb_GainPreScaleRGBChange:
			stVideoUserMmi.scGainPrescaleRGB = uValue.scValue;
			break;

		case Ssb_FastBlankingDelayChange:
			stVideoUserMmi.scFastBlankingDelay = uValue.scValue;
			break;

		case Ssb_LumaChromaDelayChange:
			stVideoUserMmi.scLumaChromaDelay = uValue.scValue;
			break;
	}
	OSAL_SemRelease(ap_mmi_VideoConfSemID);
}


/******************************************************************************
Function name	: SoundDataSet
Description		: Function modifying a value of the structure
Input param		: Which value, and the value
Output param	: none
******************************************************************************/
void	SoundDataSet(APCTRL_LinkId_t eEvent, uSoundData_t uValue)
{
	OSAL_SemWait(ap_mmi_SoundConfSemID, OSAL_kWaitForEver);
	switch (eEvent)
	{
		case Ssb_DirectVolumeChange:
		case Ssb_VolumeChange:
			stSoundMmi.ucVolume = uValue.ucValue;
			break;
		
		case Ssb_BalanceChange:
			stSoundMmi.scBalance = uValue.scValue;
			break;

		case Ssb_HeadPhoneVolChange:
			stSoundMmi.ucHeadPhoneVolume = uValue.ucValue;
			break;
		
		case Ssb_HeadPhoneBalChange:
			stSoundMmi.scHeadPhoneBalance = uValue.scValue;
			break;

		case Ssb_HeadPhoneBassChange:
			stSoundMmi.scHeadPhoneBass = uValue.scValue;
			break;

		case Ssb_HeadPhoneTrebleChange:
			stSoundMmi.scHeadPhoneTreble = uValue.scValue;
			break;
	}
	OSAL_SemRelease(ap_mmi_SoundConfSemID);
}


/******************************************************************************
Function name	: ServiceSoundDataSet
Description		: Function modifying a value of the structure
Input param		: Which value, and the value
Output param	: none
******************************************************************************/
void	ServiceSoundDataSet(APCTRL_LinkId_t eEvent, uServiceSoundData_t uValue)
{
	OSAL_SemWait(ap_mmi_SoundSysConfSemID, OSAL_kWaitForEver);
	switch (eEvent)
	{
		case Ssb_ThresholdNICAMChange:
			stSoundsystemMmi.ucThresholdSwitchNicam = uValue.ucValue;
			break;

		case Ssb_LevelPrescaleNICAMChange:
			stSoundsystemMmi.scLevelPrescNicam = uValue.scValue;
			break;

		case Ssb_LevelPrescaleFMChange:
			stSoundsystemMmi.scLevelPrescFM = uValue.scValue;
			break;

		case Ssb_LevelPrescaleScartChange:
			stSoundsystemMmi.scLevelPrescSCART = uValue.scValue;
			break;

		case Ssb_FIAGCadjustementChange:
			stSoundsystemMmi.ucLLFIAGCAdjust = uValue.ucValue;
			break;		
	}
	OSAL_SemRelease(ap_mmi_SoundSysConfSemID);
}


