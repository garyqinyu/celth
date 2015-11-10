/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 7312scr8.cpp - Linear 8 bit-per-pixel screen driver. This driver can be
//   configured to work with any screen resolution, simply by passing in the
//   correct rectangle to the constructor. 
//
// Author: Kenneth G. Maxwell
//         Jim DeLisle
//
// Copyright (c) 1997-2001 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// All available configuration flags are found in the 7312scr8.hpp header file.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"

extern UCHAR DefPalette256[];

/*--------------------------------------------------------------------------*/
// Assign the video frame buffer and double buffer. This must be set up in
// 7312scr8.hpp
/*--------------------------------------------------------------------------*/
static UCHAR* gpFrameBuffer = (UCHAR*)VIDEO_BUFFER_START;
#if defined(RUNTIME_WIGGLER)
static UCHAR* gpDoubleBuffer = (UCHAR*)DOUBLE_BUFFER_START;
#else
static UCHAR gpDoubleBuffer[PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE * 3 / 2];
#endif

static DWORD gdwFrameRowLen = ((PEG_VIRTUAL_XSIZE * 3) >> 1);

/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new EP7312Screen8(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
EP7312Screen8::EP7312Screen8(PegRect &Rect) : PegScreen(Rect)
{
    mdNumColors = 256;

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();

    mpScanPointers = new UCHAR PEGFAR *[Rect.Height()];
    UCHAR PEGFAR *CurrentPtr = GetVideoAddress();

    for (SIGNED iLoop = 0; iLoop < Rect.Height(); iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += mwHRes;
    }

    mLastPointerPos.x = Rect.Width() / 2;
    mLastPointerPos.y = Rect.Height() / 2;
    mbPointerHidden = FALSE;
    mwDrawNesting = 0;

    ConfigureController();        // set up controller registers

    SetPalette(0, 232, DefPalette256);
}

/*--------------------------------------------------------------------------*/
// Since we allocated this ourselves in RAM, we can just return the address.
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *EP7312Screen8::GetVideoAddress(void)
{
   #if defined(RUNTIME_WIGGLER)
    return((UCHAR PEGFAR *)gpDoubleBuffer);
   #else
    return gpDoubleBuffer;
   #endif
}

/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
EP7312Screen8::~EP7312Screen8()
{
    delete [] mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::BeginDraw(PegThing *)
{
    LOCK_PEG
    if (!mwDrawNesting)
    {
       #ifdef PEG_MOUSE_SUPPORT
        if (miInvalidCount)
        {
            if (mInvalid.Overlap(mCapture.Pos()))
            {
                HidePointer();
                mbPointerHidden = TRUE;
            }
        }
       #endif
    }

    mwDrawNesting++;
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        mpScanPointers = new UCHAR PEGFAR *[pMap->wHeight];
        UCHAR PEGFAR *CurrentPtr = pMap->pStart;
        for (SIGNED iLoop = 0; iLoop < pMap->wHeight; iLoop++)
        {
            mpScanPointers[iLoop] = CurrentPtr;
            CurrentPtr += pMap->wWidth;
        }
        mVirtualRect.Set(0, 0, pMap->wWidth - 1, pMap->wHeight - 1);
        mbVirtualDraw = TRUE;
    }
}


/*--------------------------------------------------------------------------*/
void EP7312Screen8::EndDraw()
{
    mwDrawNesting--;

    if (!mwDrawNesting)
    {
       #ifdef PEG_MOUSE_SUPPORT
        if (mbPointerHidden)
        {
            SetPointer(mLastPointerPos);
            mbPointerHidden = FALSE;
        }
       #endif

        MemoryToScreen();

        while(miInvalidCount > 0)
        {
            miInvalidCount--;
            UNLOCK_PEG
        }
    }
    UNLOCK_PEG
}


/*--------------------------------------------------------------------------*/
void EP7312Screen8::EndDraw(PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        mbVirtualDraw = FALSE;
        delete [] mpScanPointers;
        mpScanPointers = mpSaveScanPointers;
        UNLOCK_PEG
    }
}



