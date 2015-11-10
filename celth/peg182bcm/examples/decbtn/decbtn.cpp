/*--------------------------------------------------------------------------*/
// decbtn.cpp - Example using the PegDecoratedButton on toolbars and windows
//
// Author: Jim DeLisle
//
// Date: 01/12/2000
//
// Copyright (c) 1998-2000 Swell Software, Inc.
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
#include "dbwin.hpp"

/*--------------------------------------------------------------------------*/
// Externs
/*--------------------------------------------------------------------------*/
extern PegBitmap gbGreenDotBitmap;
extern PegBitmap gbRedDotBitmap;
extern PegBitmap gbRefreshBitmap;
extern PegBitmap gbStopBitmap;
extern PegBitmap gbNewBitmap;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager* pPresent)
{
	PegRect Rect;
	Rect.Set(10, 10, 630, 370);
	PegDecoratedWindow* pWindow = new PegDecoratedWindow(Rect);

	pWindow->Add(new PegTitle("PegDecoratedButton Example"));

	PegToolBar* pToolBar = new PegToolBar();
	pWindow->Add(pToolBar);

	pWindow->SetColor(PCI_NORMAL, BROWN);

	Rect = pWindow->mClient;
	Rect.wLeft += (pWindow->mClient.Width() >> 4);
	Rect.wTop += (pWindow->mClient.Height() >> 4);
	Rect.wRight = Rect.wLeft + 150;
	Rect.wBottom = Rect.wTop + 50;
	PegDecoratedButton* pButton = new PegDecoratedButton(Rect, "Text Only", NULL);
	pWindow->Add(pButton);

	Rect.Shift(175, 0);
	pButton = new PegDecoratedButton(Rect, NULL, &gbRedDotBitmap);
	pWindow->Add(pButton);

	Rect.Shift(175, 0);
	pButton = new PegDecoratedButton(Rect, "Text and Bitmap", &gbRedDotBitmap);
	pWindow->Add(pButton);

	Rect.Shift(-175, 75);
	pButton = new PegDecoratedButton(Rect, "Text on the Bottom", &gbGreenDotBitmap,
									 0, AF_ENABLED, DBF_ORIENT_TR, TRUE);
	pWindow->Add(pButton);

	Rect.Shift(-150, 50);
	pButton = new PegDecoratedButton(Rect, "Text on the Right", &gbGreenDotBitmap,
									 0, AF_ENABLED, 0, TRUE);
	pWindow->Add(pButton);

	Rect.Shift(150, 50);
	pButton = new PegDecoratedButton(Rect, "Text on the Top", &gbGreenDotBitmap,
									 0, AF_ENABLED, DBF_ORIENT_BR, TRUE);
	pWindow->Add(pButton);

	Rect.Shift(150, -50);
	pButton = new PegDecoratedButton(Rect, "Text on the Left", &gbGreenDotBitmap,
									 0, AF_ENABLED, DBF_ORIENT_TR | DBF_ORIENT_BR,
									 TRUE);
	pWindow->Add(pButton);

	// Now add some buttons to a toolbar panel. we'll just
	// let them size themselves and have the panel put them
	// where it wants them
	PegToolBarPanel* pPanel = new PegToolBarPanel();
	pButton = new PegDecoratedButton(0, 0, "Stop", &gbStopBitmap, 0, AF_ENABLED,
									 0, TRUE);
	pPanel->Add(pButton);

	pButton = new PegDecoratedButton(0, 0, "Refresh", &gbRefreshBitmap, 0, AF_ENABLED,
								     0, TRUE);
	pPanel->Add(pButton);

	pButton = new PegDecoratedButton(0, 0, "New", &gbNewBitmap, 0, AF_ENABLED,
								     0, TRUE);
	pPanel->Add(pButton);

	pToolBar->AddPanel(pPanel);

	// Center the window and add it to the presentation manager
	pPresent->Center(pWindow);
	pPresent->Add(pWindow);
}
