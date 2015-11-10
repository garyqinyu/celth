/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// notebook.cpp - PegNotebook demonstration window.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-1999 Swell Software 
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

#include "notebook.hpp"
#include "graphtab.hpp"
#include "stdio.h"


/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresent)
{
    PegRect WinRect;
    WinRect.Set(0, 0, 400, 300);
    TabWindow *pTab = new TabWindow(WinRect);
//    pPresent->Center(pTab);
    pPresent->Add(pTab);
}


/*--------------------------------------------------------------------------*/
// TabWindow:: This is the frame window around the PegNotebook. This frame
// adds an instance of PegNotebook to itself, and also provides the menu
// and title bars.
/*--------------------------------------------------------------------------*/
TabWindow::TabWindow(const PegRect &Where) : PegDecoratedWindow(Where)
{
    static PegMenuDescription StyleMenu[] = {
        {"Client Fill",   IDB_CLIENT_FILL,  AF_ENABLED|BF_DOTABLE, NULL},
        {"Dialog Fill",   IDB_DIALOG_FILL,  AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
        {"", 0, BF_SEPERATOR, NULL},
        {"Raised Border", IDB_RAISED_BORDER,AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
        {"Thick Border",  IDB_THICK_BORDER, AF_ENABLED|BF_DOTABLE, NULL},
        {"", 0, BF_SEPERATOR, NULL},
        {"Top Tabs",      IDB_TOP_TABS,     AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
        {"Bottom Tabs",    IDB_BOTTOM_TABS,  AF_ENABLED|BF_DOTABLE, NULL},
        {"", 0, BF_SEPERATOR, NULL},
        {"Text Tabs",     IDB_TEXT_TABS,    AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
        {"Custom Tabs",   IDB_CUSTOM_TABS,  AF_ENABLED|BF_DOTABLE, NULL},
        {"", 0, 0, NULL}
    };

    static PegMenuDescription MainMenu[] = {
        {"Style", 0, AF_ENABLED, StyleMenu},
        {"", 0, 0, NULL}
    };

    SetColor(PCI_NORMAL, LIGHTGRAY);
    PegRect Frame;

    Add(new PegTitle("PegNotebook Example"));

    Add (new PegMenuBar(MainMenu));

    Frame = mClient;
    Frame -= 10;
    
    mpNotebook = new PegNotebook(Frame, FF_RAISED|NS_TEXTTABS|NS_TOPTABS,
        NUM_TABS);

    #ifdef PEG_NUM_COLORS
    #if (PEG_NUM_COLORS >= 16)
    mpNotebook->SetColor(PCI_SELECTED, BLUE);
    mpNotebook->SetColor(PCI_STEXT, WHITE);
    #endif
    #endif

    Add(mpNotebook);        // add the notebook control to myself

    SetNotebookClients();   // add the client windows to the notebook
    miPromptVal = 0;
}


/*--------------------------------------------------------------------------*/
// Override the Message function to catch the menu commands.
/*--------------------------------------------------------------------------*/
SIGNED TabWindow::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case PM_SHOW:
        PegDecoratedWindow::Message(Mesg);
        SetTimer(1, 2, 2);
        break;

    case PM_HIDE:
        //KillTimer(0);
        return PegDecoratedWindow::Message(Mesg);

    case PM_TIMER:
        //UpdatePrompt();
        break;

    case SIGNAL(IDB_RAISED_BORDER, PSF_DOT_ON):
        SetNotebookStyle(FF_RAISED, FF_THICK);
        break;

    case SIGNAL(IDB_THICK_BORDER, PSF_DOT_ON):
        SetNotebookStyle(FF_THICK, FF_RAISED);
        break;

    case SIGNAL(IDB_TOP_TABS, PSF_DOT_ON):
        SetNotebookStyle(NS_TOPTABS, NS_BOTTOMTABS);
        break;

    case SIGNAL(IDB_BOTTOM_TABS, PSF_DOT_ON):
        SetNotebookStyle(NS_BOTTOMTABS, NS_TOPTABS);
        break;

    case SIGNAL(IDB_TEXT_TABS, PSF_DOT_ON):
        SetTextTabs();
        break;

    case SIGNAL(IDB_CUSTOM_TABS, PSF_DOT_ON):
        SetCustomTabs();
        break;

    case SIGNAL(IDB_DIALOG_FILL, PSF_DOT_ON):
        mpNotebook->SetColor(PCI_NORMAL, PCLR_DIALOG);
        Invalidate(mpNotebook->mClient);
        mpNotebook->Draw();
        break;

    case SIGNAL(IDB_CLIENT_FILL, PSF_DOT_ON):
        mpNotebook->SetColor(PCI_NORMAL, PCLR_CLIENT);
        Invalidate(mpNotebook->mClient);
        mpNotebook->Draw();
        break;

    default:
        PegDecoratedWindow::Message(Mesg);
        break;
    }
    return 1;
}


/*--------------------------------------------------------------------------*/
void TabWindow::UpdatePrompt(void)
{
    char cTemp[100];
    miPromptVal++;
    ltoa(miPromptVal, cTemp, 10);
    mpPrompt->DataSet(cTemp);
    mpPrompt->Draw();
}

/*--------------------------------------------------------------------------*/
void TabWindow::SetNotebookStyle(WORD wOrFlag, WORD wAndFlag)
{
    Remove(mpNotebook);
    WORD wCurrent = mpNotebook->Style();
    wCurrent &= ~wAndFlag;
    wCurrent |= wOrFlag;
    mpNotebook->Style(wCurrent);
    Add(mpNotebook);
}

/*--------------------------------------------------------------------------*/
void TabWindow::SetNotebookClients(void)
{
    char cTemp[20];

    for (UCHAR uLoop = 0; uLoop < NUM_TABS; uLoop++)
    {
        sprintf(cTemp, "TabText%d", uLoop);
        mpNotebook->SetTabString(uLoop, cTemp);
    }

    PegRect ChildRect = mpNotebook->mClient;
    ChildRect -= 4;
    
    // make a PegTextBox for the first client:

    char EditText[] = "This is a PegNotebook window. The notebook tabs can be "
        "simple text, or custom objects. Each page of the notebook is populated "
        "with any type of PegThing, in this case a PegTextBox.";

    PegTextBox *pEdit = new PegTextBox(ChildRect, 0, FF_RECESSED|TJ_LEFT|TT_COPY|EF_WRAP);
    pEdit->DataSet(EditText);
    mpNotebook->SetPageClient(1, pEdit);
    
    // make a group for the second client:

    PegGroup *pGroup = new PegGroup(ChildRect, "Group Client");
    //PegWindow *pWin = new PegWindow(ChildRect, FF_NONE);

    SIGNED iTop = pGroup->mClient.wTop + 10;
    pGroup->Add(new PegPrompt(pGroup->mClient.wLeft + 10, iTop, "PegGroup populated with checkboxes..."));
    iTop += 24;
    SIGNED iFlag = 0;

    while (iTop < pGroup->mClient.wBottom - 40)
    {
        if (iFlag & 1)
        {
            pGroup->Add(new PegCheckBox(pGroup->mClient.wLeft + 10,
                iTop, "CheckBox"));
        }
        else
        {
            pGroup->Add(new PegCheckBox(pGroup->mClient.wLeft + 10,
                iTop, "CheckBox", 0, AF_ENABLED|BF_SELECTED));
        }
        iFlag++;
        iTop += 24;
    }

    mpNotebook->SetPageClient(0, pGroup);

    // make a different group for the third client:

    pGroup = new PegGroup(ChildRect, "Group Client");
    iTop = pGroup->mClient.wTop + 10;
    pGroup->Add(new PegPrompt(pGroup->mClient.wLeft + 10, iTop, "PegGroup populated with radio buttons..."));
    iTop += 24;

    iFlag = 0;

    while (iTop < pGroup->mClient.wBottom - 40)
    {
        if (!iFlag)
        {
            pGroup->Add(new PegRadioButton(pGroup->mClient.wLeft + 10, iTop,
                 "Radio Button", 0, AF_ENABLED|BF_SELECTED));
        }
        else
        {
            pGroup->Add(new PegRadioButton(pGroup->mClient.wLeft + 10, iTop, "Radio Button"));
        }
        iTop += 24;
        iFlag++;
    }

    iTop = pGroup->mClient.wTop + 34;
    mpPrompt = new PegPrompt(pGroup->mClient.wLeft + 140, iTop,
        "00000000", 0, FF_RECESSED|TT_COPY|TJ_RIGHT);
    mpPrompt->SetColor(PCI_NORMAL, BLACK);
    mpPrompt->SetColor(PCI_NTEXT, LIGHTGREEN);
    pGroup->Add(mpPrompt);

    mpNotebook->SetPageClient(2, pGroup);

    // make a window with two text boxes for the final client:

    //PegWindow *pChild = new PegWindow(ChildRect, FF_NONE);
    PegWindow *pChild = new TestWindow(ChildRect, FF_NONE);
    pChild->SetColor(PCI_NORMAL, PCLR_DIALOG);
    ChildRect.wBottom = ChildRect.wTop + ChildRect.Height() / 2;
    
    pEdit = new PegTextBox(ChildRect);
    pEdit->DataSet("Each Notebook client can of course also have multiple children. In "
        "this case, we have created a frameless window as the notebook page client. "
        "This allows us to add scrolling, and populate the notebook page with objects "
        "that are larger than the notebook itself!");
    pChild->Add(pEdit);

    ChildRect.wTop = ChildRect.wBottom + 10;
    ChildRect.wBottom = mpNotebook->mClient.wBottom - 40;
    pEdit = new PegTextBox(ChildRect);
    pEdit->DataSet("To the frameless window we have added two PegTextBoxes. " 
        "Any other type of object could be used just as easily");
    pChild->Add(pEdit);
    mpNotebook->SetPageClient(3, pChild);

    //mpNotebook->SelectTab(1);
}

TestWindow::TestWindow(const PegRect &Size, WORD wStyle) :
    PegWindow(Size, wStyle)
{
}

void TestWindow::Draw(void)
{
    PegWindow::Draw();
}

/*--------------------------------------------------------------------------*/
// SetTextTabs: This function sets the notebook tabs back to the standard
// text mode, and puts some text on each tab.
//
// Note: The PegNotebook only does a layout when it is 'shown'. For this
// reason we have to Remove the notebook, modify it, and then re-Add it.
/*--------------------------------------------------------------------------*/
void TabWindow::SetTextTabs(void)
{
    Remove(mpNotebook);
    mpNotebook->ResetTabStyle(NS_TEXTTABS);
    
    char cTemp[20];

    for (UCHAR uLoop = 0; uLoop < NUM_TABS; uLoop++)
    {
        sprintf(cTemp, "TabText%d", uLoop);
        mpNotebook->SetTabString(uLoop, cTemp);
    }
    Add(mpNotebook);
}

extern PegBitmap gbYellowGBitmap;
extern PegBitmap gbRedPBitmap;
extern PegBitmap gbGreenEBitmap;
extern PegBitmap gbBlueExBitmap;

/*--------------------------------------------------------------------------*/
// SetCustomTabs: This function sets the notebook tab style to !NS_TEXTTABS,
// and add instances of our newly created custom GraphicTab to each tab.
//
// Note: The PegNotebook only does a layout when it is 'shown'. For this
// reason we have to Remove the notebook, modify it, and then re-Add it.
/*--------------------------------------------------------------------------*/

void TabWindow::SetCustomTabs(void)
{
    Remove(mpNotebook);
    mpNotebook->ResetTabStyle(0);

    mpNotebook->SetTabClient(0, new GraphicTab(&gbRedPBitmap, "Page1"));
    mpNotebook->SetTabClient(1, new GraphicTab(&gbGreenEBitmap, "Page2"));
    mpNotebook->SetTabClient(2, new GraphicTab(&gbYellowGBitmap, "Page3"));
    mpNotebook->SetTabClient(3, new GraphicTab(&gbBlueExBitmap));

    Add(mpNotebook);
}