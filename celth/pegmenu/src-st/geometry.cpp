/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: GeometryMenuClass
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
#include "osal.h"
#include "peg.hpp"

#include "wbstring.hpp"
#include "geometry.hpp"
#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "service.hpp"
#include "anim.hpp"
#include "userfct.hpp"
#include "tvprobar.hpp"
#include "init_val.h"
#include "osal.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
GeometryMenuClass::GeometryMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbPictBitmap, kAutoAdjustBitmapSize, PegBlueTransparent, FF_RAISED) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 316, iTop + 414);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);
#if 0
    ChildRect.Set(iLeft + 225, iTop + 353, iLeft + 304, iTop + 377);
    pEWEHTStatus = new PegPrompt(ChildRect, LS(SID_ZERO), EWEHTStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pEWEHTStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pEWEHTStatus);

    ChildRect.Set(iLeft + 225, iTop + 328, iLeft + 304, iTop + 352);
    pHBowStatus = new PegPrompt(ChildRect, LS(SID_ZERO), HBowStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pHBowStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pHBowStatus);

    ChildRect.Set(iLeft + 225, iTop + 303, iLeft + 304, iTop + 327);
    pHParallelStatus = new PegPrompt(ChildRect, LS(SID_ZERO), HParallelStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pHParallelStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pHParallelStatus);

    ChildRect.Set(iLeft + 225, iTop + 278, iLeft + 304, iTop + 302);
    pEWLowerStatus = new PegPrompt(ChildRect, LS(SID_ZERO), EWLowerStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pEWLowerStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pEWLowerStatus);

    ChildRect.Set(iLeft + 225, iTop + 253, iLeft + 304, iTop + 277);
    pEWUpperStatus = new PegPrompt(ChildRect, LS(SID_ZERO), EWUpperStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pEWUpperStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pEWUpperStatus);

    ChildRect.Set(iLeft + 225, iTop + 228, iLeft + 304, iTop + 252);
    pEWTrapeziumStatus = new PegPrompt(ChildRect, LS(SID_ZERO), EWTrapeziumStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pEWTrapeziumStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pEWTrapeziumStatus);

    ChildRect.Set(iLeft + 225, iTop + 203, iLeft + 304, iTop + 227);
    pEWPinCushionStatus = new PegPrompt(ChildRect, LS(SID_ZERO), EWPinCushionStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pEWPinCushionStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pEWPinCushionStatus);

    ChildRect.Set(iLeft + 225, iTop + 178, iLeft + 304, iTop + 202);
    pEWWidthStatus = new PegPrompt(ChildRect, LS(SID_ZERO), EWWidthStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pEWWidthStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pEWWidthStatus);

    ChildRect.Set(iLeft + 225, iTop + 153, iLeft + 304, iTop + 177);
    pHShiftStatus = new PegPrompt(ChildRect, LS(SID_ZERO), HShiftStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pHShiftStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pHShiftStatus);

    ChildRect.Set(iLeft + 225, iTop + 128, iLeft + 304, iTop + 152);
    pHBlankingStatus = new PegPrompt(ChildRect, LS(SID_ZERO), HBlankingStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pHBlankingStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pHBlankingStatus);

    ChildRect.Set(iLeft + 225, iTop + 103, iLeft + 304, iTop + 127);
    pVSCorrectionStatus = new PegPrompt(ChildRect, LS(SID_ZERO), VSCorrectionStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pVSCorrectionStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pVSCorrectionStatus);

    ChildRect.Set(iLeft + 225, iTop + 78, iLeft + 304, iTop + 102);
    pVAmplitudeStatus = new PegPrompt(ChildRect, LS(SID_ZERO), VAmplitudeStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pVAmplitudeStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pVAmplitudeStatus);

    ChildRect.Set(iLeft + 225, iTop + 53, iLeft + 304, iTop + 77);
    pVPositionStatus = new PegPrompt(ChildRect, LS(SID_ZERO), VPositionStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pVPositionStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pVPositionStatus);

    ChildRect.Set(iLeft + 225, iTop + 28, iLeft + 304, iTop + 52);
    pVSlopeStatus = new PegPrompt(ChildRect, LS(SID_ZERO), VSlopeStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pVSlopeStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pVSlopeStatus);
#endif
    ChildRect.Set(iLeft + 40, iTop + 2, iLeft + 179, iTop + 26);
    pGeometryMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_GeometryMenuWindowTitle), VideoMenuWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pGeometryMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pGeometryMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pGeometryMenuWindowTitle);
