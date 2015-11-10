/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ptitle.cpp - PegTitle class implementation.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2000 Swell Software 
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


extern PegBitmap gbSysButtonBitmap;
extern PegBitmap gbCloseButtonBitmap;
extern PegBitmap gbMaxButtonBitmap;
extern PegBitmap gbRestoreButtonBitmap;
extern PegBitmap gbMinButtonBitmap;

extern PEGCHAR lsNULL[];
extern PEGCHAR lsCLOSE[];
extern PEGCHAR lsMINIMIZE[];
extern PEGCHAR lsMAXIMIZE[];
extern PEGCHAR lsRESTORE[];


#ifdef PEG_RUNTIME_COLOR_CHECK
extern PegBitmap gbMixPatternBitmap;
#else
#if (PEG_NUM_COLORS == 2 && defined(MONO_INDICATE_TITLE_FOCUS))
extern PegBitmap gbMixPatternBitmap;
#endif
#endif


/*--------------------------------------------------------------------------*/
PegTitle::PegTitle(const PEGCHAR *Title, WORD wFlags) : PegThing(0, wFlags),
    PegTextThing(Title, wFlags & TT_COPY, PEG_TITLE_FONT)
{

static PegMenuDescription DefSystemMenu[] = {
{lsCLOSE,    IDB_CLOSE, AF_ENABLED, NULL},
{lsNULL,         0,       BF_SEPERATOR, NULL},
{lsMINIMIZE, IDB_MIN, AF_ENABLED, NULL},
{lsMAXIMIZE, IDB_MAX, AF_ENABLED, NULL},
{lsNULL, 0, 0, NULL}
};

    miHeight = mpFont->uHeight + 2;

    if (miHeight < SYS_BUTTON_SIZE + 2)
    {
        miHeight = SYS_BUTTON_SIZE + 2;
    }
    mReal.Set(0, 0, 100, miHeight);
    mClient = mReal;
    Type(TYPE_TITLE);


   #ifdef PEG_RUNTIME_COLOR_CHECK
    if (NumColors() < 4)
    {
        muColors[PCI_NORMAL] = WHITE;
        muColors[PCI_SELECTED] = WHITE;
        muColors[PCI_STEXT] = BLACK;
        muColors[PCI_NTEXT] = BLACK;
    }
    else
    {
        muColors[PCI_NORMAL] = PCLR_INACTIVE_TITLE;
        muColors[PCI_SELECTED] = PCLR_ACTIVE_TITLE;
        muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
        muColors[PCI_STEXT] = PCLR_HIGH_TEXT;
    }

   #else
    muColors[PCI_NORMAL] = PCLR_INACTIVE_TITLE;
    muColors[PCI_SELECTED] = PCLR_ACTIVE_TITLE;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_STEXT] = PCLR_HIGH_TEXT;
   #endif

    AddStatus(PSF_SIZEABLE | PSF_NONCLIENT);
    PegRect Rect;

    if (wFlags & TF_SYSBUTTON)
    {
        mpMenu = new PegMenu(DefSystemMenu);
        mpMenu->SetOwner(this);

        Rect.Set(2, 2, 2 + SYS_BUTTON_SIZE - 3, 2 + SYS_BUTTON_SIZE - 3);
        mpSysButton = new PegSystemButton(Rect, &gbSysButtonBitmap,
            IDB_SYSTEM);
        mpSysButton->AddStatus(PSF_NONCLIENT);
        mpSysButton->RemoveStatus(PSF_ACCEPTS_FOCUS);
        Add(mpSysButton);
    }
    else
    {
        mpSysButton = NULL;
        mpMenu = NULL;
    }

    if (wFlags & TF_CLOSEBUTTON)
    {
        Rect.Set(2, 2, 2 + SYS_BUTTON_SIZE - 3, 2 + SYS_BUTTON_SIZE - 3);
        mpCloseButton = new PegBitmapButton(Rect, &gbCloseButtonBitmap,
            IDB_CLOSE);
        mpCloseButton->AddStatus(PSF_NONCLIENT);
        mpCloseButton->RemoveStatus(PSF_ACCEPTS_FOCUS);
        Add(mpCloseButton);
    }
    else
    {
        mpCloseButton = NULL;

        if (mpMenu)
        {
            PegMenuButton *pButton = mpMenu->FindButton(lsCLOSE);
            if (pButton)
            {
                pButton->SetEnabled(FALSE);
            }
        }
    }

    if (wFlags & TF_MINMAXBUTTON)
    {
        PegRect Rect;
        Rect.Set(0, 0, SYS_BUTTON_SIZE, SYS_BUTTON_SIZE);
        mpMaxButton = new PegBitmapButton(Rect, &gbMaxButtonBitmap,
            IDB_MAX);
        mpMinButton = new PegBitmapButton(Rect, &gbMinButtonBitmap,
            IDB_MIN);
        mpMaxButton->AddStatus(PSF_NONCLIENT);
        mpMinButton->AddStatus(PSF_NONCLIENT);
        mpMaxButton->RemoveStatus(PSF_ACCEPTS_FOCUS);
        mpMinButton->RemoveStatus(PSF_ACCEPTS_FOCUS);
        Add(mpMaxButton);
        Add(mpMinButton);
    }
    else
    {
        mpMaxButton = NULL;
        mpMinButton = NULL;
        if (mpMenu)
        {
            PegMenuButton *pButton = mpMenu->FindButton(lsMINIMIZE);
            if (pButton)
            {
                pButton->SetEnabled(FALSE);
            }
            pButton = mpMenu->FindButton(lsMAXIMIZE);
            if (pButton)
            {
                pButton->SetEnabled(FALSE);
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegTitle::~PegTitle()
{
    if (mpMenu)
    {
        Destroy(mpMenu);
    }
}

/*--------------------------------------------------------------------------*/
SIGNED PegTitle::Message(const PegMessage &Mesg)
{
    PegMessage NewMessage;

    switch(Mesg.wType)
    {
    case PM_SHOW:
    case PM_PARENTSIZED:
        // make myself the right size:
        SizeToParent();
        PegThing::Message(Mesg);
        break;

    case PM_SIZE:
        {
        PegThing::Message(Mesg);
        
        NewMessage.wType = PM_SIZE;
        NewMessage.Rect = mReal;
        NewMessage.Rect.wTop += 2;
        NewMessage.Rect.wBottom = NewMessage.Rect.wTop + SYS_BUTTON_SIZE - 1;
        NewMessage.Rect.wLeft = mReal.wRight;

        // keep the system buttons at my right:

        if (mpCloseButton)
        {
            NewMessage.Rect.wRight -= 2;
            NewMessage.Rect.wLeft = NewMessage.Rect.wRight - (SYS_BUTTON_SIZE - 1);
            mpCloseButton->Message(NewMessage);
            NewMessage.Rect.wRight = NewMessage.Rect.wLeft;
        }

        if (mpMaxButton)
        {
            NewMessage.Rect.wRight    -= 2;
            NewMessage.Rect.wLeft = NewMessage.Rect.wRight - (SYS_BUTTON_SIZE - 1);
            mpMaxButton->Message(NewMessage);
            NewMessage.Rect.wRight = NewMessage.Rect.wLeft;
        }

        if (mpMinButton)
        {
            NewMessage.Rect.wRight -= 1;
            NewMessage.Rect.wLeft = NewMessage.Rect.wRight - (SYS_BUTTON_SIZE - 1);
            mpMinButton->Message(NewMessage);
        }
        mClient.wRight = NewMessage.Rect.wLeft - 4;
        }
        break;

    case PM_LBUTTONDOWN:
        {
            if (mpMenu)
            {
                if (mpMenu->Parent())
                {
                    mpMenu->Parent()->Remove(mpMenu);
                }
            }

            PegWindow *pWin = (PegWindow *) Parent();
            if (!pWin->IsMaximized())
            {
                NewMessage.wType = PM_BEGIN_MOVE;
                    NewMessage.iData = PMM_MOVEALL;
                NewMessage.Point = Mesg.Point;
                Parent()->Message(NewMessage);
            }
        }
        break;

    case PEG_SIGNAL(IDB_SYSTEM, PSF_CLICKED):
        {
        PegMessage NewMessage(NULL, PM_SIZE);
        NewMessage.Rect = mpMenu->GetMinSize();
        NewMessage.Rect.MoveTo(mReal.wLeft, mReal.wBottom + 2);
        mpMenu->Message(NewMessage);
        Presentation()->Add(mpMenu);
        }
        break;

    case PM_NONCURRENT:
        if (mpMenu)
        {
            if (mpMenu->Parent())
            {
                mpMenu->Parent()->Remove(mpMenu);
            }
        }
        PegThing::Message(Mesg);
        break;

    case PEG_SIGNAL(IDB_MAX, PSF_CLICKED):
        if (mpMenu)
        {
            if (mpMenu->Parent())
            {
                mpMenu->Parent()->Remove(mpMenu);
            }
        }
        NewMessage.wType = PM_MAXIMIZE;
        return(Parent()->Message(NewMessage));

    case PEG_SIGNAL(IDB_RESTORE, PSF_CLICKED):
        if (mpMenu)
        {
            if (mpMenu->Parent())
            {
                mpMenu->Parent()->Remove(mpMenu);
            }
        }
        NewMessage.wType = PM_RESTORE;
        return(Parent()->Message(NewMessage));

    case PEG_SIGNAL(IDB_MIN, PSF_CLICKED):
        if (mpMenu)
        {
            if (mpMenu->Parent())
            {
                mpMenu->Parent()->Remove(mpMenu);
            }
        }

        NewMessage.wType = PM_MINIMIZE;
        return(Parent()->Message(NewMessage));

    case PEG_SIGNAL(IDB_CLOSE, PSF_CLICKED):
        NewMessage.wType = PM_CLOSE;
        return(Parent()->Message(NewMessage));

    default:
        PegThing::Message(Mesg);
        break;
    }
    return(0);
}

#ifndef PEG_RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegTitle::Draw(void)
{
    BeginDraw();
    PegColor Color;
    WORD wTemp;

   #if (PEG_NUM_COLORS >= 4)

    if (Parent()->StatusIs(PSF_CURRENT))
    {
        Color.Set(muColors[PCI_SELECTED], muColors[PCI_SELECTED], CF_FILL);
    }
    else
    {
        Color.Set(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    }

    Rectangle(mReal, Color, 0);

   #else

    #ifdef MONO_INDICATE_TITLE_FOCUS
     if (Parent()->StatusIs(PSF_CURRENT))
     {
        Color.Set(PCLR_BORDER, muColors[PCI_SELECTED], CF_FILL);
        Rectangle(mReal, Color, 1);
        Color.uForeground = PCLR_NORMAL_TEXT;
     }
     else
     {
        BitmapFill(mReal, &gbMixPatternBitmap);
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
     }
    #else
     Color.Set(PCLR_BORDER, muColors[PCI_NORMAL], CF_FILL);
     Rectangle(mReal, Color, 1);
     Color.uForeground = muColors[PCI_NTEXT];
    #endif        
   #endif   // NUM_COLORS >= 4 if

    // Draw the Text:

    if (mpText)
    {
        wTemp = mClip.wRight;

        if (mClip.wRight > mClient.wRight)
        {
            mClip.wRight = mClient.wRight;
        }

       #if (PEG_NUM_COLORS >= 4)

        if (Parent()->StatusIs(PSF_CURRENT))
        {
            Color.uForeground = muColors[PCI_STEXT];
        }
        else
        {
            Color.uForeground = muColors[PCI_NTEXT];
        }
       #else
         #ifndef MONO_INDICATE_TITLE_FOCUS
             Color.uForeground = muColors[PCI_NTEXT];
         #endif
       #endif
       
        Color.uFlags = 0;
       
        PegPoint Point;

        if (mpSysButton)
        {
            Point.x = mpSysButton->mReal.wRight + 4;
        }
        else
        {
            Point.x = mReal.wLeft + 4;
        }

        Point.y = mReal.wTop + 2;
        DrawText(Point, mpText, Color, mpFont);
        mClip.wRight = wTemp;
    }
    PegThing::Draw();
    EndDraw();
}

#else    // here for drawing with run-time color checking enabled

/*--------------------------------------------------------------------------*/
void PegTitle::Draw(void)
{
    BeginDraw();
    PegColor Color;
    WORD wTemp;

    if (NumColors() >= 4)
    {
        if (Parent()->StatusIs(PSF_CURRENT))
        {
            Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_FILL);
        }
        else
        {
            Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
        }
        Rectangle(mReal, Color, 0);
    }
    else
    {
	    if (Parent()->StatusIs(PSF_CURRENT))
	    {
	       Color.Set(BLACK, muColors[PCI_SELECTED], CF_FILL);
	       Rectangle(mReal, Color, 1);
	       Color.uForeground = muColors[PCI_STEXT];
	    }
	    else
	    {
	       BitmapFill(mReal, &gbMixPatternBitmap);
	       Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
	    }
    }

    // Draw the Text:

    if (mpText)
    {
        wTemp = mClip.wRight;

        if (mClip.wRight > mClient.wRight)
        {
            mClip.wRight = mClient.wRight;
        }

        if (NumColors() >= 4)
        {
	        if (Parent()->StatusIs(PSF_CURRENT))
	        {
	            Color.uForeground = muColors[PCI_STEXT];
	        }
	        else
	        {
                 Color.uForeground = muColors[PCI_NTEXT];
	        }
        }
       
        Color.uFlags = 0;
       
        PegPoint Point;

        if (mpSysButton)
        {
            Point.x = mpSysButton->mReal.wRight + 4;
        }
        else
        {
            Point.x = mReal.wLeft + 4;
        }

        Point.y = mReal.wTop + 2;
        DrawText(Point, mpText, Color, mpFont);
        mClip.wRight = wTemp;
    }
    PegThing::Draw();
    EndDraw();
}

#endif  // alternate draw function if

/*--------------------------------------------------------------------------*/
void PegTitle::SizeToParent(void)
{
    if (!Parent())
    {
        return;
    }

    PegWindow *pWin = (PegWindow *) Parent();

    if (pWin->IsMaximized())
    {
        if (mpMaxButton)
        {
            mpMaxButton->SetBitmap(&gbRestoreButtonBitmap);
            mpMaxButton->Id(IDB_RESTORE);
        }
        if (mpMenu)
        {
            PegMenuButton *pButton = mpMenu->FindButton(lsMAXIMIZE);
            if (pButton)
            {
                pButton->DataSet(lsRESTORE);
                pButton->Id(IDB_RESTORE);
            }
        }
    }
    else
    {
        if (mpMaxButton)
        {
            mpMaxButton->SetBitmap(&gbMaxButtonBitmap);
            mpMaxButton->Id(IDB_MAX);
        }
        if (mpMenu)
        {
            PegMenuButton *pButton = mpMenu->FindButton(lsRESTORE);
            if (pButton)
            {
                pButton->DataSet(lsMAXIMIZE);
                pButton->Id(IDB_MAX);
            }
        }
    }

    PegRect NewSize = Parent()->mReal;

    if (pWin->Style() & FF_THICK)
    {
        NewSize.wTop += PEG_FRAME_WIDTH - 1;
        NewSize.wLeft += PEG_FRAME_WIDTH - 1;
        NewSize.wRight -= PEG_FRAME_WIDTH - 2;
    }
    else
    {
        if (!(pWin->Style() & FF_NONE))
        {
            NewSize.wTop += 2;
            NewSize.wLeft += 2;
           #ifdef PEG_NUM_COLORS
            #if PEG_NUM_COLORS >= 4
            NewSize.wRight -= 3;
            #else
            NewSize.wRight -= 2;
            #endif
           #else
            NewSize.wRight -= 3;
           #endif
        }
    }
    NewSize.wBottom = NewSize.wTop + miHeight;
    PegMessage NewMessage(PM_SIZE);
    NewMessage.Rect = NewSize;
    Message(NewMessage);
}

/*--------------------------------------------------------------------------*/
void PegTitle::AssignMenu(PegMenuDescription *pMenu)
{
    if (pMenu && mpMenu)
    {
        Destroy(mpMenu);
        mpMenu = new PegMenu(pMenu);
        mpMenu->SetOwner(this);
    }
}

// End of file
