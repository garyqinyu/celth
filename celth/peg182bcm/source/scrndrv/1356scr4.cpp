/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1356scr4.cpp - PegScreen driver template for 4 bit-per-pixel
//      linear frame buffer access. This driver can be configured
//      to work with any screen resolution, simply by passing in the correct
//      rectangle to the constructor. 
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2000 Swell Software 
//              All Rights Reserved.
//
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// This driver has several build options. First, the driver is set up to 
// run in either a PCI or direct connect configuration. If running with a
// PCI configuration (like the SDU1356BOC eval card), we find the card
// using PCI protocol. If a direct connect (ColdFire, 68K, ARM, other),
// you must define the register and memory addresses.
//
// This driver can also be configured to use double buffering, hardware cursor,
// etc. Please read the comments above each configuration flag.
//
// This driver is meant for use with 16 color/gray systems only. The display
// can be LCD, CRT/TV or a combination. If you are using other than 16 
// colors, you should use the driver created for your color depth.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"

#if defined(USE_PCI)
#include "pegpci.hpp"
#endif


#ifdef GRAYSCALE

UCHAR GrayPalette[16 * 3] = {
      0,   0,   0,
     17,  17,  17,
     34,  34,  34,
     51,  51,  51,
     68,  68,  68,
     85,  85,  85,
    102, 102, 102,
    119, 119, 119,
    136, 136, 136,
    153, 153, 153,
    170, 170, 170,
    187, 187, 187,
    204, 204, 204,
    221, 221, 221,
    238, 238, 238,
    255, 255, 255
};

#else

extern UCHAR DefPalette256[];

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/

#ifndef PEGWIN32
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new SED1356Screen(Rect);
    return pScreen;
}
#endif

/*--------------------------------------------------------------------------*/
// Information read from PCI configuration registers:
/*--------------------------------------------------------------------------*/

#ifdef USE_PCI
PCI_CONFIG PciConfig;
#endif

#ifdef PEGWIN32

// This is a HAL function to access the Win32 virtual device driver. This
// enables us to run the SDU1356 Eval card in a PC under Windows for
// development purposes only.

int IntelGetLinAddressW32(DWORD physaddr, DWORD *linaddr);
#endif

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
SED1356Screen::SED1356Screen(PegRect &Rect) : PegScreen(Rect)
{
    mdNumColors = 16;  

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();

    mpScanPointers = new UCHAR PEGFAR *[Rect.Height()];

    UCHAR PEGFAR *CurrentPtr = GetVideoAddress();
    WORD wPitch = mwHRes >> 1;

    for (SIGNED iLoop = 0; iLoop < Rect.Height(); iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += wPitch;
    }

    mLastPointerPos.x = Rect.Width() / 2;
    mLastPointerPos.y = Rect.Height() / 2;
    mbPointerHidden = FALSE;
    mwDrawNesting = 0;

    ConfigureController();        // set up controller registers

   #ifdef USE_VID_MEM_MANAGER

    UCHAR *pStart = mpScanPointers[0];
    pStart += (DWORD) mwVRes * (DWORD) wPitch;
    UCHAR *pEnd = mpVidMemBase + VID_MEM_SIZE - 1;

    #ifdef HARDWARE_CURSOR
     pEnd -= (NUM_POINTER_TYPES + 1) * 1024;
    #endif

    InitVidMemManager(pStart, pEnd);
   #endif

    #ifdef GRAYSCALE
    SetPalette(0, 16, GrayPalette);
    #else
    SetPalette(0, 16, DefPalette256);
    #endif
}


/*--------------------------------------------------------------------------*/
// This version of GetVideoAddress() is for PharLap ETS, running with PCI bus:
/*--------------------------------------------------------------------------*/

#ifdef USE_PCI
#if defined(PHARLAP)
#include "embpci.h"         // include PHARLAP PCI functions
#endif
#endif

