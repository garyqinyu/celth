/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ct545_8.cpp - Linear 256 color screen driver for C&T65545 on ISA bus.
//
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
// This driver is intended for systems which have direct, linear (i.e. not
// paged) access to the video frame buffer. This driver supports hardware cursor
// and double-buffering using the controller bit-blit feature.
// 
// The driver supports two different resolutions. These resolutions are:
//
// MODE320:  Configure for 320x240 full color LCD.
// MODE640:  Configure for 640x480 full color LCD.
// MODE800:  Configure for 800x600 full color LCD.
//
//
// All available configuration flags are found in the ct545_8.hpp header file.
//
//
// Known Limitations:
// --------------------------------------------------------------------------------------------
// 6/22/99  Modifications by Alberto Pasquale, all changes marked with [ap]
//
// Revisions to the CT545 initialization 
// 1 reg 1Bh changed from 30h to 2Fh Alternate Horizontal Total
// 
// Revisions to run on PharLap TNT
// 1	Map physical address to 13C00000h  
// 2    Modify BitBlit to use physical address 
// 3    Modify Cursor Drawing Routine to use PharLap linear address    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#define __COLORS        // don't want conio colors
#include <conio.h>

#include "peg.hpp"

extern UCHAR DefPalette256[];

#if defined(PEGTNT)
#include "pharlap.h"
#endif

/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new CTLinear8Screen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
CTLinear8Screen::CTLinear8Screen(PegRect &Rect) : PegScreen(Rect)
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
// *** This function must be filled in by the developer ***
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *CTLinear8Screen::GetVideoAddress(void)
{
    UCHAR PEGFAR *pVidMem;

   #ifdef PHARLAP
    #ifndef PEGTNT      // running with ETS?
     UCHAR *pMem = (UCHAR PEGFAR *) VID_MEM_BASE;

     // if double-buffering, drawing into a virtual buffer that is offset
     // 128K into video memory:
    
     #ifdef DOUBLE_BUFFER
      #if defined(LINEAR_ADDRESSING)
      pMem += (128 * 1024);  // offset 128K for second memory buffer
      #else

      // if we are running with a VGA memory window, we will draw
      // everything to local memory, and then use the bit-blit
      // engine to transfer into video memory:

      pMem = new UCHAR[mwHRes * mwVRes];
      #endif
     #endif
     return pMem;

    #else               // running with TNT?

    pVidMem = (UCHAR *) VirtualAlloc(NULL, VID_MEM_SIZE, MEM_RESERVE,
      PAGE_NOACCESS);
    _dx_map_pgsn((LPVOID) pVidMem, VID_MEM_SIZE, VID_MEM_BASE);

     // if double-buffering, drawing into a virtual buffer that is offset
     // 128K into video memory:

     #ifdef DOUBLE_BUFFER
      pVidMem += (128 * 1024);
     #endif

     return pVidMem;

    #endif

   #elif defined(PEGSMX)

     pVidMem = (UCHAR *) VID_MEM_BASE;

     #ifdef DOUBLE_BUFFER
      pVidMem += (128 * 1024);  // offset 128K for second memory buffer
     #endif

     return pVidMem;

   #else
     // user must define VID_MEM_BASE to point to the video buffer.
     pVidMem = (UCHAR *) VID_MEM_BASE;

     #ifdef DOUBLE_BUFFER
      pVidMem += (128 * 1024);  // offset 128K for second memory buffer
     #endif

      return(pVidMem);

   #endif
}

