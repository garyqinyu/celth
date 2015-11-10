/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// permedia.cpp - Linear 256 color screen driver for 
//  3DLabs Permedia2 video controller in a linear address
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
    PegScreen *pScreen = new Permedia2Screen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
Permedia2Screen::Permedia2Screen(PegRect &Rect) : PegScreen(Rect)
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
// This version of GetVideoAddress() is for PharLap ETS, running with PCI bus:
/*--------------------------------------------------------------------------*/
UCHAR *gpVidMemBase;

UCHAR PEGFAR *Permedia2Screen::GetVideoAddress(void)
{
    UCHAR *pMem = NULL;

   #if defined(USE_PCI)

    #if defined(USE_RADSTONEPPC)

	sysPciDevProbe(0x3d07104c, 0,&busNum, &slotNum);
	DWORD reg_addr = sysPciConfigReadLong(busNum,slotNum,0,0x10);
	DWORD mem_addr = sysPciConfigReadLong(busNum,slotNum,0,0x14);

	reg_addr |= PCI_MEM_SPACE;
    mpVidRegBase = (UCHAR *) reg_addr;
    mpVidMemBase = (UCHAR *) mem_addr;

    #else

    PCI_CONFIG tPciConfig;
    if(PegPCIReadControllerConfig(PEGPCI_VGACLASS, PEGPCI_VENDORID, 
                                  PEGPCI_DEVICEID, &tPciConfig))
    {
        pMem = (UCHAR *) (tPciConfig.BaseAddr2 & 0xFFFF0000L);
        mpVidMemBase = pMem;

        pMem = (UCHAR *) (tPciConfig.BaseAddr1 & 0xFFFF0000L);
        mpVidRegBase = pMem;
        pMem = mpVidMemBase;
    }

    #endif
   
   #else        // not using PCI bus configuration

    // Here for a direct connection. Use pre-defined addresses:

    mpVidMemBase = (UCHAR *) VID_MEM_BASE;

   #endif

    pMem = mpVidMemBase;
    gpVidMemBase = mpVidMemBase;

   #ifdef DOUBLE_BUFFER
    pMem += FRAME_BUFFER_SIZE;
   #endif

    return (pMem);
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
Permedia2Screen::~Permedia2Screen()
{
    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void Permedia2Screen::BeginDraw(PegThing *)
{
    LOCK_PEG

    if (!mwDrawNesting)
    {
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
void Permedia2Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
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
void Permedia2Screen::EndDraw()
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
void Permedia2Screen::EndDraw(PegBitmap *pMap)
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
void Permedia2Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void Permedia2Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
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
void Permedia2Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
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
void Permedia2Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
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
void Permedia2Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
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
void Permedia2Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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
void Permedia2Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
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
void Permedia2Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
void Permedia2Screen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void Permedia2Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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

void Permedia2Screen::DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
void Permedia2Screen::HidePointer(void)
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
void Permedia2Screen::SetPointer(PegPoint Where)
{
   #if defined(PEG_MOUSE_SUPPORT)
    if (mbShowPointer)
    {
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
    }
   #endif
}


/*--------------------------------------------------------------------------*/
void Permedia2Screen::SetPointerType(UCHAR bType)
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
void Permedia2Screen::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *Permedia2Screen::GetPalette(DWORD *pPutSize)
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


BOOL write_colour(UCHAR index, UCHAR red, UCHAR green, UCHAR blue);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Permedia2Screen::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
#if 0
    POUTB(0x3c8, iFirst);       // write palette index
    VGA_DELAY();

    for (SIGNED iLoop = 0; iLoop < iCount * 3; iLoop++)
    {
        UCHAR uVal = *pGet++;
        uVal >>= 2;
        POUTB(0x3c9, uVal);     // write palette data
        VGA_DELAY();
    }
#endif

    for (SIGNED iLoop = 0; iLoop < iCount; iLoop++)
    {
        write_colour(iFirst, *pGet, *(pGet + 1), *(pGet + 2));
        pGet += 3;
        iFirst++;
    }

}

/*--------------------------------------------------------------------------*/
#if defined(MODE640) || defined(MODE800) || defined(MODE1024)
#else

 #error ** You Must Define a Recognized Mode for Conguration **

#endif

PERMEDIA_REGS	*permedia2_reg;

//#define DO_BYTE_SWAP    // turn this off if x86 target, turn on for big-endian

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DWORD word_convert(DWORD reg_val)
{
	DWORD conv_val;

   #ifdef DO_BYTE_SWAP
	
	conv_val = ((reg_val & 0x000000ff) << 24 | ((reg_val & 0x0000ff00) << 8) | ((reg_val & 0x00ff0000) >> 8) | ((reg_val & 0xff000000) >> 24)); 
   #else

    conv_val = reg_val;

   #endif

	return conv_val;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL write_colour(UCHAR index, UCHAR red, UCHAR green, UCHAR blue)
{
	permedia2_reg->RD_WRITE_ADDR = index;
	VGA_DELAY();
	permedia2_reg->RD_DATA = red;
	VGA_DELAY();
	permedia2_reg->RD_DATA = green;
	VGA_DELAY();
	permedia2_reg->RD_DATA = blue;

	return TRUE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void x_sequencer(DWORD index, DWORD rval)
{
	permedia2_reg->X_SEQ_INDEX = (UCHAR) index; 
	VGA_DELAY(); 
	permedia2_reg->X_SEQ_DATA = (UCHAR) rval;
	VGA_DELAY();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void x_graphics(DWORD index, DWORD rval)
{
	permedia2_reg->X_GRA_INDEX = (UCHAR) index; 
	VGA_DELAY(); 
	permedia2_reg->X_GRA_DATA = (UCHAR) rval;
	VGA_DELAY();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void x_ramdac(DWORD index, DWORD rval)
{
	permedia2_reg->RD_WRITE_ADDR = (UCHAR) index; 
	VGA_DELAY(); 
	permedia2_reg->RD_INDEX_DATA = (UCHAR) rval;
	VGA_DELAY();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
unsigned long PM2DAC_CalculateMNPCForClock
(
 unsigned long reqclock,		/* In kHz units */
 unsigned long refclock,		/* In kHz units */
 unsigned char *rm, 			/* M Out */
 unsigned char *rn, 			/* N Out */
 unsigned char *rp			/* P Out */
 )
{
    unsigned char	m, n, p;
    unsigned long	f;
    long		freqerr, lowestfreqerr = 10000;
    unsigned long  	clock,actualclock = 0;

    for (n = 2; n <= 14; n++) {
        for (m = 2; m != 0; m++) { /* this is a char, so this counts to 255 */
	    f = refclock * m / n;
	    if ( (f < 15000) || (f > 230000) )
	    	continue;
	    for (p = 0; p <= 4; p++) {
	    	clock = f >> p;
		freqerr = reqclock - clock;
		if (freqerr < 0)
		    freqerr = -freqerr;
		if (freqerr < lowestfreqerr) {
		    *rn = n;
		    *rm = m;
		    *rp = p;
		    lowestfreqerr = freqerr;
		    actualclock = clock;
		}
	    }
	}
    }

    return(actualclock);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL	init_8x6() /* 800x600 32 bpp resolution, 75Hz */
{
	unsigned char n,m,p;
	unsigned long clk_used;
	unsigned long fref = 14318;
	int i;

	/* calculate M,M,P for oixel clock settings */

	clk_used = PM2DAC_CalculateMNPCForClock(75000,fref,&m ,&n, &p);
        VGA_DELAY(); 

	/*
	** Let's set-up a 800x600 video display.
	*/

	/* Enable Video Clock using PLL C */
	permedia2_reg->VCLK_CTRL	= word_convert(0x17);	/* 0x17 */
	VGA_DELAY();
	/* Configure the Permedia2 chip */
	permedia2_reg->CHIP_CONFIG	= word_convert(0x1c24);	/* 0x1c24 */
	VGA_DELAY();
	/* Screen Base is offset 0 from base of SGRAM */
	permedia2_reg->SCREEN_BASE 	= 0x0;		/* 0 */
	VGA_DELAY();
	/* Set the line increment value */
	permedia2_reg->SCREEN_STRIDE 	= word_convert(0x00000190);	/* 0xc8 */
	VGA_DELAY();
	/* Set the video display timing parameters */
	permedia2_reg->HTOTAL 		= word_convert(0x0000020f);     /* 0x209 */
	VGA_DELAY();
	permedia2_reg->HGEND 		= word_convert(0x00000080);	/* 0x80 */
	VGA_DELAY();
	permedia2_reg->HBEND 		= word_convert(0x00000080);	/* 0x80 */
	VGA_DELAY();
	permedia2_reg->HSSTART 		= word_convert(0x00000008);	/* 0x14 */
	VGA_DELAY();
	permedia2_reg->HSEND 		= word_convert(0x00000030);	/* 0x2a */
	VGA_DELAY();
	permedia2_reg->VTOTAL 		= word_convert(0x00000270);	/* 0x1f3 */
	VGA_DELAY();
	permedia2_reg->VBEND 		= word_convert(0x00000019);	/* 0x1c */
	VGA_DELAY();
	permedia2_reg->VSSTART 		= word_convert(0x00000000);	/* 0x1 */
	VGA_DELAY();
	permedia2_reg->VSEND 		= word_convert(0x00000003);	/* 0x4 */
	VGA_DELAY();
	permedia2_reg->VIDEO_CTRL 	= word_convert(0x00010229);	/* 0x29 */
	VGA_DELAY();
	permedia2_reg->INT_LINE 	= 0x0;		/* 0 */
	VGA_DELAY();
	permedia2_reg->DPY_DATA 	= word_convert(0xfccf);	/* 0xFCCE */
	VGA_DELAY();
/*	permedia2_reg->LINE_COUNT       = word_convert(0x0000012d);
	VGA_DELAY();*/
	permedia2_reg->FIFO_CTRL 	= word_convert(0x00011808);  // 0x08180100;
	VGA_DELAY();
	permedia2_reg->SBASE_RIGHT 	= 0x0;		/* 0 */
	VGA_DELAY();

	/*
	** Let's set-up the RAMDAC.
	*/

	/* Disable the cursor */
	x_ramdac(P2_RD_CURSOR_CTRL, 0x0);
	VGA_DELAY();
	/* True colour with CI8/SVGA */
	x_ramdac(P2_RD_COLOUR_MODE,0xb6); /* changed DGT 25/01/00 RGB565*/
	VGA_DELAY();
	/* Double buffering disabled */
	x_ramdac(P2_RD_MODE_CTRL, 0x0);
	VGA_DELAY();
	x_ramdac(P2_RD_PALETTE_PAGE,0x00);
	VGA_DELAY();
	/* Blanking pedestal and pixel width = 8 */
	x_ramdac(P2_RD_MISC_CTRL, 0x3e);
	VGA_DELAY();
	
	/* PCLK A1 = 25.06 MHz */
	x_ramdac(P2_RD_PCLK_A1, 0x02);
	VGA_DELAY();
	x_ramdac(P2_RD_PCLK_A2, 0x0b);
	VGA_DELAY();
	x_ramdac(P2_RD_PCLK_A3, 0x53);
	VGA_DELAY();

	
	/* PCLK A2 = 28.64 MHz */
	x_ramdac(P2_RD_PCLK_B1, 0x02);
	VGA_DELAY();
	x_ramdac(P2_RD_PCLK_B2, 0x0B);
	VGA_DELAY();
	x_ramdac(P2_RD_PCLK_B3, 0x7B);
	VGA_DELAY();


 	/* PCLK A3 = 60 MHz */

	x_ramdac(P2_RD_PCLK_C1, 0xb);
	VGA_DELAY();
	x_ramdac(P2_RD_PCLK_C2, 0x8);
	VGA_DELAY();
	x_ramdac(P2_RD_PCLK_C3, 0x14);
	VGA_DELAY(); 
	/* PCLK A3 = 60 MHz */
	x_ramdac(P2_RD_PCLK_C3, 0x6);
	VGA_DELAY();
	x_ramdac(P2_RD_PCLK_C1, m);
	VGA_DELAY();
	x_ramdac(P2_RD_PCLK_C2, n);
	VGA_DELAY();
	x_ramdac(P2_RD_PCLK_C3, p | 0x8);
	VGA_DELAY(); 

	for (i=0; i< 1000; i++) VGA_DELAY();

	x_ramdac(P2_RD_MCLK_1, 0x58);
	VGA_DELAY();
	x_ramdac(P2_RD_MCLK_2, 0x07);
	VGA_DELAY();
	x_ramdac(P2_RD_MCLK_3, 0x09);
	VGA_DELAY();
	x_ramdac(P2_RD_CKEY_CTRL, 0x0);
	VGA_DELAY();
	x_ramdac(P2_RD_CKEY_OVERLAY, 0x0);
	VGA_DELAY();
	x_ramdac(P2_RD_CKEY_RED, 0x0);
	VGA_DELAY();
	x_ramdac(P2_RD_CKEY_GREEN, 0x0);
	VGA_DELAY();
	x_ramdac(P2_RD_CKEY_BLUE, 0x0);
	VGA_DELAY();

	x_sequencer(P2_VGA_CTRL, 0x43);
	VGA_DELAY();

	x_graphics(P2_MODE_640, 0x0);
	VGA_DELAY(); 

	/*
	** Set up FIFO and memory configurations
	*/

	permedia2_reg->BOOT_ADDR = word_convert(0x00000020);
	VGA_DELAY(); 
	permedia2_reg->MEM_CONFIG = word_convert(0xe6002021);
	VGA_DELAY(); 
 	permedia2_reg->FIFO_CTRL = word_convert(0x00011e08);
	VGA_DELAY();  

	VGA_DELAY();

	return TRUE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL	graphics_initialise()
{
	/*
	** Let's set-up a 1024x768 video display.
	*/

	/* Enable Video Clock using PLL C */
	permedia2_reg->VCLK_CTRL	= word_convert(0x23);	/* 0x23 */
	VGA_DELAY();
	/* Configure the Permedia2 chip */
	permedia2_reg->CHIP_CONFIG	= word_convert(0x126);	/* 0x126 */
	VGA_DELAY();
	/* Screen Base is offset 0 from base of SGRAM */
	permedia2_reg->SCREEN_BASE 	= 0x0;		/* 0 */
	VGA_DELAY();
	/* Set the line increment value */
	permedia2_reg->SCREEN_STRIDE 	= word_convert(0x80);	/* 0x80 */
	VGA_DELAY();
	/* Set the video display timing parameters */
	permedia2_reg->HTOTAL 		= word_convert(0x14f); // 0x4F010000;	/* 0x14F */
	VGA_DELAY();
	permedia2_reg->HGEND 		= word_convert(0x50);  // 0x50000000;	/* 0x50 */
	VGA_DELAY();
	permedia2_reg->HBEND 		= word_convert(0x50);  // 0x50000000;	/* 0x50 */
	VGA_DELAY();
	permedia2_reg->HSSTART 		= word_convert(0x06);  // 0x06000000;	/* 0x6 */
	VGA_DELAY();
	permedia2_reg->HSEND 		= word_convert(0x28);  // 0x28000000;	/* 0x28 */
	VGA_DELAY();
	permedia2_reg->VTOTAL 		= word_convert(0x031f); //0x1F030000;	/* 0x31F */
	VGA_DELAY();
	permedia2_reg->VBEND 		= word_convert(0x20);  // 0x20000000;	/* 0x20 */
	VGA_DELAY();
	permedia2_reg->VSSTART 		= word_convert(0x01);  // 0x01000000;	/* 0x1 */
	VGA_DELAY();
	permedia2_reg->VSEND 		= word_convert(0x04);  // 0x04000000;	/* 0x4 */
	VGA_DELAY();
	permedia2_reg->VIDEO_CTRL 	= word_convert(0x0229); // 0x29020000;	/* 0x292 */
	VGA_DELAY();
	permedia2_reg->INT_LINE 	= 0x0;		/* 0 */
	VGA_DELAY();
	permedia2_reg->DPY_DATA 	= word_convert(0xf8fe); // 0xFEF80000;	/* 0xF8FE */
	VGA_DELAY();
	permedia2_reg->FIFO_CTRL 	= word_convert(0x1d08); // 0x081D0000;	/* 0x1D08 */
	VGA_DELAY();
	permedia2_reg->SBASE_RIGHT 	= 0x0;		/* 0 */
	VGA_DELAY();

	/*
	** Let's set-up the RAMDAC.
	*/

	/* Disable the cursor */
	P2_RAMDAC(P2_RD_CURSOR_CTRL)	= 0x0;
	VGA_DELAY();
	/* True colour with CI8/SVGA */
	P2_RAMDAC(P2_RD_COLOUR_MODE)	= 0x30;
	VGA_DELAY();
	/* Double buffering disabled */
	P2_RAMDAC(P2_RD_MODE_CTRL)	= 0x0;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_PALETTE_PAGE)	= 0x01;
	VGA_DELAY();
	/* Blanking pedestal and pixel width = 8 */
	P2_RAMDAC(P2_RD_MISC_CTRL)	= 0x12;
	VGA_DELAY();
	/* PCLK A1 = 25.06 MHz */
	P2_RAMDAC(P2_RD_PCLK_A1)	= 0x1C;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_PCLK_A2)	= 0x02;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_PCLK_A3)	= 0x0B;
	VGA_DELAY();
	/* PCLK A2 = 28.64 MHz */
	P2_RAMDAC(P2_RD_PCLK_B1)	= 0x20;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_PCLK_B2)	= 0x02;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_PCLK_B3)	= 0x0B;
	VGA_DELAY();
	/* PCLK A3 = 80.70 MHz */
	P2_RAMDAC(P2_RD_PCLK_C1)	= 0x7C;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_PCLK_C2)	= 0x0B;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_PCLK_C3)	= 0x09;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_MCLK_1)		= 0x58;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_MCLK_2)		= 0x07;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_MCLK_3)		= 0x09;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_CKEY_CTRL)	= 0x0;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_CKEY_OVERLAY)	= 0x0;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_CKEY_RED)	= 0x0;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_CKEY_GREEN)	= 0x0;
	VGA_DELAY();
	P2_RAMDAC(P2_RD_CKEY_BLUE)	= 0x0;
	VGA_DELAY();

	P2_X_SEQUENCER(P2_VGA_CTRL)	= 0x43;
	VGA_DELAY();

	P2_X_GRAPHICS(P2_MODE_640)	= 0x0;
	VGA_DELAY();

	return TRUE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void	fill_block(int x, int y, int s, int c)
{
	int		x1, y1;
	UCHAR *addr;

	addr = (UCHAR *)(gpVidMemBase + (s * 1024 * 1024) +
		(y * 1024 * 64));

	for(y1 = 0; y1 < 64; y1++) {

		for(x1 = (x * 64); x1 < ((x * 64) + 64); x1++) {

			addr[x1] = c;
		}

		addr += 1024;
	}
}


