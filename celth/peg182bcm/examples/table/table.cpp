/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// table.cpp - Demonstration of PegTable class.
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
#include "table.hpp"
#include "stdio.h"

extern PegBitmap gbBitmap1;
extern PegBitmap gbBitmap2;
extern PegBitmap gbBlurLogoBitmap;

/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresent)
{
    PegRect WinRect;
    WinRect.Set(0, 0, 400, 300);
    TableFrameWin *pFrame = new TableFrameWin(WinRect);
    pPresent->Center(pFrame);
    pPresent->Add(pFrame);
}


/*--------------------------------------------------------------------------*/
// TabWindow:: This is the frame window around the table. This frame
// adds an instance of PegTable to itself, and also provides the menu
// and title bars.
/*--------------------------------------------------------------------------*/
TableFrameWin::TableFrameWin(const PegRect &Where) : PegDecoratedWindow(Where)
{
    static PegMenuDescription GridMenu[] = {
        {"Width",  IDB_GRID_WIDTH, AF_ENABLED, NULL},
        {"", 0, 0, NULL}
    };

    static PegMenuDescription HelpMenu[] = {
        {"About",  IDB_HELP_ABOUT, AF_ENABLED, NULL},
        {"", 0, 0, NULL}
    };

    static PegMenuDescription MainMenu[] = {
        {"Help",  0, AF_ENABLED, HelpMenu},
        {"Gridlines", 0, AF_ENABLED,  GridMenu},
        {"", 0, 0, NULL}
    };

    SetColor(PCI_NORMAL, LIGHTGRAY);
    PegRect Frame;

    Add(new PegTitle("PegTable Example"));

    Add (new PegMenuBar(MainMenu));

    Frame = mClient;
    Frame -= 10;
 
    mpTable = new PegTable(Frame.wLeft, Frame.wTop, 5, 5);
    mpTable->Style(mpTable->Style() | AF_TRANSPARENT);
    
    mpTable->Style(mpTable->Style() & ~TS_DRAW_GRID);
    mpTable->Style(mpTable->Style() | TS_DRAW_HORZ_GRID);
    
    mpTable->RemoveStatus(PSF_VIEWPORT);

    AddTableClients();   // add the client object to the table cells
    mpTable->Layout();
    Add(mpTable);
}


/*--------------------------------------------------------------------------*/
SIGNED TableFrameWin::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(IDB_HELP_ABOUT, PSF_CLICKED):
        DisplayHelpWindow();
        break;

    case SIGNAL(IDB_TBUTTON, PSF_CLICKED):
        {
            SIGNED iRow, iCol;
            mpTable->GetRowCol(Mesg.pSource, &iRow, &iCol);
            mpTable->RemoveCellClient(iRow, iCol);

            if (iRow == 2)
            {
                iRow = 3;
            }
            else
            {
                iRow = 2;
            }
            mpTable->SetCellClient(iRow, iCol, Mesg.pSource, 1, 1, TRUE);
            mpTable->Layout();
            mpTable->Draw();
        }
        break;

    default:
        PegDecoratedWindow::Message(Mesg);
        break;
    }
    return 1;
}

/*--------------------------------------------------------------------------*/
void TableFrameWin::Draw(void)
{
    BeginDraw();
    DrawFrame();
    BitmapFill(mClient, &gbBlurLogoBitmap);
    DrawChildren();
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void TableFrameWin::DisplayHelpWindow(void)
{
    PegMessageWindow *pWin = new PegMessageWindow("PegDemo About",
        Version(), MW_OK|FF_RAISED);

    Presentation()->Center(pWin);
    Presentation()->Add(pWin);
    pWin->Execute();
}

/*--------------------------------------------------------------------------*/
void TableFrameWin::AddTableClients(void)
{
    // put the PEG icon in the top left, spanning 3 columns:
    PegIcon *pIcon = new PegIcon(&gbBitmap1);
    mpTable->SetCellClient(0, 0, pIcon, 1, 3, TRUE);

    // add a few miscellaneous objects:
    PegRect Size;
    Size.Set(0, 0, 70, 40);    
    PegTextButton *pButton = new PegTextButton(Size, "PushMe", IDB_TBUTTON);
    mpTable->SetCellClient(2, 1, pButton, 1, 1, TRUE);

    pButton = new PegTextButton(Size, "PushMe", IDB_TBUTTON);
    mpTable->SetCellClient(3, 2, pButton, 1, 1, TRUE);

    // put some prompts in column 4, rows 0-3:

    PegPrompt *pp = new PegPrompt(0, 0, "Test Prompt 1", 0, FF_NONE|TJ_CENTER|AF_TRANSPARENT);
    mpTable->SetCellClient(0, 4, pp, 1, 1, FALSE);
    pp = new PegPrompt(0, 0, "Test Prompt 2", 0, FF_NONE|TJ_CENTER|AF_TRANSPARENT);
    mpTable->SetCellClient(1, 4, pp, 1, 1, FALSE);
    pp = new PegPrompt(0, 0, "Test Prompt 3", 0, FF_NONE|TJ_CENTER|AF_TRANSPARENT);
    mpTable->SetCellClient(2, 4, pp, 1, 1, FALSE);




    // put a group in the bottom right cell
    Size.Set(0, 0, 120, 100);
    PegGroup *pGroup = new PegGroup(Size, "CellGroup");
    pGroup->Add(new PegRadioButton(10, 15, "Radio1"));
    pGroup->Add(new PegRadioButton(10, 35, "Radio2"));
    pGroup->Add(new PegRadioButton(10, 55, "Radio3"));
    mpTable->SetCellClient(4, 4, pGroup, 1, 1, TRUE);
    
    // put the cartoon in the lower right, spanning 3 rows:
    pIcon = new PegIcon(&gbBitmap2);
    mpTable->SetCellClient(1, 3, pIcon, 3, 1, TRUE);
}