#if 0
    ChildRect.Set(iLeft + 7, iTop + 381, iLeft + 216, iTop + 405);
    pDefaultGeometryButton = new TVDecoratedButton(ChildRect, LS(SID_RestoreDefaultButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, DefaultGeometryButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pDefaultGeometryButton->SetColor(PCI_NORMAL, 223);
    pDefaultGeometryButton->SetColor(PCI_NTEXT, BLUE);
    Add(pDefaultGeometryButton);

    ChildRect.Set(iLeft + 7, iTop + 356, iLeft + 216, iTop + 380);
    pEWEHTButton = new TVDecoratedButton(ChildRect, LS(SID_EWEHTCompButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, EWEHTButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pEWEHTButton->SetColor(PCI_NORMAL, 223);
    pEWEHTButton->SetColor(PCI_NTEXT, BLUE);
    Add(pEWEHTButton);

    ChildRect.Set(iLeft + 7, iTop + 331, iLeft + 216, iTop + 355);
    pHBowButton = new TVDecoratedButton(ChildRect, LS(SID_HBowButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HBowButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pHBowButton->SetColor(PCI_NORMAL, 223);
    pHBowButton->SetColor(PCI_NTEXT, BLUE);
    Add(pHBowButton);

    ChildRect.Set(iLeft + 7, iTop + 306, iLeft + 216, iTop + 330);
    pHParallelButton = new TVDecoratedButton(ChildRect, LS(SID_HParallelButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HParallelButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pHParallelButton->SetColor(PCI_NORMAL, 223);
    pHParallelButton->SetColor(PCI_NTEXT, BLUE);
    Add(pHParallelButton);

    ChildRect.Set(iLeft + 7, iTop + 281, iLeft + 216, iTop + 305);
    pEWLowerButton = new TVDecoratedButton(ChildRect, LS(SID_EWLowerCornerButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, EWLowerButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pEWLowerButton->SetColor(PCI_NORMAL, 223);
    pEWLowerButton->SetColor(PCI_NTEXT, BLUE);
    Add(pEWLowerButton);

    ChildRect.Set(iLeft + 7, iTop + 256, iLeft + 216, iTop + 280);
    pEWUpperButton = new TVDecoratedButton(ChildRect, LS(SID_EWUpperCornerButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, EWUpperButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pEWUpperButton->SetColor(PCI_NORMAL, 223);
    pEWUpperButton->SetColor(PCI_NTEXT, BLUE);
    Add(pEWUpperButton);

    ChildRect.Set(iLeft + 7, iTop + 231, iLeft + 216, iTop + 255);
    pEWTrapeziumButton = new TVDecoratedButton(ChildRect, LS(SID_EWTrapeziumButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, EWTrapeziumButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pEWTrapeziumButton->SetColor(PCI_NORMAL, 223);
    pEWTrapeziumButton->SetColor(PCI_NTEXT, BLUE);
    Add(pEWTrapeziumButton);

    ChildRect.Set(iLeft + 7, iTop + 206, iLeft + 216, iTop + 230);
    pEWPinCushionButton = new TVDecoratedButton(ChildRect, LS(SID_EWPinCushionButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, EWPinCushionButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pEWPinCushionButton->SetColor(PCI_NORMAL, 223);
    pEWPinCushionButton->SetColor(PCI_NTEXT, BLUE);
    Add(pEWPinCushionButton);

    ChildRect.Set(iLeft + 7, iTop + 181, iLeft + 216, iTop + 205);
    pEWWidthButton = new TVDecoratedButton(ChildRect, LS(SID_EWWidthButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, EWWidthButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pEWWidthButton->SetColor(PCI_NORMAL, 223);
    pEWWidthButton->SetColor(PCI_NTEXT, BLUE);
    Add(pEWWidthButton);

    ChildRect.Set(iLeft + 7, iTop + 156, iLeft + 216, iTop + 180);
    pHShiftButton = new TVDecoratedButton(ChildRect, LS(SID_HShiftButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HShiftButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pHShiftButton->SetColor(PCI_NORMAL, 223);
    pHShiftButton->SetColor(PCI_NTEXT, BLUE);
    Add(pHShiftButton);

    ChildRect.Set(iLeft + 7, iTop + 131, iLeft + 216, iTop + 155);
    pHBlankingButton = new TVDecoratedButton(ChildRect, LS(SID_HBlankingButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, HBlankingButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pHBlankingButton->SetColor(PCI_NORMAL, 223);
    pHBlankingButton->SetColor(PCI_NTEXT, BLUE);
    Add(pHBlankingButton);

    ChildRect.Set(iLeft + 7, iTop + 106, iLeft + 216, iTop + 130);
    pVSCorrectionButton = new TVDecoratedButton(ChildRect, LS(SID_VSCorrectionButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, VSCorrectionButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pVSCorrectionButton->SetColor(PCI_NORMAL, 223);
    pVSCorrectionButton->SetColor(PCI_NTEXT, BLUE);
    Add(pVSCorrectionButton);

    ChildRect.Set(iLeft + 7, iTop + 81, iLeft + 216, iTop + 105);
    pVAmplitudeButton = new TVDecoratedButton(ChildRect, LS(SID_VAmplitudeButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, VAmplitudeButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pVAmplitudeButton->SetColor(PCI_NORMAL, 223);
    pVAmplitudeButton->SetColor(PCI_NTEXT, BLUE);
    Add(pVAmplitudeButton);

    ChildRect.Set(iLeft + 7, iTop + 56, iLeft + 216, iTop + 80);
    pVPositionButton = new TVDecoratedButton(ChildRect, LS(SID_VPositionButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, VPositionButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pVPositionButton->SetColor(PCI_NORMAL, 223);
    pVPositionButton->SetColor(PCI_NTEXT, BLUE);
    Add(pVPositionButton);

    ChildRect.Set(iLeft + 7, iTop + 31, iLeft + 216, iTop + 55);
    pVSlopeButton = new TVDecoratedButton(ChildRect, LS(SID_VSlopeButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, VSlopeButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pVSlopeButton->SetColor(PCI_NORMAL, 223);
    pVSlopeButton->SetColor(PCI_NTEXT, BLUE);
    Add(pVSlopeButton);
#endif
    /* WB End Construction */
		Id(GEOMETRY_MENU_ID);

		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pGeometryMenuWindowTitle->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pGeometryMenuWindowTitle->SetFont(&korean);
		}
		else /* default for English & French */
		{
	    pGeometryMenuWindowTitle->SetFont(&Pronto_16);
		}

		/* initialize values */
#if 0
		RefreshGeometryValues();
#endif
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED GeometryMenuClass::Message(const PegMessage &Mesg)
{
		ServiceMenuClass *pServiceWindow;
		PegThing *ptDisplayedWindow[MaxNumberOfWindow];
		PegThing *pTest;
		PegThing *SomeThing = NULL;
		PegMessage TmpMesg;
		int iIndex;

    switch (Mesg.wType)
    {
    case SIGNAL(DefaultGeometryButtonID, PSF_FOCUS_RECEIVED):
		break;

    case SIGNAL(DefaultGeometryButtonID, PSF_FOCUS_LOST):
		break;

    case SIGNAL(DefaultGeometryButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = DefaultGeometryButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(EWEHTButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = EWEHTButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(HBowButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = HBowButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(HParallelButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = HParallelButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(EWLowerButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = EWLowerButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(EWUpperButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = EWUpperButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(EWTrapeziumButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = EWTrapeziumButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(EWPinCushionButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = EWPinCushionButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(EWWidthButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = EWWidthButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(HShiftButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = HShiftButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(HBlankingButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = HBlankingButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(VSCorrectionButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = VSCorrectionButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(VAmplitudeButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = VAmplitudeButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(VPositionButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = VPositionButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(VSlopeButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = VSlopeButtonID;
				this->Message(TmpMesg);
        break;

		case PM_KEYLEFT:
		case PM_KEYRIGHT:
				if (Mesg.iData != DefaultGeometryButtonID)
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
					case EWEHTButtonID:
							break;

					case HBowButtonID:
							break;

					case HParallelButtonID:
							break;

					case EWLowerButtonID:
							break;

					case EWUpperButtonID:
							break;

					case EWTrapeziumButtonID:
							break;

					case EWPinCushionButtonID:
							break;

					case EWWidthButtonID:
							break;

					case HShiftButtonID:
							break;

					case HBlankingButtonID:
							break;

					case VSCorrectionButtonID:
							break;

					case VAmplitudeButtonID:
							break;

					case VPositionButtonID:
							break;

					case VSlopeButtonID:
							break;

					}
					EndDraw();
				}
				else
				{ /* Restore Default button click */
				}
				break;

		case PM_UPDATEVALUE:
				/* the value needs to be updated on OSD */
				break;

		case PM_CLOSE:
			  /* left key = close = go back to previous menu */
				Destroy(this);
				pServiceWindow = (ServiceMenuClass*) Presentation()->Find(SERVICE_MENU_ID);
				pServiceWindow->SetBackgroundColor(PegBlueTransparent);
				break;

		case PM_REFRESHWINDOW:
			RefreshGeometryValues();
			break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

void GeometryMenuClass::RefreshGeometryValues(void)
{
}

