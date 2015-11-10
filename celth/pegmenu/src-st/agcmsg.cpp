/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: AGCMessageClass
//  
//  Copyright (c) STMicroElectronics Ltd.
//                TVD Applicatiion Lab
//                Noida, India.
//  
//  All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "wbstrnga.hpp"
#include "agcmsg.hpp"
#include "usertype.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
AGCMessageClass::AGCMessageClass(SIGNED iLeft, SIGNED iTop) : 
  PegWindow(FF_THIN)
{
	PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 393, iTop + 39);
	InitClient();
	SetColor(PCI_NORMAL, 223);
	RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE|PSF_ACCEPTS_FOCUS);

	ChildRect.Set(iLeft + 30, iTop + 10, iLeft + 374, iTop + 29);
	pAGCMessage = new PegPrompt(ChildRect, LSA(SID_AGCMessage), AGCMessageID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
	pAGCMessage->SetColor(PCI_NTEXT, RED);
	Add(pAGCMessage);

	/* WB End Construction */

	Id(GENAGC_MESSAGE_ID);	
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED AGCMessageClass::Message(const PegMessage &Mesg)
{
	PegThing *SomeThing;
	PegMessage NewMessage;

	switch (Mesg.wType)
	{
	case PM_CLOSE:
		/* Get the pointer to the Service Audio Menu Window */
		SomeThing = PegThing::Presentation()->Find(SERVICE_AUDIO_MENU_ID);

		if (SomeThing)
		{/*  */
			NewMessage.pTarget = SomeThing;
			NewMessage.wType = Mesg.wType;
			NewMessage.iData = Mesg.iData;
			NewMessage.lData = Mesg.lData;
			SomeThing->MessageQueue()->Push(NewMessage);
		}
		break;

	default:
		return PegWindow::Message(Mesg);
	}
	return 0;
}

