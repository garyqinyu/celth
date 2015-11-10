/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: EqualizerMenuClass
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
#include "lrtvdbut.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class EqualizerMenuClass : public PegBitmapWindow
{
	public:
		EqualizerMenuClass(SIGNED iLeft, SIGNED iTop);
		SIGNED Message(const PegMessage &Mesg);
		enum ChildIds {
			EqualizerMenuWindowID = 1,
			Eq_Band5_ID,
			Eq_Band4_ID,
			Eq_Band3_ID,
			Eq_Band2_ID,
			Eq_Band1_ID,
			Eq6600HzValueID,
			Eq3300HzValueID,
			Eq1000HzValueID,
			Eq330HzValueID,
			Eq100HzValueID,
			SoundPresetStatusID,
			Bar6600HzButtonID,
			Bar3300HzButtonID,
			Bar1000HzButtonID,
			Bar330HzButtonID,
			Bar100HzButtonID,
			SoundPresetButtonID,
			EqualizerMenuTitleID,
		};

	private:
		void RefreshEqualizerMenuWindow(void);
		void UpdateSliderPrompt(PegPrompt *pp, LONG lVal);
		void UpdateSlidersAndPrompts();
		PegPrompt *pSoundPresetStatus;
		PegPrompt *pEq6600HzValue;
		PegPrompt *pEq3300HzValue;
		PegPrompt *pEq1000HzValue;
		PegPrompt *pEq330HzValue;
		PegPrompt *pEq100HzValue;
		PegSlider *pEq_Band5;
		PegSlider *pEq_Band4;
		PegSlider *pEq_Band3;
		PegSlider *pEq_Band2;
		PegSlider *pEq_Band1;
		LRTVDecoratedButton *pBar6600HzButton;
		LRTVDecoratedButton *pBar3300HzButton;
		LRTVDecoratedButton *pBar1000HzButton;
		LRTVDecoratedButton *pBar330HzButton;
		LRTVDecoratedButton *pBar100HzButton;
		LRTVDecoratedButton *pSoundPresetButton;
		PegPrompt *pEqualizerMenuTitle;
};