/*--------------------------------------------------------------------------*/
void EP7312Screen8::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
    SIGNED wYEnd, PegRect &Rect, PegColor Color, SIGNED wWidth)
{
    if (wYStart == wYEnd)
    {
        HorizontalLine(Rect.wLeft, Rect.wRight, Rect.wTop, Color.uForeground,
                       wWidth);
        return;
    }
    if (wXStart == wXEnd)
    {
        VerticalLine(Rect.wTop, Rect.wBottom, Rect.wLeft, Color.uForeground,
                     wWidth);
        return;
    }

    SIGNED dx = abs(wXEnd - wXStart);
    SIGNED dy = abs(wYEnd - wYStart);

    if (((dx >= dy && (wXStart > wXEnd)) ||
        ((dy > dx) && wYStart > wYEnd)))
    {
        PEGSWAP(wXEnd, wXStart);
        PEGSWAP(wYEnd, wYStart);
    }

    SIGNED y_sign = ((int) wYEnd - (int) wYStart) / dy;
    SIGNED x_sign = ((int) wXEnd - (int) wXStart) / dx;
    SIGNED decision;

    SIGNED wCurx, wCury, wNextx, wNexty, wpy, wpx;

    if (Rect.Contains(wXStart, wYStart) &&
        Rect.Contains(wXEnd, wYEnd))
    {
	    if (dx >= dy)
	    {
	        for (wCurx = wXStart, wCury = wYStart, wNextx = wXEnd,
	             wNexty = wYEnd, decision = (dx >> 1);
	             wCurx <= wNextx; wCurx++, wNextx--, decision += dy)
	        {
	            if (decision >= dx)
	            {
	                decision -= dx;
	                wCury += y_sign;
	                wNexty -= y_sign;
	            }
	            for (wpy = wCury - wWidth / 2;
	                 wpy <= wCury + wWidth / 2; wpy++)
	            {
                    PlotPointView(wCurx, wpy, Color.uForeground);
	            }
	
	            for (wpy = wNexty - wWidth / 2;
	                 wpy <= wNexty + wWidth / 2; wpy++)
	            {
                    PlotPointView(wNextx, wpy, Color.uForeground);
	            }
	        }
	    }
	    else
	    {
	        for (wCurx = wXStart, wCury = wYStart, wNextx = wXEnd,
	                wNexty = wYEnd, decision = (dy >> 1);
	            wCury <= wNexty; wCury++, wNexty--, decision += dx)
	        {
	            if (decision >= dy)
	            {
	                decision -= dy;
	                wCurx += x_sign;
	                wNextx -= x_sign;
	            }
	            for (wpx = wCurx - wWidth / 2;
	                 wpx <= wCurx + wWidth / 2; wpx++)
	            {
                    PlotPointView(wpx, wCury, Color.uForeground);
	            }
	
	            for (wpx = wNextx - wWidth / 2;
	                 wpx <= wNextx + wWidth / 2; wpx++)
	            {
                    PlotPointView(wpx, wNexty, Color.uForeground);
	            }
	        }
	    }
    }
    else
    {
	    if (dx >= dy)
	    {
	        for (wCurx = wXStart, wCury = wYStart, wNextx = wXEnd,
	             wNexty = wYEnd, decision = (dx >> 1);
	             wCurx <= wNextx; wCurx++, wNextx--, decision += dy)
	        {
	            if (decision >= dx)
	            {
	                decision -= dx;
	                wCury += y_sign;
	                wNexty -= y_sign;
	            }
	            for (wpy = wCury - wWidth / 2;
	                 wpy <= wCury + wWidth / 2; wpy++)
	            {
	                if (wCurx >= Rect.wLeft &&
	                    wCurx <= Rect.wRight &&
	                    wpy >= Rect.wTop &&
	                    wpy <= Rect.wBottom)
	                {
	                    PlotPointView(wCurx, wpy, Color.uForeground);
	                }
	            }
	
	            for (wpy = wNexty - wWidth / 2;
	                 wpy <= wNexty + wWidth / 2; wpy++)
	            {
	                if (wNextx >= Rect.wLeft &&
	                    wNextx <= Rect.wRight &&
	                    wpy >= Rect.wTop &&
	                    wpy <= Rect.wBottom)
	                {
	                    PlotPointView(wNextx, wpy, Color.uForeground);
	                }
	            }
	        }
	    }
	    else
	    {
	        for (wCurx = wXStart, wCury = wYStart, wNextx = wXEnd,
	                wNexty = wYEnd, decision = (dy >> 1);
	            wCury <= wNexty; wCury++, wNexty--, decision += dx)
	        {
	            if (decision >= dy)
	            {
	                decision -= dy;
	                wCurx += x_sign;
	                wNextx -= x_sign;
	            }
	            for (wpx = wCurx - wWidth / 2;
	                 wpx <= wCurx + wWidth / 2; wpx++)
	            {
	                if (wpx >= Rect.wLeft &&
	                    wpx <= Rect.wRight &&
	                    wCury >= Rect.wTop &&
	                    wCury <= Rect.wBottom)
	                {
	                    PlotPointView(wpx, wCury, Color.uForeground);
	                }
	            }
	
	            for (wpx = wNextx - wWidth / 2;
	                 wpx <= wNextx + wWidth / 2; wpx++)
	            {
	                if (wpx >= Rect.wLeft &&
	                    wpx <= Rect.wRight &&
	                    wNexty >= Rect.wTop &&
	                    wNexty <= Rect.wBottom)
	                {
	                    PlotPointView(wpx, wNexty, Color.uForeground);
	                }
	            }
	        }
	    }
    }
}


