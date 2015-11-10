/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1375scr4.cpp - PegScreen driver for the SED 1375 eval card at
//		4 bit-per-pixel color depth. This driver can be configured
//      to work with any screen resolution, simply by passing in the correct
//      rectangle to the constructor. 
//
// Author: Kenneth G. Maxwell & Jim DeLisle
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
// There are conditional sections of code delineated by #ifdef PEGWIN32. This
// is used ONLY for testing the driver. These sections of code can be removed
// to improve readability without affecting your target system.
//
//
// All available configuration flags are found in the 1375scr4.hpp header file.
//
//      *****************  IMPORTANT  ********************
// Known Limitations:
// 
// This driver implements off-screen drawing using the native 4-bpp format.
// This means that transparency information is lost if a bitmap is drawn
// to the off-screen buffer. The tradeoff here is that off-screen drawing
// uses less memory, while supporting transparency in off-screen drawing
// would double the memory requirements for the off-screen bitmap.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"


#ifdef GRAYSCALE

// These are 16 grayscale values used to provide equivalant luminance
// to the default 16-color color palette. We defined "RGB" grayscale
// values for testing, however on your system you will probably only
// need one component from each row. These are also 8-bit values, and will
// need to be shifted right to match your palette register width.

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
    PegScreen *pScreen = new SED1375Screen4(Rect);
    return pScreen;
}
#endif


/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
#ifdef PEGWIN32
SED1375Screen4::SED1375Screen4(HWND hWnd, PegRect &Rect) : PegScreen(Rect)
#else
SED1375Screen4::SED1375Screen4(PegRect &Rect) : PegScreen(Rect)
#endif
{
    mdNumColors = 16;  

   #ifdef PEGWIN32
    mhPalette = NULL;
   #endif

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();

    mpScanPointers = new UCHAR PEGFAR *[Rect.Height()];

    UCHAR PEGFAR *CurrentPtr = GetVideoAddress();

    for (SIGNED iLoop = 0; iLoop < Rect.Height(); iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += mwHRes >> 1;
    }

    mLastPointerPos.x = Rect.Width() / 2;
    mLastPointerPos.y = Rect.Height() / 2;
    mbPointerHidden = FALSE;
    mwDrawNesting = 0;

    ConfigureController();        // set up controller registers


#ifdef PEGWIN32

   // Some setup stuff for the BitBlitting function:

   mHWnd = hWnd;
   mhPalette = NULL;
   RECT lSize;
   ::GetClientRect(mHWnd, &lSize);

   mwWinRectXOffset = (lSize.right - mwHRes) / 2;
   mwWinRectYOffset = (lSize.bottom -mwVRes) / 2;
#endif

   #ifdef GRAYSCALE
    SetPalette(0, 16, GrayPalette);
   #else
    SetPalette(0, 16, DefPalette256);
   #endif
}


