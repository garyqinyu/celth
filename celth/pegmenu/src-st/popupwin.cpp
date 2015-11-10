/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: StringDisplayClass
//  
//   Copyright (c) STMicroelectronics
//                 Display & TV Division
//                 SW Application Lab.
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "popupwin.hpp"
#include "anim.hpp"
#include "pbitmwin.hpp"
#include "usertype.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PopupWindowClass::PopupWindowClass(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, SIGNED iHeight, PegBitmap* pBitmap, UCHAR BitmapStyle , UCHAR FillColor, WORD wStyle, DisplayedString_t* psStringToDisplay, UCHAR TextColor, UCHAR uPopUpWindowTimeOut, int iPopupID) :
    PegBitmapWindow(pBitmap, BitmapStyle, FillColor, wStyle) 
{
    PegRect ChildRect;
		SIGNED DeltaWidth;
		SIGNED DeltaHeight;

    mReal.Set(iLeft, iTop, iLeft + iWidth, iTop + iHeight);
    InitClient();

		/* the prompts will cover the complete window area */
		if ((pBitmap == NULL) || ((pBitmap != NULL) && (BitmapStyle == kCenterBitmap)))
		{ /* no window resize required */
			DeltaWidth = iWidth;
			DeltaHeight = iHeight;
		}
		else 
		{ /* window resize to bitmap size required */
			DeltaWidth = pBitmap->wWidth;
			DeltaHeight = pBitmap->wHeight;
		}

		if (psStringToDisplay->ucTableSize == 1)
		{ /* only one string line */
			ChildRect.Set(iLeft, iTop, iLeft + DeltaWidth, iTop + DeltaHeight);
			pString1 = new PegPrompt(ChildRect, psStringToDisplay->pTextString1, String1PromptID, FF_NONE);
			pString1->SetColor(PCI_NTEXT, TextColor);
			pString1->Style(AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString1->RemoveStatus(PSF_ACCEPTS_FOCUS);
			Add(pString1);
			pString2 = NULL;
			pString3 = NULL;
		}
		else if (psStringToDisplay->ucTableSize == 2)
		{ /* 2 string lines */
			ChildRect.Set(iLeft, iTop, iLeft + DeltaWidth, iTop + DeltaHeight/2);
			pString1 = new PegPrompt(ChildRect, psStringToDisplay->pTextString1, String1PromptID, FF_NONE|AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString1->SetColor(PCI_NTEXT, TextColor);
			pString1->Style(AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString1->RemoveStatus(PSF_ACCEPTS_FOCUS);
			Add(pString1);
			ChildRect.Set(iLeft, iTop+ DeltaHeight/2, iLeft + DeltaWidth + DeltaHeight/2, iTop + DeltaHeight/2);
			pString2 = new PegPrompt(ChildRect, psStringToDisplay->pTextString2, String2PromptID, FF_NONE|AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString2->SetColor(PCI_NTEXT, TextColor);
			pString2->Style(AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString2->RemoveStatus(PSF_ACCEPTS_FOCUS);
			Add(pString2);
			pString3 = NULL;
		}
		else if (psStringToDisplay->ucTableSize == 3)
		{ /* 3 string lines */
			ChildRect.Set(iLeft, iTop, iLeft + DeltaWidth, iTop + DeltaHeight/3);
			pString1 = new PegPrompt(ChildRect, psStringToDisplay->pTextString1, String1PromptID, FF_NONE|AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString1->SetColor(PCI_NTEXT, TextColor);
			pString1->Style(AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString1->RemoveStatus(PSF_ACCEPTS_FOCUS);
			Add(pString1);
			ChildRect.Set(iLeft, iTop+ DeltaHeight/3, iLeft + DeltaWidth, iTop + 2*DeltaHeight/3);
			pString2 = new PegPrompt(ChildRect, psStringToDisplay->pTextString2, String2PromptID, FF_NONE|AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString2->SetColor(PCI_NTEXT, TextColor);
			pString2->Style(AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString2->RemoveStatus(PSF_ACCEPTS_FOCUS);
			Add(pString2);
			ChildRect.Set(iLeft, iTop+ 2*DeltaHeight/3, iLeft + DeltaWidth, iTop + DeltaHeight);
			pString3 = new PegPrompt(ChildRect, psStringToDisplay->pTextString3, String3PromptID, FF_NONE|AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString3->SetColor(PCI_NTEXT, TextColor);
			pString3->Style(AF_TRANSPARENT|TJ_CENTER|TT_COPY);
			pString3->RemoveStatus(PSF_ACCEPTS_FOCUS);
			Add(pString3);
		}

    /* WB End Construction */
		Id(iPopupID);

		if (gbCurrentLanguage == 1) /* language = chinese */
		{
			if (pString1)
				pString1->SetFont(&A12RFont);
			if (pString2)
				pString2->SetFont(&A12RFont);
			if (pString3)
				pString3->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    if (pString1)
				pString1->SetFont(&korean);
	    if (pString2)
				pString2->SetFont(&korean);
	    if (pString3)
				pString3->SetFont(&korean);
		}
		else /* default for English & French */
		{
			if (pString1)
				pString1->SetFont(&Pronto_12);
			if (pString2)
				pString2->SetFont(&Pronto_12);
			if (pString3)
				pString3->SetFont(&Pronto_12);
		}

		RemoveStatus(PSF_ACCEPTS_FOCUS);
		if(uPopUpWindowTimeOut>0)
		{ /* Set a timer only if Time out >0 */
		  /* If no time out then pop up will be permanent */
		SetTimer(POPUP_WINDOW_TIMER, uPopUpWindowTimeOut*ONE_SECOND, 0);
		}
		mTimout = uPopUpWindowTimeOut;

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PopupWindowClass::Message(const PegMessage &Mesg)
{
	  unsigned int iLoopID;
		PegMessage NewMessage;
		PegThing *SomeThing = NULL;

    switch (Mesg.wType)
    {

		case PM_HIDE:
				KillTimer(POPUP_WINDOW_TIMER);
				PegBitmapWindow::Message(Mesg);
				break;

		case PM_SHOW:
				if(mTimout>0)
				{
				SetTimer(POPUP_WINDOW_TIMER, mTimout*ONE_SECOND, 0);
				}
				PegBitmapWindow::Message(Mesg);
				break;

		case PM_CLOSEFORSTANDBY:
		case PM_TIMER:
				KillTimer(POPUP_WINDOW_TIMER);
				Destroy(this);
			break;

		case PM_CLOSE:
			// forward close message to other window
			for	(iLoopID = FIRST_POPUP_ID - 1 ; iLoopID >FIRST_USER_ID ; iLoopID--)
			{
				SomeThing = PegThing::Presentation()->Find(iLoopID);

				if (SomeThing)
				{/* yes the volume bar is on display currently */
					NewMessage.pTarget = SomeThing;
					NewMessage.wType = PM_CLOSE;
					NewMessage.iData = 0;
					NewMessage.lData = 0;
					SomeThing->MessageQueue()->Push(NewMessage);
					break;
				}
			}
			break;

		default:
        return PegBitmapWindow::Message(Mesg);
    }
    return 0;
}

void PopupWindowClass::SetFont(PegFont* pFont)
{
	if (pString1)
		pString1->SetFont(pFont);
	if (pString2)
		pString2->SetFont(pFont);
	if (pString3)
		pString3->SetFont(pFont);
}




