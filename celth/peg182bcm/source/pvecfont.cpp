/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pvecfont.cpp - 
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
//    Vector font used by PEG library for creating variable height and style
//    bitmapped fonts at runtime.
//
//    This vector font was hand-created by Swell Software.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#ifdef PEG_VECTOR_FONTS

ROMDATA UCHAR Vector_width_table[128] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x04,0x02,0x05,0x06,0x06,0x07,0x06,0x04,0x05,0x05,0x06,0x06,0x06,0x06,0x03,0x06,
0x06,0x04,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x04,0x04,0x06,0x06,0x05,0x06,
0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x05,0x06,0x06,0x06,0x06,0x06,0x06,
0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x05,0x06,0x05,0x06,0x06,
0x06,0x06,0x06,0x06,0x06,0x06,0x04,0x06,0x06,0x04,0x05,0x05,0x04,0x06,0x06,0x06,
0x06,0x06,0x05,0x06,0x04,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x03,0x06,0x06,0x06,

};

// The data table for a vector font is different than a bitmapped font.
// The data is comprised of 3-byte triplets. This first byte is an opcode,
// and the second and third bytes are x,y vectors from the lower-left corner
// of the character.

ROMDATA UCHAR Vector_data_table[] = {
2,4,0,0,
2,0,2,3,0,7,2,0,1,3,0,0,2,2,0,0,
2,5,0,0,
2,1,0,3,1,6,2,3,6,3,3,0,2,0,4,3,3,4,3,4,4,2,0,2,3,4,2,2,6,0,0,
2,0,1,3,1,0,3,3,0,3,4,1,3,4,2,3,3,3,3,1,3,3,0,4,3,0,5,3,1,6,3,3,6,3,4,5,2,2,6,3,2,0,2,6,0,0,
2,0,1,3,5,6,2,0,6,3,0,7,3,1,7,3,1,6,3,0,6,2,4,0,3,4,1,3,5,1,3,5,0,3,4,0,2,7,0,0,
2,4,0,3,3,1,3,2,0,3,1,0,3,0,1,3,0,2,3,1,3,3,0,4,3,0,5,3,1,6,3,2,6,3,3,5,3,3,4,3,3,4,3,2,3,3,3,2,3,3,1,2,4,3,3,3,2,2,1,3,3,2,3,2,6,0,0,
2,1,7,3,1,5,3,0,4,2,4,0,0,
2,2,0,3,0,2,3,0,4,3,2,6,2,5,0,0,
2,0,0,3,2,2,3,2,4,3,0,6,2,5,0,0,
2,0,5,3,4,1,2,0,1,3,4,5,2,0,3,3,4,3,2,2,1,3,2,5,2,6,0,0,
2,0,3,3,4,3,2,2,5,3,2,1,2,6,0,0,
2,1,255,3,2,0,3,2,1,3,1,1,3,1,0,3,2,0,3,1,255,2,1,255,3,0,254,2,6,0,0,
2,0,3,3,4,3,2,6,0,0,
2,0,0,3,0,1,3,1,1,3,1,0,3,0,0,2,5,0,0,
2,0,0,3,4,4,2,6,0,0,
2,0,5,3,1,6,3,3,6,3,4,5,3,4,1,3,3,0,3,1,0,3,0,1,3,0,5,2,6,0,0,
2,0,0,3,2,0,3,1,0,3,1,6,3,1,6,3,0,5,2,5,0,0,
2,4,0,3,0,0,3,4,4,3,4,5,3,3,6,3,1,6,3,0,5,2,6,0,0,
2,0,5,3,1,6,3,3,6,3,4,5,3,4,4,3,4,4,3,4,4,3,4,4,3,3,3,3,2,3,3,3,3,3,4,2,3,4,1,3,3,0,3,1,0,3,0,1,2,6,0,0,
2,3,0,3,3,6,3,0,3,3,4,3,2,6,0,0,
2,4,6,3,0,6,3,0,3,3,2,4,3,3,4,3,4,3,3,4,1,3,3,0,3,1,0,3,0,1,2,6,0,0,
2,4,6,3,2,5,3,0,3,3,0,1,3,1,0,3,3,0,3,4,1,3,4,2,3,3,3,3,0,3,2,6,0,0,
2,0,6,3,4,6,3,4,5,3,0,1,3,0,0,2,6,0,0,
2,0,5,3,1,6,3,3,6,3,4,5,3,4,4,3,3,3,3,4,2,3,4,1,3,3,0,3,1,0,3,0,1,3,0,2,3,1,3,3,0,4,3,0,5,2,1,3,3,3,3,2,6,0,0,
2,0,1,3,1,0,3,3,0,3,4,1,3,4,5,3,3,6,3,1,6,3,0,5,3,0,4,3,1,3,3,4,3,2,6,0,0,
2,0,4,3,1,4,3,1,3,3,0,3,3,0,4,2,0,1,3,1,1,3,1,0,3,0,0,3,0,1,2,5,0,0,
2,0,3,3,0,4,3,1,4,3,1,3,3,0,3,2,0,1,3,1,1,3,1,0,3,0,255,2,1,0,3,0,0,3,0,1,2,5,0,0,
2,3,0,3,0,3,3,3,6,2,6,0,0,
2,0,2,3,4,2,2,0,4,3,4,4,2,6,0,0,
2,0,0,3,3,3,3,0,6,2,5,0,0,
2,0,5,3,1,6,3,3,6,3,4,5,3,4,4,3,2,2,2,2,1,3,2,0,2,6,0,0,
2,3,2,3,3,3,3,2,3,3,2,2,3,3,2,3,4,3,3,4,5,3,3,6,3,1,6,3,0,5,3,0,1,3,1,0,3,4,0,2,6,0,0,
2,0,0,3,0,4,3,2,6,3,4,4,3,4,0,3,4,3,3,0,3,2,6,0,0,
2,0,6,3,0,0,3,3,0,3,4,1,3,4,2,3,3,3,3,0,3,3,0,3,3,0,3,3,0,3,3,3,3,3,4,4,3,4,5,3,3,6,3,0,6,2,6,0,0,
2,4,5,3,3,6,3,1,6,3,0,5,3,0,1,3,1,0,3,3,0,3,4,1,2,6,0,0,
2,0,6,3,0,0,3,3,0,3,4,1,3,4,5,3,3,6,3,0,6,2,6,0,0,
2,4,6,3,0,6,3,0,0,3,4,0,2,0,3,3,2,3,2,6,0,0,
2,4,6,3,0,6,3,0,3,3,2,3,3,0,3,3,0,0,2,6,0,0,
2,4,5,3,3,6,3,1,6,3,0,5,3,0,1,3,1,0,3,3,0,3,4,1,3,4,3,3,2,3,2,6,0,0,
2,0,6,3,0,0,3,0,3,3,4,3,3,4,6,3,4,0,2,6,0,0,
2,0,6,3,2,6,3,1,6,3,1,0,3,0,0,3,2,0,2,5,0,0,
2,4,6,3,2,6,3,3,6,3,3,1,3,2,0,3,1,0,3,0,1,2,6,0,0,
2,0,6,3,0,0,3,0,2,3,4,6,3,1,3,3,4,0,2,6,0,0,
2,0,6,3,0,0,3,4,0,2,6,0,0,
2,0,0,3,0,6,3,2,4,3,4,6,3,4,0,2,6,0,0,
2,0,0,3,0,6,3,4,0,3,4,6,2,6,0,0,
2,3,6,3,1,6,3,0,5,3,0,1,3,1,0,3,3,0,3,4,1,3,4,5,3,3,6,2,6,0,0,
2,0,0,3,0,6,3,3,6,3,4,5,3,4,3,3,3,2,3,0,2,2,6,0,0,
2,4,1,3,4,5,3,3,6,3,1,6,3,0,5,3,0,1,3,1,0,3,3,0,2,2,2,3,4,0,2,3,0,3,4,1,2,6,0,0,
2,0,0,3,0,6,3,3,6,3,4,5,3,4,3,3,3,2,3,0,2,2,2,2,3,4,0,2,6,0,0,
2,4,5,3,3,6,3,1,6,3,0,5,3,0,4,3,1,3,3,3,3,3,4,2,3,4,1,3,3,0,3,1,0,3,0,1,2,6,0,0,
2,0,6,3,4,6,3,2,6,3,2,0,2,6,0,0,
2,0,6,3,0,1,3,1,0,3,3,0,3,4,1,3,4,6,2,6,0,0,
2,0,6,3,0,2,3,2,0,3,4,2,3,4,6,2,6,0,0,
2,0,6,3,0,0,3,2,2,3,4,0,3,4,6,2,6,0,0,
2,0,6,3,4,0,2,4,6,3,0,0,2,6,0,0,
2,0,6,3,0,5,3,2,3,3,4,5,3,4,6,2,2,3,3,2,0,2,6,0,0,
2,0,6,3,4,6,3,4,5,3,0,1,3,0,0,3,4,0,2,6,0,0,
2,2,0,3,0,0,3,0,6,3,2,6,2,5,0,0,
2,4,0,3,0,4,2,6,0,0,
2,0,0,3,2,0,3,2,6,3,0,6,2,5,0,0,
2,0,3,3,2,5,3,4,3,2,6,0,0,
2,0,255,3,4,255,2,6,0,0,
2,0,5,3,2,3,2,6,0,0,
2,1,4,3,3,4,3,4,3,3,4,0,3,1,0,3,0,1,3,1,2,3,4,2,2,6,0,0,
2,0,6,3,0,0,3,3,0,3,4,1,3,4,2,3,4,3,3,3,4,3,0,4,2,6,0,0,
2,4,4,3,1,4,3,0,3,3,0,1,3,1,0,3,4,0,2,6,0,0,
2,4,6,3,4,0,3,1,0,3,0,1,3,0,3,3,1,4,3,4,4,2,6,0,0,
2,3,0,3,1,0,3,0,1,3,0,3,3,1,4,3,3,4,3,4,3,3,4,2,3,0,2,2,6,0,0,
2,1,0,3,1,5,3,1,3,3,0,3,3,2,3,3,1,3,3,1,5,3,2,6,2,5,0,0,
2,2,254,3,3,254,3,4,255,3,4,4,3,1,4,3,0,3,3,0,1,3,1,0,3,4,0,2,6,0,0,
2,0,6,3,0,0,3,0,3,3,1,4,3,3,4,3,4,3,3,4,0,2,6,0,0,
2,0,0,3,2,0,3,1,0,3,1,4,3,0,4,2,1,6,2,1,6,3,1,6,2,5,0,0,
2,0,254,3,1,254,3,2,255,3,2,4,2,2,6,3,2,6,2,6,0,0,
2,0,0,3,0,6,2,0,2,3,3,4,2,0,2,3,3,0,2,6,0,0,
2,0,0,3,2,0,3,1,0,3,1,6,3,0,6,2,5,0,0,
2,0,0,3,0,4,3,1,4,3,2,3,3,2,0,3,2,3,3,3,4,3,4,3,3,4,0,2,6,0,0,
2,0,0,3,0,4,3,3,4,3,4,3,3,4,0,2,6,0,0,
2,1,0,3,3,0,3,4,1,3,4,3,3,3,4,3,1,4,3,0,3,3,0,1,3,1,0,2,6,0,0,
2,0,254,3,0,4,3,3,4,3,4,3,3,4,1,3,3,0,3,0,0,2,6,0,0,
2,4,254,3,4,4,3,1,4,3,0,3,3,0,1,3,1,0,3,4,0,2,6,0,0,
2,0,4,3,0,0,3,0,2,3,1,3,3,2,4,3,3,4,2,6,0,0,
2,0,0,3,3,0,3,4,1,3,3,2,3,1,2,3,0,3,3,1,4,3,4,4,2,6,0,0,
2,1,5,3,1,4,3,0,4,3,2,4,3,1,4,3,1,1,3,2,0,2,5,0,0,
2,0,4,3,0,1,3,1,0,3,4,0,3,4,4,2,6,0,0,
2,0,4,3,2,0,3,4,4,2,6,0,0,
2,0,4,3,0,1,3,1,0,3,2,1,3,3,0,3,4,1,3,4,4,2,6,0,0,
2,0,4,3,4,0,3,2,2,3,4,4,3,0,0,2,6,0,0,
2,0,4,3,0,1,3,1,0,3,4,0,3,4,255,3,3,254,3,2,254,2,4,0,3,4,4,2,6,0,0,
2,0,4,3,4,4,3,0,0,3,4,0,2,6,0,0,
2,3,6,3,2,6,3,1,5,3,1,4,3,0,3,3,1,2,3,1,1,3,2,0,3,3,0,2,6,0,0,
2,0,6,3,0,0,2,3,0,0,
2,0,6,3,1,6,3,2,5,3,2,4,3,3,3,3,2,2,3,2,1,3,1,0,3,0,0,2,6,0,0,
2,0,5,3,1,6,3,3,6,3,4,7,2,6,0,0,
2,0,2,3,0,0,3,4,0,3,4,2,3,2,5,3,0,2,2,6,0,0,

};


