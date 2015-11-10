/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: HeadPhoneMenuClass
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
class HeadPhoneMenuClass : public PegBitmapWindow
{
	public:
		HeadPhoneMenuClass(SIGNED iLeft, SIGNED iTop);
		SIGNED Message(const PegMessage &Mesg);
		enum ChildIds {
			HeadPhoneMenuWindowID = 1,
			HpLanguageStatusID,
			HpTrebleBarID,
			HpBassBarID,
			HpBalanceBarID,
			HpVolumeBarID,
			HpLanguageButtonID,
			HpTrebleButtonID,
			HpBassButtonID,
			HpBalanceButtonID,
			HpVolumeButtonID,
			HeadPhoneMenuTitleID,
		};

	private:
		void RefreshHeadPhoneMenuWindow(void);
		PegPrompt *pHpLanguageStatus;
		PegSlider *pHpTrebleBar;
		PegSlider *pHpBassBar;
		PegSlider *pHpBalanceBar;
		TVProgressBar *pHpVolumeBar;
		TVDecoratedButton *pHpLanguageButton;
		TVDecoratedButton *pHpTrebleButton;
		TVDecoratedButton *pHpBassButton;
		TVDecoratedButton *pHpBalanceButton;
		TVDecoratedButton *pHpVolumeButton;
		PegPrompt *pHeadPhoneMenuTitle;
};
