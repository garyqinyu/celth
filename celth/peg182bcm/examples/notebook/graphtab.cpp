/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// graphtab.cpp - A simple class for diplaying text and a small graphic.
//    This class is used by the notebook demo to illustrate using custom
//    notebook tabs. This could be used for many other things as well.
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
#include "graphtab.hpp"

/*--------------------------------------------------------------------------*/
GraphicTab::GraphicTab(PegBitmap *bm, char *Text) : PegThing(0, FF_NONE),
    PegTextThing(Text)
{
    SIGNED iWidth, iHeight;

    iWidth = bm->wWidth;
    iHeight = bm->wHeight;

    if (Text)
    {
        iWidth += Screen()->TextWidth(Text, mpFont) + 3;
    }

    // set my initial mReal and mClient values. Only the width and height
    // matter, since the notebook will position me when I am displayed:

    mReal.Set(0, 0, iWidth + 2, iHeight + 2);
    mClient = mReal;
    mpBitmap = bm;
}

/*--------------------------------------------------------------------------*/
void GraphicTab::Draw(void)
{
    Screen()->BeginDraw(this);

    // display the bitmap, left justified and vertically centered:

    PegPoint Put;
    Put.x = mClient.wLeft;
    Put.y = (mClient.wTop + mClient.wBottom) >> 1;
    Put.y -= mpBitmap->wHeight >> 1;
    Screen()->Bitmap(this, Put, mpBitmap);

    // display the text, right next to the bitmap and vertically centered:

    if (mpText)     // PegTextThing data member
    {
        PegColor Color(BLACK, 0, CF_NONE);
        Put.x += mpBitmap->wWidth + 3;
        Put.y = (mClient.wTop + mClient.wBottom) >> 1;
        Put.y -= Screen()->TextHeight(mpText, mpFont) >> 1;

        Screen()->DrawText(this, Put, mpText, Color, mpFont);
    }

    Screen()->EndDraw();
}