/*--------------------------------------------------------------------------*/
// GetVideoAddress
//
// On the Win32 development platform, we allocate a buffer the size of
// the development window to allow for double buffering.
//
// On the target, if DOUBLE_BUFFER is turned on, we allocate an appropriate
// size buffer in local memory to which we'll do our drawing. If
// DOUBLE_BUFFER'ing is turned off, we'll just return the address of 
// video memory.
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *SED1375Screen4::GetVideoAddress(void)
{
   #ifdef PEGWIN32
    DWORD dSize = mwHRes / 2 * mwVRes;
    UCHAR *pMem = new UCHAR[dSize];
    return pMem; 
   #endif

   #ifdef DOUBLE_BUFFER
	DWORD dSize = (mwHRes >> 1) * mwVRes;
	UCHAR PEGFAR *pMem = new UCHAR[dSize];
	return pMem;
   #else

    #ifdef X86_REAL_MODE
     return((UCHAR PEGFAR *) (VID_MEM_BASE << 12));
    #else
	 return((UCHAR PEGFAR *) VID_MEM_BASE);
    #endif
   #endif
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
SED1375Screen4::~SED1375Screen4()
{
    #ifdef PEGWIN32

    delete [] mpScanPointers[0];

    #endif

    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void SED1375Screen4::BeginDraw(PegThing *)
{
    LOCK_PEG
    if (!mwDrawNesting)
    {
       #ifdef PEGWIN32
        mHdc = GetDC(mHWnd);
       #endif

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
void SED1375Screen4::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

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
void SED1375Screen4::EndDraw()
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

       #ifdef PEGWIN32
        MemoryToScreen();
        ReleaseDC(mHWnd, mHdc);
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
void SED1375Screen4::EndDraw(PegBitmap *pMap)
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
PegBitmap *SED1375Screen4::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
{
   PegBitmap *pMap = NULL;

    if (wWidth && wHeight)
    {
        pMap = new PegBitmap;
        pMap->wWidth = wWidth;
        pMap->wHeight = wHeight;
        wWidth++;
        wWidth /= 2;
        pMap->pStart = new UCHAR[(DWORD) wWidth * (DWORD) wHeight];
        if (!pMap->pStart)
        {
            delete pMap;
            return NULL;
        }
		if(bHasTrans)
		{
			memset(pMap->pStart, TRANSPARENCY, (DWORD) wWidth * (DWORD) wHeight);
			pMap->uFlags = BMF_HAS_TRANS;
		}
		else
		{
			// fill the whole thing with BLACK:
			memset(pMap->pStart, BLACK, (DWORD) wWidth * (DWORD) wHeight);
			pMap->uFlags = 0;
		}
        pMap->uBitsPix = 4;
    }
    return pMap;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1375Screen4::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void SED1375Screen4::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR PEGFAR *Put;
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
void SED1375Screen4::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR uFill = (UCHAR) (Color | (Color << 4));

    while(wYStart <= wYEnd)
    {
        UCHAR PEGFAR *Put = mpScanPointers[wYStart] + (wXPos >> 1);
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
void SED1375Screen4::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    UCHAR PEGFAR *Put = mpScanPointers[wYPos] + (wXStart >> 1);
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
void SED1375Screen4::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    UCHAR uVal = 0xf0;

    if (wXPos & 1)
    {
        uVal >>= 4;
    }
    while (wYStart <= wYEnd)
    {
        UCHAR PEGFAR *Put = mpScanPointers[wYStart] + (wXPos >> 1);
        *Put ^= uVal;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void SED1375Screen4::Capture(PegCapture *Info, PegRect &CaptureRect)
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

    UCHAR PEGFAR *GetStart = mpScanPointers[CaptureRect.wTop] + (CaptureRect.wLeft >> 1);

    // make room for the memory bitmap:

    pMap->uFlags = 0;        // raw format
    pMap->uBitsPix = 4;     // 4 bits per pixel

    // fill in the image with our captured info:

    UCHAR PEGFAR *Put = pMap->pStart;

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
void SED1375Screen4::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uVal1;

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
            UCHAR PEGFAR *Get = Getmap->pStart;
            Get += (View.wTop - Where.y) * Getmap->wWidth;
            Get += View.wLeft - Where.x;

            for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
            {
                UCHAR PEGFAR *Put = mpScanPointers[wLine] + (View.wLeft >> 1);
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
void SED1375Screen4::Draw16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    // always padded to nearest full byte per scan line:

    UCHAR uVal;
    WORD wBytesPerLine = (Getmap->wWidth + 1) / 2;
    UCHAR PEGFAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 1;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        UCHAR PEGFAR *Put = mpScanPointers[wLine] + (View.wLeft >> 1);
        UCHAR PEGFAR *Get = GetStart;
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
void SED1375Screen4::DrawUnaligned16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uVal1;
    SIGNED iCount;
    UCHAR PEGFAR *Get;

    // always padded to nearest full byte per scan line:

    WORD wBytesPerLine = (Getmap->wWidth + 1) / 2;
    UCHAR PEGFAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 1;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        Get = GetStart;
        iCount = View.Width();

        // do the first pixel:

        uVal1 = *Get++;
        UCHAR PEGFAR *Put = mpScanPointers[wLine] + (View.wLeft >> 1);
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
void SED1375Screen4::DrawRleBitmap(const PegPoint Where, const PegRect View,
    const PegBitmap *Getmap)
{
    UCHAR PEGFAR *Get = Getmap->pStart;
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
void SED1375Screen4::RectMoveView(PegThing *Caller, const PegRect &View,
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
void SED1375Screen4::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
void SED1375Screen4::HidePointer(void)
{
#ifndef PEGWIN32
   #ifdef PEG_MOUSE_SUPPORT
    if (mbShowPointer)
    {
        PegThing *pt = NULL;
        PegPresentationManager *pp = pt->Presentation();
        Restore(pp, &mCapture, TRUE);
        mCapture.SetValid(FALSE);
    }
   #endif
#endif
}

/*--------------------------------------------------------------------------*/
void SED1375Screen4::SetPointer(PegPoint Where)
{
#ifndef PEGWIN32
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
	
	   #ifdef DOUBLE_BUFFER
		if (!miInvalidCount)
		{
			mInvalid = mCapture.Pos();
		}
	   #endif
	
	    Capture(&mCapture, MouseRect);
	        
	    Bitmap(pp, Where, mpCurPointer, TRUE);
	
	   #ifdef DOUBLE_BUFFER
	
		// If double buffering and the mouse is being drawn because of a move,
		// not because it is over an area being re-draw, then the mouse
		// area must be moved to video memory.
	
		if (!miInvalidCount)
		{
			miInvalidCount = 1;
			mInvalid |= MouseRect;
			MemoryToScreen();
			miInvalidCount = 0;
		}
	   #endif
	
	    UNLOCK_PEG
    }
    #endif
#endif
}

/*--------------------------------------------------------------------------*/
void SED1375Screen4::SetPointerType(UCHAR bType)
{
   #ifdef PEGWIN32
    if (bType < NUM_POINTER_TYPES)
    {
        mpCurPointer = mpPointers[bType].Bitmap;
        miCurXOffset = mpPointers[bType].xOffset;
        miCurYOffset = mpPointers[bType].yOffset;
    }

    switch(bType)
    {
    case PPT_NORMAL:
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        break;

    case PPT_VSIZE:
        SetCursor(LoadCursor(NULL, IDC_SIZENS));
        break;

    case PPT_HSIZE:
        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        break;

    case PPT_NWSE_SIZE:
        SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
        break;

    case PPT_NESW_SIZE:
        SetCursor(LoadCursor(NULL, IDC_SIZENESW));
        break;

    case PPT_IBEAM:
        SetCursor(LoadCursor(NULL, IDC_IBEAM));
        break;

    case PPT_HAND:
        SetCursor(LoadCursor(NULL, IDC_CROSS));
        break;
    }
   #endif
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
void SED1375Screen4::ResetPalette(void)
{
   #ifdef GRAYSCALE
    SetPalette(0, 16, GrayPalette);
   #else
    SetPalette(0, 16, DefPalette256);
   #endif
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *SED1375Screen4::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 16;
   #ifdef GRAYSCALE
	return GrayPalette;
   #else
	return DefPalette256;
   #endif
}


#ifdef PEGWIN32

typedef struct 
{
    BITMAPINFOHEADER bmhead;
    RGBQUAD  ColorTable[256];
} BMHEAD;

BMHEAD BMhead;

typedef struct
{
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[256];
} PEG_WIN_PALETTE;

PEG_WIN_PALETTE WinPal;

#endif


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1375Screen4::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *pGet)
{
   #ifdef PEGWIN32

    BMhead.bmhead.biSize = sizeof(BITMAPINFOHEADER);
    BMhead.bmhead.biWidth = mwHRes;
    BMhead.bmhead.biHeight = -mwVRes;
    BMhead.bmhead.biPlanes = 1;
    BMhead.bmhead.biBitCount = 4;
    BMhead.bmhead.biCompression = BI_RGB;
    BMhead.bmhead.biSizeImage = mwHRes * mwVRes;
    BMhead.bmhead.biClrUsed = iNum;
    BMhead.bmhead.biClrImportant = iNum;
    WinPal.palNumEntries = iNum;
    WinPal.palVersion = 0x0300;

    UCHAR *pPut = muPalette;

    for (WORD loop = 0; loop < iNum; loop++)
    {
        *pPut++ = *pGet;
        BMhead.ColorTable[loop].rgbRed = *pGet++;
        *pPut++ = *pGet;
        BMhead.ColorTable[loop].rgbGreen = *pGet++;
        *pPut++ = *pGet;
        BMhead.ColorTable[loop].rgbBlue = *pGet++;

        WinPal.palPalEntry[loop].peRed = BMhead.ColorTable[loop].rgbRed;
        WinPal.palPalEntry[loop].peGreen = BMhead.ColorTable[loop].rgbGreen;
        WinPal.palPalEntry[loop].peBlue = BMhead.ColorTable[loop].rgbBlue;
        WinPal.palPalEntry[loop].peFlags = 0;
    }
    if (mhPalette)
    {
        DeleteObject(mhPalette);
    }
    mhPalette = CreatePalette((LOGPALETTE *) &WinPal);

   #else

	// Registers 15, 16 and 17 control the palette

   #ifdef X86_REAL_MODE
	UCHAR PEGFAR* pRegBase = (UCHAR PEGFAR *) (VID_REG_BASE << 12);
   #else
	UCHAR PEGFAR* pRegBase = (UCHAR PEGFAR *) VID_REG_BASE;
   #endif

   //#ifdef GRAYSCALE
	// Register 15 - Lookup table select/address
	//*(pRegBase + 0x15) = 0x10;
   //#else
	// Register 15 - Lookup table select/address
	*(pRegBase + 0x15) = 0x00;
   //#endif

	// Register 16 - Lookup table bank select
	//*(pRegBase + 0x16) = 0x00;

	// Register 17 - Lookup table data
	const UCHAR* pShade = pGet;

   //#ifdef GRAYSCALE

   //#else
	for(SIGNED i = iFirst; i < iNum; i++)
	{
		// Use the first 16 entries from the 256 color palette
		for(UCHAR j = 0; j < 3; j++)
		{
			*(pRegBase + 0x17) = (UCHAR)(*pShade) & 0xf0;
			pShade++;
		}
	}
   //#endif

   #endif	// PEGWIN32
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SED1375Screen4::ConfigureController(void)
{
   #ifdef PEGWIN32
   #else

   #ifdef X86_REAL_MODE
	UCHAR PEGFAR* pMemBase = (UCHAR PEGFAR*) (VID_MEM_BASE << 12);
	UCHAR PEGFAR* pRegBase = (UCHAR PEGFAR*) (VID_REG_BASE << 12);
   #else	
	UCHAR PEGFAR* pMemBase = (UCHAR PEGFAR*) VID_MEM_BASE;
	UCHAR PEGFAR* pRegBase = (UCHAR PEGFAR*) VID_REG_BASE;
   #endif

	// Check for the existence of the SED1375

	if(*pRegBase != SED1375ID)
	{
		return;
	}

	*(pRegBase + 0x01) = 0x23;	// 4bpp color mode / 8 bit data format 2

	// Register 2 - Performance
	*(pRegBase + 0x02) = 0x80;	// hi performance 4 bpp

	// Register 3 - Power operation
	*(pRegBase + 0x03) = 0x03;	// normal power operation

	// Register 4 - Horizontal screen size
	UCHAR ucRes = ((UCHAR)(mwHRes / 8)) - 1;
	*(pRegBase + 0x04) = ucRes;

	// Register 5 - LSB of the Vertical screen size
	*(pRegBase + 0x05) = (UCHAR)(mwVRes & 0xff);

	// Register 6 - MSB of the Vertical screen size
	// Only use bits 0 and 1
	ucRes = ((UCHAR)((mwVRes >> 8) & 0x03));
	*(pRegBase + 0x06) = ucRes;

	// Register 7 - not used
	*(pRegBase + 0x07) = 0x00;

	// Register 8 - Horizontal non-display period
	*(pRegBase + 0x08) = 0x00;

	// Register 9 - not used
	*(pRegBase + 0x09) = 0x00;

	// Register 0a - Vertical non-display register
	*(pRegBase + 0x0a) = 0x03;
	
	// Register 0b - not used
	*(pRegBase + 0x0b) = 0x00;

	// Register 0c - Screen 1 start word address LSB
	*(pRegBase + 0x0c) = 0x00;

	// Register 0d - Screen 1 start word address MSB
	*(pRegBase + 0x0d) = 0x00;

	// Register 0f - Screen 2 start word address LSB
	*(pRegBase + 0x0f) = 0x00;

	// Register 10 - Screen 2 start word address MSB
	*(pRegBase + 0x10) = 0x00;

	// Register 12 - Memory address offset
	*(pRegBase + 0x12) = 0x00;

	// Register 13 - Screen 1 Vertical size LSB
	// If this is set different than register 5, then 
	// split screen will be enabled. We don't want to
	// do that here.
	*(pRegBase + 0x13) = (UCHAR)(mwVRes & 0xff);

	// Register 14 - Screen 1 Vertical size MSB
	*(pRegBase + 0x14) = ucRes;

	// Registers 15 through 17 set the up the palette
	// See the ::SetPalette member for this.

	// Register 18 - GPIO config
	*(pRegBase + 0x18) = 0x00;

	// Register 19 - GPIO status
	*(pRegBase + 0x19) = 0x00;

	// Register 1a - Scratch pad
	*(pRegBase + 0x1a) = 0x00;

	// Register 1b - Swivel mode
	*(pRegBase + 0x1b) = 0x00;

	// Register 1c - Line byte count - not used in landscape mode
	*(pRegBase + 0x1c) = 0x00;

   #endif
}

#if defined(PEGWIN32) || defined(DOUBLE_BUFFER)

/*--------------------------------------------------------------------------*/
// MemoryToScreen is only used if on the target if DOUBLE_BUFFER is turned
// on. This causes the nested drawing operations to draw to a local off
// screen buffer, then, when the EndDraw stack unwinds, we copy the
// invalidated region into video memory in a fast copy. This helps eliminate
// flashing on the display.
//
// The caveat to this is that you will need a block of free memory the
// size of (screen size / color depth) in local memory. If your hardware
// doesn't meet this requirement, turn off DOUBLE_BUFFER'ing.
//
// If we're running on the Win32 development environment, double buffering
// is turned on by default.
/*--------------------------------------------------------------------------*/
void SED1375Screen4::MemoryToScreen(void)
{
#ifdef PEGWIN32
	if (!miInvalidCount)
    {
        return;
    }

    PegRect Copy;
    Copy.wTop = 0;
    Copy.wLeft = 0;
    Copy.wRight = mwHRes - 1;
    Copy.wBottom = mwVRes - 1;
    Copy &= mInvalid;

    SetMapMode(mHdc, MM_TEXT);

    HPALETTE hOldPal = SelectPalette(mHdc, mhPalette, FALSE);
    RealizePalette(mHdc);

    SIGNED iTop = Copy.wTop + mwWinRectYOffset;
    SIGNED iLeft = Copy.wLeft + mwWinRectXOffset;

    SIGNED iWidth = Copy.Width();
    SIGNED iHeight = Copy.Height();

    if (Copy.wTop + iHeight == mwVRes)
    {
        iHeight--;
    }

    StretchDIBits(mHdc, iLeft, iTop, iWidth, iHeight,
        Copy.wLeft, mwVRes - (Copy.wTop + iHeight), iWidth, iHeight,
        mpScanPointers[0], (const struct tagBITMAPINFO *) &BMhead,
        DIB_RGB_COLORS, SRCCOPY);

    SelectObject(mHdc, hOldPal);
#else

   #ifdef DOUBLE_BUFFER
    if (!miInvalidCount)
    {
        return;
    }

    WORD wLoop, wLoop1;

    PegRect Copy;
    Copy.Set(0, 0, mwHRes - 1, mwVRes - 1);
    Copy &= mInvalid;

    Copy.wLeft &= 0xfffe;   // for byte alignment on the left edge
	Copy.wRight |= 1;

    SIGNED iTop = Copy.wTop;
    SIGNED iLeft = Copy.wLeft;
    SIGNED iWidth = Copy.Width();

    if (iWidth <= 0)
    {
        return;
    }
    //iWidth += 1;
    //iWidth &= 0xfffe;       // for byte alignment on the right edge
    iWidth /= 2;            // for byte transfers


    SIGNED iAdjust = (mwHRes >> 1) - iWidth;

    SIGNED iHeight = Copy.Height();

    if (iHeight <= 0)
    {
        return;
    }

    UCHAR PEGFAR *pSource = mpScanPointers[iTop];
    pSource += iLeft / 2;
    UCHAR PEGFAR *pDest;
    
   #ifdef X86_REAL_MODE
    pDest = (UCHAR PEGFAR *) (VID_MEM_BASE << 12);
   #else
    pDest = (UCHAR PEGFAR *) VID_MEM_BASE;
   #endif

    pDest += (pSource - mpScanPointers[0]);

    for (wLoop = 0; wLoop < iHeight; wLoop++)
    {
        for (wLoop1 = 0; wLoop1 < iWidth; wLoop1++)
        {
            *pDest++ = *pSource++;
        }
        pDest += iAdjust;
        pSource += iAdjust;
    }
   #endif
#endif
}

#endif
