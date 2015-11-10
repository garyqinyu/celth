/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// cl54xpci.cpp - Instantiable PegScreen driver optimized for Cirrus Logic
//                  GD54xx 'Alpine' family of video controllers.
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
// This driver is written specifically for a Cirrus Logic 5430 video
// controller, and is written assuming the controller memory can be fully
// accessed without paging or other schemes. For x86, this requires a PCI
// bus, while for everything else direct linear memory access is the norm.
//
// This driver is written to make extensive use of the hardware bit-blit
// engine of the GD5430. This allows for fast rectangle filling, fast scrolling,
// and fast bitmap rendering. In order to use fast bitmap rendering, the 
// application level software should:
//
// 1) Use the CreateBitmap function to open a bitmap in video memory.
// 2) Draw into the bitmap using the alternate Begin-EndDraw() functions.
// 3) Use the Bitmap() function to transfer the image into visible video
//    video memory.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"

#if defined(USE_PCI)
#include "pegpci.hpp"
#endif

#define __COLORS        // don't want conio colors
#include <conio.h>

extern UCHAR DefPalette256[];

/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new SVGAScreen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/

SVGAScreen::SVGAScreen(PegRect &Rect) : PegScreen(Rect)
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

   #ifdef USE_VID_MEM_MANAGER
    UCHAR *pStart = mpScanPointers[0];
    UCHAR *pEnd = pStart + VID_MEM_SIZE - 1;
    pStart += (DWORD) mwHRes * (DWORD) mwVRes;
    InitVidMemManager(pStart, pEnd);
   #endif

    ConfigureSVGA();        // set up for SVGA, PCI-linear memory
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// GetVideoAddress- Returns the base address of the video frame buffer.
//
// There are different forms of the GetVideoAddress function. These
// different forms are for different operating systems and system 
// architechtures.
//
// If you are using the GD5470 in a simple 32-bit RISC environment, you can 
// probably just provide a version of this function that returns a
// pre-defined address.
//
// The version below assumes that you have defined USE_PCI.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *SVGAScreen::GetVideoAddress(void)
{
    UCHAR* pMem = PegPCIGetVideoAddress(PEGPCI_VGACLASS, PEGPCI_VENDORID,
                                        PEGPCI_DEVICEID);

    mpVidMemBase = (DWORD*) pMem;

    if(pMem)
    {
    #ifdef DOUBLE_BUFFER
     #ifdef LOCAL_BUFFERING
      pMem = new UCHAR[mwHRes * mwVRes];
     #else
      pMem += (DWORD) mwHRes * (DWORD) mwVRes;
     #endif
    #endif
    }

    return (pMem);
}

/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
SVGAScreen::~SVGAScreen()
{
    delete mpScanPointers;
}

UCHAR gbPreventOptimize;

