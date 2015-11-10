/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: HalfMuteClass
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
#include "halfmute.hpp"
#include "fullmute.hpp"
#include "anim.hpp"
#include "usertype.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
HalfMuteClass::HalfMuteClass(SIGNED iLeft, SIGNED iTop) : 
	PegBitmapWindow(NULL, kAutoAdjustBitmapSize, PegBlueTransparent, FF_RAISED) 
{
	PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 41, iTop + 41);
	InitClient();
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

	ChildRect.Set(iLeft + 0, iTop + 0, iLeft + 41, iTop + 41);
	pHalfMuteBitmap = new PegBitmapButton(ChildRect, &gbHalfMuteBitmap, HalfMuteID, FF_NONE|AF_ENABLED);
	pHalfMuteBitmap->SetColor(PCI_NORMAL, 154);
	Add(pHalfMuteBitmap);

	/* WB End Construction */

	RemoveStatus(PSF_ACCEPTS_FOCUS); /* no focus on the window */
	pHalfMuteBitmap->RemoveStatus(PSF_ACCEPTS_FOCUS); /* no focus on the bitmap */

	Id(HALF_MUTE_ID);

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED HalfMuteClass::Message(const PegMessage &Mesg)
{
	FullMuteClass *pFullMuteWindow;
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
#if defined (SVPOW_CTV100SSB)
				pFullMuteWindow = new FullMuteClass(22,375);
#else
				pFullMuteWindow = new FullMuteClass(100,375);
#endif
				Presentation()->Add(pFullMuteWindow);
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
				}
			}
			break;

	default:
		return PegWindow::Message(Mesg);
	}
	return 0;
}

