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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include "peg.hpp"
#include "pegdemo.hpp"

#ifdef PEG_TOUCH_SUPPORT
//#include "elotouch.hpp"   // testing with ELO TouchSystems touch screen
//#include "cirtouch.hpp"   // testing with Cirtron Infrared touch screen
#endif

extern PegFont SysFont;
extern PegFont MonsterFont;
extern PegBitmap gbPegGlobeBitmap;
extern PegFont   Script;


/****************************************************************************/
/*--------------------------------------------------------------------------*/
// This function is called by the PegTask implementation, regardless of which
// platform we are running on.
/*--------------------------------------------------------------------------*/

// This demo is included as a sub-set of the RTOS integration demos. When
// running those programs, the startup function is supplied by an external
// module, and should not be defined here.

#if !defined(PEGSMX) && !defined(PEGX) && !defined(PEGSUPERTASK)

void PegAppInitialize(PegPresentationManager *pPresentation)
{
    // create some simple Windows:

    PegRect Rect;

    Rect.Set(0, 0, 288, 220);
    PegThing *NewWin = new MainDemoDialog(Rect, "Peg Demo Dialog");
    pPresentation->Center(NewWin);
    pPresentation->Add(NewWin);
}

#endif

/*************************** MAIN DIALOG WINDOW *****************************/
/*--------------------------------------------------------------------------*/
// The following strings are displayed in the small demo dialog window. They
// provide information to the reader about each control/window displayed as
// part of the demo.
/*--------------------------------------------------------------------------*/

#if defined(__WATCOMC__)
#pragma warning 438 9  // disables warning about concatenated message strings by setting to lowest level (9)
#endif

char *DemoMessage[] = {
//0
"Welcome!\r"
"This program, written entirely using the PEG class library, "
"is intended to introduce you to the usage and capabilities of "
"our graphical interface for embedded systems. \r"
"As you review the following information, "
"we encourage you to imagine YOUR product running PEG!",

//1
"This application is currently executing "
#if defined(PEGDOS)
"as a DOS real-mode executable. \r"
#elif defined(PEGSMX)
"under the SMX operating system. \r"
#elif defined(PEGX)
"under the ThreadX operating system. \r"
#elif defined(PHARLAP)
"under PharLap ETS. \r"
#elif defined(LINUXPEG)
"under Linux. \r"
#elif defined(LYNXPEG)
"under LynxOS. \r"
#elif defined(PEGX11)
"in the X Window System Development Platform "
#if defined(X11LINUX)
"on Linux. \r"
#elif defined(X11LYNX)
"on Lynx. \r"
#endif
#else
"in our Win32 development environment. \r"
#endif
"This application will execute unchanged on any real-time OS or video system. "
"In this instance, we have configured PEG to provide a 640x480x16 display interface "
"and support for mouse input.",


//2
"This short demo will present many of the control types provided in the class library. \r"
"Your job is to experiment with the objects as they are presented, and click on the "
"Continue... button below when you are ready to move on.",

//3
"The text you are reading is presented in a PegTextBox. The button below is a PegTextButton. "
"Like all PEG objects, it can be moved...",

//4
"or re-sized...",

//5
"at any time. It can also be enabled, disabled, and drawn with several different border styles. ",

//6
"This is an example of a PegWindow. \rThis window has been configured for no border and to be "
"non-moveable. Several PEG control types have been added to the window to build a useful screen. "
"This type of window is often used in touch-screen control panel applications.",

//7
"This window contains several other PEG control types: PegVertList, PegHorzList,"
"and PegCombo. PEG lists can contain several different types of objects. Shown "
"here are PegPrompts, PegTextButtons, and PegBitmapButtons. Also, this window "
"has been configured to support a virtual client area. You can pan around in "
"this area using the scroll bars, and experiment with each list control",

//8
"This window demonstrates PegSlider controls. The sliders are drawn with flat, "
"raised, recessed, and transparent styles. All slider components are automatically "
"proportionally sized relative to the overall slider control size.",

//9
"This window demonstrates PegProgressBar controls. PegProgress bar supports "
"several appearance styles and drawing options. PegProgressWindow (not shown) "
"makes it very easy to display and use a progress bar for lengthy operations.",

// 10

"This window contains PegString and PegEditBox controls. These objects can also be "
"configured with different colors, border styles, and other attributes, and each of these "
"attributes can be modified at any time.",

// 11
"This is a PegDecoratedWindow. This window supports all of the features you would expect in a "
"complex GUI application. \rThis dialog will now close, and the rest of the demo can be viewed "
"by executing the decorated window menu commands.",

"\0"

};

#if defined(__WATCOMC__)
#pragma warning 438 3  // re-enable warning to level 3 (see pragma above)
#endif