UCHAR PEGFAR *SED1356Screen::GetVideoAddress(void)
{
   #if defined(USE_PCI)

    UCHAR *pMem = PegPCIGetVideoAddress(PEGPCI_VGACLASS, PEGPCI_VENDORID,
                                        PEGPCI_DEVICEID);
    mpVidMemBase = pMem + 0x200000;     // offset 2M for PCI mem
    mpVidRegBase = pMem;
    mpBitBlitRegs = pMem + 0x100000;    // offset 1M for PCI bitblt
    pMem = mpVidMemBase;
   
   #elif defined(PEGWIN32)

    // Here for using the Win32 device driver:
    DWORD dLinAddr;

    IntelGetLinAddressW32(0, &dLinAddr);
    UCHAR *pMem = (UCHAR *) dLinAddr;
    mpVidMemBase = pMem + 0x200000;     // offset 2M for PCI mem
    mpVidRegBase = pMem;
    mpBitBlitRegs = pMem + 0x100000;    // offset 1M for PCI bitblt
    pMem = mpVidMemBase;

   #else


    // Here for a direct connection. Use pre-defined addresses:

    UCHAR *pMem = (UCHAR *) VID_MEM_BASE;
    mpVidMemBase = pMem;
    mpVidRegBase = (UCHAR *) VID_REG_BASE;
    mpBitBlitRegs = (UCHAR *) BITBLT_REG_BASE;

   #endif

    mpVidMemBase = pMem;

    #ifdef DOUBLE_BUFFER
     #ifdef LOCAL_BUFFERING
      pMem = new UCHAR[(mwHRes >> 1) * mwVRes];
     #else
      pMem += (DWORD) (mwHRes >> 1) * (DWORD) mwVRes;
     #endif
    #endif

    return (pMem);
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
SED1356Screen::~SED1356Screen()
{
    delete mpScanPointers;
}

UCHAR gbPreventOptimize;

#define BLIT_CHECK\
	while (*(mpVidRegBase + REG_BITBLT_CTRL0) & 0x80 == 0x00)\
    {\
        gbPreventOptimize++;\
    }\


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1356Screen::BeginDraw(PegThing *)
{
    LOCK_PEG
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
void SED1356Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG

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
        WORD wBytesPerLine = (pMap->wWidth + 1) >> 1;

        mpScanPointers = new UCHAR PEGFAR *[pMap->wHeight];
        UCHAR PEGFAR *CurrentPtr = pMap->pStart;
        for (SIGNED iLoop = 0; iLoop < pMap->wHeight; iLoop++)
        {
            mpScanPointers[iLoop] = CurrentPtr;
            CurrentPtr += wBytesPerLine;
        }
        mVirtualRect.Set(0, 0, pMap->wWidth - 1, pMap->wHeight - 1);
        mbVirtualDraw = TRUE;
    }
}

