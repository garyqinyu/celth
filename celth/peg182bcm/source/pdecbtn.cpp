/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pdecbtn.cpp - PegDecoratedButton. Derived button that supports both
//               text and a bitmap.
//
// Author: Jim DeLisle
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

extern PEGCHAR lsTEST[];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegDecoratedButton::PegDecoratedButton(PegRect& Rect, const PEGCHAR *pText, 
                                       PegBitmap* pBitmap, WORD wId /*=0*/, 
                                       WORD wStyle /*=AF_ENABLED*/,
                                       BOOL bFlyOver /*=FALSE*/) :
    PegButton(Rect, wId, wStyle),
    PegTextThing(pText, wStyle & TT_COPY, PEG_TBUTTON_FONT),
    mpBitmap(pBitmap),
    mbFlyOver(bFlyOver),
    mbInFlyOver(FALSE)
{
    Type(TYPE_DECORATEDBUTTON);

    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
}

/*--------------------------------------------------------------------------*/
PegDecoratedButton::PegDecoratedButton(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, 
                                       const PEGCHAR *pText, PegBitmap* pBitmap, 
                                       WORD wId /*=0*/, 
                                       WORD wStyle /*=AF_ENABLED*/,
                                       BOOL bFlyOver /*=FALSE*/) :
    PegButton(wId, wStyle),
    PegTextThing(pText, wStyle & TT_COPY, PEG_TBUTTON_FONT),
    mpBitmap(pBitmap),
    mbFlyOver(bFlyOver),
    mbInFlyOver(FALSE)
{
    Type(TYPE_DECORATEDBUTTON);

    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;

    mReal.wLeft = iLeft;
    mReal.wTop = iTop;
    mReal.wRight = mReal.wLeft + iWidth - 1;

    SIGNED iHeight = 0, iTextHeight = 0;
    
    if(pText)
    {
        iTextHeight = TextHeight(pText, mpFont) + TBUTTON_PADDING;
    }

    if(mpBitmap)
    {
        if(((mwStyle & BF_ORIENT_TR) && (mwStyle & BF_ORIENT_BR)) ||
           ((!(mwStyle & BF_ORIENT_TR)) && (!(mwStyle & BF_ORIENT_BR))))
        {
            // The bitmap is on the left or right of the text
            iHeight = (mpBitmap->wHeight + TBUTTON_PADDING) > iTextHeight ? 
                       mpBitmap->wHeight + TBUTTON_PADDING : iTextHeight;
            iHeight += BMP_TEXT_PADDING;
        }
        else
        {
            // The bitmap is above or below the text
            iHeight = mpBitmap->wHeight + iTextHeight + BMP_TEXT_PADDING;
        }
    }

    mReal.wBottom = mReal.wTop + iHeight;
    
    mClient = mReal;
}

/*--------------------------------------------------------------------------*/
PegDecoratedButton::PegDecoratedButton(SIGNED iLeft, SIGNED iTop, 
                                       const PEGCHAR *pText, PegBitmap* pBitmap, 
                                       WORD wId /*=0*/, 
                                       WORD wStyle /*=AF_ENABLED*/,
                                       BOOL bFlyOver /*=FALSE*/) :
    PegButton(wId, wStyle),
    PegTextThing(pText, wStyle & TT_COPY, PEG_TBUTTON_FONT),
    mpBitmap(pBitmap),
    mbFlyOver(bFlyOver),
    mbInFlyOver(FALSE)
{
    Type(TYPE_DECORATEDBUTTON);

    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;

    mReal.wLeft = iLeft;
    mReal.wTop = iTop;

    SIGNED iHeight = 0, iTextHeight = 0, iWidth = 0, iTextWidth = 0;
    
    if(pText)
    {
        iTextHeight = TextHeight(pText, mpFont) + TBUTTON_PADDING;
        iTextWidth = TextWidth(pText, mpFont) + TBUTTON_PADDING;
        iHeight = iTextHeight;
        iWidth = iTextWidth;
    }

    if(mpBitmap)
    {
        if(((mwStyle & BF_ORIENT_TR) && (mwStyle & BF_ORIENT_BR)) ||
           ((!(mwStyle & BF_ORIENT_TR)) && (!(mwStyle & BF_ORIENT_BR))))
        {
            // The bitmap is on the left or right of the text
            iHeight = (mpBitmap->wHeight + TBUTTON_PADDING) > iTextHeight ? 
                       mpBitmap->wHeight + TBUTTON_PADDING : iTextHeight;
            iHeight += BMP_TEXT_PADDING;

            iWidth = iTextWidth + mpBitmap->wWidth + TBUTTON_PADDING +
                     (pText ? BMP_TEXT_PADDING + 3 : TBUTTON_PADDING);
        }
        else
        {
            // The bitmap is above or below the text
            iHeight = mpBitmap->wHeight + iTextHeight + BMP_TEXT_PADDING +
                      TBUTTON_PADDING;

            iWidth = (mpBitmap->wWidth + TBUTTON_PADDING) > iTextWidth ?
                      mpBitmap->wWidth + TBUTTON_PADDING : iTextHeight;
            iWidth += BMP_TEXT_PADDING;
        }
    }

    mReal.wBottom = mReal.wTop + iHeight;
    mReal.wRight = mReal.wLeft + iWidth;    

    mClient = mReal;
}

