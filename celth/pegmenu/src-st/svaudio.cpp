/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: ServiceAudioMenuClass
//  
//  Copyright (c) STMicroelectronics Pvt.Ltd.
//                TVD Application Lab
//                Noida, India.
//  
//  All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


#include "peg.hpp"
#include "wbstrnga.hpp"
#include "svaudio.hpp"
#include "service.hpp"
#include "popupwin.hpp"
#include "anim.hpp"
#include "init_val.h"
#include "usertype.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ServiceAudioMenuClass::ServiceAudioMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(NULL, kAutoAdjustBitmapSize, PegBlueTransparent, FF_RAISED) 
{
	PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 393, iTop + 213);
	InitClient();
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

	ChildRect.Set(iLeft + 200, iTop + 172, iLeft + 379, iTop + 192);
	pThrSwNICAMBar = new TVProgressBar(ChildRect, FF_NONE|PS_SHOW_VAL, ucThrSwNICAMMin, ucThrSwNICAMMax, 0);
	Add(pThrSwNICAMBar);

	ChildRect.Set(iLeft + 200, iTop + 142, iLeft + 379, iTop + 162);
	pPrescNICAMBar = new TVProgressBar(ChildRect, FF_NONE|PS_SHOW_VAL, scPrescNICAMMin, scPrescNICAMMax, 0);
	Add(pPrescNICAMBar);

	ChildRect.Set(iLeft + 200, iTop + 112, iLeft + 379, iTop + 132);
	pPrescFMBar = new TVProgressBar(ChildRect, FF_NONE|PS_SHOW_VAL, scPrescFMMin, scPrescFMMax, 0);
	Add(pPrescFMBar);

	ChildRect.Set(iLeft + 200, iTop + 82, iLeft + 379, iTop + 102);
	pPrescSCARTBar = new TVProgressBar(ChildRect, FF_NONE|PS_SHOW_VAL, scPrescSCARTMin, scPrescSCARTMax, 0);
	Add(pPrescSCARTBar);

	ChildRect.Set(iLeft + 200, iTop + 52, iLeft + 379, iTop + 72);
	pAGCAdjBar = new TVProgressBar(ChildRect, FF_NONE|PS_SHOW_VAL, ucAGCMin, ucAGCMax, 0);
	Add(pAGCAdjBar);

	ChildRect.Set(iLeft + 10, iTop + 170, iLeft + 189, iTop + 197);
	pThrSwNICAMButton = new TVDecoratedButton(ChildRect, LSA(SID_ThrSwNICAMButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, ThrSwNICAMButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pThrSwNICAMButton->SetColor(PCI_NORMAL, 223);
	pThrSwNICAMButton->SetColor(PCI_NTEXT, BLUE);
	Add(pThrSwNICAMButton);

	ChildRect.Set(iLeft + 10, iTop + 140, iLeft + 169, iTop + 167);
	pPrescNICAMButton = new TVDecoratedButton(ChildRect, LSA(SID_PrescNICAMButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, PrescNICAMButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pPrescNICAMButton->SetColor(PCI_NORMAL, 223);
	pPrescNICAMButton->SetColor(PCI_NTEXT, BLUE);
	Add(pPrescNICAMButton);

	ChildRect.Set(iLeft + 10, iTop + 110, iLeft + 169, iTop + 137);
	pPrescFMButton = new TVDecoratedButton(ChildRect, LSA(SID_PrescFMButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, PrescFMButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pPrescFMButton->SetColor(PCI_NORMAL, 223);
	pPrescFMButton->SetColor(PCI_NTEXT, BLUE);
	Add(pPrescFMButton);

	ChildRect.Set(iLeft + 10, iTop + 80, iLeft + 169, iTop + 107);
	pPrescSCARTButton = new TVDecoratedButton(ChildRect, LSA(SID_PrescSCARTButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, PrescSCARTButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pPrescSCARTButton->SetColor(PCI_NORMAL, 223);
	pPrescSCARTButton->SetColor(PCI_NTEXT, BLUE);
	Add(pPrescSCARTButton);

	ChildRect.Set(iLeft + 10, iTop + 50, iLeft + 169, iTop + 77);
	pAGCAdjButton = new TVDecoratedButton(ChildRect, LSA(SID_AGCAdjButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, AGCAdjButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pAGCAdjButton->SetColor(PCI_NORMAL, 223);
	pAGCAdjButton->SetColor(PCI_NTEXT, BLUE);
	Add(pAGCAdjButton);

	ChildRect.Set(iLeft + 10, iTop + 2, iLeft + 389, iTop + 48);
	pServiceAudioMenuTitle = new PegPrompt(ChildRect, LSA(SID_ServiceAudioMenuWindowTitle), ServiceAudioMenuTitleID);
	pServiceAudioMenuTitle->SetColor(PCI_NORMAL, BLUE);
	pServiceAudioMenuTitle->SetColor(PCI_NTEXT, BLUE);
	Add(pServiceAudioMenuTitle);

	/* WB End Construction */

	Id(SERVICE_AUDIO_MENU_ID);

	/* Set font for the Title of the window */
	if (gbCurrentLanguage == 1) /* language = chinese */
	{
		pServiceAudioMenuTitle->SetFont(&A12RFont);
	}
	else if (gbCurrentLanguage == 2)  /* language = korean */
	{
		pServiceAudioMenuTitle->SetFont(&korean);
	}
	else /* default for English & French */
	{
		pServiceAudioMenuTitle->SetFont(&Pronto_16);
	}

	/* Set font for the value written in the bars */
	pThrSwNICAMBar->SetFont(&Pronto_12);
	pPrescNICAMBar->SetFont(&Pronto_12);
	pPrescFMBar->SetFont(&Pronto_12);
	pPrescSCARTBar->SetFont(&Pronto_12);
	pAGCAdjBar->SetFont(&Pronto_12);

	/* Remove focus from the bars */
	pThrSwNICAMBar->RemoveStatus(PSF_ACCEPTS_FOCUS);
	pPrescNICAMBar->RemoveStatus(PSF_ACCEPTS_FOCUS);
	pPrescFMBar->RemoveStatus(PSF_ACCEPTS_FOCUS);
	pPrescSCARTBar->RemoveStatus(PSF_ACCEPTS_FOCUS);
	pAGCAdjBar->RemoveStatus(PSF_ACCEPTS_FOCUS);

	pAGCMessageWindow = NULL;

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED ServiceAudioMenuClass::Message(const PegMessage &Mesg)
{
	PegThing	*SomeThing;
	ServiceMenuClass	*pServiceWindow;
		
	switch (Mesg.wType)
	{
	case SIGNAL(ThrSwNICAMButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(ThrSwNICAMButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		break;

	case SIGNAL(ThrSwNICAMButtonID, PSF_FOCUS_LOST):
		// Enter your code here:
		break;

	case SIGNAL(PrescNICAMButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(PrescNICAMButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		break;

	case SIGNAL(PrescNICAMButtonID, PSF_FOCUS_LOST):
		// Enter your code here:
		break;

	case SIGNAL(PrescFMButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(PrescFMButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		break;

	case SIGNAL(PrescFMButtonID, PSF_FOCUS_LOST):
		// Enter your code here:
		break;

	case SIGNAL(PrescSCARTButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(PrescSCARTButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		break;

	case SIGNAL(PrescSCARTButtonID, PSF_FOCUS_LOST):
		// Enter your code here:
		break;

	case SIGNAL(AGCAdjButtonID, PSF_CLICKED):
		// Enter your code here:
		break;

	case SIGNAL(AGCAdjButtonID, PSF_FOCUS_RECEIVED):
		// Enter your code here:
		if(pAGCMessageWindow == NULL)
		{
			pAGCMessageWindow = new AGCMessageClass(100,50);
			Presentation()->Add(pAGCMessageWindow);
		}
		break;

	case SIGNAL(AGCAdjButtonID, PSF_FOCUS_LOST):
		// Enter your code here:
		if(pAGCMessageWindow != NULL)
		{
			Destroy(pAGCMessageWindow);
			pAGCMessageWindow = NULL;
		}
		break;

	case PM_KEYLEFT:
	case PM_KEYRIGHT:

		switch(Mesg.iData)
		{
		case ThrSwNICAMButtonID:
			
			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pThrSwNICAMBar->Value() < pThrSwNICAMBar->GetMaxValue())
				{
					pThrSwNICAMBar->Update(pThrSwNICAMBar->Value()+1,TRUE);					
				}
			}
			else	/* PM_KEYLEFT */
			{
				if (pThrSwNICAMBar->Value() > pThrSwNICAMBar->GetMinValue())
				{
					pThrSwNICAMBar->Update(pThrSwNICAMBar->Value()-1,TRUE);
				}
			}
			
			break;

		case PrescNICAMButtonID:
			
			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pPrescNICAMBar->Value() < pPrescNICAMBar->GetMaxValue())
				{
					pPrescNICAMBar->Update(pPrescNICAMBar->Value()+1,TRUE);
				}			
			}
			else	/* PM_KEYLEFT */
			{
				if (pPrescNICAMBar->Value() > pPrescNICAMBar->GetMinValue())
				{
					pPrescNICAMBar->Update(pPrescNICAMBar->Value()-1,TRUE);
				}
			}
			
			break;

		case PrescFMButtonID:
			
			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pPrescFMBar->Value() < pPrescFMBar->GetMaxValue())
				{
					pPrescFMBar->Update(pPrescFMBar->Value()+1,TRUE);
				}						
			}
			else	/* PM_KEYLEFT */
			{
				if (pPrescFMBar->Value() > pPrescFMBar->GetMinValue())
				{
					pPrescFMBar->Update(pPrescFMBar->Value()-1,TRUE);
				}				
			}
			
			break;
		
		case PrescSCARTButtonID:
			
			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pPrescSCARTBar->Value() < pPrescSCARTBar->GetMaxValue())
				{
					pPrescSCARTBar->Update(pPrescSCARTBar->Value()+1,TRUE);
				}
			}
			else	/* PM_KEYLEFT */
			{
				if (pPrescSCARTBar->Value() > pPrescSCARTBar->GetMinValue())
				{
					pPrescSCARTBar->Update(pPrescSCARTBar->Value()-1,TRUE);
				}				
			}
			
			break;

		case AGCAdjButtonID:
			
			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pAGCAdjBar->Value() < pAGCAdjBar->GetMaxValue())
				{
					pAGCAdjBar->Update(pAGCAdjBar->Value()+1,TRUE);
				}			
			}
			else	/* PM_KEYLEFT */
			{
				if (pAGCAdjBar->Value() > pAGCAdjBar->GetMinValue())
				{
					pAGCAdjBar->Update(pAGCAdjBar->Value()-1,TRUE);
				}				
			}
			
			break;			
		}
		break;

	case PM_REFRESHWINDOW:

		unsigned char ucCurrentValue;
		

		pAGCAdjBar->Update(ucCurrentValue,TRUE);
		
		if(ucCurrentValue == 0)
		{
			/* Get the pointer to the LOW AGC Message Window */
			SomeThing = PegThing::Presentation()->Find(LOWAGC_MESSAGE_ID);

			if(SomeThing)
			{
				Destroy(SomeThing);
			}
			
			PopupWindowClass *pPopupWindow;
			char *LowAGCMessage = "Low Signal Strength";
			PEGCHAR PegTable[20]; /* 15 char max to 20*/
			PEGCHAR* pPegText;
			DisplayedString_t sStringToDisplay;
			char* pText;
			unsigned char iLoop;
		
			PegThing::Presentation()->BeginDraw();
			pText= LowAGCMessage;
			pPegText = PegTable;
			for (iLoop = 0 ; iLoop<20 ; iLoop++) /* 15 to 20 */
				*pPegText++ = *pText++;
			
			sStringToDisplay.ucTableSize = 1; /* one string to display */
			sStringToDisplay.pTextString1 = PegTable;
			
			/* now display message */
			pPopupWindow = new PopupWindowClass(50, 50, 200, 50, NULL, kCenterBitmap , PegBlueTransparent, FF_NONE, &sStringToDisplay, BLUE, PopupTimout, LOWAGC_MESSAGE_ID);
		
			PegThing::Presentation()->Add(pPopupWindow);
			PegThing::Presentation()->EndDraw();
		}
		break;
		
	case PM_CLOSE:
		
		if(pAGCMessageWindow != NULL)
		{
			Destroy(pAGCMessageWindow);
			pAGCMessageWindow = NULL;
		}
		
		Destroy(this);		
		pServiceWindow = (ServiceMenuClass*) Presentation()->Find(SERVICE_MENU_ID);
		pServiceWindow->SetBackgroundColor(PegBlueTransparent);
		break;
	
	default:
		return PegWindow::Message(Mesg);
  }
  return 0;
}

