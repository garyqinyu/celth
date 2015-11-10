/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: TVProgressWindowClass
//  
//   Copyright (c) STMicroelectronics
//                 ZI de Rousset, BP2
//                 13106 ROUSSET CEDEX
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifndef _TEMPHEADER_HPP
#define _TEMPHEADER_HPP

typedef	enum 
	{
			Ssb_BrightnessChange,
			Ssb_ContrastChange,
			Ssb_ColorChange,
			Ssb_SharpnessChange,
			Ssb_TintChange,
			Ssb_ColorTemperatureChange,
			Ssb_VolumeChange,
			Ssb_MuteModeChange,
			Ssb_CloseCaptionChange,
			Ssb_ZoomModeChange,
			Ssb_InputChange,
			Ssb_HelpModeChange,
			Ssb_GreenBoostChange,
			Ssb_EdgeReplacementChange,
			Ssb_DCIChange,
			Ssb_PeakingChange,
			Ssb_AutoFleshChange,
			kNoLink
}APCTRL_LinkId_t;

typedef enum {
		kDirectKeyState,
		kPegMenuPicture,
		kPegMenuSound,
		kPegMenuSuperFunction,
		kPegMenuSuperSetting,
		kPegMenuWhiteBalance,
		kPegMenuGeometry,
		kPegMenuVideo,
		kPegStateNumber /* This line MUST always be the last one of this enum */
}APCTRL_PegState_t;
#endif /*_TEMPHEADER_HPP*/
