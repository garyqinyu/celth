/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// proscrn8.cpp - Linear 8 bit-per-pixel screen driver. This driver can be
//   configured to work with any screen resolution, simply by passing in the
//   correct rectangle to the constructor. 
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
// double buffering, although that could easily be added.
//
// All available configuration flags are found in the L4Screen.hpp header file.
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"

extern UCHAR DefPalette256[];

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
    PegScreen *pScreen = new Pro8Screen(Rect);
    return pScreen;
}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
#ifdef PEGWIN32
Pro8Screen::Pro8Screen(HWND hWnd, PegRect &Rect) : PegScreen(Rect)
#else
Pro8Screen::Pro8Screen(PegRect &Rect) : PegScreen(Rect)
#endif
{
    mdNumColors = 256;

   #ifdef PEGWIN32
    mhPalette = NULL;
   #endif

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();
    miPitch = mwVRes;

   #ifdef PEGWIN32
    
    // Windows bitmaps must be modulo-4 byte in width:
    miPitch += 3;
    miPitch &= 0xfffc;

   #endif

    mpScanPointers = new UCHAR PEGFAR *[Rect.Height()];
    UCHAR PEGFAR *CurrentPtr = GetVideoAddress();

   #ifdef ROTATE_CCW
    CurrentPtr += mwVRes - 1;

    for (SIGNED iLoop = 0; iLoop < mwHRes; iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += miPitch;
    }

   #else
    CurrentPtr += mwVRes * (mwHRes - 1);

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
   RECT lSize;
   ::GetClientRect(mHWnd, &lSize);

   mwWinRectXOffset = (lSize.right - mwHRes) / 2;
   mwWinRectYOffset = (lSize.bottom -mwVRes) / 2;
   mhPalette = NULL;
   mpDisplayBuff = new UCHAR[(DWORD) mwHRes * (DWORD) mwVRes];
#endif

    SetPalette(0, 232, DefPalette256);
}

/*--------------------------------------------------------------------------*/
// *** This function must be filled in by the developer ***
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *Pro8Screen::GetVideoAddress(void)
{
#ifdef PEGWIN32

    DWORD dSize = mwHRes * miPitch;
    UCHAR *pMem = new UCHAR[dSize];
    return pMem; 

#else

    // for an example, just allocate a buffer in dynamic memory:

    DWORD dSize = mwHRes * mwVRes;
    UCHAR *pMem = new UCHAR[dSize];
    return pMem; 

#endif
}

/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
Pro8Screen::~Pro8Screen()
{
    #ifdef PEGWIN32

    UCHAR *pMem = mpScanPointers[0];

    #ifdef ROTATE_CCW
    pMem -= mwVRes;
    pMem++;
    #else
    pMem -= mwVRes * (mwHRes - 1);
    #endif

    delete pMem;
    delete mpDisplayBuff;

    #else

    delete [] mpScanPointers[0];

    #endif

    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void Pro8Screen::BeginDraw(PegThing *)
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
void Pro8Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        mpScanPointers = new UCHAR PEGFAR *[pMap->wWidth];
        UCHAR PEGFAR *CurrentPtr = pMap->pStart;

      #ifdef ROTATE_CCW
        CurrentPtr += pMap->wHeight - 1;
      #endif

        for (SIGNED iLoop = 0; iLoop < pMap->wWidth; iLoop++)
        {
            mpScanPointers[iLoop] = CurrentPtr;
            CurrentPtr += pMap->wHeight;
        }

        mVirtualRect.Set(0, 0, pMap->wWidth - 1, pMap->wHeight - 1);
        miPitch = pMap->wHeight;
        mbVirtualDraw = TRUE;
    }
}


