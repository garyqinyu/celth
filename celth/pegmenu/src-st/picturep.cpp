/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: PicturePlusMenuClass
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
#include "screen.hpp"
#include "picturep.hpp"
#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "mainmenu.hpp"
#include "anim.hpp"
#include "userfct.hpp"
#include "init_val.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PicturePlusMenuClass::PicturePlusMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbPictpBitmap, kAutoAdjustBitmapSize, 0, FF_NONE) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 320, iTop + 375);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 205, iTop + 133, iLeft + 314, iTop + 157);
    pContrastEnhStatus = new PegPrompt(ChildRect, LS(SID_BWShrink), ContrastEnhStatusID);
    pContrastEnhStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pContrastEnhStatus);

    ChildRect.Set(iLeft + 205, iTop + 313, iLeft + 304, iTop + 337);
    pDigitalScanStatus = new PegPrompt(ChildRect, LS(SID_AUTO), DigitalScanStatusID);
    pDigitalScanStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pDigitalScanStatus);

	ChildRect.Set(iLeft + 205, iTop + 343, iLeft + 304, iTop + 367);
    pIfpLevelStatus = new PegPrompt(ChildRect, LS(SID_MED), IfpLevelStatusID);
    pIfpLevelStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pIfpLevelStatus);

    ChildRect.Set(iLeft + 205, iTop + 283, iLeft + 304, iTop + 307);
    pCTIStatus = new PegPrompt(ChildRect, LS(SID_OFF), CTIStatusID);
    pCTIStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pCTIStatus);

    ChildRect.Set(iLeft + 205, iTop + 253, iLeft + 304, iTop + 277);
    pNRStatus = new PegPrompt(ChildRect, LS(SID_AUTO), NRStatusID);
    pNRStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pNRStatus);

    ChildRect.Set(iLeft + 205, iTop + 193, iLeft + 304, iTop + 217);
    pChromaCoringStatus = new PegPrompt(ChildRect, LS(SID_OFF), ChromaCoringStatusID);
    pChromaCoringStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pChromaCoringStatus);

    ChildRect.Set(iLeft + 205, iTop + 163, iLeft + 304, iTop + 187);
    pCombFilterStatus = new PegPrompt(ChildRect, LS(SID_OFF), CombFilterStatusID);
    pCombFilterStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pCombFilterStatus);

    ChildRect.Set(iLeft + 205, iTop + 103, iLeft + 304, iTop + 127);
    pGreenBoostStatus = new PegPrompt(ChildRect, LS(SID_OFF), GreenBoostStatusID);
    pGreenBoostStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pGreenBoostStatus);

    ChildRect.Set(iLeft + 205, iTop + 73, iLeft + 304, iTop + 97);
    pAutoFleshStatus = new PegPrompt(ChildRect, LS(SID_OFF), AutoFleshStatusID);
    pAutoFleshStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pAutoFleshStatus);

    ChildRect.Set(iLeft + 205, iTop + 38, iLeft + 304, iTop + 62);
    pTintBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucTintMin, ucTintMax, 0);
    Add(pTintBar);

    ChildRect.Set(iLeft + 205, iTop + 223, iLeft + 304, iTop + 247);
    pRotationBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucRotationMin, ucRotationMax, 0);
    Add(pRotationBar);

    ChildRect.Set(iLeft + 11, iTop + 343, iLeft + 200, iTop + 367);
    pIfPLevelButton = new TVDecoratedButton(ChildRect, LS(SID_IFPLevelID), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, IfpLevelButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pIfPLevelButton->SetColor(PCI_NORMAL, 223);
    pIfPLevelButton->SetColor(PCI_NTEXT, BLUE);
    Add(pIfPLevelButton);

    ChildRect.Set(iLeft + 11, iTop + 313, iLeft + 200, iTop + 337);
    pDigitalScanButton = new TVDecoratedButton(ChildRect, LS(SID_DigitalScanButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, DigitalScanButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pDigitalScanButton->SetColor(PCI_NORMAL, 223);
    pDigitalScanButton->SetColor(PCI_NTEXT, BLUE);
    Add(pDigitalScanButton);

    ChildRect.Set(iLeft + 11, iTop + 283, iLeft + 200, iTop + 307);
    pCTIButton = new TVDecoratedButton(ChildRect, LS(SID_CTIButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, CTIButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pCTIButton->SetColor(PCI_NORMAL, 223);
    pCTIButton->SetColor(PCI_NTEXT, BLUE);
    Add(pCTIButton);

    ChildRect.Set(iLeft + 11, iTop + 253, iLeft + 200, iTop + 277);
    pNRButton = new TVDecoratedButton(ChildRect, LS(SID_NRButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, NRButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pNRButton->SetColor(PCI_NORMAL, 223);
    pNRButton->SetColor(PCI_NTEXT, BLUE);
    Add(pNRButton);

    ChildRect.Set(iLeft + 11, iTop + 223, iLeft + 200, iTop + 247);
    pRotationButton = new TVDecoratedButton(ChildRect, LS(SID_RotationButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, RotationButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pRotationButton->SetColor(PCI_NORMAL, 223);
    pRotationButton->SetColor(PCI_NTEXT, BLUE);
    Add(pRotationButton);

    ChildRect.Set(iLeft + 11, iTop + 193, iLeft + 200, iTop + 217);
    pChromaCoringButton = new TVDecoratedButton(ChildRect, LS(SID_ChromaCoringButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, ChromaCoringButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pChromaCoringButton->SetColor(PCI_NORMAL, 223);
    pChromaCoringButton->SetColor(PCI_NTEXT, BLUE);
    Add(pChromaCoringButton);

    ChildRect.Set(iLeft + 11, iTop + 163, iLeft + 200, iTop + 187);
    pCombFilterButton = new TVDecoratedButton(ChildRect, LS(SID_CombFilterButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, CombFilterButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pCombFilterButton->SetColor(PCI_NORMAL, 223);
    pCombFilterButton->SetColor(PCI_NTEXT, BLUE);
    Add(pCombFilterButton);

    ChildRect.Set(iLeft + 11, iTop + 133, iLeft + 200, iTop + 157);
    pContrastEnhButton = new TVDecoratedButton(ChildRect, LS(SID_ContrastEnhButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, ContrastEnhButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pContrastEnhButton->SetColor(PCI_NORMAL, 223);
    pContrastEnhButton->SetColor(PCI_NTEXT, BLUE);
    Add(pContrastEnhButton);

    ChildRect.Set(iLeft + 11, iTop + 103, iLeft + 200, iTop + 127);
    pGreenBoostButton = new TVDecoratedButton(ChildRect, LS(SID_GreenBoostButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, GreenBoostButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pGreenBoostButton->SetColor(PCI_NORMAL, 223);
    pGreenBoostButton->SetColor(PCI_NTEXT, BLUE);
    Add(pGreenBoostButton);

    ChildRect.Set(iLeft + 11, iTop + 73, iLeft + 200, iTop + 97);
    pAutoFleshButton = new TVDecoratedButton(ChildRect, LS(SID_AutoFleshButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, AutoFleshButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pAutoFleshButton ->SetColor(PCI_NORMAL, 223);
    pAutoFleshButton ->SetColor(PCI_NTEXT, BLUE);
    Add(pAutoFleshButton );

    ChildRect.Set(iLeft + 11, iTop + 43, iLeft + 200, iTop + 67);
    pTintButton = new TVDecoratedButton(ChildRect, LS(SID_TintButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, TintButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pTintButton->SetColor(PCI_NORMAL, 223);
    pTintButton->SetColor(PCI_NTEXT, BLUE);
    Add(pTintButton);

    ChildRect.Set(iLeft + 20, iTop + 4, iLeft + 159, iTop + 33);
    pPicturePlusMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_PicturePlusMenuButton), PicturePlusWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pPicturePlusMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pPicturePlusMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pPicturePlusMenuWindowTitle);

    /* WB End Construction */
		Id(PICTURE_PLUS_MENU_ID);

		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pPicturePlusMenuWindowTitle->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pPicturePlusMenuWindowTitle->SetFont(&korean);
		}
		else /* default for English & French */
		{
	    pPicturePlusMenuWindowTitle->SetFont(&Pronto_16);
		}

		RefreshPicturePlusWindow(); /* Initialize Picture plus window */
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PicturePlusMenuClass::Message(const PegMessage &Mesg)
{
		ScreenMenuClass *pPictureWindow;
		BOOL bValueChanged = FALSE;

		switch (Mesg.wType)
    {
		case PM_KEYLEFT:
		case PM_KEYRIGHT:
				switch(Mesg.iData)
				{
					case TintButtonID:
						/* use uPictureValueToUpdate temporary union in case we want to display
						   a small bar alone for tint tuning (see Picture->Contrast, Brightness Sharpness or Color, Video or Geometry menu)*/
						if (Mesg.wType == PM_KEYLEFT)
						{
							if (pTintBar->Value() > pTintBar->GetMinValue())
							{
								pTintBar->Update(pTintBar->Value()-1,TRUE);
								bValueChanged = TRUE;
							}
						}
						else if (Mesg.wType == PM_KEYRIGHT)
						{
							if (pTintBar->Value() < pTintBar->GetMaxValue())
							{
								pTintBar->Update(pTintBar->Value()+1,TRUE);
								bValueChanged = TRUE;
							}
						}
						if (bValueChanged)
						{
							bValueChanged = FALSE;
						}
						break;

					case RotationButtonID:
						/* use uPictureValueToUpdate temporary union in case we want to display
						   a small bar alone for tint tuning (see Picture->Contrast, Brightness Sharpness or Color, Video or Geometry menu)*/
						if (Mesg.wType == PM_KEYLEFT)
						{
							if (pRotationBar->Value() > pRotationBar->GetMinValue())
							{
								pRotationBar->Update(pRotationBar->Value()-1,TRUE);
								bValueChanged = TRUE;
							}
						}
						else if (Mesg.wType == PM_KEYRIGHT)
						{
							if (pRotationBar->Value() < pRotationBar->GetMaxValue())
							{
								pRotationBar->Update(pRotationBar->Value()+1,TRUE);
								bValueChanged = TRUE;
							}
						}
						if (bValueChanged)
						{
							bValueChanged = FALSE;
						}
						break;

					case NRButtonID:
						break;

					case DigitalScanButtonID:
						break;

					case CTIButtonID:
						break;

					case GreenBoostButtonID:
						break;

					case ChromaCoringButtonID:
						break;

					case CombFilterButtonID:
						break;

					case ContrastEnhButtonID:
						break;

					case AutoFleshButtonID:
						break;
					case IfpLevelButtonID:
						break;
				}
				
				break;

		case PM_REFRESHWINDOW:
				RefreshPicturePlusWindow();
				break;
    
		case PM_CLOSE:
			  /* left key = close = go back to previous menu */
				Destroy(this);
				pPictureWindow = (ScreenMenuClass*) Presentation()->Find(PICTURE_MENU_ID);
				pPictureWindow->SetBitmap(&gbPictBitmap);
				break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}


void PicturePlusMenuClass::RefreshPicturePlusWindow(void)
{
}
