/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: PictureMenuClass
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
#include "languageset.hpp"
#include "picturep.hpp"
#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "tvprobar.hpp"
#include "anim.hpp"
#include "mainmenu.hpp"
#include "usertype.hpp"
#include "init_val.h"

extern PegFont chinesefont ;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
languagesetMenuClass::languagesetMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbMainBitmap, kAutoAdjustBitmapSize, 0, FF_NONE) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 183, iTop + 269);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

/*    ChildRect.Set(iLeft + 13, iTop + 106, iLeft + 152, iTop + 133);*/
    ChildRect.Set(iLeft + 33, iTop + 106, iLeft + 152, iTop + 133);
    pContrastButton = new TVDecoratedButton(ChildRect, LS(SID_LanguageChineseButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, ContrastButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pContrastButton->SetColor(PCI_NORMAL, 223);
    pContrastButton->SetColor(PCI_NTEXT, BLUE);
    Add(pContrastButton);

/*    ChildRect.Set(iLeft + 13, iTop + 66, iLeft + 152, iTop + 93);*/
    ChildRect.Set(iLeft + 33, iTop + 66, iLeft + 152, iTop + 93);
    pBrightnessButton = new TVDecoratedButton(ChildRect, LS(SID_LanguageEnglishButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, BrightnessButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pBrightnessButton->SetColor(PCI_NORMAL, 223);
    pBrightnessButton->SetColor(PCI_NTEXT, BLUE);
    Add(pBrightnessButton);
/*
    ChildRect.Set(iLeft + 20, iTop + 15, iLeft + 160, iTop + 44);
    pPictureMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_LanguageSetMenuWindowTitle), PictureWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pPictureMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pPictureMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pPictureMenuWindowTitle); */

    /* WB End Construction */
		Id(PICTURE_MENU_ID);

		RefreshPictureWindow(); /* initialize window */
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED languagesetMenuClass::Message(const PegMessage &Mesg)
{
		MainMenuClass *pMainWindow;
		PegThing *ptDisplayedWindow[MaxNumberOfWindow];
		PegThing *SomeThing = NULL;
		PegThing *pTest;
		int iIndex;
		PegMessage TmpMesg;
		UCHAR uLoop;


		switch (Mesg.wType)
    {
    case SIGNAL(ContrastButtonID, PSF_CLICKED):
        /* send message to this window to simulate RIGHT KEY pressed (same function) */
        			if (  gbCurrentLanguage == 1)
        				break;
        				gbCurrentLanguage = 1;
        
				PegTextThing::SetDefaultFont(PEG_TBUTTON_FONT, &chinesefont);
				PegTextThing::SetDefaultFont(PEG_PROMPT_FONT, &chinesefont);  
				
				DrawScreenNewLanguage();
        
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = ContrastButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(BrightnessButtonID, PSF_CLICKED):
        /* send message to this window to simulate RIGHT KEY pressed (same function) */
                			if (  gbCurrentLanguage == 0)
        				break;

        			gbCurrentLanguage = 0;
        
				PegTextThing::SetDefaultFont(PEG_TBUTTON_FONT, &Pronto_12);
				PegTextThing::SetDefaultFont(PEG_PROMPT_FONT, &Pronto_12);  
				
				DrawScreenNewLanguage();      
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = BrightnessButtonID;
				this->Message(TmpMesg);
        break;

    case PM_UPDATEVALUE:
	break;
    
     case PM_KEYLEFT:
     case PM_KEYRIGHT:
	break;
			
		case PM_REFRESHWINDOW:
			RefreshPictureWindow();
			break;

		case PM_CLOSE:
			  /* left key = close = go back to previous menu */
				Destroy(this);
				pMainWindow = (MainMenuClass*) Presentation()->Find(MAIN_MENU_ID);
				pMainWindow->SetBitmap(&gbMainBitmap);
				break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

/*---------------------------------------------------*/
void languagesetMenuClass::DisplayChildWindow(int WindowID)
{
		PicturePlusMenuClass *pPicturePlusMenuWindow;

		switch(WindowID)
		{
		case PicturePlusMenuButtonID:
				SetBitmap(&gbPict1Bitmap);
				pPicturePlusMenuWindow = new PicturePlusMenuClass(100,70);
				Presentation()->Add(pPicturePlusMenuWindow);
				break;

		}
}

void languagesetMenuClass::RefreshPictureWindow(void)
{
}

void languagesetMenuClass::DrawScreenNewLanguage(void)
{
	PegThing *pTest;
	languagesetMenuClass *pHelpMenuWindow;
	MainMenuClass *pMainMenu;

	BeginDraw();

	pTest = PegPresentationManager::Presentation()->Find(MAIN_MENU_ID);
	PegPresentationManager::Presentation()->Destroy(pTest);

	pTest = PegPresentationManager::Presentation()->Find(PICTURE_MENU_ID);
	PegPresentationManager::Presentation()->Destroy(pTest);

	/* Reconstruct with new language */
	pMainMenu = new MainMenuClass(200, 100);
	Presentation()->Add(pMainMenu); /* add Main Menu */
	
	pHelpMenuWindow = new languagesetMenuClass(250,150); /* Add Install Menu */
	Presentation()->Add(pHelpMenuWindow);

	EndDraw();
}