/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
CTLinear8Screen::~CTLinear8Screen()
{
    // delete [] mpScanPointers[0];
    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void CTLinear8Screen::BeginDraw(PegThing *)
{
    LOCK_PEG

    if (!mwDrawNesting)
    {
       #ifdef DOUBLE_BUFFER

        // make sure the BitBlit engine is not busy:

        WORD wStat = PINW(BB_STAT_REG);
        while(wStat & 0x10)
        {
            wStat = PINW(BB_STAT_REG);
        }

       #endif

       #if defined(PEG_MOUSE_SUPPORT) && !defined(HARDWARE_CURSOR)
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
void CTLinear8Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
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
void CTLinear8Screen::EndDraw()
{
    mwDrawNesting--;

    if (!mwDrawNesting)
    {
       #if defined(PEG_MOUSE_SUPPORT) && !defined(HARDWARE_CURSOR)
        if (mbPointerHidden)
        {
            SetPointer(mLastPointerPos);
            mbPointerHidden = FALSE;
        }
       #endif

       #ifdef DOUBLE_BUFFER
        MemoryToScreen();
       #endif

        while(miInvalidCount > 0)
        {
            miInvalidCount--;
            UNLOCK_PEG
        }
    }
    UNLOCK_PEG
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::EndDraw(PegBitmap *pMap)
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
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
    SIGNED wYEnd, PegRect &Rect, PegColor Color, SIGNED wWidth)
{
    if (wYStart == wYEnd)
    {
        HorizontalLine(Rect.wLeft, Rect.wRight, Rect.wTop, Color.uForeground, wWidth);
        return;
    }
    if (wXStart == wXEnd)
    {
        VerticalLine(Rect.wTop, Rect.wBottom, Rect.wLeft, Color.uForeground, wWidth);
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
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
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
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
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
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
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
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
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
void CTLinear8Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
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
	            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
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
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::RectMoveView(PegThing *Caller, const PegRect &View,
     const SIGNED xMove, const SIGNED yMove)
{
    // software emulated RectMove using Capture and Bitmap

    PegCapture BlockCapture;
    PegRect CaptureRect = View;
    Capture(&BlockCapture, CaptureRect);
    BlockCapture.Shift(xMove, yMove);
    Bitmap(Caller, BlockCapture.Point(), BlockCapture.Bitmap(), TRUE);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
            iCharWidth = (SIGNED) Font->pOffsets;
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

void CTLinear8Screen::DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
    PegFont *Font, SIGNED iCount, PegRect &Rect)
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
void CTLinear8Screen::ShowPointer(BOOL bShow)
{
    #ifdef HARDWARE_CURSOR
    if (bShow)
    {
        POUTW(HC_ENABLE, 0x0021);
    }
    else
    {
        POUTW(HC_ENABLE, 0x0000);
    }
    #endif

    PegScreen::ShowPointer(bShow);

}


/*--------------------------------------------------------------------------*/
void CTLinear8Screen::HidePointer(void)
{
   #if defined(PEG_MOUSE_SUPPORT) && !defined(HARDWARE_CURSOR)
    if (mbShowPointer)
    {
        PegThing *pt = NULL;
        PegPresentationManager *pp = pt->Presentation();
        Restore(pp, &mCapture, TRUE);
        mCapture.SetValid(FALSE);
    }
   #endif
}

int iBads;

/*--------------------------------------------------------------------------*/
void CTLinear8Screen::SetPointer(PegPoint Where)
{
   #if defined(PEG_MOUSE_SUPPORT)
    if (mbShowPointer)
    {
       #if !defined(HARDWARE_CURSOR)
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
	   #else
	    // here for a hardware cursor:
	    mLastPointerPos = Where;
	    Where.x -= miCurXOffset;
	    Where.y -= miCurYOffset;
	
	    POUTW(HC_YOFFSET, Where.y);
	    POUTW(HC_XOFFSET, Where.x);
	   #endif
    }
   #endif
}


/*--------------------------------------------------------------------------*/
void CTLinear8Screen::SetPointerType(UCHAR bType)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (bType < NUM_POINTER_TYPES)
    {
       #if !defined(HARDWARE_CURSOR)
        LOCK_PEG
        HidePointer();
       #endif

        mpCurPointer = mpPointers[bType].Bitmap;

       #if defined(HARDWARE_CURSOR)
        // reset the pointer register:
        DWORD dAddress = bType;
        dAddress <<= 10;    // * 1024, each bitmap consumes 1024 bytes
        dAddress += CURSOR_MEM_BASE;

        POUTW(HC_ADDR_HI, (WORD) (dAddress >> 16));
        POUTW(HC_ADDR_LO, (WORD) dAddress);
       #endif
        
        miCurXOffset = mpPointers[bType].xOffset;
        miCurYOffset = mpPointers[bType].yOffset;
        SetPointer(mLastPointerPos);

       #if !defined(HARDWARE_CURSOR)
        UNLOCK_PEG
       #endif
    }
   #endif
}


/*--------------------------------------------------------------------------*/
void CTLinear8Screen::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *CTLinear8Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 232;
    return muPalette;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
void VGA_DELAY(void)
{
    #ifdef __WATCOMC__
    for (WORD wLoop = 0; wLoop < 200; wLoop++)
    {
        dummy++;
    }
    #else
    for (WORD wLoop = 0; wLoop < 200; wLoop++)
    {
        wLoop++;
    }
    #endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
    POUTB(0x3c8, iFirst);
    VGA_DELAY();
	UCHAR *pPut = muPalette;
	const UCHAR *pPal = pGet;

    for (SIGNED iLoop = 0; iLoop < iCount * 3; iLoop++)
    {
        UCHAR uVal = *pGet++;
        uVal >>= 2;
        POUTB(0x3c9, uVal);
        VGA_DELAY();
    }

	// keep local copy
	for(SIGNED i = iFirst; i < iCount; i++)
	{
		*pPut++ = *pPal++;
		*pPut++ = *pPal++;
		*pPut++ = *pPal++;
	}
}

/*--------------------------------------------------------------------------*/

#define NUM_SEQ_REGS            5
#define NUM_CONTROLLER_REGS     25
#define NUM_ATTRIBUTE_REGS      21
#define NUM_GRAPHICS_REGS       9
#define NUM_X_REGS              128


#if defined(MODE320)      // configure for 320x240 LCD?

const UCHAR SRegs[NUM_SEQ_REGS] = {0x03, 0x01, 0x0f, 0x00, 0x0e};

const UCHAR CRegs[NUM_CONTROLLER_REGS] = {
 0x5f, 0x4f, 0x50, 0x82, 0x53, 0x9f, 0xbf, 0x1d,
 0x00, 0x40, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xea, 0x0c, 0xef, 0x28, 0x00, 0x96, 0xb9, 0xe3, 0xff
};

const UCHAR ARegs[NUM_ATTRIBUTE_REGS] = {
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
 0x01, 0x00, 0x0f, 0x00, 0x00
};

const UCHAR GRegs[NUM_GRAPHICS_REGS] = {
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff
};

const UCHAR XRegs[NUM_X_REGS] = {
  0xda, 0xfd, 0x81, 0x22, 0xac, 0x00, 0xc0,

#ifdef PEGTNT
  0xf4, 0x0C, 0x00, 0x00,
#else
  0xf4,  (VID_MEM_BASE >> 20), 0x00, 0x00,
#endif

#ifndef LINEAR_ADDRESSING
 // Use this for VGA memory mapping (A0000)
 0x05, 0x00, 0x00,

#else
 // Use this for Linear memory mapping (C00000)
 0x15, 0x00, 0x00,
#endif

 /* 0e */ 0x80, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00,
 /* 15 */ 0x00, 0x00, 0x00, 0x05, 0x29, 0x1a, 0x2f,   
 /* 1c */ 0x27, 0x7f, 0x05, 0x03, 0x00, 0x00, 0x00,
 /* 23 */ 0x00, 0x12, 0x59, 0x00, 0x00, 0x90, 0x4c,
 /* 2a */ 0x00, 0x30, 0x00, 0x29, 0x2e, 0x0b, 0x05,
 /* 31 */ 0x02, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
 /* 38 */ 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
 /* 3f */ 0xff, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00,
 /* 46 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /* 4d */ 0x00, 0x00, 0x47, 0x80, 0xc4, 0x44, 0x0c,
 /* 54 */ 0x38, 0xe5, 0x00, 0x23, 0x00, 0x84, 0x04,
 /* 5b */ 0x81, 0x02, 0x10, 0x80, 0x06, 0x88, 0x2e,
 /* 62 */ 0x07, 0x01, 0x05, 0x01, 0xf1, 0x04, 0xef,
 /* 69 */ 0x00, 0x00, 0x00, 0x0a, 0x00, 0x26, 0x00,
 /* 70 */ 0x80, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
 /* 77 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /* 7e */ 0x00, 0x3c
};

#elif defined(MODE640)      // configure for 640x480 mode?

const UCHAR SRegs[NUM_SEQ_REGS] = {0x03, 0x01, 0x0f, 0x00, 0x0e};

const UCHAR CRegs[NUM_CONTROLLER_REGS] = {
 0x61, 0x4f, 0x50, 0x82, 0x53, 0x9f, 0x0b, 0x3e,
 0x00, 0x40, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xea, 0x0c, 0xdf, 0x50, 0x00, 0xe7, 0x04, 0xe3, 0xff
};

const UCHAR ARegs[NUM_ATTRIBUTE_REGS] = {
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
 0x01, 0x00, 0x0f, 0x00, 0x00
};

const UCHAR GRegs[NUM_GRAPHICS_REGS] = {
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff
};

const UCHAR XRegs[NUM_X_REGS] = {
  0xda, 0xdd, 0x81, 0x22, 0xad, 0x00, 0x00,

//0xf4,  (VID_MEM_BASE >> 20), 0x00, 0x00,
0xf4, 0x0c, 0x00, 0x00,

#ifndef LINEAR_ADDRESSING
 // Use this for VGA memory mapping (A0000)
 0x05, 0x00, 0x00,

#else
 // Use this for Linear memory mapping (C00000)
 0x15, 0x00, 0x00,
#endif

 /* 0e */ 0x80, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00,
 /* 15 */ 0x00, 0x00, 0x00, 0x05, 0x53, 0x1f, 0x61,   
 /* 1c */ 0x4f, 0x7f, 0x05, 0x03, 0x00, 0x00, 0x00,
 /* 23 */ 0x00, 0x12, 0x59, 0x00, 0x00, 0x90, 0x4c,
 /* 2a */ 0x00, 0x30, 0x23, 0x51, 0x51, 0x0b, 0x03,
 /* 31 */ 0x17, 0x09, 0x20, 0x00, 0x00, 0x00, 0x00,
 /* 38 */ 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
 /* 3f */ 0xff, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
 /* 46 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /* 4d */ /* 0x00, 0x00, 0x44, 0x90, 0x67, 0x41, 0x0c, */
 /* 4d */    0x00, 0x00, 0x44, 0x90, 0x67, 0x41, 0x0c,
 /* 54 */ /* 0xfa, 0xe5, 0x00, 0x23, 0x00, 0x84, 0x04, */
 /* 54 */    0xe0, 0xe5, 0x00, 0x23, 0x00, 0x84, 0x04, 
 /* 5b */ 0x81, 0x02, 0x10, 0x80, 0x06, 0x88, 0x2e,
 /* 62 */ 0x07, 0x01, 0x0b, 0x26, 0xe9, 0x0b, 0xdf,
 /* 69 */ 0x00, 0x00, 0x00, 0x0b, 0x00, 0x26, 0x1b,
 /* 70 */ 0x80, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
 /* 77 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /* 7e */ 0x00, 0x3c
};

#elif defined(MODE800)

const UCHAR SRegs[NUM_SEQ_REGS] = {0x03, 0x01, 0x0f, 0x00, 0x0e};

const UCHAR CRegs[NUM_CONTROLLER_REGS] = {
 0x81, 0x63, 0x64, 0x82, 0x6a, 0x1a, 0x72, 0xf0,
 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x58, 0x8c, 0x57, 0x64, 0x00, 0x58, 0x72, 0xe3, 0xff
};

const UCHAR ARegs[NUM_ATTRIBUTE_REGS] = {
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
 0x01, 0x00, 0x0f, 0x00, 0x00
};

const UCHAR GRegs[NUM_GRAPHICS_REGS] = {
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff
};

const UCHAR XRegs[NUM_X_REGS] = {
  0xda, 0xdd, 0x01, 0x22, 0xad, 0x00, 0xc2,

//0xf4,  (VID_MEM_BASE >> 20), 0x00, 0x00,
0xf4, 0x1c, 0x00, 0x00,

#ifndef LINEAR_ADDRESSING
 // Use this for VGA memory mapping (A0000)
 0x00, 0x00, 0x00,

#else
 // Use this for Linear memory mapping (C00000)
 0x15, 0x00, 0x00,
#endif

 /* 0e */ 0x80, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00,
 /* 15 */ 0x00, 0x00, 0x00, 0x05, 0x6a, 0x1a, 0x81,   
 /* 1c */ 0x63, 0x7f, 0x05, 0x00, 0x00, 0x00, 0x00,
 /* 23 */ 0x00, 0x12, 0x59, 0x00, 0x00, 0x90, 0x4c,
 /* 2a */ 0x00, 0x32, 0x00, 0x68, 0x68, 0x0f, 0x03,
 /* 31 */ 0x11, 0x06, 0x20, 0x00, 0x00, 0x00, 0x00,
 /* 38 */ 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
 /* 3f */ 0xff, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
 /* 46 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /* 4d */ 0x00, 0x00, 0x46, 0x08, 0xc4, 0x41, 0x0c,
 /* 54 */ 0xe0, 0xe7, 0x00, 0x23, 0x00, 0x84, 0x04,
 /* 5b */ 0x81, 0x02, 0x10, 0x80, 0x06, 0x88, 0x2e,
 /* 62 */ 0x07, 0x01, 0x72, 0xe0, 0x58, 0x0c, 0x57,
 /* 69 */ 0x00, 0x00, 0x00, 0x0b, 0x00, 0xbd, 0x00,
 /* 70 */ 0x80, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
 /* 77 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /* 7e */ 0x00, 0x3c
};

#else
 ** ERROR, undefined mode
#endif
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void WriteExtReg(int index, int val)
{
    VGA_DELAY();
    POUTB(0x3d6, index);
    VGA_DELAY();
    POUTB(0x3d7, val);
    VGA_DELAY();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void WriteVgaReg(int index, int val)
{
    VGA_DELAY();
    POUTB(0x3d4, index);
    VGA_DELAY();
    POUTB(0x3d5, val);
    VGA_DELAY();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int ReadVgaReg(int index)
{ 
    VGA_DELAY();
    POUTB(0x3d4, index);
    VGA_DELAY();
    return(PINB(0x3d5));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR ReadSeqReg(int index)
{
    VGA_DELAY();
    POUTB(0x3c4, index);
    VGA_DELAY();
    return((UCHAR) PINB(0x3c5));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void WriteSeqReg(int index, int val)
{
    VGA_DELAY();
    POUTB(0x3c4, index);
    VGA_DELAY();
    POUTB(0x3c5, val);
    VGA_DELAY();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void WriteAttribReg(int rg, int val)
{
    PINB(0x3da);
    PINB(0x3ba);
    POUTB(0x3c0, rg);        // kills attrib controller
    VGA_DELAY();
    POUTB(0x3c0, val);
    VGA_DELAY();
    POUTB(0x3c0, rg | 0x20); // restart attrib controller
    VGA_DELAY();
}

void WriteVidReg(int index, int val)
{
    POUTB(0x3CE, index);
    VGA_DELAY();
    POUTB(0x3cf, val);
    VGA_DELAY();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//#include "pegdemo.hpp"

void CTLinear8Screen::ConfigureController(void)
{
    WORD wLoop;
    UCHAR uVal;

    #if defined(MODE320)
    POUTB(0x3da, 0x00);        // feature control
    #elif defined(MODE640)
    POUTB(0x3da, 0x01);
    #else
    POUTB(0x3da, 0x00);
    #endif
    VGA_DELAY();

    WriteSeqReg(0, 0x00);   // synchronous reset sequencer
    #if defined(MODE320)
    POUTB(0x3c2, 0x6f);     // Misc Output, select clock
    #elif defined(MODE640)
    POUTB(0x3c2, 0xeb);
    #else
    POUTB(0x3c2, 0x2b);
    #endif

    WriteSeqReg(0, 0x03);   // de-assert synchronous reset

    // setup the extension registers:

    for (wLoop = 0; wLoop < 0x30; wLoop++)
    {
        WriteExtReg(wLoop, XRegs[wLoop]);
    }
    WriteExtReg(0x33, XRegs[0x33]);
    WriteExtReg(0x30, XRegs[0x30]);
    WriteExtReg(0x31, XRegs[0x31]);
    WriteExtReg(0x32, XRegs[0x32]);

    for (wLoop = 0x34; wLoop < NUM_X_REGS; wLoop++)
    {
        WriteExtReg(wLoop, XRegs[wLoop]);
    }

    // setup the attribute controller:

    for (wLoop = 0; wLoop < NUM_ATTRIBUTE_REGS; wLoop++)
    {
        WriteAttribReg(wLoop, ARegs[wLoop]);
    }
    WriteAttribReg(0x20, ARegs[0]);

    // setup the crt controller regs:

    uVal = ReadVgaReg(0x11);
    uVal &= 0x7f;
    WriteVgaReg(0x11, uVal);

    for (wLoop = 0; wLoop < NUM_CONTROLLER_REGS; wLoop++)
    {
        WriteVgaReg(wLoop, CRegs[wLoop]);
    }

    // setup the graphics registers:

    for (wLoop = 0; wLoop < NUM_GRAPHICS_REGS; wLoop++)
    {
        WriteVidReg(wLoop, GRegs[wLoop]);
    }
    POUTB(0x3c6, 0xff);     // pixel mask register

    // setup the sequencer regs:

    WriteSeqReg(0, 0x03);   // restart sequencer

    for (wLoop = 0; wLoop < NUM_SEQ_REGS; wLoop++)
    {
        WriteSeqReg(wLoop, SRegs[wLoop]);
    }

    SetPalette(0, 232, DefPalette256);

    #if defined(MODE320)
    HorizontalLine(0, 319, 0, BLACK, 240);
    #elif defined(MODE640)
    HorizontalLine(0, 639, 0, BLACK, 480);
    #elif defined(MODE800)
    HorizontalLine(0, 799, 0, BLACK, 600);
    #endif

   #ifdef HARDWARE_CURSOR
     
    // program the cursor bitmaps for hardware cursor support:

    WORD wRow, wCol;
    UCHAR uSrcVal;
    DWORD aBits, xBits;
    DWORD dMask = 0x80000000L;
   
   #ifdef PEGTNT
    UCHAR *pPutCursBitmap = mpScanPointers[0]  - (1024 * 28);  // [ap] map to linear address
   #else
    UCHAR *pPutCursBitmap = (UCHAR *) VID_MEM_BASE + (100 * 1024);
   #endif

    PegPointer *pPointer = mpPointers;

    for (wLoop = 0; wLoop <= NUM_POINTER_TYPES; wLoop++)
    {
        UCHAR *pPut = pPutCursBitmap;
        PegBitmap *pPointerMap = pPointer->Bitmap;
        UCHAR *pGet = pPointerMap->pStart;

        for (wRow = 0; wRow < 32; wRow++)
        {
            aBits = xBits = 0;

            for (wCol =0 ; wCol < 32; wCol++)
            {
                // re-encode the bitmap as 2-bpp 
                if (wCol < pPointerMap->wWidth &&
                    wRow < pPointerMap->wHeight)
                {
                    uSrcVal = *pGet++;
                }
                else
                {
                    uSrcVal = TRANSPARENCY;
                }
                switch(uSrcVal)
                {
                case BLACK:
                    break;      // leave these bits 0

                case TRANSPARENCY:
                    aBits |= dMask;
                    break;

                default:
                    xBits |= dMask;
                }
                dMask >>= 1;

                if (!dMask)
                {
                    *pPut++ = (UCHAR) (aBits >> 24);
                    *pPut++ = (UCHAR) (xBits >> 24);
                    *pPut++ = (UCHAR) (aBits >> 16);
                    *pPut++ = (UCHAR) (xBits >> 16);
                    *pPut++ = (UCHAR) (aBits >> 8);
                    *pPut++ = (UCHAR) (xBits >> 8);
                    *pPut++ = (UCHAR) aBits;
                    *pPut++ = (UCHAR) xBits;
                    aBits = xBits = 0;
                    dMask = 0x80000000L;
                }
            }
        }
        pPutCursBitmap += 1024;
        pPointer++;
    }

    // program the extension registers to turn on the hardware cursor:

    POUTW(HC_COLOR0, 0);
    POUTW(HC_COLOR1, 0xffff);
    POUTW(HC_XOFFSET, 160);
    POUTW(HC_YOFFSET, 120);
    POUTW(HC_ADDR_HI, (WORD) (CURSOR_MEM_BASE >> 16));
    POUTW(HC_ADDR_LO, (WORD) CURSOR_MEM_BASE);

    // enable the 32x32 hardware cursor:

    POUTW(HC_ENABLE, 0x0021);
   #endif
}


#ifdef DOUBLE_BUFFER

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the video frame buffer:

void CTLinear8Screen::MemoryToScreen(void)
{
    if (!miInvalidCount)
    {
        return;
    }

    PegRect Copy;
    Copy.Set(0, 0, 319, 239);
    Copy &= mInvalid;

    Copy.wLeft &= 0xfffc;   // for DWORD alignment on the left edge

    SIGNED iTop = Copy.wTop;
    SIGNED iLeft = Copy.wLeft;
    SIGNED iWidth = Copy.Width();

    if (iWidth <= 0)
    {
        return;
    }
    iWidth += 3;
    iWidth &= 0xfffc;       // for DWORD alignment on the right edge

    SIGNED iHeight = Copy.Height();
    if (iHeight <= 0)
    {
        return;
    }

   #ifdef MODE320
    POUTW(BB_PITCH_REG, 0x0140);   // OFFSET 
    POUTW(BB_PITCH_REG + 2, 0x140);
   #else
    POUTW(BB_PITCH_REG, 0x320);
    POUTW(BB_PITCH_REG + 2, 0x320);
   #endif
   
    UCHAR *pGet = mpScanPointers[iTop] + iLeft;
    UCHAR *pBase = mpScanPointers[0];
    DWORD dDest = (DWORD) pGet;

   #ifdef PEGTNT					// convert linear address to physical
    DWORD dOffset = (DWORD) pBase - (128 * 1024);
    dDest-=dOffset;    
    dDest+=VID_MEM_BASE;
   #endif

   #ifdef LINEAR_ADDRESSING

    POUTD(BB_SRC_REG, (DWORD) dDest);
    dDest -= (128 * 1024);
    POUTD(BB_DEST_REG, dDest);

    POUTW(BB_CONTROL, 0x03cc);
    POUTW(BB_CONTROL + 2, 0x0000);

    POUTW(BB_COMMAND, (WORD) iWidth);
    POUTW(BB_COMMAND + 2, (WORD) iHeight);

   #else
    POUTD(BB_SRC_REG, dDest);
    dDest -= (DWORD) mpScanPointers[0];
    POUTD(BB_DEST_REG, dDest);

    POUTW(BB_CONTROL, 0x43cc);
    POUTW(BB_CONTROL + 2, 0x0000);

    POUTW(BB_COMMAND, (WORD) iWidth);
    POUTW(BB_COMMAND + 2, (WORD) iHeight);

    UCHAR *pPut = (UCHAR *) VID_MEM_BASE;
    DWORD *pGetStart = (DWORD *) pGet;
    DWORD *pGetd;
    
    for (WORD wLoop = 0; wLoop < (WORD) iHeight; wLoop++)
    {
        pGetd = pGetStart;

        for (WORD wLoop1 = 0; wLoop1 < iWidth; wLoop1 += 4)
        {
            *pPut = *pGetd++;
        }
        pGetStart += 0x320 / 4;
    }

   #endif
}

#endif



