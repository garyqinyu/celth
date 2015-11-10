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


#include "peg.hpp"
#include "wbstrnga.hpp"
#include "headphon.hpp"
#include "advaudio.hpp"
#include "anim.hpp"
#include "init_val.h"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
HeadPhoneMenuClass::HeadPhoneMenuClass(SIGNED iLeft, SIGNED iTop) : 
	PegBitmapWindow(&gbHeadPhoneBitmap, kAutoAdjustBitmapSize, 220, FF_NONE)
{
	PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 309, iTop + 259);
	InitClient();
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

	ChildRect.Set(iLeft + 150, iTop + 222, iLeft + 229, iTop + 245);
	pHpLanguageStatus = new PegPrompt(ChildRect, LSA(SID_LeftOnBoth), HpLanguageStatusID);
	pHpLanguageStatus->SetColor(PCI_NTEXT, BLUE);
	Add(pHpLanguageStatus);

	ChildRect.Set(iLeft + 150, iTop + 182, iLeft + 294, iTop + 205);
	pHpTrebleBar = new PegSlider(ChildRect, scHpBassMin, scHpBassMax, HpTrebleBarID, FF_RAISED, ucHpBassTicWidth);
	pHpTrebleBar->SetCurrentValue(0);
	pHpTrebleBar->SetColor(PCI_NORMAL, 223);
	Add(pHpTrebleBar);

	ChildRect.Set(iLeft + 150, iTop + 142, iLeft + 294, iTop + 165);
	pHpBassBar = new PegSlider(ChildRect, scHpTrebleMin, scHpTrebleMax, HpBassBarID, FF_RAISED, ucHpTrebleTicWidth);
	pHpBassBar->SetCurrentValue(0);
	pHpBassBar->SetColor(PCI_NORMAL, 223);
	Add(pHpBassBar);

	ChildRect.Set(iLeft + 150, iTop + 102, iLeft + 294, iTop + 125);
	pHpBalanceBar = new PegSlider(ChildRect, scHpBalMin, scHpBalMax, HpBalanceBarID, FF_RAISED, ucHpBalTicWidth);
	pHpBalanceBar->SetCurrentValue(0);
	pHpBalanceBar->SetColor(PCI_NORMAL, 223);
	Add(pHpBalanceBar);

	ChildRect.Set(iLeft + 150, iTop + 62, iLeft + 294, iTop + 85);
	pHpVolumeBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucHpVolumeMin, ucHpVolumeMax, 0);
	Add(pHpVolumeBar);

	ChildRect.Set(iLeft + 13, iTop + 220, iLeft + 132, iTop + 247);
	pHpLanguageButton = new TVDecoratedButton(ChildRect, LSA(SID_HpLanguageButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HpLanguageButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pHpLanguageButton->SetColor(PCI_NORMAL, 223);
	pHpLanguageButton->SetColor(PCI_NTEXT, BLUE);
	Add(pHpLanguageButton);

	ChildRect.Set(iLeft + 13, iTop + 180, iLeft + 132, iTop + 207);
	pHpTrebleButton = new TVDecoratedButton(ChildRect, LSA(SID_TrebleButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HpTrebleButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pHpTrebleButton->SetColor(PCI_NORMAL, 223);
	pHpTrebleButton->SetColor(PCI_NTEXT, BLUE);
	Add(pHpTrebleButton);

	ChildRect.Set(iLeft + 13, iTop + 140, iLeft + 132, iTop + 167);
	pHpBassButton = new TVDecoratedButton(ChildRect, LSA(SID_BassButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HpBassButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pHpBassButton->SetColor(PCI_NORMAL, 223);
	pHpBassButton->SetColor(PCI_NTEXT, BLUE);
	Add(pHpBassButton);

	ChildRect.Set(iLeft + 13, iTop + 100, iLeft + 132, iTop + 127);
	pHpBalanceButton = new TVDecoratedButton(ChildRect, LSA(SID_BalanceButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HpBalanceButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pHpBalanceButton->SetColor(PCI_NORMAL, 223);
	pHpBalanceButton->SetColor(PCI_NTEXT, BLUE);
	Add(pHpBalanceButton);

	ChildRect.Set(iLeft + 13, iTop + 60, iLeft + 132, iTop + 87);
	pHpVolumeButton = new TVDecoratedButton(ChildRect, LSA(SID_VolumeButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HpVolumeButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pHpVolumeButton->SetColor(PCI_NORMAL, 223);
	pHpVolumeButton->SetColor(PCI_NTEXT, BLUE);
	Add(pHpVolumeButton);

	ChildRect.Set(iLeft + 20, iTop + 5, iLeft + 319, iTop + 51);
	pHeadPhoneMenuTitle = new PegPrompt(ChildRect, LSA(SID_HeadPhoneMenuWindowTitle), HeadPhoneMenuTitleID);
	pHeadPhoneMenuTitle->SetColor(PCI_NORMAL, BLUE);
	pHeadPhoneMenuTitle->SetColor(PCI_NTEXT, BLUE);
	Add(pHeadPhoneMenuTitle);

	/* WB End Construction */

	/* Set font for the title, buttons, prompts and numeric values */

	pHeadPhoneMenuTitle->SetFont(&Pronto_16);

	pHpVolumeButton->SetFont(&Pronto_12);
	pHpBalanceButton->SetFont(&Pronto_12);
	pHpBassButton->SetFont(&Pronto_12);
	pHpTrebleButton->SetFont(&Pronto_12);
	pHpLanguageButton->SetFont(&Pronto_12);

	pHpLanguageStatus->SetFont(&Pronto_12);

	pHpVolumeBar->SetFont(&Pronto_12);

	RefreshHeadPhoneMenuWindow(); /* Refresh the parameters with values from sound structure */
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED HeadPhoneMenuClass::Message(const PegMessage &Mesg)
{
	AdvAudioMenuClass		*pAdvAudioWindow;

	switch (Mesg.wType)
	{
	case SIGNAL(HpLanguageButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(HpTrebleButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(HpBassButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(HpBalanceButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(HpVolumeButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case PM_KEYLEFT:
	case PM_KEYRIGHT:

		switch(Mesg.iData)
		{
		case HpVolumeButtonID:

			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pHpVolumeBar->Value() < pHpVolumeBar->GetMaxValue())
				{
					pHpVolumeBar->Update(pHpVolumeBar->Value()+1,TRUE);						
				}
			}
			else // PM_KEYLEFT
			{
				if (pHpVolumeBar->Value() > pHpVolumeBar->GetMinValue())
				{
					pHpVolumeBar->Update(pHpVolumeBar->Value()-1,TRUE);
				}
			}

			break;

		case HpBalanceButtonID:

			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pHpBalanceBar->GetCurrentValue() < pHpBalanceBar->GetMaxValue())
				{
					pHpBalanceBar->SetCurrentValue(pHpBalanceBar->GetCurrentValue()+1, TRUE);
				}
			}
			else // PM_KEYLEFT
			{
				if (pHpBalanceBar->GetCurrentValue() > pHpBalanceBar->GetMinValue())
				{
					pHpBalanceBar->SetCurrentValue(pHpBalanceBar->GetCurrentValue()-1, TRUE);
				}
			}

			break;

		case HpBassButtonID:

			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pHpBassBar->GetCurrentValue() < pHpBassBar->GetMaxValue())
				{
					pHpBassBar->SetCurrentValue(pHpBassBar->GetCurrentValue()+1, TRUE);
				}
			}
			else // PM_KEYLEFT
			{
				if (pHpBassBar->GetCurrentValue() > pHpBassBar->GetMinValue())
				{
					pHpBassBar->SetCurrentValue(pHpBassBar->GetCurrentValue()-1, TRUE);
				}
			}

			break;

		case HpTrebleButtonID:

			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pHpTrebleBar->GetCurrentValue() < pHpTrebleBar->GetMaxValue())
				{
					pHpTrebleBar->SetCurrentValue(pHpTrebleBar->GetCurrentValue()+1, TRUE);
				}
			}
			else // PM_KEYLEFT
			{
				if (pHpTrebleBar->GetCurrentValue() > pHpTrebleBar->GetMinValue())
				{
					pHpTrebleBar->SetCurrentValue(pHpTrebleBar->GetCurrentValue()-1, TRUE);
				}
			}

			break;

		case HpLanguageButtonID:


			break;
		}
		break;    

	case PM_CLOSE:
		/* left key = close = go back to previous menu */
		Destroy(this);
		pAdvAudioWindow = (AdvAudioMenuClass*) Presentation()->Find(ADV_AUDIO_MENU_ID);
		pAdvAudioWindow->SetBitmap(&gbAdvAudioBitmap);
		break;

	default:
		return PegWindow::Message(Mesg);
	}
	return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void HeadPhoneMenuClass::RefreshHeadPhoneMenuWindow(void)
{
}

