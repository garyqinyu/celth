/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1354scrn.cpp - PegScreen screen driver for Seiko-Epson 1354 video
//      controller using 256 colors.
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
// This driver has several build options. First, the driver is set up to 
// run in either an ISA or direct connect configuration. If running with an
// ISA configuration (like the SDU1354BOC eval card), a few things are 
// required that are not required when running in a direct connect environment
// such as an ARM, 68K, or similar environment. Choose the ISA environment
// by turning on the definition ISA_BOARD in the 1354scrn.hpp header file.
//
// If running with a custom direct-connect, you also need to set the base
// address for video memory and video register access. These are also defined
// in the header file.
//
//
// This driver can also be configured to use double buffering. Double buffering
// can be done in local memory or in video memory, again controlled with
// #defines.
//
//
// This driver is meant for use with 256 color systems only. The display can
// be LCD, CRT/TV or a combination. If you are using other than 256 colors,
// you should use the driver created for your color depth.
//
// Limitations:
// 
// This driver was developed and tested using the SED1354BOC eval card. It
// has been verified with both a 640x480 CRT and with a 320x240 LCD. All
// other configurations will likely require slight modification of the
// ConfigureController() function (toward the end of this file).
//
//                  ***     IMPORTANT     ***
//
// Please read the SED1354 documentation to insure that your controller is
// configured correctly for your display before calling Swell Software.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "string.h"     // for memcpy
#include "stdlib.h"     // for abs
#include "peg.hpp"

#if defined(EXTERNAL_RAMDAC)

// here if using an external 256-color RAMDAC

extern UCHAR DefPalette256[];

#else

const UCHAR LUTVALS[8*3] = {
    0x00, 0x00, 0x00,
    0x02, 0x02, 0x05,
    0x04, 0x04, 0x0a,
    0x06, 0x06, 0x0f,
    0x09, 0x09, 0x00,
    0x0b, 0x0b, 0x00,
    0x0d, 0x0d, 0x00,
    0x0f, 0x0f, 0x00
};

#endif


/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new SED1354Screen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/

SED1354Screen::SED1354Screen(PegRect &Rect) : PegScreen(Rect)
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

    ConfigureController();        // Configure Controller Registers

   #ifdef USE_VID_MEM_MANAGER

    // Calculate where the "free" video memory starts and ends, and
    // initialize the video memory manager.

    UCHAR *pStart = mpScanPointers[0];
    pStart += (DWORD) mwHRes * (DWORD) mwVRes;
    UCHAR *pEnd = mpVidMemBase + VID_MEM_SIZE - 1;

    InitVidMemManager(pStart, pEnd);

   #endif

    // SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// GetVideoAddress- Returns the base address of the video frame buffer.
//
// There are different forms of the GetVideoAddress function. These
// different forms are for different operating systems and system architechtures.
//
// When running the SDU1356BOC eval card in a PCI environment, we must
// find the card using PCI BIOS calls. If running in any other environment
// the base address is simply determined by a #define in the 1356 header file.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
// This version of GetVideoAddress() is for PharLap ETS, running with PCI bus:
/*--------------------------------------------------------------------------*/

