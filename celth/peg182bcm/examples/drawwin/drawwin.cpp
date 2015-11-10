/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// drawwin.cpp - Example overriding a base PEG class
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
#include "drawwin.hpp"


/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresentation)
{
	// create some simple Windows:

	PegRect Rect;
	Rect.Set(180, 80, 468, 300);
	pPresentation->Add(new PegDecoratedWindow(Rect));
	//pPresentation->Add(new DerivedWin(Rect));

}

/*--------------------------------------------------------------------------*/
DerivedWin::DerivedWin(PegRect &Rect) : PegWindow(Rect)
{
}

extern PegBitmap gbBricksBitmap;

/*--------------------------------------------------------------------------*/
void DerivedWin::Draw(void)
{
	BeginDraw();
	PegWindow::Draw();
	BitmapFill(mClient, &gbBricksBitmap);
	EndDraw();
}



