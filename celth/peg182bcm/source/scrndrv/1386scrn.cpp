/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1386scr.cpp - PegScreen screen driver for Seiko-Epson 1386 video
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
// run in either a PCI or direct connect configuration. If running with a
// PCI configuration (like the SDU1386BOC eval card), we find the card
// using PCI protocol. If a direct connect (ColdFire, 68K, ARM, other),
// you must define the register and memory addresses.
//
// This driver can also be configured to use double buffering, hardware cursor,
// etc. Please read the comments above each configuration flag.
//
// This driver is meant for use with 256 color systems only. The display can
// be LCD, CRT/TV or a combination. If you are using other than 256 colors,
// you should use the driver created for your color depth.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "string.h"     // for memcpy
#include "stdlib.h"     // for abs
#include "peg.hpp"

#if defined(USE_PCI)
#include "pegpci.hpp"
#endif

extern UCHAR DefPalette256[];


/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/

#ifndef PEGWIN32

PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new SED1386Screen(Rect);
    return pScreen;
}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifdef PEGWIN32

// This is a HAL function to access the Win32 virtual device driver. This
// enables us to run the SDU1386 Eval card in a PC under Windows for
// development purposes only.

int IntelGetLinAddressW32(DWORD physaddr, DWORD *linaddr);

#endif

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/

#ifdef PEGWIN32
// Here if running the SDU1386 Eval card under Win32:
SED1386Screen::SED1386Screen(HWND hWnd, PegRect &Rect) : PegScreen(Rect)
#else
SED1386Screen::SED1386Screen(PegRect &Rect) : PegScreen(Rect)
#endif
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
    UCHAR *pStart = mpScanPointers[0];
    pStart += (DWORD) mwHRes * (DWORD) mwVRes;
    UCHAR *pEnd = mpVidMemBase + VID_MEM_SIZE - 1;

    #ifdef HARDWARE_CURSOR
     pEnd -= (NUM_POINTER_TYPES + 1) * 1024;
    #endif

    InitVidMemManager(pStart, pEnd);
   #endif

    SetPalette(0, 232, DefPalette256);

   #ifdef PEGWIN32

   // This section of code is only needed for running the SDU1386 eval
   // card in a Win32 environment. For development ONLY!

   RECT lSize;
   ::GetClientRect(hWnd, &lSize);

   mwWinRectXOffset = (lSize.right - mwHRes) / 2;
   mwWinRectYOffset = (lSize.bottom -mwVRes) / 2;

   #endif       // running in Win32 development environment?

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// GetVideoAddress- Returns the base address of the video frame buffer.
//
// There are different forms of the GetVideoAddress function. These
// different forms are for different operating systems and system architechtures.
//
// When running the SDU1386BOC eval card in a PCI environment, we must
// find the card using PCI BIOS calls. If running in any other environment
// the base address is simply determined by a #define in the 1386 header file.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

