/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pprompt.cpp - simple prompt object.
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
// This is the basic window class. It supports the addition of decorations
// such as a border, title, scroll bar, status bar, and menu, in addition
// to any other derived PegThings.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

extern PEGCHAR lsTEST[];

/*--------------------------------------------------------------------------*/
PegPrompt::PegPrompt(const PegRect &Rect, const PEGCHAR *Text, WORD wId, WORD wStyle) : 
    PegThing(Rect, wId, wStyle),
    PegTextThing(Text, wStyle & TT_COPY, PEG_PROMPT_FONT)
{
    Type(TYPE_PROMPT);
    InitClient();

    if (!(wStyle & AF_ENABLED))
    {
        RemoveStatus(PSF_SELECTABLE|PSF_ACCEPTS_FOCUS);
    }
    else
    {
        AddStatus(PSF_TAB_STOP);
        SetSignals(SIGMASK(PSF_FOCUS_RECEIVED));
    }

    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_STEXT] = PCLR_HIGH_TEXT;
    muColors[PCI_SELECTED] = PCLR_HIGH_TEXT_BACK;
    muColors[PCI_NORMAL] = PCLR_CLIENT;
}

/*--------------------------------------------------------------------------*/
PegPrompt::PegPrompt(SIGNED iLeft, SIGNED iTop, SIGNED iWidth,
 const PEGCHAR *Text, WORD wId, WORD wStyle, PegFont *pFont) :
 PegThing(wId, wStyle), PegTextThing(Text, wStyle & TT_COPY, PEG_PROMPT_FONT)
{
    Type(TYPE_PROMPT);

    if (pFont)
    {
        mpFont = pFont;
    }

    mReal.wLeft = iLeft;
    mReal.wTop = iTop;
    mReal.wRight = mReal.wLeft + iWidth;
    mReal.wBottom = mReal.wTop + TextHeight(lsTEST, mpFont) + 2;

    switch(wStyle & FF_MASK)
    {
    case FF_THICK:
        mReal.wBottom += PEG_FRAME_WIDTH * 2;
        break;

    case FF_THIN:
        mReal.wBottom += 2;
        break;

  #ifdef PEG_RUNTIME_COLOR_CHECK
    case FF_RAISED:
    case FF_RECESSED:
        if (NumColors() >= 4)
        {
            mReal.wBottom += 4;
        }
        else
        {
            mReal.wBottom += 2;
        }
        break;
  #else
   #if (PEG_NUM_COLORS != 2)

    case FF_RAISED:
    case FF_RECESSED:
        mReal.wBottom += 4;
        break;
   #endif
  #endif

    default:
        break;
    }
    
    InitClient();

    if (!(wStyle & AF_ENABLED))
    {
        RemoveStatus(PSF_SELECTABLE|PSF_ACCEPTS_FOCUS);
    }
    else
    {
        AddStatus(PSF_TAB_STOP);
        SetSignals(SIGMASK(PSF_FOCUS_RECEIVED));
    }

    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_SELECTED] = PCLR_HIGH_TEXT_BACK;
    muColors[PCI_STEXT] = PCLR_HIGH_TEXT;
    muColors[PCI_NORMAL] = PCLR_CLIENT;
}