/*--------------------------------------------------------------------------*/
void SVGAScreen::BeginDraw(PegThing *)
{
    LOCK_PEG        // if multitasking, lock the screen
    if (!mwDrawNesting)
    {
       #if defined(DOUBLE_BUFFER) && !defined(LOCAL_BUFFERING)
        // make sure bit-blit engine is done:
        while(ReadVidReg(0x31) & 0x01)
        {
            gbPreventOptimize++;
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
void SVGAScreen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG        // if multitasking, lock the screen

    #if defined(DOUBLE_BUFFER) && !defined(LOCAL_BUFFERING)
    // make sure bit-blit engine is done:
    while(ReadVidReg(0x31) & 0x01)
    {
        gbPreventOptimize++;
    }
    #endif

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
void SVGAScreen::EndDraw()
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
void SVGAScreen::EndDraw(PegBitmap *pMap)
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
void SVGAScreen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void SVGAScreen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR *Put;
    while(wWidth-- > 0)
    {
        Put = mpScanPointers[wYPos] + wXStart;

        // most compilers seem to do a good job of optimizing 
        // memset to do 32-bit data writes. If your compiler doesn't
        // make the most of your CPU, you might want to re-write this
        // in assembly.

        memset(Put, Color, wXEnd - wXStart + 1);
        wYPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SVGAScreen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
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
void SVGAScreen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    UCHAR *Put = mpScanPointers[wYPos] + wXStart;
    UCHAR uVal;

    while (wXStart <= wXEnd)
    {
        uVal = *Put;
        uVal ^= 0xf;
        *Put = uVal;
        Put += 2;
        wXStart += 2;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SVGAScreen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
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
void SVGAScreen::Capture(PegCapture *Info, PegRect &CaptureRect)
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

    // fill in the image with our captured info:

    UCHAR *Put = pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        UCHAR *Get = GetStart;
        memcpy(Put, Get, pMap->wWidth);
        GetStart += mwHRes;
        Put += pMap->wWidth;
    }
    Info->SetValid(TRUE);
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SVGAScreen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
   #ifdef USE_VID_MEM_MANAGER
    if (IS_SPRITE(Getmap))
    {
        // bitmap is already in video memory, just do a bit-blit:
        WORD wTemp = View.Width() - 1;
        WriteVidReg(0x20, wTemp);
        WriteVidReg(0x21, wTemp >> 8);        
        wTemp = View.Height() - 1;
        WriteVidReg(0x22, wTemp);
        WriteVidReg(0x23, wTemp >> 8);
        WriteVidReg(0x24, mwHRes);
        WriteVidReg(0x25, mwHRes >> 8);
        wTemp = Getmap->wWidth;
        WriteVidReg(0x26, wTemp);
        WriteVidReg(0x27, wTemp >> 8);

        DWORD dDest = (DWORD) (mpScanPointers[View.wTop] + View.wLeft);
        dDest -= (DWORD) mpScanPointers[0];

        WriteVidReg(0x28, dDest);
        WriteVidReg(0x29, dDest >> 8);
        WriteVidReg(0x2a, dDest >> 16);

        dDest = (DWORD) Getmap->pStart;
        dDest += (View.wTop - Where.y) * Getmap->wWidth;
        dDest += (View.wLeft - Where.x);
        dDest -= (DWORD) mpScanPointers[0];

        WriteVidReg(0x2c, dDest);
        WriteVidReg(0x2d, dDest >> 8);
        WriteVidReg(0x2e, dDest >> 16);

        WriteVidReg(0x30, 0);
        WriteVidReg(0x32, 0x0d);
        WriteVidReg(0x33, 0);
        WriteVidReg(0x31, 2);

        // wait for the bit-blit to finish:

        while(ReadVidReg(0x31) & 0x01);
        return;
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
            WORD wWidth = View.Width();
            for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
            {
                UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
                memcpy(Put, Get, wWidth);
                Get += Getmap->wWidth;
            }

        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SVGAScreen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
void SVGAScreen::RectMoveView(PegThing *Caller, const PegRect &View,
     const SIGNED xMove, const SIGNED yMove)
{
    PegCapture tCapture;
    PegRect CaptureRect = View;
    Capture(&tCapture, CaptureRect);
    tCapture.Shift(xMove, yMove);
    Bitmap(Caller, tCapture.Point(), tCapture.Bitmap(), TRUE);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SVGAScreen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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

/*--------------------------------------------------------------------------*/
//  Begin utility I/O functions
/*--------------------------------------------------------------------------*/
void VGA_DELAY(void)
{
    for (WORD wLoop = 0; wLoop < 50; wLoop++)
    {
        wLoop++;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SVGAScreen::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get)
{
    POUTB(0x3c8, iFirst);
    VGA_DELAY();

    for (SIGNED iLoop = 0; iLoop < iNum * 3; iLoop++)
    {
        POUTB(0x3c9, (*Get++) >> 2);
        VGA_DELAY();
    }
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
    POUTB(0x3c0, val);
    POUTB(0x3c0, rg | 0x20); // restart attrib controller
}
/*--------------------------------------------------------------------------*/
// End Utility I/O functions
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void SVGAScreen::HidePointer(void)
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

/*--------------------------------------------------------------------------*/
void SVGAScreen::SetPointer(PegPoint Where)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (mbShowPointer)
    {
      #ifdef HARDWARE_CURSOR
	    mLastPointerPos = Where;
	    Where.x -= miCurXOffset;
	    Where.y -= miCurYOffset;
	
	    WORD wVal = Where.x >> 3;
	    wVal <<= 8;
	    wVal |= 0x10;
	    wVal |= (Where.x & 7) << 5;
	    POUTW(0x3c4, wVal);
	
	    wVal = Where.y >> 3;
	    wVal <<= 8;
	    wVal |= 0x11;
	    wVal |= (Where.y & 7) << 5;
	    POUTW(0x3c4, wVal);
	
	   #else
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
       #endif   // HARDWARE_CURSOR ?
    }
   #endif   // MOUSE_SUPPORT ?
}

/*--------------------------------------------------------------------------*/
void SVGAScreen::SetPointerType(UCHAR bType)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (bType < NUM_POINTER_TYPES)
    {
       #ifndef HARDWARE_CURSOR
        LOCK_PEG
        HidePointer();
       #endif

        mpCurPointer = mpPointers[bType].Bitmap;

       #if defined(HARDWARE_CURSOR)
        // reset the pointer register:
        WriteSeqReg(0x13, bType);
       #endif

        miCurXOffset = mpPointers[bType].xOffset;
        miCurYOffset = mpPointers[bType].yOffset;
        SetPointer(mLastPointerPos);

       #ifndef HARDWARE_CURSOR
        UNLOCK_PEG
       #endif
    }
   #endif   // MOUSE_SUPPORT?
}


/*--------------------------------------------------------------------------*/
void SVGAScreen::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *SVGAScreen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 232;
    return mPalette;
}

void SVGAScreen::ShowPointer(BOOL bShow)
{
   #ifdef HARDWARE_CURSOR
    if (bShow)
    {
        WriteSeqReg(0x12, 0x01);        // turn on 32x32 hardware cursor
    }
    else
    {
        WriteSeqReg(0x12, 0x00);        // turn on 32x32 hardware cursor
    }
   #endif
    PegScreen::ShowPointer(bShow);
}


/*--------------------------------------------------------------------------*/
void SVGAScreen::Configure1024_768_256(void)
{
    int temp;

    PINB(0x3da);         // read the status register, set attrib mode to index
    VGA_DELAY();
    POUTB(0x3c0, 0);     // stop the attribute controller
    VGA_DELAY();
    POUTB(0x3c0, 0);
    VGA_DELAY();

    VGA_DELAY();
    PINB(0x3da);         // read the status register, sets attrib mode to index
    VGA_DELAY();
    POUTB(0x3c0, 0x10);
    VGA_DELAY();
    POUTB(0x3c0, 0xa1);
    VGA_DELAY();
    POUTB(0x3c0, 0x11);
    VGA_DELAY();
    POUTB(0x3c0, 0);
    VGA_DELAY();
    POUTB(0x3c0, 0x12);
    VGA_DELAY();
    POUTB(0x3c0, 0x0f);
    VGA_DELAY();
    POUTB(0x3c0, 0x13);
    VGA_DELAY();
    POUTB(0x3c0, 0x00);
    VGA_DELAY();
    POUTB(0x3c0, 0x14);
    VGA_DELAY();
    POUTB(0x3c0, 0x00);
    VGA_DELAY();

    PINB(0x3da);         // read the status register, set attrib mode to index
    POUTB(0x3c0, 0x20);  // re-enable the attribute controller
    VGA_DELAY();


    WriteSeqReg(0, 0x01); // shut down the sequencer
    VGA_DELAY();
    POUTB(0x3c2, 0xef);  // misc output
    VGA_DELAY();

    // re-program the sequencer clocks:

    WriteSeqReg(0x09, 0x60);
    WriteSeqReg(0x0a, 0x30);
    WriteSeqReg(0x0b, 0x4a);
    WriteSeqReg(0x0c, 0x5b);
    WriteSeqReg(0x0d, 0x42);
    WriteSeqReg(0x0e, 0x61);
    WriteSeqReg(0x0f, 0xb5);
    WriteSeqReg(0x10, 0x00);
    WriteSeqReg(0x11, 0x00);
    WriteSeqReg(0x12, 0x00);
    WriteSeqReg(0x13, 0x00);
    WriteSeqReg(0x14, 0x08);
    WriteSeqReg(0x15, 0x02);
    WriteSeqReg(0x16, 0x76);
    WriteSeqReg(0x17, 0x21);
    WriteSeqReg(0x18, 0x00);
    WriteSeqReg(0x19, 0x01);
    WriteSeqReg(0x1a, 0x00);
    WriteSeqReg(0x1b, 0x2b);
    WriteSeqReg(0x1c, 0x2f);
    WriteSeqReg(0x1d, 0x1f);
    WriteSeqReg(0x1e, 0x24);
    WriteSeqReg(0x1f, 0x20);

    // re-program the standard VGA registers:

    temp = ReadVgaReg(0x11);
    temp &= 0x7f;
    WriteVgaReg(0x11, temp);

    WriteVgaReg(0x00, 0xa1);
    WriteVgaReg(0x01, 0x7f);
    WriteVgaReg(0x02, 0x80);
    WriteVgaReg(0x03, 0x84);
    WriteVgaReg(0x04, 0x85);
    WriteVgaReg(0x05, 0x93); 
    WriteVgaReg(0x06, 0x2a); 
    WriteVgaReg(0x07, 0xfd); 
    WriteVgaReg(0x08, 0); 
    WriteVgaReg(0x09, 0x60); 
    WriteVgaReg(0x0a, 0); 
    WriteVgaReg(0x0b, 0); 
    WriteVgaReg(0x0c, 0); 
    WriteVgaReg(0x0d, 0); 
    WriteVgaReg(0x0e, 0); 
    WriteVgaReg(0x0f, 0); 
    WriteVgaReg(0x10, 0x12); 
    WriteVgaReg(0x11, 0x89); 
    WriteVgaReg(0x12, 0xff); 
    WriteVgaReg(0x13, 0x80);      // low 8 bits of pitch
    WriteVgaReg(0x14, 0x00); 
    WriteVgaReg(0x15, 0x00); 
    WriteVgaReg(0x16, 0x2a); 
    WriteVgaReg(0x17, 0xe3); 
    WriteVgaReg(0x18, 0xff); 
    WriteVgaReg(0x19, 0x4a); 
    WriteVgaReg(0x1a, 0x00); 
    WriteVgaReg(0x1b, 0x02);      // extended address wrap
    //WriteVgaReg(0x1c, 0x23); 
    
    WriteSeqReg(0, 0x03);
    WriteSeqReg(0x01, 0x01);
    WriteSeqReg(0x02, 0x0f);
    WriteSeqReg(0x03, 00);
    WriteSeqReg(0x04, 0x0e);      // Chain-4
    //WriteSeqReg(0x06, 0x12);      // 
    WriteSeqReg(0x07, 0x11);      // 

    WriteVidReg(0, 0);
    WriteVidReg(0x01, 0);
    WriteVidReg(0x02, 0);
    WriteVidReg(0x03, 0);
    WriteVidReg(0x04, 0);
    WriteVidReg(0x05, 0x40);
    WriteVidReg(0x06, 5);
    WriteVidReg(0x07, 0x0f);
    WriteVidReg(0x08, 0xff);
    WriteVidReg(0x09, 0x00);
    //WriteVidReg(0x0a, 0x00);
    WriteVidReg(0x0b, 0x20);
    WriteVidReg(0x31, 0x40);    // reset bit-blit

    // write the hidden DAC register:
    VGA_DELAY();
    PINB(0x3c6);
    PINB(0x3c6);
    PINB(0x3c6);
    PINB(0x3c6);
    POUTB(0x3c6, 0x00);
    
    VGA_DELAY();

    /* Take this next line out for a CL5436 chipset: */

    POUTB(0x3c6, 0xff);

    VGA_DELAY();

    PINB(0x3da);         // read the status register, set attrib mode to index
    POUTB(0x3c0, 0x20);  // re-enable the attribute controller
    VGA_DELAY();



   #if defined(HARDWARE_CURSOR) && defined(PEG_MOUSE_SUPPORT)
     
    // program the cursor bitmaps for hardware cursor support:

    WORD wRow, wCol;
    UCHAR uSrcVal;
    DWORD Plane0, Plane1;
    DWORD dMask = 0x80000000L;
   
    UCHAR *pPutCursBitmap = (UCHAR *) mpVidMemBase;
    pPutCursBitmap += VID_MEM_SIZE; // end of video memory
    pPutCursBitmap -= (16*1024);    // back up 16K Bytes

    PegPointer *pPointer = mpPointers;

    for (WORD wLoop = 0; wLoop <= NUM_POINTER_TYPES; wLoop++)
    {
        UCHAR *pPut = pPutCursBitmap;
        UCHAR *pPut1 = pPut + 128;
        PegBitmap *pPointerMap = pPointer->Bitmap;
        UCHAR *pGet = pPointerMap->pStart;

        for (wRow = 0; wRow < 32; wRow++)
        {
            Plane0 = Plane1 = 0;

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
                case WHITE:
                    Plane1 |= dMask;
                    break;      // leave these bits 0

                case TRANSPARENCY:
                    break;

                default:
                    Plane1 |= dMask;
                    Plane0 |= dMask;
                }
                dMask >>= 1;

                if (!dMask)
                {
                    *pPut++ = (UCHAR) (Plane0 >> 24);
                    *pPut++ = (UCHAR) (Plane0 >> 16);
                    *pPut++ = (UCHAR) (Plane0 >> 8);
                    *pPut++ = (UCHAR) Plane0;
                    *pPut1++ = (UCHAR) (Plane1 >> 24);
                    *pPut1++ = (UCHAR) (Plane1 >> 16);
                    *pPut1++ = (UCHAR) (Plane1 >> 8);
                    *pPut1++ = (UCHAR) Plane1;
                    Plane0 = Plane1 = 0;
                    dMask = 0x80000000L;
                }
            }
        }
        pPutCursBitmap += 256;
        pPointer++;
    }

    WriteSeqReg(0x12, 0x03);        // enable LUT 256 and 257
    POUTB(0x3c8, 0x00);             // select entry 256 (background)
    POUTB(0x3c9, 0xff);
    POUTB(0x3c9, 0xff);
    POUTB(0x3c9, 0xff);             // background is white
    POUTB(0x3c8, 0x0f);             // select entry 257 (foreground)
    POUTB(0x3c9, 0x00);
    POUTB(0x3c9, 0x00);
    POUTB(0x3c9, 0x00);             // foreground is black
    WriteSeqReg(0x13, 0);           // pointer type 0
    PegPoint sPoint;
    sPoint.x = mwHRes / 2;
    sPoint.y = mwVRes / 2;
    SetPointer(sPoint);
    WriteSeqReg(0x12, 0x01);        // turn on 32x32 hardware cursor

   #endif

    #if 0
    // for testing only, insure that memory is configured correctly:

    memset(mpScanPtrs[0], 0x0f, 1024);
    memset(mpScanPtrs[1], 0x02, 1024);
    memset(mpScanPtrs[2], 0x03, 1024);
    memset(mpScanPtrs[3], 0x04, 1024);
    memset(mpScanPtrs[5], 0x07, 1024);
    memset(mpScanPtrs[6], 0x08, 1024);
    memset(mpScanPtrs[7], 0x0a, 1024);

    #endif

    // clear the screen:

    HorizontalLine(0, mwHRes - 1, 0, 0, mwVRes);

    #ifdef DOUBLE_BUFFER
    mInvalid.Set(0, 0, mwHRes - 1, mwVRes - 1);
    miInvalidCount = 1;
    MemoryToScreen();
    miInvalidCount = 0;
    while(ReadVidReg(0x31) & 0x01)
    {
        gbPreventOptimize++;
    }
    #endif

}


/*--------------------------------------------------------------------------*/
void SVGAScreen::Configure640_480_256(void)
{
    int temp;
    WORD wLoop;

    PINB(0x3da);         // read the status register, set attrib mode to index
    VGA_DELAY();
    for (wLoop = 0; wLoop < 16; wLoop++)
    {
        VGA_DELAY();
        POUTB(0x3c0, wLoop);     // stop the attribute controller
        VGA_DELAY();
        POUTB(0x3c0, wLoop);
        VGA_DELAY();
    }
    VGA_DELAY();
    POUTB(0x3c0, wLoop);
    VGA_DELAY();
    POUTB(0x3c0, 0);
    VGA_DELAY();
    
    POUTB(0x3c8, 00);

    VGA_DELAY();
    PINB(0x3ba);
    VGA_DELAY();
    PINB(0x3da);         // read the status register, sets attrib mode to index
    VGA_DELAY();
    POUTB(0x3c0, 0x10);
    VGA_DELAY();
    POUTB(0x3c0, 0xa1);
    VGA_DELAY();
    POUTB(0x3c0, 0x11);
    VGA_DELAY();
    POUTB(0x3c0, 0);
    VGA_DELAY();
    POUTB(0x3c0, 0x12);
    VGA_DELAY();
    POUTB(0x3c0, 0x0f);
    VGA_DELAY();
    POUTB(0x3c0, 0x13);
    VGA_DELAY();
    POUTB(0x3c0, 0x00);
    VGA_DELAY();
    POUTB(0x3c0, 0x14);
    VGA_DELAY();
    POUTB(0x3c0, 0x00);
    VGA_DELAY();

    POUTB(0x3c0, 0x20);
    VGA_DELAY();

    WriteSeqReg(0, 0x01);
    VGA_DELAY();
    POUTB(0x3c2, 0xe3);  // misc output
    VGA_DELAY();

    temp = ReadVgaReg(0x11);
    temp &= 0x7f;
    WriteVgaReg(0x11, temp);

    WriteVgaReg(0x00, 0x5f);
    WriteVgaReg(0x01, 0x4f);
    WriteVgaReg(0x02, 0x50);
    WriteVgaReg(0x03, 0x82);
    WriteVgaReg(0x04, 0x54);
    WriteVgaReg(0x05, 0x80); 
    WriteVgaReg(0x06, 0x0b); 
    WriteVgaReg(0x07, 0x3e); 
    WriteVgaReg(0x08, 0); 
    WriteVgaReg(0x09, 0x40); 
    WriteVgaReg(0x0a, 0); 
    WriteVgaReg(0x0b, 0); 
    WriteVgaReg(0x0c, 0); 
    WriteVgaReg(0x0d, 0); 
    WriteVgaReg(0x0e, 0); 
    WriteVgaReg(0x0f, 0); 
    WriteVgaReg(0x10, 0xea); 
    WriteVgaReg(0x11, 0x8c); 
    WriteVgaReg(0x12, 0xdf); 
    WriteVgaReg(0x13, 0x50);      // low 8 bits of pitch
    WriteVgaReg(0x14, 0x00); 
    WriteVgaReg(0x15, 0xe7); 
    WriteVgaReg(0x16, 0x04); 
    WriteVgaReg(0x17, 0xe3); 
    WriteVgaReg(0x18, 0xff); 
    WriteVgaReg(0x1b, 0x02);      // extended address wrap
    
    WriteSeqReg(0, 0x03);
    WriteSeqReg(0x01, 0x01);
    WriteSeqReg(0x02, 0x0f);
    WriteSeqReg(0x03, 00);
    WriteSeqReg(0x04, 0x0e);      // Chain-4
    WriteSeqReg(0x07, 0x11);      // 
    WriteVidReg(0, 0);
    WriteVidReg(0x01, 0);
    WriteVidReg(0x02, 0);
    WriteVidReg(0x03, 0);
    WriteVidReg(0x04, 0);
    WriteVidReg(0x05, 0x40);
    WriteVidReg(0x06, 5);
    WriteVidReg(0x07, 0x0f);
    WriteVidReg(0x08, 0xff);

    // write the hidden DAC register:

    VGA_DELAY();
    PINB(0x3c6);
    PINB(0x3c6);
    PINB(0x3c6);
    PINB(0x3c6);
    POUTB(0x3c6, 0x00);
    
    VGA_DELAY();

    /* Take this next line out for a CL5436 chipset: */

    POUTB(0x3c6, 0xff);

    VGA_DELAY();

    #if 0

    // for testing only, insure that memory is configured correctly:

    memset(mpScanPtrs[0], 0x0f, 640);
    memset(mpScanPtrs[1], 0x02, 640);
    memset(mpScanPtrs[2], 0x03, 640);
    memset(mpScanPtrs[3], 0x04, 640);
    memset(mpScanPtrs[5], 0x07, 640);
    memset(mpScanPtrs[6], 0x08, 640);
    memset(mpScanPtrs[7], 0x0a, 640);

    #endif

    // clear the screen:

    HorizontalLine(0, mwHRes - 1, 0, 0, mwVRes);

    #ifdef DOUBLE_BUFFER
    mInvalid.Set(0, 0, mwHRes - 1, mwVRes - 1);
    miInvalidCount = 1;
    MemoryToScreen();
    miInvalidCount = 0;
    while(ReadVidReg(0x31) & 0x01)
    {
        gbPreventOptimize++;
    }
    #endif
}

/*--------------------------------------------------------------------------*/
void SVGAScreen::ConfigureSVGA(void)
{
    if (mwHRes == 640)
    {
        Configure640_480_256();
    }
    else
    {
        Configure1024_768_256();
    }
}

#ifdef DOUBLE_BUFFER

void SVGAScreen::MemoryToScreen(void)
{
    // perform a system-to-screen or screen-screen bitblit:

    if (!miInvalidCount)
    {
        return;
    }

    WORD wLoop, wLoop1;
    DWORD *pGet, *pPut, *pGetStart;

    PegRect Copy;
    Copy.Set(0, 0, mwHRes - 1, mwVRes - 1);
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

    WriteVidReg(0x20, iWidth - 1);
    WriteVidReg(0x21, (iWidth - 1) >> 8);
    WriteVidReg(0x22, iHeight - 1);
    WriteVidReg(0x23, (iHeight - 1) >> 8);
    WriteVidReg(0x24, mwHRes);
    WriteVidReg(0x25, mwHRes >> 8);

    #ifndef LOCAL_BUFFERING
    WriteVidReg(0x26, mwHRes);
    WriteVidReg(0x27, mwHRes >> 8);
    #endif

    DWORD dDest = iTop * mwHRes + iLeft;
    WriteVidReg(0x28, dDest);
    WriteVidReg(0x29, dDest >> 8);
    WriteVidReg(0x2a, dDest >> 16);

    #ifndef LOCAL_BUFFERING
    dDest += (DWORD) mwHRes * (DWORD) mwVRes;
    WriteVidReg(0x2c, dDest);
    WriteVidReg(0x2d, dDest >> 8);
    WriteVidReg(0x2e, dDest >> 16);
    WriteVidReg(0x30, 0);       // screen to screen
    #else
    WriteVidReg(0x30, 0x04);    // system to screen
    #endif

    WriteVidReg(0x32, 0x0d);    // src copy
    WriteVidReg(0x33, 0);
    WriteVidReg(0x31, 2);       // start going!

    #ifdef LOCAL_BUFFERING

    pGetStart = (DWORD *) (mpScanPointers[iTop] + iLeft);
    pPut = mpVidMemBase;

    for (wLoop = 0; wLoop < iHeight; wLoop++)
    {
        pGet = pGetStart;
        wLoop1 = iWidth / 4;
        
        while(wLoop1--)
        {
            *pPut++ = *pGet++;
        }
        pGetStart += mwHRes / 4;
    }

    #endif
}

#endif


