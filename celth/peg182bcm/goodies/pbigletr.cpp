/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pbitletr.cpp - A PegPrompt that make the first letter bigger than
//    the rest.
//
// Author: Kenneth G. Maxwell
//
// Version: 1.04
//
// Copyright (c) 1997-1998 Swell Software 
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
#include "pbigletr.hpp"


// Change this to be whatever the large first character font should be:
#define FIRST_LETTER_FONT &SysFont

// And change this to be the font for the rest of the character(s)
#define OTHER_LETTERS_FONT &MenuFont

/*--------------------------------------------------------------------------*/
// provide all of the same constructors as PegPrompt:
/*--------------------------------------------------------------------------*/
BigLetterPrompt::BigLetterPrompt(PegRect &Rect, char *Text, WORD wId) :
    PegPrompt(Rect, Text, wId, FF_RECESSED|TJ_LEFT)
{
    mpFont = OTHER_LETTERS_FONT;
}


/*--------------------------------------------------------------------------*/
BigLetterPrompt::BigLetterPrompt(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, 
    char *Text, WORD wId) : PegPrompt(iLeft, iTop, iWidth, Text, wId,
    FF_RECESSED|TJ_LEFT)
{
    // put the main font you would like to use here:
    mpFont = OTHER_LETTERS_FONT;
}

/*--------------------------------------------------------------------------*/
BigLetterPrompt::BigLetterPrompt(SIGNED iLeft, SIGNED iTop, char *Text,
    WORD wId) : PegPrompt(iLeft, iTop, Text, wId, FF_NONE|TJ_LEFT)
{
    // put the font you would like to use here:
    mpFont = OTHER_LETTERS_FONT;
}


/*--------------------------------------------------------------------------*/
// Override draw function for some custom appearance:
/*--------------------------------------------------------------------------*/
void BigLetterPrompt::Draw(void)
{
    PegColor Color(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
    BeginDraw();

    StandardBorder(Color.uBackground);

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
        // The first character is in a bigger font than the rest.
        // For this example we are using SysFont for the big font, and
        // the default prompt font for the small font. The fonts used can
        // be custom fonts just as easily.

        PegPoint Point;
        SIGNED iFirstWidth;
        char cTemp[2];

        Point.y = mClient.wTop;

        // get width of first character in big font:
        cTemp[0] = *mpText;
        cTemp[1] = '\0';
        iFirstWidth = TextWidth(cTemp, FIRST_LETTER_FONT);

        if (mwStyle & TJ_LEFT)
        {
            Point.x = mClient.wLeft + 1;
        }
        else
        {
            SIGNED iTotWidth;
            iTotWidth = iFirstWidth + TextWidth(mpText + 1, mpFont);

            if (mwStyle & TJ_RIGHT)
            {
                Point.x = mClient.wRight - iTotWidth - 1;
            }
            else
            {
                // center justified:

                Point.x = (mClient.wLeft + mClient.wRight) / 2;
                Point.x += iTotWidth / 2;
            }
        }
        DrawText(Point, cTemp, Color, FIRST_LETTER_FONT);

        // move over by the width of the first character:
        Point.x += iFirstWidth;

        // move down by difference in font heights so that text lines up
        // at the bottom (this might have to be adjusted by a few pixels
        // depending on fonts used):

        Point.y += TextHeight("H", FIRST_LETTER_FONT);
        Point.y -= TextHeight("H", mpFont) + 1;
        DrawText(Point, mpText + 1, Color, mpFont);
    }
    EndDraw();
}

