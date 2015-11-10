/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  
//    Copyright (c) Swell Software, Inc.
//                 3022 Lindsay Ln
//                 Port Huron, MI 48060
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "popmenu.hpp"


void PegAppInitialize(PegPresentationManager *pPresent)
{
    pPresent->Add(new PopupMenuWin(10, 10));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PopupMenuWin::PopupMenuWin(SIGNED iLeft, SIGNED iTop) : 
    PegDialog("Popup Menu Window")
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 242, iTop + 210);
    InitClient();

    ChildRect.Set(iLeft + 19, iTop + 176, iLeft + 98, iTop + 195);
    PegTextButton *pButton = new PegTextButton(ChildRect, "Menu...", IDB_POPMENU);
    pButton->SetSignals(pButton->GetSignals()|SIGMASK(PSF_FOCUS_LOST));
    Add(pButton);

    /* WB End Construction */
    
    mpMenu = NULL;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PopupMenuWin::~PopupMenuWin()
{
    if (mpMenu)
    {
        if (!mpMenu->StatusIs(PSF_VISIBLE))
        {
            Destroy(mpMenu);
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PegMenuDescription Dummy3_3[] = {
{"Dummy3_3_1", 0, AF_ENABLED|BF_DOTABLE, NULL},
{"Dummy3_3_2", 0, AF_ENABLED|BF_DOTABLE, NULL},
{"Dummy3_3_3", 0, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription Dummy1[] = {
{"Dummy1_1", 0, AF_ENABLED|BF_DOTABLE, NULL},
{"Dummy1_2", 0, AF_ENABLED|BF_DOTABLE, NULL},
{"Dummy1_3", 0, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription Dummy2[] = {
{"Dummy2_1", 0, AF_ENABLED|BF_DOTABLE, NULL},
{"Dummy2_2", 0, AF_ENABLED|BF_DOTABLE, NULL},
{"Dummy2_3", 0, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription Dummy3[] = {
{"Dummy3_3", 0, AF_ENABLED|BF_DOTABLE|BF_DOTTED, Dummy3_3},
{"Dummy3_2", 0, AF_ENABLED|BF_DOTABLE, NULL},
{"Dummy3_1", 0, AF_ENABLED|BF_DOTABLE, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription ClientColorOptions[] = {
{"Black",     PopupMenuWin::IDB_SET_COLOR_BLACK,     AF_ENABLED|BF_DOTABLE, NULL},
{"LightGray", PopupMenuWin::IDB_SET_COLOR_LIGHTGRAY, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"Red",       PopupMenuWin::IDB_SET_COLOR_RED,       AF_ENABLED|BF_DOTABLE, NULL},
{"LightBlue", PopupMenuWin::IDB_SET_COLOR_BLUE,      AF_ENABLED|BF_DOTABLE, NULL},
{"Dark gray", PopupMenuWin::IDB_SET_COLOR_DARKGRAY,  AF_ENABLED|BF_DOTABLE, NULL},
{"", 0, 0, NULL}
};

static PegMenuDescription OptionsMenu[] = {
{"Dummy1", 0, AF_ENABLED, Dummy1},
{"Dummy2", 0, AF_ENABLED, Dummy2},
{"Dummy3", 0, AF_ENABLED, Dummy3},
{"Client Color", 0, AF_ENABLED, ClientColorOptions},
{"", 0, 0, NULL}
};



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PopupMenuWin::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(IDB_POPMENU, PSF_CLICKED):
        // Enter your code here:
        AddPopupMenu();
        break;

    case PM_NONCURRENT:
    case SIGNAL(IDB_POPMENU, PSF_FOCUS_LOST):
        // If the window or text button loses focus, 
        // we want to be sure to remove the popup menu:
        RemovePopupMenu();
        PegThing::Message(Mesg);
        break;

    // catch the popup-menu messages:

    case SIGNAL(IDB_SET_COLOR_BLACK, PSF_DOT_ON):
        SetClientColor(BLACK);
        break;

    case SIGNAL(IDB_SET_COLOR_LIGHTGRAY, PSF_DOT_ON):
        SetClientColor(LIGHTGRAY);
        break;

    case SIGNAL(IDB_SET_COLOR_RED, PSF_DOT_ON):
        SetClientColor(RED);
        break;

    case SIGNAL(IDB_SET_COLOR_BLUE, PSF_DOT_ON):
        SetClientColor(CYAN);
        break;

    case SIGNAL(IDB_SET_COLOR_DARKGRAY, PSF_DOT_ON):
        SetClientColor(DARKGRAY);
        break;

    default:
        return PegDialog::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PopupMenuWin::AddPopupMenu(void)
{
    if (!mpMenu)
    {
        // first time, create the menu:
        mpMenu = new PegMenu(OptionsMenu, TRUE);

        // tell the menu who to send messages to:
        mpMenu->SetOwner(this);
    }

    // position the menu to line up with the button, right below
    // the window:

    PegMessage NewMessage(NULL, PM_SIZE);
    NewMessage.Rect = mpMenu->GetMinSize();
    NewMessage.Rect.MoveTo(mReal.wLeft + 19, mReal.wBottom + 2);
    mpMenu->Message(NewMessage);
    Presentation()->Add(mpMenu);
    //Presentation()->MoveFocusTree(mpMenu);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PopupMenuWin::RemovePopupMenu(void)
{
    if (mpMenu)
    {
        if (mpMenu->Parent())
        {
            mpMenu->Parent()->Remove(mpMenu);
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PopupMenuWin::SetClientColor(COLORVAL Color)
{
    SetColor(PCI_NORMAL, Color);
    Invalidate();
    Draw();
}


