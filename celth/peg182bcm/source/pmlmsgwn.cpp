/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pmlmsgwin.cpp - Multiline popup message window class definition.
//
// Author: Jim DeLisle
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

#include "string.h"
#include "peg.hpp"

extern PEGCHAR lsOK[];
extern PEGCHAR lsNO[];
extern PEGCHAR lsYES[];
extern PEGCHAR lsABORT[];
extern PEGCHAR lsCANCEL[];
extern PEGCHAR lsRETRY[];
extern PEGCHAR lsTEST[];

static const SIGNED ICON_PAD = 12;
static const SIGNED TEXT_PAD = 8;

/*--------------------------------------------------------------------------*/
PegMLMessageWindow::PegMLMessageWindow(const PegRect &Rect, 
    const PEGCHAR *Title, const PEGCHAR *Message, WORD wStyle, 
    PegBitmap *pIcon, PegThing *Owner) :
    PegWindow(Rect, wStyle)
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

    if (Message)
    {
        CalcButtonTop();
        PegRect tRect = mClient;
        tRect.wBottom = miButtonTop - TEXT_PAD;
        if(mpIcon)
        {
            tRect.wTop += ICON_PAD;
            tRect.wLeft += mpIcon->wWidth + (ICON_PAD * 2);
        }
        else
        {
            tRect.wTop += TEXT_PAD;
            tRect.wLeft += TEXT_PAD;
        }
        tRect.wRight -= TEXT_PAD;

        mpTextBox = new PegTextBox(tRect, 0, FF_NONE | EF_WRAP | TJ_LEFT | 
                                   (mwStyle & TT_COPY), (PEGCHAR*)Message, 
                                   strlen(Message) + 1);
        mpTextBox->SetFont(PegTextThing::GetDefaultFont(PEG_MESGWIN_FONT));
        mpTextBox->SetColor(PCI_NORMAL, GetColor(PCI_NORMAL));
        SIGNED iLineHeight = TextHeight(lsTEST, mpTextBox->GetFont());
        SIGNED iNumLines = mpTextBox->LineCount();
        SIGNED iGrow = 0;
        while((iLineHeight * iNumLines) > tRect.Height())
        {
            tRect.wBottom += iLineHeight + 2;
            iGrow += iLineHeight + 2;
        }

        if(iGrow)
        {
            mpTextBox->Resize(tRect);

            tRect = mReal;
            tRect.wBottom += iGrow;
            Resize(tRect);
        }
        Add(mpTextBox);
    }
    else
    {
        mpTextBox = NULL;
    }

    AddButtons();
}


/*--------------------------------------------------------------------------*/
PegMLMessageWindow::PegMLMessageWindow(const PEGCHAR *Title, 
    const PEGCHAR *Message, WORD wOptions, PegBitmap *pIcon, PegThing *Owner) :
    PegWindow(wOptions)
{
    muColors[PCI_NORMAL] = PCLR_DIALOG;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;

    mpOwner = Owner;
    mpIcon = pIcon;

    SIGNED iTitleWidth = 0;
    SIGNED iSize = 0;

    if (wOptions & (MW_OK|MW_YES|MW_NO|MW_ABORT|MW_RETRY|MW_CANCEL))
    {
        iSize = TextHeight(lsOK, 
                           PegTextThing::GetDefaultFont(PEG_TBUTTON_FONT));
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

    CalcButtonWidth();

    iSize = iTitleWidth > miButtonWidth ? iTitleWidth : miButtonWidth;

    if (!iSize)
    {
        iSize = 80;
    }

    if(mpIcon)
    {
        iSize += (ICON_PAD * 2) + mpIcon->wWidth;
        mReal.wBottom += (ICON_PAD * 2) + mpIcon->wHeight;
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

    if (Message)
    {
        CalcButtonTop();
        PegRect tRect = mClient;
        tRect.wBottom = miButtonTop - TEXT_PAD;
        if(mpIcon)
        {
            tRect.wTop += ICON_PAD;
            tRect.wLeft += mpIcon->wWidth + (ICON_PAD * 2);
        }
        else
        {
            tRect.wTop += TEXT_PAD;
            tRect.wLeft += TEXT_PAD;
        }
        tRect.wRight -= TEXT_PAD;

        mpTextBox = new PegTextBox(tRect, 0, FF_NONE | EF_WRAP | TJ_LEFT | 
                                   (mwStyle & TT_COPY), (PEGCHAR*)Message, 
                                   strlen(Message) + 1);
        mpTextBox->SetFont(PegTextThing::GetDefaultFont(PEG_MESGWIN_FONT));
        mpTextBox->SetColor(PCI_NORMAL, GetColor(PCI_NORMAL));
        SIGNED iLineHeight = TextHeight(lsTEST, mpTextBox->GetFont());
        SIGNED iNumLines = mpTextBox->LineCount();
        SIGNED iGrow = 0;
        while((iLineHeight * iNumLines) > tRect.Height())
        {
            tRect.wBottom += iLineHeight + 2;
            iGrow += iLineHeight + 2;
        }

        if(iGrow)
        {
            mpTextBox->Resize(tRect);

            tRect = mReal;
            tRect.wBottom += iGrow;
            Resize(tRect);
        }
        Add(mpTextBox);
    }
    else
    {
        mpTextBox = NULL;
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
PegMLMessageWindow::~PegMLMessageWindow()
{
}

/*--------------------------------------------------------------------------*/
SIGNED PegMLMessageWindow::Message(const PegMessage &Mesg)
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
void PegMLMessageWindow::Draw()
{
    BeginDraw();

    PegWindow::Draw();

    if(mpIcon)
    {
        PegPoint tPoint;
        tPoint.x = mClient.wLeft + ICON_PAD;
        tPoint.y = mClient.wTop + ICON_PAD;
        Bitmap(tPoint, mpIcon);
    }

    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegMLMessageWindow::CalcButtonWidth()
{
    miOneButtonWidth = TextWidth(lsCANCEL, 
                                 PegTextThing::GetDefaultFont(PEG_TBUTTON_FONT))
                                 + 8;
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
void PegMLMessageWindow::CalcButtonTop()
{
    miButtonTop = mClient.wBottom;

    if (mwStyle & (MW_OK|MW_YES|MW_NO|MW_ABORT|MW_RETRY|MW_CANCEL))
    {
        miButtonTop -= TextHeight(lsOK, 
                       PegTextThing::GetDefaultFont(PEG_TBUTTON_FONT)) 
                       + (TEXT_PAD * 2);
    }
}

/*--------------------------------------------------------------------------*/
void PegMLMessageWindow::AddButtons()
{
    WORD wMask = MW_OK;
    SIGNED iSpare = (mClient.Width() - miButtonWidth) / 2;
    iSpare += mClient.wLeft;
    PegRect ButtonRect;

    CalcButtonTop();
    ButtonRect.Set(iSpare, miButtonTop, iSpare + miOneButtonWidth,
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



