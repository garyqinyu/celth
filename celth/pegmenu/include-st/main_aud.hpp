/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: MainAudioMenuClass
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
#include "tvprobar.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class MainAudioMenuClass : public PegBitmapWindow
{
	public:
		MainAudioMenuClass(SIGNED iLeft, SIGNED iTop);
		SIGNED Message(const PegMessage &Mesg);
		enum ChildIds {
			AudioMenuWindowID = 1,
			SpEffectStatusID,
			LoudnessStatusID,
			BalanceBarID,
			VolumeBarID,
			AudioTypeStatusID,
			AdvancedAudioButtonID,
			SpLevelButtonID,
			SpEffectButtonID,
			LoudnessButtonID,
			BalanceButtonID,
			VolumeButtonID,
			AudioTypeButtonID,
			AudioMenuTitleID,
		};

	private:
		void RefreshMainAudioWindow(void);
		void DisplayChildWindow(int WindowID);
		PegPrompt *pSpEffectStatus;
		PegPrompt *pLoudnessStatus;
		PegSlider *pBalanceBar;
		TVProgressBar *pVolumeBar;
		PegPrompt *pAudioTypeStatus;
		TVDecoratedButton *pAdvancedAudioButton;
		TVDecoratedButton *pSpLevelButton;
		TVDecoratedButton *pSpEffectButton;
		TVDecoratedButton *pLoudnessButton;
		TVDecoratedButton *pBalanceButton;
		TVDecoratedButton *pVolumeButton;
		TVDecoratedButton *pAudioTypeButton;
		PegPrompt *pAudioMenuTitle;
};