ROMDATA WORD Vector_offset_table[128] = {
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0004,0x0014,0x0018,0x0037,0x0065,0x008d,0x00d0,0x00dd,0x00ed,0x00fd,0x0119,0x0129,0x0148,0x0152,0x0165,
0x016f,0x018e,0x01a4,0x01bd,0x01f1,0x0201,0x0223,0x0245,0x0258,0x028f,0x02b4,0x02d6,0x02fe,0x030b,0x031b,0x0328,
0x0344,0x036f,0x0388,0x03b9,0x03d5,0x03ee,0x0404,0x041a,0x043c,0x0452,0x0468,0x0481,0x0497,0x04a4,0x04b7,0x04c7,
0x04e6,0x04ff,0x0527,0x0546,0x056e,0x057e,0x0594,0x05a7,0x05ba,0x05ca,0x05e3,0x05f9,0x0609,0x0613,0x0623,0x0630,
0x063a,0x0644,0x0660,0x067c,0x0692,0x06ab,0x06ca,0x06e6,0x0705,0x071e,0x073a,0x0750,0x0766,0x0779,0x0798,0x07ab,
0x07ca,0x07e3,0x07fc,0x0812,0x082e,0x0847,0x085a,0x0867,0x0880,0x0893,0x08b2,0x08c2,0x08e1,0x08eb,0x090a,0x091a,
};