/*--------------------------------------------------------------------------*/
PegPrompt::PegPrompt(SIGNED iLeft, SIGNED iTop, const PEGCHAR *Text, WORD wId,
    WORD wStyle, PegFont *pFont) : PegThing(wId, wStyle),
    PegTextThing(Text, wStyle & TT_COPY)
{
    Type(TYPE_PROMPT);

    if (!pFont)
    {
        #if defined(DEF_PROMPT_FONT)
         mpFont = DEF_PROMPT_FONT;
        #else
         mpFont = &SysFont;
        #endif
    }
    else
    {
        mpFont = pFont;
    }

    mReal.wLeft = iLeft;
    mReal.wTop = iTop;
    if (Text)
    {
        mReal.wRight = mReal.wLeft + TextWidth(Text, mpFont) + 2;
        mReal.wBottom = mReal.wTop + TextHeight(Text, mpFont) + 2;
    }

    switch(wStyle & FF_MASK)
    {
    case FF_THICK:
        mReal.wBottom += PEG_FRAME_WIDTH * 2;
        break;

    case FF_THIN:
        mReal.wBottom += 2;
        break;

  #ifdef PEG_RUNTIME_COLOR_CHECK
    case FF_RAISED:
    case FF_RECESSED:
        if (NumColors() >= 4)
        {
            mReal.wBottom += 4;
        }
        else
        {
            mReal.wBottom += 2;
        }
        break;

  #else
   #if (PEG_NUM_COLORS != 2)
    case FF_RAISED:
    case FF_RECESSED:
        mReal.wBottom += 4;
        break;
   #endif
  #endif

    default:
        break;
    }

    InitClient();

    if (!(wStyle & AF_ENABLED))
    {
        RemoveStatus(PSF_SELECTABLE|PSF_ACCEPTS_FOCUS);
    }
    else
    {
        AddStatus(PSF_TAB_STOP);
        SetSignals(SIGMASK(PSF_FOCUS_RECEIVED));
    }
    
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_SELECTED] = PCLR_HIGH_TEXT_BACK;
    muColors[PCI_STEXT] = PCLR_HIGH_TEXT;
    muColors[PCI_NORMAL] = PCLR_CLIENT;
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegPrompt::~PegPrompt()
{
}

/*--------------------------------------------------------------------------*/
void PegPrompt::SetFont(PegFont *Font)
{
    PegTextThing::SetFont(Font);

    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate(mReal);
    }
}

/*--------------------------------------------------------------------------*/
void PegPrompt::DataSet(const PEGCHAR *Text)
{
    PegTextThing::DataSet(Text);
    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate();
    }
}

/*--------------------------------------------------------------------------*/
SIGNED PegPrompt::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_CURRENT:
        PegThing::Message(Mesg);
        if (!(Style() & AF_TRANSPARENT))
        {
            Invalidate(mClip);
            Draw();
        }
        break;

    case PM_NONCURRENT:
        PegThing::Message(Mesg);
        if (!(Style() & AF_TRANSPARENT))
        {
            Invalidate(mClip);
            Draw();
        }
        break;

    default:
        PegThing::Message(Mesg);
        break;
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void PegPrompt::Draw(void)
{
    if (!Parent())
    {
        return;
    }
    PegColor Color;

    if (mwStyle & AF_TRANSPARENT && Parent())
    {
        Color.uBackground = Parent()->muColors[PCI_NORMAL];
    }
    else
    {
        if (StatusIs(PSF_CURRENT))
        {
            Color.uBackground = muColors[PCI_SELECTED];
        }
        else
        {
            Color.uBackground = muColors[PCI_NORMAL];
        }
    }

    BeginDraw();

    if ((Style() & AF_TRANSPARENT) != AF_TRANSPARENT ||
        (Style() & FF_MASK) != FF_NONE)
    {
        StandardBorder(Color.uBackground);
    }

    Color.uFlags = CF_NONE;

    if (StatusIs(PSF_CURRENT))
    {
        Color.uForeground = muColors[PCI_STEXT];
    }
    else
    {
        Color.uForeground = muColors[PCI_NTEXT];
    }

    // Draw the Text:
    if (mpText)
    {
        PegRect mSave = mClip;
        mClip &= mClient;

        PegPoint Point;
        Point.y = mClient.wTop + ((mClient.Height() - TextHeight(mpText, mpFont)) / 2);

        if (mwStyle & TJ_LEFT)
        {
            Point.x = mClient.wLeft + 1;
        }
        else
        {
            if (mwStyle & TJ_RIGHT)
            {
                Point.x = mClient.wRight - TextWidth(mpText, mpFont) - 1;
            }
            else
            {
                Point.x = mClient.wLeft;
                Point.x += (mClient.Width() - TextWidth(mpText, mpFont)) / 2;
            }
        }
        DrawText(Point, mpText, Color, mpFont);
        mClip = mSave;
    }

    if (First())
    {
        DrawChildren();
    }
    EndDraw();
}

// End of file
