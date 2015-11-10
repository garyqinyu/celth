/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: FullMuteClass
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
#include "fullmute.hpp"
#include "anim.hpp"
#include "usertype.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
FullMuteClass::FullMuteClass(SIGNED iLeft, SIGNED iTop) : 
	PegBitmapWindow(NULL, kAutoAdjustBitmapSize, PegBlueTransparent, FF_RAISED) 
{
	PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 74, iTop + 74);
	InitClient();
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

	ChildRect.Set(iLeft + 0, iTop + 0, iLeft + 74, iTop + 74);
	pFullMuteBitmap = new PegBitmapButton(ChildRect, &gbFullMuteBitmap, FullMuteID, FF_NONE|AF_ENABLED);
	pFullMuteBitmap->SetColor(PCI_NORMAL, 156);
	Add(pFullMuteBitmap);

	/* WB End Construction */

	RemoveStatus(PSF_ACCEPTS_FOCUS); /* no focus on the window */
	pFullMuteBitmap->RemoveStatus(PSF_ACCEPTS_FOCUS); /* no focus on the bitmap */

	Id(FULL_MUTE_ID);

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED FullMuteClass::Message(const PegMessage &Mesg)
{
  unsigned int iLoopID;
	PegMessage NewMessage;
	PegThing *SomeThing = NULL;

	switch (Mesg.wType)
	{
	case PM_KEY:

		switch(Mesg.iData)
		{
		case PK_MUTE_UNMUTE:
		
			Destroy(this);
			PegWindow::Message(Mesg);
			break;
		}
		break;

		case PM_CLOSEFORSTANDBY:
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
			return PegWindow::Message(Mesg);
	}
	return 0;
}
