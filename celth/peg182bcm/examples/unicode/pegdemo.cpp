/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pegdemo.cpp - Example program that makes use of a lot of the basic
//  PEG control and window types.
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
//  This example has not been completed. Contact Swell Software to
//  receive an updated version of the PEG library..
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include "peg.hpp"
#include "pegdemo.hpp"
#include "wbstring.hpp"

extern PegFont MonsterFont;
extern PegFont Script;
extern PegFont UniFont;
extern PegBitmap gbPegGlobeBitmap;
extern PEGCHAR lsTEST[];
extern UCHAR gbCurrentLanguage;

PEGCHAR *OldLS(WORD wId);

/****************************************************************************/
/*--------------------------------------------------------------------------*/
// This function is called by the PegTask implementation, regardless of which
// platform we are running on.
/*--------------------------------------------------------------------------*/

void PegAppInitialize(PegPresentationManager *pPresentation)
{
    // create some simple Windows:

    PegRect Rect;

    Rect.Set(0, 0, 288, 220);
    PegThing *NewWin = new MainDemoDialog(Rect, LS(SID_DIALOG));
    pPresentation->Center(NewWin);
    pPresentation->Add(NewWin);

    Rect.Set(4, 450, 74, 475);
    pPresentation->Add(new SwapLanguageButton(Rect));
}


/*************************** MAIN DIALOG WINDOW *****************************/
/*--------------------------------------------------------------------------*/
// The following strings are displayed in the small demo dialog window. They
// provide information to the reader about each control/window displayed as
// part of the demo.
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
MainDemoDialog:: MainDemoDialog(const PegRect &InRect, const PEGCHAR *Title) :
    PegDialog(InRect, Title)
{
    Id(ID_MAIN_DIALOG);

    PegRect Rect = mClient;
    Rect.wLeft += 10;
    Rect.wRight -= 10;
    Rect.wTop += 10;
    Rect.wBottom -= Rect.Height() / 3;
    Rect.wBottom -= 4;

    mpTextBox = new PegTextBox(Rect, 0, FF_RECESSED|EF_WRAP);
    mpTextBox->SetScrollMode(WSM_AUTOVSCROLL|WSM_CONTINUOUS);
    mpTextBox->DataSet(LS(SID_DEM_MSG1));

    Add(mpTextBox);

    Rect.wTop = Rect.wBottom + 10;
    Rect.wBottom = Rect.wTop + 26;
    Rect.wLeft += 70;
    Rect.wRight -= 70;

    mpContinueButton = new PegTextButton(Rect, LS(SID_CONTINUE), IDB_CONTINUE);
    mwStep = 0;
    Add(mpContinueButton);
    mbComplete = FALSE;
}

/*--------------------------------------------------------------------------*/
SIGNED MainDemoDialog::Message(const PegMessage &Mesg)
{
    PegRect Rect;
    PegMessage NewMessage;

    switch (Mesg.wType)
    {
    case SIGNAL(IDB_CONTINUE, PSF_CLICKED):

        mwStep++;
        mpTextBox->RewindDataSet(LS(SID_DEM_MSG1 + mwStep));

        switch(mwStep)
        {
        case 1:
        case 2:
        case 3:
        default:
            mpTextBox->Draw();
            break;

        case 4:
            Rect = mpContinueButton->mReal;
            Rect.Shift(-70, 0);
            mpContinueButton->Resize(Rect);
            Draw();
            break;

        case 5:
            mpTextBox->Draw();
            Rect.wTop = mpTextBox->mReal.wBottom + 4;
            Rect.wBottom = mClient.wBottom - 4;
            Rect.wLeft = mClient.wLeft + 4;
            Rect.wRight = mClient.wRight - 4;
            mpContinueButton->Resize(Rect);
            mpContinueButton->Draw();
            break;

        case 6:
            // make the continue button normal size:

            Rect.wTop = mpTextBox->mReal.wBottom + 10;
            Rect.wBottom = Rect.wTop + 26;
            Rect.wLeft = mpTextBox->mReal.wLeft + 60;
            Rect.wRight = mpTextBox->mReal.wRight - 60;
            mpContinueButton->Resize(Rect);

            // put myself in the upper right corner:

            Rect.wRight = Presentation()->mClient.wRight - 5;
            Rect.wLeft = Rect.wRight - mReal.Width() + 1;
            Rect.wTop = 10;
            Rect.wBottom = Rect.wTop + mReal.Height() - 1;
            Resize(Rect);
            Parent()->Draw();

            // create the control panel window:

            Rect.Set(10, 10, 320, 240);
            Presentation()->Add(new ControlPanelWindow(Rect, FF_THIN));
            break;

        case 7:
        case 8:
        case 9:
            mpTextBox->Draw();
            CloseLastWindow();

            // create the next window:
    
            Rect.Set(10, 10, 320, 0);
            
            switch(mwStep)
            {
            case 7:
                Rect.wBottom = 280;
                Presentation()->Add(new DemoListWindow(Rect, LS(SID_PEGLIST))); // "Example PEG List Types"));
                break;

            case 8:
                Rect.wBottom = 460;
                Presentation()->Add(new DemoSliderWindow(Rect, LS(SID_SLIDER)));
                break;

            case 9:
                Rect.wBottom = 180;
                Presentation()->Add(new ProgBarWindow(Rect, LS(SID_PROGBAR)));
                break;
            }
            break;

        case 10:
            mpTextBox->Draw();
            CloseLastWindow();

            // create the string and text box window:

            Rect.Set(10, 10, 320, 280);
            Presentation()->Add(new StringWindow(Rect, LS(SID_EDITWIN)));
            break;

        case 11:
            mpTextBox->Draw();
            CloseLastWindow();
            mpContinueButton->DataSet(LS(SID_CLOSE));
            mpContinueButton->Draw();

            // create the decorated window:

            Rect.Set(10, 10, 520, 326);
            Presentation()->AddToEnd(new MainAppWindow(Rect, LS(SID_DECWIN)));
            mbComplete = TRUE;
            break;

        case 12:
            NewMessage.wType = PM_CLOSE;
            NewMessage.pTarget = this;
            MessageQueue()->Push(NewMessage);
            break;
        }
        break;

    case PM_CLOSE:
        if (mbComplete)
        {
            PegDialog::Message(Mesg);
        }
        else
        {
            PegMessageWindow *mw = new PegMessageWindow(LS(SID_EARLY),
                LS(SID_MISS_DEMO),
                MW_YES|MW_NO|FF_RAISED);

            Presentation()->Add(mw);

            if (mw->Execute() == IDB_YES)
            {
                // make sure we don't leave any other windows hanging around

                PegThing *pTest = Presentation()->First();

                while(pTest)
                {
                    if (pTest != this)
                    {
                        Destroy(pTest);
                        pTest = Presentation()->First();
                    }
                    else
                    {
                        pTest = pTest->Next();
                    }
                }
                PegDialog::Message(Mesg);
            }
        }
        break;

    case UM_LANGUAGE_CHANGE:
        mpTextBox->DataSet(NULL);
        mpTextBox->SetFont(PegTextThing::GetDefaultFont(PEG_TEXTBOX_FONT));
        mpTextBox->RewindDataSet(LS(SID_DEM_MSG1 + mwStep));
        mpContinueButton->SetFont(PegTextThing::GetDefaultFont(PEG_TBUTTON_FONT));
        mpContinueButton->DataSet(LS(SID_CONTINUE));
        Invalidate();
        Draw();
        CloseLastWindow();
        mwStep--;

        if (mwStep == 3)
        {
            Rect = mpContinueButton->mReal;
            Rect.Shift(70, 0);
            mpContinueButton->Resize(Rect);
        }
        Message(SIGNAL(IDB_CONTINUE, PSF_CLICKED));        
        break;
    
    default:
        return(PegDialog::Message(Mesg));
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
void MainDemoDialog::CloseLastWindow(void)
{
    PegThing *pTest;

    pTest = Presentation()->First();
    while(pTest)
    {
        // Find a window with no ID that is not me and is not an Icon.
        // This needs to be done this way because the RTOS integration
        // demos create extra windows. Those windows have Id values, and
        // we don't want to close them!

        if (pTest->Type() != TYPE_ICON && pTest != this && !pTest->Id())
        {
            Destroy(pTest);
            break;
        }
        pTest = pTest->Next();
    }
}

/*************************** MAIN WINDOW ************************************/

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MainAppWindow::MainAppWindow (PegRect Rect, const PEGCHAR *Title, BOOL bFirst) :
    PegDecoratedWindow(Rect)
{

/*--------------------------------------------------------------------------*/
// The following menu descriptions are used for the main demo window, which
// is the last window displayed in the demo.
/*--------------------------------------------------------------------------*/

PegMenuDescription ClientColorOptions[] = {
{LS(SID_BLACK), IDB_SET_COLOR_BLACK, AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_WHITE), IDB_SET_COLOR_WHITE, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{LS(SID_BLUE),  IDB_SET_COLOR_BLUE, AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_DARKGRAY), IDB_SET_COLOR_DARKGRAY, AF_ENABLED|BF_DOTABLE, NULL},
{NULL, 0, 0, NULL}
};

PegMenuDescription DeskColorOptions[] = {
{LS(SID_BLACK), IDB_DESK_COLOR_BLACK, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{LS(SID_RED),   IDB_DESK_COLOR_RED,   AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_GREEN), IDB_DESK_COLOR_GREEN, AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_BROWN), IDB_DESK_COLOR_BROWN, AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_MAGENTA), IDB_DESK_COLOR_MAGENTA, AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_CYAN),  IDB_DESK_COLOR_CYAN,  AF_ENABLED|BF_DOTABLE, NULL},
{NULL, 0, 0, NULL}
};