UCHAR PEGFAR *SED1386Screen::GetVideoAddress(void)
{
    UCHAR* pMem = NULL;

   #if defined(USE_PCI)

    pMem = PegPCIGetVideoAddress(PEGPCI_VGACLASS, PEGPCI_VENDORID, 
                                 PEGPCI_DEVICEID);
    if(pMem)
    {
        mpVidMemBase = pMem + 0x200000;     // offset 2M for PCI mem
        mpVidRegBase = pMem;
        mpBitBlitRegs = pMem + 0x100000;    // offset 1M for PCI bitblt
        pMem = mpVidMemBase;
    }
   
   #elif defined(PEGWIN32)

    // Here for using the Win32 device driver:
    DWORD dLinAddr;

    IntelGetLinAddressW32(0, &dLinAddr);
    pMem = (UCHAR *) dLinAddr;
    mpVidMemBase = pMem + 0x200000;     // offset 2M for PCI mem
    mpVidRegBase = pMem;
    mpBitBlitRegs = pMem + 0x100000;    // offset 1M for PCI bitblt
    pMem = mpVidMemBase;

   #else


    // Here for a direct connection. Use pre-defined addresses:

    pMem = (UCHAR *) VID_MEM_BASE;
    mpVidMemBase = pMem;
    mpVidRegBase = (UCHAR *) VID_REG_BASE;
    mpBitBlitRegs = (UCHAR *) BITBLT_REG_BASE;

   #endif

    mpVidMemBase = pMem;

    #ifdef DOUBLE_BUFFER
     #ifdef LOCAL_BUFFERING
      pMem = new UCHAR[mwHRes * mwVRes];
     #else
      pMem += (DWORD) mwHRes * (DWORD) mwVRes;
     #endif
    #endif

    return (pMem);
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
SED1386Screen::~SED1386Screen()
{
    delete mpScanPointers;
}

UCHAR gbPreventOptimize;

#define BLIT_CHECK\
    while(*(mpVidRegBase + REG_BITBLT_CTRL0) & 0x80)\
    {\
        gbPreventOptimize++;\
    }\
    gbPreventOptimize = *mpBitBlitRegs; \



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1386Screen::BeginDraw(PegThing *)
{
    LOCK_PEG        // if multitasking, lock the screen
    if (!mwDrawNesting)
    {
       #if defined(DOUBLE_BUFFER) && !defined(LOCAL_BUFFERING)
        // make sure bit-blit engine is done:

        BLIT_CHECK
        
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
void SED1386Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG        // if multitasking, lock the screen

    #if defined(DOUBLE_BUFFER) && !defined(LOCAL_BUFFERING)
    // make sure bit-blit engine is done:

    BLIT_CHECK

    #endif

    mpSaveScanPointers = mpScanPointers;

    WORD wWidth = pMap->wWidth;

    if (IS_SPRITE(pMap))
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
void SED1386Screen::EndDraw()
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
void SED1386Screen::EndDraw(PegBitmap *pMap)
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
// CreateBitmap: The default version creates an 8-bpp bitmap.
/*--------------------------------------------------------------------------*/
PegBitmap *SED1386Screen::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
{
   #ifdef USE_VID_MEM_MANAGER

    // A BUG IN THE 1386 BITBLT ENGINE. If the bitmap width is odd,
    // we must add a padding byte:

    PegBitmap *pMap;
    if (wWidth & 1)
    {
        pMap = PegScreen::CreateBitmap(wWidth + 1, wHeight, bHasTrans);
        pMap->wWidth = wWidth;
    }
    else
    {
        pMap = PegScreen::CreateBitmap(wWidth, wHeight, bHasTrans);
    }
    return pMap;
   #else
    return PegScreen::CreateBitmap(wWidth, wHeight, bHasTrans);
   #endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1386Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void SED1386Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
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
void SED1386Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
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
void SED1386Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
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
void SED1386Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
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
void SED1386Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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

       #ifdef MEMCPY_NOT_SUPPORTED

        WORD wCount = pMap->wWidth;
        while(wCount--)
        {
            *Put++ = *Get++;
        }
       #else

        memcpy(Put, Get, pMap->wWidth);
        Put += pMap->wWidth;

       #endif

        GetStart += mwHRes;
    }
    Info->SetValid(TRUE);
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1386Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
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
            WORD wWidth = Getmap->wWidth;
            wWidth--;

            WORD wHeight = Getmap->wHeight - 1;

            BLIT_CHECK

            *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR0) = (UCHAR) dSource;
            *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR1) = (UCHAR) (dSource >> 8);
            *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR2) = (UCHAR) (dSource >> 16);
            *(mpVidRegBase + REG_BITBLT_DEST_START_ADDR0) = (UCHAR) dDest;
            *(mpVidRegBase + REG_BITBLT_DEST_START_ADDR1) = (UCHAR) (dDest >> 8);
            *(mpVidRegBase + REG_BITBLT_DEST_START_ADDR2) = (UCHAR) (dDest >> 16);

            *(mpVidRegBase + REG_BITBLT_WIDTH0) = (UCHAR) wWidth;
            *(mpVidRegBase + REG_BITBLT_WIDTH1) = (UCHAR) (wWidth >> 8);
            *(mpVidRegBase + REG_BITBLT_HEIGHT0) = (UCHAR) wHeight;
            *(mpVidRegBase + REG_BITBLT_HEIGHT1) = (UCHAR) (wHeight >> 8);

            if (HAS_TRANS(Getmap))
            {
                *(mpVidRegBase + REG_BITBLT_BACKGND_COLOR0) = (UCHAR) Getmap->dTransColor; 
                *(mpVidRegBase + REG_BITBLT_OPERATION) = 0x05;
            }
            else
            {
                *(mpVidRegBase + REG_BITBLT_OPERATION) = 0x02;
            }
            *(mpVidRegBase + REG_BITBLT_ROP_CODE) = 0x0c;
            *(mpVidRegBase + REG_BITBLT_CTRL0) = 0x81; // go!!

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

       #ifdef USE_VID_MEM_MANAGER
        if (IS_SPRITE(Getmap))
        {
            if (wPitch & 1)
            {
                wPitch++;
            }
        }
       #endif

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
    
                    if (uVal != Getmap->dTransColor)
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

               #ifdef MEMCPY_NOT_SUPPORTED

                WORD wCount = wWidth;
                while(wCount--)
                {
                    *Put++ = *Get++;
                }
                Get -= wWidth;

               #else

                memcpy(Put, Get, wWidth);

               #endif

                Get += wPitch;
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1386Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
void SED1386Screen::RectMoveView(PegThing *Caller, const PegRect &View,
     const SIGNED xMove, const SIGNED yMove)
{
    PegCapture tCapture;
    PegRect CaptureRect = View;
    Capture(&tCapture, CaptureRect);
    tCapture.Shift(xMove, yMove);
    Bitmap(Caller, tCapture.Point(), tCapture.Bitmap(), TRUE);

#if 0
    // use the bit-blit engine to do a fast rectmove:
    WORD wHeight = View.Height() - 1;
    WORD wWidth = View.Width() - 1;

    if (yMove >= 0)
    {
        wHeight -= yMove;
    }
    else
    {
        wHeight += yMove;
    }

    if (xMove >= 0)
    {
        wWidth -= xMove;
    }
    else
    {
        wWidth += xMove;
    }

    DWORD dSource = (DWORD) mpScanPointers[View.wTop] - (DWORD) mpVidMemBase;
    dSource += View.wLeft;
    DWORD dDest = (DWORD) mpScanPointers[View.wTop + yMove] - (DWORD) mpVidMemBase;
    dDest += View.wLeft + xMove;

    BLIT_CHECK

    *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR0) = (UCHAR) dSource;
    *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR1) = (UCHAR) (dSource >> 8);
    *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR2) = (UCHAR) (dSource >> 16);
    *(mpVidRegBase + REG_BITBLT_DEST_START_ADDR0) = (UCHAR) dDest;
    *(mpVidRegBase + REG_BITBLT_DEST_START_ADDR1) = (UCHAR) (dDest >> 8);
    *(mpVidRegBase + REG_BITBLT_DEST_START_ADDR2) = (UCHAR) (dDest >> 16);

    *(mpVidRegBase + REG_BITBLT_WIDTH0) = (UCHAR) wWidth;
    *(mpVidRegBase + REG_BITBLT_WIDTH1) = (UCHAR) (wWidth >> 8);
    *(mpVidRegBase + REG_BITBLT_HEIGHT0) = (UCHAR) wHeight;
    *(mpVidRegBase + REG_BITBLT_HEIGHT1) = (UCHAR) (wHeight >> 8);

    if (dDest <= dSource)
    {
        *(mpVidRegBase + REG_BITBLT_OPERATION) = 0x02;
    }
    else
    {
        *(mpVidRegBase + REG_BITBLT_OPERATION) = 0x03;
    }
    *(mpVidRegBase + REG_BITBLT_ROP_CODE) = 0x0c;
    *(mpVidRegBase + REG_BITBLT_CTRL0) = 0x80; // go!!
