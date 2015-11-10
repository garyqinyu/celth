/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: TuningMenuClass
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
#include "math.h"
#include "peg.hpp"

#include "wbstring.hpp"
#include "tuning.hpp"
#include "install.hpp"
#include "language.hpp"
#include "anim.hpp"
#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "usertype.hpp"
#include "userfct.hpp"

#include "init_val.h"

#define PROGRAM_TO_STORE
/* This macro determines a new behavior of the installation menu:
 * When defined the L/R key actions don't switch channels. They just scroll the
 * CH # from 1 to 9. The selection of "STORE" saves the current settings into the
 * the channel quoted at the first line of the install menu.
 * When disabled the L/R key actions on the program # line cause a channel change.
 */
/* The following flag keeps trace of the state of the scanning user i/f i.e. when
 * either the key L or R is pressed on the frequency button. 
 * A null value means the "Searching" msg indicates that a scan operation is on-going
 * whereas a non-null value indicates the timer has been killed and the "Searching"
 * is not flashing anylonger.
 */
unsigned int uiSearchingOn = 0;
BOOL bRefreshPgrNbr;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
TuningMenuClass::TuningMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbTuningBitmap, kAutoAdjustBitmapSize, 0, FF_NONE) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 354, iTop + 294);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 315, iTop + 141, iLeft + 351, iTop + 168);
    pMHzStatus = new PegPrompt(ChildRect, LS(SID_MHZ), MHzStatusID);
    pMHzStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pMHzStatus);

    ChildRect.Set(iLeft + 222, iTop + 221, iLeft + 301, iTop + 248);
    pRFStandardStatus = new PegPrompt(ChildRect, LS(SID_L), RFStandardStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pRFStandardStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pRFStandardStatus);

    ChildRect.Set(iLeft + 222, iTop + 181, iLeft + 301, iTop + 208);
    pVideoStandardStatus = new PegPrompt(ChildRect, LS(SID_SECAM), VideoStandardStatusID);
    pVideoStandardStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pVideoStandardStatus);

    ChildRect.Set(iLeft + 222, iTop + 141, iLeft + 311, iTop + 168);
    pFrequencyStatus = new PegPrompt(ChildRect, LS(SID_FrequencyZero), FrequencyStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pFrequencyStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pFrequencyStatus);

    ChildRect.Set(iLeft + 222, iTop + 101, iLeft + 301, iTop + 128);
    pCountryStatus = new PegPrompt(ChildRect, LS(SID_FRANCE), CountryStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pCountryStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pCountryStatus);

    ChildRect.Set(iLeft + 222, iTop + 61, iLeft + 301, iTop + 88);
    pPrgNumberStatus = new PegPrompt(ChildRect, LS(SID_ZERO), PrgNumberStatusID, FF_NONE|AF_TRANSPARENT|TJ_LEFT|TT_COPY);
    pPrgNumberStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pPrgNumberStatus);

    ChildRect.Set(iLeft + 13, iTop + 261, iLeft + 212, iTop + 288);
    pStoreButton = new TVDecoratedButton(ChildRect, LS(SID_StoreButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, StoreButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pStoreButton->SetColor(PCI_NORMAL, 223);
    pStoreButton->SetColor(PCI_NTEXT, BLUE);
    Add(pStoreButton);

    ChildRect.Set(iLeft + 13, iTop + 221, iLeft + 212, iTop + 248);
    pRFStandardButton = new TVDecoratedButton(ChildRect, LS(SID_RFStandardButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, RFStandardButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pRFStandardButton->SetColor(PCI_NORMAL, 223);
    pRFStandardButton->SetColor(PCI_NTEXT, BLUE);
    Add(pRFStandardButton);

    ChildRect.Set(iLeft + 13, iTop + 181, iLeft + 212, iTop + 208);
    pVideoStandardButton = new TVDecoratedButton(ChildRect, LS(SID_VideoStandardButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, VideoStandardButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pVideoStandardButton->SetColor(PCI_NORMAL, 223);
    pVideoStandardButton->SetColor(PCI_NTEXT, BLUE);
    Add(pVideoStandardButton);

    ChildRect.Set(iLeft + 13, iTop + 141, iLeft + 212, iTop + 168);
    pFrequencyButton = new TVDecoratedButton(ChildRect, LS(SID_FrequencyButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, FrequencyButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pFrequencyButton->SetColor(PCI_NORMAL, 223);
    pFrequencyButton->SetColor(PCI_NTEXT, BLUE);
    Add(pFrequencyButton);

    ChildRect.Set(iLeft + 13, iTop + 101, iLeft + 212, iTop + 128);
    pCountryButton = new TVDecoratedButton(ChildRect, LS(SID_CountryButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, CountryButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pCountryButton->SetColor(PCI_NORMAL, 223);
    pCountryButton->SetColor(PCI_NTEXT, BLUE);
    Add(pCountryButton);

    ChildRect.Set(iLeft + 13, iTop + 61, iLeft + 212, iTop + 88);
    pPrgNumberButton = new TVDecoratedButton(ChildRect, LS(SID_PrgNumberButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, PrgNumberButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pPrgNumberButton->SetColor(PCI_NORMAL, 223);
    pPrgNumberButton->SetColor(PCI_NTEXT, BLUE);
    Add(pPrgNumberButton);

    ChildRect.Set(iLeft + 20, iTop + 16, iLeft + 159, iTop + 45);
    pTuningMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_TuningMenuWindowTitle), TuningMenuWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pTuningMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pTuningMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pTuningMenuWindowTitle);

    /* WB End Construction */
		Id(TUNING_MENU_ID);

		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pTuningMenuWindowTitle->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pTuningMenuWindowTitle->SetFont(&korean);
		}
		else /* default for English & French */
		{
	    pTuningMenuWindowTitle->SetFont(&Pronto_16);
		}
	/* The RF system and video std lines are just for display, 
	   no action from the user is authorized */	
		pRFStandardButton->Disable();
		pVideoStandardButton->Disable();
		
		bRefreshPgrNbr = TRUE;
		RefreshTuningWindow(); /* initialize window */
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED TuningMenuClass::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(StoreButtonID, PSF_CLICKED):
		{
        }
		break;

    case PM_KEYRIGHT:
    case PM_KEYLEFT:
						break;

					case CountryButtonID:
						break;

					case VideoStandardButtonID:
						break;

					case RFStandardButtonID:
						break;

					case FrequencyButtonID:
				break;

    case PM_KEYUP:
    case PM_KEYDOWN:
				break;

		case PM_TIMER:
			/* used for animation */
					if (Mesg.iData == TUNING_SEARCH_TIMER)
					{
						if (pFrequencyStatus->GetColor(PCI_NTEXT) == RED)
						{
							pFrequencyStatus->SetColor(PCI_NTEXT, BLUE);
						}
						else
						{
							pFrequencyStatus->SetColor(PCI_NTEXT, RED);
						}
						pFrequencyStatus->Parent()->Draw();
					}
				break;

		case PM_REFRESHWINDOW:
				RefreshTuningWindow();
				break;

		case PM_TUNINGFREQUENCYFOUND:
				KillTimer(TUNING_SEARCH_TIMER);
				pFrequencyStatus->SetColor(PCI_NTEXT, BLUE);
				bRefreshPgrNbr = FALSE;
				RefreshTuningWindow();
				uiSearchingOn = 0;
				break;

		case PM_CLOSE:
				GoBack();
				break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

void TuningMenuClass::GoBack(void)
{
	InstallMenuClass *pInstallWindow;
	PegThing *SomeThing = NULL;
	KillTimer(TUNING_SEARCH_TIMER);
	Destroy(this);
	pInstallWindow = (InstallMenuClass*) Presentation()->Find(INSTALL_MENU_ID);
	pInstallWindow->SetBitmap(&gbMainBitmap);
	uiSearchingOn =0;
}

void TuningMenuClass::RefreshTuningWindow(void)
{
		PEGCHAR tTuningFrequency[10];
		int iShift;
		int iFrequency;
		unsigned char ucChannelToStore;
		
		pFrequencyStatus->DataSet(tTuningFrequency);
		pMHzStatus->DataSet(LS(SID_MHZ)); /* set MHz display */
		pMHzStatus->Invalidate();

		pPrgNumberStatus->Parent()->Draw(); /* redraw the fields to reflect the changes */

	/*	uiSearchingOn =0; no searching */
}

