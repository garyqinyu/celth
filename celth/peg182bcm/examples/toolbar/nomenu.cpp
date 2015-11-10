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
#include "nomenu.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
NoMenuWindow::NoMenuWindow(SIGNED iLeft, SIGNED iTop) : 
    PegDecoratedWindow()
{

    mReal.Set(iLeft, iTop, iLeft + 500, iTop + 352);
    InitClient();
    SetScrollMode(WSM_VSCROLL);
    Add (new PegTitle("Big Things on Toolbars", TF_SYSBUTTON|TF_MINMAXBUTTON|TF_CLOSEBUTTON));

    PegStatusBar *pStat = new PegStatusBar();
    Add(pStat);

    /* WB End Construction */

    PegToolBar* pToolBar = new PegToolBar();

    PegRect Rect;
    
    PegToolBarPanel* pPanel = new PegToolBarPanel();

    Rect.Set(0, 0, 50, 20);
    pPanel->Add(new PegPrompt(Rect, "50", IDC_SLIDER_VAL, 
                              FF_RECESSED | TJ_CENTER | AF_TRANSPARENT | TT_COPY));

    Rect.Set(0, 0, 150, 50);
    PegSlider* pSlider = new PegSlider(Rect, 0, 100, IDC_SLIDER, FF_RECESSED, 10);
    pSlider->SetCurrentValue(50);
    pPanel->Add(pSlider);    

    pToolBar->AddPanel(pPanel);
    
    Rect.Set(0, 0, 75, 20);
    pPanel = new PegToolBarPanel();
    pPanel->Add(new PegRadioButton(Rect, "Option 3"));
    pPanel->Add(new PegRadioButton(Rect, "Option 2"));
    pPanel->Add(new PegRadioButton(Rect, "Option 1"));
    
    pToolBar->AddPanel(pPanel);

    Add(pToolBar);    
}

/*--------------------------------------------------------------------------*/
SIGNED NoMenuWindow::Message(const PegMessage& Mesg)
{
    switch(Mesg.wType)
    {
    case SIGNAL(IDC_SLIDER, PSF_SLIDER_CHANGE):
        {
            PEGCHAR cBuffer[10];
            ltoa(Mesg.lData, cBuffer, 10);

            PegPrompt* pPrompt = (PegPrompt*) Find(IDC_SLIDER_VAL);
            if(pPrompt)
            {
                pPrompt->DataSet(cBuffer);
                pPrompt->Draw();
            }
        }
        break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }

    return 0;
}

