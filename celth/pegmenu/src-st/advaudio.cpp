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


#include "peg.hpp"
#include "wbstring.hpp"
#include "wbstrnga.hpp"
#include "advaudio.hpp"
#include "headphon.hpp"
#include "equalizr.hpp"
#include "main_aud.hpp"
#include "anim.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
AdvAudioMenuClass::AdvAudioMenuClass(SIGNED iLeft, SIGNED iTop) : 
	PegBitmapWindow(&gbAdvAudioBitmap, kAutoAdjustBitmapSize, 220, FF_NONE)
{
	PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 309, iTop + 259);
	InitClient();
	SetColor(PCI_NTEXT, BLUE);
	SetColor(PCI_STEXT, BLACK);
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

	ChildRect.Set(iLeft + 220, iTop + 135, iLeft + 299, iTop + 162);
	pAudioLangStatus = new PegPrompt(ChildRect, LSA(SID_LeftOnBoth), AudioLangStatusID);
	pAudioLangStatus->SetColor(PCI_NTEXT, BLUE);
	Add(pAudioLangStatus);

	ChildRect.Set(iLeft + 220, iTop + 95, iLeft + 299, iTop + 122);
	pSubwooferStatus = new PegPrompt(ChildRect, LS(SID_OFF), SubwooferStatusID);
	pSubwooferStatus->SetColor(PCI_NTEXT, BLUE);
	Add(pSubwooferStatus);

	ChildRect.Set(iLeft + 220, iTop + 55, iLeft + 299, iTop + 82);
	pSmartVolumeStatus = new PegPrompt(ChildRect, LS(SID_OFF), SmartVolumeStatusID);
	pSmartVolumeStatus->SetColor(PCI_NTEXT, BLUE);
	Add(pSmartVolumeStatus);

	ChildRect.Set(iLeft + 13, iTop + 215, iLeft + 172, iTop + 242);
	pHeadPhoneButton = new TVDecoratedButton(ChildRect, LSA(SID_HeadPhoneButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HeadPhoneButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pHeadPhoneButton->SetColor(PCI_NORMAL, 223);
	pHeadPhoneButton->SetColor(PCI_NTEXT, BLUE);
	Add(pHeadPhoneButton);

	ChildRect.Set(iLeft + 13, iTop + 175, iLeft + 172, iTop + 202);
	pEqualizerButton = new TVDecoratedButton(ChildRect, LSA(SID_EqualizerButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, EqualizerButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pEqualizerButton->SetColor(PCI_NORMAL, 223);
	pEqualizerButton->SetColor(PCI_NTEXT, BLUE);
	Add(pEqualizerButton);

	ChildRect.Set(iLeft + 13, iTop + 135, iLeft + 172, iTop + 162);
	pAudioLanguageButton = new TVDecoratedButton(ChildRect, LSA(SID_AudioLanguageButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, AudioLanguageButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pAudioLanguageButton->SetColor(PCI_NORMAL, 223);
	pAudioLanguageButton->SetColor(PCI_NTEXT, BLUE);
	Add(pAudioLanguageButton);

	ChildRect.Set(iLeft + 13, iTop + 95, iLeft + 172, iTop + 122);
	pSubwooferButton = new TVDecoratedButton(ChildRect, LSA(SID_SubwooferButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, SubwooferButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pSubwooferButton->SetColor(PCI_NORMAL, 223);
	pSubwooferButton->SetColor(PCI_NTEXT, BLUE);
	Add(pSubwooferButton);

	ChildRect.Set(iLeft + 13, iTop + 55, iLeft + 182, iTop + 82);
	pSmartVolumeButton = new TVDecoratedButton(ChildRect, LSA(SID_SmartVolumeButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, SmartVolumeButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pSmartVolumeButton->SetColor(PCI_NORMAL, 223);
	pSmartVolumeButton->SetColor(PCI_NTEXT, BLUE);
	Add(pSmartVolumeButton);

	ChildRect.Set(iLeft + 20, iTop + 5, iLeft + 219, iTop + 51);
	pAdvAudioMenuTitle = new PegPrompt(ChildRect, LSA(SID_AdvAudioMenuWindowTitle), AdvAudioMenuTitleID);
	pAdvAudioMenuTitle->SetColor(PCI_NORMAL, BLUE);
	pAdvAudioMenuTitle->SetColor(PCI_NTEXT, BLUE);
	Add(pAdvAudioMenuTitle);

	/* WB End Construction */

	Id(ADV_AUDIO_MENU_ID);

	/* Set font for the title, buttons and prompts */

	pAdvAudioMenuTitle->SetFont(&Pronto_16);

	pHeadPhoneButton->SetFont(&Pronto_12);
	pEqualizerButton->SetFont(&Pronto_12);
	pAudioLanguageButton->SetFont(&Pronto_12);
	pSubwooferButton->SetFont(&Pronto_12);
	pSmartVolumeButton->SetFont(&Pronto_12);

	pAudioLangStatus->SetFont(&Pronto_12);
	pSubwooferStatus->SetFont(&Pronto_12);
	pSmartVolumeStatus->SetFont(&Pronto_12);

	RefreshAdvAudioWindow();/* Refresh the parameters with values from the sound structure */
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED AdvAudioMenuClass::Message(const PegMessage &Mesg)
{
	MainAudioMenuClass *pMainAudioWindow;
			
	switch (Mesg.wType)
	{
	case SIGNAL(HeadPhoneButtonID, PSF_CLICKED):
		// Enter your code here:
		DisplayChildWindow(HeadPhoneButtonID);
		break;

	case SIGNAL(EqualizerButtonID, PSF_CLICKED):
		// Enter your code here:
		DisplayChildWindow(EqualizerButtonID);
		break;

	case SIGNAL(AudioLanguageButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(SubwooferButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(SmartVolumeButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(SmartVolumeButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		break;

	case PM_KEYLEFT:
	case PM_KEYRIGHT:
	
			break;

		case SubwooferButtonID:
			break;

		case AudioLanguageButtonID:

			break;

		case HeadPhoneButtonID:
		
			if(Mesg.wType == PM_KEYRIGHT)
			{
				DisplayChildWindow(HeadPhoneButtonID);
			}
			break;

		case EqualizerButtonID:
		
		break;    

	case PM_REFRESHWINDOW:

		break;

	default:
		return PegWindow::Message(Mesg);
	}
	return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void AdvAudioMenuClass::RefreshAdvAudioWindow(void)
{
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void AdvAudioMenuClass::DisplayChildWindow(int WindowID)
{
	HeadPhoneMenuClass *pHeadPhoneMenuWindow;
	EqualizerMenuClass *pEqualizerMenuWindow;
	
	switch(WindowID)
	{
	case HeadPhoneButtonID:
		SetBitmap(&gbAdvAudio1Bitmap);
		pHeadPhoneMenuWindow = new HeadPhoneMenuClass(300,215);
		Presentation()->Add(pHeadPhoneMenuWindow);
		break;
	
	case EqualizerButtonID:
		SetBitmap(&gbAdvAudio1Bitmap);
		pEqualizerMenuWindow = new EqualizerMenuClass(125,128);
		Presentation()->Add(pEqualizerMenuWindow);
		break;
	}
}

