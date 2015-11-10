/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pcombo.cpp - PegComboBox implementation. 
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

extern PegBitmap gbDownArrowBitmap;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// PegComboBox class implementation.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegComboBox::PegComboBox(const PegRect &Rect, WORD wId, WORD wStyle) : 
    PegVertList(Rect, wId, wStyle)
{
    Type(TYPE_COMBO);
    mbOpen = FALSE;
    miOpenHeight = mReal.Height();

    PegRect Pos;
    Pos.Set(0, 0, 0, 0);
    mpOpenButton = new PegBitmapButton(Pos, &gbDownArrowBitmap,
        IDB_OPEN, AF_ENABLED|BF_FULLBORDER);

    mpOpenButton->AddStatus(PSF_NONCLIENT);
    mpOpenButton->RemoveStatus(PSF_ACCEPTS_FOCUS);
    Add(mpOpenButton);
    mpPrompt = new PegPrompt(Pos, NULL, 0, TJ_LEFT|TT_COPY|FF_NONE);
    mpPrompt->AddStatus(PSF_NONCLIENT);
    Add(mpPrompt);
}


/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegComboBox::~PegComboBox()
{
}

/*--------------------------------------------------------------------------*/
SIGNED PegComboBox::Message(const PegMessage &Mesg)
{
    PegPrompt *SelThing;
    PegRect NewSize;

    switch (Mesg.wType)
    {
    case PM_LBUTTONDOWN:
        if (mbOpen)
        {
            Message(PEG_SIGNAL(IDB_OPEN, PSF_CLICKED));
            if (mpLastSelected)
            {
                if (!mpLastSelected->StatusIs(PSF_CURRENT))
                {
                    mpLastSelected->AddStatus(PSF_CURRENT);
                }
            }
        }
        break;

    case PM_SHOW:
        CalcCloseHeight();
        NewSize = mReal;
        NewSize.wBottom = NewSize.wTop + miCloseHeight - 1;
        Resize(NewSize);
        mbOpen = FALSE;

        SelThing = (PegPrompt *) GetSelected();

        if (!SelThing)
        {
            SelThing = (PegPrompt *) SetSelected((SIGNED) 0);
        }

        if (SelThing)
        {
            if (SelThing->Type() == TYPE_PROMPT)
            {
                mpPrompt->DataSet(SelThing->DataGet());
                mpPrompt->SetFont(SelThing->GetFont());
            }
        }
        PegVertList::Message(Mesg);
        break;

    case PEG_SIGNAL(IDB_OPEN, PSF_CLICKED):
        if (mbOpen)
        {
            mbOpen = FALSE;
            miOpenHeight = mReal.Height();
            NewSize = mReal;
            NewSize.wBottom = NewSize.wTop + miCloseHeight - 1;
            Resize(NewSize);
            Parent()->Draw();
        }
        else
        {
            mbOpen = TRUE;
            
            //Parent()->MoveToFront(this);    // get on top
            Parent()->Add(this);            // get on top
            NewSize = mReal;
            NewSize.wBottom = NewSize.wTop + miOpenHeight - 1;
            mClient.wBottom = mReal.wBottom - 1;
            if (Style() & (FF_RAISED | FF_RECESSED))
            {
                mClient.wBottom--;
            }
            Resize(NewSize);
            //CheckAutoScroll();
            
            ScrollCurrentIntoView(FALSE);
            
            Draw();
        }
        break;

    case PM_NONCURRENT:
        PegVertList::Message(Mesg);
        if (mbOpen)
        {
            SelThing = (PegPrompt *) GetSelected();
            if (SelThing)
            {
                mpPrompt->DataSet(SelThing->DataGet());
                mpPrompt->SetFont(SelThing->GetFont());
            }
            mbOpen = FALSE;
            NewSize = mReal;
            miOpenHeight = mReal.Height();
            NewSize.wBottom = NewSize.wTop + miCloseHeight - 1;
            Resize(NewSize);
            Parent()->Draw();
        }
        else
        {
            Invalidate(mReal);
            Draw();
        }
        break;

    case PM_CURRENT:
        PegVertList::Message(Mesg);
        Invalidate(mReal);
        Draw();
        break;

#ifdef PEG_KEYBOARD_SUPPORT
    case PM_KEY:

        switch(Mesg.iData)
        {
        case PK_CR:     // carriage return?

        #ifdef PEG_ARROW_KEY_SUPPORT
        case PK_LNDN:
        case PK_LNUP:
        #endif

            if (!mbOpen)
            {
               #ifdef PEG_ARROW_KEY_SUPPORT
                if (Mesg.iData == PK_LNDN ||
                    Mesg.iData == PK_LNUP ||
                    (Mesg.lData & (KF_SHIFT | KF_ALT)))
               #else
                if (Mesg.lData & (KF_SHIFT | KF_ALT))
               #endif
                {
                    PegMessage NewMessage;
                    NewMessage.wType = PEG_SIGNAL(IDB_OPEN, PSF_CLICKED);
                    NewMessage.pTarget = NULL;
                    Message(NewMessage);
                }
                else
                {
                    return PegVertList::Message(Mesg);
                }
            }
            else
            {
               #ifdef PEG_ARROW_KEY_SUPPORT
                if (Mesg.iData == PK_CR)
                {
               #endif
                    SelThing = (PegPrompt *) GetSelected();
                    if (SelThing)
                    {
                        mpPrompt->DataSet(SelThing->DataGet());
                        mpPrompt->SetFont(SelThing->GetFont());
                    }

                    mbOpen = FALSE;
                    NewSize = mReal;
                    NewSize.wBottom = NewSize.wTop + miCloseHeight - 1;
                    Resize(NewSize);
                    Parent()->Draw();
               #ifdef PEG_ARROW_KEY_SUPPORT
                }
                else
               #endif
                {
                    return PegVertList::Message(Mesg);
                }
            }
            break;

        default:
            return PegVertList::Message(Mesg);
        }
        break;
#endif

    default:
        if (PegList::Message(Mesg) == PSF_FOCUS_RECEIVED)
        {
            if (!mbOpen)
            {
                SelThing = (PegPrompt *) GetSelected();
                if (SelThing)
                {
                    mpPrompt->DataSet(SelThing->DataGet());
                    mpPrompt->SetFont(SelThing->GetFont());
                    mpPrompt->Draw();
                }
            }
            else
            {
                ScrollCurrentIntoView(TRUE);
            }
        }
        break;
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void PegComboBox::Draw(void)
{
    PegThing *SelThing = (PegPrompt *) GetSelected();

    BeginDraw();
    PegWindow::DrawFrame();

    if (mbOpen)
    {
        if (SelThing)
        {
            mpPrompt->SetColor(PCI_NORMAL, SelThing->muColors[PCI_NORMAL]);
            mpPrompt->SetColor(PCI_NTEXT, SelThing->muColors[PCI_NTEXT]);
        }

        PegColor Color(BLACK, BLACK, CF_NONE);
        SIGNED iRight = mClient.wRight;

        if (mpVScroll)
        {
            iRight += PEG_SCROLL_WIDTH;
        }
        Line(mClient.wLeft, mClient.wTop - 1, iRight,
            mClient.wTop - 1, Color);

        if (Style() & (FF_RECESSED|FF_RAISED))
        {
            Color.uForeground = PCLR_BORDER;
            Line(mClient.wLeft, mClient.wTop - 2, iRight,
                mClient.wTop - 2, Color);
        }
        DrawChildren();
    }
    else
    {
        mpOpenButton->Draw();
        if (SelThing)
        {
            if (StatusIs(PSF_CURRENT))
            {
                mpPrompt->SetColor(PCI_NORMAL, SelThing->muColors[PCI_SELECTED]);
                mpPrompt->SetColor(PCI_NTEXT, SelThing->muColors[PCI_STEXT]);
            }
            else
            {
                mpPrompt->SetColor(PCI_NORMAL, SelThing->muColors[PCI_NORMAL]);
                mpPrompt->SetColor(PCI_NTEXT, SelThing->muColors[PCI_NTEXT]);
            }
        }
        mpPrompt->Draw();
    }
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegComboBox::Resize(PegRect NewSize)
{
    if (NewSize.wLeft != mReal.wLeft ||
        NewSize.wRight != mReal.wRight)
    {
        PegVertList::Resize(NewSize);
        PegRect Pos = mpOpenButton->mReal;
        Pos.wRight = mClient.wRight;
        if (mpVScroll)
        {
            Pos.wRight += PEG_SCROLL_WIDTH;
        }
        Pos.wLeft = Pos.wRight - PEG_SCROLL_WIDTH + 1;
        mpOpenButton->Resize(Pos);
        Pos.wLeft = mClient.wLeft;
        Pos.wRight = mpOpenButton->mReal.wLeft - 1;
        mpPrompt->Resize(Pos);
    }
    else
    {
        PegVertList::Resize(NewSize);
    }
}

/*--------------------------------------------------------------------------*/
void PegComboBox::CalcCloseHeight(void)
{
    miCloseHeight = PEG_SCROLL_WIDTH;

    PegThing *pTest = First();

    while(pTest)
    {
        if (!pTest->StatusIs(PSF_NONCLIENT))
        {
            if (pTest->mReal.Height() > miCloseHeight)
            {
                miCloseHeight = pTest->mReal.Height();
            }
        }
        pTest = pTest->Next();
    }

    if (Style() & (FF_RAISED|FF_RECESSED))
    {
        miCloseHeight += 4;
    }
    else
    {
        if (Style() & FF_THIN)
        {
            miCloseHeight += 2;
        }
    }

    if (!mbOpen)
    {
        mReal.wBottom = mReal.wTop + miCloseHeight - 1;
        mClient.wTop = mReal.wBottom + 1;
        PegRect Pos = mReal;

        if (Style() & (FF_RAISED|FF_RECESSED))
        {
            Pos -= 2;
        }
        else
        {
            if (Style() & (FF_THIN))
            {
                Pos--;
            }
        }
        Pos.wLeft = Pos.wRight - PEG_SCROLL_WIDTH + 1;
        mpOpenButton->Resize(Pos);
        Pos.wLeft = mClient.wLeft;
        Pos.wRight = mpOpenButton->mReal.wLeft - 1;
        mpPrompt->Resize(Pos);
    }
}


// End of file
