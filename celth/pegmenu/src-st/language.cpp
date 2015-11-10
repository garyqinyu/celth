/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: LanguageMenuClass
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
#include "language.hpp"
#include "install.hpp"
#include "anim.hpp"
#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "mainmenu.hpp"
#include "init_val.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LanguageMenuClass::LanguageMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbLangBitmap, kAutoAdjustBitmapSize, 0, FF_NONE) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 169, iTop + 230);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 13, iTop + 186, iLeft + 112, iTop + 213);
    pChineseMenuButton = new TVDecoratedButton(ChildRect, LS(SID_ChineseMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, ChineseMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pChineseMenuButton->SetColor(PCI_NORMAL, 223);
    pChineseMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pChineseMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 146, iLeft + 112, iTop + 173);
    pKoreanMenuButton = new TVDecoratedButton(ChildRect, LS(SID_KoreanMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, KoreanMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pKoreanMenuButton->SetColor(PCI_NORMAL, 223);
    pKoreanMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pKoreanMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 106, iLeft + 112, iTop + 133);
    pFrenchMenuButton = new TVDecoratedButton(ChildRect, LS(SID_FrenchMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, FrenchMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pFrenchMenuButton->SetColor(PCI_NORMAL, 223);
    pFrenchMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pFrenchMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 66, iLeft + 112, iTop + 93);
    pEnglishMenuButton = new TVDecoratedButton(ChildRect, LS(SID_EnglishMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, EnglishMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pEnglishMenuButton->SetColor(PCI_NORMAL, 223);
    pEnglishMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pEnglishMenuButton);

    ChildRect.Set(iLeft + 20, iTop + 15, iLeft + 160, iTop + 44);
    pLanguageMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_LanguageMenuWindowTitle), LanguageMenuWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pLanguageMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pLanguageMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pLanguageMenuWindowTitle);

    /* WB End Construction */
		Id(LANGUAGE_MENU_ID);

		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pLanguageMenuWindowTitle->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pLanguageMenuWindowTitle->SetFont(&korean);
		}
		else /* default for English & French */
		{
	    pLanguageMenuWindowTitle->SetFont(&Pronto_16);
		}

		pEnglishMenuButton->SetFont(&Pronto_12);
		pFrenchMenuButton->SetFont(&Pronto_12);
		pKoreanMenuButton->SetFont(&korean);
		pChineseMenuButton->SetFont(&A12RFont);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED LanguageMenuClass::Message(const PegMessage &Mesg)
{
		InstallMenuClass *pInstallWindow;
		UCHAR uLoop;

    switch (Mesg.wType)
    {
    case SIGNAL(ChineseMenuButtonID, PSF_CLICKED):
        // Enter your code here:
				gbCurrentLanguage = 1;
	    	for (uLoop = 0; uLoop < PEG_NUMBER_OF_DEFAULT_FONTS; uLoop++)
				{
					PegTextThing::SetDefaultFont(uLoop, &A12RFont);
				}
				DrawScreenNewLanguage();
        break;

    case SIGNAL(KoreanMenuButtonID, PSF_CLICKED):
        // Enter your code here:
				gbCurrentLanguage = 2;
	    	for (uLoop = 0; uLoop < PEG_NUMBER_OF_DEFAULT_FONTS; uLoop++)
				{
					PegTextThing::SetDefaultFont(uLoop, &korean);
				}
				DrawScreenNewLanguage();
        break;

    case SIGNAL(FrenchMenuButtonID, PSF_CLICKED):
        // Enter your code here:
				gbCurrentLanguage = 3;
	    	for (uLoop = 0; uLoop < PEG_NUMBER_OF_DEFAULT_FONTS; uLoop++)
				{
					PegTextThing::SetDefaultFont(uLoop, &Pronto_12);
				}
				DrawScreenNewLanguage();
        break;

    case SIGNAL(EnglishMenuButtonID, PSF_CLICKED):
        // Enter your code here:
				gbCurrentLanguage = 0;
	    	for (uLoop = 0; uLoop < PEG_NUMBER_OF_DEFAULT_FONTS; uLoop++)
				{
					PegTextThing::SetDefaultFont(uLoop, &Pronto_12);
				}
				DrawScreenNewLanguage();
				
        break;

		case PM_CLOSE:
				Destroy(this);
				pInstallWindow = (InstallMenuClass*) Presentation()->Find(INSTALL_MENU_ID);
				pInstallWindow->SetBitmap(&gbMainBitmap);
				break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

void LanguageMenuClass::DrawScreenNewLanguage(void)
{
	PegThing *pTest;
	InstallMenuClass *pInstallMenuWindow;
	MainMenuClass *pMainMenu;

	BeginDraw();

	pTest = PegPresentationManager::Presentation()->Find(MAIN_MENU_ID);
	PegPresentationManager::Presentation()->Destroy(pTest);

	pTest = PegPresentationManager::Presentation()->Find(INSTALL_MENU_ID);
	PegPresentationManager::Presentation()->Destroy(pTest);

	/* Reconstruct with new language */
	pMainMenu = new MainMenuClass(200, 100);
	Presentation()->Add(pMainMenu); /* add Main Menu */
	pMainMenu->SetBitmap(&gbMain1Bitmap);

	pInstallMenuWindow = new InstallMenuClass(250,150); /* Add Install Menu */
	Presentation()->Add(pInstallMenuWindow);

	pTest = PegPresentationManager::Presentation()->Find(LANGUAGE_MENU_ID); /* remove old Language menu */
	PegPresentationManager::Presentation()->Destroy(pTest);

	EndDraw();
}


