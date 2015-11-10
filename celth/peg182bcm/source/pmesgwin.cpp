/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pmesgwin.cpp - Message Window class definition.
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

extern PEGCHAR lsOK[];
extern PEGCHAR lsNO[];
extern PEGCHAR lsYES[];
extern PEGCHAR lsABORT[];
extern PEGCHAR lsCANCEL[];
extern PEGCHAR lsRETRY[];

static const SIGNED ICON_PAD = 12;

/*--------------------------------------------------------------------------*/
PegMessageWindow::PegMessageWindow(const PegRect &Rect, const PEGCHAR *Title, 
    const PEGCHAR *Message, WORD wStyle, PegBitmap *pIcon, PegThing *Owner) :
    PegWindow(Rect, wStyle),
    PegTextThing(Message, wStyle & TT_COPY, PEG_MESGWIN_FONT)
{
    Type(TYPE_MESSAGE);
    muColors[PCI_NORMAL] = PCLR_DIALOG;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;

    if (Title)
    {
        mpTitle = new PegTitle(Title, TT_COPY);
        Add(mpTitle);
        mClient.wTop += mpTitle->mReal.Height();
        AddStatus(PSF_MOVEABLE);
    }
    else
    {
        mpTitle = NULL;
    }
    mpOwner = Owner;
    mpIcon = pIcon;

    CalcButtonWidth();
    AddButtons();

    if (Message)
    {
        miMessageWidth = TextWidth(Message, mpFont);
    }
    else
    {
        miMessageWidth = 0;
    }
}


/*--------------------------------------------------------------------------*/
PegMessageWindow::PegMessageWindow(const PEGCHAR *Title, const PEGCHAR *Message,
    WORD wOptions, PegBitmap *pIcon, PegThing *Owner) :
    PegWindow(wOptions),
    PegTextThing(Message, wOptions & TT_COPY, PEG_MESGWIN_FONT)
{
    muColors[PCI_NORMAL] = PCLR_DIALOG;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;

    SIGNED iTitleWidth = 0;
    miMessageWidth = 0;
    SIGNED iSize = 0;

    if (wOptions & (MW_OK|MW_YES|MW_NO|MW_ABORT|MW_RETRY|MW_CANCEL))
    {
        iSize = TextHeight(lsOK, GetDefaultFont(PEG_TBUTTON_FONT));
    }
    mReal.wBottom = mReal.wTop + iSize + 26;
    mReal.wRight = mReal.wLeft + 80;
    InitClient();

    if (Title)
    {
        mpTitle = new PegTitle(Title, TT_COPY);
        Add(mpTitle);
        mClient.wTop += mpTitle->mReal.Height();
        AddStatus(PSF_MOVEABLE);
        iTitleWidth = TextWidth(Title, mpTitle->GetFont()) + 16;
        mReal.wBottom += mpTitle->mReal.Height();
    }
    else
    {
        mpTitle = NULL;
    }
    mpOwner = Owner;
    mpIcon = pIcon;

    if (Message)
    {
        miMessageWidth = TextWidth(Message, mpFont) + 16;
        mReal.wBottom += TextHeight(Message, mpFont) + 8;
    }

    CalcButtonWidth();

    if (iTitleWidth >= miMessageWidth && 
        iTitleWidth >= miButtonWidth)
    {
        iSize = iTitleWidth;
    }
    else
    {
        if (miButtonWidth >= miMessageWidth &&
            miButtonWidth >= iTitleWidth)
        {
            iSize = miButtonWidth;
        }
        else
        {
            iSize = miMessageWidth;
        }
    }
    if (!iSize)
    {
        iSize = 80;
    }

    if(mpIcon)
    {
        SIGNED iDiff = iSize - miMessageWidth;
        if(iDiff < ((ICON_PAD * 2) + mpIcon->wWidth))
        {
            iSize += (((ICON_PAD * 2) + mpIcon->wWidth) - iDiff);
        }

        if(Message)
        {
            mReal.wBottom += mpIcon->wHeight;
        }
        else
        {
            mReal.wBottom += (ICON_PAD * 2) + mpIcon->wHeight;
        }
    }

    mReal.wRight = mReal.wLeft + iSize;

    if (mwStyle & FF_THIN)
    {
        mClient.wBottom = mReal.wBottom - 1;
        mClient.wRight = mReal.wRight - 1;
    }
    else
    {
        if (mwStyle & (FF_RAISED|FF_RECESSED))
        {
            mClient.wBottom = mReal.wBottom - 2;
            mClient.wRight = mReal.wRight - 2;
        }
        else
        {
            if (mwStyle & FF_THICK)
            {
                mClient.wBottom = mReal.wBottom - PEG_FRAME_WIDTH;
                mClient.wRight = mReal.wRight - PEG_FRAME_WIDTH;
            }
            else
            {
                mClient.wBottom = mReal.wBottom;
                mClient.wRight = mReal.wRight;
            }
        }
    }

    if (mpOwner)
    {
        mpOwner->Center(this);
    }
    else
    {
        Presentation()->Center(this);
    }

    // now add the buttons to myself:

    AddButtons();
}


/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegMessageWindow::~PegMessageWindow()
{
}

