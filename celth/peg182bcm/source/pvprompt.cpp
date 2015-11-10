/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pvprompt.cpp - PegPrompt object that displays text vertically.
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

#include "string.h"
#include "peg.hpp"

extern PEGCHAR lsTEST[];

/*--------------------------------------------------------------------------*/
PegVPrompt::PegVPrompt(const PegRect &Rect, const PEGCHAR *Text, WORD wId,
    WORD wStyle) : PegPrompt(Rect, Text, wId, wStyle)
{
    Type(TYPE_VPROMPT);
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
/*--------------------------------------------------------------------------*/
void PegVPrompt::Draw(void)
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

    StandardBorder(Color.uBackground);

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
        SIGNED iTxtHeight, iCharHeight, iTemp;
        PegPoint Point;
        PEGCHAR cTemp[2];
        cTemp[1] = 0;
        PEGCHAR *pGet = mpText;
        iCharHeight = TextHeight(mpText, mpFont) - 1;
        iTxtHeight = iCharHeight * strlen(mpText);

        Point.y = mClient.wTop;
        Point.y += (mClient.Height() - iTxtHeight) / 2;
        iTemp = (mClient.wLeft + mClient.wRight) / 2;

        while(*pGet)
        {
            cTemp[0] = *pGet++;
            Point.x = iTemp - (TextWidth(cTemp, mpFont) / 2);
            DrawText(Point, cTemp, Color, mpFont);
            Point.y += iCharHeight;
        }
    }
    DrawChildren();
    EndDraw();
}

// End of file
