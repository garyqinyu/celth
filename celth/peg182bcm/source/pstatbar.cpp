/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pstatbar.cpp - 
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

/*--------------------------------------------------------------------------*/
PegStatusBar::PegStatusBar() : PegThing()
{
    mpFont = PegTextThing::GetDefaultFont(PEG_PROMPT_FONT);

    mReal.Set(0, 0, 100, mpFont->uHeight + 6);
    mClient.Set(0, 0, 100, mpFont->uHeight + 6);
    Type(TYPE_STATUS_BAR);
    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    AddStatus(PSF_SIZEABLE|PSF_NONCLIENT);
    mpLastPrompt = NULL;
    mbShowPointer = FALSE;
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegStatusBar::~PegStatusBar()
{
}


/*--------------------------------------------------------------------------*/
// 
/*--------------------------------------------------------------------------*/
SIGNED PegStatusBar::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
    case PM_PARENTSIZED:
        // make myself the right size:
        SizeToParent();
        PegThing::Message(Mesg);
        break;

    case PM_POINTER_MOVE:
        if (Parent()->StatusIs(PSF_SIZEABLE))
        {
            PegRect DragRect = mReal;
            DragRect.wTop = DragRect.wBottom - PEG_SCROLL_WIDTH;
            DragRect.wLeft = DragRect.wRight - PEG_SCROLL_WIDTH;

            if (DragRect.Contains(Mesg.Point))
            {
                if (!mbShowPointer)
                {
                    mbShowPointer = TRUE;
                    SetPointerType(PPT_NWSE_SIZE);
                }
            }
            else
            {
                if (mbShowPointer)
                {
                    mbShowPointer = FALSE;
                    SetPointerType(PPT_NORMAL);
                }
            }
        }
        break;

    case PM_POINTER_EXIT:
        if (mbShowPointer)
        {
            SetPointerType(PPT_NORMAL);
            mbShowPointer = FALSE;
        }
        break;

    case PM_LBUTTONDOWN:
        if (mbShowPointer)
        {
            mbShowPointer = FALSE;
            PegMessage SizeMesg(Parent(), PM_BEGIN_MOVE);
            SizeMesg.iData = PMM_MOVELR;
            SizeMesg.Point = Mesg.Point;
            Parent()->Message(SizeMesg);
        }
        else
        {
            return (Parent()->Message(Mesg));
        }
        break;

    default:
        PegThing::Message(Mesg);
        break;
    }
    return(0);
}

extern PegBitmap gbDragBitmap;

/*--------------------------------------------------------------------------*/
void PegStatusBar::Draw(void)
{
    if (!Parent())
    {
        return;
    }
    BeginDraw();
    PegColor Color(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    Rectangle(mReal, Color, 0);
    Color.uForeground = PCLR_HIGHLIGHT;
    Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wRight, mReal.wTop + 1, Color);
    if (Parent()->Style() & FF_THICK)
    {
        Screen()->PutPixel(this, mReal.wRight, mReal.wTop, WHITE);
    }
    PegThing::Draw();
    CheckDrawDragMap();
    EndDraw();
}


/*--------------------------------------------------------------------------*/
PegPrompt *PegStatusBar::AddTextField(SIGNED iWidth, WORD wId, const PEGCHAR *Text)
{
    PegRect Rect;
    Rect.wLeft = mReal.wLeft;
    Rect.wTop = mReal.wTop + 2;
    Rect.wBottom = mReal.wBottom - 1;
    PegThing *pField = First();

    while(pField)
    {
        if (pField->mReal.wRight > Rect.wLeft)
        {
            Rect.wLeft = pField->mReal.wRight + 3;
        }
        pField = pField->Next();        
    }

    Rect.wRight = Rect.wLeft + iWidth - 1;

    PegPrompt *pPrompt = new PegPrompt(Rect, Text, wId, FF_RECESSED|TJ_LEFT|AF_TRANSPARENT|TT_COPY);
    pPrompt->SetFont(mpFont);
    AddToEnd(pPrompt);
    mpLastPrompt = pPrompt;
    return pPrompt;
}

