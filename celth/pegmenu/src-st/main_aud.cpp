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


#include "peg.hpp"
#include "wbstring.hpp"
#include "wbstrnga.hpp"
#include "main_aud.hpp"
#include "mainmenu.hpp"
#include "levlbars.hpp"
#include "advaudio.hpp"
#include "anim.hpp"
#include "init_val.h"

/*
extern "C"
{
	ST_ErrorCode_t DownMixAudio(BOOL whichstyle);
}

*/
extern UCHAR gbCurrentLanguageA;
extern PegFont chinesefont;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MainAudioMenuClass::MainAudioMenuClass(SIGNED iLeft, SIGNED iTop) : 
  PegBitmapWindow(&gbMainBitmap, kAutoAdjustBitmapSize, 220, FF_NONE)
{
	PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 183, iTop + 269);

	InitClient();
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

	if (gbCurrentLanguage == 1) /* language = chinese */
	{
		gbCurrentLanguageA =1;
	}
	else if (gbCurrentLanguage == 0)
	{
		gbCurrentLanguageA =0;
	}

	ChildRect.Set(iLeft + 13, iTop + 85, iLeft + 172, iTop + 112);
	pVolumeButton = new TVDecoratedButton(ChildRect, LSA(SID_VolumeButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, VolumeButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pVolumeButton->SetColor(PCI_NORMAL, 223);
	pVolumeButton->SetColor(PCI_NTEXT, BLUE);
	Add(pVolumeButton);

	ChildRect.Set(iLeft + 13, iTop + 50, iLeft + 172, iTop + 77);
	pAudioTypeButton = new TVDecoratedButton(ChildRect, LSA(SID_AudioTypeButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, AudioTypeButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pAudioTypeButton->SetColor(PCI_NORMAL, 223);
	pAudioTypeButton->SetColor(PCI_NTEXT, BLUE);
	Add(pAudioTypeButton);
/*
	ChildRect.Set(iLeft + 17, iTop + 5, iLeft + 316, iTop + 44);
	pAudioMenuTitle = new PegPrompt(ChildRect, LSA(SID_MainAudioMenuWindowTitle), AudioMenuTitleID);
	pAudioMenuTitle->SetColor(PCI_NORMAL, BLUE);
	pAudioMenuTitle->SetColor(PCI_NTEXT, BLUE);
	Add(pAudioMenuTitle); */

	/* WB End Construction */

	Id(MAIN_AUDIO_MENU_ID);

	/* Set font for the title, buttons, prompts and numeric values */
	
	/*pAudioMenuTitle->SetFont(&Pronto_16);*/
#if 0
	pAdvancedAudioButton->SetFont(&Pronto_12);
	pSpLevelButton->SetFont(&Pronto_12);
	pSpEffectButton->SetFont(&Pronto_12);
	pLoudnessButton->SetFont(&Pronto_12);
	pBalanceButton->SetFont(&Pronto_12);
#endif

	if (gbCurrentLanguage == 1) /* language = chinese */
	{
		pAudioTypeButton->SetFont(&chinesefont);
	}
	else if (gbCurrentLanguage == 0)
	{
		pAudioTypeButton->SetFont(&Pronto_12);
	}
	
	pVolumeButton->SetFont(&Pronto_12);

#if 0
	pAudioTypeStatus->SetFont(&Pronto_12);
	pLoudnessStatus->SetFont(&Pronto_12);
	pSpEffectStatus->SetFont(&Pronto_12);

	pVolumeBar->SetFont(&Pronto_12);
#endif
	RefreshMainAudioWindow(); /* Refresh the parameters with values from sound structure */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED MainAudioMenuClass::Message(const PegMessage &Mesg)
{
	PegThing				*SomeThing;
	MainMenuClass		*pMainWindow;
	
	switch (Mesg.wType)
	{
	case SIGNAL(VolumeButtonID, PSF_CLICKED):
		// Enter your code here:
		//ac3
		/*DownMixAudio(FALSE);*/
		break;

	case SIGNAL(AudioTypeButtonID, PSF_CLICKED):
		// Enter your code here:
		//stereo
	/*	DownMixAudio(TRUE);*/
		break;

	case PM_KEYLEFT:
	case PM_KEYRIGHT:
		
		switch(Mesg.iData)
		{
		case AudioTypeButtonID:

			break;

		case VolumeButtonID:

			/* check whether half mute icon is on display */
			SomeThing = PegThing::Presentation()->Find(HALF_MUTE_ID);

			if(SomeThing)
			{ /* kill half mute */
				PegThing::Presentation()->Destroy(SomeThing);
			}
			else
			{ /* check whether full mute icon is on display */
				SomeThing = PegThing::Presentation()->Find(FULL_MUTE_ID);
			
				if(SomeThing)
				{ /* kill full mute */
					PegThing::Presentation()->Destroy(SomeThing);
				}
			}

			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pVolumeBar->Value() < pVolumeBar->GetMaxValue())
				{
					pVolumeBar->Update(pVolumeBar->Value()+1,TRUE);
				}
			}
			else /* PM_KEYLEFT */
			{
				if (pVolumeBar->Value() > pVolumeBar->GetMinValue())
				{
					pVolumeBar->Update(pVolumeBar->Value()-1,TRUE);
				}
			}

			break;

		case BalanceButtonID:

			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pBalanceBar->GetCurrentValue() < pBalanceBar->GetMaxValue())
				{
					pBalanceBar->SetCurrentValue(pBalanceBar->GetCurrentValue()+1, TRUE);
				}
			}
			else /* PM_KEYLEFT */
			{
				if (pBalanceBar->GetCurrentValue() > pBalanceBar->GetMinValue())
				{
					pBalanceBar->SetCurrentValue(pBalanceBar->GetCurrentValue()-1, TRUE);
				}
			}

			break;

		case LoudnessButtonID:

			break;

		case SpEffectButtonID:

			break;

		case SpLevelButtonID:

			if(Mesg.wType == PM_KEYRIGHT)
			{
				DisplayChildWindow(SpLevelButtonID);
			}
			break;

		case AdvancedAudioButtonID:

			if(Mesg.wType == PM_KEYRIGHT)
			{
				DisplayChildWindow(AdvancedAudioButtonID);
			}
			break;
		}
		break;

	case PM_REFRESHWINDOW:
		
		break;

	case PM_CLOSE:
		
		Destroy(this);
		pMainWindow = (MainMenuClass*) Presentation()->Find(MAIN_MENU_ID);
		pMainWindow->SetBitmap(&gbMainBitmap);
		break;

	default:
		return PegWindow::Message(Mesg);
	}
	return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MainAudioMenuClass::RefreshMainAudioWindow(void)
{
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MainAudioMenuClass::DisplayChildWindow(int WindowID)
{
	AdvAudioMenuClass *pAdvAudioMenuWindow;
	STWideGainBarClass *pSTGainWindow;
	SRS3DBarsClass *pSRS3DWindow;

	switch(WindowID)
	{
	case AdvancedAudioButtonID:
		
		SetBitmap(&gbMainAudio1Bitmap);
		pAdvAudioMenuWindow = new AdvAudioMenuClass(265,200);
		Presentation()->Add(pAdvAudioMenuWindow);
		break;
	
	case SpLevelButtonID:
		break;
	}
}