BOOL permedia_is_reset(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void	configure_permedia2(UCHAR *pRegBase)
{
	permedia2_reg = (PERMEDIA_REGS *) pRegBase;

	/*
	** Reset the Permedia2, graphics processor.
	*/

	permedia2_reg->RST_STATUS = 0xFFFFFFFF;
	VGA_DELAY();

    while(permedia_is_reset())
    {
    }


	/*
	** Set-up the Memory control and configuration registers.
	*/

	permedia2_reg->MEM_CTRL = P2_MEM_CTRL_DEFAULT;
	VGA_DELAY();
	permedia2_reg->MEM_CONFIG = P2_MEM_CONFIG_DEFAULT;
	VGA_DELAY();
	permedia2_reg->WRITE_MASK = 0xFFFFFFFF;
	VGA_DELAY();

    #ifdef MODE800
    init_8x6();
    #else
    graphics_initialise();
    #endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL permedia_is_reset(void)
{
	if (permedia2_reg->RST_STATUS)
    {
        return TRUE;
    }
    return FALSE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void Permedia2Screen::ConfigureController(void)
{
    configure_permedia2(mpVidRegBase);
}


#ifdef DOUBLE_BUFFER

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the video frame buffer:

void Permedia2Screen::MemoryToScreen(void)
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
}

#endif



