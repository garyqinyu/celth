/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  
//    Copyright (c) swell Software
//                 3022 Lindsay Ln
//                 Port Huron, MI 48060
//  
//    All Rights Reserved
//  
//  Notes:
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "peg.hpp"
#include "Exam2.hpp"


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
ExampleTwo::ExampleTwo(SIGNED iLeft, SIGNED iTop) : PegDecoratedWindow()
{
    static PegMenuDescription HelpMenu [] = {
    {"Index",  ExampleTwo::IDB_HELPINDEX,  AF_ENABLED|BF_CHECKABLE, NULL},
    {"About",  ExampleTwo::IDB_ABOUT,  AF_ENABLED, NULL},
    {"", 0, 0, NULL}
    };
    static PegMenuDescription FileMenu [] = {
    {"Close",  ExampleTwo::IDB_CLOSE,  AF_ENABLED, NULL},
    {"Command",  0,  BF_SEPERATOR, NULL},
    {"Save",  ExampleTwo::IDB_FILESAVE,  AF_ENABLED, NULL},
    {"Open",  ExampleTwo::IDB_OPENFILE,  AF_ENABLED, NULL},
    {"", 0, 0, NULL}
    };
    static PegMenuDescription TopMenu [] = {
    {"Help",  0,  AF_ENABLED, HelpMenu},
    {"File",  0,  AF_ENABLED, FileMenu},
    {"", 0, 0, NULL}
    };
    PegRect ChildRect;
    PegThing *pChild1;

    mReal.Set(111, 93, 462, 369);
    InitClient();
    SetColor(PCI_NORMAL, CYAN);
    Add (new PegTitle("Example Two", TF_CLOSEBUTTON));
    Add(new PegMenuBar(TopMenu));

    ChildRect.Set(282, 145, 311, 164);
    Add(new PegPrompt(ChildRect, "/sec"));
    ChildRect.Set(222, 145, 275, 164);
    mpRatePrompt = new PegPrompt(ChildRect, "500", IDP_RATEVAL, FF_RECESSED|TJ_RIGHT|TT_COPY);
    mpRatePrompt->SetColor(PCI_NORMAL, BLACK);
    mpRatePrompt->SetColor(PCI_NTEXT, LIGHTGREEN);
    Add(mpRatePrompt);

    ChildRect.Set(127, 145, 212, 164);
    pChild1 = new PegPrompt(ChildRect, "Sample Rate");
    pChild1->SetColor(PCI_NORMAL, DARKGRAY);
    Add(pChild1);

    ChildRect.Set(126, 179, 312, 205);
    pChild1 = new PegSlider(ChildRect, 100, 1000, IDSL_RATE, FF_RECESSED, 50);
    ((PegSlider *) pChild1)->SetCurrentValue(500);
    Add(pChild1);

    ChildRect.Set(334, 260, 449, 354);
    pChild1 = new PegGroup(ChildRect, "TimeStamp");
    ChildRect.Set(346, 326, 412, 345);
    pChild1->Add(new PegRadioButton(ChildRect, "Daily", IDRB_DAILY));
    ChildRect.Set(346, 303, 408, 322);
    pChild1->Add(new PegRadioButton(ChildRect, "Hourly", IDRB_HOURLY));
    ChildRect.Set(346, 280, 411, 299);
    pChild1->Add(new PegRadioButton(ChildRect, "Minute", IDRB_MINUTE, FF_RAISED|AF_ENABLED|BF_SELECTED));
    Add(pChild1);

    ChildRect.Set(347, 233, 436, 252);
    pChild1 = new PegCheckBox(ChildRect, "All Data", IDRB_ALLDATA, FF_RAISED|AF_ENABLED|BF_SELECTED);
    pChild1->SetColor(PCI_NORMAL, WHITE);
    pChild1->SetColor(PCI_NTEXT, WHITE);
    Add(pChild1);

    ChildRect.Set(352, 188, 431, 214);
    pChild1 = new PegTextButton(ChildRect, "Stop", IDB_STOPBUTTON);
    pChild1->SetColor(PCI_NORMAL, LIGHTRED);
    Add(pChild1);

    ChildRect.Set(352, 150, 431, 176);
    pChild1 = new PegTextButton(ChildRect, "Start", IDB_STARTBUTTON);
    pChild1->SetColor(PCI_NORMAL, LIGHTGREEN);
    Add(pChild1);

    ChildRect.Set(128, 223, 314, 354);
    pChild1 = new PegWindow(ChildRect, FF_RECESSED);;
    pChild1->SetColor(PCI_NORMAL, BLACK);
    ((PegWindow *) pChild1)->SetScrollMode(WSM_HSCROLL);
    Add(pChild1);

}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
SIGNED ExampleTwo::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(IDB_HELPINDEX, PSF_CHECK_ON):
        // Enter your code here:
        break;

    case SIGNAL(IDB_HELPINDEX, PSF_CHECK_OFF):
        // Enter your code here:
        break;

    case SIGNAL(IDB_ABOUT, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(IDB_CLOSE, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(IDB_FILESAVE, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(IDB_OPENFILE, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(IDSL_RATE, PSF_SLIDER_CHANGE):
        // Enter your code here:
        break;

    case SIGNAL(IDRB_ALLDATA, PSF_CHECK_ON):
        // Enter your code here:
        break;

    case SIGNAL(IDRB_ALLDATA, PSF_CHECK_OFF):
        // Enter your code here:
        break;

    case SIGNAL(IDB_STOPBUTTON, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(IDB_STARTBUTTON, PSF_CLICKED):
        // Enter your code here:
        break;

    default:
        return PegDecoratedWindow::Message(Mesg);
    }
    return 0;
}