#endif

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1386Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
/*--------------------------------------------------------------------------*/
void SED1386Screen::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get)
{
    UCHAR *pRegs = mpVidRegBase;
    const UCHAR *pPal;
    UCHAR rgb;
    SIGNED Index;

    // Initialize the LCD Palette:

	*(pRegs + REG_LUT_MODE) = 0x01;	     // Enable the LCD LUT for read/write.
	*(pRegs + REG_LUT_ADDR) = (UCHAR) iFirst;    // Reset the LUT address.

    pPal = Get;

	for (Index = 0; Index < iNum; Index++)
	{
		for (rgb = 0; rgb < 3; rgb++)
		{
			*(pRegs + REG_LUT_DATA) = *pPal++;
		}
	}

    // Initialize the CRT Palette:

	*(pRegs + REG_LUT_MODE) = 0x00;	// Enable the CRT LUT for read/write.
	*(pRegs + REG_LUT_ADDR) = (UCHAR) iFirst;	// Reset the LUT address.

    pPal = Get;

	for (Index = 0; Index < iNum; Index++)
	{
		for (rgb = 0; rgb < 3; rgb++)
		{
			*(pRegs + REG_LUT_DATA) = *pPal++;
		}
	}

    UCHAR *pPut = mPalette;
    pPut += iFirst * 3;
    memcpy(pPut, Get, iNum * 3); // keep local copy


}