/*--------------------------------------------------------------------------*/
void Pro8Screen::EndDraw()
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
void Pro8Screen::EndDraw(PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        mbVirtualDraw = FALSE;
        delete [] mpScanPointers;
        mpScanPointers = mpSaveScanPointers;
        miPitch = mwVRes;
        UNLOCK_PEG
    }
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro8Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void Pro8Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    while(wXStart <= wXEnd)
    {
        SIGNED lWidth = wWidth;
        #ifdef ROTATE_CCW
        UCHAR *Put = mpScanPointers[wXStart] - wYPos;
        #else
        UCHAR *Put = mpScanPointers[wXStart] + wYPos;
        #endif
        while (lWidth-- > 0)
        {
            #ifdef ROTATE_CCW
            *Put-- = (UCHAR) Color;
            #else
            *Put++ = (UCHAR) Color;
            #endif
        }
        wXStart++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro8Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    SIGNED iLen = wYEnd - wYStart + 1;

    if (iLen <= 0)
    {
        return;
    }

    while(wWidth-- > 0)
    {
        #ifdef ROTATE_CCW
        UCHAR *Put = mpScanPointers[wXPos] - wYEnd;
        #else
        UCHAR *Put = mpScanPointers[wXPos] + wYStart;
        #endif
        memset(Put, Color, iLen);
        wXPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro8Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    while (wXStart <= wXEnd)
    {
        #ifdef ROTATE_CCW
        UCHAR *Put = mpScanPointers[wXStart] - wYPos;
        #else
        UCHAR *Put = mpScanPointers[wXStart] + wYPos;
        #endif

        *Put ^= 0x0f;
        wXStart += 2;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro8Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    UCHAR uVal;

    #ifdef ROTATE_CCW
    UCHAR *Put = mpScanPointers[wXPos] - wYEnd;
    #else
    UCHAR *Put = mpScanPointers[wXPos] + wYStart;
    #endif

    while (wYStart <= wYEnd)
    {
        uVal = *Put;
        uVal ^= 0xf;
        *Put = uVal;
        Put += 2;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void Pro8Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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
    if (CaptureRect.wLeft > CaptureRect.wRight ||
        CaptureRect.wTop > CaptureRect.wBottom)
    {
        return;
    }

    Info->SetPos(CaptureRect);
    LONG Size = pMap->wWidth * pMap->wHeight;
    Info->Realloc(Size);

   #ifdef ROTATE_CCW
    UCHAR *GetStart = mpScanPointers[CaptureRect.wLeft] - CaptureRect.wBottom;
   #else
    UCHAR *GetStart = mpScanPointers[CaptureRect.wLeft] + CaptureRect.wTop;
   #endif

    // make room for the memory bitmap:

    pMap->uFlags = 0;        // raw format
    pMap->uBitsPix = 8;      // 8 bits per pixel

    // fill in the image with our captured info:

    UCHAR *Put = pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wWidth; wLine++)
    {
        memcpy(Put, GetStart, pMap->wHeight);
        Put += pMap->wHeight;
        #ifdef ROTATE_CCW
        GetStart += miPitch;
        #else
        GetStart -= miPitch;
        #endif
    }
    Info->SetValid(TRUE);
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro8Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
    }
    else
    {
        SIGNED iHeight = View.Height();
        UCHAR *Get = Getmap->pStart;
        Get += (View.wLeft - Where.x) * Getmap->wHeight;

        #ifdef ROTATE_CCW
        Get += (Where.y + Getmap->wHeight - 1 - View.wBottom);
        #else
        Get += View.wTop - Where.y;
        #endif

        if (HAS_TRANS(Getmap))
        {
            for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
            {
                #ifdef ROTATE_CCW
	            UCHAR *Put = mpScanPointers[wCol] - View.wBottom;
                #else
	            UCHAR *Put = mpScanPointers[wCol] + View.wTop;
                #endif

	            for (SIGNED wLoop1 = View.wTop; wLoop1 <= View.wBottom; wLoop1++)
	            {
	                UCHAR uVal = *Get++;
	
	                if (uVal != 0xff)
	                {
	                    *Put = uVal;
	                }
	                Put++;
	            }
	            Get += Getmap->wHeight - iHeight;
	        }
        }
        else
        {
            for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
            {
                #ifdef ROTATE_CCW
	            UCHAR *Put = mpScanPointers[wCol] - View.wBottom;
                #else
	            UCHAR *Put = mpScanPointers[wCol] + View.wTop;
                #endif
                memcpy(Put, Get, iHeight);
	            Get += Getmap->wHeight;
	        }
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro8Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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

    if (HAS_TRANS(Getmap))
    {
	    while (wCol <= View.wRight)
	    {
            #ifdef ROTATE_CCW
	        SIGNED wLoop1 = Where.y + Getmap->wHeight - 1;
	        UCHAR *Put = mpScanPointers[wCol] - wLoop1;
	        while (wLoop1 >= Where.y)
	        {
            #else
	        UCHAR *Put = mpScanPointers[wCol] + Where.y;
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
	                    if (wLoop1 >= View.wTop &&
	                        wLoop1 <= View.wBottom &&
	                        uVal != 0xff)
	                    {
	                        *Put = uVal;
	                    }
	                    Put++;
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
	                    Put += uCount;
	                }
	                else
	                {
	                    while(uCount--)
	                    {
	                        if (wLoop1 >= View.wTop &&
	                            wLoop1 <= View.wBottom)
	                        {
	                            *Put++ = uVal;
	                        }
	                        else
	                        {
	                            Put++;
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
    else
    {
        // here if the source has no transparency, we can run
        // a little bit faster loop:
	    while (wCol <= View.wRight)
	    {
            #ifdef ROTATE_CCW
	        SIGNED wLoop1 = Where.y + Getmap->wHeight - 1;
	        UCHAR *Put = mpScanPointers[wCol] - wLoop1;
	        while (wLoop1 >= Where.y)
	        {
            #else
	        UCHAR *Put = mpScanPointers[wCol] + Where.y;
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
	                    if (wLoop1 >= View.wTop &&
	                        wLoop1 <= View.wBottom)
	                    {
	                        *Put = uVal;
	                    }
	                    Put++;
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
	
	                while(uCount--)
	                {
	                    if (wLoop1 >= View.wTop &&
	                        wLoop1 <= View.wBottom)
	                    {
	                        *Put++ = uVal;
	                    }
	                    else
	                    {
	                        Put++;
	                    }
                        #ifdef ROTATE_CCW
	                    wLoop1--;
                        #else
	                    wLoop1++;
                        #endif
	                }
	            }
	        }
	        wCol++;
	    }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro8Screen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void Pro8Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
                #ifdef ROTATE_CCW
                UCHAR *Put = mpScanPointers[wXpos] - ScanRow;
                #else
                UCHAR *Put = mpScanPointers[wXpos] + ScanRow;
                #endif

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

void Pro8Screen::DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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

            while(wBitsOutput < wCharWidth)
            {
                #ifdef ROTATE_CCW
                UCHAR *Put = mpScanPointers[wXPos] - ScanRow;
                #else
                UCHAR *Put = mpScanPointers[wXPos] + ScanRow;
                #endif

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
void Pro8Screen::HidePointer(void)
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
void Pro8Screen::SetPointer(PegPoint Where)
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
	    Capture(&mCapture, MouseRect);
	        
	    Bitmap(pp, Where, mpCurPointer, TRUE);
	    UNLOCK_PEG
    }
   #endif
#endif
}


/*--------------------------------------------------------------------------*/
void Pro8Screen::SetPointerType(UCHAR bType)
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
void Pro8Screen::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *Pro8Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 232;
    return muPalette;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

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
void Pro8Screen::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
   #ifdef PEGWIN32

    BMhead.bmhead.biSize = sizeof(BITMAPINFOHEADER);
    BMhead.bmhead.biWidth = mwHRes;
    BMhead.bmhead.biHeight = -mwVRes;
    BMhead.bmhead.biPlanes = 1;
    BMhead.bmhead.biBitCount = 8;
    BMhead.bmhead.biCompression = BI_RGB;
    BMhead.bmhead.biSizeImage = mwHRes * mwVRes;

    if (iFirst == 0)        // not appending?
    {
        BMhead.bmhead.biClrUsed = iCount;
        BMhead.bmhead.biClrImportant = iCount;
        WinPal.palNumEntries = iCount;
    }
    else
    {
        if (iFirst + iCount > WinPal.palNumEntries)
        {
            BMhead.bmhead.biClrUsed = iFirst + iCount;
            BMhead.bmhead.biClrImportant = iFirst + iCount;
            WinPal.palNumEntries = iFirst + iCount;
        }
    }

    WinPal.palVersion = 0x0300;

    UCHAR *pPut = muPalette;

    for (WORD loop = iFirst; loop < iCount; loop++)
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
        //WinPal.palPalEntry[loop].peFlags = 0;
        WinPal.palPalEntry[loop].peFlags = PC_RESERVED;
    }
    if (mhPalette)
    {
        DeleteObject(mhPalette);
    }
    mhPalette = CreatePalette((LOGPALETTE *) &WinPal);

   #else

    #error SetPalette() function must be completed.

   #endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Pro8Screen::ConfigureController(void)
{
    #ifdef PEGWIN32
    #else

    #error ConfigureController() function must be completed.

    #endif

}
 
#ifdef PEGWIN32

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the windows screen:

void Pro8Screen::MemoryToScreen(void)
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

    UCHAR *pPut = mpDisplayBuff;

    for (WORD wLoop = 0; wLoop < mwVRes; wLoop++)
    {
        UCHAR *pGet = mpScanPointers[0];

        #ifdef ROTATE_CCW
        pGet -= wLoop;
        for (WORD wLoop1 = 0; wLoop1 < mwHRes; wLoop1++)
        {
            *pPut++ = *pGet;
            pGet += mwVRes;
        }
        #else
        pGet += wLoop;
        for (WORD wLoop1 = 0; wLoop1 < mwHRes; wLoop1++)
        {
            *pPut++ = *pGet;
            pGet -= mwVRes;
        }
        #endif
    }


    SetMapMode(mHdc, MM_TEXT);

    HPALETTE hOldPal = SelectPalette(mHdc, mhPalette, FALSE);
    RealizePalette(mHdc);

    //SIGNED iTop = Copy.wTop;
    //SIGNED iLeft = Copy.wLeft;
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



