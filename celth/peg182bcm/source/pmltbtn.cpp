/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pmltbtn.cpp - Multi-line text button class.
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

#include "string.h"
#include "peg.hpp"

extern PEGCHAR lsTEST[];

/*--------------------------------------------------------------------------*/
// PegMLTextButton class implementation:
/*--------------------------------------------------------------------------*/
PegMLTextButton::PegMLTextButton(PegRect &Rect, const PEGCHAR *Text,
    PEGCHAR cMarker, WORD wId, WORD wStyle) : PegButton(Rect, wId, wStyle)
{
    Type(TYPE_MLTEXTBUTTON);
   
    mpFont = PegTextThing::GetDefaultFont(PEG_TBUTTON_FONT);

   mcMarker = cMarker;
   mwNumRows = 0;
   ParseButtonText(Text);
}

/*--------------------------------------------------------------------------*/
PegMLTextButton::PegMLTextButton(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, 
    const PEGCHAR *Text, PEGCHAR cMarker, WORD wId, WORD wStyle) :
    PegButton(wId, wStyle)
{
    Type(TYPE_MLTEXTBUTTON);

    mpFont = PegTextThing::GetDefaultFont(PEG_TBUTTON_FONT);

    mcMarker = cMarker;
    mReal.wLeft = iLeft;
    mReal.wTop = iTop;
    mReal.wRight = mReal.wLeft + iWidth - 1;

    mwNumRows = 0;
    ParseButtonText(Text);

    if (mwNumRows)
    {
        mReal.wBottom = mReal.wTop + (mwNumRows * TextHeight(Text, mpFont)) + 3;
    }
    else
    {
        mReal.wBottom = mReal.wTop + 3;
    }
    mClient = mReal;
}


/*--------------------------------------------------------------------------*/
PegMLTextButton::PegMLTextButton(SIGNED iLeft, SIGNED iTop, const PEGCHAR *Text,
    PEGCHAR cMarker, WORD wId, WORD wStyle) : PegButton(wId, wStyle)
{
    Type(TYPE_MLTEXTBUTTON);
    mpFont = PegTextThing::GetDefaultFont(PEG_TBUTTON_FONT);

    mcMarker = cMarker;
    mReal.wLeft = iLeft;
    mReal.wTop = iTop;

    mwNumRows = 0;
    WORD wWidest = ParseButtonText(Text);

    if (mwNumRows)
    {
        mReal.wRight = mReal.wLeft + wWidest + 6;
        mReal.wBottom = mReal.wTop + (mwNumRows * TextHeight(Text, mpFont)) + 3;
    }
    else
    {
        mReal.wRight = mReal.wLeft + 3;
        mReal.wBottom = mReal.wTop + 3;
    }
    mClient = mReal;
}

/*--------------------------------------------------------------------------*/
PegMLTextButton::~PegMLTextButton()
{
    FreeTextInfo();
}