/*--------------------------------------------------------------------------*/
MainDemoDialog:: MainDemoDialog(const PegRect &InRect, char *Title) :
    PegDialog(InRect, Title)
{
    PegRect Rect = mClient;
    Rect.wLeft += 10;
    Rect.wRight -= 10;
    Rect.wTop += 10;
    Rect.wBottom -= Rect.Height() / 3;
    Rect.wBottom -= 4;

    mpTextBox = new PegTextBox(Rect, 0, FF_RECESSED|EF_WRAP);
    mpTextBox->SetScrollMode(WSM_AUTOVSCROLL|WSM_CONTINUOUS);
    mpTextBox->DataSet(DemoMessage[0]);

    Add(mpTextBox);

    Rect.wTop = Rect.wBottom + 10;
    Rect.wBottom = Rect.wTop + 26;
    Rect.wLeft += 70;
    Rect.wRight -= 70;

    mpContinueButton = new PegTextButton(Rect, "Continue...", IDB_CONTINUE);
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
        mpTextBox->RewindDataSet(DemoMessage[mwStep]);

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
                Presentation()->Add(new DemoListWindow(Rect, "Example PEG List Types"));
                break;

            case 8:
                Rect.wBottom = 460;
                Presentation()->Add(new DemoSliderWindow(Rect, "Peg Slider Controls"));
                break;

            case 9:
                Rect.wBottom = 180;
                Presentation()->Add(new ProgBarWindow(Rect, "Peg Progress Bar"));
                break;
            }
            break;

        case 10:
            mpTextBox->Draw();
            CloseLastWindow();

            // create the string and text box window:

            Rect.Set(10, 10, 320, 280);
            Presentation()->Add(new StringWindow(Rect, "Editable Text Controls"));
            break;

        case 11:
            mpTextBox->Draw();
            CloseLastWindow();
            mpContinueButton->DataSet("Close Me!");
            mpContinueButton->Draw();

            // create the decorated window:

            Rect.Set(10, 10, 480, 320);
            Presentation()->AddToEnd(new MainAppWindow(Rect, "Peg Decorated Window Example"));
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
            PegMessageWindow *mw = new PegMessageWindow("Early Exit!",
                "Are you sure you want to miss the rest of the demo?",
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
MainAppWindow::MainAppWindow (PegRect Rect, char *Title, BOOL bFirst) :
    PegDecoratedWindow(Rect)
{

/*--------------------------------------------------------------------------*/
// The following menu descriptions are used for the main demo window, which
// is the last window displayed in the demo.
/*--------------------------------------------------------------------------*/

static PegMenuDescription ClientColorOptions[] = {
{"Black", IDB_SET_COLOR_BLACK, AF_ENABLED|BF_DOTABLE, NULL},
{"White", IDB_SET_COLOR_WHITE, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"Blue",  IDB_SET_COLOR_BLUE, AF_ENABLED|BF_DOTABLE, NULL},
{"Dark gray", IDB_SET_COLOR_DARKGRAY, AF_ENABLED|BF_DOTABLE, NULL},
{"", 0, 0, NULL}
};


static PegMenuDescription DeskColorOptions[] = {
{"Black", IDB_DESK_COLOR_BLACK, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"Red",   IDB_DESK_COLOR_RED,   AF_ENABLED|BF_DOTABLE, NULL},
{"Green", IDB_DESK_COLOR_GREEN, AF_ENABLED|BF_DOTABLE, NULL},
{"Brown", IDB_DESK_COLOR_BROWN, AF_ENABLED|BF_DOTABLE, NULL},
{"Magenta", IDB_DESK_COLOR_MAGENTA, AF_ENABLED|BF_DOTABLE, NULL},
{"Cyan",  IDB_DESK_COLOR_CYAN,  AF_ENABLED|BF_DOTABLE, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription PaperOptions[] = {
{"Sand", IDB_SET_SAND_PAPER, AF_ENABLED|BF_DOTABLE, NULL},
{"Pool",  IDB_SET_POOL_PAPER, AF_ENABLED|BF_DOTABLE, NULL},
{"None",  IDB_SET_NO_PAPER, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription WindowOptions[] = {
{"Sizeable", IDB_SET_SIZEABLE, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
{"Moveable", IDB_SET_MOVEABLE, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription OptionsMenu[] = {
{"Wallpaper", 0, AF_ENABLED, PaperOptions},
{"Desktop Color", 0, AF_ENABLED, DeskColorOptions},
{"Window Style", 0, AF_ENABLED, WindowOptions},
{"Client Color", 0, AF_ENABLED, ClientColorOptions},
{"", 0, 0, NULL}
};

static PegMenuDescription FileMenu[] = {
{"Exit", IDB_DEMO_EXIT, AF_ENABLED, NULL},
{"Close", IDB_CLOSE, AF_ENABLED, NULL},
{"Save", 0, 0, NULL},
{"", 0, BF_SEPERATOR, NULL},
{"Restore", IDB_RESTORE, AF_ENABLED, NULL},
{"Minimize", IDB_MINIMIZE, AF_ENABLED, NULL},
{"Maximize", IDB_MAXIMIZE, AF_ENABLED, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription WindowMenu[] = {

{"Button Dialog", IDB_ADD_BUTTON_DIALOG, AF_ENABLED, NULL},
{"Progress Bars", IDB_ADD_PROGWIN, AF_ENABLED, NULL},
{"Add Twin", IDB_ADD_TWIN, AF_ENABLED, NULL},
{"Control Panel",  IDB_ADD_CONTROL_WIN, AF_ENABLED|BF_CHECKABLE, NULL},
{"", 0, BF_SEPERATOR, NULL},
{"Floating Bitmap", IDB_ADD_FLOATING_CHILD, AF_ENABLED|BF_CHECKABLE, NULL},
{"Line-Drawing", IDB_ADD_CUSTOM_CHILD, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription HelpMenu[] = {
    {"About",   IDB_HELP_ABOUT, AF_ENABLED, NULL},
    {"", 0, 0, NULL}
};

static PegMenuDescription MainMenu[] = {
    {"Help",     0, AF_ENABLED, HelpMenu},
    {"Disabled", 1, 0, NULL},
    {"Windows", 0, AF_ENABLED, WindowMenu},
    {"Options", 0, AF_ENABLED, OptionsMenu},
    {"File", 0, AF_ENABLED, FileMenu},
    {"", 0, 0, NULL}
};

    Id(MAIN_APP_WINDOW);
    Add(new PegTitle(Title));

    // Disable the "Add Twin" menu button if I am already the second
    // instance of myself:

    PegMenuBar *pBar = new PegMenuBar(MainMenu);
    mpTwinButton = pBar->FindButton("Add Twin");

    if (!bFirst)
    {
        mpTwinButton->SetEnabled(FALSE);
    }

    Add(pBar);      // add the menu bar

    PegStatusBar *pStat = new PegStatusBar();
    pStat->AddTextField(100, 0, "Fixed Field 1");
    pStat->AddTextField(100, 0, "Fixed Field 2");
    pStat->AddTextField(20, 0, "Variable Field 3");

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
        #if defined(PEG_WIN32)

        // If the user hits <ALT-F4> to close under windows, then the control
        // panel window could already be deleted. Check to be sure that it
        // still exists:

        PegThing pTest = Presentation()->First();

        while(pTest)
        {
            if (pTest == mpControlWin)
            {
                Destroy(mpControlWin);
                break;
            }
            pTest = pTest->Next();
        }
        #endif
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

   #if (PEG_NUM_COLORS >= 16) || defined(PEG_RUNTIME_COLOR_CHECK)   // not monochrome?
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
   #endif

    case CHILD_CLONE_CLOSED:
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

   #if (PEG_NUM_COLORS >= 16) || defined(PEG_RUNTIME_COLOR_CHECK)   // not monochrome?
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
   #endif

    case SIGNAL(IDB_SET_SIZEABLE, PSF_CHECK_ON):
        AddStatus(PSF_SIZEABLE);
        break;

    case SIGNAL(IDB_SET_SIZEABLE, PSF_CHECK_OFF):
        RemoveStatus(PSF_SIZEABLE);
        break;

    case SIGNAL(IDB_SET_MOVEABLE, PSF_CHECK_ON):
        AddStatus(PSF_MOVEABLE);
        break;

    case SIGNAL(IDB_SET_MOVEABLE, PSF_CHECK_OFF):
        RemoveStatus(PSF_MOVEABLE);
        break;

    case SIGNAL(IDB_ADD_BUTTON_DIALOG, PSF_CLICKED):
        Rect.Set(400, 200, 630, 478);
        Presentation()->Add(new ButtonDialog(Rect, "Peg Dialog Window"));
        break;

    case SIGNAL(IDB_HELP_ABOUT, PSF_CLICKED):
        {
            
            PegMessageWindow *pWin = new PegMessageWindow("PegDemo About",
                Version(), MW_OK|FF_RAISED);
            
            Presentation()->Center(pWin);
            Presentation()->Add(pWin);
            pWin->Execute();
        }
        break;

    case SIGNAL(IDB_ADD_TWIN, PSF_CLICKED):
        Rect = mReal;
           Rect.Shift(20, 20);
        Parent()->Add(new MainAppWindow(Rect, "My Twin Brother!", FALSE));
        pSource = (PegMenuButton *) Mesg.pSource;
        pSource->SetEnabled(FALSE);
        break;

    case TWIN_CLOSED:
        mpTwinButton->SetEnabled(TRUE);
        break; 

    case SIGNAL(IDB_ADD_PROGWIN, PSF_CLICKED):
        Rect.Set(10, 10, 320, 180);
        Presentation()->Add(new ProgBarWindow(Rect, "Progess Bars"));
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
        Destroy(mpControlWin);
        mpControlWin = NULL;
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
            PegMessageWindow *mw = new PegMessageWindow("PEG Demo",
            "Are you sure you want to quit?", MW_OK|MW_CANCEL|FF_RAISED);

            Presentation()->Add(mw);

            if (mw->Execute() == IDB_OK)
            {
                // get rid of everyone else:

                #ifndef PHARLAP

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

                #endif
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

StringWindow::StringWindow(PegRect InRect, char *Title) : 
    PegDecoratedWindow(InRect)
{
static PegMenuDescription TextBoxColorOptions[] = {
{"White On Red", IDB_TBCLR_WHITEONRED,  AF_ENABLED|BF_DOTABLE, NULL},
{"Green On Black", IDB_TBCLR_GREENONBLACK, AF_ENABLED|BF_DOTABLE, NULL},
{"Black On White", IDB_TBCLR_BLACKONWHITE, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription TextBoxFontOptions[] = {
{"Script Font", IDB_TBFONT_SCRIPT,  AF_ENABLED|BF_DOTABLE, NULL},
{"System Font", IDB_TBFONT_SYSTEM, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"", 0, 0, NULL}
};


static PegMenuDescription TextBoxMenu[] = {
{"Color", 0, AF_ENABLED, TextBoxColorOptions},
{"Font", 0, AF_ENABLED, TextBoxFontOptions},
{"", 0, BF_SEPERATOR, NULL},
{"Allow Edit", IDB_ALLOW_TB_EDIT, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
{"", 0, 0, NULL}
};


static PegMenuDescription StringColorOptions[] = {
{"White On Red",  IDB_STRCLR_WHITEONRED,  AF_ENABLED|BF_DOTABLE, NULL},
{"Green On Black", IDB_STRCLR_GREENONBLACK, AF_ENABLED|BF_DOTABLE, NULL},
{"Black On White", IDB_STRCLR_BLACKONWHITE, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription StringFontOptions[] = {
{"Menu Font",   IDB_STRFONT_MENU,  AF_ENABLED|BF_DOTABLE, NULL},
{"System Font", IDB_STRFONT_SYSTEM, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"", 0, 0, NULL}
};


static PegMenuDescription StringMenu[] = {
{"Color", 0, AF_ENABLED, StringColorOptions},
{"Font", 0, AF_ENABLED, StringFontOptions},
{"", 0, BF_SEPERATOR, NULL},
{"Allow Edit", IDB_ALLOW_STRING_EDIT, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription StringMainMenu[] = {
    {"EditBox", 0, AF_ENABLED, TextBoxMenu},
    {"String", 0, AF_ENABLED, StringMenu},
    {"", 0, 0, NULL}
};

    SetColor(PCI_NORMAL, PCLR_DIALOG);
    Add(new PegTitle(Title));
    Add(new PegMenuBar(StringMainMenu));

    mpString = new PegString(mClient.wLeft + 8, mClient.wTop + 8,
        mClient.Width() - 16, "PegString");
    Add(mpString);

    PegRect Rect;
    Rect.Set(mClient.wLeft + 8, mClient.wTop + 30, mClient.wRight - 8,
        mClient.wBottom - 8);

    mpTextBox = new PegEditBox(Rect, 0, FF_RECESSED|EF_EDIT|EF_WRAP);
    mpTextBox->DataSet("PegEditBox");
    mpTextBox->SetScrollMode(WSM_AUTOVSCROLL|WSM_CONTINUOUS);
    Add(mpTextBox);
}

/*--------------------------------------------------------------------------*/
SIGNED StringWindow::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
#if (PEG_NUM_COLORS >= 16) || defined(PEG_RUNTIME_COLOR_CHECK)  // not monochrome?
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
#endif

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

#if (PEG_NUM_COLORS >= 16)  || defined(PEG_RUNTIME_COLOR_CHECK)// not monochrome?
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
#endif

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

    mpPrompt = new PegPrompt(mCountRect, "0", 0, FF_THIN|TJ_RIGHT|TT_COPY);
    mpPrompt->SetFont(&MonsterFont);
    mpPrompt->SetColor(PCI_NORMAL, BLACK);
    mpPrompt->SetColor(PCI_SELECTED, WHITE);

   #if (PEG_NUM_COLORS >= 16) || defined(PEG_RUNTIME_COLOR_CHECK)  // not monochrome?
    mpPrompt->SetColor(PCI_NTEXT, LIGHTGREEN);
   #else
    mpPrompt->SetColor(PCI_NTEXT, WHITE);
   #endif

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

    PegTextButton *pButton = new PegTextButton(Rect, "START", IDB_START);

   #if (PEG_NUM_COLORS >= 16) || defined(PEG_RUNTIME_COLOR_CHECK)  // not monochrome?
    pButton->SetColor(PCI_NORMAL, LIGHTGREEN);
   #else
    pButton->SetColor(PCI_NORMAL, WHITE);
   #endif

    pButton->SetFont(&SysFont);
    Add(pButton);

    Rect.MoveTo(Rect.wRight + 10, Rect.wTop);
    pButton = new PegTextButton(Rect, "STOP", IDB_STOP);

   #if (PEG_NUM_COLORS >= 16) || defined(PEG_RUNTIME_COLOR_CHECK)  // not monochrome?
    pButton->SetColor(PCI_NORMAL, LIGHTRED);
   #endif

    pButton->SetFont(&SysFont);
    AddToEnd(pButton);

    Rect.wBottom = mClient.wBottom - 10;
    Rect.wTop = Rect.wBottom - 24;
    Rect.wLeft = mClient.wLeft + 10;
    Rect.wRight = Rect.wLeft + 60;

    AddToEnd(new PegTextButton(Rect, "Setup"));
    Rect.MoveTo(Rect.wRight + 14, Rect.wTop);
    AddToEnd(new PegTextButton(Rect, "Faults"));
    Rect.MoveTo(Rect.wRight + 14, Rect.wTop);
    AddToEnd(new PegTextButton(Rect, "History"));
    Rect.MoveTo(Rect.wRight + 14, Rect.wTop);
    AddToEnd(new PegTextButton(Rect, "More..."));
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

    Rectangle(mRpmRect, Color, 1);

    Color.uForeground = PCLR_LOWLIGHT;
    Rectangle(mDividerRect, Color, 1);

    // Draw large Text:

    Color.uForeground = BLACK;
    Color.uFlags = CF_NONE;

    Point.x = mClient.wLeft + 20;
    Point.y = mRpmRect.wTop + 8;
    DrawText(Point, "RPM:", Color, &SysFont);

    Point.y = mCountRect.wTop + 8;
    DrawText(Point, "COUNT:", Color, &SysFont);

  #if (PEG_NUM_COLORS >= 16) || defined(PEG_RUNTIME_COLOR_CHECK)   // not monochrome?
    Color.uForeground = LIGHTGREEN;
  #else
    Color.uForeground = WHITE;
  #endif

    Point.x = mRpmRect.wRight - 10 - TextWidth("120.44", &MonsterFont);
    Point.y = mRpmRect.wTop + 3;
    DrawText(Point, "120.44", Color, &MonsterFont);
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void ControlPanelWindow::DrawCount(void)
{
    char cTemp[32];
    PegPoint Point;
    BeginDraw();
    PegColor Color(WHITE, BLACK, CF_FILL);
    Rectangle(mCountRect, Color, 1);

   #if (PEG_NUM_COLORS >= 16) || defined(PEG_RUNTIME_COLOR_CHECK)      // not monochrome?
    Color.uForeground = LIGHTGREEN;
   #else
    Color.uForeground = WHITE;
   #endif

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
    char cTemp[40];
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

/*--------------------------------------------------------------------------*/
DemoListWindow::DemoListWindow (PegRect Rect, char *Title):
    PegDecoratedWindow(Rect)
{
    SIGNED iLoop;
    PegRect ListRect;
    PegVertList *pvlist;
    Add(new PegTitle(Title));

    PegRect ClientRect = mClient;
    ClientRect.wRight += 220;
    ClientRect.wBottom += 220;
    PegWindow *pWin = new PegWindow(ClientRect, FF_NONE);

    #ifdef PEG_NUM_COLORS
    #if (PEG_NUM_COLORS > 4)
    pWin->SetColor(PCI_NORMAL, CYAN);
    #endif
    #endif

    WORD wLineHeight = TextHeight("A", &Script);

    ListRect.wLeft = pWin->mClient.wLeft + 10;
    ListRect.wTop = pWin->mClient.wTop + 10;
    ListRect.wRight = ListRect.wLeft + 220;
    ListRect.wBottom = ListRect.wTop + wLineHeight;
    PegPrompt *pPrompt = new PegPrompt(ListRect,
        "PegHorzList w/");
    pPrompt->SetFont(&Script);
    pWin->Add(pPrompt);
    ListRect.Shift(0, wLineHeight);
    pPrompt = new PegPrompt(ListRect, "PegBitmapButtons");
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
    pWin->AddToEnd(pList);

    char cTemp[40];

    ListRect.wTop = pWin->mClient.wTop + 10;
    ListRect.wBottom = ListRect.wTop + wLineHeight;
    ListRect.wLeft = ListRect.wRight + 80;
    ListRect.wRight = ListRect.wLeft + 180;
    pPrompt = new PegPrompt(ListRect, "PegVertList");
    pPrompt->SetFont(&Script);
    pWin->Add(pPrompt);

    ListRect.wTop += wLineHeight;
    ListRect.wLeft += 20;
    ListRect.wRight = ListRect.wLeft + 80;
    ListRect.wBottom = ListRect.wTop + 100;

    pvlist = new PegVertList(ListRect, ID_VERT_LIST, FF_RECESSED|LS_WRAP_SELECT);
    //pvlist->SetSeparation(20);
    strcpy(cTemp, "Prompt");
    for (iLoop = 0; iLoop < 12; iLoop++)
    {
        ltoa(iLoop, cTemp + 6, 10);
        pvlist->AddToEnd(new PegPrompt(0, 0, cTemp, iLoop + 1,
            FF_NONE|TJ_LEFT|AF_ENABLED|TT_COPY));
    }
    pvlist->SetScrollMode(WSM_VSCROLL|WSM_CONTINUOUS);
    pWin->AddToEnd(pvlist);
    pvlist->SetSelected(6);

    ListRect.wTop = pWin->mClient.wBottom - (wLineHeight * 4);
    ListRect.wLeft = pWin->mClient.wLeft + 10;
    ListRect.wRight = ListRect.wLeft + 200;
    ListRect.wBottom = ListRect.wTop + wLineHeight;

    pPrompt = new PegPrompt(ListRect, "PegHorzList");
    pPrompt->SetFont(&Script);
    pWin->Add(pPrompt);

    ListRect.wTop += wLineHeight;
    ListRect.wRight = ListRect.wLeft + 158;
    ListRect.wBottom = ListRect.wTop + 40;

    pList = new PegHorzList(ListRect, 0, FF_RECESSED|LS_WRAP_SELECT);
    pList->AddToEnd(new PegTextButton(0, 0, "Button1", ID_BUTTON1, AF_ENABLED|TT_COPY));
    pList->AddToEnd(new PegTextButton(0, 0, "Button2", ID_BUTTON2, AF_ENABLED|TT_COPY));
    pList->AddToEnd(new PegTextButton(0, 0, "Button3", ID_BUTTON3, AF_ENABLED|TT_COPY));
    pList->AddToEnd(new PegTextButton(0, 0, "Button4", ID_BUTTON4, AF_ENABLED|TT_COPY));
    pList->AddToEnd(new PegTextButton(0, 0, "Button5", ID_BUTTON5, AF_ENABLED|TT_COPY));
    pList->AddToEnd(new PegTextButton(0, 0, "Button6", ID_BUTTON6, AF_ENABLED|TT_COPY));
    pList->AddToEnd(new PegTextButton(0, 0, "Button7", ID_BUTTON7, AF_ENABLED|TT_COPY));
    pList->AddToEnd(new PegTextButton(0, 0, "Button8", ID_BUTTON8, AF_ENABLED|TT_COPY));

    pList->SetScrollMode(WSM_HSCROLL|WSM_CONTINUOUS);
    pWin->AddToEnd(pList);

    ListRect.wLeft = ListRect.wRight + 100;
    ListRect.wRight = ListRect.wLeft + 100;
    ListRect.wTop -= 100;
    ListRect.wBottom -= 40;

    pList = new PegComboBox(ListRect, ID_COMBO_BOX, FF_RECESSED);

    strcpy(cTemp, "Select");
    for (iLoop = 10; iLoop > 0; iLoop--)
    {
        ltoa(iLoop, cTemp + 6, 10);
        PegPrompt *pp = new PegPrompt(0, 0, cTemp, iLoop,
            FF_NONE|TJ_LEFT|AF_ENABLED|TT_COPY);

        #if (PEG_NUM_COLORS >= 16)
        pp->SetColor(PCI_SELECTED, BLACK);
        pp->SetColor(PCI_STEXT, GREEN);
        #endif
        pList->Add(pp);
    }
    pList->SetScrollMode(WSM_VSCROLL|WSM_CONTINUOUS);
    pList->SetSelected(5);
    pWin->AddToEnd(pList);

    ListRect.wTop -= wLineHeight + 5;
    ListRect.wBottom = ListRect.wTop + wLineHeight;
    ListRect.wLeft -= 20;
    ListRect.wRight = ListRect.wLeft + 180;
    pPrompt = new PegPrompt(ListRect, "PegComboBox:");
    pPrompt->SetFont(&Script);
    pWin->Add(pPrompt);
    Add(pWin);

    //SetScrollMode(WSM_VSCROLL|WSM_HSCROLL|WSM_CONTINUOUS);
    SetScrollMode(WSM_AUTOSCROLL|WSM_CONTINUOUS);
}

/*--------------------------------------------------------------------------*/
SIGNED DemoListWindow::Message(const PegMessage &Mesg)
{
    PegMessageWindow *pWin;

    switch(Mesg.wType)
    {
    case SIGNAL(ID_BUTTON1, PSF_CLICKED):
        // Some stuff just for testing, replace with your code:

        pWin = new PegMessageWindow("Hello",
                "Button1 was clicked", MW_OK|FF_RAISED);
        Center(pWin);
        Presentation()->Add(pWin);
        pWin->Execute();
        break;

    case SIGNAL(ID_BUTTON2, PSF_CLICKED):
        // Some stuff just for testing, replace with your code:

        pWin = new PegMessageWindow("Hello",
                "Button2 was clicked", MW_OK|FF_RAISED);
        Center(pWin);
        Presentation()->Add(pWin);
        pWin->Execute();
        break;

    case SIGNAL(ID_BUTTON3, PSF_CLICKED):
        // Some stuff just for testing, replace with your code:

        pWin = new PegMessageWindow("Hello",
            "Button3 was clicked", MW_OK|FF_RAISED);
        Center(pWin);
        Presentation()->Add(pWin);
        pWin->Execute();
        break;

    default:
        return PegDecoratedWindow::Message(Mesg);
    }
    return 0;
}



/*************************** SLIDER WINDOW **********************************/
/*--------------------------------------------------------------------------*/
// Demo slider window:
/*--------------------------------------------------------------------------*/

DemoSliderWindow::DemoSliderWindow (PegRect Rect, char *Title):
    PegDecoratedWindow(Rect)
{
    PegRect ListRect;
    Add(new PegTitle(Title));

    ListRect.wLeft = mClient.wLeft + 10;
    ListRect.wRight = ListRect.wLeft + 160; 
    ListRect.wTop = mClient.wTop + 10;
    ListRect.wBottom = ListRect.wTop + 30;

    mpSlider = new PegSlider(ListRect, 0, 100, IDS_SLIDER1, FF_THIN, 10);
    Add(mpSlider);
    
    Add(new PegPrompt(ListRect.wRight + 20, ListRect.wTop,
        40, "0", IDS_SLIDER1 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    
    ListRect.Shift(0, 50);
    ListRect.wBottom = ListRect.wTop + 40;
    PegSlider *pSlider = new PegSlider(ListRect, 100, 0, IDS_SLIDER2, FF_RAISED, 0); // 8
    pSlider->SetCurrentValue(25);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wRight + 20, ListRect.wTop,
        40, "25", IDS_SLIDER2 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    ListRect.Shift(0, 60);
    pSlider = new PegSlider(ListRect, 0, 100, IDS_SLIDER3, FF_RECESSED, -1); //10
    pSlider->SetCurrentValue(50);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wRight + 20, ListRect.wTop,
        40, "50", IDS_SLIDER3 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    ListRect.Shift(0, 60);
    ListRect.wBottom = ListRect.wTop + 30;
    pSlider = new PegSlider(ListRect, 0, 100, IDS_SLIDER4, AF_TRANSPARENT, 6);
    pSlider->SetCurrentValue(75);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wRight + 20, ListRect.wTop,
        40, "75", IDS_SLIDER4 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));


    ListRect.wTop = ListRect.wBottom + 60;
    ListRect.wRight = ListRect.wLeft + 30;
    ListRect.wBottom = ListRect.wTop + 140;

    miSlider5Val = 0;

    Add(new PegSlider(ListRect, 0, 100, IDS_SLIDER5, FF_THIN, 10));
    Add(new PegPrompt(ListRect.wLeft - 5, ListRect.wTop - 24,
        32, "0", IDS_SLIDER5 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    ListRect.Shift(70, 0);
    pSlider = new PegSlider(ListRect, 0, 100, IDS_SLIDER6, FF_RAISED, 0); // 10
    pSlider->SetCurrentValue(25);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wLeft - 5, ListRect.wTop - 24,
        32, "25", IDS_SLIDER6 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    ListRect.Shift(70, 0);
    pSlider = new PegSlider(ListRect, 100, 0, IDS_SLIDER7, FF_RECESSED|SF_SNAP, 20);
    pSlider->SetCurrentValue(60);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wLeft - 5, ListRect.wTop - 24,
        32, "60", IDS_SLIDER7 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
    ListRect.Shift(70, 0);
    pSlider = new PegSlider(ListRect, 0, 100, IDS_SLIDER8, AF_TRANSPARENT|SF_SNAP, 10);
    pSlider->SetCurrentValue(80);
    Add(pSlider);
    Add(new PegPrompt(ListRect.wLeft - 5, ListRect.wTop - 24,
        32, "80", IDS_SLIDER8 + 100, FF_RECESSED|TJ_RIGHT|TT_COPY));
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
    char cTemp[40];

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

ProgBarWindow::ProgBarWindow (PegRect Rect, char *Title): PegDecoratedWindow(Rect)
{
    #if (PEG_NUM_COLORS >= 4) || defined(PEG_RUNTIME_COLOR_CHECK)  // not monochrome?
    SetColor(PCI_NORMAL, LIGHTGRAY);
    #else
    SetColor(PCI_NORMAL, WHITE);
    #endif
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

    #if (PEG_NUM_COLORS >= 16) || defined(PEG_RUNTIME_COLOR_CHECK) // not monochrome?
    mpProgBars[3]->SetColor(PCI_SELECTED, RED);
    mpProgBars[1]->SetColor(PCI_NORMAL, WHITE);
    mpProgBars[1]->SetColor(PCI_NTEXT, BLACK);
    mpProgBars[5]->SetColor(PCI_SELECTED, GREEN);
    mpProgBars[5]->SetColor(PCI_NORMAL, WHITE);
    mpProgBars[5]->SetColor(PCI_NTEXT, BLACK);
    mpProgBars[4]->SetColor(PCI_SELECTED, YELLOW);
    mpProgBars[4]->SetColor(PCI_NORMAL, BLUE);
    #endif
    miValue = 0;
}

/*--------------------------------------------------------------------------*/
SIGNED ProgBarWindow::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
        PegDecoratedWindow::Message(Mesg);
        SetTimer(1, ONE_SECOND / 4, ONE_SECOND / 4);
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

   #if (PEG_NUM_COLORS >= 16) || defined(PEG_RUNTIME_COLOR_CHECK)  // not monochrome?

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
   #endif
    EndDraw();
}


/*************************** BUTTON DIALOG WINDOW ***************************/

extern PegBitmap gbGlobeBitmap;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// The stand-alone dialog window.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ButtonDialog::ButtonDialog(PegRect &InRect, char *Title) : PegDialog(InRect, Title)
{
    PegBitmapButton *pb = new PegBitmapButton(mClient.wLeft + 150,
                            mClient.wTop + 70, &gbGlobeBitmap,
                            IDB_BITMAP_BTN);
    Add(pb);

    PegRect Rect = pb->mReal;
    PegRect PromptRect = Rect;
    PromptRect.Shift(0, -32);
    PromptRect.wBottom = PromptRect.wTop + 22;
    PegPrompt *pVal = new PegPrompt(PromptRect, "10", 0, TJ_RIGHT|FF_RECESSED);
    pVal->SetFont(&SysFont);
    PromptRect.wLeft = PromptRect.wRight + 1;
    PromptRect.wRight = PromptRect.wLeft + PEG_SCROLL_WIDTH;
    PegSpinButton *pSpin = new PegSpinButton(PromptRect, pVal, 0, 20, 1);
    pSpin->SetOutputWidth(2);
    Add(pVal);
    Add(pSpin);

    Rect.Set(mClient.wLeft + 5, mClient.wTop + 5,
        mClient.wLeft + 70, mClient.wTop + 28);
    Add(new PegTextButton(Rect, "Normal"));

    Rect.Shift(70, 0);
    PegTextButton *pButton = new PegTextButton(Rect, "Disabled", 0, 0);
    Add(pButton);

    Rect.Shift(70, 0);
    Rect.wRight += 10;
    pButton = new PegTextButton(Rect, "BigFont", 0, AF_ENABLED|FF_RECESSED);
    pButton->SetFont(&SysFont);
    Add(pButton);

    Rect.MoveTo(mClient.wLeft + 5, mClient.wTop + 30);
    PegRadioButton *pRadio = new PegRadioButton(Rect, "Moveable", IDB_MOVEABLE);
    pRadio->SetSelected(TRUE);
    Add(pRadio);

    Rect.Shift(0, 25);
    Add(new PegRadioButton(Rect, "Fixed", IDB_FIXED));

    Rect.Shift(0, 25);
    Rect.wRight = mClient.wRight - 10;
    PegCheckBox *pCheckBox = new PegCheckBox(Rect.wLeft,
        Rect.wTop, "EnableGroup", IDB_TOGGLE_GROUP);
    pCheckBox->SetSelected(TRUE);
    Add(pCheckBox);

    Rect.wLeft = mClient.wLeft + 5;
    Rect.wRight = mClient.wRight - 5;
    Rect.wTop += 30;
    Rect.wBottom = mClient.wBottom - 80;

    mpGroup = new PegGroup(Rect, "PegGroup");

    Rect.Shift(5, 15);
    Rect.wRight = Rect.wLeft + 80;
    Rect.wBottom = Rect.wTop + 25;
    pRadio = new PegRadioButton(Rect, "Radio 1");
    pRadio->SetSelected(TRUE);
    mpGroup->Add(pRadio);
    Rect.Shift(0, 24);
    mpGroup->Add(new PegRadioButton(Rect, "Radio 2"));
    Rect.Shift(105, -24);
    mpGroup->Add(new PegRadioButton(Rect, "Radio 3"));
    Rect.Shift(0, 24);
    mpGroup->Add(new PegCheckBox(Rect, "CheckBox"));
    Add(mpGroup);

    Rect.Set(mClient.wLeft + 10, mClient.wBottom - 72, 
        mClient.wLeft + 100, mClient.wBottom - 52);

    Add(new PegTextButton(Rect, "Left", 0, TJ_LEFT|AF_ENABLED));
    Rect.Shift(0, 24);
    Add(new PegTextButton(Rect, "Center", 0, TJ_CENTER|AF_ENABLED));
    Rect.Shift(0, 24);
    Add(new PegTextButton(Rect, "Right", 0, TJ_RIGHT|AF_ENABLED));
    Rect.Shift(116, -48);
    Rect.wBottom += 24;
    PegMLTextButton *pMLButton = new PegMLTextButton(Rect, "Multiple|Line", '|', 10);
    pMLButton->SetFont(&SysFont);
    Add(pMLButton);

    Rect.Shift(0, 48);
    Rect.wBottom -= 24;
    Add(new PegTextButton(Rect, "OK", IDB_CLOSE));
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
    Add(new PegTitle("Floating Window", 0));
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
            PegMessage NewMessage(CHILD_CLONE_CLOSED);
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



