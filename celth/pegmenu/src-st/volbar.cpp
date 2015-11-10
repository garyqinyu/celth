/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: VolumeBarClass
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
#include "volbar.hpp"
#include "anim.hpp"
#include "init_val.h"
#include "usertype.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
VolumeBarClass::VolumeBarClass(SIGNED iLeft, SIGNED iTop) : 
	PegBitmapWindow(NULL, kAutoAdjustBitmapSize, PegBlueTransparent, FF_RAISED) 
{
	PegRect ChildRect;
	
	mReal.Set(iLeft, iTop, iLeft + 439, iTop + 36);
	InitClient();
	SetColor(PCI_NTEXT, BLUE);
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

	ChildRect.Set(iLeft + 10, iTop + 9, iLeft + 53, iTop + 27);
	pVolumeIcon = new PegIcon(ChildRect, &gbVolIconBitmap);
	Add(pVolumeIcon);

	ChildRect.Set(iLeft + 63, iTop + 9, iLeft + 427, iTop + 27);
	pVolumeBar = new TVProgressBar(ChildRect, FF_NONE|PS_SHOW_VAL, ucLSVolumeMin, ucLSVolumeMax, 0);
	Add(pVolumeBar);

	/* WB End Construction */

	Id(POPUP_VOLBAR_ID);

	RemoveStatus(PSF_ACCEPTS_FOCUS); /* no focus on the window */
	pVolumeBar->RemoveStatus(PSF_ACCEPTS_FOCUS); /* no focus on the volume bar */

	pVolumeBar->SetFont(&Pronto_12);
	SetTimer(kTimerVolumeBar, ONE_SECOND, ONE_SECOND/2);

	iVolumeBarTimeOut = 0;

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED VolumeBarClass::Message(const PegMessage &Mesg)
{
	  unsigned int iLoopID;
		PegMessage NewMessage;
		PegThing *SomeThing = NULL;


	switch (Mesg.wType)
	{
		case PM_TIMER:
			if(iVolumeBarTimeOut != kVolumeBarTimeOut)
			{
				iVolumeBarTimeOut++;
			}
			else
			{
				KillTimer(kTimerVolumeBar);
				Destroy(this);
			}
			break;

    case PM_HIDE:
			PegBitmapWindow::Message(Mesg);
			KillTimer(kTimerVolumeBar);   /* kill all timers for this object */
			break;

    case PM_SHOW:
			PegBitmapWindow::Message(Mesg);
	    SetTimer(kTimerVolumeBar, ONE_SECOND, ONE_SECOND/2);
			break;

		case PM_KEY:
		switch(Mesg.iData)
		{
		case VOLUME_VALUE_INCREASE:
		case VOLUME_VALUE_DECREASE:
			/* start counting for timeout again */
			iVolumeBarTimeOut = 0;

			/* update the bar if needed */
			if(Mesg.iData == VOLUME_VALUE_INCREASE)
			{
				if (pVolumeBar->Value() < pVolumeBar->GetMaxValue())
				{
					pVolumeBar->Update(pVolumeBar->Value()+1,TRUE);
				}
			}
			else /* VOLUME_VALUE_DECREASE */
			{
				if (pVolumeBar->Value() > pVolumeBar->GetMinValue())
				{
					pVolumeBar->Update(pVolumeBar->Value()-1,TRUE);					
				}
			}
			
			break;
		}
		break;

		case PM_CLOSEFORSTANDBY:
			KillTimer(kTimerVolumeBar);
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

