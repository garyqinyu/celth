/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: InstallMenuClass
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
#include "install.hpp"
#include "mainmenu.hpp"
#include "language.hpp"
#include "anim.hpp"
#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "inout.hpp"
#include "tuning.hpp"

extern "C"
{
	BOOL OutPutRateSetup(int iIndex);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
InstallMenuClass::InstallMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbMainBitmap, kAutoAdjustBitmapSize, 0, FF_NONE) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 183, iTop + 269);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 13, iTop + 100, iLeft + 162, iTop + 127);
    pAutosearchMenuButton = new TVDecoratedButton(ChildRect, LS(SID_1080i50hzButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, AutosearchMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pAutosearchMenuButton->SetColor(PCI_NORMAL, 223);
    pAutosearchMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pAutosearchMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 60, iLeft + 172, iTop + 87);
    pManualTuningMenuButton = new TVDecoratedButton(ChildRect, LS(SID_ManualTuningMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, ManualTuningMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pManualTuningMenuButton->SetColor(PCI_NORMAL, 223);
    pManualTuningMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pManualTuningMenuButton);
/*
    ChildRect.Set(iLeft + 16, iTop + 12, iLeft + 145, iTop + 41);
    pInstallMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_InstallMenuButton), InstallMenuWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pInstallMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pInstallMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pInstallMenuWindowTitle); */

    /* WB End Construction */
		Id(INSTALL_MENU_ID);

	pAutosearchMenuButton->SetFont(&Pronto_16);
	pManualTuningMenuButton->SetFont(&Pronto_16);

#if 0
		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pInstallMenuWindowTitle->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pInstallMenuWindowTitle->SetFont(&korean);
		}
		else /* default for English & French */
		{
	    pInstallMenuWindowTitle->SetFont(&Pronto_16);
		}
#endif
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED InstallMenuClass::Message(const PegMessage &Mesg)
{
		MainMenuClass *pMainWindow;

    switch (Mesg.wType)
    {

    case SIGNAL(ManualTuningMenuButtonID, PSF_CLICKED):
        // Enter your code here:
				OutPutRateSetup(1);
        break;

    case SIGNAL(AutosearchMenuButtonID, PSF_CLICKED):
        // Enter your code here:
				OutPutRateSetup(0);
        break;

		case PM_KEYRIGHT:/* right key => enter submenu */
				DisplayChildWindow(Mesg.iData);
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
void InstallMenuClass::DisplayChildWindow(int WindowID)
{
		LanguageMenuClass *pLanguageWindow;
		InputOutputClass *pIOWindow;
		TuningMenuClass *pTuningWindow;

		switch(WindowID)
		{

		case ManualTuningMenuButtonID:
				SetBitmap(&gbMain1Bitmap);
				pTuningWindow = new TuningMenuClass(180,100);
				Presentation()->Add(pTuningWindow);
				break;

		case AutosearchMenuButtonID:
				break;

		case OrganizeMenuButtonID:
				break;

		}
}