PegFont  VectorFont = {2, 7, 2, 9, 0, 0, 127, 
(WORD *)  Vector_offset_table,
(PegFont *) Vector_width_table,
(UCHAR *) Vector_data_table};



/*--------------------------------------------------------------------------*/
// FontLine- Draws a 'line' directly into a PegFont bitmap.
/*--------------------------------------------------------------------------*/
void PegScreen::FontLine(UCHAR *pData, WORD wBitOffset,
    PegPoint Start, PegPoint End, WORD wBytesPerRow)
{

    UCHAR *pBase;
    UCHAR uMask;

    if (Start.y == End.y)
    {
        if (Start.x > End.x)
        {
            PEGSWAP(Start.x, End.x)
        }
        wBitOffset += Start.x;
        pBase = pData + (wBitOffset >> 3);
        pBase += Start.y * wBytesPerRow;
        uMask = 0x80 >> (wBitOffset & 7);
        while(Start.x <= End.x)
        {
            *pBase |= uMask;
            uMask >>= 1;
            if (!uMask)
            {
                uMask = 0x80;
                pBase++;
            }
            Start.x++;
        }
        return;
    }
    if (Start.x == End.x)
    {
        if (Start.y > End.y)
        {
            PEGSWAP(Start.y, End.y);
        }
        wBitOffset += Start.x;
        pBase = pData + (wBitOffset >> 3);
        pBase += Start.y * wBytesPerRow;
        uMask = 0x80 >> (wBitOffset & 7);
        while(Start.y <= End.y)
        {
            *pBase |= uMask;
            pBase += wBytesPerRow;
            Start.y++;
        }
        return;
    }

    SIGNED dx = abs(End.x - Start.x);
    SIGNED dy = abs(End.y - Start.y);

    if (((dx >= dy && (Start.x > End.x)) ||
        ((dy > dx) && Start.y > End.y)))
    {
        PEGSWAP(End.x, Start.x);
        PEGSWAP(End.y, Start.y);
    }

    SIGNED y_sign = ((int) End.y - (int) Start.y) / dy;
    SIGNED x_sign = ((int) End.x - (int) Start.x) / dx;
    SIGNED decision;
    
    SIGNED wCurx, wCury;

    wBitOffset += Start.x;
    pBase = pData + (wBitOffset >> 3);
    pBase += Start.y * wBytesPerRow;

    if (dx >= dy)
    {
        SIGNED iPointerAdd = wBytesPerRow * y_sign;
        uMask = 0x80 >> (wBitOffset & 7);

        for (wCurx = Start.x, decision = (dx >> 1);
             wCurx <= End.x; wCurx++, decision += dy)
        {
            if (decision >= dx)
            {
                decision -= dx;
                pBase += iPointerAdd;
            }
            *pBase |= uMask;
            uMask >>= 1;

            if (!uMask)
            {
                uMask = 0x80;
                pBase++;
            }
        }
    }
    else
    {
        uMask = 0x80 >> (wBitOffset & 7);
        for (wCury = Start.y, decision = (dy >> 1);
            wCury <= End.y; wCury++, decision += dx)
        {
            if (decision >= dy)
            {
                decision -= dy;
                if (x_sign > 0)
                {
                    uMask >>= 1;
                    if (!uMask)
                    {
                        pBase++;
                        uMask = 0x80;
                    }
                }
                else
                {
                    uMask <<= 1;
                    if (!uMask)
                    {
                        uMask = 0x01;
                        pBase--;
                    }
                }
            }
            *pBase |= uMask;
            pBase += wBytesPerRow;
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegFont *PegScreen::MakeFont(UCHAR uHeight, BOOL bBold,
    BOOL bItalic)
{
    PegFont *pFont = &VectorFont;
    PegPoint Current;
    PegPoint To;
    WORD InVal;
    Current.x = Current.y = 0;
    WORD wTotWidth = 0;
    WORD wBytesPerRow;
    LONG iVector;
    UCHAR *pWidths = (UCHAR *) pFont->pNext;

    PegFont *pNewFont = new PegFont;
    pNewFont->uType = 1;

    if (bBold)
    {
        uHeight++;
    }

    // fill in Ascent, Descent, Height, FirstChar, LastChar:

    iVector = pFont->uAscent * uHeight;
    iVector /= pFont->uHeight;
    pNewFont->uAscent = (UCHAR) iVector;
    iVector = pFont->uDescent * uHeight;
    iVector /= pFont->uHeight;
    pNewFont->uDescent = (UCHAR) iVector;
    pNewFont->uHeight = uHeight;
    pNewFont->wFirstChar = pFont->wFirstChar;
    pNewFont->wLastChar = pFont->wLastChar;

    // create the "bit offset" array:

    WORD *pOffsets = new WORD[pFont->wLastChar - pFont->wFirstChar + 1];
    pNewFont->pOffsets = pOffsets;
    pNewFont->pNext = NULL;

    WORD *pPutOffset = pOffsets;
    SIGNED iScale = uHeight  * 2 / pFont->uHeight / 3;

    if (bBold && bItalic)
    {
        iScale++;
    }

    for (InVal = pFont->wFirstChar; InVal <= pFont->wLastChar; InVal++)
    {
        *pPutOffset++ = wTotWidth;
        iVector = pWidths[InVal - pFont->wFirstChar];
        iVector *= uHeight;
        iVector /= pFont->uHeight;

        if (bItalic && iVector)
        {
            iVector += iScale;
        }
        wTotWidth += (SIGNED) iVector;

    }

    if (bBold && bItalic)
    {
        iScale--;
    }
    wTotWidth += 7;
    wBytesPerRow = wTotWidth >> 3;  // divide by 8 = byte width
    pNewFont->wBytesPerLine = wBytesPerRow;

    // create a memory block for character bitmap:

    UCHAR *pMem = new UCHAR[uHeight * wBytesPerRow];
    memset(pMem, 0, uHeight * wBytesPerRow);
    pNewFont->pData = pMem;
    wTotWidth = 0;
    Current.x = 0;
    Current.y = pNewFont->uAscent;

    for (InVal = pFont->wFirstChar; InVal <= pFont->wLastChar; InVal++)
    {
        if (!pWidths[InVal - pFont->wFirstChar])
        {
            continue;
        }
        signed char *pGet = (signed char *) pFont->pData;
        pGet += pFont->pOffsets[InVal - pFont->wFirstChar];

        UCHAR Opcode = (UCHAR) *pGet++;

        while(Opcode)
        {
            if (Opcode == 3)
            {
                iVector = (LONG) *pGet++ * uHeight;
                iVector /= pFont->uHeight;
                To.x = (SIGNED) iVector;
                iVector = *pGet++ * (SIGNED) uHeight;
                iVector /= (SIGNED) pFont->uHeight;

                if (bItalic)
                {
                    To.x += (iVector / 4) + iScale;
                }
                To.y = pNewFont->uAscent - (SIGNED) iVector;

                if (To.y < 0)
                {
                    To.y = 0;
                }
                if (To.y >= uHeight)
                {
                    To.y = uHeight - 1;
                }
                FontLine(pMem, wTotWidth, Current, To, wBytesPerRow);

                if (bBold)
                {
                    PegPoint p1, p2;
                    p1.x = Current.x;
                    p1.y = Current.y - 1;
                    p2.x = To.x;
                    p2.y = To.y - 1;

                    if (p1.y < 0)
                    {
                        p1.y = 0;
                    }
                    if (p2.y < 0)
                    {
                        p2.y = 0;
                    }

                    FontLine(pMem, wTotWidth + 1, Current, To, wBytesPerRow);
                    FontLine(pMem, wTotWidth, p1, p2, wBytesPerRow);
                }

                Current = To;
            }
            else
            {
                if (Opcode == 2)
                {
                    iVector = (LONG) *pGet++ * uHeight;
                    iVector /= pFont->uHeight;
                    Current.x = (SIGNED) iVector;
                    iVector = *pGet++ * (SIGNED) uHeight;
                    iVector /= (SIGNED) pFont->uHeight;
                    Current.y = pNewFont->uAscent - (SIGNED) iVector;

                    if (bItalic)
                    {
                        Current.x += iVector / 4 + iScale;
                    }

                    if (Current.y < 0)
                    {
                        Current.y = 0;
                    }
                    if (Current.y >= uHeight)
                    {
                        Current.y = uHeight - 1;
                    }
                }
            }
            Opcode = (UCHAR) *pGet++;
        }
        iVector = pWidths[InVal - pFont->wFirstChar];
        iVector *= uHeight;
        iVector /= pFont->uHeight;

        if (bItalic)
        {
            iVector += iScale;
            if (bBold)
            {
                iVector++;
            }
        }
        wTotWidth += (SIGNED) iVector;
    }
    return pNewFont;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegScreen::DeleteFont(PegFont *pFont)
{
    delete pFont->pData;
    delete pFont->pOffsets;
    delete pFont;
}

#endif  // PEG_VECTOR_FONTS if
