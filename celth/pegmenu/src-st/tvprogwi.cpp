/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: TVProgressWindowClass
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
#include "tvprogwi.hpp"
#include "anim.hpp"
#include "tvprobar.hpp"
#include "mainmenu.hpp"
#include "picture.hpp"
#include "pbitmwin.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
TVProgressWindowClass::TVProgressWindowClass(SIGNED iLeft, SIGNED iTop, PegThing* pCaller, APCTRL_LinkId_t eEventToNotify, APCTRL_PegState_t eSubStateToNotify, const PEGCHAR *Text, UCHAR TextColor, WORD wStyle, SIGNED iMin, SIGNED iMax, SIGNED iCurrent, PegThing** pptWindow) : 
    PegBitmapWindow(&gbWinbarBitmap, kAutoAdjustBitmapSize, 200, FF_NONE) 
{
    PegRect ChildRect;
		int iIndex = 0;

    mReal.Set(iLeft, iTop, iLeft + 319, iTop + 44);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

			ChildRect.Set(iLeft + 14, iTop + 7, iLeft + 305, iTop + 37);
			pProgressWindowPrompt = new PegPrompt(ChildRect, Text, ProgressWindowPromptID);
			pProgressWindowPrompt->SetFont(&Pronto_12);
			pProgressWindowPrompt->Style(AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			Add(pProgressWindowPrompt);

			pProgressWindowBar = NULL;

      SetTimer(G2ALIGN_TIMER, ONE_SECOND / 2, ONE_SECOND / 2);

    /* WB End Construction */
		Id(PROGRESSWINDOW_ID);
		pProgressWindowPrompt->SetColor(PCI_NTEXT, TextColor);

		while (*pptWindow != NULL) /* save table list */
		{
			ptHiddenWindow[iIndex++] = *pptWindow++;
		}
		ptHiddenWindow[iIndex]  = NULL;

		pCallerObject = pCaller;
		eEvent = eEventToNotify;
		eSubState = eSubStateToNotify;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED TVProgressWindowClass::Message(const PegMessage &Mesg)
{
		int iIndex;
		PegMessage TmpMesg;

    switch (Mesg.wType)
    {
    case PM_KEYLEFT:
				break;

    case PM_KEYRIGHT:
				break;

		case PM_TIMER:
				if (Mesg.iData == G2ALIGN_TIMER)
				{	/* for G2 alignement */
					pProgressWindowPrompt->Invalidate();
					pProgressWindowPrompt->Parent()->Draw();
				}
				else
				{
					return PegWindow::Message(Mesg);
				}
				break;

    case PM_KEYUP:
    case PM_KEYDOWN:
    case PM_CLOSE:


				Destroy(this);
				BeginDraw();
				iIndex = 0;
				while(ptHiddenWindow[iIndex])
					iIndex++;

				if (iIndex > 0)
				{ /* some window must be added */
					iIndex--; /* point to last not NULL index */

					while(iIndex >= 0)
					{
						PegPresentationManager::Presentation()->Add(ptHiddenWindow[iIndex--]);
					}
				}
				MoveToFront(pCallerObject);
				EndDraw();
				break;

    default:
        return PegBitmapWindow::Message(Mesg);
    }
    return 0;
}

/* function to store in the PEG module the new scrollbar value and send it to APPLI */
void TVProgressWindowClass::StoreAndSend(short CurrentBarValue)
{
}


