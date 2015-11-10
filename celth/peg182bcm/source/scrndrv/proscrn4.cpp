/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// proscrn4.cpp - PegScreen driver template for 4 bit-per-pixel
//      linear frame buffer access. This driver can be configured
//      to work with any screen resolution, simply by passing in the correct
//      rectangle to the constructor.
//
//      This driver is for use when the physical display screen has been
//      rotated 90 degrees clockwise or CCW.
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
// There are conditional sections of code delineated by #ifdef PEGWIN32. This
// is used ONLY for testing the driver. These sections of code can be removed
// to improve readability without affecting your target system.
//
// This driver is intended for systems which have direct, linear (i.e. not
// paged) access to the video frame buffer. This driver does not implement
// double buffering, although that could be added.
//
// All available configuration flags are found in the proscrn4.hpp header file.
//
//      *****************  IMPORTANT  ********************
//
// In order to use this driver in your system, you need to fill in the 
// following functions:
//
// 1) The ConfigureController() function, at the end of this file. This
//    function should initialize the video controller registers to properly
//    driver the display screen, and to properly access the video buffer.
//
// 2) The GetVideoAddress() function, to return a pointer
//    to the video frame buffer. This is target system dependant.
//
// 3) The SetPalette() function, to program the palette registers (if any).
//    The initial pallet programmed is the standard PEG 16-color palette.
//
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
    PegScreen *pScreen = new Pro4Screen(Rect);
    return pScreen;
}

#endif

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
#ifdef PEGWIN32
Pro4Screen::Pro4Screen(HWND hWnd, PegRect &Rect) : PegScreen(Rect)
#else
Pro4Screen::Pro4Screen(PegRect &Rect) : PegScreen(Rect)
#endif
{
    mdNumColors = 16;  

   #ifdef PEGWIN32
    mhPalette = NULL;
   #endif

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();
    miPitch = (mwVRes + 1) >> 1;

  #ifdef PEGWIN32
    // Windows bitmaps must be modulo-4 byte in width:
    miPitch += 3;
    miPitch &= 0xfffc;
  #endif

    mpScanPointers = new UCHAR PEGFAR *[mwHRes];

    UCHAR PEGFAR *CurrentPtr = GetVideoAddress();

   #ifdef ROTATE_CCW
    CurrentPtr += miPitch - 1;

    for (SIGNED iLoop = 0; iLoop < mwHRes; iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += miPitch;
    }

   #else
    CurrentPtr += (DWORD) miPitch * (DWORD) (mwHRes - 1);

    for (SIGNED iLoop = 0; iLoop < mwHRes; iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr -= miPitch;
    }

   #endif

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

   // make a second buffer to hold the bitmap data rotated back to
   // landscape mode:

   WORD wBytesPerLine = mwHRes >> 1;    // bitblt source is 4-bpp
   wBytesPerLine += 3;
   wBytesPerLine &= 0xfffc;
   mpDisplayBuff = new UCHAR[wBytesPerLine * mwVRes];

#endif

    #ifdef GRAYSCALE
    SetPalette(0, 16, GrayPalette);
    #else
    SetPalette(0, 16, DefPalette256);
    #endif
}


/*--------------------------------------------------------------------------*/
// *** This function must be filled in by the developer ***
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *Pro4Screen::GetVideoAddress(void)
{
#ifdef PEGWIN32

    DWORD dSize = (DWORD) miPitch * mwHRes;
    UCHAR *pMem = new UCHAR[dSize];
    return pMem; 

#else
    #error GetVideoAddress() function must be completed.
#endif
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
Pro4Screen::~Pro4Screen()
{
    #ifdef PEGWIN32

    UCHAR *pMem = mpScanPointers[0];

    #ifdef ROTATE_CCW
    pMem -= miPitch;
    pMem++;
    #else
    pMem -= (DWORD) miPitch * (DWORD) (mwHRes - 1);
    #endif

    delete pMem;
    delete mpDisplayBuff;

    #endif

    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void Pro4Screen::BeginDraw(PegThing *)
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
void Pro4Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        WORD wBytesPerLine = (pMap->wHeight + 1) >> 1;

        mpScanPointers = new UCHAR PEGFAR *[pMap->wWidth];
        UCHAR PEGFAR *CurrentPtr = pMap->pStart;

      #ifdef ROTATE_CCW
        CurrentPtr += wBytesPerLine - 1;
      #endif

        for (SIGNED iLoop = 0; iLoop < pMap->wWidth; iLoop++)
        {
            mpScanPointers[iLoop] = CurrentPtr;
            CurrentPtr += wBytesPerLine;
        }
                 
        mVirtualRect.Set(0, 0, pMap->wWidth - 1, pMap->wHeight - 1);
        miPitch = wBytesPerLine;
        mbVirtualDraw = TRUE;
    }
}