/*--------------------------------------------------------------------------*/
void PegStatusBar::SetFont(PegFont *pFont)
{
    if (!pFont)
    {
        return;
    }
    mpFont = pFont;
    PegRect NewSize = mReal;
    SIGNED iOldTop = mReal.wTop;
    NewSize.wTop = NewSize.wBottom - (mpFont->uHeight + 6);
    PegMessage NewMessage(PM_SIZE);
    NewMessage.Rect = NewSize;
    Message(NewMessage);

    // resize all my child prompts:
    NewSize.wTop += 2;
    NewSize.wBottom--;

    PegThing *pTest = First();
    while(pTest)
    {
        if (pTest->Type() == TYPE_PROMPT)
        {
            PegPrompt *pp = (PegPrompt *) pTest;
            NewSize.wLeft = pp->mReal.wLeft;
            NewSize.wRight = pp->mReal.wRight;
            pp->Resize(NewSize);
        }
        pTest = pTest->Next();
    }

    if (StatusIs(PSF_VISIBLE))
    {
        Parent()->mClient.wBottom += mReal.wTop - iOldTop;
        Parent()->Resize(Parent()->mReal);
        Parent()->Draw();
    }
}

/*--------------------------------------------------------------------------*/
void PegStatusBar::SetTextField(WORD wId, PEGCHAR *Text)
{
    PegPrompt *pPrompt;
    PegThing *pTest = First();

    while(pTest)
    {
        if (pTest->Type() == TYPE_PROMPT && pTest->Id() == wId)
        {
            BeginDraw();
            pPrompt = (PegPrompt *) pTest;
            pPrompt->DataSet(Text);
            pPrompt->Draw();
            CheckDrawDragMap(pPrompt);
            EndDraw();
            return;
        }
        pTest = pTest->Next();
    }
}


/*--------------------------------------------------------------------------*/
PegPrompt *PegStatusBar::GetPrompt(WORD wId)
{
    PegPrompt *pPrompt;
    PegThing *pTest = First();

    while(pTest)
    {
        if (pTest->Type() == TYPE_PROMPT && pTest->Id() == wId)
        {
            pPrompt = (PegPrompt *) pTest;
            return pPrompt;
        }
        pTest = pTest->Next();
    }
    return (NULL);
}

/*--------------------------------------------------------------------------*/
void PegStatusBar::SizeToParent(void)
{
    if (!Parent())
    {
        return;
    }

    PegWindow *pWin = (PegWindow *) Parent();

    PegRect NewSize = Parent()->mReal;

    if (pWin->Style() & FF_THICK)
    {
        NewSize.wBottom -= PEG_FRAME_WIDTH - 2;
        NewSize.wLeft += PEG_FRAME_WIDTH - 2;
        NewSize.wRight -= PEG_FRAME_WIDTH - 2;
    }
    else
    {
        if (!(pWin->Style() & FF_NONE))
        {
            NewSize.wBottom -= 2;
            NewSize.wLeft += 2;
            NewSize.wRight -= 2;
        }
    }

    NewSize.wTop = NewSize.wBottom - (mpFont->uHeight + 6);
    PegMessage NewMessage(PM_SIZE);
    NewMessage.Rect = NewSize;
    Message(NewMessage);

    if (Parent()->StatusIs(PSF_SIZEABLE))
    {
        mClient.wRight = mReal.wRight - PEG_SCROLL_WIDTH;
    }

    // Resize the last prompt so that it fills to my right,

    if (mpLastPrompt)
    {
        NewSize = mpLastPrompt->mReal;
        NewSize.wRight = mReal.wRight - 1;
        if (Parent())
        {
            if (Parent()->StatusIs(PSF_SIZEABLE))
            {
                NewSize.wRight -= PEG_SCROLL_WIDTH;
            }
        }
        mpLastPrompt->Resize(NewSize);
    }
}

/*--------------------------------------------------------------------------*/
void PegStatusBar::CheckDrawDragMap(PegThing *Check)
{
    if (Check)
    {
        if (Check->mReal.wRight < mReal.wRight - (SIGNED) gbDragBitmap.wWidth)
        {
            return;
        }
    }

    if (Parent())
    {
        if (Parent()->StatusIs(PSF_SIZEABLE))
        {
            // draw the little 'corner drag' bitmap:
            PegPoint Point;
            Point.x = mReal.wRight - gbDragBitmap.wWidth + 1;
            Point.y = mReal.wBottom - gbDragBitmap.wHeight;
            Bitmap(Point, &gbDragBitmap);
        }
    }
}



// End of file
