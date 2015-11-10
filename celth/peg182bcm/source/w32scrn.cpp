/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// w32scrn.cpp - Instantiable PegWinScreen written to support development
//   of PEG applications in a standard Win32 programming environment.
//   Note that the resolution is completely arbitrary and can be set as 
//     desired to meet the requirements of the final target.
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "memory.h"
#include "peg.hpp"

extern UCHAR DefPalette256[];

UCHAR GrayPalette16[16 * 3] = {
0, 0, 0,
17,17,17,
34,34,34,
51,51,51,
68,68,68,
85,85,85,
102,102,102,
119,119,119,
136,136,136,
153,153,153,
170,170,170,
187,187,187,
204,204,204,
221,221,221,
238,238,238,
255, 255, 255
};


PegScreen *CreateWinScreen(HWND hwnd)
{
    PegRect rSize;
    rSize.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    return (new PegWinScreen(hwnd, rSize));
}

/*--------------------------------------------------------------------------*/
PegWinScreen::PegWinScreen(HWND hwnd, PegRect &Rect) : PegScreen(Rect)
{
    mdNumColors = 256;

    mHWnd = hwnd;
    mpVidMem = new COLORVAL[Rect.Width() * Rect.Height()];
    memset(mpVidMem, 0, Rect.Width() * Rect.Height());
    mwHRes = Rect.Width();
    mwVRes = Rect.Height();

    mpScanPointers = new COLORVAL *[Rect.Height()];
    COLORVAL *CurrentPtr = mpVidMem;

    for (SIGNED iLoop = 0; iLoop < Rect.Height(); iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += mwHRes;
    }

    // the following is only for testing the Video Memory Manager

   #ifdef USE_VID_MEM_MGR

    CurrentPtr = new UCHAR[256 * 1024]; // 256 vid mem buffer

    if (CurrentPtr)
    {
        InitVidMemManager(CurrentPtr, CurrentPtr + (256 * 1024) - 1);
    }

   #endif

    mhPalette = NULL;

    SetPalette(0, 232, DefPalette256);      // pallette used to draw 1, 2, 4, and 8bpp images
    SetPalette(232, 16, GrayPalette16);     // palette used to draw 4bpp grayscale images

   // Setup for the BitBlitting function:

   RECT lSize, lOuterSize;
   ::GetClientRect(mHWnd, &lSize);

   mwWinRectXOffset = (lSize.right - mwHRes) / 2;
   mwWinRectYOffset = (lSize.bottom -mwVRes) / 2;

   // For keeping the mouse in the right position:

   ::GetWindowRect(mHWnd, &lOuterSize);
   mwWinRectMouseXOffset = (SIGNED) lOuterSize.left;
   mwWinRectMouseYOffset = (SIGNED) (lOuterSize.top + (lOuterSize.bottom - lOuterSize.top - lSize.bottom));

}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
PegWinScreen::~PegWinScreen()
{
    delete mpVidMem;

    if (mhPalette)
    {
        DeleteObject(mhPalette);
    }
    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void PegWinScreen::BeginDraw(PegThing *)
{
    LOCK_PEG
    if (!mwDrawNesting)
    {
        mHdc = GetDC(mHWnd);
    }
    mwDrawNesting++;
}

/*--------------------------------------------------------------------------*/
void PegWinScreen::BeginDraw(PegThing *, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        mpScanPointers = new COLORVAL *[pMap->wHeight];
        COLORVAL *CurrentPtr = (COLORVAL *) pMap->pStart;
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
void PegWinScreen::EndDraw()
{
    mwDrawNesting--;

    if (!mwDrawNesting)
    {
        MemoryToScreen();
        ReleaseDC(mHWnd, mHdc);

        while(miInvalidCount)
        {
            miInvalidCount--;
            UNLOCK_PEG
        }
    }
    UNLOCK_PEG
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::EndDraw(PegBitmap *)
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
void PegWinScreen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void PegWinScreen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    SIGNED iLen = wXEnd - wXStart + 1;

    if (!iLen)
    {
        return;
    }
    while(wWidth-- > 0)
    {
        COLORVAL *Put = mpScanPointers[wYPos] + wXStart;
        memset(Put, Color, iLen);
        wYPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    while(wYStart <= wYEnd)
    {
        SIGNED lWidth = wWidth;
        COLORVAL *Put = mpScanPointers[wYStart] + wXPos;

        while (lWidth-- > 0)
        {
            *Put++ = (UCHAR) Color;
        }
        wYStart++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    COLORVAL *Put = mpScanPointers[wYPos] + wXStart;

    while (wXStart <= wXEnd)
    {
        *Put ^= 0x0f;
        Put += 2;
        wXStart += 2;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    while (wYStart <= wYEnd)
    {
        COLORVAL *Put = mpScanPointers[wYStart] + wXPos;
        *Put ^= 0x0f;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void PegWinScreen::Capture(PegCapture *Info, PegRect &CaptureRect)
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

    COLORVAL *GetStart = mpScanPointers[CaptureRect.wTop] + CaptureRect.wLeft;

    // make room for the memory bitmap:

    pMap->uFlags = 0;        // raw format
    pMap->uBitsPix = 8;

    // fill in the image with our captured info:

    COLORVAL *Put = (COLORVAL *) pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        COLORVAL *Get = GetStart;
        for (WORD wLoop = 0; wLoop < pMap->wWidth; wLoop++)
        {
            *Put++ = *Get++;
        }
        GetStart += mwHRes;
    }
    Info->SetValid(TRUE);
}



/*--------------------------------------------------------------------------*/
// BitmapView::
// The MS Windows version of BitmapView can draw any color depth bitmap. This
// is the only real difference between this screen driver and the l8scrn.cpp
// screen driver template.
/*--------------------------------------------------------------------------*/
void PegWinScreen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
        return;
    }

    switch(Getmap->uBitsPix)
    {
    case 1:
        DrawMonochromeBitmap(Where, Getmap, View);
        break;

    case 2:
        DrawFourGrayBitmap(Where, Getmap, View);
        break;

    case 4:
        if (IS_GRAYSCALE(Getmap))
        {
            Draw16GrayBitmap(Where, Getmap, View);
        }
        else
        {
            Draw16ColorBitmap(Where, Getmap, View);
        }
        break;

    case 8:
        Draw256ColorBitmap(Where, Getmap, View);
        break;

    case 16:
        DrawHiColorBitmap(Where, Getmap, View);
        break;

    case 24:
        DrawTrueColorBitmap(Where, Getmap, View);
        break;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::DrawMonochromeBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uiPix, uVal1;
    SIGNED iCount;
    UCHAR *Get;

    // always padded to nearest full byte per scan line:

    WORD wBytesPerLine = (Getmap->wWidth + 7) >> 3;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 3;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        Get = GetStart;
        uVal1 = *Get++;
        iCount = View.Width();
        UCHAR *pPut = mpScanPointers[wLine] + View.wLeft;

        uiPix = (View.wLeft - Where.x) & 7;
        uVal1 <<= uiPix;

        while (iCount-- > 0)
        {
            uVal = uVal1 >> 7;

            if (uVal)
            {
                *pPut++ = WHITE;
            }
            else
            {
                *pPut++ = BLACK;
            }
            uiPix++;
            if (uiPix == 8)
            {
                uiPix = 0;
                uVal1 = *Get++;
            }
            else
            {
                uVal1 <<= 1;
            }
        }
        GetStart += wBytesPerLine;
    }
}

UCHAR FourGrayTransPalette[4] = {BLACK, DARKGRAY, LIGHTGRAY, WHITE};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::DrawFourGrayBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uiPix, uVal1;
    SIGNED iCount;
    UCHAR *Get;

    // always padded to nearest full byte per scan line:

    WORD wBytesPerLine = (Getmap->wWidth + 3) >> 2;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 2;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        Get = GetStart;
        uVal1 = *Get++;
        iCount = View.Width();
        UCHAR *Put = mpScanPointers[wLine] + View.wLeft;

        uiPix = ((View.wLeft - Where.x) & 3) << 1;
        uVal1 <<= uiPix;

        while (iCount-- > 0)
        {
            uVal = uVal1 >> 6;

            if (uVal != 0 && uVal != 3)
            {
                *Put = 0;
            }
            *Put++ = FourGrayTransPalette[uVal];
            uiPix += 2;
            if (uiPix == 8)
            {
                uiPix = 0;
                uVal1 = *Get++;
            }
            else
            {
                uVal1 <<= 2;
            }
        }
        GetStart += wBytesPerLine;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::Draw16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal1;
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
        UCHAR bNibble = 0;

        UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
        uVal1 = *Get++;

        if (Where.x & 1)
        {
            // do the first pixel:
            *Put++ = uVal1 >> 4;
            iCount--;
            bNibble = 1;
        }

        while (iCount-- >= 0)
        {
            if (bNibble)
            {
                *Put++ = uVal1 & 0x0f;
                uVal1 = *Get++;
                bNibble = 0;
            }
            else
            {
                *Put++ = uVal1 >> 4;
                bNibble = 1;
            }
        }
        GetStart += wBytesPerLine;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::Draw16GrayBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal1;
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
        UCHAR bNibble = 0;

        UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
        uVal1 = *Get++;

        if (Where.x & 1)
        {
            // do the first pixel:
            *Put++ = (uVal1 >> 4) + GRAY_PALETTE_INDEX;
            iCount--;
            bNibble = 1;
        }

        while (iCount-- > 0)
        {
            if (bNibble)
            {
                *Put++ = (uVal1 & 0x0f) + GRAY_PALETTE_INDEX;
                uVal1 = *Get++;
                bNibble = 0;
            }
            else
            {
                *Put++ = (uVal1 >> 4) + GRAY_PALETTE_INDEX;
                bNibble = 1;
            }
        }

        GetStart += wBytesPerLine;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::Draw256ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR *Get = Getmap->pStart;
    Get += (View.wTop - Where.y) * Getmap->wWidth;
    Get += View.wLeft - Where.x;

    if (HAS_TRANS(Getmap))
    {
        UCHAR uTrans = (UCHAR) Getmap->dTransColor;

        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
            {
                UCHAR uVal = *Get++;
    
                if (uVal != uTrans)
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
            COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
        
            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
            {   
                *Put++ = *Get++;
            }
            Get += Getmap->wWidth - View.Width();
        }
    }
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::DrawHiColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    WORD wColor;
    int iColor;

    WORD *Get = (WORD *) Getmap->pStart;
    Get += (View.wTop - Where.y) * Getmap->wWidth;
    Get += View.wLeft - Where.x;

    if (HAS_TRANS(Getmap))
    {
        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
            {
                wColor = *Get++;

                if (wColor != Getmap->dTransColor)
                {
                    iColor = ((((wColor << 3) & 0xf8) + 25) / 51) * 36;
                    iColor += ((((wColor >> 3) & 0xf8) + 25) / 51) * 6;
                    iColor += (((wColor >> 8) & 0xf8) + 25) / 51;
                    iColor += 16;
                    *Put = (UCHAR) iColor;
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
            COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
            {
                WORD wColor = *Get++;

                iColor = ((((wColor << 3) & 0xf8) + 25) / 51) * 36;
                iColor += ((((wColor >> 3) & 0xf8) + 25) / 51) * 6;
                iColor += (((wColor >> 8) & 0xf8) + 25) / 51;
                iColor += 16;
                *Put++ = (UCHAR) iColor;
            }
            Get += Getmap->wWidth - View.Width();
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::DrawTrueColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uRed, uGreen, uBlue;
    int iColor;

    UCHAR *Get = Getmap->pStart;
    Get += (View.wTop - Where.y) * Getmap->wWidth * 3;
    Get += (View.wLeft - Where.x) * 3;

    if (HAS_TRANS(Getmap))
    {
        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
            {
                uRed = *Get++;
                uGreen = *Get++;
                uBlue = *Get++;

                if (uRed != 0 || uGreen != 0 || uBlue != 1)
                {
                    iColor = uBlue / 51 * 36;
                    iColor += uGreen / 51 * 6;
                    iColor += uRed / 51;
                    iColor += 16;
                    *Put = (UCHAR) iColor;
                }
                Put++;
            }
            Get += (Getmap->wWidth - View.Width()) * 3;
        }
    }
    else
    {
        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
            {
                uRed = *Get++;
                uGreen = *Get++;
                uBlue = *Get++;
                iColor = ((int) uBlue + 25)/ 51 * 36;
                iColor += ((int) uGreen + 25) / 51 * 6;
                iColor += ((int) uRed  + 25) / 51;

                iColor += 16;
                *Put++ = (UCHAR) iColor;
            }
            Get += (Getmap->wWidth - View.Width()) * 3;
        }
    }

}

long dummy;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::DrawRleBitmap(const PegPoint Where, const PegRect View,
    const PegBitmap *Getmap)
{
    UCHAR *Get = Getmap->pStart;
    UCHAR uColor;
    WORD wColor;
    WORD Index;
    UCHAR uRed, uGreen, uBlue;
    BYTE bBytesPerPix;
    LONG lTestColor;
    SIGNED uCount;
    LONG lTrans = 0xff;
    
    if (HAS_TRANS(Getmap))
    {
        lTrans = (UCHAR) Getmap->dTransColor;
    }

    switch(Getmap->uBitsPix)
    {
    case 1:
    case 2:
    case 4:
    case 8:
        bBytesPerPix = 1;
        break;

    case 16:
        bBytesPerPix = 2;
        lTrans = 1;
        break;

    case 24:
        bBytesPerPix = 3;
        lTrans = 1;
        break;
    }

    SIGNED wLine = Where.y;

    while (wLine < View.wTop)
    {
        uCount = 0;

        while(uCount < Getmap->wWidth)
        {
            uColor = *Get++;
            if (uColor & 0x80)
            {
                uColor = (uColor & 0x7f) + 1;
                uCount += uColor;
                Get += uColor * bBytesPerPix;
            }
            else
            {
                Get += bBytesPerPix;
                uCount += uColor + 1;
            }
        }
        wLine++;
    }

    while (wLine <= View.wBottom)
    {
        COLORVAL *pPut = mpScanPointers[wLine] + Where.x;
        SIGNED wLoop1 = Where.x;

        while (wLoop1 < Where.x + Getmap->wWidth)
        {
            uColor = *Get++;

            if (uColor & 0x80)        // raw packet?
            {
                uCount = (uColor & 0x7f) + 1;

                while (uCount--)
                {
                    switch(Getmap->uBitsPix)
                    {
                    case 1:
                    case 2:
                    case 4:
                    case 8:
                        uColor = *Get++;
                        break;

                    case  16:
                        wColor = *(Get + 1);
                        wColor <<= 8;
                        wColor |= *Get;
                        Get += 2;
                        break;

                    case 24:
                        uRed = *Get++;
                        uGreen = *Get++;
                        uBlue = *Get++;
                        break;
                    }

                    if (wLoop1 >= View.wLeft &&
                        wLoop1 <= View.wRight)
                    {
                        switch(Getmap->uBitsPix)
                        {
                        case 1:
                            if (uColor != lTrans)
                            {
                                if (uColor)
                                {
                                    *pPut = WHITE;
                                }
                                else
                                {
                                    *pPut = BLACK;
                                }
                            }
                            break;

                        case 2:
                            if (uColor != lTrans)
                            {
                                *pPut = FourGrayTransPalette[uColor];
                            }
                            break;

                        case 4:
                            if (uColor != lTrans)
                            {
                                if (IS_GRAYSCALE(Getmap))
                                {
                                    *pPut = uColor + GRAY_PALETTE_INDEX;
                                }
                                else
                                {
                                    *pPut = uColor;
                                }
                            }
                            break;

                        case 8:
                            if (uColor != lTrans)
                            {
                                *pPut = uColor;
                            }
                            break;

                        case 16:
                            if (wColor != lTrans || !HAS_TRANS(Getmap))
                            {
                                Index = ((((wColor << 3) & 0xf8) + 25) / 51) * 36;
                                Index += ((((wColor >> 3) & 0xf8) + 25) / 51) * 6;
                                Index += (((wColor >> 8) & 0xf8) + 25) / 51;
                                Index += 16;                            
                                *pPut = (UCHAR) Index;
                            }
                            break;

                        case 24:
                            if (!HAS_TRANS(Getmap) ||uRed || uGreen || uBlue != 1)
                            {
                                Index = ((int) uBlue + 25)/ 51 * 36;
                                Index += ((int) uGreen + 25) / 51 * 6;
                                Index += ((int) uRed  + 25) / 51;
                                Index += 16;                            
                                *pPut = (UCHAR) Index;
                            }
                            else
                            {
                                dummy++;
                            }
                            break;
                        }
                    }
                    pPut++;
                    wLoop1++;
                }
            }
            else
            {
                uCount = uColor + 1;

                switch(Getmap->uBitsPix)
                {
                case 1:
                case 2:
                case 4:
                case 8:
                    uColor = *Get++;
                    lTestColor = uColor;
                    break;

                case 16:
                    wColor = *(Get + 1);
                    wColor <<= 8;
                    wColor |= *Get;
                    Get += 2;
                    lTestColor = wColor;
                    break;

                case 24:
                    uRed = *Get++;
                    uGreen = *Get++;
                    uBlue = *Get++;
                    lTestColor = uBlue;
                    lTestColor <<= 8;
                    lTestColor |= uGreen;
                    lTestColor <<= 8;
                    lTestColor |= uRed;
                    break;      
                }

                if (lTestColor == lTrans && HAS_TRANS(Getmap))
                {
                    wLoop1 += uCount;
                    pPut += uCount;
                }
                else
                {
                    while(uCount--)
                    {
                        if (wLoop1 >= View.wLeft &&
                            wLoop1 <= View.wRight)
                        {
                            switch(Getmap->uBitsPix)
                            {
                            case 1:
                                if (uColor)
                                {
                                    *pPut = WHITE;
                                }
                                else
                                {
                                    *pPut = BLACK;
                                }
                                break;

                            case 2:
                                *pPut = FourGrayTransPalette[uColor];
                                break;

                            case 4:
                                if (IS_GRAYSCALE(Getmap))
                                {
                                    *pPut = uColor + GRAY_PALETTE_INDEX;
                                }
                                else
                                {
                                    *pPut = uColor;
                                }
                                break;
                                
                            case 8:
                                *pPut = uColor;
                                break;

                            case 16:
                                Index = ((((wColor << 3) & 0xf8) + 25) / 51) * 36;
                                Index += ((((wColor >> 3) & 0xf8) + 25) / 51) * 6;
                                Index += (((wColor >> 8) & 0xf8) + 25) / 51;
                                Index += 16;                            
                                *pPut = (UCHAR) Index;
                                break;

                            case 24:
                                Index = ((int) uBlue + 25)/ 51 * 36;
                                Index += ((int) uGreen + 25) / 51 * 6;
                                Index += ((int) uRed  + 25) / 51;
                                Index += 16;                            
                                *pPut = (UCHAR) Index;
                                break;
                            }
                        }
                        pPut++;
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
void PegWinScreen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void PegWinScreen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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

            COLORVAL *Put = mpScanPointers[ScanRow] + wXpos;

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
// val = 2 = background color
// val = 3 = undefined (treated as transparent)
/*--------------------------------------------------------------------------*/

void PegWinScreen::DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
    WORD  wBitOffset;
    SIGNED  wXpos = Where.x;
    PEGCHAR cVal = *pCurrentChar++;

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
        #endif

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
            signed char cValShift = 6;
            cValShift -= (wBitOffset & 3) * 2;
            WORD wBitsOutput = 0;
            UCHAR cData = *pGetData++;
            COLORVAL *Put = mpScanPointers[ScanRow] + wXpos;

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
                        *Put = (UCHAR) Color.uBackground;
                        break;
                    
                    case 2:
                        *Put = (UCHAR) Color.uForeground;
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
void PegWinScreen::SetPointerType(UCHAR bType)
{
    if (bType < NUM_POINTER_TYPES)
    {
        mpCurPointer = mpPointers[bType].Bitmap;
        miCurXOffset = mpPointers[bType].xOffset;
        miCurYOffset = mpPointers[bType].yOffset;

        #if defined(PEG_MULTITHREAD)

        // Post a message to the top level Win32 window so
        // that no matter which calling thread wishes to change
        // the cursor type, the thread controlling the Win32 
        // window does the work in the WndProc function.
        PostMessage(mHWnd, WM_SETCURSOR, (WPARAM)bType, 0);

        #else

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
    }
}


#ifdef PEG_FULL_GRAPHICS

#endif      // PEG_FULL_GRAPHICS if

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


BMHEAD BMhead;

typedef struct
{
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[256];
} PEG_WIN_PALETTE;

PEG_WIN_PALETTE WinPal;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegWinScreen::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
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

    UCHAR *pPut = muPalette + (iFirst * 3);

    for (WORD loop = iFirst; loop < iFirst + iCount; loop++)
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
}

/*--------------------------------------------------------------------------*/
void PegWinScreen::ResetPalette(void)
{
     SetPalette(0, 232, DefPalette256);
     SetPalette(232, 16, GrayPalette16);
}

             
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *PegWinScreen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = WinPal.palNumEntries;
    return muPalette;
}
 

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the windows screen:

void PegWinScreen::MemoryToScreen(void)
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
		  mpVidMem, (const struct tagBITMAPINFO *) &BMhead,
		  DIB_RGB_COLORS, SRCCOPY);

	 SelectObject(mHdc, hOldPal);
}


#ifdef SIZE_TO_WIN_CLIENT

void PegWinScreen::SetNewWindowsWinSize(WORD wWidth, WORD wHeight)
{
	 LOCK_PEG

	 mwWinRectXOffset = 0;
	 mwWinRectYOffset = 0;

	 if (mpVidMem)
	 {
		  delete [] mpVidMem;
		  delete [] mpScanPointers;
	 }

	 wWidth &= 0xfffffffCL;
	 wHeight &= 0xfffffffCL;
	 mwHRes = wWidth;
	 mwVRes = wHeight;

	 mpVidMem = new COLORVAL[(LONG) wWidth * (LONG) wHeight];

	 if (!mpVidMem)
	 {
		  // not enough memory for screen. Try going back to default
		  // size:

		  mpVidMem = new COLORVAL[PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE];
		  mwHRes = PEG_VIRTUAL_XSIZE;
		  mwVRes = PEG_VIRTUAL_YSIZE;
		  mwWinRectXOffset = (wWidth - mwHRes) / 2;
		  mwWinRectYOffset = (wHeight - mwVRes) / 2;
	 }


	 mpScanPointers = new COLORVAL *[mwVRes];
	 COLORVAL *CurrentPtr = mpVidMem;

	 for (SIGNED iLoop = 0; iLoop < mwVRes; iLoop++)
	 {
		  mpScanPointers[iLoop] = CurrentPtr;
		  CurrentPtr += mwHRes;
	 }

	 BMhead.bmhead.biWidth = mwHRes;
	 BMhead.bmhead.biHeight = -mwVRes;
	 BMhead.bmhead.biSizeImage = mwHRes * mwVRes;

	 // resize the presentation manager to the new screen size:

	 PegThing *pThing = NULL;
	 PegPresentationManager *pPresent = pThing->Presentation();
	 PegRect NewSize;
	 NewSize.Set(0, 0, mwHRes - 1, mwVRes - 1);
	 pPresent->mReal = NewSize;
	 pPresent->mClient = NewSize;
	 pPresent->mClip = NewSize;
	 pPresent->UpdateChildClipping();

    #ifdef PEG_FULL_CLIPPING
	 GenerateViewportList(pPresent);
    #endif

	 pThing = pPresent->First();
	 while(pThing)
	 {
		  pThing->Message(PegMessage(NULL, PM_PARENTSIZED));
		  pThing = pThing->Next();
	 }
    Invalidate(NewSize);
    pPresent->Draw();

    UNLOCK_PEG
}

#endif 



