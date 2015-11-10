/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// dialog.cpp - Example dialog window using signals.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2002 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "dialog.hpp"


/*--------------------------------------------------------------------------*/
// PegAppInitialize- called by the PEG library during program startup.
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresentation)
{
	// create the dialog and add it to PegPresentationManager:

	PegRect Rect;
	Rect.Set(0, 0, 240, 140);
	DialogWin *pWin = new DialogWin(Rect);
	pPresentation->Center(pWin);
	pPresentation->Add(pWin);
}

/*--------------------------------------------------------------------------*/
// DialogWin- example dialog window.
/*--------------------------------------------------------------------------*/
DialogWin::DialogWin(const PegRect &Rect) :
	PegDialog(Rect, "Edit User Information")
{
	RemoveStatus(PSF_SIZEABLE);

	// add the "Have Email" checkbox:

	PegPoint Put;
	Put.x = mClient.wLeft + 10;
	Put.y = mClient.wBottom - 24;
	Add(new PegCheckBox(Put.x, Put.y, "Have Email", IDB_HAS_EMAIL));

    // add the phone number prompt and string:

	Put.y -= 34;
	Add(new PegPrompt(Put.x, Put.y, "Phone:"));
	Add(new PegString(Put.x + 52, Put.y, mClient.Width() - 72,
		"555-1234"));

    // add the "Title" prompt and string:

	Put.y -= 24;
	Add(new PegPrompt(Put.x, Put.y, "Title:"));
	Add(new PegString(Put.x + 52, Put.y, mClient.Width() - 72,
		"Engineer"));

    // add the name prompt and string:

	Put.y -= 24;
	Add(new PegPrompt(Put.x, Put.y, "Name:"));
	Add(new PegString(Put.x + 52, Put.y, mClient.Width() - 72,
		"Dilbert"));
}


/*--------------------------------------------------------------------------*/
// Override the Message() function to watch for the user clicking on the
// "Have Email" checkbox.
/*--------------------------------------------------------------------------*/

SIGNED DialogWin::Message(const PegMessage &Mesg)
{
	PegRect NewSize;

	switch(Mesg.wType)
	{
	case SIGNAL(IDB_HAS_EMAIL, PSF_CHECK_ON):

		// make myself taller:

		NewSize = mReal;
		NewSize.wBottom += 24;
		Resize(NewSize);
		Draw();

		// add the new email string:

		mpEmailPrompt = new PegPrompt(mClient.wLeft + 10, mClient.wBottom - 24,
			"Email:");
		Add(mpEmailPrompt);
		mpEmailString = new PegString(mClient.wLeft + 62, mClient.wBottom - 24,
			mClient.Width() - 72, "info@swellsoftware.com");
		Add(mpEmailString);
		break;

	case SIGNAL(IDB_HAS_EMAIL, PSF_CHECK_OFF):
		
		// make myself shorter:

		NewSize = mReal;
		NewSize.wBottom -= 24;
		Resize(NewSize);
		Parent()->Draw();

		// get rid of the email prompt and string:

		Destroy(mpEmailPrompt);
		Destroy(mpEmailString);
		break;

	default:
		PegDialog::Message(Mesg);
		break;
	}
	return 0;
}