UCHAR PEGFAR *SED1354Screen::GetVideoAddress(void)
{
    // Here for a direct connection. Use pre-defined addresses:

    UCHAR PEGFAR *pMem = (UCHAR PEGFAR *) VID_MEM_BASE;
    DWORD dSize = (DWORD) mwHRes * (DWORD) mwVRes;

    mpVidMemBase = pMem;

    #ifdef DOUBLE_BUFFER

     #ifdef LOCAL_BUFFERING
      pMem = new UCHAR[dSize];
     #else
      pMem += dSize;
     #endif
    #endif

    return (pMem);
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
SED1354Screen::~SED1354Screen()
{
    delete mpScanPointers;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1354Screen::BeginDraw(PegThing *)
{
    LOCK_PEG        // if multitasking, lock the screen
    if (!mwDrawNesting)
    {
       #if defined(PEG_MOUSE_SUPPORT)

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
void SED1354Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG        // if multitasking, lock the screen

    mpSaveScanPointers = mpScanPointers;

    WORD wWidth = pMap->wWidth;

    if (IS_SPRITE(pMap))        // is bitmap in video memory??
    {
        if (wWidth & 1)
        {
            wWidth++;
        }
    }

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        mpScanPointers = new UCHAR PEGFAR *[pMap->wHeight];
        UCHAR PEGFAR *CurrentPtr = pMap->pStart;
        for (SIGNED iLoop = 0; iLoop < pMap->wHeight; iLoop++)
        {
            mpScanPointers[iLoop] = CurrentPtr;
            CurrentPtr += wWidth;
        }
        mVirtualRect.Set(0, 0, pMap->wWidth - 1, pMap->wHeight - 1);
        mbVirtualDraw = TRUE;
    }
}

/*--------------------------------------------------------------------------*/
void SED1354Screen::EndDraw()
{
    mwDrawNesting--;

    if (!mwDrawNesting)
    {
       #if defined(PEG_MOUSE_SUPPORT)
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
void SED1354Screen::EndDraw(PegBitmap *pMap)
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
void SED1354Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void SED1354Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
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
void SED1354Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
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
void SED1354Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
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
void SED1354Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
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
void SED1354Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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
        Put += pMap->wWidth;

        GetStart += mwHRes;
    }
    Info->SetValid(TRUE);
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1354Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
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

            UCHAR PEGFAR *pSrc = Getmap->pStart;
            UCHAR PEGFAR *pDest = mpScanPointers[Where.y];
            pDest += Where.x;
            WORD wWidth = Getmap->wWidth;

            for (WORD wLoop = 0; wLoop < pMap->wHeight; wLoop++)
            {
                memcpy(pDest, pSrc, wWidth);
                pDest += mwVRes;
                pSrc += wWidth;
            }
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
        WORD wPitch = Getmap->wWidth;
        UCHAR uTransColor = (UCHAR) Getmap->dTransColor;

        Get += (View.wTop - Where.y) * wPitch;
        Get += View.wLeft - Where.x;

        if (HAS_TRANS(Getmap))
        {
            for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
            {
                UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
                for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
                {
                    UCHAR uVal = *Get++;
    
                    if (uVal != uTransColor)
                    {
                        *Put = uVal;
                    }
                    Put++;
                }
                Get += wPitch - View.Width();
            }
        }
        else
        {
            WORD wWidth = View.Width();
            for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
            {
                UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
                memcpy(Put, Get, wWidth);
                Get += wPitch;
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1354Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
	    UCHAR uTransColor = (UCHAR) Getmap->dTransColor;

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
	                        uVal != uTransColor)
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
	
	                if (uVal == uTransColor)
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
	                        wLoop1 <= View.wRight)
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
/*--------------------------------------------------------------------------*/
void SED1354Screen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void SED1354Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
// The 1354 driver supports only LUT mode. The palette registers are
// programmed linearly.
//
// This function MUST be modified if you are using an external RAMDAC.
//
/*--------------------------------------------------------------------------*/
void SED1354Screen::SetPalette(SIGNED, SIGNED, const UCHAR *)
{
    UCHAR *pRegs = (UCHAR *) VID_REG_BASE;
    const UCHAR *pGet;
    SIGNED Index;
    UCHAR *pPut = mPalette;

    *(pRegs + 0x2c) = 0;
    *(pRegs + 0x24) = 0;

   #ifdef EXTERNAL_RAMDAC

    // here to program the RAMDAC regs
    pGet = DefPalette256;

    for (Index = 0; Index < 256; Index++)
    {
        *(pRegs + 0x2e) = *pGet >> 2;    // program RED
        *pPut++ = *pGet++;
        *(pRegs + 0x2e) = *pGet >> 2;    // program GREEN
        *pPut++ = *pGet++;
        *(pRegs + 0x2e) = *pGet >> 2;    // program BLUE
        *pPut++ = *pGet++;
    }

   #else

    // here to program the LUT
    pGet = LUTVALS;

   	for (Index = 0; Index < 8; Index++)
	{
        *(pRegs + 0x26) = *pGet;    // program RED
        *pPut++ = *pGet++;
        *(pRegs + 0x26) = *pGet;    // program GREEN
        *pPut++ = *pGet++;
        *(pRegs + 0x26) = *pGet;    // program BLUE
        *pPut++ = *pGet++;
	}

   #endif

    *(pRegs + 0x27) = 0;


}

/*--------------------------------------------------------------------------*/
void SED1354Screen::HidePointer(void)
{
   #if defined(PEG_MOUSE_SUPPORT)
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
void SED1354Screen::SetPointer(PegPoint Where)
{
   #ifdef PEG_MOUSE_SUPPORT

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
   #endif   // MOUSE_SUPPORT ?
}

/*--------------------------------------------------------------------------*/
void SED1354Screen::SetPointerType(UCHAR bType)
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
   #endif   // MOUSE_SUPPORT?
}


/*--------------------------------------------------------------------------*/
void SED1354Screen::ResetPalette(void)
{
    SetPalette(0, 0, NULL);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *SED1354Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 232;
    return mPalette;
}


/*--------------------------------------------------------------------------*/
// There are currently two versions of the register configuration:
//
// 1) 640x480 CRT and
// 2) 320x240 LCD
//
// You will have to modify the register values for your display if it is
// neither of these types. Please refer to the SED1354 data sheets and
// register descriptions.
//
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
void SED1354Screen::ConfigureController(void)
{
    UCHAR *pRegs = (UCHAR *) VID_REG_BASE;
//	UCHAR *pVidMem = (UCHAR *) VID_MEM_BASE;

   #ifdef ISA_BOARD

    // When running in ISA environment, a write to location
    // 0xd00000 is required to set 16-bit ISA mode:

    UCHAR *pTemp = (UCHAR *) 0xd00000;
    *pTemp = 0;         // dummy write to set 16-bit mode
    
   #endif

    *(pRegs + 0x1b) = 0x00;     // enable the host interface
    *(pRegs + 0x23) = 0x80;     // disable the FIFO
    *(pRegs + 0x01) = 0x30;     // memory type 4ms refresh, EDO
    //*(pRegs + 0x01) = 0x20;     // 2 ms refresh, EDO
    //*(pRegs + 0x01) = 0x60;     // 32ms refresh, EDO
    //*(pRegs + 0x01)   = 0x70;     // 64ms refresh, EDO
    //*(pRegs + 0x22) = 0x24;     // performance enhancement
    //*(pRegs + 0x22) = 0x04;
    *(pRegs + 0x22) = 0x00;   // no performance enhancement
    *(pRegs + 0x02) = 0x1C;     // 8-bit, single panel, format 2
    *(pRegs + 0x03) = 0x00;     // FPLINE toggles every frame
    *(pRegs + 0x04) = (mwHRes / 8) - 1; // horizontal width

  #ifdef CRT_MODE
    *(pRegs + 0x05) = 0x1e;     // 75 Hz on CRT, see specs
    *(pRegs + 0x06) = 0x01;
    *(pRegs + 0x07) = 0x07;
  #else
    *(pRegs + 0x05) = 0x04;     // frame rate = 40 Hz on LCD
    *(pRegs + 0x06) = 0x00;
    *(pRegs + 0x07) = 0x00;
  #endif

    *(pRegs + 0x08) = (UCHAR) ((mwVRes - 1) & 0xff);
    *(pRegs + 0x09) = (UCHAR) ((mwVRes - 1) >> 8);


  #ifdef CRT_MODE
    *(pRegs + 0x0a) = 0x13;
    *(pRegs + 0x0b) = 0x00;
    *(pRegs + 0x0c) = 0x02;
    *(pRegs + 0x0e) = 0xff;
    *(pRegs + 0x0f) = 0x03;
  #else
    *(pRegs + 0x0a) = 0x01;
    *(pRegs + 0x0b) = 0x00;
    *(pRegs + 0x0c) = 0x00;
    *(pRegs + 0x0e) = 0xff;
    *(pRegs + 0x0f) = 0x03;
  #endif

    *(pRegs + 0x10) = 0x00;
    *(pRegs + 0x11) = 0x00;
    *(pRegs + 0x12) = 0x00;
    *(pRegs + 0x13) = 0x00;
    *(pRegs + 0x14) = 0x00;
    *(pRegs + 0x15) = 0x00;

    *(pRegs + 0x16) = (UCHAR) ((mwHRes / 2) & 0xff);
    *(pRegs + 0x17) = (UCHAR) ((mwHRes / 2) >> 8);

    *(pRegs + 0x18) = 0x00;

  #ifdef CRT_MODE
    *(pRegs + 0x19) = 0x00;
  #else
    *(pRegs + 0x19) = 0x06;
  #endif

    *(pRegs + 0x1a) = 0x00;

    // Misc I/O pins, default to all zero:

    *(pRegs + 0x1e) = 0x00;
    *(pRegs + 0x1f) = 0x00;
    *(pRegs + 0x20) = 0x00;
    *(pRegs + 0x21) = 0x00;


    // Turn on the display FIFO

    *(pRegs + 0x23) = 0x10;

  #ifdef CRT_MODE
   *(pRegs + 0x0d) = 0x0e;
  #else
   *(pRegs + 0x0d) = 0x0d;
  #endif

      ResetPalette();

     // clear the screen:

    HorizontalLine(0, mwHRes - 1, 0, 0, mwVRes);

    #ifdef DOUBLE_BUFFER

    mInvalid.Set(0, 0, mwHRes - 1, mwVRes - 1);
    miInvalidCount = 1;
    MemoryToScreen();
    miInvalidCount = 0;

    #endif
}

#ifdef DOUBLE_BUFFER

void SED1354Screen::MemoryToScreen(void)
{
    // perform a system-to-screen or screen-screen bitblit:

    if (!miInvalidCount)
    {
        return;
    }

    WORD wLoop, wLoop1;

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
    iWidth /= 4;            // for DWORD transfers


    SIGNED iAdjust = (mwHRes >> 2) - iWidth;

    SIGNED iHeight = Copy.Height();

    if (iHeight <= 0)
    {
        return;
    }

    DWORD *pSource = (DWORD *) mpScanPointers[iTop];
    pSource += iLeft / 4;
    DWORD *pDest = (DWORD *) mpVidMemBase + (pSource - (DWORD *) mpScanPointers[0]);

    for (wLoop = 0; wLoop < iHeight; wLoop++)
    {
        for (wLoop1 = 0; wLoop1 < iWidth; wLoop1++)
        {
            *pDest++ = *pSource++;
        }
        pDest += iAdjust;
        pSource += iAdjust;
    }
}

#endif


