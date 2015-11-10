/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pgroup.cpp - Implementation of PegGroup class.
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
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

extern PEGCHAR lsTEST[];

/*--------------------------------------------------------------------------*/
// PegGroup base class definition:
/*--------------------------------------------------------------------------*/
PegGroup::PegGroup(PegRect &Rect, const PEGCHAR *Text, WORD wFlags) :
    PegThing(Rect, 0, wFlags),
    PegTextThing(Text, wFlags & TT_COPY, PEG_GROUP_FONT)
{
    Type(TYPE_GROUP);
    AddStatus(PSF_TAB_STOP);
    mClient = mReal;
    mClient -= 2;
    mClient.wTop += TextHeight(lsTEST, mpFont) / 2;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
}


/*--------------------------------------------------------------------------*/
SIGNED PegGroup::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case PM_SHOW:
        PegThing::Message(Mesg);
        muColors[PCI_NORMAL] = Parent()->muColors[PCI_NORMAL];
       
       #if defined(PEG_KEYBOARD_SUPPORT) && defined(PEG_TAB_KEY_SUPPORT)
        SetDefaultTabLinks();
       #endif

        break;

  #if defined(PEG_KEYBOARD_SUPPORT)

    case PM_GAINED_KEYBOARD:
        MoveFocusToFirstClientChild();
        break;

   #if defined(PEG_TAB_KEY_SUPPORT)
    // if tabbing is supported, the group gets an arrow key input, it means that
    // the user has reached the limit of navigation within the group. Don't allow
    // moving outside the group if TAB key is supported:
     case PM_KEY:
        if (Mesg.iData == PK_LEFT ||
            Mesg.iData == PK_RIGHT ||
            Mesg.iData == PK_LNUP ||
            Mesg.iData == PK_LNDN)
        {
            break;
        }
        return PegThing::Message(Mesg);
    #endif

  #else

    case PM_CURRENT:
        if (Presentation()->GetCurrentThing() == this)
        {
            MoveFocusToFirstClientChild();
        }
        PegThing::Message(Mesg);
        break;
  #endif

    default:
        PegThing::Message(Mesg);
        break;
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
void PegGroup::Draw(void)
{
    #ifdef PEG_UNICODE
    PEGCHAR cTest[2] = {'E', '\0'};
    #else
    PEGCHAR cTest[] = "E";
    #endif

    if (Parent())
    {
        muColors[PCI_NORMAL] = Parent()->muColors[PCI_NORMAL];
    }
    BeginDraw();

    PegColor Color(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    Rectangle(mClient, Color, 0);

    Color.Set(PCLR_LOWLIGHT, PCLR_LOWLIGHT, CF_NONE);
    
    Color.uForeground = PCLR_LOWLIGHT;
    SIGNED iTextLeft = mReal.wLeft + TextWidth(cTest, mpFont);
    SIGNED iTextRight = iTextLeft + TextWidth(mpText, mpFont) + 4;

    if(mpText)
    {
        Line(mReal.wLeft, mClient.wTop - 2, iTextLeft,
            mClient.wTop - 2, Color);
        Line(iTextRight, mClient.wTop - 2, mReal.wRight,
            mClient.wTop - 2, Color);
    }
    else
    {
        Line(mReal.wLeft, mClient.wTop - 2, mReal.wRight,
             mClient.wTop - 2, Color);
    }
    Line(mReal.wLeft, mClient.wTop - 1, mReal.wLeft,
        mReal.wBottom, Color);
    Line(mReal.wLeft + 1, mReal.wBottom - 1,
        mReal.wRight - 1, mReal.wBottom - 1, Color);
    Line(mReal.wRight - 1, mClient.wTop - 2,
        mReal.wRight - 1, mReal.wBottom - 1, Color);

    Color.uForeground = PCLR_HIGHLIGHT;
    if(mpText)
    {
        Line(mReal.wLeft + 1, mClient.wTop - 1, iTextLeft,
            mClient.wTop - 1, Color);
        Line(iTextRight, mClient.wTop - 1, mReal.wRight - 2,
            mClient.wTop - 1, Color);
    }
    else
    {
        Line(mReal.wLeft + 1, mClient.wTop - 1, mReal.wRight - 2,
             mClient.wTop - 1, Color);
    }
    Line(mReal.wLeft + 1, mClient.wTop, mReal.wLeft + 1,
        mReal.wBottom - 1, Color);
    Line(mReal.wLeft, mReal.wBottom, mReal.wRight,
        mReal.wBottom, Color);
    Line(mReal.wRight, mClient.wTop - 2, mReal.wRight,
        mReal.wBottom, Color);

    if(mpText)
    {
        if (mwStyle & AF_ENABLED)
        {
            Color.uForeground = muColors[PCI_NTEXT];
        }
        else
        {
            Color.uForeground = PCLR_LOWLIGHT;
        }
        PegPoint Put;
        Put.x = iTextLeft + 2;
        Put.y = mReal.wTop;

        DrawText(Put, mpText, Color, mpFont);
    }

    PegThing::Draw();       // to draw children
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegGroup::Add(PegThing *Who, BOOL bDraw)
{
    Who->RemoveStatus(PSF_VIEWPORT);
    PegThing::Add(Who, bDraw);
}


/*--------------------------------------------------------------------------*/
void PegGroup::Enable(BOOL bEnable)
{
    if (bEnable)
    {
        if (!(mwStyle & AF_ENABLED))
        {
            mwStyle |= AF_ENABLED;
            AddStatus(PSF_TAB_STOP|PSF_SELECTABLE);
            PegThing *pChild = First();

            while(pChild)
            {
                if (pChild->Type() == TYPE_BUTTON ||
                    pChild->Type() == TYPE_TEXTBUTTON ||
                    pChild->Type() == TYPE_TEXTBUTTON ||
                    pChild->Type() == TYPE_BMBUTTON ||
                    pChild->Type() == TYPE_RADIOBUTTON ||
                    pChild->Type() == TYPE_MLTEXTBUTTON ||
                    pChild->Type() == TYPE_CHECKBOX ||
                    pChild->Type() == TYPE_DECORATEDBUTTON)
                {
                    PegButton *pButton = (PegButton *) pChild;
                    pButton->Enable();
                    pButton->AddStatus(PSF_SELECTABLE|PSF_TAB_STOP);
                }
                pChild = pChild->Next();
            }
            if (StatusIs(PSF_VISIBLE))
            {
                Invalidate(mReal);
                Draw();
            }
        }
    }
    else
    {
        if (mwStyle & AF_ENABLED)
        {
            mwStyle &= ~AF_ENABLED;            
            RemoveStatus(PSF_TAB_STOP|PSF_SELECTABLE);
            PegThing *pChild = First();

            while(pChild)
            {
                if (pChild->Type() == TYPE_BUTTON ||
                    pChild->Type() == TYPE_TEXTBUTTON ||
                    pChild->Type() == TYPE_TEXTBUTTON ||
                    pChild->Type() == TYPE_BMBUTTON ||
                    pChild->Type() == TYPE_RADIOBUTTON ||
                    pChild->Type() == TYPE_MLTEXTBUTTON ||
                    pChild->Type() == TYPE_CHECKBOX ||
                    pChild->Type() == TYPE_DECORATEDBUTTON)
                {
                    PegButton *pButton = (PegButton *) pChild;
                    pButton->Disable();
                    pButton->RemoveStatus(PSF_SELECTABLE|PSF_TAB_STOP);
                }
                pChild = pChild->Next();
            }
            if (StatusIs(PSF_VISIBLE))
            {
                Invalidate(mReal);
                Draw();
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
PegThing *PegGroup::Remove(PegThing *Who, BOOL bDraw)
{
    PegThing *pRemoved = PegThing::Remove(Who, FALSE);

    if (pRemoved && StatusIs(PSF_VISIBLE) && bDraw)
    {
        Parent()->Draw();
    }
    return (pRemoved);
}

// End of file
