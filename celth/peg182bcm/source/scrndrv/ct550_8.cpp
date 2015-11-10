/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ct5550_8.cpp - Linear 256 color screen driver for 
//  Chips & Technology 65550 video controller in a linear address
//  configuration such as PCI bus.
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
// The driver supports three different resolutions out of the box.
// These resolutions are:
//
// MODE640:  Configure for 640x480  full color CRT and/or LCD.
// MODE800:  Configure for 800x600  full color CRT and/or LCD.
// MODE1024: Configure for 1024x768 full color CRT and/or LCD.
//
// Any other resolution can also be supported by modifying the timing
// register configuration in the ConfigureController function below.
//
// This driver will run with CRT Only, LCD Only, or dual output modes.
//
// All available configuration flags are found in the ct690008.hpp
// header file.
//
//
// Known Limitations:
// --------------------------------------------------------------------------------------------
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#define __COLORS        // don't want conio colors
#include <conio.h>

#include "peg.hpp"

#if defined(USE_PCI)
#include "pegpci.hpp"
#endif

extern UCHAR DefPalette256[];

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

    ConfigureController();        // set up controller registers

   #ifdef USE_VID_MEM_MANAGER
    UCHAR *pStart = CurrentPtr;
            
    #ifdef HARDWARE_CURSOR
    pStart += 4 * 1024;        // leave 4K for mouse pointer bitmaps
    #endif

    UCHAR *pEnd = mpVidMemBase;
    pEnd += VID_MEM_SIZE - 1;
    InitVidMemManager(pStart, pEnd);
   #endif

    mLastPointerPos.x = Rect.Width() / 2;
    mLastPointerPos.y = Rect.Height() / 2;
    mbPointerHidden = FALSE;
    mwDrawNesting = 0;

    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