/*--------------------------------------------------------------------------*/
void PegDecoratedButton::Draw()
{
    BeginDraw();

    if(!mbFlyOver || (mbFlyOver && mbInFlyOver))
    {
        PegButton::Draw();
    }
    else
    {
        // Draw ourselves flat, no border
        PegColor tColor(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
        Rectangle(mReal, tColor);
        mClient = mReal;
    }

    // Draw the text and the bitmap
    SIGNED iTextHeight = 0, iTextWidth = 0;
    if(mpText)
    {
        iTextHeight = TextHeight(mpText, mpFont);
        iTextWidth = TextWidth(mpText, mpFont);
    }

    PegPoint tPutText, tPutBitmap;

   #if (PEG_NUM_COLORS > 2)
    PegColor tTextColor(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
   #else
    PegColor tTextColor(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
   #endif

    if(mpBitmap)
    {
        // Where is it suppose to be and where is the text suppose to be
        if((mwStyle & BF_ORIENT_TR) && (mwStyle & BF_ORIENT_BR))
        {
            // The bitmap is on the right of the text.
            tPutBitmap.x = mClient.wRight - (mpBitmap->wWidth + TBUTTON_PADDING);
            tPutBitmap.y = mClient.wTop + ((mClient.Height() - mpBitmap->wHeight) >> 1);

            if(mpText)
            {
                //tPutText.x = mClient.wLeft + TBUTTON_PADDING;
                tPutText.x = tPutBitmap.x - (BMP_TEXT_PADDING + iTextWidth);
                tPutText.y = mClient.wTop + ((mClient.Height() - iTextHeight) >> 1);
            }
        }
        else if((!(mwStyle & BF_ORIENT_TR)) && (!(mwStyle & BF_ORIENT_BR)))
        {
            // The bitmap is on the left of the text
            tPutBitmap.x = mClient.wLeft + TBUTTON_PADDING;
            tPutBitmap.y = mClient.wTop + ((mClient.Height() - mpBitmap->wHeight) >> 1);

            if(mpText)
            {
                tPutText.x = tPutBitmap.x + mpBitmap->wWidth + BMP_TEXT_PADDING;
                tPutText.y = mClient.wTop + ((mClient.Height() - iTextHeight) >> 1);
            }
        }
        else if((mwStyle & BF_ORIENT_TR) && (!(mwStyle & BF_ORIENT_BR)))
        {
            // The bitmap is on the top of the text
            tPutBitmap.x = mClient.wLeft + ((mClient.Width() - mpBitmap->wWidth) >> 1);
            tPutBitmap.y = mClient.wTop + TBUTTON_PADDING;

            if(mpText)
            {
                tPutText.x = mClient.wLeft + ((mClient.Width() - iTextWidth) >> 1);
                tPutText.y = tPutBitmap.y + mpBitmap->wHeight + BMP_TEXT_PADDING;
            }
        }
        else
        {
            // The bitmap is under the text
            tPutBitmap.x = mClient.wLeft + ((mClient.Width() - mpBitmap->wWidth) >> 1);
            tPutBitmap.y = mClient.wBottom - (mpBitmap->wHeight + BMP_TEXT_PADDING);

            if(mpText)
            {
                tPutText.x = mClient.wLeft + ((mClient.Width() - iTextWidth) >> 1);
                tPutText.y = tPutBitmap.y - (iTextHeight + BMP_TEXT_PADDING);
            }
        }

        Bitmap(tPutBitmap, mpBitmap);

        if(mpText)
        {
            DrawText(tPutText, mpText, tTextColor, mpFont);
        }
    }
    else
    {
        if(mpText)
        {
            tPutText.x = mClient.wLeft + ((mClient.Width() - iTextWidth) >> 1);
            tPutText.y = mClient.wTop + ((mClient.Height() - iTextHeight) >> 1);

            DrawText(tPutText, mpText, tTextColor, mpFont);
        }
    }

    EndDraw();
}

/*--------------------------------------------------------------------------*/
SIGNED PegDecoratedButton::Message(const PegMessage& Mesg)
{
    switch(Mesg.wType)
    {
    case PM_NONCURRENT:
        PegButton::Message(Mesg);

        if(mbFlyOver)
        {
            mbInFlyOver = FALSE;
        }
        break;

    case PM_POINTER_ENTER:
        PegButton::Message(Mesg);
        
        if(mbFlyOver)
        {
            mbInFlyOver = TRUE;
        }

        Invalidate(mReal);
        Draw();

        break;

    case PM_POINTER_EXIT:
        PegButton::Message(Mesg);

        if(mbFlyOver)
        {
            mbInFlyOver = FALSE;
        }

        Invalidate(mReal);
        Draw();

        break;

    default:
        return(PegButton::Message(Mesg));
    }

    return 0;
}


