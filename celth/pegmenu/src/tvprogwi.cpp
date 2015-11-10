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
//#include "anim.hpp"
#include "tvprobar.hpp"
#include "mainmenu.hpp"
//#include "picture.hpp"
//#include "pbitmwin.hpp"
//#include "data_srv.h"
//#include "ap_cmd.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
TVProgressWindowClass::TVProgressWindowClass(SIGNED iLeft, SIGNED iTop, PegThing* pCaller, \
											 APCTRL_LinkId_t eEventToNotify, APCTRL_PegState_t eSubStateToNotify, \
											 const PEGCHAR *Text, UCHAR TextColor, WORD wStyle, SIGNED iMin, SIGNED iMax,\
											 SIGNED iCurrent, PegThing** pptWindow) : 
PegWindow(FF_THIN) 
{
    PegRect ChildRect;
    int iIndex = 0;
	
    mReal.Set(iLeft, iTop, iLeft + 319, iTop + 44);
    InitClient();
	SetColor(PCI_NORMAL, LIGHTGRAY);
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);		
	
    ChildRect.Set(iLeft + 14, iTop + 7, iLeft + 200, iTop + 34);
    pProgressWindowPrompt = new PegPrompt(ChildRect, Text, ProgressWindowPromptID);
    Add(pProgressWindowPrompt);
	
    ChildRect.Set(iLeft + 200, iTop + 7, iLeft + 300, iTop + 34);
    pProgressWindowBar = new TVProgressBar(ChildRect, wStyle, iMin, iMax, iCurrent);
    Add(pProgressWindowBar);
	
    /* WB End Construction */
    pProgressWindowPrompt->SetColor(PCI_NTEXT, TextColor);
	
    while (*pptWindow != NULL) /* save table list */
    {
		ptHiddenWindow[iIndex++] = *pptWindow++;
     }
     ptHiddenWindow[iIndex]  = NULL;
	
     pCallerObject = pCaller;
     eEvent = eEventToNotify;
     eSubState = eSubStateToNotify;

     Id(PROGRESSWINDOW_ID);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED TVProgressWindowClass::Message(const PegMessage &Mesg)
{
	int iIndex;
	PegMessage tmpMsg;
	
    switch (Mesg.wType)
    {
    case PM_KEYLEFT:
		if (pProgressWindowBar->Value() > pProgressWindowBar->GetMinValue())
		{
			pProgressWindowBar->Update(pProgressWindowBar->Value()-1,TRUE);
			StoreAndSend(pProgressWindowBar->Value());
		}
		break;
		
    case PM_KEYRIGHT:
		if (pProgressWindowBar->Value() < pProgressWindowBar->GetMaxValue())
		{
			pProgressWindowBar->Update(pProgressWindowBar->Value()+1,TRUE);
			StoreAndSend(pProgressWindowBar->Value());
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
		
	    	tmpMsg.pTarget = pCallerObject->Parent();
	    	tmpMsg.pSource = this;
	    	tmpMsg.wType = PM_REFRESHHELPWND;
		pCallerObject->Parent()->Message(tmpMsg);
		break;
		
    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

/* function to store in the PEG module the new scrollbar value and send it to APPLI */
void TVProgressWindowClass::StoreAndSend(short CurrentBarValue)
{
		switch (eSubState) /* save value in the proper variable structure (see data_srv.c) */
		{
		case kPegMenuPicture:
			/*add the function of changing global variable below*/
			break;
		case kPegMenuSound:
			/*add the function of changing global variable below*/
			break;
		case kPegMenuSuperFunction:
			/*add the function of changing global variable below*/
			break;
		case kPegMenuSuperSetting:
			/*add the function of changing global variable below*/
			break;
		case kPegMenuWhiteBalance:
			/*add the function of changing global variable below*/
			break;
		case kPegMenuGeometry:
			/*add the function of changing global variable below*/
			break;
		case kPegMenuVideo:
			/*add the function of changing global variable below*/
			break;
		default:
			break;
		}
		/*add ap_cmd_PegCallBack(eSubState, eEvent) function here to realize the function*/
		/*ap_cmd_PegCallBack(eSubState, eEvent); *//* send message to APPLI */
}


