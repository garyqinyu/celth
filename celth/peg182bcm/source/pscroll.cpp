/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pscroll.cpp - Horizontal and Vertical scroll bar classes.
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


extern PegBitmap gbUpArrowBitmap;
extern PegBitmap gbDownArrowBitmap;
extern PegBitmap gbLeftArrowBitmap;
extern PegBitmap gbMoreBitmap;
extern PegBitmap gbMixPatternBitmap;

/*--------------------------------------------------------------------------*/
PegVScroll::PegVScroll() : PegThing()
{
    Type(TYPE_VSCROLL);
    AddStatus(PSF_SIZEABLE|PSF_NONCLIENT);
    RemoveStatus(PSF_ACCEPTS_FOCUS);

    PegRect Rect;
    Rect.Set(0, 0, PEG_SCROLL_WIDTH - 1, PEG_SCROLL_WIDTH - 1);
    mpUpButton = new PegBitmapButton(Rect, &gbUpArrowBitmap,
        IDB_UPBUTTON, AF_ENABLED|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT);
    mpDownButton = new PegBitmapButton(Rect, &gbDownArrowBitmap,
        IDB_DOWNBUTTON, AF_ENABLED|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT);
    mpScrollButton = new PegScrollButton(TRUE);

    mpUpButton->AddStatus(PSF_NONCLIENT);
    mpUpButton->RemoveStatus(PSF_ACCEPTS_FOCUS);
    mpDownButton->AddStatus(PSF_NONCLIENT);
    mpDownButton->RemoveStatus(PSF_ACCEPTS_FOCUS);

    Add(mpUpButton);
    Add(mpDownButton);
    Add(mpScrollButton);

    mClient.wTop += PEG_SCROLL_WIDTH;
    mClient.wBottom -= PEG_SCROLL_WIDTH;

    mScrollInfo.wMin = 0;
    mScrollInfo.wStep = 1;
    mScrollInfo.wMax = 1;
    mScrollInfo.wVisible = 1;
    mScrollInfo.wCurrent = 0;
}

