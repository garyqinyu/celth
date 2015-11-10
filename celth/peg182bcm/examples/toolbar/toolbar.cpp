/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  Author: Jim DeLisle
//  
//    Copyright (c) 1997-2002, Swell Software, Inc.
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#include "toolbar.hpp"
#include "nomenu.hpp"

#include <string.h>

extern PegBitmap gbBullsEyeBitmap;
extern PegBitmap gbBlueDotBitmap;
extern PegBitmap gbGreyDotBitmap;
extern PegBitmap gbGreenDotBitmap;
extern PegBitmap gbRedDotBitmap;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ToolBarWindow::ToolBarWindow(SIGNED iLeft, SIGNED iTop) : 
    PegDecoratedWindow()
{
    static PegMenuDescription FileMenu [] = {
    {"Exit", IDB_CLOSE,  AF_ENABLED, NULL},
    {"", 0, 0, NULL}
    };
    static PegMenuDescription TopMenu [] = {
    {"File",  0,  AF_ENABLED, FileMenu},
    {"", 0, 0, NULL}
    };

    mReal.Set(iLeft, iTop, iLeft + 475, iTop + 352);
    InitClient();

    SetScrollMode(WSM_VSCROLL);
    Add (new PegTitle("ToolBar Example Window", TF_SYSBUTTON|TF_MINMAXBUTTON|TF_CLOSEBUTTON));
    Add(new PegMenuBar(TopMenu));

    PegStatusBar *pStat = new PegStatusBar();
    Add(pStat);

    /* WB End Construction */

    PegToolBar* pToolBar = new PegToolBar();
    
    PegRect Rect;
    PegToolBarPanel* pPanel = new PegToolBarPanel();
    Rect.Set(0, 0, 70, 20);
    pPanel->Add(new PegTextButton(Rect, "Remove It ->", IDB_ALPHA_BUTTON));
    Rect.Set(0, 0, 20, 20);
    pPanel->AddToEnd(new PegTextButton(Rect, "B"));
    pToolBar->AddPanel(pPanel);
    
    Rect.Set(0, 0, 200, 20);
    pPanel = new PegToolBarPanel(IDC_STRING_PANEL);
    pPanel->Add(new PegString(Rect, "String on a ToolBarPanel"));
    pToolBar->AddPanel(pPanel);

    Rect.Set(0, 0, 19, 19);
    pPanel = new PegToolBarPanel();
    pPanel->Add(new PegBitmapButton(Rect, &gbBullsEyeBitmap, IDB_BULL_BUTTON), FALSE);
    pPanel->Add(new PegBitmapButton(Rect, &gbBlueDotBitmap, IDB_BLUE_BUTTON), FALSE);
    pPanel->Add(new PegBitmapButton(Rect, &gbGreyDotBitmap, IDB_GREY_BUTTON), FALSE);
    pPanel->Add(new PegBitmapButton(Rect, &gbGreenDotBitmap, IDB_GREEN_BUTTON), FALSE);
    pPanel->Add(new PegBitmapButton(Rect, &gbRedDotBitmap, IDB_RED_BUTTON), FALSE);
    pToolBar->AddPanel(pPanel);

    Add(pToolBar); 
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED ToolBarWindow::Message(const PegMessage &Mesg)
{
    static PegThing* pThing = NULL;

    switch (Mesg.wType)
    {
    case SIGNAL(IDB_ALPHA_BUTTON, PSF_CLICKED):
        {
            // Remove the string panel
            PegThing* pPanel = Find(IDC_STRING_PANEL);
            PegTextButton* pButton = (PegTextButton*) Find(IDB_ALPHA_BUTTON);
            if(pPanel)
            {
                pThing = ToolBar()->RemovePanel(pPanel);
                if(pButton)
                {
                    pButton->DataSet("Add It ->");
                    pButton->Draw();
                }
            }
            else
            {
                ToolBar()->AddPanel((PegToolBarPanel*) pThing);
                pThing = NULL;
                if(pButton)
                {
                    pButton->DataSet("Remove It ->");
                    pButton->Draw();
                }
            }

        } break;

    case SIGNAL(IDB_BULL_BUTTON,  PSF_CLICKED):
        DisplayMessageWindow(1);
        break;

    case SIGNAL(IDB_RED_BUTTON,   PSF_CLICKED):
        DisplayMessageWindow(2);
        break;

    case SIGNAL(IDB_GREY_BUTTON,  PSF_CLICKED):
        DisplayMessageWindow(3);
        break;

    case SIGNAL(IDB_BLUE_BUTTON,  PSF_CLICKED):
        DisplayMessageWindow(4);
        break;

    case SIGNAL(IDB_GREEN_BUTTON, PSF_CLICKED):
        DisplayMessageWindow(5);
        break;

    case SIGNAL(IDB_CLOSE, PSF_CLICKED):
        // Enter your code here:
        if(pThing)
        {
            delete pThing;
        }
        //break;
    default:
        return PegDecoratedWindow::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void ToolBarWindow::DisplayMessageWindow(WORD wForWho)
{
    PEGCHAR cBuffer[40], cWho[10];
    switch(wForWho)
    {
    case 1:
        strcpy(cWho, " BullsEye ");
        break;

    case 2:
        strcpy(cWho, " Red ");
        break;

    case 3:
        strcpy(cWho, " Grey ");
        break;

    case 4:
        strcpy(cWho, " Blue ");
        break;

    case 5:
        strcpy(cWho, " Green ");
        break;
    }

    strcpy(cBuffer, "You've pressed the");
    strcat(cBuffer, cWho);
    strcat(cBuffer, "button.");
    PegMessageWindow* pWindow = new PegMessageWindow("Message Received",
                                                     cBuffer);
    Presentation()->Center(pWindow);
    Presentation()->Add(pWindow);
    pWindow->Execute();
}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager* pPresent)
{
    pPresent->Add(new NoMenuWindow(100, 40));
    pPresent->Add(new ToolBarWindow(20, 60));
}