/*--------------------------------------------------------------------------*/
void SED1356Screen::EndDraw()
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
void SED1356Screen::EndDraw(PegBitmap *pMap)
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
PegBitmap *SED1356Screen::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
{
   PegBitmap *pMap = NULL;

    if (wWidth && wHeight)
    {
        pMap = new PegBitmap;
        pMap->wWidth = wWidth;
        pMap->wHeight = wHeight;
        wWidth++;
        DWORD dSize = (DWORD) (wWidth >> 1) * wHeight;

       #ifdef USE_VID_MEM_MANAGER
        pMap->pStart = AllocBitmap(dSize);

        if (!pMap->pStart)  // out of video memory?
        {
            pMap->pStart = new UCHAR[dSize];    // try local memory
            pMap->uFlags = 0;
        }
        else
        {
            pMap->uFlags = BMF_SPRITE;
        }
       #else
        pMap->pStart = new UCHAR[dSize];    // try local memory
        pMap->uFlags = 0;
       #endif

        if (!pMap->pStart)
        {
            delete pMap;
            return NULL;
        }

		if(bHasTrans)
		{
			pMap->uFlags |= BMF_HAS_TRANS;
			memset(pMap->pStart, TRANSPARENCY, (DWORD) wWidth * (DWORD) wHeight);
		}
		else
		{
			// fill the whole thing with BLACK:
			memset(pMap->pStart, BLACK, (DWORD) wWidth * (DWORD) wHeight);
		}
        pMap->uBitsPix = 4;
    }
    return pMap;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1356Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void SED1356Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR *Put;
    UCHAR uVal;
    UCHAR uFill = (UCHAR) (Color | (Color << 4));

    while(wWidth-- > 0)
    {
        SIGNED iLen = wXEnd - wXStart + 1;
        Put = mpScanPointers[wYPos] + (wXStart >> 1);

        // most compilers seem to do a good job of optimizing 
        // memset to do 32-bit data writes. If your compiler doesn't
        // make the most of your CPU, you might want to re-write this
        // in assembly.

        if (wXStart & 1)
        {
            uVal = *Put;
            uVal &= 0xf0;
            uVal |= Color;
            *Put++ = uVal;
            iLen--;
        }
        if (iLen > 0)
        {
            memset(Put, uFill, iLen >> 1);

            if (!(wXEnd & 1))
            {
                uVal = *(Put + (iLen >> 1));
                uVal &= 0x0f;
                uVal |= uFill & 0xf0;
                *(Put + (iLen >> 1)) = uVal;
            }
        }
        wYPos++;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1356Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR uFill = (UCHAR) (Color | (Color << 4));

    while(wYStart <= wYEnd)
    {
        UCHAR *Put = mpScanPointers[wYStart] + (wXPos >> 1);
        SIGNED iLen = wWidth;

        if (wXPos & 1)
        {
            UCHAR uVal = *Put;
            uVal &= 0xf0;
            uVal |= Color;
            *Put++ = uVal;
            iLen--;
        }

        if (iLen > 0)
        {
            memset(Put, uFill, iLen >> 1);

            if ((wXPos ^ iLen) & 1)
            {
                UCHAR uVal = *(Put + (iLen >> 1));
                uVal &= 0x0f;
                uVal |= uFill & 0xf0;
                *(Put + (iLen >> 1)) = uVal;
            }
        }
        wYStart++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1356Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    UCHAR *Put = mpScanPointers[wYPos] + (wXStart >> 1);
    SIGNED iLen = wXEnd - wXStart;

    if (!iLen)
    {
        return;
    }
    while(wXStart < wXEnd - 1)
    {
        *Put++ ^= 0x0f;
        wXStart += 2;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1356Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    UCHAR uVal = 0xf0;

    if (wXPos & 1)
    {
        uVal >>= 4;
    }
    while (wYStart <= wYEnd)
    {
        UCHAR *Put = mpScanPointers[wYStart] + (wXPos >> 1);
        *Put ^= uVal;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void SED1356Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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

    // If the capture is not evenly aligned, align it and capture one more
    // pixel to the left:

    if (CaptureRect.wLeft & 1)
    {
        CaptureRect.wLeft--;
    }

    if (CaptureRect.Width() & 1)
    {
        CaptureRect.wRight++;
    }

    Info->SetPos(CaptureRect);
    LONG Size = (pMap->wWidth * pMap->wHeight) >> 1;
    Info->Realloc(Size);

    UCHAR *GetStart = mpScanPointers[CaptureRect.wTop] + (CaptureRect.wLeft >> 1);

    // make room for the memory bitmap:

    pMap->uFlags = 0;        // raw format
    pMap->uBitsPix = 4;     // 4 bits per pixel

    // fill in the image with our captured info:

    UCHAR *Put = pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        memcpy(Put, GetStart, pMap->wWidth >> 1);
        GetStart += mwHRes >> 1;
        Put += pMap->wWidth >> 1;
    }
    Info->SetValid(TRUE);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1356Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uVal1;

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
        if (Getmap->uBitsPix == 4)
        {
            if (Where.x & 1)
            {
                DrawUnaligned16ColorBitmap(Where, Getmap, View);
            }
            else
            {
                Draw16ColorBitmap(Where, Getmap, View);
            }
        }
        else
        {
            // here for a source bitmap of 8-bpp:

            SIGNED iWidth = View.Width();
            UCHAR *Get = Getmap->pStart;
            Get += (View.wTop - Where.y) * Getmap->wWidth;
            Get += View.wLeft - Where.x;

            for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
            {
                UCHAR *Put = mpScanPointers[wLine] + (View.wLeft >> 1);
                if (View.wLeft & 1)
                {
                    uVal1 = *Put & 0xf0;
                }

                for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
                {
                    uVal = *Get++;
                    if (uVal == Getmap->dTransColor)
                    {
                        if (wLoop1 & 1)
                        {
                            uVal = *Put;
                            uVal &= 0x0f;
                            uVal |= uVal1;
                            *Put++ = uVal;
                        }
                        else
                        {
                            uVal1 = *Put & 0xf0;
                        }
                        continue;
                    }

                    if (wLoop1 & 1)
                    {
                        uVal1 |= uVal;
                        *Put++ = uVal1;
                    }
                    else
                    {
                        uVal1 = uVal << 4;
                    }
                }
                if (!(View.wRight & 1))
                {
                    uVal = *Put;
                    uVal &= 0x0f;
                    uVal |= uVal1;
                    *Put = uVal;
                }                    
                Get += Getmap->wWidth - iWidth;
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
// here for an aligned 16-color bitmap, no nibble shifting required.
/*--------------------------------------------------------------------------*/
void SED1356Screen::Draw16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    // always padded to nearest full byte per scan line:

    UCHAR uVal;
    WORD wBytesPerLine = (Getmap->wWidth + 1) / 2;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 1;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        UCHAR *Put = mpScanPointers[wLine] + (View.wLeft >> 1);
        UCHAR *Get = GetStart;
        SIGNED iCount = View.Width();

        if (View.wLeft & 1)
        {
            uVal = *Put;
            uVal &= 0xf0;
            uVal |= *Get++ & 0x0f;
            *Put++ = uVal;
            iCount--;
        }

        if (iCount > 0)
        {
            // copy two pixels at a time:
            memcpy(Put, Get, iCount >> 1);

            // check for an odd width:

            if (iCount & 1)
            {
                Put += iCount >> 1;
                Get += iCount >> 1;
                uVal = *Put;
                uVal &= 0x0f;
                uVal |= *Get & 0xf0;
                *Put = uVal;
            }
        }
        GetStart += wBytesPerLine;
    }
}

/*--------------------------------------------------------------------------*/
// here for a misaligned 16-color bitmap, nibble shifting required.
/*--------------------------------------------------------------------------*/
void SED1356Screen::DrawUnaligned16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uVal1;
    SIGNED iCount;
    UCHAR *Get;

    // always padded to nearest full byte per scan line:

    WORD wBytesPerLine = (Getmap->wWidth + 1) / 2;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 1;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        Get = GetStart;
        iCount = View.Width();

        // do the first pixel:

        uVal1 = *Get++;
        UCHAR *Put = mpScanPointers[wLine] + (View.wLeft >> 1);
        uVal = *Put;
        uVal &= 0xf0;
        uVal |= uVal1 >> 4;
        *Put++ = uVal;
        iCount--;

        while (iCount >= 2)
        {
            // do two pixels at a time:
            uVal = uVal1 << 4;
            uVal1 = *Get++;
            uVal |= uVal1 >> 4;
            *Put++ = uVal;
            iCount -= 2;
        }

        if (iCount)     // trailing pixel??
        {
            uVal = *Put;
            uVal &= 0x0f;
            uVal |= uVal1 << 4;
            *Put = uVal;
        }
        GetStart += wBytesPerLine;
    }
}




/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1356Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
                        PlotPointView(wLoop1, wLine, uVal);
                    }
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
                }
                else
                {
                    while(uCount--)
                    {
                        if (wLoop1 >= View.wLeft &&
                            wLoop1 <= View.wRight)
                        {
                            PlotPointView(wLoop1, wLine, uVal);
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
void SED1356Screen::RectMoveView(PegThing *Caller, const PegRect &View,
     const SIGNED xMove, const SIGNED yMove)
{
    PegCapture BlockCapture;
    PegRect CaptureRect = View;
    Capture(&BlockCapture, CaptureRect);
    BlockCapture.Shift(xMove, yMove);
    Bitmap(Caller, BlockCapture.Point(), BlockCapture.Bitmap(), TRUE);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1356Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
            iCharWidth = Font->pOffsets[0];
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
                        PlotPointView(wXpos, ScanRow, Color.uForeground);
                    }
                    else
                    {
                        if (Color.uFlags & CF_FILL)
                        {
                            PlotPointView(wXpos, ScanRow, Color.uBackground);
                        }
                    }
                }
                InMask >>= 1;
                wXpos++;
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
void SED1356Screen::ShowPointer(BOOL bShow)
{
    #if defined(USE_PCI) || defined(PEGWIN32)
     UCHAR *pRegs = mpVidRegBase;
    #else    
	 UCHAR *pRegs = (UCHAR *) VID_REG_BASE;
    #endif
   #ifdef HARDWARE_CURSOR
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
void SED1356Screen::HidePointer(void)
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
void SED1356Screen::SetPointer(PegPoint Where)
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
     #endif
    }
	#endif
}

/*--------------------------------------------------------------------------*/
void SED1356Screen::SetPointerType(UCHAR bType)
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
void SED1356Screen::ResetPalette(void)
{
    #ifdef GRAYSCALE
    SetPalette(0, 16, GrayPalette);
    #else
    SetPalette(0, 16, DefPalette256);
    #endif
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *SED1356Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 16;
    return muPalette;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1356Screen::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *pGet)
{
    UCHAR *pRegs = mpVidRegBase;
    const UCHAR *pPal;
    UCHAR rgb;
    SIGNED Index;

    // Initialize the LCD Palette:

	*(pRegs + REG_LUT_MODE) = 0x01;	     // Enable the LCD LUT for read/write.
	*(pRegs + REG_LUT_ADDR) = (UCHAR) iFirst;    // Reset the LUT address.

    pPal = pGet;

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

    pPal = pGet;

	for (Index = 0; Index < iNum; Index++)
	{
		for (rgb = 0; rgb < 3; rgb++)
		{
			*(pRegs + REG_LUT_DATA) = *pPal++;
		}
	}

    UCHAR *pPut = muPalette;
	// copy palette back to local pointer
    for (WORD loop = iFirst; loop < iNum; loop++)
    {
        *pPut++ = *pGet++;
        *pPut++ = *pGet++;
        *pPut++ = *pGet++;
	}
}



#include "1356regs.h"       // pull in HAL_INFO structure

/*--------------------------------------------------------------------------*/
void SED1356Screen::ConfigureController(void)
{
    #if defined(USE_PCI) || defined(PEGWIN32)
     UCHAR *pRegs = mpVidRegBase;
     UCHAR *pVidMem = mpVidMemBase;
    #else    
	 UCHAR *pRegs = (UCHAR *) VID_REG_BASE;
	 UCHAR *pVidMem = (UCHAR *) VID_MEM_BASE;
    #endif

    SED1356RegEntry *pGetReg = &HalInfo.rInitVals[0];

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
      1356 only allows 8K bitmaps, we will copy the current or active cursor
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

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the windows screen. This is only used
// for testing purposes, and can be deleted.

void SED1356Screen::MemoryToScreen(void)
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
    DWORD dSource = (DWORD) (mpScanPointers[iTop] + iLeft);
    DWORD dDest = (DWORD) (mpVidMemBase + (mpScanPointers[iTop] - mpScanPointers[0]) + iLeft);
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

	*(mpVidRegBase + REG_BITBLT_OPERATION) = 0x00;	// JMD
	*(mpVidRegBase + REG_BITBLT_ROP_CODE) = 0x0C;	// JMD
	*(mpVidRegBase + REG_BITBLT_CTRL0) = 0x80;	// JMD
    
	pGetStart = (WORD *) dSource;
    pPut = (WORD*) mpBitBlitRegs;
	WORD wCount = 0;

	while (*(mpVidRegBase + REG_BITBLT_CTRL0) & 0x80 == 0x00)
	{
		gbPreventOptimize++;
	}

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

/*---------------------------------------------------------------------------*/
/*
$Workfile: 1356scr4.cpp $
$Author: Ken $
$Date: 11/20/:2 12:43p $
$Revision: 29 $
$Log: /peg/source/1356scr4.cpp $
// 
// 29    11/20/:2 12:43p Ken
// Improved line drawing routine.
// 
// 28    11/15/:2 2:36p Jarret
// 
// 27    11/15/:2 1:30p Jim
// Changed to GrayPalette to linear spaced.
// 
// 26    11/11/:2 4:20p Jarret
// 
// 25    11/11/:2 3:51p Jim
// Removed the PharLap specific PCI stuff
// 
// 24    11/11/:2 3:24p Jarret
// 
// 22    8/01/:2 5:42p Jim
// Truly, the last keyword expansion test
// 
// 21    8/01/:2 5:40p Jim
// The last keyword expansion test
*/
/*---------------------------------------------------------------------------*/

// eof