/*--------------------------------------------------------------------------*/
PegVScroll::PegVScroll(const PegRect &InRect, PegScrollInfo *si, WORD wId) :
    PegThing(InRect, wId)
{
    Type(TYPE_VSCROLL);
    AddStatus(PSF_SIZEABLE);
    RemoveStatus(PSF_ACCEPTS_FOCUS);

    PegRect Rect;
    Rect.wLeft = mReal.wLeft;
    Rect.wTop = mReal.wTop;
    Rect.wRight = Rect.wLeft + (PEG_SCROLL_WIDTH);
    Rect.wBottom = Rect.wTop + (PEG_SCROLL_WIDTH);

    mpUpButton = new PegBitmapButton(Rect, &gbUpArrowBitmap,
        IDB_UPBUTTON, AF_ENABLED|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT);

    Rect.wBottom = mReal.wBottom;
    Rect.wTop = Rect.wBottom - (PEG_SCROLL_WIDTH);
    mpDownButton = new PegBitmapButton(Rect, &gbDownArrowBitmap,
        IDB_DOWNBUTTON, AF_ENABLED|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT);

    mpScrollButton = new PegScrollButton(TRUE);

    mpUpButton->AddStatus(PSF_NONCLIENT);
    mpUpButton->RemoveStatus(PSF_ACCEPTS_FOCUS);
    mpDownButton->AddStatus(PSF_NONCLIENT);
    mpDownButton->RemoveStatus(PSF_ACCEPTS_FOCUS);

    Add(mpUpButton);
    Add(mpDownButton);
    Add(mpScrollButton);

    mClient = mReal;
    mClient.wTop += PEG_SCROLL_WIDTH;
    mClient.wBottom -= PEG_SCROLL_WIDTH;

    mScrollInfo = *si;
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegVScroll::~PegVScroll()
{
}

/*--------------------------------------------------------------------------*/
SIGNED PegVScroll::Message(const PegMessage &Mesg)
{
    PegMessage NewMessage;
    PegWindow *pParent;

    switch(Mesg.wType)
    {
    case PM_SHOW:
    case PM_PARENTSIZED:
        PegThing::Message(Mesg);

        if (StatusIs(PSF_NONCLIENT))
        {
            pParent = (PegWindow *) Parent();
            pParent->GetVScrollInfo(&mScrollInfo);
            NewMessage.wType = PM_SIZE;
            NewMessage.Rect = Parent()->mClient;
            NewMessage.Rect.wLeft = NewMessage.Rect.wRight + 1;
            NewMessage.Rect.wRight = NewMessage.Rect.wLeft + PEG_SCROLL_WIDTH - 1;
            Message(NewMessage);
        }
        else
        {
            SetScrollButtonPos();
        }
        break;

    case PEG_SIGNAL(IDB_UPBUTTON, PSF_CLICKED):
        NewMessage.iData = mScrollInfo.wCurrent;
        mScrollInfo.wCurrent -= mScrollInfo.wStep;
        CheckScrollLimits();

        if (mScrollInfo.wCurrent != NewMessage.iData)
        {
            NewMessage.lData = mScrollInfo.wCurrent;
            NewMessage.pSource = this;
            if (StatusIs(PSF_NONCLIENT))
            {
                NewMessage.wType = PM_VSCROLL;
                Parent()->Message(NewMessage);
            }
            else
            {
                if (Id())
                {
                    NewMessage.wType = PEG_SIGNAL(Id(), PSF_SCROLL_CHANGE);
                    Parent()->Message(NewMessage);
                }
            }
            SetScrollButtonPos();
            Draw();
        }
        break;

    case PEG_SIGNAL(IDB_DOWNBUTTON, PSF_CLICKED):
        NewMessage.iData = mScrollInfo.wCurrent;
        mScrollInfo.wCurrent += mScrollInfo.wStep;
        CheckScrollLimits();

        if (NewMessage.iData != mScrollInfo.wCurrent)
        {
            NewMessage.lData = mScrollInfo.wCurrent;
            NewMessage.pSource = this;

            if (StatusIs(PSF_NONCLIENT))
            {
                NewMessage.wType = PM_VSCROLL;
                Parent()->Message(NewMessage);
            }
            else
            {
                if (Id())
                {
                    NewMessage.wType = PEG_SIGNAL(Id(), PSF_SCROLL_CHANGE);
                    Parent()->Message(NewMessage);
                }
            }
            SetScrollButtonPos();
            Draw();
        }
        break;

    case PM_VSCROLL:
        // This message comes to me from the scroll button. The lData
        // member contains the absolute scroll value, in pixels.
        // We need to convert this to the parent window Delta amount
        // and pass the message on up to the parent
        {

        LONG lRange = (mScrollInfo.wMax - mScrollInfo.wMin) - mScrollInfo.wVisible + 1;
        lRange *= Mesg.lData - mClient.wTop;
        if (mClient.Height() != Mesg.iData)
        {
            lRange /= (mClient.Height() - Mesg.iData);
        }
        lRange += mScrollInfo.wMin;

        if ((SIGNED) lRange != mScrollInfo.wCurrent)
        {
            NewMessage.iData = mScrollInfo.wCurrent;
            mScrollInfo.wCurrent = (SIGNED) lRange;
            CheckScrollLimits();
            NewMessage.lData = mScrollInfo.wCurrent;
            if ((SIGNED) NewMessage.lData == NewMessage.iData)
            {
                break;
            }
            NewMessage.pSource = this;

            if (StatusIs(PSF_NONCLIENT))
            {
                NewMessage.wType = PM_VSCROLL;
                Parent()->Message(NewMessage);
            }
            else
            {
                if (Id())
                {
                    NewMessage.wType = PEG_SIGNAL(Id(), PSF_SCROLL_CHANGE);
                    Parent()->Message(NewMessage);
                }
            }
        }
        }
        break;

    case PM_LBUTTONDOWN:
        // see if I am above or below the scroll button:
        NewMessage.iData = mScrollInfo.wCurrent;
        if (Mesg.Point.y < mpScrollButton->mReal.wTop)
        {
            mScrollInfo.wCurrent -= mScrollInfo.wVisible;
        }
        else
        {
            // check for page down:
            if (Mesg.Point.y > mpScrollButton->mReal.wBottom)
            {
                mScrollInfo.wCurrent += mScrollInfo.wVisible;
            }
        }
        CheckScrollLimits();
        NewMessage.lData = mScrollInfo.wCurrent;
        NewMessage.pSource = this;

        if (StatusIs(PSF_NONCLIENT))
        {
            NewMessage.wType = PM_VSCROLL;
            Parent()->Message(NewMessage);
        }
        else
        {
            if (Id())
            {
                NewMessage.wType = PEG_SIGNAL(Id(), PSF_SCROLL_CHANGE);
                Parent()->Message(NewMessage);
            }
        }
        SetScrollButtonPos();
        Draw();
        break;

    default:
        PegThing::Message(Mesg);
        break;
    }
    return(0);
}


/*--------------------------------------------------------------------------*/
void PegVScroll::Draw(void)
{
    if (!(StatusIs(PSF_VISIBLE)))
    {
        return;
    }
    BeginDraw();

    PegRect Rect = mReal;
    Rect.wTop += PEG_SCROLL_WIDTH;
    Rect.wBottom -= PEG_SCROLL_WIDTH;

    BitmapFill(Rect, &gbMixPatternBitmap);
    PegThing::Draw();
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegVScroll::Resize(PegRect NewSize)
{
    PegThing::Resize(NewSize);
    PegRect Rect = mReal;
    Rect.wTop = Rect.wBottom - PEG_SCROLL_WIDTH + 1;
    mpDownButton->Resize(Rect);
    SetScrollButtonPos();
}

/*--------------------------------------------------------------------------*/
void PegVScroll::CheckScrollLimits(void)
{
    if (mScrollInfo.wCurrent < mScrollInfo.wMin)
    {
        mScrollInfo.wCurrent = mScrollInfo.wMin;
    }
    if (mScrollInfo.wCurrent > mScrollInfo.wMax - mScrollInfo.wVisible + 1)
    {
        mScrollInfo.wCurrent = mScrollInfo.wMax - mScrollInfo.wVisible + 1;

        if (mScrollInfo.wCurrent < mScrollInfo.wMin)
        {
            mScrollInfo.wCurrent = mScrollInfo.wMin;
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegVScroll::SetScrollButtonPos(void)
{
    PegRect Rect;
    Rect.wLeft = mClient.wLeft;
    Rect.wRight = mClient.wRight;

    if (mScrollInfo.wVisible > mScrollInfo.wMax - mScrollInfo.wMin)
    {
        Rect.wTop = mClient.wTop;
        Rect.wBottom = mClient.wBottom;
    }
    else
    {
        LONG lVirtualHeight = mScrollInfo.wMax - mScrollInfo.wMin + 1;
        LONG dHeight = mClient.Height();

        if (lVirtualHeight)
        {
            dHeight *= mScrollInfo.wVisible;
            dHeight /= lVirtualHeight;
        }

        if (dHeight < PEG_SCROLL_WIDTH)
        {
            dHeight = PEG_SCROLL_WIDTH;
        }

        LONG lSpare = mClient.Height() - dHeight;
        lSpare *= (mScrollInfo.wCurrent - mScrollInfo.wMin);
        lVirtualHeight -= mScrollInfo.wVisible;
        lSpare /= lVirtualHeight;
        Rect.wTop = mClient.wTop + (SIGNED) lSpare;
        Rect.wBottom = Rect.wTop + (SIGNED) dHeight;
    }

    mpScrollButton->Resize(Rect);
    Invalidate();
}

/*--------------------------------------------------------------------------*/
void PegVScroll::Reset(void)
{
    mScrollInfo.wMin = 0;
    mScrollInfo.wStep = 1;
    mScrollInfo.wMax = 0;
    mScrollInfo.wVisible = 0;
    mScrollInfo.wCurrent = 0;
    
    PegWindow *pParent = (PegWindow *) Parent();
    pParent->GetVScrollInfo(&mScrollInfo);
    SetScrollButtonPos();
}

/*--------------------------------------------------------------------------*/
void PegVScroll::Reset(PegScrollInfo *si)
{
    mScrollInfo = *si;
    SetScrollButtonPos();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Horizontal Scroll bar definition
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegHScroll::PegHScroll() : PegThing()
{
    Type(TYPE_HSCROLL);
    AddStatus(PSF_SIZEABLE|PSF_NONCLIENT);
    RemoveStatus(PSF_ACCEPTS_FOCUS);

    PegRect Rect;
    Rect.Set(0, 0, PEG_SCROLL_WIDTH - 1, PEG_SCROLL_WIDTH - 1);
    mpLeftButton = new PegBitmapButton(Rect, &gbLeftArrowBitmap,
        IDB_LEFTBUTTON, AF_ENABLED|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT);
    mpRightButton = new PegBitmapButton(Rect, &gbMoreBitmap,
        IDB_RIGHTBUTTON, AF_ENABLED|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT);
    mpScrollButton = new PegScrollButton(FALSE);

    mpLeftButton->AddStatus(PSF_NONCLIENT);
    mpLeftButton->RemoveStatus(PSF_ACCEPTS_FOCUS);
    mpRightButton->AddStatus(PSF_NONCLIENT);
    mpRightButton->RemoveStatus(PSF_ACCEPTS_FOCUS);

    Add(mpLeftButton);
    Add(mpRightButton);
    Add(mpScrollButton);

    mClient.wLeft += PEG_SCROLL_WIDTH;
    mClient.wRight -= PEG_SCROLL_WIDTH;

    mScrollInfo.wMin = 0;
    mScrollInfo.wStep = 1;
    mScrollInfo.wMax = 1;
    mScrollInfo.wVisible = 1;
    mScrollInfo.wCurrent = 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegHScroll::PegHScroll(const PegRect &InRect, PegScrollInfo *si, WORD wId) :
    PegThing(InRect, wId)
{
    Type(TYPE_HSCROLL);
    AddStatus(PSF_SIZEABLE);
    RemoveStatus(PSF_ACCEPTS_FOCUS);

    PegRect Rect;
    Rect.wLeft = mReal.wLeft;
    Rect.wTop = mReal.wTop;
    Rect.wRight = Rect.wLeft + PEG_SCROLL_WIDTH;
    Rect.wBottom = Rect.wTop + PEG_SCROLL_WIDTH;

    mpLeftButton = new PegBitmapButton(Rect, &gbLeftArrowBitmap,
        IDB_LEFTBUTTON, AF_ENABLED|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT);

    Rect.wRight = mReal.wRight;
    Rect.wLeft = Rect.wRight - PEG_SCROLL_WIDTH;

    mpRightButton = new PegBitmapButton(Rect, &gbMoreBitmap,
        IDB_RIGHTBUTTON, AF_ENABLED|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT);

    mpScrollButton = new PegScrollButton(FALSE);

    mpLeftButton->AddStatus(PSF_NONCLIENT);
    mpLeftButton->RemoveStatus(PSF_ACCEPTS_FOCUS);
    mpRightButton->AddStatus(PSF_NONCLIENT);
    mpRightButton->RemoveStatus(PSF_ACCEPTS_FOCUS);

    Add(mpLeftButton);
    Add(mpRightButton);
    Add(mpScrollButton);

    mClient = mReal;
    mClient.wLeft += PEG_SCROLL_WIDTH;
    mClient.wRight -= PEG_SCROLL_WIDTH;

    mScrollInfo = *si;
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegHScroll::~PegHScroll()
{
}

/*--------------------------------------------------------------------------*/
SIGNED PegHScroll::Message(const PegMessage &Mesg)
{
    PegMessage NewMessage;
    PegWindow *pWindow;

    switch(Mesg.wType)
    {
    case PM_SHOW:
    case PM_PARENTSIZED:
        PegThing::Message(Mesg);

        if (StatusIs(PSF_NONCLIENT))
        {
            pWindow = (PegWindow *) Parent();
            pWindow->GetHScrollInfo(&mScrollInfo);
            NewMessage.wType = PM_SIZE;
            NewMessage.Rect = Parent()->mClient;
            NewMessage.Rect.wTop = NewMessage.Rect.wBottom + 1;
            NewMessage.Rect.wBottom = NewMessage.Rect.wTop + PEG_SCROLL_WIDTH - 1;
            Message(NewMessage);
        }
        else
        {
            SetScrollButtonPos();
        }
        break;

    case PEG_SIGNAL(IDB_LEFTBUTTON, PSF_CLICKED):
        NewMessage.iData = mScrollInfo.wCurrent;
        mScrollInfo.wCurrent -= mScrollInfo.wStep;
        CheckScrollLimits();

        if (mScrollInfo.wCurrent != NewMessage.iData)
        {
            NewMessage.lData = mScrollInfo.wCurrent;
            NewMessage.pSource = this;
            if (StatusIs(PSF_NONCLIENT))
            {
                NewMessage.wType = PM_HSCROLL;
                Parent()->Message(NewMessage);
            }
            else
            {
                if (Id())
                {
                    NewMessage.wType = PEG_SIGNAL(Id(), PSF_SCROLL_CHANGE);
                    Parent()->Message(NewMessage);
                }
            }
            SetScrollButtonPos();
            Draw();
        }
        break;

    case PEG_SIGNAL(IDB_RIGHTBUTTON, PSF_CLICKED):
        NewMessage.iData = mScrollInfo.wCurrent;
        mScrollInfo.wCurrent += mScrollInfo.wStep;
        CheckScrollLimits();

        if (mScrollInfo.wCurrent != NewMessage.iData)
        {
            NewMessage.lData = mScrollInfo.wCurrent;
            NewMessage.pSource = this;
            if (StatusIs(PSF_NONCLIENT))
            {
                NewMessage.wType = PM_HSCROLL;
                Parent()->Message(NewMessage);
            }
            else
            {
                if (Id())
                {
                    NewMessage.wType = PEG_SIGNAL(Id(), PSF_SCROLL_CHANGE);
                    Parent()->Message(NewMessage);
                }
            }
            SetScrollButtonPos();
            Draw();
        }
        break;

    case PM_HSCROLL:
        {
        // this message comes to me from the scroll button. The dData
        // member contains the scroll amount, in pixels. I need to convert
        // this to the parent window pixel amount and pass the message on
        // up to the parent

        LONG lRange = (mScrollInfo.wMax - mScrollInfo.wMin) - mScrollInfo.wVisible + 1;
        lRange *= Mesg.lData - mClient.wLeft;
        if (mClient.Width() != Mesg.iData)
        {
            lRange /= (mClient.Width() - Mesg.iData);
        }
        lRange += mScrollInfo.wMin;

        if ((SIGNED) lRange != mScrollInfo.wCurrent)
        {
            NewMessage.iData = mScrollInfo.wCurrent;
            mScrollInfo.wCurrent = (SIGNED) lRange;
            CheckScrollLimits();
            NewMessage.lData = mScrollInfo.wCurrent;
            NewMessage.pSource = this;

            if (StatusIs(PSF_NONCLIENT))
            {
                NewMessage.wType = PM_HSCROLL;
                Parent()->Message(NewMessage);
            }
            else
            {
                if (Id())
                {
                    NewMessage.wType = PEG_SIGNAL(Id(), PSF_SCROLL_CHANGE);
                    Parent()->Message(NewMessage);
                }
            }
        }
        }
        break;


    case PM_LBUTTONDOWN:
        // see if I am left or right of the scroll button:
        NewMessage.iData = mScrollInfo.wCurrent;
        if (Mesg.Point.x < mpScrollButton->mReal.wLeft)
        {
            mScrollInfo.wCurrent -= mScrollInfo.wVisible;
        }
        else
        {
            // check for page down:
            if (Mesg.Point.x > mpScrollButton->mReal.wRight)
            {
                mScrollInfo.wCurrent += mScrollInfo.wVisible;
            }
        }
        CheckScrollLimits();
        NewMessage.lData = mScrollInfo.wCurrent;
        NewMessage.pSource = this;

        if (StatusIs(PSF_NONCLIENT))
        {
            NewMessage.wType = PM_HSCROLL;
            Parent()->Message(NewMessage);
        }
        else
        {
            if (Id())
            {
                NewMessage.wType = PEG_SIGNAL(Id(), PSF_SCROLL_CHANGE);
                Parent()->Message(NewMessage);
            }
        }
            
        SetScrollButtonPos();
        Draw();
        break;

    default:
        PegThing::Message(Mesg);
    }
    return(0);
}


/*--------------------------------------------------------------------------*/
void PegHScroll::Draw(void)
{
    if (!(StatusIs(PSF_VISIBLE)))
    {
        return;
    }
    BeginDraw();

    PegRect Rect = mReal;
    Rect.wLeft += PEG_SCROLL_WIDTH;
    Rect.wRight -= PEG_SCROLL_WIDTH;

    BitmapFill(Rect, &gbMixPatternBitmap);

    PegThing::Draw();
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegHScroll::Resize(PegRect NewSize)
{
    PegThing::Resize(NewSize);
    PegRect Rect = mReal;
    Rect.wLeft = Rect.wRight - PEG_SCROLL_WIDTH + 1;
    mpRightButton->Resize(Rect);
    SetScrollButtonPos();
}

/*--------------------------------------------------------------------------*/
void PegHScroll::CheckScrollLimits(void)
{
    if (mScrollInfo.wCurrent < mScrollInfo.wMin)
    {
        mScrollInfo.wCurrent = mScrollInfo.wMin;
    }
    if (mScrollInfo.wCurrent > mScrollInfo.wMax - mScrollInfo.wVisible + 1)
    {
        mScrollInfo.wCurrent = mScrollInfo.wMax - mScrollInfo.wVisible + 1;
    }
}

/*--------------------------------------------------------------------------*/
void PegHScroll::SetScrollButtonPos(void)
{

    PegRect Rect;
    Rect.wTop = mClient.wTop;
    Rect.wBottom = mClient.wBottom;

    if (mScrollInfo.wVisible > mScrollInfo.wMax - mScrollInfo.wMin)
    {
        Rect.wLeft = mClient.wLeft;
        Rect.wRight = mClient.wRight;
    }
    else
    {
        LONG lVirtualWidth = mScrollInfo.wMax - mScrollInfo.wMin + 1;
        LONG dWidth = mClient.Width();

        if (lVirtualWidth)
        {
            dWidth *= mScrollInfo.wVisible;
            dWidth /= lVirtualWidth;
        }
        if (dWidth < PEG_SCROLL_WIDTH)
        {
            dWidth = PEG_SCROLL_WIDTH;
        }

        LONG lSpare = mClient.Width() - dWidth;
        lSpare *= (mScrollInfo.wCurrent - mScrollInfo.wMin);
        lVirtualWidth -= mScrollInfo.wVisible;
        lSpare /= lVirtualWidth;
        Rect.wLeft = mClient.wLeft + (SIGNED) lSpare;
        Rect.wRight = Rect.wLeft + (SIGNED) dWidth;
    }

    mpScrollButton->Resize(Rect);
    Invalidate(mClient);
}

/*--------------------------------------------------------------------------*/
void PegHScroll::Reset(void)
{
    mScrollInfo.wMin = 0;
    mScrollInfo.wStep = 1;
    mScrollInfo.wMax = 0;
    mScrollInfo.wVisible = 0;
    mScrollInfo.wCurrent = 0;
    
    PegWindow *pParent = (PegWindow *) Parent();
    pParent->GetHScrollInfo(&mScrollInfo);
    SetScrollButtonPos();
}

/*--------------------------------------------------------------------------*/
void PegHScroll::Reset(PegScrollInfo *si)
{
    mScrollInfo = *si;
    SetScrollButtonPos();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//
//  PegScrollButton Class implementation
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegScrollButton::PegScrollButton(BOOL bAmVertical) : PegThing(0, FF_RAISED)
{
    mbAmVertical = bAmVertical;
    mbMoveMode = FALSE;
    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    muColors[PCI_SELECTED] = PCLR_BUTTON_FACE;
    RemoveStatus(PSF_ACCEPTS_FOCUS);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegScrollButton::~PegScrollButton()
{
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegScrollButton::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_LBUTTONDOWN:
        if (StatusIs(PSF_VISIBLE) && Parent()->StatusIs(PSF_VISIBLE))
        {
            Presentation()->CapturePointer(this);
            mbMoveMode = TRUE;
            mwPosRect = mReal;

            if (mbAmVertical)
            {
                mwMoveStart = mwLastMove = Mesg.Point.y;
            }
            else
            {
                mwMoveStart = mwLastMove = Mesg.Point.x;
            }
        }
        break;

    case PM_LBUTTONUP:
        if (mbMoveMode)
        {
            mbMoveMode = FALSE;
            ReleasePointer();
            PegMessage NewMessage;

            if (mbAmVertical)
            {
                NewMessage.wType = PM_VSCROLL;
                NewMessage.lData = mReal.wTop;
                NewMessage.iData = mReal.Height();
            }
            else
            {
                NewMessage.wType = PM_HSCROLL;
                NewMessage.lData = mReal.wLeft;
                NewMessage.iData = mReal.Width();
            }
            Parent()->Message(NewMessage);
        }
        break;

    case PM_POINTER_MOVE:
        if (mbMoveMode)
        {
            if (mbAmVertical)
            {
                if (Mesg.Point.y != mwLastMove)
                {
                    mwLastMove = Mesg.Point.y;
                    if (MoveDraw(Mesg.Point.y))
                    {
                        if (Parent()->Parent()->StatusIs(PSF_CONTINUOUS_SCROLL))
                        {
                            PegMessage NewMessage(NULL, PM_VSCROLL);
                            NewMessage.lData = mReal.wTop;
                            NewMessage.iData = mReal.Height();
                            Parent()->Message(NewMessage);
                        }
                    }
                }
            }
            else
            {
                if (Mesg.Point.x != mwLastMove)
                {
                    mwLastMove = Mesg.Point.x;
                    if (MoveDraw(Mesg.Point.x))
                    {
                        if (Parent()->Parent()->StatusIs(PSF_CONTINUOUS_SCROLL))
                        {
                            PegMessage NewMessage(NULL, PM_HSCROLL);
                            NewMessage.lData = mReal.wLeft;
                            NewMessage.iData = mReal.Width();                    
                            Parent()->Message(NewMessage);
                       }
                    }
                }
            }
        }
        break;

    default:
        PegThing::Message(Mesg);
        break;
    }

    return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegScrollButton::Draw(void)
{
    BeginDraw();
    StandardBorder(muColors[PCI_NORMAL]);
    EndDraw();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegScrollButton::MoveDraw(SIGNED  iNewVal)
{
    PegRect Rect = mwPosRect;
    SIGNED iShift = iNewVal - mwMoveStart;

    if (mbAmVertical)
    {
        Rect.Shift(0, iShift);

        if (Rect.wBottom > Parent()->mClient.wBottom)
        {
            Rect.wBottom = Parent()->mClient.wBottom;
            Rect.wTop = Rect.wBottom - mReal.Height() + 1;
        }

        if (Rect.wTop < Parent()->mClient.wTop)
        {
            Rect.wTop = Parent()->mClient.wTop;
            Rect.wBottom = Rect.wTop + mReal.Height() - 1;
        }

        if (Rect != mReal)
        {
            PegRect FillRect = Parent()->mClient;

            if (Rect.wTop < mReal.wTop)
            {
                // moving up, fill the bottom
                FillRect.wTop = Rect.wBottom + 1;
            }
            else
            {
                // moving down, fill the top
                FillRect.wBottom = Rect.wTop - 1;
            }

            Resize(Rect);
            
            PegRect OldClip = mClip;
            mClip = Parent()->mClip & FillRect;
            BeginDraw();
            BitmapFill(Parent()->mClient, &gbMixPatternBitmap);
            mClip = OldClip;
            Draw();
            EndDraw();
            return TRUE;
        }
    }
    else
    {
        Rect.Shift(iShift, 0);
        if (Rect.wRight > Parent()->mClient.wRight)
        {
            Rect.wRight = Parent()->mClient.wRight;
            Rect.wLeft = Rect.wRight - mReal.Width() + 1;
        }

        if (Rect.wLeft < Parent()->mClient.wLeft)
        {
            Rect.wLeft = Parent()->mClient.wLeft;
            Rect.wRight = Rect.wLeft + mReal.Width() - 1;
        }

        if (Rect != mReal)
        {
            PegRect FillRect = Parent()->mClient;

            if (Rect.wLeft > mReal.wLeft)
            {
                // moving right, fill the left:
                FillRect.wRight = Rect.wLeft - 1;
            }
            else
            {
                // moving left, fill the right:
                FillRect.wLeft = Rect.wRight + 1;
            }

            Resize(Rect);
            PegRect OldClip = mClip;
            mClip = Parent()->mClip & FillRect;
            BeginDraw();
            BitmapFill(Parent()->mClient, &gbMixPatternBitmap);
            mClip = OldClip;
            Draw();
            EndDraw();
            return TRUE;
        }
    }
    return FALSE;
}



// End of file