/*--------------------------------------------------------------------------*/
void EP7312Screen8::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    SIGNED iLen = wXEnd - wXStart + 1;

    if (!iLen)
    {
        return;
    }
    while(wWidth-- > 0)
    {
        UCHAR *Put = mpScanPointers[wYPos] + wXStart;
        memset(Put, Color, iLen);
        wYPos++;
    }
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    while(wYStart <= wYEnd)
    {
        SIGNED lWidth = wWidth;
        UCHAR *Put = mpScanPointers[wYStart] + wXPos;

        while (lWidth-- > 0)
        {
            *Put++ = (UCHAR) Color;
        }
        wYStart++;
    }
}


/*--------------------------------------------------------------------------*/
void EP7312Screen8::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, 
        SIGNED wYPos)
{
    UCHAR *Put = mpScanPointers[wYPos] + wXStart;

    while (wXStart <= wXEnd)
    {
        *Put ^= 0x0f;
        Put += 2;
        wXStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    UCHAR uVal;

    while (wYStart <= wYEnd)
    {
        UCHAR *Put = mpScanPointers[wYStart] + wXPos;
        uVal = *Put;
        uVal ^= 0xf;
        *Put = uVal;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::Capture(PegCapture *Info, PegRect &CaptureRect)
{
    PegBitmap *pMap = Info->Bitmap();

    if (CaptureRect.wLeft < 0)
    {
        CaptureRect.wLeft = 0;
    }

    if (CaptureRect.wTop < 0)
    {
        CaptureRect.wTop = 0;
    }

    Info->SetPos(CaptureRect);
    LONG Size = pMap->wWidth * pMap->wHeight;
    Info->Realloc(Size);

    UCHAR *GetStart = mpScanPointers[CaptureRect.wTop] + CaptureRect.wLeft;

    // make room for the memory bitmap:

    pMap->uFlags = 0;        // raw format
    pMap->uBitsPix = 8;      // 8 bits per pixel

    // fill in the image with our captured info:

    UCHAR *Put = pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        UCHAR *Get = GetStart;
        for (WORD wLoop = 0; wLoop < pMap->wWidth; wLoop++)
        {
            *Put++ = *Get++;
        }
        GetStart += mwHRes;
    }
    Info->SetValid(TRUE);
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
    }
    else
    {
        UCHAR *Get = Getmap->pStart;
        Get += (View.wTop - Where.y) * Getmap->wWidth;
        Get += View.wLeft - Where.x;

        if (HAS_TRANS(Getmap))
        {
	        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
	        {
	            UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
	            for(SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
	            {
	                UCHAR uVal = *Get++;
	
	                if (uVal != Getmap->dTransColor)
	                {
	                    *Put = uVal;
	                }
	                Put++;
	            }
	            Get += Getmap->wWidth - View.Width();
	        }
        }
        else
        {
	        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
	        {
	            UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
                memcpy(Put, Get, View.Width());
	            Get += Getmap->wWidth;
	        }
        }
    }
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::DrawRleBitmap(const PegPoint Where, const PegRect View,
    const PegBitmap *Getmap)
{
    UCHAR *Get = Getmap->pStart;
    UCHAR uVal;
    SIGNED uCount;

    SIGNED wLine = Where.y;

    uCount = 0;

    while (wLine < View.wTop)
    {
        uCount = 0;

        while(uCount < Getmap->wWidth)
        {
            uVal = *Get++;
            if (uVal & 0x80)
            {
                uVal = (uVal & 0x7f) + 1;
                uCount += uVal;
                Get += uVal;
            }
            else
            {
                Get++;
                uCount += uVal + 1;
            }
        }
        wLine++;
    }

    if (HAS_TRANS(Getmap))
    {
	    while (wLine <= View.wBottom)
	    {
	        UCHAR *Put = mpScanPointers[wLine] + Where.x;
	        SIGNED wLoop1 = Where.x;
	
	        while (wLoop1 < Where.x + Getmap->wWidth)
	        {
	            uVal = *Get++;
	
	            if (uVal & 0x80)        // raw packet?
	            {
	                uCount = (uVal & 0x7f) + 1;
	                
	                while (uCount--)
	                {
	                    uVal = *Get++;
	                    if (wLoop1 >= View.wLeft &&
	                        wLoop1 <= View.wRight &&
	                        uVal != Getmap->dTransColor)
	                    {
	                        *Put = uVal;
	                    }
	                    Put++;
	                    wLoop1++;
	                }
	            }
	            else
	            {
	                uCount = uVal + 1;
	                uVal = *Get++;
	
	                if (uVal == Getmap->dTransColor)
	                {
	                    wLoop1 += uCount;
	                    Put += uCount;
	                }
	                else
	                {
	                    while(uCount--)
	                    {
	                        if (wLoop1 >= View.wLeft &&
	                            wLoop1 <= View.wRight)
	                        {
	                            *Put++ = uVal;
	                        }
	                        else
	                        {
	                            Put++;
	                        }
	                        wLoop1++;
	                    }
	                }
	            }
	        }
	        wLine++;
	    }
    }
    else
    {
	    while (wLine <= View.wBottom)
	    {
	        UCHAR *Put = mpScanPointers[wLine] + Where.x;
	        SIGNED wLoop1 = Where.x;
	
	        while (wLoop1 < Where.x + Getmap->wWidth)
	        {
	            uVal = *Get++;
	
	            if (uVal & 0x80)        // raw packet?
	            {
	                uCount = (uVal & 0x7f) + 1;
	                
	                while (uCount--)
	                {
	                    uVal = *Get++;
	                    if (wLoop1 >= View.wLeft &&
	                        wLoop1 <= View.wRight &&
                            uVal != Getmap->dTransColor)
	                    {
	                        *Put = uVal;
	                    }
	                    Put++;
	                    wLoop1++;
	                }
	            }
	            else
	            {
	                uCount = uVal + 1;
	                uVal = *Get++;
	
                    while(uCount--)
	                {
	                    if (wLoop1 >= View.wLeft &&
	                        wLoop1 <= View.wRight)
	                    {
	                        *Put++ = uVal;
	                    }
	                    else
	                    {
	                        Put++;
	                    }
	                    wLoop1++;
	                }
	            }
	        }
	        wLine++;
	    }
    }
}

/*--------------------------------------------------------------------------*/
// software emulated RectMove using Capture and Bitmap
/*--------------------------------------------------------------------------*/
void EP7312Screen8::RectMoveView(PegThing *Caller, const PegRect &View,
     const SIGNED xMove, const SIGNED yMove)
{
    PegCapture BlockCapture;
    PegRect CaptureRect = View;
    Capture(&BlockCapture, CaptureRect);
    BlockCapture.Shift(xMove, yMove);
    Bitmap(Caller, BlockCapture.Point(), BlockCapture.Bitmap(), TRUE);
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
    PegFont *Font, SIGNED iCount, PegRect &Rect)
{
   #ifdef PEG_UNICODE
    PEGCHAR PEGFAR *pCurrentChar = (PEGCHAR PEGFAR *) Text;
    PegFont *pFontStart = Font;
   #else
    UCHAR PEGFAR *pCurrentChar = (UCHAR PEGFAR *) Text;
   #endif

    UCHAR PEGFAR *pGetData;
    UCHAR PEGFAR *pGetDataBase;
    WORD  wIndex;
    WORD  wBitOffset;
    SIGNED  wXpos = Where.x;
    WORD cVal = *pCurrentChar++;
    SIGNED iCharWidth;

   #ifdef DO_OUTLINE_TEXT
    if (IS_OUTLINE(Font))
    {
        DrawOutlineText(Where, Text, Color, Font, iCount, Rect);
        return;
    }
   #endif

    while(cVal && wXpos <= Rect.wRight)
    {
        if (iCount == 0)
        {
            return;
        }
        iCount--;

       #ifdef PEG_UNICODE
        Font = pFontStart;

        while(Font)
        {
            if (cVal >= Font->wFirstChar &&
                cVal <= Font->wLastChar)
            {
                break;
            }
            Font = Font->pNext;
        }
        if (!Font)                 // this font doesn't contain this glyph?
        {
            cVal = *pCurrentChar++; // just skip to next char
            continue;               
        }

        wIndex = cVal - (WORD) Font->wFirstChar;

        if (IS_VARWIDTH(Font))
        {
            wBitOffset = Font->pOffsets[wIndex];
            iCharWidth = Font->pOffsets[wIndex+1] - wBitOffset;
        }
        else
        {
            iCharWidth = (WORD) Font->pOffsets;
            wBitOffset = iCharWidth * wIndex;
        }

       #else

        wIndex = cVal - (WORD) Font->wFirstChar;
        wBitOffset = Font->pOffsets[wIndex];
        iCharWidth = Font->pOffsets[wIndex+1] - wBitOffset;

       #endif

        if (wXpos + iCharWidth > Rect.wRight)
        {
            iCharWidth = Rect.wRight - wXpos + 1;
        }

        WORD ByteOffset = wBitOffset / 8;
        pGetDataBase = Font->pData + ByteOffset;
        pGetDataBase += (Rect.wTop - Where.y) * Font->wBytesPerLine;

        for (SIGNED ScanRow = Rect.wTop; ScanRow <= Rect.wBottom; ScanRow++)
        {
            pGetData = pGetDataBase;
            UCHAR InMask = 0x80 >> (wBitOffset & 7);
            WORD wBitsOutput = 0;
            UCHAR cData;

           #ifdef PEG_UNICODE
            if (ScanRow - Where.y < Font->uHeight)
            {
                cData = *pGetData++;
            }
            else
            {
                cData = 0;
            }
           #else
            cData = *pGetData++;
           #endif

            UCHAR *Put = mpScanPointers[ScanRow] + wXpos;

            while(wBitsOutput < iCharWidth)
            {
                if (!InMask)
                {
                    InMask = 0x80;
                    // read a byte:

                   #ifdef PEG_UNICODE
                    if (ScanRow - Where.y < Font->uHeight)
                    {
                        cData = *pGetData++;
                    }
                    else
                    {
                        cData = 0;
                    }
                   #else
                    cData = *pGetData++;
                   #endif
                }

                if (wXpos >= Rect.wLeft)
                {
                    if (cData & InMask)        // is this bit a 1?
                    {
                        *Put = (UCHAR) Color.uForeground;
                    }
                    else
                    {
                        if (Color.uFlags & CF_FILL)
                        {
                            *Put = (UCHAR) Color.uBackground;
                        }
                    }
                }
                InMask >>= 1;
                wXpos++;
                Put++;
                wBitsOutput++;
                if (wXpos > Rect.wRight)
                {
                    break;
                }
            }
            pGetDataBase += Font->wBytesPerLine;
            wXpos -= iCharWidth;
        }
        wXpos += iCharWidth;
        cVal = *pCurrentChar++;
    }
}


#ifdef DO_OUTLINE_TEXT

/*--------------------------------------------------------------------------*/
// Outline fonts are encoded as 2 bpp, with the following meanings:
//
// val = 0 = transparent color
// val = 1 = foreground color
// val = 2 = PCLR_OUTLINE
// val = 3 = undefined
/*--------------------------------------------------------------------------*/
#define PCLR_OUTLINE BLACK

void EP7312Screen8::DrawOutlineText(PegPoint Where, const PEGCHAR *Text,
        PegColor &Color, PegFont *Font, SIGNED iCount, PegRect &Rect)
{
    UCHAR PEGFAR *pCurrentChar = (UCHAR PEGFAR *) Text;
    UCHAR PEGFAR *pGetData;
    UCHAR PEGFAR *pGetDataBase;
    WORD  wBitOffset;
    SIGNED  wXpos = Where.x;
    UCHAR cVal = *pCurrentChar++;

    while(cVal && wXpos <= Rect.wRight)
    {
        if (iCount == 0)
        {
            return;
        }
        iCount--;
        UCHAR cOffset = cVal - (UCHAR) Font->wFirstChar;
        wBitOffset = Font->pOffsets[cOffset];
        SIGNED  wCharWidth = Font->pOffsets[cOffset+1] - wBitOffset;

        if (wXpos + wCharWidth > Rect.wRight)
        {
            wCharWidth = Rect.wRight - wXpos + 1;
        }

        WORD ByteOffset = Font->pOffsets[cOffset] / 4;  // div by 8, * 2
        pGetDataBase = Font->pData + ByteOffset;
        pGetDataBase += (Rect.wTop - Where.y) * Font->wBytesPerLine;

        for (SIGNED ScanRow = Rect.wTop; ScanRow <= Rect.wBottom; ScanRow++)
        {
            pGetData = pGetDataBase;
            char cValShift = 6;
            cValShift -= (wBitOffset & 3) * 2;
            WORD wBitsOutput = 0;
            UCHAR cData = *pGetData++;
            UCHAR *Put = mpScanPointers[ScanRow] + wXpos;

            while(wBitsOutput < wCharWidth)
            {
                if (cValShift < 0)
                {
                    cValShift = 6;
                    // read a byte:
                    cData = *pGetData++;
                }

                if (wXpos >= Rect.wLeft)
                {
                    switch((cData >> cValShift) & 0x03)
                    {
                    case 0:
                    case 3:
                        break;

                    case 1:
                        *Put = (UCHAR) Color.uForeground;
                        break;
                    
                    case 2:
                        *Put = (UCHAR) PCLR_OUTLINE;
                        break;
                    }
                }
                cValShift -= 2;
                wXpos++;
                Put++;
                wBitsOutput++;
                if (wXpos > Rect.wRight)
                {
                    break;
                }
            }
            pGetDataBase += Font->wBytesPerLine;
            wXpos -= wCharWidth;
        }
        wXpos += wCharWidth;
        cVal = *pCurrentChar++;
    }
}

#endif // Outline text style is supported

/*--------------------------------------------------------------------------*/
void EP7312Screen8::HidePointer(void)
{
   #ifdef PEG_MOUSE_SUPPORT
    PegThing *pt = NULL;
    PegPresentationManager *pp = pt->Presentation();
    Restore(pp, &mCapture, TRUE);
    mCapture.SetValid(FALSE);
   #endif
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::SetPointer(PegPoint Where)
{
   #ifdef PEG_MOUSE_SUPPORT
    LOCK_PEG
    HidePointer();
    mLastPointerPos = Where;

    PegThing *pt = NULL;
    PegPresentationManager *pp = pt->Presentation();
    Where.x -= miCurXOffset;
    Where.y -= miCurYOffset;

    PegRect MouseRect;
    MouseRect.wLeft = Where.x;
    MouseRect.wTop =  Where.y;
    MouseRect.wBottom = MouseRect.wTop + mpCurPointer->wHeight - 1;
    MouseRect.wRight = MouseRect.wLeft + mpCurPointer->wWidth - 1;
    Capture(&mCapture, MouseRect);
        
    Bitmap(pp, Where, mpCurPointer, TRUE);
    UNLOCK_PEG
   #endif
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::SetPointerType(UCHAR bType)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (bType < NUM_POINTER_TYPES)
    {
        LOCK_PEG
        HidePointer();
        mpCurPointer = mpPointers[bType].Bitmap;
        miCurXOffset = mpPointers[bType].xOffset;
        miCurYOffset = mpPointers[bType].yOffset;
        SetPointer(mLastPointerPos);
        UNLOCK_PEG
    }
   #endif
}

/*--------------------------------------------------------------------------*/
void EP7312Screen8::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
UCHAR *EP7312Screen8::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 232;
    return muPalette;
}

/*--------------------------------------------------------------------------*/
// There are 16 nibbles in 2 32 bit registers. We're just going to program
// them to 0 - 15, low to high.
//
// The palette registers are actually backward from the documentation.
// (Both Cirrus Logic's and Cogent's.) The LSW of the palette is at memory
// address MSW, and vice-versa. So, we'll put the 0-7 in the MSW and the
// 8-15 in the LSW.
/*--------------------------------------------------------------------------*/
void EP7312Screen8::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
    DWORD dwRegVal = 0x00l;
    DWORD* pReg = (DWORD*)LCD_PALMSW;

    // MSW, 0 through 7
    dwRegVal = (DWORD)
               ((0x07 << 28) +
                (0x06 << 24) +
                (0x05 << 20) +
                (0x04 << 16) +
                (0x03 << 12) +
                (0x02 << 8)  +
                (0x01 << 4)  +
                (0x00));

    *pReg = dwRegVal;

    // LSW, 8 through 15
    dwRegVal = (DWORD)
               ((0x0F << 28) +
                (0x0E << 24) +
                (0x0D << 20) +
                (0x0C << 16) +
                (0x0B << 12) +
                (0x0A << 8)  +
                (0x09 << 4)  +
                (0x08));

    pReg = (DWORD*)LCD_PALLSW;
    *pReg = dwRegVal;
}

/*--------------------------------------------------------------------------*/
// Based on the way the controller is set up, we have to 'fool' the thing
// into thinking that there are 3 times as many x pixels as what we really
// have, since the panel wants 12bpp and the controller thinks it's running
// in 4bpp. It seems odd, but that's why we're multiplying PEG_VIRTUAL_XSIZE
// by 3 everywhere.
/*--------------------------------------------------------------------------*/
void EP7312Screen8::ConfigureController(void)
{
    // Turn the LCD controller off
    DWORD dwSysConVal = *((DWORD*)LCD_SYSCON1);
    dwSysConVal &= 0xFFFFEFFF;
    *((DWORD*)LCD_SYSCON1) = dwSysConVal;

    DWORD dwRegVal = 0x00l;

    // Set the controller in 4bpp gray scale mode
    dwRegVal = (DWORD)(LCD_GSMD2 << 31);
    dwRegVal |= (LCD_GSMD1 << 30);

    // Set the AC Prescale value. (We're trying 13)
    //dwRegVal |= 0x1A000000;//(0x0D << 25);

    // Set the AC Prescale value (We're trying 19)
    dwRegVal |= (0x13 << 25);

    // Set the Pixel Prescale value
    // We're asking for a 70Hz refresh rate. It's perfectly acceptable to
    // drop the remainder from the division operation.
    //DWORD dwPrescale = (LCD_CLOCKRATE / 
    //                   (70 * (PEG_VIRTUAL_XSIZE * 3) * PEG_VIRTUAL_YSIZE));
    // Any value greater than 1 makes the screen *REALLY* flickery,
    // so we'll just hard code it here since we know our screen size
    // and such won't change on this development board.
    DWORD dwPrescale = 0x01l;

    // We only want 6 bits of the value
    dwPrescale &= (0x3F);
    dwRegVal |= (dwPrescale << 19);

    // Line Length, in WORDs
    DWORD dwLineLength = ((PEG_VIRTUAL_XSIZE * 3) / 16) - 1;
    dwLineLength &= (0x3F);
    dwRegVal |= (dwLineLength << 13);

    // Video buffer size, in quad WORDs
    DWORD dwVidSize = (((PEG_VIRTUAL_XSIZE * 3) * PEG_VIRTUAL_YSIZE * 4)
                      / 128) - 1;

    dwVidSize &= (0x1FFF);
    dwRegVal |= dwVidSize;

    // Write the register
    *((DWORD*)LCD_LCDCON) = dwRegVal;

    // Set the address of the frame buffer
    *((UCHAR*)LCD_FBADDR) = 0x0C;

    // NOTE: The following (configuring the keys) need to be moved to
    // a setup keyboard function.
    // Drive the Keyboard scan registers to column 5 to enable the
    // 6 user keys
    dwSysConVal &= 0xFFFFFFF8;
    dwSysConVal |= 0x0000000D;

    // Turn the LCD controller back on
    dwSysConVal |= 0x00001000;
    *((DWORD*)LCD_SYSCON1) = dwSysConVal;
    
    // Invalidate the entire double buffer and transfer it over to the
    // frame buffer
    mInvalid.Set(0, 0, mwHRes - 1, mwVRes - 1);
    miInvalidCount = 1;
    MemoryToScreen();
    miInvalidCount = 0;
}
 
/*--------------------------------------------------------------------------*/
// Here's where all the real work happens. The controller thinks that we're
// running in 4bpp gray scale, the PegScreen is running in 8bpp packed pixel
// mode (3:3:2), and the Optrex panel wants everything in 12bpp. Hence,
// we're using a double buffer (the PegScreen is using it just like an 8bpp
// frame buffer), and at this point, we have to convert the 8bpp into 12bpp
// that looks like 4bpp.
//
// That all being clear as mud, what we're doing here is checking for even
// byte alignment in the image that we're moving from the double buffer
// into the frame buffer. If the start or end is on an odd byte, then
// we have to handle that pixel a little differently. The center portion of
// the scan line assumes even byte alignment.
/*--------------------------------------------------------------------------*/
void EP7312Screen8::MemoryToScreen(void)
{
    if (!miInvalidCount)
    {
        return;
    }

    PegRect Copy;
    Copy.Set(0, 0, mwHRes - 1, mwVRes - 1);
    Copy &= mInvalid;

    SIGNED iWidth = Copy.Width();
    if (iWidth <= 0)
    {
        return;
    }

    SIGNED iHeight = Copy.Height();
    if (iHeight <= 0)
    {
        return;
    }

    // It is important to begin the scan line at an even
    // pixel boundary. This saves a ton of work as well as
    // the necessity to read out the first pixel of each
    // line, & it then | it then write it back.
    if(Copy.wLeft & 0x01)
    {
        Copy.wLeft--;
        iWidth++;
    }

    // The same holds true for the right edge of the scan
    // line. We always want to run in multiples of 2 so that
    // we're writing 3 bytes to video memory each time 
    // through the draw loop. This simplifies everything.
    if(iWidth & 0x01)
    {
        Copy.wRight++;
        iWidth++;
    }

    UCHAR* pGet = mpScanPointers[Copy.wTop] + Copy.wLeft;
    UCHAR* pPut = gpFrameBuffer + (Copy.wTop * gdwFrameRowLen) +
                  ((Copy.wLeft * 3) >> 1);
    UCHAR ucByte1, ucByte2, ucByte3;
    UCHAR ucR, ucG, ucB;
    SIGNED i, j;

    // We know that iWidth will always be an even number greater
    // than 2. Therefore, we can safely lop off 2 pixels, or
    // 3 bytes, at a time.
    for(i = 0; i < iHeight; i++)
    {
        for(j = 0; j < iWidth; j += 2)
        {
            // The sequence is non-contiguous nibbles (3 nibbles
            // per pixel) that repeats itself every 3 bytes.
            // Like so:
            //
            // g0 r0 r1 b0 b1 g1
            //

            ucR = ucG = ucB = *pGet++;
            ucR &= 0xE0;
            /*if(ucR & 0x20)
            {
                ucR |= 0x10;
            }
            */
            ucR >>= 4;

            ucG &= 0x1C;
            /*if(ucG & 0x08)
            {
                ucG |= 0x04;
            }
            */
            ucG <<= 3;

            ucByte1 = ucG + ucR;

            ucB &= 0x03;
            ucB <<= 2;
            //ucB |= 0x03;

            //ucByte2 = (ucB + (ucB << 2));
            ucByte2 = ucB;

            ucR = ucG = ucB = *pGet++;

            ucR &= 0xE0;
            /*if(ucR & 0x20)
            {
                ucR |= 0x10;
            }
            */
            ucByte2 += ucR;

            ucG &= 0x1C;
            ucG >>= 1;
            /*if(ucG & 0x02)
            {
                ucG |= 0x01;
            }
            */

            ucByte3 = ucG;

            ucB &= 0x03;
            ucB <<= 6;
            ucB |= 0x30;

            ucByte3 += ucB;

            *pPut++ = ucByte1;
            *pPut++ = ucByte2;
            *pPut++ = ucByte3;
        }

        pGet = mpScanPointers[Copy.wTop + i + 1] + Copy.wLeft;
        pPut = gpFrameBuffer + ((Copy.wTop + i + 1) * gdwFrameRowLen) +
                  ((Copy.wLeft * 3) >> 1);
    }
}