PegMenuDescription PaperOptions[] = {
{LS(SID_SAND), IDB_SET_SAND_PAPER, AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_POOL),  IDB_SET_POOL_PAPER, AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_NONE),  IDB_SET_NO_PAPER, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{NULL, 0, 0, NULL}
};

PegMenuDescription WindowOptions[] = {
{LS(SID_SIZEABLE), IDB_SET_SIZEABLE, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
{LS(SID_MOVEABLE), IDB_SET_MOVEABLE, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
{NULL, 0, 0, NULL}
};

PegMenuDescription OptionsMenu[] = {
{LS(SID_PAPER), 0, AF_ENABLED, PaperOptions},
{LS(SID_DESKTOP), 0, AF_ENABLED, DeskColorOptions},
{LS(SID_STYLE), 0, AF_ENABLED, WindowOptions},
{LS(SID_CLNT_CLR), 0, AF_ENABLED, ClientColorOptions},
{NULL, 0, 0, NULL}
};

PegMenuDescription FileMenu[] = {
{LS(SID_EXIT), IDB_DEMO_EXIT, AF_ENABLED, NULL},
{LS(SID_CLOSE), IDB_CLOSE, AF_ENABLED, NULL},
{LS(SID_SAVE), 0, 0, NULL},
{NULL, 0, BF_SEPERATOR, NULL},
{LS(SID_RESTORE), IDB_RESTORE, AF_ENABLED, NULL},
{LS(SID_MINIMIZE), IDB_MINIMIZE, AF_ENABLED, NULL},
{LS(SID_MAXIMIZE), IDB_MAXIMIZE, AF_ENABLED, NULL},
{NULL, 0, 0, NULL}
};

PegMenuDescription WindowMenu[] = {

{LS(SID_BUTN_DLG), IDB_ADD_BUTTON_DIALOG, AF_ENABLED, NULL},
{LS(SID_PROGBAR), IDB_ADD_PROGWIN, AF_ENABLED, NULL},
{LS(SID_TWIN), IDB_ADD_TWIN, AF_ENABLED, NULL},
{LS(SID_CONTROLS),  IDB_ADD_CONTROL_WIN, AF_ENABLED|BF_CHECKABLE, NULL},
{NULL, 0, BF_SEPERATOR, NULL},
{LS(SID_FLOAT), IDB_ADD_FLOATING_CHILD, AF_ENABLED|BF_CHECKABLE, NULL},
{LS(SID_DRAW), IDB_ADD_CUSTOM_CHILD, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
{NULL, 0, 0, NULL}
};

PegMenuDescription HelpMenu[] = {
    {LS(SID_ABOUT),   IDB_HELP_ABOUT, AF_ENABLED, NULL},
    {NULL, 0, 0, NULL}
};

PegMenuDescription MainMenu[] = {
    {LS(SID_HELP),     0, AF_ENABLED, HelpMenu},
    {LS(SID_DISABLED), 1, 0, NULL},
    {LS(SID_WINDOWS), 0, AF_ENABLED, WindowMenu},
    {LS(SID_OPTIONS), 0, AF_ENABLED, OptionsMenu},
    {LS(SID_FILE), 0, AF_ENABLED, FileMenu},
    {NULL, 0, 0, NULL}
};

    Id(MAIN_APP_WINDOW);
    Add(new PegTitle(Title));

    // Disable the "Add Twin" menu button if I am already the second
    // instance of myself:

    PegMenuBar *pBar = new PegMenuBar(MainMenu);
    mpTwinButton = pBar->FindButton(LS(SID_TWIN));

    if (!bFirst && mpTwinButton)
    {
        mpTwinButton->SetEnabled(FALSE);
    }

    Add(pBar);      // add the menu bar

    PegStatusBar *pStat = new PegStatusBar();
    pStat->AddTextField(100, 0, LS(SID_FIXED_FIELD));
    pStat->AddTextField(100, 0, LS(SID_FIXED_FIELD));
    pStat->AddTextField(20, 0, LS(SID_VAR_FIELD));

    Add(pStat);     // add the status bar

    // add the line drawing child window:

    mpDemoWin = new DemoDrawWin();
    Add(mpDemoWin);
    mpControlWin = NULL;
    mpFloater = NULL;
}


/*--------------------------------------------------------------------------*/
MainAppWindow::~MainAppWindow()
{
    if (mpControlWin)
    {
        // If the user hits <ALT-F4> to close under windows, then the control
        // panel window could already be deleted. Check to be sure that it
        // still exists:

        PegThing *pTest = Presentation()->First();

        while(pTest)
        {
            if (pTest == mpControlWin)
            {
                Destroy(mpControlWin);
                break;
            }
            pTest = pTest->Next();
        }
    }
}

extern PegBitmap gbSandBitmap;
extern PegBitmap gbPoolBitmap;

/*--------------------------------------------------------------------------*/
SIGNED MainAppWindow::Message(const PegMessage &Mesg)
{
    PegMenuButton *pSource;
    PegMessage NewMessage;
    PegRect Rect;

    switch (Mesg.wType)
    {
     case PM_HIDE:
        PegDecoratedWindow::Message(Mesg);
        break;

    case SIGNAL(IDB_SET_COLOR_BLACK, PSF_DOT_ON):
        SetColor(PCI_NORMAL, BLACK);
        Invalidate();
        Draw();
        break;

    case SIGNAL(IDB_SET_SAND_PAPER, PSF_DOT_ON):
        Presentation()->SetWallpaper(&gbSandBitmap);
        Invalidate(Presentation()->mReal);
        Presentation()->Draw();
        break;

    case SIGNAL(IDB_SET_POOL_PAPER, PSF_DOT_ON):
        Presentation()->SetWallpaper(&gbPoolBitmap);
        Invalidate(Presentation()->mReal);
        Presentation()->Draw();
        break;

    case SIGNAL(IDB_SET_NO_PAPER, PSF_DOT_ON):
        Presentation()->SetWallpaper(NULL);
        Invalidate(Presentation()->mReal);
        Presentation()->Draw();
        break;

    case SIGNAL(IDB_DESK_COLOR_BLACK, PSF_DOT_ON):
        Presentation()->SetWallpaper(NULL);
        Presentation()->SetColor(PCI_NORMAL, BLACK);
        Invalidate(Presentation()->mReal);
        Presentation()->Draw();
        break;

    case SIGNAL(IDB_DESK_COLOR_RED, PSF_DOT_ON):
        Presentation()->SetWallpaper(NULL);
        Presentation()->SetColor(PCI_NORMAL, RED);
        Invalidate(Presentation()->mReal);
        Presentation()->Draw();
        break;

    case SIGNAL(IDB_DESK_COLOR_GREEN, PSF_DOT_ON):
        Presentation()->SetWallpaper(NULL);
        Presentation()->SetColor(PCI_NORMAL, GREEN);
        Invalidate(Presentation()->mReal);
        Presentation()->Draw();
        break;

    case SIGNAL(IDB_DESK_COLOR_BROWN, PSF_DOT_ON):
        Presentation()->SetWallpaper(NULL);
        Presentation()->SetColor(PCI_NORMAL, BROWN);
        Invalidate(Presentation()->mReal);
        Presentation()->Draw();
        break;

    case SIGNAL(IDB_DESK_COLOR_MAGENTA, PSF_DOT_ON):
        Presentation()->SetWallpaper(NULL);
        Presentation()->SetColor(PCI_NORMAL, MAGENTA);
        Invalidate(Presentation()->mReal);
        Presentation()->Draw();
        break;

    case SIGNAL(IDB_DESK_COLOR_CYAN, PSF_DOT_ON):
        Presentation()->SetWallpaper(NULL);
        Presentation()->SetColor(PCI_NORMAL, CYAN);
        Invalidate(Presentation()->mReal);
        Presentation()->Draw();
        break;

    case FLOATING_CHILD_CLOSED:
        {
            mpFloater = NULL;
            PegMenuButton *pChange = (PegMenuButton *) Find(IDB_ADD_FLOATING_CHILD);
            if (pChange)
            {
                pChange->SetChecked(FALSE);
            }
        }
        break;

    case SIGNAL(IDB_SET_COLOR_WHITE, PSF_DOT_ON):
        SetColor(PCI_NORMAL, WHITE);
        Invalidate();
        Draw();
        break;

    case SIGNAL(IDB_SET_COLOR_BLUE, PSF_DOT_ON):
        SetColor(PCI_NORMAL, BLUE);
        Invalidate();
        Draw();
        break;

    case SIGNAL(IDB_SET_COLOR_DARKGRAY, PSF_DOT_ON):
        SetColor(PCI_NORMAL, DARKGRAY);
        Invalidate();
        Draw();
        break;

    case SIGNAL(IDB_SET_SIZEABLE, PSF_CHECK_ON):
        pSource = (PegMenuButton *) Mesg.pSource;
        if (pSource->IsChecked())
        {
            AddStatus(PSF_SIZEABLE);
        }
        else
        {
            RemoveStatus(PSF_SIZEABLE);
        }
        break;

    case SIGNAL(IDB_SET_MOVEABLE, PSF_CHECK_ON):
        AddStatus(PSF_MOVEABLE);
        break;

    case SIGNAL(IDB_SET_MOVEABLE, PSF_CHECK_OFF):
        RemoveStatus(PSF_MOVEABLE);
        break;

    case SIGNAL(IDB_ADD_BUTTON_DIALOG, PSF_CLICKED):
        Rect.Set(400, 200, 630, 478);
        Presentation()->Add(new ButtonDialog(Rect, LS(SID_DIALOG)));
        break;

    case SIGNAL(IDB_HELP_ABOUT, PSF_CLICKED):
        {
            PegMessageWindow *pWin = new PegMessageWindow(LS(SID_ABOUT),
                LS(SID_LIB_VER), MW_OK|FF_RAISED);
            
            Presentation()->Center(pWin);
            Presentation()->Add(pWin);
            pWin->Execute();
        }
        break;

    case SIGNAL(IDB_ADD_TWIN, PSF_CLICKED):
        Rect = mReal;
        Rect.Shift(20, 20);
        Parent()->Add(new MainAppWindow(Rect, LS(SID_DECWIN), FALSE));
        pSource = (PegMenuButton *) Mesg.pSource;
        pSource->SetEnabled(FALSE);
        break;

    case TWIN_CLOSED:
        if (mpTwinButton)
        {
            mpTwinButton->SetEnabled(TRUE);
        }
        break; 

   case UM_LANGUAGE_CHANGE:
        Remove(this, FALSE);
        Presentation()->Add(new MainAppWindow(mReal, LS(SID_DECWIN), FALSE));
        Destroy(this);
        break;

    case SIGNAL(IDB_ADD_PROGWIN, PSF_CLICKED):
        Rect.Set(10, 10, 320, 180);
        Presentation()->Add(new ProgBarWindow(Rect, LS(SID_PROGBAR)));
        break;

    case SIGNAL(IDB_ADD_CONTROL_WIN, PSF_CHECK_ON):
        Rect.wRight = Presentation()->mReal.wRight - 10;
        Rect.wLeft = Rect.wRight - 310;
        Rect.wBottom = Presentation()->mReal.wBottom - 10;
        Rect.wTop = Rect.wBottom - 230;
        mpControlWin = new ControlPanelWindow(Rect, FF_THIN);
        Presentation()->Add(mpControlWin);
        break;

    case SIGNAL(IDB_ADD_CONTROL_WIN, PSF_CHECK_OFF):
        if (mpControlWin)
        {
            Destroy(mpControlWin);
            mpControlWin = NULL;
        }
        break;

    case SIGNAL(IDB_ADD_CUSTOM_CHILD, PSF_CHECK_ON):
        mpDemoWin = new DemoDrawWin();
        Add(mpDemoWin);
        break;

    case SIGNAL(IDB_ADD_CUSTOM_CHILD, PSF_CHECK_OFF):
        if (mpDemoWin)
        {
            Destroy(mpDemoWin);
        }
        mpDemoWin = NULL;
        break;

    case SIGNAL(IDB_ADD_FLOATING_CHILD, PSF_CHECK_ON):
        {
        WORD wHSpace = mClient.Width() / 8;
        WORD wVSpace = mClient.Height() / 8;
        Rect.Set(mClient.wLeft + wHSpace,
                mClient.wTop + wVSpace,
         mClient.wLeft + wHSpace + gbPegGlobeBitmap.wWidth + 4,
         mClient.wTop + wVSpace + gbPegGlobeBitmap.wHeight + 30);
        mpFloater = new FloatingWindow(Rect);
        Add(mpFloater);
        }
        break;

    case SIGNAL(IDB_ADD_FLOATING_CHILD, PSF_CHECK_OFF):
        if (mpFloater)
        {
            Destroy(mpFloater);    
            mpFloater = NULL;
        }
        break;

    case SIGNAL(IDB_SET_STATUS_BAR, PSF_CLICKED):
        break;

    case SIGNAL(IDB_DEMO_EXIT, PSF_CLICKED):
        {
         PegMessageWindow *mw = new PegMessageWindow(LS(SID_DEMO),
            LS(SID_EARLY), MW_OK|MW_CANCEL|FF_RAISED);

            Presentation()->Add(mw);

            if (mw->Execute() == IDB_OK)
            {
                // get rid of everyone else:

                PegThing *pTest = Presentation()->First();

                while(pTest)
                {
                    if (pTest != this)
                    {
                        Destroy(pTest);
                        pTest = Presentation()->First();
                    }
                    else
                    {
                        pTest = pTest->Next();
                    }
                }
                mpControlWin = NULL;
                Destroy(this);
            }
        }
        break;

    case PM_CLOSE:
        {
            // see if there are any other top level windows hanging
            // around besides me and my control panel:

            PegThing *pTest = Presentation()->First();

            while(pTest)
            {
                if (pTest != this && pTest->Id() == MAIN_APP_WINDOW)
                {
                    NewMessage.wType = TWIN_CLOSED;
                    pTest->Message(NewMessage);
                    break;
                }
                pTest = pTest->Next();
            }

            if (pTest)
            {
                // Yes, there are still other windows open, allow myself to
                // close.

                PegDecoratedWindow::Message(Mesg);
            }
            else
            {
                // No, I'm the last window open, ask the user if he
                // really wants to quit:

                NewMessage.wType = SIGNAL(IDB_DEMO_EXIT, PSF_CLICKED);
                NewMessage.pTarget = this;
                MessageQueue()->Push(NewMessage);
            }
        }
        break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }
    return (0);
}


/******************* STRING AND TEXT BOX DISPLAY WINDOW *********************/

/*--------------------------------------------------------------------------*/

StringWindow::StringWindow(PegRect InRect, const PEGCHAR *Title) : 
    PegDecoratedWindow(InRect)
{

PegMenuDescription TextBoxColorOptions[] = {
{LS(SID_WHITE_RED), IDB_TBCLR_WHITEONRED,  AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_GREEN_BLK), IDB_TBCLR_GREENONBLACK, AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_BLK_WHITE), IDB_TBCLR_BLACKONWHITE, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{NULL, 0, 0, NULL}
};


PegMenuDescription TextBoxFontOptions[] = {
{LS(SID_SCRIPT), IDB_TBFONT_SCRIPT,  AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_SMALL_FON), IDB_TBFONT_SYSTEM, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{NULL, 0, 0, NULL}
};

PegMenuDescription TextBoxMenu[] = {
{LS(SID_COLOR), 0, AF_ENABLED, TextBoxColorOptions},
{LS(SID_FONT), 0, AF_ENABLED, TextBoxFontOptions},
{NULL, 0, BF_SEPERATOR, NULL},
{LS(SID_ALLOW_EDIT), IDB_ALLOW_TB_EDIT, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
{NULL, 0, 0, NULL}
};


PegMenuDescription StringColorOptions[] = {
{LS(SID_WHITE_RED), IDB_STRCLR_WHITEONRED,  AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_GREEN_BLK), IDB_STRCLR_GREENONBLACK, AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_BLK_WHITE), IDB_STRCLR_BLACKONWHITE, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{NULL, 0, 0, NULL}
};


PegMenuDescription StringFontOptions[] = {
{LS(SID_SMALL_FON),   IDB_STRFONT_MENU,  AF_ENABLED|BF_DOTABLE, NULL},
{LS(SID_BIGFONT), IDB_STRFONT_SYSTEM, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{NULL, 0, 0, NULL}
};

PegMenuDescription StringMenu[] = {
{LS(SID_COLOR), 0, AF_ENABLED, StringColorOptions},
{LS(SID_FONT), 0, AF_ENABLED, StringFontOptions},
{NULL, 0, BF_SEPERATOR, NULL},
{LS(SID_ALLOW_EDIT), IDB_ALLOW_STRING_EDIT, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
{NULL, 0, 0, NULL}
};

PegMenuDescription StringMainMenu[] = {
    {LS(SID_TEXTBOX), 0, AF_ENABLED, TextBoxMenu},
    {LS(SID_STRING), 0, AF_ENABLED, StringMenu},
    {NULL, 0, 0, NULL}
};

    SetColor(PCI_NORMAL, PCLR_DIALOG);
    Add(new PegTitle(Title));
    Add(new PegMenuBar(StringMainMenu));

    mpString = new PegString(mClient.wLeft + 8, mClient.wTop + 8,
        mClient.Width() - 16, LS(SID_STRING));
    Add(mpString);

    PegRect Rect;
    Rect.Set(mClient.wLeft + 8, mClient.wTop + 38, mClient.wRight - 8,
        mClient.wBottom - 8);

    mpTextBox = new PegEditBox(Rect, 0, FF_RECESSED|EF_EDIT|EF_WRAP);
    mpTextBox->DataSet(LS(SID_TEXTBOX));
    mpTextBox->SetScrollMode(WSM_AUTOVSCROLL|WSM_CONTINUOUS);
    Add(mpTextBox);
}

/*--------------------------------------------------------------------------*/
SIGNED StringWindow::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case SIGNAL(IDB_TBCLR_WHITEONRED, PSF_DOT_ON):
        mpTextBox->SetColor(PCI_NORMAL, RED);
        mpTextBox->SetColor(PCI_NTEXT, WHITE);
        mpTextBox->Draw();
        break;

    case SIGNAL(IDB_TBCLR_GREENONBLACK, PSF_DOT_ON):
        mpTextBox->SetColor(PCI_NORMAL, BLACK);
        mpTextBox->SetColor(PCI_NTEXT, LIGHTGREEN);
        mpTextBox->Draw();
        break;

    case SIGNAL(IDB_TBCLR_BLACKONWHITE, PSF_DOT_ON):
        mpTextBox->SetColor(PCI_NORMAL, WHITE);
        mpTextBox->SetColor(PCI_NTEXT, BLACK);
        mpTextBox->Draw();
        break;

    case SIGNAL(IDB_TBFONT_SCRIPT, PSF_DOT_ON):
        mpTextBox->SetFont(&Script);
        mpTextBox->Draw();
        break;

    case SIGNAL(IDB_TBFONT_SYSTEM, PSF_DOT_ON):
        mpTextBox->SetFont(&SysFont);
        mpTextBox->Draw();
        break;

    case SIGNAL(IDB_ALLOW_TB_EDIT, PSF_CHECK_ON):
        mpTextBox->AddStatus(PSF_ACCEPTS_FOCUS);
        break;

    case SIGNAL(IDB_ALLOW_TB_EDIT, PSF_CHECK_OFF):
        mpTextBox->RemoveStatus(PSF_ACCEPTS_FOCUS);
        break;

    case SIGNAL(IDB_STRCLR_WHITEONRED, PSF_DOT_ON):
        mpString->SetColor(PCI_NORMAL, RED);
        mpString->SetColor(PCI_NTEXT, WHITE);
        mpString->Draw();
        break;

    case SIGNAL(IDB_STRCLR_GREENONBLACK, PSF_DOT_ON):
        mpString->SetColor(PCI_NORMAL, BLACK);
        mpString->SetColor(PCI_NTEXT, LIGHTGREEN);
        mpString->Draw();
        break;

    case SIGNAL(IDB_STRCLR_BLACKONWHITE, PSF_DOT_ON):
        mpString->SetColor(PCI_NORMAL, WHITE);
        mpString->SetColor(PCI_NTEXT, BLACK);
        mpString->Draw();
        break;

    case SIGNAL(IDB_STRFONT_MENU, PSF_DOT_ON):
        mpString->SetFont(&MenuFont);
        mpString->Draw();
        break;

    case SIGNAL(IDB_STRFONT_SYSTEM, PSF_DOT_ON):
        mpString->SetFont(&SysFont);
        mpString->Draw();
        break;

    case SIGNAL(IDB_ALLOW_STRING_EDIT, PSF_CHECK_ON):
        mpString->AddStatus(PSF_ACCEPTS_FOCUS);
        break;

    case SIGNAL(IDB_ALLOW_STRING_EDIT, PSF_CHECK_OFF):
        mpString->RemoveStatus(PSF_ACCEPTS_FOCUS);
        break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }
    return 0;
}


/*************************** CONTROL PANEL WINDOW ***************************/
/*--------------------------------------------------------------------------*/
// Control panel window: borderless window with lots of buttons and
// things.
/*--------------------------------------------------------------------------*/
ControlPanelWindow::ControlPanelWindow(const PegRect &InRect, UCHAR fflags) :
    PegDecoratedWindow(InRect, fflags)
{
    SetColor(PCI_NORMAL, PCLR_DIALOG);

    RemoveStatus(PSF_MOVEABLE | PSF_SIZEABLE);
    mRpmRect.wTop = mClient.wTop + 10;
    mRpmRect.wBottom = mRpmRect.wTop + 40;
    mRpmRect.wLeft = mClient.wLeft + (mClient.Width() / 3);
    mRpmRect.wRight = mClient.wRight - 10;

    mCountRect = mRpmRect;
    mCountRect.wTop = mCountRect.wBottom + 4;
    mCountRect.wBottom = mCountRect.wTop + 40;

    PEGCHAR cTemp[2];
    cTemp[0] = '0';
    cTemp[1] = 0;
    mpPrompt = new PegPrompt(mCountRect, cTemp, 0, FF_THIN|TJ_RIGHT|TT_COPY);
    mpPrompt->SetFont(&MonsterFont);
    mpPrompt->SetColor(PCI_NORMAL, BLACK);
    mpPrompt->SetColor(PCI_SELECTED, WHITE);

    mpPrompt->SetColor(PCI_NTEXT, LIGHTGREEN);

    Add(mpPrompt);

    mDividerRect.wTop = mCountRect.wBottom + 10;
    mDividerRect.wBottom = mDividerRect.wTop + 8;
    mDividerRect.wLeft = mClient.wLeft + 10;
    mDividerRect.wRight = mClient.wRight - 10;

    PegRect Rect;
    Rect.wLeft = mClient.wLeft + 10;
    Rect.wRight = mClient.wLeft + mClient.Width() / 2 - 5;
    Rect.wTop = mDividerRect.wBottom + 10;
    Rect.wBottom = Rect.wTop + 36;

    PegTextButton *pButton = new PegTextButton(Rect, LS(SID_START), IDB_START);

    if (!gbCurrentLanguage)
    {
        pButton->SetFont(&SysFont);
    }

    pButton->SetColor(PCI_NORMAL, LIGHTGREEN);

    Add(pButton);

    Rect.MoveTo(Rect.wRight + 10, Rect.wTop);
    pButton = new PegTextButton(Rect, LS(SID_STOP), IDB_STOP);

    if (!gbCurrentLanguage)
    {
        pButton->SetFont(&SysFont);
    }

    pButton->SetColor(PCI_NORMAL, LIGHTRED);

    AddToEnd(pButton);

    Rect.wBottom = mClient.wBottom - 10;
    Rect.wTop = Rect.wBottom - 24;
    Rect.wLeft = mClient.wLeft + 10;

    if (gbCurrentLanguage)
    {
        Rect.wRight = Rect.wLeft + 112;
    }
    else
    {
        Rect.wRight = Rect.wLeft + 66;
    }

    AddToEnd(new PegTextButton(Rect, LS(SID_SETUP)));
    Rect.MoveTo(Rect.wRight + 8, Rect.wTop);
    if (gbCurrentLanguage)
    {
        Rect.wRight = Rect.wLeft + 50;
    }
    AddToEnd(new PegTextButton(Rect, LS(SID_FAULTS)));
    Rect.MoveTo(Rect.wRight + 8, Rect.wTop);
    AddToEnd(new PegTextButton(Rect, LS(SID_HISTORY)));
    Rect.MoveTo(Rect.wRight + 8, Rect.wTop);
    AddToEnd(new PegTextButton(Rect, LS(SID_MORE)));
    mwCount = 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ControlPanelWindow::Draw(void)
{
    BeginDraw();
    PegPoint Point;
    PegDecoratedWindow::Draw();     // call base class draw function
    PegColor Color(WHITE, BLACK, CF_FILL);
    PEGCHAR s3[] = {'1', '2', '0', '.', '4', '4', 0};

    Rectangle(mRpmRect, Color, 1);

    Color.uForeground = PCLR_LOWLIGHT;
    Rectangle(mDividerRect, Color, 1);

    // Draw large Text:

    Color.uForeground = BLACK;
    Color.uFlags = CF_NONE;

    Point.x = mClient.wLeft + 20;
    Point.y = mRpmRect.wTop + 8;
    DrawText(Point, LS(SID_RPM), Color, &SysFont);

    Point.y = mCountRect.wTop + 8;

    if (gbCurrentLanguage)
    {
        DrawText(Point, LS(SID_COUNT), Color, &UniFont);
    }
    else
    {
        DrawText(Point, LS(SID_COUNT), Color, &SysFont);
    }

    Color.uForeground = LIGHTGREEN;

    Point.x = mRpmRect.wRight - 10 - TextWidth(s3, &MonsterFont);
    Point.y = mRpmRect.wTop + 3;
    DrawText(Point, s3, Color, &MonsterFont);
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void ControlPanelWindow::DrawCount(void)
{
    PEGCHAR cTemp[32];
    PegPoint Point;
    BeginDraw();
    PegColor Color(WHITE, BLACK, CF_FILL);
    Rectangle(mCountRect, Color, 1);

    Color.uForeground = LIGHTGREEN;
    Color.uFlags = CF_NONE;

    Point.y = mCountRect.wTop + 3;
    ltoa(mwCount, cTemp, 10);
    Point.x = mCountRect.wRight - 10 - TextWidth(cTemp, &MonsterFont);
    DrawText(Point, cTemp, Color, &MonsterFont);
    EndDraw();
}

/*--------------------------------------------------------------------------*/
SIGNED ControlPanelWindow::Message(const PegMessage &Mesg)
{
    PEGCHAR cTemp[40];

    switch (Mesg.wType)
    {
    case SIGNAL(IDB_START, PSF_CLICKED):
        SetTimer(0, 2, 2);
        break;

    case SIGNAL(IDB_STOP, PSF_CLICKED):
        KillTimer(0);
        break;

    case PM_TIMER:
        mwCount++;
        ltoa(mwCount, cTemp, 10);
        mpPrompt->DataSet(cTemp);
        mpPrompt->Draw();
        break;

    case PM_HIDE:
        KillTimer(0);
        break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }
    return 0;
}


/*************************** LIST WINDOW ************************************/
/*--------------------------------------------------------------------------*/
// Demo different list styles.
/*--------------------------------------------------------------------------*/

// bitmaps for buttons in horizontal list:

extern PegBitmap gbThunderBitmap;
extern PegBitmap gbLightBitmap;
extern PegBitmap gbSateliteBitmap;
extern PegBitmap gbDynamiteBitmap;
extern PegBitmap gbAppleBitmap;


DemoListWindow::DemoListWindow (PegRect Rect, const PEGCHAR *Title):
    PegDecoratedWindow(Rect)
{
    SIGNED iLoop;
    PegRect ListRect;
    PegVertList *pvlist;
    static PEGCHAR s1[] = {'P','e','g','H','o','r','z','L','i','s','t',' ','w','/',0};
    static PEGCHAR s2[] = {'P','e','g','B','i','t','m','a','p','B','u','t','t','o','n','s',0};
    static PEGCHAR s3[] = {'P','e','g','V','e','r','t','L','i','s','t',0};
    static PEGCHAR s4[] = {'P','r','o','m','p','t',0};
    static PEGCHAR s5[] = {'P','e','g','H','o','r','z','L','i','s','t',0};
    static PEGCHAR s6[] = {'B','u','t','t','o','n','x',0, 0};
    static PEGCHAR s7[] = {'S','e','l','e','c','t',0, 0};
    static PEGCHAR s8[] = {'P','e','g','C','o','m','b','o','B','o','x',':',0};
    
    Add(new PegTitle(Title));

    PegRect ClientRect = mClient;
    ClientRect.wRight += 220;
    ClientRect.wBottom += 220;
    PegWindow *pWin = new PegWindow(ClientRect, FF_NONE);

    pWin->SetColor(PCI_NORMAL, CYAN);

    WORD wLineHeight = TextHeight(lsTEST, &Script);

    ListRect.wLeft = pWin->mClient.wLeft + 10;
    ListRect.wTop = pWin->mClient.wTop + 10;
    ListRect.wRight = ListRect.wLeft + 220;
    ListRect.wBottom = ListRect.wTop + wLineHeight;

    PegPrompt *pPrompt = new PegPrompt(ListRect, s1);
    pPrompt->SetFont(&Script);
    pWin->Add(pPrompt);
    ListRect.Shift(0, wLineHeight);
    pPrompt = new PegPrompt(ListRect, s2);
    pPrompt->SetFont(&Script);
    pWin->Add(pPrompt);

    ListRect.wLeft = pWin->mClient.wLeft + 10;
    ListRect.wRight = ListRect.wLeft + 180;
    ListRect.wTop = ListRect.wBottom + 10;
    ListRect.wBottom = ListRect.wTop + 36;

    PegList *pList = new PegHorzList(ListRect);

    pList->AddToEnd(new PegBitmapButton(0, 0, &gbThunderBitmap, 1));
    pList->AddToEnd(new PegBitmapButton(0, 0, &gbLightBitmap, 2));
    pList->AddToEnd(new PegBitmapButton(0, 0, &gbSateliteBitmap, 3));
    pList->AddToEnd(new PegBitmapButton(0, 0, &gbDynamiteBitmap, 4));
    pList->AddToEnd(new PegBitmapButton(0, 0, &gbAppleBitmap, 5));
    pWin->Add(pList);

    PEGCHAR cTemp[40];

    ListRect.wTop = pWin->mClient.wTop + 10;
    ListRect.wBottom = ListRect.wTop + wLineHeight;
    ListRect.wLeft = ListRect.wRight + 80;
    ListRect.wRight = ListRect.wLeft + 180;
    pPrompt = new PegPrompt(ListRect, s3);
    pPrompt->SetFont(&Script);
    pWin->Add(pPrompt);

    ListRect.wTop += wLineHeight;
    ListRect.wLeft += 20;
    ListRect.wRight = ListRect.wLeft + 80;
    ListRect.wBottom = ListRect.wTop + 100;

    pvlist = new PegVertList(ListRect, ID_VERT_LIST, FF_RECESSED|LS_WRAP_SELECT);
    //pvlist->SetSeperation(20);
    strcpy(cTemp, s4);
    for (iLoop = 0; iLoop < 20; iLoop++)
    {
        ltoa(iLoop, cTemp + 6, 10);
        pvlist->AddToEnd(new PegPrompt(0, 0, cTemp, iLoop + 1,
            FF_NONE|TJ_LEFT|AF_ENABLED|TT_COPY));
    }
    pvlist->SetScrollMode(WSM_VSCROLL|WSM_CONTINUOUS);
    pWin->Add(pvlist);
    pvlist->SetSelected(83);

    ListRect.wTop = pWin->mClient.wBottom - (wLineHeight * 4);
    ListRect.wLeft = pWin->mClient.wLeft + 10;
    ListRect.wRight = ListRect.wLeft + 200;
    ListRect.wBottom = ListRect.wTop + wLineHeight;

    pPrompt = new PegPrompt(ListRect, s5);
    pPrompt->SetFont(&Script);
    pWin->Add(pPrompt);

    ListRect.wTop += wLineHeight;
    ListRect.wRight = ListRect.wLeft + 158;
    ListRect.wBottom = ListRect.wTop + 40;

    strcpy(cTemp, s6);
    pList = new PegHorzList(ListRect, 0, FF_RECESSED|LS_WRAP_SELECT);
    cTemp[6] = '1';
    pList->AddToEnd(new PegTextButton(0, 0, cTemp, ID_BUTTON1, AF_ENABLED|TT_COPY));
    cTemp[6] = '2';
    pList->AddToEnd(new PegTextButton(0, 0, cTemp, ID_BUTTON2, AF_ENABLED|TT_COPY));
    cTemp[6] = '3';
    pList->AddToEnd(new PegTextButton(0, 0, cTemp, ID_BUTTON3, AF_ENABLED|TT_COPY));
    cTemp[6] = '4';
    pList->AddToEnd(new PegTextButton(0, 0, cTemp, ID_BUTTON4, AF_ENABLED|TT_COPY));
    cTemp[6] = '5';
    pList->AddToEnd(new PegTextButton(0, 0, cTemp, ID_BUTTON5, AF_ENABLED|TT_COPY));
    cTemp[6] = '6';
    pList->AddToEnd(new PegTextButton(0, 0, cTemp, ID_BUTTON6, AF_ENABLED|TT_COPY));
    cTemp[6] = '7';
    pList->AddToEnd(new PegTextButton(0, 0, cTemp, ID_BUTTON7, AF_ENABLED|TT_COPY));
    cTemp[6] = '8';
    pList->AddToEnd(new PegTextButton(0, 0, cTemp, ID_BUTTON8, AF_ENABLED|TT_COPY));

    pList->SetScrollMode(WSM_HSCROLL|WSM_CONTINUOUS);
    pWin->Add(pList);

    ListRect.wLeft = ListRect.wRight + 100;
    ListRect.wRight = ListRect.wLeft + 100;
    ListRect.wTop -= 100;
    ListRect.wBottom -= 40;

    pList = new PegComboBox(ListRect, ID_COMBO_BOX, FF_RECESSED);

    strcpy(cTemp, s7);
    for (iLoop = 10; iLoop > 0; iLoop--)
    {
        ltoa(iLoop, cTemp + 6, 10);
        pList->Add(new PegPrompt(0, 0, cTemp, iLoop,
            FF_NONE|TJ_LEFT|AF_ENABLED|TT_COPY));
    }
    pList->SetScrollMode(WSM_VSCROLL|WSM_CONTINUOUS);
    pList->SetSelected(5);
    pWin->Add(pList);

    ListRect.wTop -= wLineHeight + 5;
    ListRect.wBottom = ListRect.wTop + wLineHeight;
    ListRect.wLeft -= 20;
    ListRect.wRight = ListRect.wLeft + 180;
    pPrompt = new PegPrompt(ListRect, s8);
    pPrompt->SetFont(&Script);
    pWin->Add(pPrompt);
    Add(pWin);
    SetScrollMode(WSM_VSCROLL|WSM_HSCROLL|WSM_CONTINUOUS);
}

SIGNED DemoListWindow::Message(const PegMessage &Mesg)
{
    return PegDecoratedWindow::Message(Mesg);
}



/*************************** SLIDER WINDOW **********************************/
/*--------------------------------------------------------------------------*/
// Demo slider window:
/*--------------------------------------------------------------------------*/

DemoSliderWindow::DemoSliderWindow (PegRect Rect, const PEGCHAR *Title):
    PegDecoratedWindow(Rect)
{
    PegRect ListRect;
    Add(new PegTitle(Title));
    PEGCHAR s1[] = {'0', 0};
    PEGCHAR s2[] = {'2', '5', 0};
    PEGCHAR s3[] = {'5', '0', 0};
    PEGCHAR s4[] = {'7', '5', 0};
    PEGCHAR s5[] = {'6', '0', 0};
    PEGCHAR s6[] = {'8', '0', 0};

    ListRect.wLeft = mClient.wLeft + 10;
    ListRect.wRight = ListRect.wLeft + 160; 
    ListRect.wTop = mClient.wTop + 10;
    ListRect.wBottom = ListRect.wTop + 30;

    mpSlider = new PegSlider(ListRect, 0, 100, IDS_SLIDER1, FF_THIN, 10);
    Add(mpSlider);
    
    Add(new PegPrompt(ListRect.wRight + 20, ListRect.wTop,
        40, s1, IDS_SLIDER1 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    
    ListRect.Shift(0, 50);
    ListRect.wBottom = ListRect.wTop + 40;
    PegSlider *pSlider = new PegSlider(ListRect, 100, 0, IDS_SLIDER2, FF_RAISED, 8);
    pSlider->SetCurrentValue(25);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wRight + 20, ListRect.wTop,
        40, s2, IDS_SLIDER2 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    ListRect.Shift(0, 60);
    pSlider = new PegSlider(ListRect, 0, 100, IDS_SLIDER3, FF_RECESSED, 10);
    pSlider->SetCurrentValue(50);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wRight + 20, ListRect.wTop,
        40, s3, IDS_SLIDER3 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    ListRect.Shift(0, 60);
    ListRect.wBottom = ListRect.wTop + 30;
    pSlider = new PegSlider(ListRect, 0, 100, IDS_SLIDER4, AF_TRANSPARENT, 6);
    pSlider->SetCurrentValue(75);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wRight + 20, ListRect.wTop,
        40, s4, IDS_SLIDER4 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));


    ListRect.wTop = ListRect.wBottom + 60;
    ListRect.wRight = ListRect.wLeft + 30;
    ListRect.wBottom = ListRect.wTop + 140;

    miSlider5Val = 0;

    Add(new PegSlider(ListRect, 0, 100, IDS_SLIDER5, FF_THIN, 10));
    Add(new PegPrompt(ListRect.wLeft - 5, ListRect.wTop - 24,
        32, s1, IDS_SLIDER5 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    ListRect.Shift(70, 0);
    pSlider = new PegSlider(ListRect, 0, 100, IDS_SLIDER6, FF_RAISED, 10);
    pSlider->SetCurrentValue(25);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wLeft - 5, ListRect.wTop - 24,
        32, s2, IDS_SLIDER6 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    ListRect.Shift(70, 0);
    pSlider = new PegSlider(ListRect, 100, 0, IDS_SLIDER7, FF_RECESSED|SF_SNAP, 20);
    pSlider->SetCurrentValue(60);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wLeft - 5, ListRect.wTop - 24,
        32, s5, IDS_SLIDER7 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    ListRect.Shift(70, 0);
    pSlider = new PegSlider(ListRect, 0, 100, IDS_SLIDER8, AF_TRANSPARENT|SF_SNAP, 10);
    pSlider->SetCurrentValue(80);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wLeft - 5, ListRect.wTop - 24,
        32, s6, IDS_SLIDER8 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
}

/*--------------------------------------------------------------------------*/
SIGNED DemoSliderWindow::Message(const PegMessage &Mesg)
{

    switch(Mesg.wType)
    {
    case SIGNAL(IDS_SLIDER1, PSF_SLIDER_CHANGE):
    case SIGNAL(IDS_SLIDER2, PSF_SLIDER_CHANGE):
    case SIGNAL(IDS_SLIDER3, PSF_SLIDER_CHANGE):
    case SIGNAL(IDS_SLIDER4, PSF_SLIDER_CHANGE):
    case SIGNAL(IDS_SLIDER5, PSF_SLIDER_CHANGE):
    case SIGNAL(IDS_SLIDER6, PSF_SLIDER_CHANGE):
    case SIGNAL(IDS_SLIDER7, PSF_SLIDER_CHANGE):
    case SIGNAL(IDS_SLIDER8, PSF_SLIDER_CHANGE):
        UpdateSliderPrompt(Mesg.iData, Mesg.lData);
        break;

    case PM_SHOW:
        PegDecoratedWindow::Message(Mesg);
        SetTimer(1, ONE_SECOND / 4, ONE_SECOND / 4);
        //SetTimer(1, ONE_SECOND * 2, ONE_SECOND * 2);
        break;

    case PM_HIDE:
        KillTimer(1);
        break;

    case PM_TIMER:
        if (mpSlider->GetCurrentValue() < 90)
        {
            mpSlider->SetCurrentValue(mpSlider->GetCurrentValue() + 5, TRUE);
            UpdateSliderPrompt(IDS_SLIDER1, mpSlider->GetCurrentValue());
        }
        else
        {
            KillTimer(1);
        }
        break;

    default:

        return(PegDecoratedWindow::Message(Mesg));
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void DemoSliderWindow::UpdateSliderPrompt(WORD wId, LONG lVal)
{
    wId += 100;
    PEGCHAR cTemp[40];

    // find the prompt with the matching ID

    PegThing *pTest = First();

    while (pTest)
    {
        if (pTest->Id() == wId)
        {
            PegPrompt *pp = (PegPrompt *) pTest;
            ltoa((SIGNED) lVal, cTemp, 10);
            pp->DataSet(cTemp);
            pp->Draw();
            return;
        }
        pTest = pTest->Next();
    }
}


/*************************** PROGRESS BAR WINDOW ****************************/
/*--------------------------------------------------------------------------*/
// Demo PegProgressBars:
/*--------------------------------------------------------------------------*/

ProgBarWindow::ProgBarWindow (PegRect Rect, const PEGCHAR *Title): PegDecoratedWindow(Rect)
{
    SetColor(PCI_NORMAL, LIGHTGRAY);

    PegRect BarRect;
    Add(new PegTitle(Title));

    BarRect.Set(mClient.wLeft + 30, mClient.wTop + 10, mClient.wRight - 30,
        mClient.wTop + 30); 
    
    mpProgBars[0] = new PegProgressBar(BarRect);
    Add(mpProgBars[0]);

    BarRect.Shift(0, 30);
    mpProgBars[1] = new PegProgressBar(BarRect, FF_RAISED|PS_LED);
    Add(mpProgBars[1]);

    BarRect.Shift(0, 30);
    mpProgBars[2] = new PegProgressBar(BarRect, FF_THIN|PS_RECESSED|PS_PERCENT|PS_SHOW_VAL,
        -100, 100);
    Add(mpProgBars[2]);

    BarRect.Shift(0, 30);
    BarRect.wBottom += 10;
    mpProgBars[3] = new PegProgressBar(BarRect, FF_RAISED);
    Add(mpProgBars[3]);

    BarRect.Set(mClient.wLeft + 4, mClient.wTop + 10, mClient.wLeft + 24,
        mClient.wBottom - 10); 
    mpProgBars[4] = new PegProgressBar(BarRect, PS_VERTICAL|FF_RECESSED);
    Add(mpProgBars[4]);

    BarRect.Set(mClient.wRight - 24, mClient.wTop + 10, mClient.wRight - 4,
        mClient.wBottom - 10); 
    mpProgBars[5] = new PegProgressBar(BarRect, PS_VERTICAL|PS_LED|PS_SHOW_VAL|FF_THIN);
    Add(mpProgBars[5]);

    mpProgBars[3]->SetColor(PCI_SELECTED, RED);
    mpProgBars[1]->SetColor(PCI_NORMAL, WHITE);
    mpProgBars[1]->SetColor(PCI_NTEXT, BLACK);
    mpProgBars[5]->SetColor(PCI_SELECTED, GREEN);
    mpProgBars[5]->SetColor(PCI_NORMAL, WHITE);
    mpProgBars[5]->SetColor(PCI_NTEXT, BLACK);
    mpProgBars[4]->SetColor(PCI_SELECTED, YELLOW);
    mpProgBars[4]->SetColor(PCI_NORMAL, BLUE);
    miValue = 0;
}

/*--------------------------------------------------------------------------*/
SIGNED ProgBarWindow::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
        PegDecoratedWindow::Message(Mesg);
        SetTimer(1, ONE_SECOND, ONE_SECOND / 2);
        break;

    case PM_TIMER:
        {
        miValue += 10;
        if (miValue > 100)
        {
            miValue = 0;
        }

        for (WORD wLoop = 0; wLoop < NUM_PROG_BARS; wLoop++)
        {
            mpProgBars[wLoop]->Update(miValue, TRUE);
        }
        }
        break;

    case PM_HIDE:
        KillTimer(1);
        PegDecoratedWindow::Message(Mesg);
        break;

    default:
        return PegDecoratedWindow::Message(Mesg);
    }
    return 0;        
}

/*************************** LINE DRAWING WINDOW ****************************/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DemoDrawWin::DemoDrawWin() : PegWindow()
{
    FrameStyle(FF_THIN);
    RemoveStatus(PSF_SIZEABLE);
    SetColor(PCI_NORMAL, BLACK);
}


/*--------------------------------------------------------------------------*/
SIGNED DemoDrawWin::Message(const PegMessage &Mesg)
{
    PegRect Rect;

    switch (Mesg.wType)
    {
    case PM_PARENTSIZED:
    case PM_SHOW:
        PegWindow::Message(Mesg);
        Rect = Parent()->mClient;
        Rect.wBottom -= Parent()->mClient.Height() / 2;
        Resize(Rect);
        break;

    default:
        return(PegWindow::Message(Mesg));
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
void DemoDrawWin::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();

    WORD wLoop;

    PegColor Color(RED, WHITE, CF_NONE);

    SIGNED xStep = mClient.Width() / 11;
    SIGNED yStep = mClient.Height() / 11;
    SIGNED xPos = mClient.wLeft + xStep + xStep / 2;
    SIGNED yPos = mClient.wTop + yStep / 2;

    for (wLoop = 0; wLoop < 10; wLoop++)
    {
        Line(mClient.wLeft + xStep / 2, yPos, xPos,
            mClient.wBottom - yStep / 2, Color);
        yPos += yStep;
        xPos += xStep;
    }

    Color.uForeground = GREEN;

    xPos = mClient.wLeft + xStep / 2;
    yPos = mClient.wTop + yStep + yStep / 2;

    for (wLoop = 0; wLoop < 10; wLoop++)
    {
        Line(xPos, mClient.wTop + yStep / 2,
            mClient.wRight - xStep / 2,    yPos, Color);
        yPos += yStep;
        xPos += xStep;
    }
    EndDraw();
}


/*************************** BUTTON DIALOG WINDOW ***************************/

extern PegBitmap gbGlobeBitmap;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// The stand-alone dialog window.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ButtonDialog::ButtonDialog(PegRect &InRect, const PEGCHAR *Title) : PegDialog(InRect, Title)
{
    PegRect Rect;
    PegBitmap *pbm = &gbGlobeBitmap;

    Rect.wLeft = mClient.wLeft + 150;
    Rect.wTop = mClient.wTop + 70;
    Rect.wRight = Rect.wLeft + pbm->wWidth + 4;
    Rect.wBottom = Rect.wTop + pbm->wHeight + 4;

    Add(new PegBitmapButton(Rect, pbm));

    PegRect PromptRect = Rect;
    PromptRect.Shift(0, -32);
    PromptRect.wBottom = PromptRect.wTop + 22;
    PegPrompt *pVal = new PegPrompt(PromptRect, LS(SID_10), 0, TJ_RIGHT|FF_RECESSED);
    pVal->SetFont(&SysFont);
    PromptRect.wLeft = PromptRect.wRight + 1;
    PromptRect.wRight = PromptRect.wLeft + PEG_SCROLL_WIDTH;
    PegSpinButton *pSpin = new PegSpinButton(PromptRect, pVal, 0, 20, 1);
    pSpin->SetOutputWidth(2);
    Add(pVal);
    Add(pSpin);

    Rect.Set(mClient.wLeft + 5, mClient.wTop + 5,
        mClient.wLeft + 70, mClient.wTop + 28);
    Add(new PegTextButton(Rect, LS(SID_NORMAL)));

    Rect.Shift(70, 0);
    Rect.wRight = mClient.wRight - 6;
    PegTextButton *pButton = new PegTextButton(Rect, LS(SID_DISABLED), 0, 0);
    Add(pButton);

    Rect.MoveTo(mClient.wLeft + 5, mClient.wTop + 30);
    Rect.wRight = Rect.wLeft + 80;
    PegRadioButton *pRadio = new PegRadioButton(Rect, LS(SID_MOVEABLE), IDB_MOVEABLE);
    pRadio->SetSelected(TRUE);
    Add(pRadio);

    Rect.Shift(0, 25);
    Add(new PegRadioButton(Rect, LS(SID_FIXED), IDB_FIXED));

    Rect.Shift(0, 25);
    Rect.wRight = mClient.wRight - 10;
    PegCheckBox *pCheckBox = new PegCheckBox(Rect.wLeft,
        Rect.wTop, LS(SID_ENABLE_GROUP), IDB_TOGGLE_GROUP);
    pCheckBox->SetSelected(TRUE);
    Add(pCheckBox);

    Rect.wLeft = mClient.wLeft + 5;
    Rect.wRight = mClient.wRight - 5;
    Rect.wTop += 30;
    Rect.wBottom = mClient.wBottom - 80;

    mpGroup = new PegGroup(Rect, LS(SID_GROUP));
    Rect.Shift(5, 15);
    Rect.wBottom = Rect.wTop + 22;

    if (gbCurrentLanguage)
    {
        Rect.wRight = Rect.wLeft + 70;
        pRadio = new PegRadioButton(Rect, LS(SID_RADIO));
        pRadio->SetSelected(TRUE);
        mpGroup->Add(pRadio);
        Rect.Shift(102, 0);
        mpGroup->Add(new PegRadioButton(Rect, LS(SID_RADIO)));

        Rect.Shift(0, 24);
        Rect.wRight = mClient.wRight - 60;
        Rect.wLeft = mClient.wLeft + 10;
    }
    else
    {
        Rect.wRight = Rect.wLeft + 80;
        pRadio = new PegRadioButton(Rect, LS(SID_RADIO));
        pRadio->SetSelected(TRUE);
        mpGroup->Add(pRadio);
        Rect.Shift(0, 24);
        mpGroup->Add(new PegRadioButton(Rect, LS(SID_RADIO)));
        Rect.Shift(105, -24);
        mpGroup->Add(new PegRadioButton(Rect, LS(SID_RADIO)));
        Rect.Shift(0, 24);
    }
    
    mpGroup->Add(new PegCheckBox(Rect, LS(SID_CHECKBOX)));
    Add(mpGroup);

    Rect.Set(mClient.wLeft + 10, mClient.wBottom - 72, 
        mClient.wLeft + 100, mClient.wBottom - 52);

    Add(new PegTextButton(Rect, LS(SID_LEFT), 0, TJ_LEFT|AF_ENABLED));
    Rect.Shift(0, 24);
    Add(new PegTextButton(Rect, LS(SID_CENTER), 0, TJ_CENTER|AF_ENABLED));
    Rect.Shift(0, 24);
    Add(new PegTextButton(Rect, LS(SID_RIGHT), 0, TJ_RIGHT|AF_ENABLED));
    Rect.Shift(116, -48);
    Rect.wBottom += 24;
    PegMLTextButton *pMLButton = new PegMLTextButton(Rect, LS(SID_MULLINE), '|', 10);
    Add(pMLButton);

    Rect.Shift(0, 48);
    Rect.wBottom -= 24;
    Add(new PegTextButton(Rect, LS(SID_OK), IDB_CLOSE));
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED ButtonDialog::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case SIGNAL(IDB_TOGGLE_GROUP, PSF_CHECK_ON):
        mpGroup->Enable(TRUE);
        break;

    case SIGNAL(IDB_TOGGLE_GROUP, PSF_CHECK_OFF):
        mpGroup->Enable(FALSE);
        break;

    case SIGNAL(IDB_MOVEABLE, PSF_DOT_ON):
        AddStatus(PSF_MOVEABLE);
        break;

    case SIGNAL(IDB_FIXED, PSF_DOT_ON):
        RemoveStatus(PSF_MOVEABLE);
        break;

    default:
        return(PegDialog::Message(Mesg));
    }
    return 0;
}

/*************************** GLOBE BITMAP CHILD WINDOW **********************/
/*--------------------------------------------------------------------------*/
FloatingWindow::FloatingWindow(const PegRect &Rect) : PegDecoratedWindow(Rect, FF_RAISED)
{
    Add(new PegTitle(LS(SID_FLOAT), 0));
    SetColor(PCI_NORMAL, BLACK);
    AddStatus(PSF_MOVEABLE);
}

/*--------------------------------------------------------------------------*/
SIGNED FloatingWindow::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case PM_HIDE:
        if (Parent())
        {
            PegMessage NewMessage(FLOATING_CHILD_CLOSED);
            Parent()->Message(NewMessage);
        }
        PegDecoratedWindow::Message(Mesg);
        break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
void FloatingWindow::Draw(void)
{
    BeginDraw();
    PegDecoratedWindow::Draw();

    PegPoint Put;
    Put.x = mClient.wLeft + (mClient.Width() - gbPegGlobeBitmap.wWidth) / 2;
    Put.y = mClient.wTop + (mClient.Height() - gbPegGlobeBitmap.wHeight) / 2;

    Bitmap(Put, &gbPegGlobeBitmap);
    EndDraw();
}

/*--------------------------------------------------------------------------*/
SwapLanguageButton::SwapLanguageButton(PegRect &Size) :
    PegTextButton(Size, LS(SID_SWAP_LANG))
{
    mpFont = &SysFont;
    SetColor(PCI_NORMAL, RED);
    SetColor(PCI_NTEXT, WHITE);
    Id(ID_LANGUAGE_BUTTON);
//    AddStatus(PSF_VIEWPORT);
}

/*--------------------------------------------------------------------------*/
SIGNED SwapLanguageButton::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_LBUTTONUP:
        if (StatusIs(PSF_OWNS_POINTER))
        {
            ReleasePointer();
        }

        if (mwStyle & BF_SELECTED)
        {
            if (gbCurrentLanguage)
            {
                // switch to English

                PegTextThing::SetDefaultFont(PEG_DEFAULT_FONT, &SysFont);
                PegTextThing::SetDefaultFont(PEG_TITLE_FONT, &SysFont);
                PegTextThing::SetDefaultFont(PEG_MENU_FONT, &MenuFont);
                PegTextThing::SetDefaultFont(PEG_TBUTTON_FONT, &MenuFont);
                PegTextThing::SetDefaultFont(PEG_RBUTTON_FONT, &MenuFont);
                PegTextThing::SetDefaultFont(PEG_CHECKBOX_FONT, &MenuFont);
                PegTextThing::SetDefaultFont(PEG_PROMPT_FONT, &SysFont);
                PegTextThing::SetDefaultFont(PEG_STRING_FONT, &SysFont);
                PegTextThing::SetDefaultFont(PEG_TEXTBOX_FONT, &SysFont);
                PegTextThing::SetDefaultFont(PEG_GROUP_FONT, &MenuFont);
                PegTextThing::SetDefaultFont(PEG_MESGWIN_FONT, &MenuFont);
                gbCurrentLanguage = 0;
            }
            else
            {
                // switch to Japanese
                // initialize all fonts to use our Unicode font:

                for (UCHAR uLoop = 0; uLoop < PEG_NUMBER_OF_DEFAULT_FONTS; uLoop++)
                {
                    PegTextThing::SetDefaultFont(uLoop, &UniFont);
                }
                gbCurrentLanguage = 1;
            }

            DataSet(LS(SID_SWAP_LANG));
            Invalidate(mClip);
            mwStyle &= ~BF_SELECTED;
            Draw();

            PegThing *pTest = Presentation()->First();

            while(pTest)
            {
                if (pTest->Id() == MAIN_APP_WINDOW ||
                    pTest->Id() == ID_MAIN_DIALOG)
                {
                    pTest->Message(UM_LANGUAGE_CHANGE);
                }
                pTest = pTest->Next();
            }
        }
        break;

    default:
        return PegTextButton::Message(Mesg);
    }
    return 0;
}

