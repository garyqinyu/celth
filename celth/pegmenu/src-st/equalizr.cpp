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


#include "peg.hpp"
#include "wbstrnga.hpp"
#include "equalizr.hpp"
#include "advaudio.hpp"
#include "anim.hpp"
#include "init_val.h"
#include "userfct.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EqualizerMenuClass::EqualizerMenuClass(SIGNED iLeft, SIGNED iTop) : 
	PegBitmapWindow(&gbEqualizerBitmap, kAutoAdjustBitmapSize, 220, FF_NONE)
{
	PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 464, iTop + 350);
	InitClient();
	SetColor(PCI_NORMAL, 188);
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

	ChildRect.Set(iLeft + 408, iTop + 60, iLeft + 431, iTop + 239);
	pEq_Band5 = new PegSlider(ChildRect, scEqBandMin, scEqBandMax, Eq_Band5_ID, FF_THIN, ucEqBandTicWidth);
	pEq_Band5->SetCurrentValue(0);
	pEq_Band5->RemoveStatus(PSF_ACCEPTS_FOCUS);
	Add(pEq_Band5);

	ChildRect.Set(iLeft + 333, iTop + 60, iLeft + 356, iTop + 239);
	pEq_Band4 = new PegSlider(ChildRect, scEqBandMin, scEqBandMax, Eq_Band4_ID, FF_THIN, ucEqBandTicWidth);
	pEq_Band4->SetCurrentValue(0);
	pEq_Band4->RemoveStatus(PSF_ACCEPTS_FOCUS);
	Add(pEq_Band4);

	ChildRect.Set(iLeft + 258, iTop + 60, iLeft + 281, iTop + 239);
	pEq_Band3 = new PegSlider(ChildRect, scEqBandMin, scEqBandMax, Eq_Band3_ID, FF_THIN, ucEqBandTicWidth);
	pEq_Band3->SetCurrentValue(0);
	pEq_Band3->RemoveStatus(PSF_ACCEPTS_FOCUS);
	Add(pEq_Band3);

	ChildRect.Set(iLeft + 183, iTop + 60, iLeft + 206, iTop + 239);
	pEq_Band2 = new PegSlider(ChildRect, scEqBandMin, scEqBandMax, Eq_Band2_ID, FF_THIN, ucEqBandTicWidth);
	pEq_Band2->SetCurrentValue(0);
	pEq_Band2->RemoveStatus(PSF_ACCEPTS_FOCUS);
	Add(pEq_Band2);

	ChildRect.Set(iLeft + 108, iTop + 60, iLeft + 131, iTop + 239);
	pEq_Band1 = new PegSlider(ChildRect, scEqBandMin, scEqBandMax, Eq_Band1_ID, FF_THIN, ucEqBandTicWidth);
	pEq_Band1->SetCurrentValue(0);
	pEq_Band1->RemoveStatus(PSF_ACCEPTS_FOCUS);
	Add(pEq_Band1);

	ChildRect.Set(iLeft + 400, iTop + 250, iLeft + 439, iTop + 277);
	pEq6600HzValue = new PegPrompt(ChildRect,LSA(SID_One), Eq6600HzValueID, FF_RECESSED|TJ_CENTER|TT_COPY);
	pEq6600HzValue->SetColor(PCI_NTEXT, BLUE);
	Add(pEq6600HzValue);

	ChildRect.Set(iLeft + 325, iTop + 250, iLeft + 364, iTop + 277);
	pEq3300HzValue = new PegPrompt(ChildRect,LSA(SID_One), Eq3300HzValueID, FF_RECESSED|TJ_CENTER|TT_COPY);
	pEq3300HzValue->SetColor(PCI_NTEXT, BLUE);
	Add(pEq3300HzValue);

	ChildRect.Set(iLeft + 250, iTop + 250, iLeft + 289, iTop + 277);
	pEq1000HzValue = new PegPrompt(ChildRect,LSA(SID_One), Eq1000HzValueID, FF_RECESSED|TJ_CENTER|TT_COPY);
	pEq1000HzValue->SetColor(PCI_NTEXT, BLUE);
	Add(pEq1000HzValue);

	ChildRect.Set(iLeft + 175, iTop + 250, iLeft + 214, iTop + 277);
	pEq330HzValue = new PegPrompt(ChildRect,LSA(SID_One), Eq330HzValueID, FF_RECESSED|TJ_CENTER|TT_COPY);
	pEq330HzValue->SetColor(PCI_NTEXT, BLUE);
	Add(pEq330HzValue);

	ChildRect.Set(iLeft + 100, iTop + 250, iLeft + 139, iTop + 277);
	pEq100HzValue = new PegPrompt(ChildRect,LSA(SID_One), Eq100HzValueID, FF_RECESSED|TJ_CENTER|TT_COPY);
	pEq100HzValue->SetColor(PCI_NTEXT, BLUE);
	Add(pEq100HzValue);

	ChildRect.Set(iLeft + 10, iTop + 250, iLeft + 84, iTop + 277);
	pSoundPresetStatus = new PegPrompt(ChildRect, LSA(SID_PresetMovie), SoundPresetStatusID, TJ_CENTER|TT_COPY);
	pSoundPresetStatus->SetColor(PCI_NTEXT, BLUE);
	Add(pSoundPresetStatus);

	ChildRect.Set(iLeft + 385, iTop + 290, iLeft + 459, iTop + 334);
	pBar6600HzButton = new LRTVDecoratedButton(ChildRect, LSA(SID_Band5), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, Bar6600HzButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT|BF_ORIENT_BR);
	pBar6600HzButton->SetColor(PCI_NORMAL, 223);
	pBar6600HzButton->SetColor(PCI_NTEXT, BLUE);
	Add(pBar6600HzButton);

	ChildRect.Set(iLeft + 310, iTop + 290, iLeft + 384, iTop + 334);
	pBar3300HzButton = new LRTVDecoratedButton(ChildRect, LSA(SID_Band4), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, Bar3300HzButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT|BF_ORIENT_BR);
	pBar3300HzButton->SetColor(PCI_NORMAL, 223);
	pBar3300HzButton->SetColor(PCI_NTEXT, BLUE);
	Add(pBar3300HzButton);

	ChildRect.Set(iLeft + 235, iTop + 290, iLeft + 309, iTop + 334);
	pBar1000HzButton = new LRTVDecoratedButton(ChildRect, LSA(SID_Band3), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, Bar1000HzButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT|BF_ORIENT_BR);
	pBar1000HzButton->SetColor(PCI_NORMAL, 223);
	pBar1000HzButton->SetColor(PCI_NTEXT, BLUE);
	Add(pBar1000HzButton);

	ChildRect.Set(iLeft + 160, iTop + 290, iLeft + 234, iTop + 334);
	pBar330HzButton = new LRTVDecoratedButton(ChildRect, LSA(SID_Band2), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, Bar330HzButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT|BF_ORIENT_BR);
	pBar330HzButton->SetColor(PCI_NORMAL, 223);
	pBar330HzButton->SetColor(PCI_NTEXT, BLUE);
	Add(pBar330HzButton);

	ChildRect.Set(iLeft + 85, iTop + 290, iLeft + 159, iTop + 334);
	pBar100HzButton = new LRTVDecoratedButton(ChildRect, LSA(SID_Band1), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, Bar100HzButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT|BF_ORIENT_BR);
	pBar100HzButton->SetColor(PCI_NORMAL, 223);
	pBar100HzButton->SetColor(PCI_NTEXT, BLUE);
	Add(pBar100HzButton);

	ChildRect.Set(iLeft + 10, iTop + 290, iLeft + 84, iTop + 334);
	pSoundPresetButton = new LRTVDecoratedButton(ChildRect, LSA(SID_SoundPresetButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, SoundPresetButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT|BF_ORIENT_BR);
	pSoundPresetButton->SetColor(PCI_NORMAL, 223);
	pSoundPresetButton->SetColor(PCI_NTEXT, BLUE);
	Add(pSoundPresetButton);

	ChildRect.Set(iLeft + 50, iTop + 2, iLeft + 399, iTop + 48);
	pEqualizerMenuTitle = new PegPrompt(ChildRect, LSA(SID_EqualizerMenuWindowTitle), EqualizerMenuTitleID);
	pEqualizerMenuTitle->SetColor(PCI_NORMAL, BLUE);
	pEqualizerMenuTitle->SetColor(PCI_NTEXT, BLUE);
	Add(pEqualizerMenuTitle);

	/* WB End Construction */

	/* Set font for the title, prompt, equalizer values and TVDecorated buttons */
	pEqualizerMenuTitle->SetFont(&Pronto_16);

	pSoundPresetStatus->SetFont(&Pronto_12);

	pEq100HzValue->SetFont(&Pronto_12);
	pEq330HzValue->SetFont(&Pronto_12);
	pEq1000HzValue->SetFont(&Pronto_12);
	pEq3300HzValue->SetFont(&Pronto_12);
	pEq6600HzValue->SetFont(&Pronto_12);

	pSoundPresetButton->SetFont(&Pronto_12);
	pBar100HzButton->SetFont(&Pronto_12);
	pBar330HzButton->SetFont(&Pronto_12);
	pBar1000HzButton->SetFont(&Pronto_12);
	pBar3300HzButton->SetFont(&Pronto_12);
	pBar6600HzButton->SetFont(&Pronto_12);

	/* Set color for the equalizer sliders and value background */
	pEq_Band1->SetColor(PCI_NORMAL, LIGHTGRAY);
	pEq_Band2->SetColor(PCI_NORMAL, LIGHTGRAY);
	pEq_Band3->SetColor(PCI_NORMAL, LIGHTGRAY);
	pEq_Band4->SetColor(PCI_NORMAL, LIGHTGRAY);
	pEq_Band5->SetColor(PCI_NORMAL, LIGHTGRAY);

	pEq100HzValue->SetColor(PCI_NORMAL, LIGHTGRAY);
	pEq330HzValue->SetColor(PCI_NORMAL, LIGHTGRAY);
	pEq1000HzValue->SetColor(PCI_NORMAL, LIGHTGRAY);
	pEq3300HzValue->SetColor(PCI_NORMAL, LIGHTGRAY);
	pEq6600HzValue->SetColor(PCI_NORMAL, LIGHTGRAY);

	RefreshEqualizerMenuWindow(); /* Refresh the parameters with appropriate data */
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED EqualizerMenuClass::Message(const PegMessage &Mesg)
{
	AdvAudioMenuClass *pAdvAudioWindow;

	switch (Mesg.wType)
	{
	case SIGNAL(SoundPresetButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(Bar6600HzButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(Bar6600HzButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		pEq_Band5->SetColor(PCI_NORMAL, WHITE);
		pEq6600HzValue->SetColor(PCI_NORMAL, WHITE);
		break;

	case SIGNAL(Bar6600HzButtonID, PSF_FOCUS_LOST):
		// Enter your code here:
		pEq_Band5->SetColor(PCI_NORMAL, LIGHTGRAY);
		pEq6600HzValue->SetColor(PCI_NORMAL, LIGHTGRAY);
		break;

	case SIGNAL(Bar3300HzButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(Bar3300HzButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		pEq_Band4->SetColor(PCI_NORMAL, WHITE);
		pEq3300HzValue->SetColor(PCI_NORMAL, WHITE);
		break;

	case SIGNAL(Bar3300HzButtonID, PSF_FOCUS_LOST):
		// Enter your code here:
		pEq_Band4->SetColor(PCI_NORMAL, LIGHTGRAY);
		pEq3300HzValue->SetColor(PCI_NORMAL, LIGHTGRAY);
		break;

	case SIGNAL(Bar1000HzButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(Bar1000HzButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		pEq_Band3->SetColor(PCI_NORMAL, WHITE);
		pEq1000HzValue->SetColor(PCI_NORMAL, WHITE);
		break;

	case SIGNAL(Bar1000HzButtonID, PSF_FOCUS_LOST):
		// Enter your code here:
		pEq_Band3->SetColor(PCI_NORMAL, LIGHTGRAY);
		pEq1000HzValue->SetColor(PCI_NORMAL, LIGHTGRAY);
		break;

	case SIGNAL(Bar330HzButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(Bar330HzButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		pEq_Band2->SetColor(PCI_NORMAL, WHITE);
		pEq330HzValue->SetColor(PCI_NORMAL, WHITE);
		break;

	case SIGNAL(Bar330HzButtonID, PSF_FOCUS_LOST):
		// Enter your code here:
		pEq_Band2->SetColor(PCI_NORMAL, LIGHTGRAY);
		pEq330HzValue->SetColor(PCI_NORMAL, LIGHTGRAY);
		break;

	case SIGNAL(Bar100HzButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(Bar100HzButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		pEq_Band1->SetColor(PCI_NORMAL, WHITE);
		pEq100HzValue->SetColor(PCI_NORMAL, WHITE);	
		break;

	case SIGNAL(Bar100HzButtonID, PSF_FOCUS_LOST):
		// Enter your code here:
		pEq_Band1->SetColor(PCI_NORMAL, LIGHTGRAY);
		pEq100HzValue->SetColor(PCI_NORMAL, LIGHTGRAY);
		break;

	case PM_KEYUP:
	case PM_KEYDOWN:

		switch(Mesg.iData)
		{
		case SoundPresetButtonID:

			break;

		case Bar100HzButtonID:

			if(Mesg.wType == PM_KEYUP)
			{
				if(pEq_Band1->GetCurrentValue() > pEq_Band1->GetMinValue())
					pEq_Band1->SetCurrentValue(pEq_Band1->GetCurrentValue()-1,TRUE);
			}
			else /* PM_KEYDOWN */
			{
				if(pEq_Band1->GetCurrentValue() < pEq_Band1->GetMaxValue())
					pEq_Band1->SetCurrentValue(pEq_Band1->GetCurrentValue()+1,TRUE);
			}

			UpdateSliderPrompt(pEq100HzValue, -(pEq_Band1->GetCurrentValue()));

			break;

		case Bar330HzButtonID:

			if(Mesg.wType == PM_KEYUP)
			{
				if(pEq_Band2->GetCurrentValue() > pEq_Band2->GetMinValue())
					pEq_Band2->SetCurrentValue(pEq_Band2->GetCurrentValue()-1,TRUE);
			}
			else /* PM_KEYDOWN */
			{
				if(pEq_Band2->GetCurrentValue() < pEq_Band2->GetMaxValue())
					pEq_Band2->SetCurrentValue(pEq_Band2->GetCurrentValue()+1,TRUE);
			}

			UpdateSliderPrompt(pEq330HzValue, -(pEq_Band2->GetCurrentValue()));

			break;

		case Bar1000HzButtonID:

			if(Mesg.wType == PM_KEYUP)
			{
				if(pEq_Band3->GetCurrentValue() > pEq_Band3->GetMinValue())
					pEq_Band3->SetCurrentValue(pEq_Band3->GetCurrentValue()-1,TRUE);
			}
			else /* PM_KEYDOWN */
			{
				if(pEq_Band3->GetCurrentValue() < pEq_Band3->GetMaxValue())
					pEq_Band3->SetCurrentValue(pEq_Band3->GetCurrentValue()+1,TRUE);
			}

			UpdateSliderPrompt(pEq1000HzValue, -(pEq_Band3->GetCurrentValue()));

			break;

		case Bar3300HzButtonID:

			if(Mesg.wType == PM_KEYUP)
			{
				if(pEq_Band4->GetCurrentValue() > pEq_Band4->GetMinValue())
					pEq_Band4->SetCurrentValue(pEq_Band4->GetCurrentValue()-1,TRUE);
			}
			else /* PM_KEYDOWN */
			{
				if(pEq_Band4->GetCurrentValue() < pEq_Band4->GetMaxValue())
					pEq_Band4->SetCurrentValue(pEq_Band4->GetCurrentValue()+1,TRUE);
			}

			UpdateSliderPrompt(pEq3300HzValue, -(pEq_Band4->GetCurrentValue()));

			break;

		case Bar6600HzButtonID:

			if(Mesg.wType == PM_KEYUP)
			{
				if(pEq_Band5->GetCurrentValue() > pEq_Band5->GetMinValue())
					pEq_Band5->SetCurrentValue(pEq_Band5->GetCurrentValue()-1,TRUE);
			}
			else /* PM_KEYDOWN */
			{
				if(pEq_Band5->GetCurrentValue() < pEq_Band5->GetMaxValue())
					pEq_Band5->SetCurrentValue(pEq_Band5->GetCurrentValue()+1,TRUE);
			}

			UpdateSliderPrompt(pEq6600HzValue, -(pEq_Band5->GetCurrentValue()));

			break;		

		}
		break;    

	case PM_REFRESHWINDOW:
		RefreshEqualizerMenuWindow();
		break;

	case PM_CLOSE:
		/* left key = close = go back to previous menu */
		Destroy(this);
		pAdvAudioWindow = (AdvAudioMenuClass*) Presentation()->Find(ADV_AUDIO_MENU_ID);
		pAdvAudioWindow->SetBitmap(&gbAdvAudioBitmap);
		break;
	
	case PM_CURRENT:
		Presentation()->MoveFocusTree(pSoundPresetButton);
		return PegWindow::Message(Mesg);

	default:
		return PegWindow::Message(Mesg);
	}
	return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void EqualizerMenuClass::RefreshEqualizerMenuWindow(void)
{
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void EqualizerMenuClass::UpdateSliderPrompt(PegPrompt *pp, LONG lVal)
{
	PEGCHAR cTemp[40];

	ltoa((SIGNED) lVal, cTemp, 10);
	pp->DataSet(cTemp);
	pp->Draw();    
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void EqualizerMenuClass::UpdateSlidersAndPrompts()
{
}



