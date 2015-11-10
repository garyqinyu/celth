/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: STStereoSTWideGainBarClass
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
#include "levlbars.hpp"
#include "main_aud.hpp"
#include "anim.hpp"
#include "init_val.h"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
STWideGainBarClass::STWideGainBarClass(SIGNED iLeft, SIGNED iTop) : 
  PegBitmapWindow(NULL, kAutoAdjustBitmapSize, 223, FF_NONE)
{
	PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 219, iTop + 39);
	InitClient();
	SetColor(PCI_NORMAL, LIGHTGREEN);
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

	ChildRect.Set(iLeft + 4, iTop + 8, iLeft + 90, iTop + 31);
	pSTWideGainButton = new TVDecoratedButton(ChildRect, LSA(SID_STWideGainButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, STWideGainButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pSTWideGainButton->SetColor(PCI_NORMAL, 223);
	pSTWideGainButton->SetColor(PCI_NTEXT, BLUE);
	Add(pSTWideGainButton);

	ChildRect.Set(iLeft + 95, iTop + 8, iLeft + 209, iTop + 31);
	pSTWideGainBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucSTWideGainMin, ucSTWideGainMax, 0);
	Add(pSTWideGainBar);

	/* WB End Construction */

	pSTWideGainBar->RemoveStatus(PSF_ACCEPTS_FOCUS);
	pSTWideGainBar->SetFont(&Pronto_12);

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED STWideGainBarClass::Message(const PegMessage &Mesg)
{
	MainAudioMenuClass *pMainAudioWindow;
	
	switch (Mesg.wType)
	{
	case PM_KEYLEFT:
	case PM_KEYRIGHT:
		
		switch(Mesg.iData)
		{
		case STWideGainButtonID:
				
			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pSTWideGainBar->Value() < pSTWideGainBar->GetMaxValue())
				{
					pSTWideGainBar->Update(pSTWideGainBar->Value()+1,TRUE);
				}			
			}
			else /* PM_KEYLEFT */
			{
				if (pSTWideGainBar->Value() > pSTWideGainBar->GetMinValue())
				{
					pSTWideGainBar->Update(pSTWideGainBar->Value()-1,TRUE);
				}				
			}
			
			break;
		}
		break;

	case PM_CLOSE:
		
		Destroy(this);
		pMainAudioWindow = (MainAudioMenuClass*) Presentation()->Find(MAIN_AUDIO_MENU_ID);
		pMainAudioWindow->SetBitmap(&gbMainAudioBitmap);
		break;
	
	default:
		return PegWindow::Message(Mesg);
	}
	return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SRS3DBarsClass::SRS3DBarsClass(SIGNED iLeft, SIGNED iTop) : 
  PegBitmapWindow(NULL, kAutoAdjustBitmapSize, 223, FF_NONE)
{
	PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 219, iTop + 66);
	InitClient();
	SetColor(PCI_NORMAL, LIGHTGREEN);
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

	ChildRect.Set(iLeft + 4, iTop + 35, iLeft + 90, iTop + 58);
	pSRS3DCentreButton = new TVDecoratedButton(ChildRect, LSA(SID_SRS3DCentreButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, SRS3DCentreButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pSRS3DCentreButton->SetColor(PCI_NORMAL, 223);
	pSRS3DCentreButton->SetColor(PCI_NTEXT, BLUE);
	Add(pSRS3DCentreButton);

	ChildRect.Set(iLeft + 4, iTop + 8, iLeft + 90, iTop + 31);
	pSRS3DSpaceButton = new TVDecoratedButton(ChildRect, LSA(SID_SRS3DSpaceButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, SRS3DSpaceButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
	pSRS3DSpaceButton->SetColor(PCI_NORMAL, 223);
	pSRS3DSpaceButton->SetColor(PCI_NTEXT, BLUE);
	Add(pSRS3DSpaceButton);

	ChildRect.Set(iLeft + 95, iTop + 8, iLeft + 209, iTop + 31);
	pSRS3DSpaceBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucSRS3DSpaceMin, ucSRS3DSpaceMax, 0);
	Add(pSRS3DSpaceBar);

	ChildRect.Set(iLeft + 95, iTop + 35, iLeft + 209, iTop + 58);
	pSRS3DCentreBar = new TVProgressBar(ChildRect, FF_RAISED|PS_LED|PS_SHOW_VAL, ucSRS3DCenterMin, ucSRS3DCenterMax, 0);
	Add(pSRS3DCentreBar);

	/* WB End Construction */

	pSRS3DSpaceButton->SetFont(&Pronto_12);
	pSRS3DCentreButton->SetFont(&Pronto_12);

	pSRS3DSpaceBar->RemoveStatus(PSF_ACCEPTS_FOCUS);
	pSRS3DSpaceBar->SetFont(&Pronto_12);

	pSRS3DCentreBar->RemoveStatus(PSF_ACCEPTS_FOCUS);
	pSRS3DCentreBar->SetFont(&Pronto_12);

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED SRS3DBarsClass::Message(const PegMessage &Mesg)
{
	MainAudioMenuClass *pMainAudioWindow;

	switch (Mesg.wType)
  {
	case PM_KEYLEFT:
	case PM_KEYRIGHT:
		
		switch(Mesg.iData)
		{
		case SRS3DSpaceButtonID:
				
			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pSRS3DSpaceBar->Value() < pSRS3DSpaceBar->GetMaxValue())
				{
					pSRS3DSpaceBar->Update(pSRS3DSpaceBar->Value()+1,TRUE);
				}			
			}
			else /* PM_KEYLEFT */
			{
				if (pSRS3DSpaceBar->Value() > pSRS3DSpaceBar->GetMinValue())
				{
					pSRS3DSpaceBar->Update(pSRS3DSpaceBar->Value()-1,TRUE);
				}				
			}
			
			break;

		case SRS3DCentreButtonID:
			
			if(Mesg.wType == PM_KEYRIGHT)
			{
				if (pSRS3DCentreBar->Value() < pSRS3DCentreBar->GetMaxValue())
				{
					pSRS3DCentreBar->Update(pSRS3DCentreBar->Value()+1,TRUE);
				}			
			}
			else /* PM_KEYLEFT */
			{
				if (pSRS3DCentreBar->Value() > pSRS3DCentreBar->GetMinValue())
				{
					pSRS3DCentreBar->Update(pSRS3DCentreBar->Value()-1,TRUE);
				}				
			}				
			
			break;
		}
		break;

	case PM_CLOSE:
		
		Destroy(this);
		pMainAudioWindow = (MainAudioMenuClass*) Presentation()->Find(MAIN_AUDIO_MENU_ID);
		pMainAudioWindow->SetBitmap(&gbMainAudioBitmap);
		break;
	
	default:
		return PegWindow::Message(Mesg);
  }
  return 0;
}

