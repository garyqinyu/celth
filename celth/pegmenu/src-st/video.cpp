/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: VideoMenuClass
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
#include "video.hpp"
#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "service.hpp"
#include "anim.hpp"
#include "userfct.hpp"
#include "tvprobar.hpp"
#include "init_val.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
VideoMenuClass::VideoMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(NULL, kAutoAdjustBitmapSize, PegBlueTransparent, FF_RAISED) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 344, iTop + 405);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 226, iTop + 343, iLeft + 325, iTop + 370);
    pLumaChromaDelayStatus = new PegPrompt(ChildRect, LS(SID_ZERO), LumaChromaDelayStatusID, TT_COPY);
    pLumaChromaDelayStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pLumaChromaDelayStatus);

    ChildRect.Set(iLeft + 226, iTop + 311, iLeft + 325, iTop + 338);
    pFBDelayStatus = new PegPrompt(ChildRect, LS(SID_ZERO), FBDelayStatusID, TT_COPY);
    pFBDelayStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pFBDelayStatus);

    ChildRect.Set(iLeft + 226, iTop + 279, iLeft + 325, iTop + 306);
    pPrescaleRGBStatus = new PegPrompt(ChildRect, LS(SID_ZERO), PrescaleRGBStatusID, TT_COPY);
    pPrescaleRGBStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pPrescaleRGBStatus);

    ChildRect.Set(iLeft + 226, iTop + 247, iLeft + 325, iTop + 274);
    pOffsetGStatus = new PegPrompt(ChildRect, LS(SID_ZERO), OffsetGStatusID, TT_COPY);
    pOffsetGStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pOffsetGStatus);

    ChildRect.Set(iLeft + 226, iTop + 215, iLeft + 325, iTop + 242);
    pOffsetRStatus = new PegPrompt(ChildRect, LS(SID_ZERO), OffsetRStatusID, TT_COPY);
    pOffsetRStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pOffsetRStatus);

    ChildRect.Set(iLeft + 226, iTop + 183, iLeft + 325, iTop + 210);
    pPeakWhiteLimiterStatus = new PegPrompt(ChildRect, LS(SID_ZERO), PeakWhiteLimiterStatusID, TT_COPY);
    pPeakWhiteLimiterStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pPeakWhiteLimiterStatus);

    ChildRect.Set(iLeft + 226, iTop + 151, iLeft + 325, iTop + 178);
    pWPBStatus = new PegPrompt(ChildRect, LS(SID_ZERO), WPBStatusID, TT_COPY);
    pWPBStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pWPBStatus);

    ChildRect.Set(iLeft + 226, iTop + 119, iLeft + 325, iTop + 146);
    pWPGStatus = new PegPrompt(ChildRect, LS(SID_ZERO), WPGStatusID, TT_COPY);
    pWPGStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pWPGStatus);

    ChildRect.Set(iLeft + 226, iTop + 87, iLeft + 325, iTop + 114);
    pWPRStatus = new PegPrompt(ChildRect, LS(SID_ZERO), WPRStatusID, TT_COPY);
    pWPRStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pWPRStatus);

    ChildRect.Set(iLeft + 226, iTop + 55, iLeft + 325, iTop + 82);
    pG2AlignStatus = new PegPrompt(ChildRect, LS(SID_CONFIG), G2AlignStatusID, TT_COPY);
    pG2AlignStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pG2AlignStatus);

    ChildRect.Set(iLeft + 11, iTop + 375, iLeft + 220, iTop + 399);
    pDefaultVideoButton = new TVDecoratedButton(ChildRect, LS(SID_RestoreDefaultButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, DefaultVideoButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pDefaultVideoButton->SetColor(PCI_NORMAL, 223);
    pDefaultVideoButton->SetColor(PCI_NTEXT, BLUE);
    Add(pDefaultVideoButton);

    ChildRect.Set(iLeft + 11, iTop + 343, iLeft + 220, iTop + 367);
    pLumaChromaDelayButton = new TVDecoratedButton(ChildRect, LS(SID_LumaChromaDelayButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, LumaChromaDelayButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pLumaChromaDelayButton->SetColor(PCI_NORMAL, 223);
    pLumaChromaDelayButton->SetColor(PCI_NTEXT, BLUE);
    Add(pLumaChromaDelayButton);

    ChildRect.Set(iLeft + 11, iTop + 311, iLeft + 220, iTop + 335);
    pFBDelayButton = new TVDecoratedButton(ChildRect, LS(SID_FBDelayButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, FBDelayButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pFBDelayButton->SetColor(PCI_NORMAL, 223);
    pFBDelayButton->SetColor(PCI_NTEXT, BLUE);
    Add(pFBDelayButton);

    ChildRect.Set(iLeft + 11, iTop + 279, iLeft + 220, iTop + 303);
    pPrescaleRGBButton = new TVDecoratedButton(ChildRect, LS(SID_PrescaleRGBButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, PrescaleRGBButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pPrescaleRGBButton->SetColor(PCI_NORMAL, 223);
    pPrescaleRGBButton->SetColor(PCI_NTEXT, BLUE);
    Add(pPrescaleRGBButton);

    ChildRect.Set(iLeft + 11, iTop + 247, iLeft + 220, iTop + 271);
    pOffsetGButton = new TVDecoratedButton(ChildRect, LS(SID_OffsetGButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, OffsetGButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pOffsetGButton->SetColor(PCI_NORMAL, 223);
    pOffsetGButton->SetColor(PCI_NTEXT, BLUE);
    Add(pOffsetGButton);

    ChildRect.Set(iLeft + 11, iTop + 215, iLeft + 220, iTop + 239);
    pOffsetRButton = new TVDecoratedButton(ChildRect, LS(SID_OffsetRButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, OffsetRButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pOffsetRButton->SetColor(PCI_NORMAL, 223);
    pOffsetRButton->SetColor(PCI_NTEXT, BLUE);
    Add(pOffsetRButton);

    ChildRect.Set(iLeft + 11, iTop + 183, iLeft + 220, iTop + 207);
    pPeakWhiteLimiterButton = new TVDecoratedButton(ChildRect, LS(SID_PeakLimitButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, PeakWhiteLimiterButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pPeakWhiteLimiterButton->SetColor(PCI_NORMAL, 223);
    pPeakWhiteLimiterButton->SetColor(PCI_NTEXT, BLUE);
    Add(pPeakWhiteLimiterButton);

    ChildRect.Set(iLeft + 11, iTop + 151, iLeft + 220, iTop + 175);
    pWPBButton = new TVDecoratedButton(ChildRect, LS(SID_WhitePointBButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, WPBButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pWPBButton->SetColor(PCI_NORMAL, 223);
    pWPBButton->SetColor(PCI_NTEXT, BLUE);
    Add(pWPBButton);

    ChildRect.Set(iLeft + 11, iTop + 119, iLeft + 220, iTop + 143);
    pWPGButton = new TVDecoratedButton(ChildRect, LS(SID_WhitePointGButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, WPGButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pWPGButton->SetColor(PCI_NORMAL, 223);
    pWPGButton->SetColor(PCI_NTEXT, BLUE);
    Add(pWPGButton);

    ChildRect.Set(iLeft + 11, iTop + 87, iLeft + 220, iTop + 111);
    pWPRButton = new TVDecoratedButton(ChildRect, LS(SID_WhitePointRButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, WPRButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pWPRButton->SetColor(PCI_NORMAL, 223);
    pWPRButton->SetColor(PCI_NTEXT, BLUE);
    Add(pWPRButton);

    ChildRect.Set(iLeft + 11, iTop + 55, iLeft + 220, iTop + 79);
    pG2AlignButton = new TVDecoratedButton(ChildRect, LS(SID_G2Button), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, G2AlignButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pG2AlignButton->SetColor(PCI_NORMAL, 223);
    pG2AlignButton->SetColor(PCI_NTEXT, BLUE);
    Add(pG2AlignButton);

    ChildRect.Set(iLeft + 20, iTop + 16, iLeft + 159, iTop + 45);
    pVideoMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_VideoMenuWindowTitle), VideoMenuWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pVideoMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pVideoMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pVideoMenuWindowTitle);

    /* WB End Construction */
		Id(VIDEO_SERVICE_MENU_ID);

		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pVideoMenuWindowTitle->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pVideoMenuWindowTitle->SetFont(&korean);
		}
		else /* default for English & French */
		{
	    pVideoMenuWindowTitle->SetFont(&Pronto_16);
		}

		/* initialize values */
		RefreshVideoValues();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED VideoMenuClass::Message(const PegMessage &Mesg)
{
		ServiceMenuClass *pServiceWindow;
		PegThing *ptDisplayedWindow[MaxNumberOfWindow];
		PegThing *pTest;
		PegThing *SomeThing = NULL;
		PegMessage TmpMesg;
		int iIndex;

    switch (Mesg.wType)
    {
    case SIGNAL(DefaultVideoButtonID, PSF_FOCUS_RECEIVED):
		break;

    case SIGNAL(DefaultVideoButtonID, PSF_FOCUS_LOST):
		break;

    case SIGNAL(DefaultVideoButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = DefaultVideoButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(LumaChromaDelayButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = LumaChromaDelayButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(FBDelayButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = FBDelayButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(PrescaleRGBButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = PrescaleRGBButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(OffsetGButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = OffsetGButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(OffsetRButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = OffsetRButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(PeakWhiteLimiterButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = PeakWhiteLimiterButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(WPBButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = WPBButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(WPGButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = WPGButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(WPRButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = WPRButtonID;
				this->Message(TmpMesg);
        break;

    case SIGNAL(G2AlignButtonID, PSF_CLICKED):
        // Enter your code here:
				TmpMesg.wType = PM_KEYRIGHT;
				TmpMesg.iData = G2AlignButtonID;
				this->Message(TmpMesg);
        break;

		case PM_KEYLEFT:
		case PM_KEYRIGHT:
				if (Mesg.iData != DefaultVideoButtonID)
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
					case WPRButtonID:
							break;

					case WPGButtonID:
							break;

					case WPBButtonID:
							break;

					case PeakWhiteLimiterButtonID:
							break;

					case OffsetRButtonID:
							break;

					case OffsetGButtonID:
							break;

					case PrescaleRGBButtonID:
							break;

					case FBDelayButtonID:
							break;

					case LumaChromaDelayButtonID:
							break;
					
					case G2AlignButtonID:
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
				Destroy(this);
				pServiceWindow = (ServiceMenuClass*) Presentation()->Find(SERVICE_MENU_ID);
				pServiceWindow->SetBackgroundColor(PegBlueTransparent);
				break;

		case PM_REFRESHWINDOW:
			RefreshVideoValues();
			break;

		default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

void VideoMenuClass::RefreshVideoValues(void)
{
}