/*--------------------------------------------------------------------------*/
void Pro4Screen::EndDraw()
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
void Pro4Screen::EndDraw(PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        mbVirtualDraw = FALSE;
        delete [] mpScanPointers;
        mpScanPointers = mpSaveScanPointers;
        miPitch = (mwVRes + 1) >> 1;
        UNLOCK_PEG
    }
}

/*--------------------------------------------------------------------------*/
// CreateBitmap: The default version creates an 8-bpp bitmap,so we override
// to create a 4-bpp bitmap
/*--------------------------------------------------------------------------*/
PegBitmap *Pro4Screen::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
{
   PegBitmap *pMap = NULL;

    if (wWidth && wHeight)
    {
        pMap = new PegBitmap;
        pMap->wWidth = wWidth;
        pMap->wHeight = wHeight;
        wHeight++;
        wHeight /= 2;
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
void Pro4Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void Pro4Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR *Put;
    UCHAR uVal, uFill, uMask;

    SIGNED iOffset = miPitch;

   #ifdef ROTATE_CW
    if (mbVirtualDraw)
    {
        iOffset = -miPitch;
    }
   #endif
    

    while(wWidth-- > 0)
    {
        SIGNED iLen = wXEnd - wXStart + 1;

        #ifdef ROTATE_CCW
        Put = mpScanPointers[wXStart] - (wYPos >> 1);
        if (wYPos & 1)
        {
            uFill = Color << 4;
            uMask = 0x0f;
        }
        else
        {
            uFill = Color;
            uMask = 0xf0;
        }
        #else
        Put = mpScanPointers[wXStart] + (wYPos >> 1);
        if (wYPos & 1)
        {
            uFill = Color;
            uMask = 0xf0;
        }
        else
        {
            uFill = Color << 4;
            uMask = 0x0f;
        }
        #endif

        while(iLen-- > 0)
        {
            uVal = *Put;
            uVal &= uMask;
            uVal |= uFill;
            *Put = uVal;

            #ifdef ROTATE_CCW
            Put += iOffset;
            #else
            Put -= iOffset;
            #endif
        }
        wYPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro4Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR uVal;
    UCHAR *Put;
    UCHAR uFill = (UCHAR) (Color | (Color << 4));
    if (wYEnd < wYStart)
    {
        return;
    }

    while(wWidth--)
    {
        SIGNED iLen = wYEnd - wYStart + 1;

       #ifdef ROTATE_CCW
        Put = mpScanPointers[wXPos] - (wYStart >> 1);

        if (wYStart & 1)
        {
            uVal = *Put;
            uVal &= 0x0f;
            uVal |= Color << 4;
            *Put-- = uVal;
            iLen--;
        }

        if (iLen >= 2)
        {
            Put -= (iLen >> 1) - 1;
            memset(Put, uFill, iLen >> 1);
            Put--;
        }

        if (iLen & 1)
        {
            uVal = *Put;
            uVal &= 0xf0;
            uVal |= Color;
            *Put = uVal;
        }

       #else
        Put = mpScanPointers[wXPos] + (wYStart >> 1);
        if (wYStart & 1)
        {
            uVal = *Put;
            uVal &= 0xf0;
            uVal |= Color;
            *Put++ = uVal;
            iLen--;
        }

        if (iLen >= 2)
        {
            memset(Put, uFill, iLen >> 1);
            Put += iLen >> 1;
        }

        if (iLen & 1)
        {
            uVal = *Put;
            uVal &= 0x0f;
            uVal |= Color << 4;
            *Put = uVal;
        }
       #endif
        wXPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro4Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    UCHAR uVal = 0xf0;

    if (wYPos & 1)
    {
        #ifdef ROTATE_CW
        uVal >>= 4;
        #endif
    }
    else
    {
        #ifdef ROTATE_CCW
        uVal >>= 4;
        #endif
    }
    while (wXStart <= wXEnd - 1)
    {
        #ifdef ROTATE_CCW
        UCHAR *Put = mpScanPointers[wXStart] - (wYPos >> 1);
        #else
        UCHAR *Put = mpScanPointers[wXStart] + (wYPos >> 1);
        #endif
        *Put ^= uVal;
        wXStart += 2;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro4Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    wYStart++;
    wYStart &= 0xfffe;

    #ifdef ROTATE_CCW
    UCHAR *Put = mpScanPointers[wXPos] - (wYStart >> 1);
    #else
    UCHAR *Put = mpScanPointers[wXPos] + (wYStart >> 1);
    #endif
    SIGNED iLen = wYEnd - wYStart;

    if (iLen <= 0)
    {
        return;
    }
    while(wYStart < wYEnd - 1)
    {
        #ifdef ROTATE_CCW
        *Put-- ^= 0x0f;
        #else
        *Put++ ^= 0x0f;
        #endif
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void Pro4Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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
    if (CaptureRect.wBottom >= mwVRes)
    {
        CaptureRect.wBottom = mwVRes - 1;
    }
    if (CaptureRect.wRight >= mwHRes)
    {
        CaptureRect.wRight = mwHRes - 1;
    }

    if (CaptureRect.wLeft > CaptureRect.wRight ||
        CaptureRect.wTop > CaptureRect.wBottom)
    {
        return;
    }

    // If the capture is not evenly aligned, align it and capture one more
    // pixel to the left:

    if (CaptureRect.wTop & 1)
    {
        CaptureRect.wTop--;
    }

    if (CaptureRect.Height() & 1)
    {
        CaptureRect.wBottom++;
    }

    Info->SetPos(CaptureRect);
    LONG Size = (pMap->wWidth * pMap->wHeight) >> 1;
    Info->Realloc(Size);

   #ifdef ROTATE_CCW
    UCHAR *GetStart = mpScanPointers[CaptureRect.wLeft] - (CaptureRect.wBottom >> 1);
   #else
    UCHAR *GetStart = mpScanPointers[CaptureRect.wLeft] + (CaptureRect.wTop >> 1);
   #endif

    // make room for the memory bitmap:

    pMap->uFlags = 0;        // raw format
    pMap->uBitsPix = 4;     // 4 bits per pixel

    // fill in the image with our captured info:

    UCHAR *Put = pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wWidth; wLine++)
    {
        memcpy(Put, GetStart, pMap->wHeight >> 1);
        #ifdef ROTATE_CCW
        GetStart += miPitch;
        #else
        GetStart -= miPitch;
        #endif
        Put += pMap->wHeight >> 1;
    }
    Info->SetValid(TRUE);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro4Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uVal1;
    UCHAR *Put;

    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
    }
    else
    {
        if (Getmap->uBitsPix == 4)
        {
           #ifdef ROTATE_CCW
            if (((Where.y + Getmap->wHeight - 1) & 1) != 1)
           #else
            if (Where.y & 1)
           #endif
            {
                DrawUnalignedBitmap(Where, Getmap, View);
            }
            else
            {
                DrawFastBitmap(Where, Getmap, View);
            }
        }
        else
        {
            // here for a source bitmap of 8-bpp:

            SIGNED iHeight = View.Height();
            UCHAR *Get = Getmap->pStart;
            Get += (View.wLeft - Where.x) * Getmap->wHeight;

           #ifdef ROTATE_CCW

            Get += (Where.y + Getmap->wHeight - 1 - View.wBottom);
            if (!HAS_TRANS(Getmap))
            {
                for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
                {
                    SIGNED iLen = iHeight;
            
                    Put = mpScanPointers[wCol] - (View.wBottom >> 1);

                    // check the first byte:

                    if (!(View.wBottom & 1))
                    {
                        uVal = *Put;
                        uVal &= 0xf0;
                        uVal |= *Get++;
                        *Put++ = uVal;
                        iLen--;
                    }
                
                    // now do two pixels at a time:

                    while(iLen >= 2)
                    {
                        uVal = *Get++;
                        uVal <<= 4;
                        uVal |= *Get++;
                        *Put++ = uVal;
                        iLen -= 2;
                    }

                    if (iLen)
                    {
                        uVal = *Put;
                        uVal &= 0x0f;
                        uVal |= *Get++ << 4;
                        *Put = uVal;
                    }
                    Get += Getmap->wHeight - iHeight;
                }
            }
            else
            {
                // here for a source bitmap that has transparency

                for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
                {
                    SIGNED iLen = iHeight;
                    Put = mpScanPointers[wCol] - (View.wBottom >> 1);
                    uVal = *Put;
                    SIGNED iRow;

                    for (iRow = View.wBottom; iRow >= View.wTop; iRow--)
                    {
                        uVal1 = *Get++;
                        if (iRow & 1)
                        {
                            uVal = *Put;
                        }

                        if (uVal1 != 0xff)
                        {
                            if (iRow & 1)
                            {
                                uVal &= 0x0f;
                                uVal |= uVal1 << 4;
                            }
                            else
                            {
                                uVal &= 0xf0;
                                uVal |= uVal1;
                                *Put++ = uVal;
                            }
                        }
                        else
                        {
                            if (!(iRow & 1))
                            {
                                *Put++ = uVal;
                            }
                        }
                    }   // end row loop
                    if (!(iRow & 1))
                    {
                        *Put = uVal;
                    }

                    Get += Getmap->wHeight - iHeight;
                }       // end column loop
            }           // end  transparency test

           #else
                // here for CW rotation:

            Get += View.wTop - Where.y;

            if (!HAS_TRANS(Getmap))
            {
                for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
                {
                    Put = mpScanPointers[wCol] + (View.wTop >> 1);
                    SIGNED iLen = iHeight;

                    // check the first byte:

                    if (View.wTop & 1)
                    {
                        uVal = *Put;
                        uVal &= 0xf0;
                        uVal |= *Get++;
                        *Put++ = uVal;
                        iLen--;
                    }
                
                    // now do two pixels at a time:

                    while(iLen >= 2)
                    {
                        uVal = *Get++;
                        uVal <<= 4;
                        uVal |= *Get++;
                        *Put++ = uVal;
                        iLen -= 2;
                    }

                    if (iLen)
                    {
                        uVal = *Put;
                        uVal &= 0x0f;
                        uVal |= *Get++ << 4;
                        *Put = uVal;
                    }
                    Get += Getmap->wHeight - iHeight;
                }
            }
            else
            {
                // here for a source bitmap that has transparency

                for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
                {
                    SIGNED iLen = iHeight;
                    Put = mpScanPointers[wCol] + (View.wTop >> 1);
                    uVal = *Put;
                    SIGNED iRow;

                    for (iRow = View.wTop; iRow <= View.wBottom; iRow++)
                    {
                        uVal1 = *Get++;
                        if (!(iRow & 1))
                        {
                            uVal = *Put;
                        }

                        if (uVal1 != 0xff)
                        {
                            if (iRow & 1)
                            {
                                uVal &= 0xf0;
                                uVal |= uVal1;
                                *Put++ = uVal;
                            }
                            else
                            {
                                uVal &= 0x0f;
                                uVal |= uVal1 << 4;
                            }
                        }
                        else
                        {
                            if (iRow & 1)
                            {
                                *Put++ = uVal;
                            }
                        }
                    }   // end row loop
                    if (iRow & 1)
                    {
                        *Put = uVal;
                    }

                    Get += Getmap->wHeight - iHeight;
                }       // end column loop
            }           // end  transparency test

           #endif       // CW or CCW
        }
    }
}


/*--------------------------------------------------------------------------*/
// here for an aligned 16-color bitmap, no nibble shifting required.
/*--------------------------------------------------------------------------*/
void Pro4Screen::DrawFastBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    // always padded to nearest full byte per scan line:

    UCHAR uVal;
    WORD wBytesPerLine = (Getmap->wHeight + 1) >> 1;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wLeft - Where.x) * wBytesPerLine;

   #ifdef ROTATE_CCW
    GetStart += (Where.y + Getmap->wHeight - 1 - View.wBottom) >> 1;
   #else
    GetStart += (View.wTop - Where.y) >> 1;
   #endif

    for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
    {
        UCHAR *Get = GetStart;
        SIGNED iCount = View.Height();

       #ifdef ROTATE_CCW
        UCHAR *Put = mpScanPointers[wCol] - (View.wBottom >> 1);

        if (!(View.wBottom & 1))
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
       #else
        UCHAR *Put = mpScanPointers[wCol] + (View.wTop >> 1);

        if (View.wTop & 1)
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
       #endif
        GetStart += wBytesPerLine;
    }
}

/*--------------------------------------------------------------------------*/
// here for a misaligned 16-color bitmap, nibble shifting required.
/*--------------------------------------------------------------------------*/
void Pro4Screen::DrawUnalignedBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uVal1;
    SIGNED iCount;
    UCHAR *Get;

    // always padded to nearest full byte per scan line:

    WORD wBytesPerLine = (Getmap->wHeight + 1) >> 1;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wLeft - Where.x) * wBytesPerLine;

   #ifdef ROTATE_CCW
    GetStart += (Where.y + Getmap->wHeight - 1 - View.wBottom) >> 1;
   #else
    GetStart += (View.wTop - Where.y) >> 1;
   #endif

    for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
    {
        Get = GetStart;
        iCount = View.Height();

        // do the first pixel:
        uVal1 = *Get++;

        #ifdef ROTATE_CCW
        UCHAR *Put = mpScanPointers[wCol] - (View.wBottom >> 1);
        #else
        UCHAR *Put = mpScanPointers[wCol] + (View.wTop >> 1);
        #endif

        uVal = *Put;

        #ifdef ROTATE_CCW
        if (!(View.wBottom & 1))
        {
            uVal &= 0xf0;
            uVal |= uVal1 >> 4;
            *Put++ = uVal;
            iCount--;
        }
        #else
        if (View.wTop & 1)
        {
            uVal &= 0xf0;
            uVal |= uVal1 >> 4;
            *Put++ = uVal;
            iCount--;
        }
        #endif

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
void Pro4Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
    const PegBitmap *Getmap)
{
    UCHAR *Get = Getmap->pStart;
    UCHAR uVal;
    SIGNED uCount;

    SIGNED wCol = Where.x;
    uCount = 0;

    while (wCol < View.wLeft)
    {
        uCount = 0;

        while(uCount < Getmap->wHeight)
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
        wCol++;
    }

    while (wCol <= View.wRight)
    {
       #ifdef ROTATE_CCW
        SIGNED wLoop1 = Where.y + Getmap->wHeight - 1;
        while (wLoop1 >= Where.y)
        {
       #else
        SIGNED wLoop1 = Where.y;
        while(wLoop1 < Where.y + Getmap->wHeight)
        {
       #endif
            uVal = *Get++;

            if (uVal & 0x80)        // raw packet?
            {
                uCount = (uVal & 0x7f) + 1;
                
                while (uCount--)
                {
                    uVal = *Get++;
                    if (wLoop1 <= View.wBottom &&
                        wLoop1 >= View.wTop &&
                        uVal != 0xff)
                    {
                        PlotPointView(wCol, wLoop1, uVal);
                    }
                    #ifdef ROTATE_CCW
                    wLoop1--;
                    #else
                    wLoop1++;
                    #endif
                }
            }
            else
            {
                uCount = uVal + 1;
                uVal = *Get++;

                if (uVal == 0xff)
                {
                    #ifdef ROTATE_CCW
                    wLoop1 -= uCount;
                    #else
                    wLoop1 += uCount;
                    #endif
                }
                else
                {
                    while(uCount--)
                    {
                        if (wLoop1 <= View.wBottom &&
                            wLoop1 >= View.wTop)
                        {
                            PlotPointView(wCol, wLoop1, uVal);
                        }
                        #ifdef ROTATE_CCW
                        wLoop1--;
                        #else
                        wLoop1++;
                        #endif
                    }
                }
            }
        }
        wCol++;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro4Screen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void Pro4Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
void Pro4Screen::HidePointer(void)
{
#ifndef PEGWIN32

   #ifdef PEG_MOUSE_SUPPORT
    if (mbShowPointer)
    {
        PegPresentationManager *pp = PegThing::Presentation();
        Restore(pp, &mCapture, TRUE);
        mCapture.SetValid(FALSE);
    }
   #endif
#endif
}

/*--------------------------------------------------------------------------*/
void Pro4Screen::SetPointer(PegPoint Where)
{
#ifndef PEGWIN32

   #ifdef PEG_MOUSE_SUPPORT
    if (mbShowPointer)
    {
	    LOCK_PEG
	    HidePointer();
	    mLastPointerPos = Where;
	
	    PegPresentationManager *pp = PegThing::Presentation();
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
#endif
}

/*--------------------------------------------------------------------------*/
void Pro4Screen::SetPointerType(UCHAR bType)
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
void Pro4Screen::ResetPalette(void)
{
    #ifdef GRAYSCALE
    SetPalette(0, 16, GrayPalette);
    #else
    SetPalette(0, 16, DefPalette256);
    #endif
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *Pro4Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 16;
    return muPalette;
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
void Pro4Screen::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *pGet)
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
      #error- ** The SetPalette function must be filled in **
   #endif
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro4Screen::ConfigureController(void)
{
    #ifdef PEGWIN32
    #else
     #error- ** Place video controller configuration here **
    #endif

}

#ifdef PEGWIN32

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the windows screen. This is only used
// for testing purposes, and can be deleted.

void Pro4Screen::MemoryToScreen(void)
{
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

    // Rotate the frame buffer data back to portrait mode in our display
    // buffer. This step is only required when running under Win32:

    UCHAR *pPutStart = mpDisplayBuff;
    WORD wBytesPerLine = mwHRes >> 1;
    wBytesPerLine += 3;
    wBytesPerLine &= 0xfffc;

    for (WORD wLoop = 0; wLoop < mwVRes; wLoop++)
    {
        UCHAR *pGet = mpScanPointers[0];
        UCHAR *pPut = pPutStart;
        UCHAR uVal;

       #ifdef ROTATE_CCW

        pGet -= wLoop >> 1;
        if (wLoop & 1)
        {
       #else
        pGet += wLoop >> 1;
        if (!(wLoop & 1))
        {
       #endif
            // high nibbles:

            for (WORD wLoop1 = 0; wLoop1 < mwHRes >> 1; wLoop1++)
            {
                #ifdef ROTATE_CCW
                uVal = *pGet & 0xf0;
                pGet += mwVRes >> 1;
                uVal |= *pGet >> 4;
                pGet += mwVRes >> 1;
                *pPut++ = uVal;
                #else
                uVal = *pGet & 0xf0;
                pGet -= mwVRes >> 1;
                uVal |= *pGet >> 4;
                pGet -= mwVRes >> 1;
                *pPut++ = uVal;
                #endif
            }
        }
        else
        {
            // low nibbles:

            for (WORD wLoop1 = 0; wLoop1 < mwHRes >> 1; wLoop1++)
            {
                #ifdef ROTATE_CCW
                uVal = *pGet << 4;
                pGet += mwVRes >> 1;
                uVal |= *pGet & 0x0f;
                pGet += mwVRes >> 1;
                #else
                uVal = *pGet << 4;
                pGet -= mwVRes >> 1;
                uVal |= *pGet & 0x0f;
                pGet -= mwVRes >> 1;
                #endif
                *pPut++ = uVal;
            }
        }
        pPutStart += wBytesPerLine;
    }

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
        mpDisplayBuff, (const struct tagBITMAPINFO *) &BMhead,
        DIB_RGB_COLORS, SRCCOPY);

    SelectObject(mHdc, hOldPal);
}

#endif