/*--------------------------------------------------------------------------*/
void SED1386Screen::ShowPointer(BOOL bShow)
{
    UCHAR *pRegs = mpVidRegBase;

    #if defined(HARDWARE_CURSOR)
    if (bShow)
    {
        // turn on the hardware cursor:

        #ifdef LCD_CURSOR
        *(pRegs + REG_LCD_INK_CURS_CTRL) = 0x01;  
        #endif

        #ifdef CRT_CURSOR
        *(pRegs + REG_CRTTV_INK_CURS_CTRL) = 0x01;  
        #endif
    }
    else
    {
        // turn off the hardware cursor:

        #ifdef LCD_CURSOR
        *(pRegs + REG_LCD_INK_CURS_CTRL) = 0x00;  
        #endif

        #ifdef CRT_CURSOR
        *(pRegs + REG_CRTTV_INK_CURS_CTRL) = 0x00;  
        #endif
    }
    #endif
    PegScreen::ShowPointer(bShow);
}
 
/*--------------------------------------------------------------------------*/
void SED1386Screen::HidePointer(void)
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
void SED1386Screen::SetPointer(PegPoint Where)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (mbShowPointer)
    {
	   #ifdef HARDWARE_CURSOR
	    mLastPointerPos = Where;
	    Where.x -= miCurXOffset;
	    Where.y -= miCurYOffset;
	
	    #ifdef LCD_CURSOR
	    *(mpVidRegBase + REG_LCD_CURSOR_X_POS0) = (UCHAR) Where.x;
	    *(mpVidRegBase + REG_LCD_CURSOR_X_POS1) = (UCHAR) (Where.x >> 8);
	    *(mpVidRegBase + REG_LCD_CURSOR_Y_POS0) = (UCHAR) Where.y;
	    *(mpVidRegBase + REG_LCD_CURSOR_Y_POS1) = (UCHAR) (Where.y >> 8);
	    #endif
	
	    #ifdef CRT_CURSOR
	    *(mpVidRegBase + REG_CRTTV_CURSOR_X_POS0) = (UCHAR) Where.x;
	    *(mpVidRegBase + REG_CRTTV_CURSOR_X_POS1) = (UCHAR) (Where.x >> 8);
	    *(mpVidRegBase + REG_CRTTV_CURSOR_Y_POS0) = (UCHAR) Where.y;
	    *(mpVidRegBase + REG_CRTTV_CURSOR_Y_POS1) = (UCHAR) (Where.y >> 8);
	    #endif
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
void SED1386Screen::SetPointerType(UCHAR bType)
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

        // copy the correct bitmap into last 1K of video memory:

        UCHAR *pPut = mpVidMemBase;
        pPut += VID_MEM_SIZE;
        pPut -= 1024;
        UCHAR *pGet = pPut;
        pGet -= ((NUM_POINTER_TYPES - bType) << 10);

       #ifdef MEMCPY_NOT_SUPPORTED

        WORD wCount = 1024;
        while(wCount--)
        {
            *pPut++ = *pGet++;
        }
       #else

        memcpy(pPut, pGet, 1024);

       #endif

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
void SED1386Screen::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *SED1386Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 232;
    return mPalette;
}


#include "1386regs.h"       // pull in HAL_INFO structure

