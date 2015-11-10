/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// dbwin.cpp - Example PegDecoratedButtons window
//
// Author: Jim DeLisle
//
// Copyright (c) 1998-2002 Swell Software 
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
extern PegFont      Tahoma9R;
extern PegBitmap    gbGreenDotBitmap;
extern PegBitmap    gbRefreshBitmap;
extern PegBitmap    gbRedDotBitmap;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DecButtonWindow::DecButtonWindow(SIGNED iLeft, SIGNED iTop) :
    PegDecoratedWindow()
{
    mReal.Set(iLeft, iTop, iLeft + 510, iTop + 352);
    InitClient();

    SetColor(PCI_NORMAL, BROWN);

    Add(new PegTitle("PegDecoratedButton Example"));
    Add(new PegToolBar());
    
    PegRect ButtonRect = mClient;
    ButtonRect.wLeft += 10;
    ButtonRect.wTop += (mClient.Height() >> 2);
    ButtonRect.wRight = ButtonRect.wLeft + 150;
    ButtonRect.wBottom = ButtonRect.wTop + 50;
    mpDBText = new PegDecoratedButton(ButtonRect, "DB Text Button", NULL,
                                      IDB_BUTTON_TEXT);
    mpDBText->SetFont(&Tahoma9R);
    mpDBText->SetFlyOver(FALSE);

    ButtonRect.Shift(165, 0);
    mpDBBitmap = new PegDecoratedButton(ButtonRect, NULL, &gbGreenDotBitmap,
                                        IDB_BUTTON_BITMAP, AF_ENABLED,
                                        TRUE);
    mpDBBitmap->SetFont(&Tahoma9R);

    ButtonRect.Shift(165, 0);
    mpDBBoth = new PegDecoratedButton(ButtonRect, "Real Decorated Button", 
                                      &gbGreenDotBitmap, IDB_BUTTON_BOTH, 
                                      AF_ENABLED | BF_ORIENT_TR | BF_ORIENT_BR,
                                      TRUE);
    mpDBBoth->SetFont(&Tahoma9R);

    Add(mpDBText);
    Add(mpDBBitmap);
    Add(mpDBBoth);

    PegToolBarPanel* pPanel = new PegToolBarPanel();

    ButtonRect = mpDBText->mReal;
    ButtonRect.Shift(0, 75);

    PegDecoratedButton* pButton = new PegDecoratedButton(ButtonRect, "Text On Left",
                                                         &gbGreenDotBitmap, 0,
                                                         AF_ENABLED| BF_ORIENT_TR | BF_ORIENT_BR,
                                                         TRUE);
    Add(pButton);
    
    ButtonRect.Shift(165, 0);
    pButton = new PegDecoratedButton(ButtonRect, "Text On Right", &gbGreenDotBitmap, 0,
                                     AF_ENABLED, TRUE);
    Add(pButton);

    ButtonRect.Shift(165, 0);
    pButton = new PegDecoratedButton(ButtonRect, "Text On Top", &gbRefreshBitmap, 0,
                                     AF_ENABLED| BF_ORIENT_BR, TRUE);
    Add(pButton);

    ButtonRect.Shift(0, 75);
    pButton = new PegDecoratedButton(ButtonRect, "Text On Bottom", &gbRefreshBitmap, 0,
                                     AF_ENABLED| BF_ORIENT_TR, TRUE);
    Add(pButton);
    
    pPanel->Add(new PegDecoratedButton(0, 0, "Green", &gbGreenDotBitmap, 0, AF_ENABLED| BF_ORIENT_TR | BF_ORIENT_BR, TRUE));
    pPanel->Add(new PegDecoratedButton(0, 0, "Really Green", &gbGreenDotBitmap, 0, AF_ENABLED, TRUE));
    pPanel->Add(new PegDecoratedButton(0, 0, "Refresh", &gbRefreshBitmap, 0, AF_ENABLED, TRUE));
    ToolBar()->AddPanel(pPanel);

    pPanel = new PegToolBarPanel();
    pPanel->Add(new PegDecoratedButton(0, 0, "Confused", NULL, 0, AF_ENABLED, TRUE));
    pPanel->Add(new PegDecoratedButton(0, 0, "Red", &gbRedDotBitmap, 0, AF_ENABLED, TRUE));
    pPanel->Add(new PegDecoratedButton(0, 0, "Really Red", &gbRedDotBitmap, 0, AF_ENABLED| BF_ORIENT_TR | BF_ORIENT_BR, TRUE));
    pPanel->Add(new PegDecoratedButton(0, 0, NULL, &gbRedDotBitmap, 0, AF_ENABLED, TRUE));
    ToolBar()->AddPanel(pPanel);

}