/*--------------------------------------------------------------------------*/
SIGNED PegMessageWindow::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_CURRENT:
        PegWindow::Message(Mesg);
        if (mpTitle)
        {
            Invalidate(mpTitle->mClip);
            mpTitle->Draw();
        }
        break;

    case PM_NONCURRENT:
        PegWindow::Message(Mesg);
        if (mpTitle)
        {
            Invalidate(mpTitle->mClip);
            mpTitle->Draw();
        }
        break;

    case PEG_SIGNAL(IDB_CLOSE, PSF_CLICKED):
    case PEG_SIGNAL(IDB_OK, PSF_CLICKED):
    case PEG_SIGNAL(IDB_CANCEL, PSF_CLICKED):
    case PEG_SIGNAL(IDB_YES, PSF_CLICKED):
    case PEG_SIGNAL(IDB_NO, PSF_CLICKED):
    case PEG_SIGNAL(IDB_RETRY, PSF_CLICKED):
    case PEG_SIGNAL(IDB_ABORT, PSF_CLICKED):
    case PEG_SIGNAL(IDB_APPLY, PSF_CLICKED):
        if (!mbModal)
        {
            PegMessage NewMessage(mpOwner, PM_MWCOMPLETE);
            NewMessage.iData = Mesg.wType;
            NewMessage.pSource = this;
            mpOwner->Message(NewMessage);
            Destroy(this);
        }
        else
        {
            return(PegWindow::Message(Mesg));
        }
        break;

    case PM_SHOW:
        if (mpTitle)
        {
            PegMessage NewMessage;
            NewMessage.wType = PM_SIZE;
            NewMessage.Rect = mClient;
            NewMessage.Rect.wTop = mReal.wTop + 2;
            NewMessage.Rect.wBottom = NewMessage.Rect.wTop +
                mpTitle->mReal.Height() - 1;
            mpTitle->Message(NewMessage);
        }

        PegWindow::Message(Mesg);

        if (!(mpOwner))
        {
            mpOwner = Parent();
        }
        break;


    default:
        return(PegWindow::Message(Mesg));
    }
    return (0);
}


/*--------------------------------------------------------------------------*/
void PegMessageWindow::Draw(void)
{
    PegPoint Put;
    PegRect OldClip;
    PegColor Color(muColors[PCI_NTEXT], PCLR_DIALOG, 0);

    BeginDraw();

    PegWindow::Draw();

    // Draw the icon
    SIGNED iIconOffset = 0;
    if(mpIcon)
    {
        OldClip = mClip;
        mClip = mClip & mClient;
        iIconOffset = (ICON_PAD * 2) + mpIcon->wWidth - 1;
        Put.x = mClient.wLeft + ICON_PAD;
        Put.y = mClient.wTop + ICON_PAD;
        Bitmap(Put, mpIcon);
        mClip = OldClip;
    }

    // Draw the message text:

    PEGCHAR *Text = DataGet();

    if (Text)
    {
        OldClip = mClip;
        mClip = mClip & mClient;

        if(mpIcon)
        {
            Put.x = mClient.wLeft + iIconOffset;
            Put.y = mClient.wTop + ICON_PAD + 
                    (mpIcon->wHeight / 2) - 4;
        }
        else
        {
            Put.x = mClient.wLeft + (mClient.Width() - miMessageWidth) / 2;
            Put.x += 8;
            Put.y = mClient.wTop + 8;
        }
        
        DrawText(Put, Text, Color, mpFont);
        mClip = OldClip;
    }
    EndDraw();
}


/*--------------------------------------------------------------------------*/
void PegMessageWindow::CalcButtonWidth()
{
    miOneButtonWidth = TextWidth(lsCANCEL, GetDefaultFont(PEG_TBUTTON_FONT)) + 8;
    miButtonWidth = 0;

    WORD wMask = MW_OK;

    while (wMask <= MW_CANCEL)
    {
        if (mwStyle & wMask)
        {
            if (miButtonWidth)
            {
                miButtonWidth += 8;
            }
            miButtonWidth += miOneButtonWidth;
        }
        wMask <<= 1;
    }
}


/*--------------------------------------------------------------------------*/
void PegMessageWindow::AddButtons(void)
{
    WORD wMask = MW_OK;
    SIGNED iSpare = (mClient.Width() - miButtonWidth) / 2;
    iSpare += mClient.wLeft;
    PegRect ButtonRect;
    ButtonRect.Set(iSpare, mClient.wBottom - 
                   TextHeight(lsOK, GetDefaultFont(PEG_TBUTTON_FONT)) - 10,
                   iSpare + miOneButtonWidth,
                   mClient.wBottom - 4);

    while (wMask <= MW_CANCEL)
    {
        if (mwStyle & wMask)
        {
            switch(wMask)
            {
            case MW_OK:
                AddToEnd(new PegTextButton(ButtonRect, lsOK, IDB_OK));
                break;

            case MW_YES:
                AddToEnd(new PegTextButton(ButtonRect, lsYES, IDB_YES));
                break;

            case MW_NO:
                AddToEnd(new PegTextButton(ButtonRect, lsNO, IDB_NO));
                break;

            case MW_ABORT:
                AddToEnd(new PegTextButton(ButtonRect, lsABORT, IDB_ABORT));
                break;

            case MW_RETRY:
                AddToEnd(new PegTextButton(ButtonRect, lsRETRY, IDB_RETRY));
                break;

            case MW_CANCEL:
                AddToEnd(new PegTextButton(ButtonRect, lsCANCEL, IDB_CANCEL));
                break;
            }
            ButtonRect.Shift(miOneButtonWidth + 8, 0);
        }
        wMask <<= 1;
    }
}

/*--------------------------------------------------------------------------*/
/*
$Workfile: pmesgwin.cpp $
$Author: Jim $
$Date: 8/26/:2 4:48p $
$Revision: 5 $
$Log: /peg/source/pmesgwin.cpp $
// 
// 5     8/26/:2 4:48p Jim
// Added handling and drawing the bitmap icon on the window.
*/
/*--------------------------------------------------------------------------*/

// End of file