/*--------------------------------------------------------------------------*/
void SED1386Screen::ConfigureController(void)
{
    #if defined(USE_PCI) || defined(PEGWIN32)
     UCHAR *pRegs = mpVidRegBase;
     UCHAR *pVidMem = mpVidMemBase;
    #else    
	 UCHAR *pRegs = (UCHAR *) VID_REG_BASE;
	 UCHAR *pVidMem = (UCHAR *) VID_MEM_BASE;
    #endif

    SED1386RegEntry *pGetReg = &HalInfo.rInitVals[0];

	SIGNED Index;
	SIGNED rgb;
    UCHAR *pPal;

    while(pGetReg->wIndex != FINISHED_REG_CFG)
    {
    	*(pRegs + pGetReg->wIndex) = pGetReg->uVal; // program the registers
        pGetReg++;
    }

    // Initialize the LCD Palette:

	*(pRegs + REG_LUT_MODE) = 0x01;	// Enable the LCD LUT for read/write.
	*(pRegs + REG_LUT_ADDR) = 0;    // Reset the LUT address.

    pPal = DefPalette256;
	for (Index = 0; Index < 256; Index++)
	{
		for (rgb = 0; rgb < 3; rgb++)
		{
			*(pRegs + REG_LUT_DATA) = *pPal++;
		}
	}

    // Initialize the CRT Palette:

	*(pRegs + REG_LUT_MODE) = 0x00;	// Enable the CRT LUT for read/write.
	*(pRegs + REG_LUT_ADDR) = 0;	// Reset the LUT address.

    pPal = DefPalette256;
	for (Index = 0; Index < 256; Index++)
	{
		for (rgb = 0; rgb < 3; rgb++)
		{
			*(pRegs + REG_LUT_DATA) = *pPal++;
		}
	}

    // program a few bitblit regs that never change:

    *(pRegs + REG_BITBLT_CTRL1) = 0;
    *(pRegs + REG_BITBLT_MEM_ADDR_OFFSET0) = (UCHAR) (mwHRes / 2);  
    *(pRegs + REG_BITBLT_MEM_ADDR_OFFSET1) = (UCHAR) ((mwHRes / 2) >> 8);  


   #ifdef HARDWARE_CURSOR


    /*     
      Program the cursor bitmaps for hardware cursor support. We leave the
      last 1K of video memory free, and place all cursor bitmaps into the
      preceding section of video memory, moving in 1K increments. Since the
      1386 only allows 8K bitmaps, we will copy the current or active cursor
      bitmap into the last 1K each time the cursor to be displayed is changed.
      This method saves 7K of video memory for every cursor bitmap, but it
      also requires us to do a 1K copy whenever the cursor shape is changed.
    */

    WORD wRow, wCol;
    UCHAR uSrcVal;
    UCHAR uDestVal = 0;
    UCHAR uMask = 0xc0;
   
    UCHAR *pPutCursBitmap = (UCHAR *) mpVidMemBase;
    pPutCursBitmap += VID_MEM_SIZE; // end of video memory
    pPutCursBitmap -= 1024;    // back up 1K Byte for active cursor
    pPutCursBitmap -= (NUM_POINTER_TYPES * 1024);

    PegPointer *pPointer = mpPointers;

    for (WORD wLoop = 0; wLoop <= NUM_POINTER_TYPES; wLoop++)
    {
        UCHAR *pPut = pPutCursBitmap;
        PegBitmap *pPointerMap = pPointer->Bitmap;
        UCHAR *pGet = pPointerMap->pStart;

        for (wRow = 0; wRow < 64; wRow++)
        {
            for (wCol =0 ; wCol < 64; wCol++)
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
                    uDestVal |= 0x55 & uMask;
                    break; 

                case TRANSPARENCY:
                    uDestVal |= 0xaa & uMask;
                    break;

                default:
                    break;
                }
                uMask >>= 2;

                if (!uMask)
                {
                    *pPut++ = uDestVal;
                    uMask = 0xc0;
                    uDestVal = 0;
                }
            }
        }
        pPutCursBitmap += 1024;
        pPointer++;
    }

    // Set the default cursor:

    PegPoint Pos;
    Pos.x = mwHRes / 2;
    Pos.y = mwVRes / 2;
    SetPointer(Pos);
    SetPointerType(PPT_NORMAL);

    // set color 0 to black:

    #ifdef LCD_CURSOR
    *(pRegs + REG_LCD_INK_CURS_BLUE0) = 0;
    *(pRegs + REG_LCD_INK_CURS_GREEN0) = 0;
    *(pRegs + REG_LCD_INK_CURS_RED0) = 0;
    #endif

    #ifdef CRT_CURSOR
    *(pRegs + REG_CRTTV_INK_CURS_BLUE0) = 0;
    *(pRegs + REG_CRTTV_INK_CURS_GREEN0) = 0;
    *(pRegs + REG_CRTTV_INK_CURS_RED0) = 0;
    #endif
    
    // set color 1 to white:
    
    #ifdef LCD_CURSOR
    *(pRegs + REG_LCD_INK_CURS_BLUE1) = 0xff;
    *(pRegs + REG_LCD_INK_CURS_GREEN1) = 0xff;
    *(pRegs + REG_LCD_INK_CURS_RED1) = 0xff;
    #endif

    #ifdef CRT_CURSOR
    *(pRegs + REG_CRTTV_INK_CURS_BLUE1) = 0xff;
    *(pRegs + REG_CRTTV_INK_CURS_GREEN1) = 0xff;
    *(pRegs + REG_CRTTV_INK_CURS_RED1) = 0xff;
    #endif

    // set the cursor address:

    *(pRegs + REG_LCD_INK_CURS_START_ADDR) = 0;  
    *(pRegs + REG_CRTTV_INK_CURS_START_ADDR) = 0;  


    // turn on the hardware cursor:

    #ifdef LCD_CURSOR
    *(pRegs + REG_LCD_INK_CURS_CTRL) = 0x01;  
    #endif

    #ifdef CRT_CURSOR
    *(pRegs + REG_CRTTV_INK_CURS_CTRL) = 0x01;  
    #endif

   #endif

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

