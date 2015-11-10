/********************************************************************************
COPYRIGHT (C) STMicroelectronics 2000.

File name       : mmi_stub.h 
Author          : David Meunier: 
Creation        : 24/07/2002
Modification    :
Date					DDTS Number       	Modification       	     Name 
===========				============        ===========              =====
	  
********************************************************************************/


/* Local Includes -------------------------------------------------------------*/
#include "ap_mmi.h"


/* Private constant -----------------------------------------------------------*/

/* Private Variables ----------------------------------------------------------*/
Ssb_Picture_t  stPictureMmi={32,32,22,32,Ssb_Tnr_Auto,Ssb_kContrastOff,
							Ssb_kOn,Ssb_kOn,Ssb_kOn,32,Ssb_kAuto,32,Ssb_kOn,
							Ssb_kOn};
Ssb_Tuning_t	stTuningMmi={2,65550000,Ssb_kOthers,Ssb_kPAL,Ssb_kRFBG};
Ssb_AVConf_t	stAVUserMmi={Ssb_kCvbs,Ssb_kCvbs,Ssb_kFrontCvbs,
Ssb_kTuneronAV1,Ssb_kFrontonAV2};
Ssb_CrtType_t	eCrtTypeMmi=Ssb_kCrt43;
unsigned char ucTranslucencyMmi=30;
Ssb_Geometry_t stGeometryMmi={31,31,31,0,7,31,7,7,31,31,31,31,31,31};
Ssb_VideoConf_t	stVideoUserMmi={32,32,32,7,7,7,0,0,0};
Ssb_Sound_t stSoundMmi = {Ssb_kSoundTypeAuto,Ssb_kL1,Ssb_kSRS,
					Ssb_kSRS3Dmono,10,20,15,10,Ssb_kOff,Ssb_kOff,Ssb_kOff,
					Ssb_kSoundMovie,{5,5,5,5,5},10,15,10,12,Ssb_kL1};
Ssb_SoundService_t stSoundsystemMmi = {10,0,0,0,0};

/******************************************************************************
Function name	: ap_mmi_PictureGet
Description		: Function returning the data of picture & picture+ menu
Input param		: Ssb_Picture_t* pstPictureSettings
Output param	: Ssb_Picture_t* pstPictureSettings
******************************************************************************/
void ap_mmi_PictureGet(Ssb_Picture_t* pstPictureSettings)
{
	*pstPictureSettings = stPictureMmi;
}
/******************************************************************************
Function name	: ap_mmi_TuningGet
Description		: Function returning the data of tuning menu
Input param		: Ssb_Tuning_t* pstTuningPreference
Output param	: Ssb_Tuning_t* pstTuningPreference
******************************************************************************/
void ap_mmi_TuningGet(Ssb_Tuning_t* pstTuningPreference)
{
	*pstTuningPreference = stTuningMmi;
}
/******************************************************************************
Function name	: ap_mmi_InOutAVManagementGet
Description		: Function returning the data of In Out management menu
Input param		: Ssb_AVConf_t* pstInOutManagement
Output param	: Ssb_AVConf_t* pstInOutManagement
******************************************************************************/
void ap_mmi_InOutAVManagementGet(Ssb_AVConf_t* pstInOutManagement)
{
	*pstInOutManagement = stAVUserMmi;
}
/******************************************************************************
Function name	: ap_mmi_CrtTypeGet
Description		: Function returning the data crt type choose by the user
Input param		: none
Output param	: return Ssb_CrtType_t
******************************************************************************/
Ssb_CrtType_t ap_mmi_CrtTypeGet(void)
{
	return (eCrtTypeMmi);
}
/******************************************************************************
Function name	: ap_mmi_TranslucencyGet
Description		: Function returning the translucency set by the user
Input param		: none
Output param	: return unsigned char
******************************************************************************/
unsigned char ap_mmi_TranslucencyGet(void)
{
	return (ucTranslucencyMmi);
}
/******************************************************************************
Function name	: ap_mmi_MenuLanguageValueGet
Description		: Function returning the language of the menu set by the user
Input param		: none
Output param	: return Ssb_Language_t
******************************************************************************/
Ssb_Language_t ap_mmi_MenuLanguageValueGet(void)
{
	return (Ssb_kEnglish);
}
/******************************************************************************
Function name	: ap_mmi_GeometryGet
Description		: Function returning the data of the geometry menu window
Input param		: Ssb_Geometry_t* pstGeometrySettings
Output param	: Ssb_Geometry_t* pstGeometrySettings
******************************************************************************/
void ap_mmi_GeometryGet(Ssb_Geometry_t* pstGeometrySettings)
{
	*pstGeometrySettings = stGeometryMmi;
}
/******************************************************************************
Function name	: ap_mmi_VideoGet
Description		: Function returning the data of video menu window
Input param		: Ssb_VideoConf_t* pstVideoSettings
Output param	: Ssb_VideoConf_t* pstVideoSettings
******************************************************************************/
void ap_mmi_VideoGet(Ssb_VideoConf_t* pstVideoSettings)
{
	*pstVideoSettings = stVideoUserMmi;
}
/******************************************************************************
Function name	: ap_mmi_SoundGet
Description		: Function returning the data of the sound menu window
Input param		: Ssb_Sound_t* pstSoundSettings
Output param	: Ssb_Sound_t* pstSoundSettings
******************************************************************************/
void ap_mmi_SoundGet(Ssb_Sound_t* pstSoundSettings)
{
	*pstSoundSettings = stSoundMmi;	
}
/******************************************************************************
Function name	: ap_mmi_SoundServiceGet
Description		: Function returning the data of the sound service menu window
Input param		: Ssb_SoundService_t* pstSoundServiceSettings
Output param	: Ssb_SoundService_t* pstSoundServiceSettings
******************************************************************************/
void ap_mmi_SoundServiceGet(Ssb_SoundService_t* pstSoundServiceSettings)
{
	*pstSoundServiceSettings = stSoundsystemMmi;
}