#ifndef PEG_RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegMLTextButton::Draw(void)
{
    BeginDraw();
    PegButton::Draw();

    if (!mwNumRows)
    {
        EndDraw();
        return;
    }

   #if (PEG_NUM_COLORS > 2)
    PegColor Color(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
   #else
    PegColor Color(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
   #endif

    if (!(Style() & AF_ENABLED))
    {
        Color.uForeground = PCLR_LOWLIGHT;
    }

    // now draw the text:

    PegPoint Put;
    SIGNED iSize;
    SIGNED iWidth;

    SIGNED iRowHeight = TextHeight(lsTEST, mpFont);
    iSize = mwNumRows * iRowHeight;

    if (iSize <= mClient.Height())
    {
        Put.y = mClient.wTop + (mClient.Height() - iSize) / 2;
    }
    else
    {
        Put.y = mClient.wTop;
    }

    for (WORD wLoop = 0; wLoop < mwNumRows; wLoop++)
    {
        iWidth = miWidths[wLoop];

        if (iWidth)
        {
            if (Style() & TJ_LEFT)
            {
                Put.x = mClient.wLeft + 1;
            }
            else
            {
                if (Style() & TJ_RIGHT)
                {
                    Put.x = mClient.wRight - iWidth - 1;
                }
                else
                {
                    Put.x = mClient.wLeft + (mClient.Width() - iWidth) / 2;
                }
            }

            DrawText(Put, mpText[wLoop], Color, mpFont);

           #if (PEG_NUM_COLORS == 2)
            if (!(Style() & AF_ENABLED))
            {
                SIGNED yCenter = Put.y + iRowHeight / 2;
                Line(Put.x, yCenter, Put.x + iWidth, yCenter, Color);
            }
           #endif
        }
        Put.y += iRowHeight;
    }

    DrawChildren();
    EndDraw();
}

#else   // here for RUNTIME_COLOR_CHECK
/*--------------------------------------------------------------------------*/
void PegMLTextButton::Draw(void)
{
    BeginDraw();
    PegButton::Draw();

    if (!mwNumRows)
    {
        EndDraw();
        return;
    }

    PegColor Color(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
    if (NumColors() < 4)
    {
        Color.uFlags = CF_FILL;
    }

    if (!(Style() & AF_ENABLED))
    {
        Color.uForeground = PCLR_LOWLIGHT;
    }

    // now draw the text:

    PegPoint Put;
    SIGNED iWidth;
    SIGNED iSize, iRowHeight;
    iRowHeight = TextHeight(lsTEST, mpFont);
    iSize = iRowHeight * mwNumRows;

    if (iSize <= mClient.Height())
    {
        Put.y = mClient.wTop + (mClient.Height() - iSize) / 2;
    }
    else
    {
        Put.y = mClient.wTop;
    }

    for (WORD wLoop = 0; wLoop < mwNumRows; wLoop++)
    {
        iWidth = miWidths[wLoop];
        if (iWidth)
        {
            if (Style() & TJ_LEFT)
            {
                Put.x = mClient.wLeft + 1;
            }
            else
            {
                if (Style() & TJ_RIGHT)
                {
                    Put.x = mClient.wRight - iWidth - 1;
                }
                else
                {
                    Put.x = mClient.wLeft + (mClient.Width() - iWidth) / 2;
                }
            }

            DrawText(Put, mpText[wLoop], Color, mpFont);

            if (NumColors() < 4)
            {
                if (!(Style() & AF_ENABLED))
                {
                    SIGNED yCenter = Put.y + (iRowHeight / 2);
                    Line(Put.x, yCenter, Put.x + iWidth, yCenter, Color);
                }
            }
        }
        Put.y += iRowHeight;
    }
    EndDraw();
}

#endif  // RUNTIME_COLOR_CHECK if


/*--------------------------------------------------------------------------*/
void PegMLTextButton::DataSet(const PEGCHAR *Text)
{
    ParseButtonText(Text);
    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate();
    }
}

/*--------------------------------------------------------------------------*/
void PegMLTextButton::SetFont(PegFont *pFont)
{
    mpFont = pFont;

    for (WORD wLoop =0; wLoop < mwNumRows; wLoop++)
    {
        if (mpText[wLoop])
        {
            miWidths[wLoop] = TextWidth(mpText[wLoop], mpFont);
        }
    }
}


/*--------------------------------------------------------------------------*/
void PegMLTextButton::FreeTextInfo(void)
{
    if (mwNumRows)
    {
        delete [] miWidths;

        for (WORD wLoop = 0; wLoop < mwNumRows; wLoop++)
        {
            if (mpText[wLoop])
            {
                delete [] mpText[wLoop];
            }
        }
        delete [] mpText;
    }
    mwNumRows = 0;
}


/*--------------------------------------------------------------------------*/
SIGNED PegMLTextButton::ParseButtonText(const PEGCHAR *Text)
{
    FreeTextInfo();

    if (!Text)
    {
        return 0;
    }
    if (!(*Text))
    {
        return 0;
    }

    // count how many strings we need:

    const PEGCHAR *pLast;
    const PEGCHAR *pGet = Text;
    mwNumRows = 1;

    while(*pGet)
    {
        if (*pGet == mcMarker)
        {
            mwNumRows++;
        }
        pGet++;
    }

    mpText = new PEGCHAR *[mwNumRows];
    miWidths = new SIGNED[mwNumRows];

    // now save off the information:

    pGet = Text;
    pLast = pGet;
    SIGNED iIndex = 0;
    SIGNED iWidest = 0;

    while(iIndex < mwNumRows)
    {
        if (*pGet == mcMarker || *pGet == 0)
        {
            if (pGet > pLast)
            {
                mpText[iIndex] = new PEGCHAR[pGet - pLast + 1];
                strncpy(mpText[iIndex], pLast, pGet - pLast);
                *(mpText[iIndex] + (pGet - pLast)) = 0;
                miWidths[iIndex] = TextWidth(mpText[iIndex], mpFont);

                if (miWidths[iIndex] > iWidest)
                {
                    iWidest = miWidths[iIndex];
                }
            }
            else
            {
                mpText[iIndex] = NULL;
                miWidths[iIndex] = 0;
            }
            iIndex++;
            pLast = pGet + 1;
        }
        pGet++;
    }
    return iWidest;
}



