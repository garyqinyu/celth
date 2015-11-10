/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: MainMenuClass
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

#include "peg.hpp"
#include "wbstring.hpp"
#include "mainmenu.hpp"
#include "pbitmwin.hpp"
#include "service.hpp"
#include "install.hpp"
#include "screen.hpp"
#include "main_aud.hpp" 
#include "languageset.hpp"
#include "userfct.hpp"

#undef __MAINMENUMASTER /* for mastership */
#define __MAINMENUMASTER /* for mastership */
#include "anim.hpp"

extern PegFont chinesefont ;

void PegSetColorTranslucency(PegPresentationManager *pPresent, unsigned char ucTranslucency)
{
		unsigned char ucLoop;
#define ColorStart 15 /* Include WHITE color */
#define ColorEnd   253

	/* modify values for only colors for 16 to 253 (0-15 are "Windows" color for foreground) and 254-255 are used for other purpose */
	/* ((STV3500Screen*)pPresent->Screen())->SetNColorsTranslucency(ColorStart, ColorEnd, ucTranslucency); */
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MainMenuClass::MainMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbMainBitmap, kAutoAdjustBitmapSize, 0, FF_NONE) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 183, iTop + 269);
    InitClient();
    SetColor(PCI_NORMAL, 223);
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 13, iTop + 226, iLeft + 142, iTop + 253);
    pHelpMenuButton = new TVDecoratedButton(ChildRect, LS(SID_HelpMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HelpMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pHelpMenuButton->SetColor(PCI_NORMAL, 223);
    pHelpMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pHelpMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 186, iLeft + 142, iTop + 213);
    pServiceMenuButton = new TVDecoratedButton(ChildRect, LS(SID_ServiceMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, ServiceMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pServiceMenuButton->SetColor(PCI_NORMAL, 223);
    pServiceMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pServiceMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 146, iLeft + 142, iTop + 173);
    pInstallMenuButton = new TVDecoratedButton(ChildRect, LS(SID_InstallMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, InstallMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pInstallMenuButton->SetColor(PCI_NORMAL, 223);
    pInstallMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pInstallMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 106, iLeft + 142, iTop + 133);
    pSoundMenuButton = new TVDecoratedButton(ChildRect, LS(SID_SoundMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, SoundMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pSoundMenuButton->SetColor(PCI_NORMAL, 223);
    pSoundMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pSoundMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 66, iLeft + 142, iTop + 93);
    pPictureMenuButton = new TVDecoratedButton(ChildRect, LS(SID_ScreenMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, PictureMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pPictureMenuButton->SetColor(PCI_NORMAL, 223);
    pPictureMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pPictureMenuButton);

    ChildRect.Set(iLeft + 20, iTop + 15, iLeft + 180, iTop + 44);
    pMainMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_MainMenuWindowTitle), MainMenuWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pMainMenuWindowTitle->SetColor(PCI_NORMAL, 231);
    pMainMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pMainMenuWindowTitle);

    /* WB End Construction */
		Id(MAIN_MENU_ID);

		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pMainMenuWindowTitle->SetFont(&chinesefont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pMainMenuWindowTitle->SetFont(&korean);
		}
		else /* default for English & French */
		{
	    pMainMenuWindowTitle->SetFont(&Pronto_16);
		}
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED MainMenuClass::Message(const PegMessage &Mesg)
{
	switch (Mesg.wType)
    {
    case SIGNAL(HelpMenuButtonID, PSF_CLICKED):
        // Enter your code here:
		DisplayChildWindow(HelpMenuButtonID);
        break;

    case SIGNAL(ServiceMenuButtonID, PSF_CLICKED):
        // Enter your code here:
		DisplayChildWindow(ServiceMenuButtonID);
        break;

    case SIGNAL(InstallMenuButtonID, PSF_CLICKED):
        // Enter your code here:
		DisplayChildWindow(InstallMenuButtonID);
        break;

    case SIGNAL(SoundMenuButtonID, PSF_CLICKED):
        // Enter your code here:
		DisplayChildWindow(SoundMenuButtonID);
		break;

    case SIGNAL(PictureMenuButtonID, PSF_CLICKED):
        // Enter your code here:
		DisplayChildWindow(PictureMenuButtonID);
        break;

    case PM_KEYRIGHT:/* right key => enter submenu */
		DisplayChildWindow(Mesg.iData);
		break;

	case PM_CLOSE:
		break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MainMenuClass::DisplayChildWindow(int WindowID)
{
	ServiceMenuClass *pServiceMenuWindow;
	InstallMenuClass *pInstallMenuWindow;
	ScreenMenuClass *pScreenMenuWindow;
	MainAudioMenuClass *pMainAudioMenuWindow;
	languagesetMenuClass *pHelpMenuWindow;
	
	switch(WindowID)
	{
	case HelpMenuButtonID:			
			pHelpMenuWindow = new languagesetMenuClass(250,150);
			Presentation()->Add(pHelpMenuWindow);
			break;

	case ServiceMenuButtonID:
			SetBitmap(&gbMain1Bitmap);
			pServiceMenuWindow = new ServiceMenuClass(250,150);
			Presentation()->Add(pServiceMenuWindow);
			break;

	case InstallMenuButtonID:
			SetBitmap(&gbMain1Bitmap);
			pInstallMenuWindow = new InstallMenuClass(250,150);
			Presentation()->Add(pInstallMenuWindow);
			break;

	case SoundMenuButtonID:
			SetBitmap(&gbMain1Bitmap);
			pMainAudioMenuWindow = new MainAudioMenuClass(235,150);
			Presentation()->Add(pMainAudioMenuWindow);
			break;

	case PictureMenuButtonID:
			SetBitmap(&gbMain1Bitmap);
			pScreenMenuWindow = new ScreenMenuClass(250,150);
			Presentation()->Add(pScreenMenuWindow);
			break;
	
		}
}





