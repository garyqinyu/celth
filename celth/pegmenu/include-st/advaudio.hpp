/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: AdvAudioMenuClass
//  
//  Copyright (c) STMicroelectronics Pvt.Ltd.
//                TVD Application Lab
//                Noida, India.
//  
//  All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


#include "pbitmwin.hpp"
#include "tvdecbut.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class AdvAudioMenuClass : public PegBitmapWindow
{
	public:
		AdvAudioMenuClass(SIGNED iLeft, SIGNED iTop);
		SIGNED Message(const PegMessage &Mesg);
		enum ChildIds {
			AdvAudioWindowID = 1,
			AudioLangStatusID,
			SubwooferStatusID,
			SmartVolumeStatusID,
			HeadPhoneButtonID,
			EqualizerButtonID,
			AudioLanguageButtonID,
			SubwooferButtonID,
			SmartVolumeButtonID,
			AdvAudioMenuTitleID,            
		};

	private:
		void RefreshAdvAudioWindow(void);
		void DisplayChildWindow(int WindowID);
		PegPrompt *pAudioLangStatus;
		PegPrompt *pSubwooferStatus;
		PegPrompt *pSmartVolumeStatus;
		TVDecoratedButton *pHeadPhoneButton;
		TVDecoratedButton *pEqualizerButton;
		TVDecoratedButton *pAudioLanguageButton;
		TVDecoratedButton *pSubwooferButton;
		TVDecoratedButton *pSmartVolumeButton;
		PegPrompt *pAdvAudioMenuTitle;
};