void SED1386Screen::MemoryToScreen(void)
{
    // perform a system-to-screen or screen-screen bitblit:

    if (!miInvalidCount)
    {
        return;
    }

   #ifdef LOCAL_BUFFERING
    WORD wLoop, wLoop1;
    WORD *pGet, *pPut, *pGetStart;
   #endif

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

    // use the bit-blit engine to do a fast rectmove:

   #ifdef LOCAL_BUFFERING
    DWORD dSource = (DWORD) mpScanPointers[iTop] - (DWORD) mpScanPointers[0];
    dSource += iLeft;
    DWORD dDest = mpVidMemBase + dSource;
    BLIT_CHECK
    *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR0) = 0;
    *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR1) = 0;
    *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR2) = 0;
    
   #else
    DWORD dSource = (DWORD) mpScanPointers[iTop] - (DWORD) mpVidMemBase;
    dSource += iLeft;
    DWORD dDest = dSource - (mwHRes * mwVRes);
    BLIT_CHECK
    *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR0) = (UCHAR) dSource;
    *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR1) = (UCHAR) (dSource >> 8);
    *(mpVidRegBase + REG_BITBLT_SRC_START_ADDR2) = (UCHAR) (dSource >> 16);
   #endif


    *(mpVidRegBase + REG_BITBLT_DEST_START_ADDR0) = (UCHAR) dDest;
    *(mpVidRegBase + REG_BITBLT_DEST_START_ADDR1) = (UCHAR) (dDest >> 8);
    *(mpVidRegBase + REG_BITBLT_DEST_START_ADDR2) = (UCHAR) (dDest >> 16);

    *(mpVidRegBase + REG_BITBLT_WIDTH0) = (UCHAR) iWidth - 1;
    *(mpVidRegBase + REG_BITBLT_WIDTH1) = (UCHAR) ((iWidth - 1) >> 8);
    *(mpVidRegBase + REG_BITBLT_HEIGHT0) = (UCHAR) iHeight - 1;
    *(mpVidRegBase + REG_BITBLT_HEIGHT1) = (UCHAR) ((iHeight - 1) >> 8);

   #ifndef LOCAL_BUFFERING
    if (dDest <= dSource)
    {
        *(mpVidRegBase + REG_BITBLT_OPERATION) = 0x02;
    }
    else
    {
        *(mpVidRegBase + REG_BITBLT_OPERATION) = 0x03;
    }
    *(mpVidRegBase + REG_BITBLT_ROP_CODE) = 0x0c;
    *(mpVidRegBase + REG_BITBLT_CTRL0) = 0x80; // go!!

   #else

    pGetStart = (WORD *) dSource;
    pPut = mpBitBlitRegs;
    WORD wCount = 0;

    for (wLoop = 0; wLoop < iHeight; wLoop++)
    {
        pGet = pGetStart;
        wLoop1 = iWidth / 2;
        
        while(wLoop1--)
        {
            *pPut = *pGet++;
            wCount++;

            if (wCount == 16)       // 16 data written??
            {
                // wait for the fifo to empty
                while(*(mpVidRegBase + REG_BITBLT_CTRL0) & 0x40)
                {
                    gbPreventOptimize++;                    
                }
                wCount = 0;
            }
        }
        pGetStart += mwHRes / 2;
    }

    #endif
}

#endif