// GetVideoAddress
//
// If USE_PCI is turned on, read the config from the PCI bus, otherwise,
// use the hardcoded address defined in ct550_8.hpp
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *CTLinear8Screen::GetVideoAddress(void)
{
    UCHAR *pMem = NULL;

   #if defined(USE_PCI) 

    pMem = PegPCIGetVideoAddress(PEGPCI_VGACLASS, PEGPCI_VENDORID,
                                 PEGPCI_DEVICEID);
    mpVidMemBase = pMem;
   
   #else

    // Here for a direct connection. Use pre-defined addresses:

    pMem = (UCHAR *) VID_MEM_BASE;
    mpVidMemBase = pMem;

   #endif

    mpVidMemBase = pMem;

   #ifdef DOUBLE_BUFFER
    pMem += FRAME_BUFFER_SIZE;
   #endif

    return (pMem);
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
CTLinear8Screen::~CTLinear8Screen()
{
    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void CTLinear8Screen::BeginDraw(PegThing *)
{
    LOCK_PEG

    if (!mwDrawNesting)
    {
        WaitNotBusy();

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
        WaitNotBusy();
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
#if 0

    while(wWidth-- > 0)
    {
        UCHAR *Put = mpScanPointers[wYPos] + wXStart;
        memset(Put, Color, iLen);
        wYPos++;
    }
#else
    // Use the bit-blit engine to do a solid color fill:

    WaitNotBusy();

    DWORD *pReg = (DWORD *) (mpVidMemBase + BITBLT_REGS);
    *pReg = ((DWORD) mwHRes << 16);

    *(pReg + 6) = 0;
    *(pReg + 5) = 0;
    *(pReg + 4) = 0x000c00f0;     // Solid Pattern Blit
    *(pReg + 3) = 0;

    DWORD dOffset = wYPos;
    dOffset *= mwHRes;

    #ifdef DOUBLE_BUFFER
    dOffset += FRAME_BUFFER_SIZE;
    #endif

    dOffset += wXStart;
    *(pReg + 7) = dOffset;

    *(pReg + 1) = Color;

    dOffset = wWidth;
    dOffset <<= 16;
    dOffset |= iLen;
    *(pReg + 8) = dOffset;      // start the copy!

    WaitNotBusy();

#endif
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
   #ifdef USE_VID_MEM_MANAGER

    if (IS_SPRITE(Getmap))      // bitmap is in video memory?
    {
        // check for no clipping required:

        if (Where.x == View.wLeft &&
            Where.y == View.wTop &&
            View.Width() == Getmap->wWidth &&
            View.Height() == Getmap->wHeight)
        {
            // blast the bitmap from vid mem to vid mem:

            DWORD dSource = (DWORD) Getmap->pStart - (DWORD) mpVidMemBase;
            DWORD dDest = (DWORD) mpScanPointers[Where.y] - (DWORD) mpVidMemBase;
            dDest += Where.x;
            WaitNotBusy();

            DWORD *pReg = (DWORD *) (mpVidMemBase + BITBLT_REGS);
            *pReg = ((DWORD) mwHRes << 16) | Getmap->wWidth;

            *(pReg + 6) = dSource;
            *(pReg + 7) = dDest;

            if (HAS_TRANS(Getmap))
            {
                *(pReg + 4) = 0x40cc;     // SRCCPY ROP with transparency
                *(pReg + 9) = Getmap->dTransColor;
            }
            else
            {
                *(pReg + 4) = 0xcc;     // SRCCPY ROP
            }
            dSource = Getmap->wHeight;
            dSource <<= 16;
            dSource |= Getmap->wWidth;
            *(pReg + 8) = dSource;      // start the copy!
            return;
        }
    }
   #endif

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

                    if (uVal != 0xff)
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
                for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
                {
                    *Put++ = *Get++;
                }
                Get += Getmap->wWidth - View.Width();
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
                        uVal != 0xff)
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

                if (uVal == 0xff)
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
#if 0
    // software emulated RectMove using Capture and Bitmap

    PegCapture BlockCapture;
    PegRect CaptureRect = View;
    Capture(&BlockCapture, CaptureRect);
    BlockCapture.Shift(xMove, yMove);
    Bitmap(Caller, BlockCapture.Point(), BlockCapture.Bitmap(), TRUE);

#else

    // Use the bit-blit engine to move the rectangle:

    SIGNED iWidth = View.Width();

    if (iWidth <= 0)
    {
        return;
    }

    SIGNED iHeight = View.Height();
    if (iHeight <= 0)
    {
        return;
    }

    WaitNotBusy();
    DWORD *pReg = (DWORD *) (mpVidMemBase + BITBLT_REGS);
    *pReg = ((DWORD) mwHRes << 16) | mwHRes;

    // program ROP and copy-direction:

    WORD wCopyStart = CS_UPPER_LEFT;

    if (yMove)
    {
        if (yMove > 0)      // shifting down?
        {
            wCopyStart = CS_LOWER_RIGHT;
        }
    }
    else
    {
        if (xMove > 0)      // shifting right?
        {
            wCopyStart = CS_LOWER_RIGHT;
        }
    }

    *(pReg + 4) = wCopyStart|0xcc;     // SRCCPY ROP

    // program source address:

    DWORD dOffset = View.wTop;
    dOffset *= mwHRes;
    dOffset += View.wLeft;

    // If we are starting in the lower-right corner, we need to
    // adjust our offset address accordingly!!

    if (wCopyStart == CS_LOWER_RIGHT)
    {
        dOffset += (DWORD) (iHeight - 1) * mwHRes;
        dOffset += iWidth - 1;
        //dOffset--;
    }

    #ifdef DOUBLE_BUFFER
    dOffset += FRAME_BUFFER_SIZE;
    #endif

    *(pReg + 6) = dOffset;

    // program the dest address:

    dOffset += yMove * mwHRes;
    dOffset += xMove;
    *(pReg + 7) = dOffset;

    dOffset = iHeight;
    dOffset <<= 16;
    dOffset |= iWidth;
    *(pReg + 8) = dOffset;      // start the copy!
#endif
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
        WriteExtReg(0xa0, 0x11);
    }
    else
    {
        WriteExtReg(0xa0, 0x00);
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
	
	    mLastPointerPos = Where;
	    Where.x -= miCurXOffset;
	    Where.y -= miCurYOffset;
	    WriteExtReg(0xa4, (UCHAR) Where.x);
	    WriteExtReg(0xa5, (UCHAR) (Where.x >> 8));
	    WriteExtReg(0xa6, (UCHAR) Where.y);
	    WriteExtReg(0xa7, (UCHAR) (Where.y >> 8));
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
        UCHAR uVal = ReadExtReg(0xa2);
        uVal &= 0xf0;
        uVal |= bType;
        WriteExtReg(0xa2, uVal);
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

int DummyDelayVal;
/*--------------------------------------------------------------------------*/
void VGA_DELAY(void)
{
    for (WORD wLoop = 0; wLoop < 200; wLoop++)
    {
        for (WORD wLoop1 = 0; wLoop1 < 2; wLoop1++)
        {
            DummyDelayVal++;
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
    POUTB(0x3c8, iFirst);       // write palette index
    VGA_DELAY();
	UCHAR *pPut = muPalette;
	const UCHAR *pPal = pGet;

    for (SIGNED iLoop = 0; iLoop < iCount * 3; iLoop++)
    {
        UCHAR uVal = *pGet++;
        uVal >>= 2;
        POUTB(0x3c9, uVal);     // write palette data
        VGA_DELAY();
    }

	for(SIGNED i = iFirst; i < iCount; i++)
	{
		*pPut++ = *pPal++;
		*pPut++ = *pPal++;
		*pPut++ = *pPal++;
	}
}

/*--------------------------------------------------------------------------*/
#if defined(MODE640) || defined(MODE800) || defined(MODE1024)
#else

 #error ** You Must Define a Recognized Mode for Conguration **

#endif

typedef struct {
    int iReg;
    int iVal;
} VID_REG_VAL;

/* Sequencer Registers: */

VID_REG_VAL SequenceRegs[] = {
{0x00,  0x03},       // Reset Register
{0x01,  0x01},       // Clock Mode  0x01??
{0x02,  0x0f},       // Map Mask
{0x03,  0x00},       // Char Map
{0x04,  0x0e},       // Memory Mode   0x02? 0x0e?
{0x07,  0x11},       // Horz Char Count
{0x00,  0x03},       // Reset Register
{-1, -1}
};

/* Graphics Registers: */

VID_REG_VAL GraphicsRegs[] = {

{0x00,  0x00},       // Set/Reset
{0x01,  0x00},       // Set/Reset Enable
{0x02,  0x00},       // Color Compare
{0x03,  0x00},       // Data Rotate
{0x04,  0x00},       // Read Plane Select
{0x05,  0x00},       // Mode Register   0x40??
{0x06,  0x05},       // Misc Reg   0x05 ??
{0x07,  0x0f},       // Color Dont Care
{0x08,  0xff},       // Bit Mask
{-1, -1}
};

/* Attrib Registers: */


VID_REG_VAL AttribRegs[] = {

{0x00,  0x00},       // ColorData00
{0x01,  0x01},       // ColorData01
{0x02,  0x02},       // ColorData02
{0x03,  0x03},       // ColorData03
{0x04,  0x04},       // ColorData04
{0x05,  0x05},       // ColorData05
{0x06,  0x06},       // ColorData06
{0x07,  0x07},       // ColorData07
{0x08,  0x08},       // ColorData08
{0x09,  0x09},       // ColorData09
{0x0a,  0x0a},       // ColorData0a
{0x0b,  0x0b},       // ColorData0b
{0x0c,  0x0c},       // ColorData0c
{0x0d,  0x0d},       // ColorData0d
{0x0e,  0x0e},       // ColorData0e
{0x0f,  0x0f},       // ColorData0f
{0x10,  0x01},       // Mode Control  0x10? 0x01?
{0x11,  0x00},       // Overscan Color
{0x12,  0x0f},       // Memory Plane En
{0x13,  0x00},       // Horz Pix Panning
{0x14,  0x00},       // Color Select
{-1, -1}
};

/* Extension Registers: (common) */

VID_REG_VAL ExtensionRegs[] = {

{0xd0,  0x0a},       // Powerdown Control- kill all!
{0x08,  0x03},       // Host Bus Config
{0x09,  0x00},       // I/O Control
{0x0a,  0x02},       // Frame Buff Mapping 0x00
{0x0b,  0x01},       // PCI Burst Support
{0x0e,  0x00},       // Frame Buff Page
{0x20,  0x00},       // BitBlt Config
{0x40,  0x03},       // Mem Access Control  0x02
{0x60,  0x00},       // Video Pin Control
{0x61,  0x00},       // DPMS Sync
{0x62,  0x00},       // GPIO Control
{0x63,  0xb6},       // GPIO Pin Data
{0x67,  0x00},       // Pin Tri-State
{0x70,  0xfe},       // Config Pins 0
{0x71,  0xff},       // Config Pins 1 
//{0x80,  0x00},       // Pipeline Config 0
{0x80,  0x80},       // Pipeline Config 0 // 8-bit DAC
{0x81,  0x02},       // Pipeline Config 1 0x00
{0x82,  0x00},       // Pipeline Config 2
{0xa0,  0x00},       // turn off hardware cursor
{0xc0,  0x7d},       // DotClk0 VCO M-Div Low
{0xc1,  0x07},       // DotClk0 VCO N-Div Low
{0xc2,  0x00},       // DotClk0 VCO M-N-Div High
{0xc3,  0x34},       // DotClk0 Divisor Select
{0xc4,  0x55},       // DotClk1 VCO M-Div Low
{0xc5,  0x09},       // DotClk1 VCO N-Div Low
{0xc6,  0x00},       // DotClk1 VCO M-N-Div High
{0xc7,  0x24},       // DotClk1 Divisor Select

//{-1, -1},

#if defined(MODE640)
{0xc8,  0x4e},       // DotClk2 VCO M-Div Low
{0xc9,  0x59},       // DotClk2 VCO N-Div Low
{0xca,  0x00},       // DotClk2 VCO M-N-Div High
{0xcb,  0x11},       // DotClk2 Divisor Select
#elif defined(MODE800)
{0xc8,  0xf0},       // DotClk2 VCO M-Div Low
{0xc9,  0x12},       // DotClk2 VCO N-Div Low
{0xca,  0x00},       // DotClk2 VCO M-N-Div High
{0xcb,  0x24},       // DotClk2 Divisor Select
#elif defined(MODE1024)
{0xc8,  0xf0},       // DotClk2 VCO M-Div Low
{0xc9,  0x0a},       // DotClk2 VCO N-Div Low
{0xca,  0x00},       // DotClk2 VCO M-N-Div High
{0xcb,  0x24},       // DotClk2 Divisor Select
#endif

{0xce, 0x21},       // default memory clock
{0xcc, 0x1a},       // 1a
{0xcd, 0x08},       // 08
{0xce, 0xa1},

{0xd0,  0x0f},       // Powerdown Control
{-1, -1}
};


/* Flat Panel Registers: */

VID_REG_VAL FlatPanelRegs[] = {

#ifdef CT_LCD_SUPPORT
{0x01,  0x02},       // panel only mode
#else
{0x01,  0x01},       // CRT only Mode
#endif

{0x03,  0x08},       // FP DotClock Src == CLK2
{0x04,  0x81},       // Power Seq Delay
{0x05,  0x21},       // Power Down Ctrl 1
{0x06,  0x03},       // FP Power Down Ctrl

#ifdef MODE640
{0x08,  0xcc},       // FP Pin Polarity
#else
{0x08,  0x00},       // FP Pin Polarity
#endif
{0x0a,  0x08},       // Prog Output Drive
{0x0b,  0x01},       // Pin Control 1
{0x0c,  0x00},       // Pin Control 2
{0x0f,  0x02},       // Activity Timer
{0x10,  0x0c},       // FP Format 0     SHFCLK==DOTCLK, COLOR, SINGLE
{0x11,  0xe0},       // FP Format 1     8bpp, no dither, no frc
{0x12,  0x50},       // FP Format 2
{0x13,  0x00},       // FP Format 3
{0x16,  0x03},       // FRC Option Select
{0x17,  0xbd},       // Polynomial FRC
{0x18,  0x00},       // Text Mode Ctrl
//{0x19,  0x88},       // Blink Rate
{0x19,  0x5a},       // Blink Rate
{0x1a,  0x00},       // STN Buffering
{0x1e,  0x80},       // ACDCLK Control

#if defined(MODE640)
{0x20,  0x4f},       // Horz Size LSB
{0x21,  0x51},       // Horz Sync Start LSB
{0x22,  0x1d},       // Horz Sync End
{0x23,  0x5e},       // Horz Total LSB
{0x24,  0x50},       // Hsynch Delay LSB
{0x25,  0x00},       // Horz Overflow 1
{0x26,  0x00},       // Horz Overflow 2
{0x27,  0x0b},       // HSynch Width
{0x30,  0xdf},       // Vert Size LSB
{0x31,  0xe8},       // Vert Synch LSB
{0x32,  0x0c},       // Vert Synch End
{0x33,  0x0b},       // Vert Total LSB
{0x34,  0x00},       // VSynch Delay LSB
{0x35,  0x11},       // Vert Overflow 1
{0x36,  0x02},       // Vert Overflow 2

#elif defined(MODE800)
{0x20,  0x63},       // Horz Size LSB
{0x21,  0x6a},       // Horz Sync Start LSB
{0x22,  0x1d},       // Horz Sync End
{0x23,  0x80},       // Horz Total LSB
{0x24,  0x50},       // Hsynch Delay LSB
{0x25,  0x00},       // Horz Overflow 1
{0x26,  0x00},       // Horz Overflow 2
{0x27,  0x0b},       // HSynch Width
{0x30,  0x57},       // Vert Size LSB
{0x31,  0x58},       // Vert Synch LSB
{0x32,  0x0c},       // Vert Synch End
{0x33,  0x72},       // Vert Total LSB
{0x34,  0x00},       // VSynch Delay LSB
{0x35,  0x22},       // Vert Overflow 1
{0x36,  0x02},       // Vert Overflow 2
#elif defined(MODE1024)
{0x20,  0x7f},       // Horz Size LSB
{0x21,  0x85},       // Horz Sync Start LSB
{0x22,  0x1d},       // Horz Sync End
{0x23,  0xa0},       // Horz Total LSB
{0x24,  0x50},       // Hsynch Delay LSB
{0x25,  0x00},       // Horz Overflow 1
{0x26,  0x00},       // Horz Overflow 2
{0x27,  0x0b},       // HSynch Width
{0x30,  0xff},       // Vert Size LSB
{0x31,  0x12},       // Vert Synch LSB
{0x32,  0x0c},       // Vert Synch End
{0x33,  0x2a},       // Vert Total LSB
{0x34,  0x00},       // VSynch Delay LSB
{0x35,  0x32},       // Vert Overflow 1
{0x36,  0x03},       // Vert Overflow 2
#endif
{0x37,  0x80},       // VSynch Disable
{0x40,  0x83},       // Horz Compensation
{0x41,  0x00},       // Horz Stretching
{0x48,  0x13},       // Vert Compensation
{0x70,  0x00},       // TMED Red Seed
{0x71,  0x55},       // TMED Green Seed
{0x72,  0xaa},       // TMED Blue Seed
{0x73,  0x00},       // TMED Control
{0x74,  0x5f},       // TMED2 Shift
{-1, -1}
};


VID_REG_VAL CRTCRegs[] = {

#if defined(MODE640)
{0x00,  0x5f},       // H Cols
{0x01,  0x4f},       // H Disp End
{0x02,  0x50},       // Start H Blank
{0x03,  0x82},       // End H Blank
{0x04,  0x55},       // H Sync Start
{0x05,  0x81},       // H Sync End
{0x06,  0x0b},       // V Total
{0x07,  0x3e},       // Overflow
{0x08,  0x00},       // Initial Row
{0x09,  0x40},       // Max Row
#elif defined(MODE800)
{0x00,  0x81},       // H Cols
{0x01,  0x63},       // H Disp End
{0x02,  0x64},       // Start H Blank
{0x03,  0x82},       // End H Blank
{0x04,  0x6a},       // H Sync Start
{0x05,  0x1a},       // H Sync End
{0x06,  0x72},       // V Total
{0x07,  0xf0},       // Overflow
{0x08,  0x00},       // Initial Row
{0x09,  0x60},       // Max Row
#elif defined(MODE1024)
{0x00,  0xa1},       // H Cols
{0x01,  0x7f},       // H Disp End
{0x02,  0x80},       // Start H Blank
{0x03,  0x84},       // End H Blank
{0x04,  0x85},       // H Sync Start
{0x05,  0x93},       // H Sync End
{0x06,  0x2a},       // V Total
{0x07,  0xfd},       // Overflow
{0x08,  0x00},       // Initial Row
{0x09,  0x60},       // Max Row
#endif

{0x0a,  0x00},       // Cursor Start
{0x0b,  0x00},       // Cursor End
{0x0c,  0x00},       // Start MSB
{0x0d,  0x00},       // Start LSB
{0x0e,  0x00},       // Cursor Loc MSB
{0x0f,  0x00},       // Cursor Loc LSB

#if defined(MODE640)
{0x10,  0xea},       // Vert Sync Start
{0x11,  0x8c},       // Vert Sync End
{0x12,  0xdf},       // Vert Disp End
{0x13,  0x50},       // Row Offset
{0x14,  0x00},       // Addr Mode / Underline Addr  0x00
{0x15,  0xe7},       // Start VBlank
{0x16,  0x04},       // End VBlank
#elif defined(MODE800)
{0x10,  0x58},       // Vert Sync Start
{0x11,  0x8c},       // Vert Sync End
{0x12,  0x57},       // Vert Disp End
{0x13,  0x64},       // Row Offset
{0x14,  0x00},       // Addr Mode / Underline Addr  0x00
{0x15,  0x58},       // Start VBlank
{0x16,  0x72},       // End VBlank
#elif defined(MODE1024)
{0x10,  0x12},       // Vert Sync Start
{0x11,  0x89},       // Vert Sync End
{0x12,  0xff},       // Vert Disp End
{0x13,  0x80},       // Row Offset
{0x14,  0x00},       // Addr Mode / Underline Addr  0x00
{0x15,  0x00},       // Start VBlank
{0x16,  0x2a},       // End VBlank
#endif
{0x17,  0xe3},       // CRTC Mode 
{0x18,  0xff},       // Line Compare
{0x30,  0x00},       // Ex Vert Total
{0x31,  0x00},       // Ex Vert Disp End
{0x32,  0x00},       // Ex Vert Sync Start
{0x33,  0x00},       // Ex Vert Blank Start
{0x38,  0x00},       // Ex Horz Total
{0x3c,  0x00},       // Ex Horz Blank End
{0x40,  0x00},       // Ex Start Addr
{0x41,  0x00},       // Ex Offset
{0x70,  0x00},       // Interlace Control
{0x71,  0x00},       // NTSC Output Control  0x00 ??
{-1, -1}
};


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::WriteExtReg(int index, int val)
{
    POUTB(0x3d6, index);    // Configuration Extensions Index
    POUTB(0x3d7, val);      // Configuration Extensions Data
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR CTLinear8Screen::ReadExtReg(int index)
{
    POUTB(0x3d6, index);           // Configuration Extensions Index
    UCHAR uVal = PINB(0x3d7);      // Configuration Extensions Data
    return uVal;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::WriteFPReg(int index, int val)
{
    POUTB(0x3d0, index);    // Flat Panel Extensions Index
    POUTB(0x3d1, val);      // Flat Panel Extensions Data
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR CTLinear8Screen::ReadFPReg(int index)
{
    POUTB(0x3d0, index);    // Flat Panel Extensions Index
    UCHAR uVal = PINB(0x3d1);
    return uVal;            // Flat Panel Extensions Data
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::WriteCRTCReg(int index, int val)
{
    POUTB(0x3d4, index);    // CRTC Index
    POUTB(0x3d5, val);      // CRTC Data
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int CTLinear8Screen::ReadCRTCReg(int index)
{ 
    POUTB(0x3d4, index);    // CRTC Index
    UCHAR uVal = PINB(0x3d5);      // CRTC Data
    return uVal;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR CTLinear8Screen::ReadSeqReg(int index)
{
    POUTB(0x3c4, index);        // Sequencer Index
    UCHAR uVal = PINB(0x3c5);   // Sequencer Data
    return uVal;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::WriteSeqReg(int index, int val)
{
    POUTB(0x3c4, index);        // Sequencer Index
    POUTB(0x3c5, val);          // Sequencer Data
}

UCHAR guDummyStatusVal;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::WriteAttribReg(int rg, int val)
{
    // read from the status register to put the attribute controller
    // index/data flip-flop back into index mode:

    PINB(0xcda);

   
    // now write the index, then the data,
    //* then index|0x20 to restart controller    

    POUTB(0x3c0, rg);
    POUTB(0x3c0, val);
    POUTB(0x3c0, rg|0x20); // restart attrib controller
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::WriteGraphicsReg(int index, int val)
{
    POUTB(0x3ce, index);
    POUTB(0x3cf, val);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CTLinear8Screen::WaitNotBusy(void)
{
    volatile DWORD *pReg = (DWORD *) (mpVidMemBase + BITBLT_REGS);
    pReg += 4;
    DWORD dVal = *pReg;

    while(dVal & 0x80000000)
    {
        dVal = *pReg;
    }
}

#ifdef HARDWARE_CURSOR
UCHAR CursorPalette[6] = {0, 0, 0, 0xff, 0xff, 0xff};
#endif


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//#include "pegdemo.hpp"

void CTLinear8Screen::ConfigureController(void)
{
    UCHAR uVal;
    UCHAR uFlags;
    VID_REG_VAL *pRegVal;

    WaitNotBusy();

    // disable CRT and LCD engines:
    WriteFPReg(0x01, 0x00);

    uFlags = ReadFPReg(0x19);

    if (uFlags == 0x5a)
    {
        WriteFPReg(0x19, 0x0f);
        ConfigureController();
    }

    WriteSeqReg(0, 0x00);   // synchronous reset sequencer

    VGA_DELAY();

    // Miscellaneous Output Register

    #if defined(MODE320)
    POUTB(0x3c2, 0x6b);     // CLK2, CGA emulation
    #elif defined(MODE640)
    POUTB(0x3c2, 0xeb);     // CLK2, CGA emulation
    #else
    POUTB(0x3c2, 0x2b);
    #endif

    // Feature Control

    #if defined(MODE320)
    POUTB(0x3da, 0);
    #elif defined(MODE640)
    POUTB(0x3da, 1);
    #else
    POUTB(0x3da, 0);
    #endif

    VGA_DELAY();

    // setup the extension registers:
    pRegVal = ExtensionRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteExtReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }

    // setup the flat panel registers:

    pRegVal = FlatPanelRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteFPReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }

    // setup the attribute controller:

    pRegVal = AttribRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteAttribReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }

    // setup the crt controller regs:

    uVal = ReadCRTCReg(0x11);
    uVal &= 0x7f;
    WriteCRTCReg(0x11, uVal);

    pRegVal = CRTCRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteCRTCReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }

    // setup the graphics registers:

    pRegVal = GraphicsRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteGraphicsReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }

    POUTB(0x3c6, 0xff);     // pixel mask reg

    // setup the sequencer regs:

    WriteSeqReg(0, 0x03);   // restart sequencer

    pRegVal = SequenceRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteSeqReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
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

    UCHAR  AndVal, XorVal, uMask, uRow, uCol, uByteCount, uSrcVal;
    UCHAR *pMem = mpVidMemBase;
    WORD wLoop;
    DWORD dOffset = FRAME_BUFFER_SIZE;
     
    #ifdef DOUBLE_BUFFER
    dOffset += FRAME_BUFFER_SIZE;
    #endif

    pMem += dOffset;

    WriteExtReg(0xa1, 0);
    WriteExtReg(0xa2, (dOffset >> 12) << 4);
    WriteExtReg(0xa3, dOffset >> 16);

    SIGNED Pos = mwHRes / 2;

    WriteExtReg(0xa4, Pos);
    WriteExtReg(0xa5, Pos >> 8);

    Pos = (mwVRes / 2);

    WriteExtReg(0xa6, Pos);
    WriteExtReg(0xa7, (Pos >> 8));

    for (wLoop = 0; wLoop < SYSTEM_POINTER_TYPES; wLoop++)
    {
        UCHAR *pPut = pMem;
        UCHAR *pPut1 = pPut + 8;
        uByteCount = 0;

        PegBitmap *pPointerMap = mpPointers[wLoop].Bitmap;

        UCHAR *pGet = pPointerMap->pStart;

        for (uRow = 0; uRow < 32; uRow++)
        {
            AndVal = XorVal = 0;
            uMask = 0x80;
            for (uCol = 0; uCol < 32; uCol++)
            {
                // re-encode the bitmap as 2-bpp
                if (uCol < pPointerMap->wWidth &&
                    uRow < pPointerMap->wHeight)
                {
                    uSrcVal = *pGet++;
                }
                else
                {
                    uSrcVal = TRANSPARENCY;
                }
                switch(uSrcVal)
                {
                case WHITE:
                    XorVal |= uMask;
                    break;

                case TRANSPARENCY:
                    AndVal |= uMask;
                    break;

                default:
                    break;
                }
                uMask >>= 1;

                if (!uMask)
                {
                    *pPut++ = AndVal;
                    *pPut1++ = XorVal;
                    uMask = 0x80;
                    AndVal = XorVal = 0;
                    uByteCount++;

                    if (uByteCount == 8)
                    {
                        pPut += 8;
                        pPut1 += 8;
                        uByteCount = 0;
                    }
                }
            }
        }
        pMem += 256;        // where to start next cursor
    }

    // Write the cursor 1 color registers:

    uSrcVal = ReadExtReg(0x80);     // read pipeline config 0
    WriteExtReg(0x80, uSrcVal | 0x01);  // set bit 0 to enable alternate pal regs

    SetPalette(0, 2, CursorPalette);

    SetPalette(4, 2, CursorPalette);

    WriteExtReg(0x80, uSrcVal | 0x10);     // put bit 0 back for normal palette
                                           // and turn on hardware cursor

    // OK, turn on the hardware cursor
    SetPointerType(0);
    WriteExtReg(0xa0, 0x11);

   #endif

   #ifdef DOUBLE_BUFFER
   miInvalidCount = 1;
   mInvalid.Set(0, 0, mwHRes - 1, mwVRes - 1);
   MemoryToScreen();
   miInvalidCount = 0;
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

    WaitNotBusy();
    DWORD *pReg = (DWORD *) (mpVidMemBase + BITBLT_REGS);
    *pReg = ((DWORD) mwHRes << 16) | mwHRes;

    *(pReg + 4) = 0xcc;     // SRCCPY ROP

    DWORD dOffset = Copy.wTop;

    dOffset *= mwHRes;
    dOffset += Copy.wLeft;
    *(pReg + 6) = (dOffset + FRAME_BUFFER_SIZE);
    *(pReg + 7) = dOffset;
    dOffset = iHeight;
    dOffset <<= 16;
    dOffset |= iWidth;
    *(pReg + 8) = dOffset;      // start the copy!
    WaitNotBusy();
}

#endif



