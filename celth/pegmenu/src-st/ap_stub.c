/********************************************************************************
COPYRIGHT (C) STMicroelectronics 2000.

File name		: ap_stub.c
Author			: David Meunier
Description		: Contains the stub of all AP_CTRL API function
Creation		: Jul 2002
Modification	:
	Date		DDTS Number			Modification			Name
	----        -----------         ------------			----


********************************************************************************/

/* Local Includes -------------------------------------------------------------*/
#include "ap_cmd.h"


/* Private constant -----------------------------------------------------------*/
static const char* tpcSoundStandard[] =
{
	"NICAM",
	"ZWEITON",
	"STEREO",
	"MONO"
};


/* Private Variables ----------------------------------------------------------*/

unsigned char ucSoundStandard =0;
unsigned char ucSoundVolume =0;
Ssb_Sound_t stSoundUser;
Ssb_SoundService_t stSoundsystemUser;

/* Interface procedures/functions ---------------------------------------------*/
/******************************************************************************
Function name	: ap_cmd_SoundStandardStringGet
Description		: Function returning the string of the sound standard detection
Input param		: none
Output param	: return a pointer of constant char
******************************************************************************/
char*	ap_cmd_SoundStandardStringGet(void)
{
	return ((char*) tpcSoundStandard[ucSoundStandard]);
}

/******************************************************************************
Function name	: ap_cmd_SoundVolumeGet
Description		: Function returning the volume sound value  
Input param		: none
Output param	: return an unsigned char.
******************************************************************************/
/*unsigned char	ap_cmd_SoundVolumeGet(void)
{
	return (ucSoundVolume);
}*/
/******************************************************************************
Function name	: ap_cmd_SoundGet
Description		: Function returning the parameters of the sound processing
Input param		: Ssb_Sound_t* pstSoundUserSettings
Output param	: Ssb_Sound_t* pstSoundUserSettings 
******************************************************************************/
/*void ap_cmd_SoundGet(Ssb_Sound_t* pstSoundUserSettings)
{
		stSoundUser.eType = Ssb_kSoundTypeAuto;
		stSoundUser.eLanguage = Ssb_kL1;
		stSoundUser.eSpatialStereoEffect= Ssb_kSRS;
		stSoundUser.eSpatialMonoEffect=Ssb_kSRS3Dmono;
		stSoundUser.ucSpatialSpaceLevel= 10;
		stSoundUser.ucSpatialCenterLevel=10;
		stSoundUser.ucVolume = 20;
		stSoundUser.ucBalance= 15;
		stSoundUser.eSVC = Ssb_kOff;
		stSoundUser.eLoudness = Ssb_kOff;
		stSoundUser.eSubwoofer = Ssb_kOff;
		stSoundUser.eSoundPreset = Ssb_kSoundMovie;
		stSoundUser.stEqualizerFiveBand.ucBand1 = 10;
		stSoundUser.stEqualizerFiveBand.ucBand2 = 10;
		stSoundUser.stEqualizerFiveBand.ucBand3 = 10;
		stSoundUser.stEqualizerFiveBand.ucBand4 = 10;
		stSoundUser.stEqualizerFiveBand.ucBand5 = 10;
		stSoundUser.ucHeadPhoneVolume = 10;
		stSoundUser.ucHeadPhoneBalance = 15;
		stSoundUser.ucHeadPhoneBass = 10;
		stSoundUser.ucHeadPhoneTreble = 3;
		stSoundUser.eHeadPhoneLanguage= 10;
		
		*pstSoundUserSettings=stSoundUser;
}*/
/******************************************************************************
Function name	: ap_cmd_SoundServiceGet
Description		: Function returning the parameters of the sound system param
Input param		: Ssb_SoundService_t* pstSoundSystem
Output param	: Ssb_SoundService_t* pstSoundSystem
******************************************************************************/
/*void ap_cmd_SoundServiceGet(Ssb_SoundService_t* pstSoundSystem)
{
	stSoundsystemUser.ucLLFIAGCAdjust = 10;
	stSoundsystemUser.scLevelPrescSCART = 20;
	stSoundsystemUser.scLevelPrescFM = 10;
	stSoundsystemUser.scLevelPrescNicam = 12;
	stSoundsystemUser.ucThresholdSwitchNicam = 15;
	*pstSoundSystem=stSoundsystemUser;
}*/




/* EOF ap_stub.c  */

