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
#include "picture.hpp"
#include "picturep.hpp"
#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "tvprobar.hpp"
#include "anim.hpp"
#include "mainmenu.hpp"
#include "usertype.hpp"
#include "init_val.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PictureMenuClass::PictureMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbPictBitmap, kAutoAdjustBitmapSize, 0, FF_NONE) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 275, iTop + 260);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 165, iTop + 186, iLeft + 264, iTop + 213);
    pSharpnessBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucSharpnessMin, ucSharpnessMax, 0);
    Add(pSharpnessBar);

    ChildRect.Set(iLeft + 165, iTop + 146, iLeft + 264, iTop + 173);
    pColorBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucColorMin, ucColorMax, 0);
    Add(pColorBar);

    ChildRect.Set(iLeft + 165, iTop + 106, iLeft + 264, iTop + 133);
    pContrastBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucContrastMin, ucContrastMax, 0);
    Add(pContrastBar);

    ChildRect.Set(iLeft + 165, iTop + 66, iLeft + 264, iTop + 93);
    pBrightnessBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucBrightnessMin, ucBrightnessMax, 0);
    Add(pBrightnessBar);

    ChildRect.Set(iLeft + 13, iTop + 226, iLeft + 152, iTop + 243);
    pPicturePlusMenuButton = new TVDecoratedButton(ChildRect, LS(SID_PicturePlusMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, PicturePlusMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pPicturePlusMenuButton->SetColor(PCI_NORMAL, 223);
    pPicturePlusMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pPicturePlusMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 186, iLeft + 152, iTop + 213);
    pSharpnessButton = new TVDecoratedButton(ChildRect, LS(SID_SharpnessButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, SharpnessButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pSharpnessButton->SetColor(PCI_NORMAL, 223);
    pSharpnessButton->SetColor(PCI_NTEXT, BLUE);
    Add(pSharpnessButton);

    ChildRect.Set(iLeft + 13, iTop + 146, iLeft + 152, iTop + 173);
    pColorButton = new TVDecoratedButton(ChildRect, LS(SID_ColorButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, ColorButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pColorButton->SetColor(PCI_NORMAL, 223);
    pColorButton->SetColor(PCI_NTEXT, BLUE);
    Add(pColorButton);

    ChildRect.Set(iLeft + 13, iTop + 106, iLeft + 152, iTop + 133);
    pContrastButton = new TVDecoratedButton(ChildRect, LS(SID_ContrastButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, ContrastButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pContrastButton->SetColor(PCI_NORMAL, 223);
    pContrastButton->SetColor(PCI_NTEXT, BLUE);
    Add(pContrastButton);

    ChildRect.Set(iLeft + 13, iTop + 66, iLeft + 152, iTop + 93);
    pBrightnessButton = new TVDecoratedButton(ChildRect, LS(SID_BrightnessButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, BrightnessButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pBrightnessButton->SetColor(PCI_NORMAL, 223);
    pBrightnessButton->SetColor(PCI_NTEXT, BLUE);
    Add(pBrightnessButton);

    ChildRect.Set(iLeft + 20, iTop + 15, iLeft + 160, iTop + 44);
    pPictureMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_PictureMenuWindowTitle), PictureWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pPictureMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pPictureMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pPictureMenuWindowTitle);

    /* WB End Construction */
		Id(PICTURE_MENU_ID);

		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pPictureMenuWindowTitle->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pPictureMenuWindowTitle->SetFont(&korean);
		}
		else /* default for English & French */
		{
	    pPictureMenuWindowTitle->SetFont(&Pronto_16);
		}

		pBrightnessBar->SetColor(PCI_NORMAL, DARKGRAY);
		pBrightnessBar->SetColor(PCI_SELECTED, BLUE);
		pBrightnessBar->SetColor(PCI_NTEXT, LIGHTGRAY);
		pBrightnessBar->SetColor(PCI_STEXT, LIGHTGRAY);
		pContrastBar->SetColor(PCI_NORMAL, DARKGRAY);
		pContrastBar->SetColor(PCI_SELECTED, BLUE);
		pContrastBar->SetColor(PCI_NTEXT, LIGHTGRAY);
		pContrastBar->SetColor(PCI_STEXT, LIGHTGRAY);
		pColorBar->SetColor(PCI_NORMAL, DARKGRAY);
		pColorBar->SetColor(PCI_SELECTED, BLUE);
		pColorBar->SetColor(PCI_NTEXT, LIGHTGRAY);
		pColorBar->SetColor(PCI_STEXT, LIGHTGRAY);
		pSharpnessBar->SetColor(PCI_NORMAL, DARKGRAY);
		pSharpnessBar->SetColor(PCI_SELECTED, BLUE);
		pSharpnessBar->SetColor(PCI_NTEXT, LIGHTGRAY);
		pSharpnessBar->SetColor(PCI_STEXT, LIGHTGRAY);

		RefreshPictureWindow(); /* initialize window */
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PictureMenuClass::Message(const PegMessage &Mesg)
{
		MainMenuClass *pMainWindow;
		PegThing *ptDisplayedWindow[MaxNumberOfWindow];
		PegThing *SomeThing = NULL;
		PegThing *pTest;
		int iIndex;
		PegMessage TmpMesg;

		switch (Mesg.wType)
    {
    case SIGNAL(PicturePlusMenuButtonID, PSF_CLICKED):
        // Enter your code here:
				DisplayChildWindow(PicturePlusMenuButtonID);
				break;

    case SIGNAL(SharpnessButtonID, PSF_CLICKED):
        /* send message to this window to simulate RIGHT KEY pressed (same function) */
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = SharpnessButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(ColorButtonID, PSF_CLICKED):
        /* send message to this window to simulate RIGHT KEY pressed (same function) */
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = ColorButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(ContrastButtonID, PSF_CLICKED):
        /* send message to this window to simulate RIGHT KEY pressed (same function) */
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = ContrastButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(BrightnessButtonID, PSF_CLICKED):
        /* send message to this window to simulate RIGHT KEY pressed (same function) */
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = BrightnessButtonID;
				this->Message(TmpMesg);
        break;

		case PM_UPDATEVALUE:
				/* the scrollbar value needs to be updated */
				if (Mesg.pSource == pBrightnessButton)
				{
					pBrightnessBar->Update(Mesg.iData, TRUE);
				}
				else if (Mesg.pSource == pContrastButton)
				{
					pContrastBar->Update(Mesg.iData, TRUE);
				}
				else if (Mesg.pSource == pColorButton)
				{
					pColorBar->Update(Mesg.iData, TRUE);
				}
				else if (Mesg.pSource == pSharpnessButton)
				{
					pSharpnessBar->Update(Mesg.iData, TRUE);
				}
				break;
    
		case PM_KEYLEFT:
		case PM_KEYRIGHT:
				if (Mesg.iData != PicturePlusMenuButtonID)
				{
					/* right key equals OK, so enter next menu level */
					BeginDraw();
					/* Delete all pop up already displayed to clean the screen */
					for(iIndex = FIRST_POPUP_ID;iIndex<LAST_USER_ID;iIndex++)
					{
						SomeThing = PegThing::Presentation()->Find(iIndex);
						if(SomeThing!=NULL && iIndex!=HALF_MUTE_ID && iIndex!=FULL_MUTE_ID)
						{
							PegThing::Presentation()->Destroy(SomeThing);
						}
					}
					pTest = PegPresentationManager::Presentation()->First();
					iIndex = 0;
					while(pTest) /* construct the current displayed window pointer table */
					{
							ptDisplayedWindow[iIndex++] = pTest;
							PegPresentationManager::Presentation()->Remove(pTest);
							pTest = PegPresentationManager::Presentation()->First();
					}
					ptDisplayedWindow[iIndex] = NULL; /* reset last pointer */

					switch(Mesg.iData)
					{
					case BrightnessButtonID:
							break;

					case ContrastButtonID:
							break;

					case ColorButtonID:
							break;

					case SharpnessButtonID:
							break;
					}
					EndDraw();
				}
				else
				{ /* Picture plus button click */
					if (Mesg.wType != PM_KEYLEFT) /* Left key has no action on the Picture+ button */
						DisplayChildWindow(PicturePlusMenuButtonID);
				}
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
void PictureMenuClass::DisplayChildWindow(int WindowID)
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

void PictureMenuClass::RefreshPictureWindow(void)
{
}


