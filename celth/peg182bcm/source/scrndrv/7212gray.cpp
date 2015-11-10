/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 7212gray.cpp - PegScreen driver for 2 bit-per-pixel operation on the
//      Cirrus Logic ARM7 development platform. This screen driver was
//      created by copying the l2scrn screen driver template, and adding in
//      the video controller configuration code. Any other color depth or
//      resolution can easily be created following this procedure.        
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
// Known Limitations:
// 
// This driver implements off-screen drawing using the native 2-bpp format.
// This means that transparency information is lost if a bitmap is drawn
// to the off-screen buffer. The tradeoff here is that off-screen drawing
// uses less memory, while supporting transparency in off-screen drawing
// would quadruple the memory requirements for the off-screen bitmap.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"
#include "cl7212.h"

UCHAR DefPalette[4*3] = {
0, 0, 0,        // black
128, 128, 128,  // darkgray
192, 192, 192,  // lightgray
255, 255, 255   // white
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new CLGrayScreen(Rect);
    return pScreen;
}


/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
CLGrayScreen::CLGrayScreen(PegRect &Rect) : PegScreen(Rect)
{
    mdNumColors = 4;  


    mwHRes = Rect.Width();
    mwVRes = Rect.Height();

    mpScanPointers = new UCHAR PEGFAR *[Rect.Height()];

    UCHAR PEGFAR *CurrentPtr = GetVideoAddress();

    for (SIGNED iLoop = 0; iLoop < Rect.Height(); iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += mwHRes >> 2;
    }

    mLastPointerPos.x = Rect.Width() / 2;
    mLastPointerPos.y = Rect.Height() / 2;
    mbPointerHidden = FALSE;
    mwDrawNesting = 0;

    ConfigureController();        // set up controller registers

    SetPalette(0, 4, DefPalette);

}




/*--------------------------------------------------------------------------*/
// *** This function must be filled in by the developer ***
/*--------------------------------------------------------------------------*/
UCHAR *CLGrayScreen::GetVideoAddress(void)
{
    return (UCHAR *) VID_MEM_BASE;
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
CLGrayScreen::~CLGrayScreen()
{
    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void CLGrayScreen::BeginDraw(PegThing *)
{
    LOCK_PEG
    if (!mwDrawNesting)
    {
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
void CLGrayScreen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        WORD wBytesPerLine = (pMap->wWidth + 3) >> 2;

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
void CLGrayScreen::EndDraw()
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

        //MemoryToScreen();

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
void CLGrayScreen::EndDraw(PegBitmap *pMap)
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
PegBitmap *CLGrayScreen::CreateBitmap(SIGNED wWidth, SIGNED wHeight,
    BOOL bHasTrans)
{
   PegBitmap *pMap = NULL;

    if (wWidth && wHeight)
    {
        pMap = new PegBitmap;
        pMap->wWidth = wWidth;
        pMap->wHeight = wHeight;
        wWidth += 3;
        wWidth >>= 2;
        pMap->pStart = new UCHAR[(DWORD) wWidth * (DWORD) wHeight];
        if (!pMap->pStart)
        {
            delete pMap;
            return NULL;
        }

        if (bHasTrans)
        {
			memset(pMap->pStart, TRANSPARENCY, (DWORD) wWidth * (DWORD) wHeight);
            pMap->uFlags = BMF_HAS_TRANS;
        }
        else
        {
            pMap->uFlags = 0;
        }
        pMap->uBitsPix = 2;
    }
    return pMap;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLGrayScreen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void CLGrayScreen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR *Put;
    UCHAR uVal;
    UCHAR uFill = (UCHAR) (Color | (Color << 2) | (Color << 4) | (Color << 6));

    while(wWidth-- > 0)
    {
        SIGNED iLen = wXEnd - wXStart + 1;
        Put = mpScanPointers[wYPos] + (wXStart >> 2);

        switch(wXStart & 3)
        {
        case 1:
            uVal = *Put;
            if (iLen >= 3)
            {
                uVal &= 0x03;
                uVal |= uFill & 0xfc;
                *Put++ = uVal;
                iLen -= 3;
            }
            else
            {
                PlotPointView(wXStart, wYPos, Color);

                if (iLen > 1)
                {
                    PlotPointView(wXStart + 1, wYPos, Color);
                }
                iLen = 0;
            }
            break;

        case 2:
            uVal = *Put;
            if (iLen >= 2)
            {
                uVal &= 0x0f;
                uVal |= uFill & 0xf0;
                *Put++ = uVal;
                iLen -= 2;
            }
            else
            {
                PlotPointView(wXStart, wYPos, Color);
                iLen = 0;
            }
            break;

        case 3:
            uVal = *Put;
            uVal &= 0x3f;
            uVal |= uFill & 0xC0;
            *Put++ = uVal;
            iLen--;
            break;

        default:
            break;
        }

        // most compilers seem to do a good job of optimizing 
        // memset to do 32-bit data writes. If your compiler doesn't
        // make the most of your CPU, you might want to re-write this
        // in assembly.

        if (iLen > 0)
        {
            memset(Put, uFill, iLen >> 2);
            Put += iLen >> 2;

            switch(wXEnd & 3)
            {
            case 2:
                uVal = *Put;
                uVal &= 0xc0;
                uVal |= uFill & 0x3f;
                *Put = uVal;
                break;

            case 1:
                uVal = *Put;
                uVal &= 0xf0;
                uVal |= uFill & 0x0f;
                *Put = uVal;
                break;

            case 0:
                uVal = *Put;
                uVal &= 0xfc;
                uVal |= uFill & 0x03;
                *Put = uVal;
                break;
            }
        }
        wYPos++;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLGrayScreen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR uVal, uShift, uMask, uFill;
    UCHAR *Put;

    while(wWidth--)
    {
        uShift = (wXPos & 3) << 1;
        uFill = (UCHAR) (Color << uShift);
        uMask = 0x03 << uShift;
        Put = mpScanPointers[wYStart] + (wXPos >> 2);
        SIGNED iLen = wYEnd - wYStart;

        while(iLen-- >= 0)
        {
            uVal = *Put;
            uVal &= ~uMask;
            uVal |= uFill;
            *Put = uVal;
            Put += mwHRes >> 2;
        }
        wXPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLGrayScreen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    UCHAR *Put = mpScanPointers[wYPos] + (wXStart >> 2);
    SIGNED iLen = wXEnd - wXStart;

    if (!iLen)
    {
        return;
    }
    while(wXStart < wXEnd - 1)
    {
        *Put++ ^= 0x33;
        wXStart += 4;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLGrayScreen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    UCHAR uVal = 0xc0 >> ((wXPos & 3) << 1);

    while (wYStart <= wYEnd)
    {
        UCHAR *Put = mpScanPointers[wYStart] + (wXPos >> 2);
        *Put ^= uVal;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void CLGrayScreen::Capture(PegCapture *Info, PegRect &CaptureRect)
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

    while (CaptureRect.wLeft & 3)
    {
        CaptureRect.wLeft--;
    }

    while ((CaptureRect.wRight & 3) != 3)
    {
        CaptureRect.wRight++;
    }

    Info->SetPos(CaptureRect);
    LONG Size = (pMap->wWidth * pMap->wHeight) >> 2;
    Info->Realloc(Size);

    UCHAR *GetStart = mpScanPointers[CaptureRect.wTop] + (CaptureRect.wLeft >> 2);

    // make room for the memory bitmap:

    pMap->uFlags = BMF_NATIVE;       // native format
    pMap->uBitsPix = 2;              // 2 bits per pixel

    // fill in the image with our captured info:

    UCHAR *Put = pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        memcpy(Put, GetStart, pMap->wWidth >> 2);
        GetStart += mwHRes >> 2;
        Put += pMap->wWidth >> 2;
    }
    Info->SetValid(TRUE);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLGrayScreen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uVal1, uPix;
    UCHAR *Get, *Put;

    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
        return;
    }
    if (Getmap->uBitsPix == 2)
    {
        if (Getmap->uFlags & BMF_NATIVE)
        {
            if ((View.wLeft & 3) == (Where.x & 3))
            {
                DrawFastNativeBitmap(Where, Getmap, View);
            }
            else
            {
                DrawSlowNativeBitmap(Where, Getmap, View);
            }
        }
        else
        {
            Draw4ColorBitmap(Where, Getmap, View);
        }
        return;
    }

    // here for a source bitmap of 8-bpp:

    SIGNED iWidth = View.Width();
    Get = Getmap->pStart;
    Get += (View.wTop - Where.y) * Getmap->wWidth;
    Get += View.wLeft - Where.x;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        Put = mpScanPointers[wLine] + (View.wLeft >> 2);
        uVal = *Put;
        uPix = (View.wLeft & 3) << 1;

        for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
        {
            uVal1 = *Get++;
            if (uVal1 != 0xff)
            {
                uVal &= ~(0x03 << uPix);
                uVal |= uVal1 << uPix;
            }
            if (uPix == 6)
            {
                *Put++ = uVal;
                uVal = *Put;
                uPix = 0;
            }
            else
            {
                uPix += 2;
            }
        }

        if (uPix)
        {
            *Put = uVal;
        }
        Get += Getmap->wWidth - iWidth;
    }
}


/*--------------------------------------------------------------------------*/
void CLGrayScreen::DrawFastNativeBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect View)
{
    UCHAR uVal;
    WORD wBytesPerLine = (Getmap->wWidth + 3) >> 2;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 2;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        UCHAR *Get = GetStart;
        UCHAR *Put = mpScanPointers[wLine] + (View.wLeft >> 2);
        SIGNED iCount = View.Width();

        switch(View.wLeft & 3)
        {
        case 0:
            break;

        case 1:
            uVal = *Put;
            uVal &= 0x03;
            uVal |= *Get++ & 0xfc;
            *Put++ = uVal;
            iCount -= 3;
            break;

        case 2:
            uVal = *Put;
            uVal &= 0x0f;
            uVal |= *Get++ & 0xf0;
            *Put++ = uVal;
            iCount -= 2;
            break;

        case 3:
            uVal = *Put;
            uVal &= 0x3f;
            uVal |= *Get++ & 0xc0;
            *Put++ = uVal;
            iCount--;
            break;
        }

        if (iCount > 0)
        {       
            // copy 4 pixels at a time:

            memcpy(Put, Get, iCount >> 2);
            Put += iCount >> 2;
            Get += iCount >> 2;

            switch (View.wRight & 3)
            {
            case 3:
                break;

            case 2:
                uVal = *Put;
                uVal &= 0xc0;
                uVal |= *Get & 0x3f;
                *Put = uVal;
                break;

            case 1:
                uVal = *Put;
                uVal &= 0xf0;
                uVal |= *Get & 0x0f;
                *Put = uVal;
                break;

            case 0:
                uVal = *Put;
                uVal &= 0xfc;
                uVal |= *Get & 0x03;
                *Put = uVal;
                break;
            }
        }
        GetStart += wBytesPerLine;
    }
}


/*--------------------------------------------------------------------------*/
void CLGrayScreen::DrawSlowNativeBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect View)
{
    WORD wBytesPerLine = (Getmap->wWidth + 3) >> 2;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 2;

    UCHAR uInPix = ((View.wLeft - Where.x) & 3) << 1;
    UCHAR uOutPix = (View.wLeft & 3) << 1;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        UCHAR *Get = GetStart;
        UCHAR *Put = mpScanPointers[wLine] + (View.wLeft >> 2);
        SIGNED iCount = View.Width();

        UCHAR uOutVal = *Put;   // read dest value
        uOutVal <<= 8 - uOutPix;    // pre-shift the output value
        UCHAR uOutCount = uOutPix;
        UCHAR uInCount = uInPix;

        UCHAR uInVal = *Get++;
        uInVal >>= uInPix;

        while(iCount-- > 0)
        {
            uOutVal >>= 2;
            uOutVal |= uInVal << 6;

            uOutCount += 2;

            if (uOutCount == 8)
            {
                *Put++ = uOutVal;
                uOutVal = *Put;
                uOutCount = 0;
            }

            uInCount += 2;
            if (uInCount == 8)
            {
                uInVal = *Get++;
                uInCount = 0;
            }
            else
            {
                uInVal >>= 2;
            }
        }
        if (uOutCount)
        {
            uInVal = *Put;
            uInVal &= 0xff << uOutCount;
            uOutVal >>= 8 - uOutCount;
            uInVal |= uOutVal;
            *Put = uInVal;
            //*Put = uOutVal;
        }
        GetStart += wBytesPerLine;
    }
}



/*--------------------------------------------------------------------------*/
// here for a bitmap that is in the native bit-swapped format.
/*--------------------------------------------------------------------------*/
void CLGrayScreen::Draw4ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    WORD wBytesPerLine = (Getmap->wWidth + 3) >> 2;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 2;

    UCHAR uInPix = ((View.wLeft - Where.x) & 3) << 1;
    UCHAR uOutPix = (View.wLeft & 3) << 1;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        UCHAR *Get = GetStart;
        UCHAR *Put = mpScanPointers[wLine] + (View.wLeft >> 2);
        SIGNED iCount = View.Width();

        UCHAR uOutVal = *Put;   // read dest value
        uOutVal <<= 8 - uOutPix;    // pre-shift the output value
        UCHAR uOutCount = uOutPix;
        UCHAR uInCount = uInPix;

        UCHAR uInVal = *Get++;
        uInVal <<= uInPix;

        while(iCount-- > 0)
        {
            uOutVal >>= 2;
            uOutVal |= uInVal & 0xc0;

            uOutCount += 2;

            if (uOutCount == 8)
            {
                *Put++ = uOutVal;
                uOutCount = 0;
                uOutVal = 0;
            }

            uInCount += 2;
            if (uInCount == 8)
            {
                uInVal = *Get++;
                uInCount = 0;
            }
            else
            {
                uInVal <<= 2;
            }
        }

        if (uOutCount)
        {
            uInVal = *Put;
            uInVal &= 0xff << uOutCount;
            uOutVal >>= 8 - uOutCount;
            uInVal |= uOutVal;
            *Put = uInVal;
        }
        GetStart += wBytesPerLine;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLGrayScreen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
                        uVal != 0xff)
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

                if (uVal == 0xff)
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
void CLGrayScreen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void CLGrayScreen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
void CLGrayScreen::HidePointer(void)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (mbShowCursor)
    {
        PegThing *pt = NULL;
        PegPresentationManager *pp = pt->Presentation();
        Restore(pp, &mCapture, TRUE);
        mCapture.SetValid(FALSE);
    }
   #endif
}

/*--------------------------------------------------------------------------*/
void CLGrayScreen::SetPointer(PegPoint Where)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (mbShowCursor)
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
void CLGrayScreen::SetPointerType(UCHAR bType)
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
void CLGrayScreen::ResetPalette(void)
{
    SetPalette(0, 4, DefPalette);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *CLGrayScreen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 4;
    return muPalette;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLGrayScreen::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *pGet)
{
//      ** Error- the SetPalette function must be filled in **
}



//****************************************************************************
//
// LCDOn turns on the LCD panel.
//
//****************************************************************************
void LCDOn(void) 
{
    DWORD *pReg;
    UCHAR *pData;
    DWORD dDelay;

    //
    // Power up the LCD controller.
    //
    pReg = (DWORD *) REG_SYSCON1;
    *pReg |= SYSCON_LCDEN;

    //
    // Power up the LCD panel.
    //

    pData = (UCHAR *) PORTD_DATA;
    *pData |= PORTD_LCDEN;

    //
    // Delay for a little while.
    //
    for(dDelay = 0; dDelay < 65536 * 4; dDelay++)
    {
    }

    //
    // Power up the LCD DC-DC converter.
    //
    *pData |= PORTD_LCD_DC_DC;
}

//****************************************************************************
//
// LCDOff turns off the LCD panel.
//
//****************************************************************************
void LCDOff(void)
{
    DWORD *pReg;
    UCHAR *pData;
    DWORD dDelay;

    //
    // Power off the LCD DC-DC converter.
    //
    pData = (UCHAR *) PORTD_DATA;
    *pData &= ~PORTD_LCD_DC_DC;

    //
    // Delay for a little while.
    //
    for(dDelay = 0; dDelay < 65536 * 4; dDelay++)
    {
    }

    //
    // Power off the LCD panel.
    //
    *pData &= ~PORTD_LCDEN;

    //
    // Power off the LCD controller.
    //
    pReg = (DWORD *) REG_SYSCON1;
    *pReg &= ~SYSCON_LCDEN;
}



//****************************************************************************
//
// LCDBacklightOn turns on the backlighting on the LCD panel.
//
//****************************************************************************
void LCDBacklightOn(void)
{
    UCHAR *pReg = (UCHAR *) PORTD_DATA;
    *pReg |= PORTD_LCDBL;
}

//****************************************************************************
//
// LCDBacklightOff turns off the backlighting on the LCD panel.
//
//****************************************************************************
void LCDBacklightOff(void)
{
    UCHAR *pReg = (UCHAR *) PORTD_DATA;
    *pReg &= ~PORTD_LCDBL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLGrayScreen::ConfigureController(void)
{
    DWORD *pReg;
    unsigned long ulLcdConfig, ulLineLength, ulVideoBufferSize;

    //
    // Determine the value to be programmed into the LCD controller in the
    // CL-PS7111 to properly drive the ALPS LCD panel at 640x240, 2 bits per
    // pixel, at 80Hz.

    ulVideoBufferSize = ((mwHRes * mwVRes * 2) / 128) - 1;
    ulLineLength = (mwHRes / 16) - 1;

    ulLcdConfig = LCD_GRAYSCALE        |  
                  LCD_AC_PRESCALE      |
                  LCD_PIX_PRESCALE     |
                  (ulLineLength << 13) |
                  ulVideoBufferSize;
                  
    //
    // Configure the palette to be a one-to-one mapping of the pixel values to
    // the available LCD pixel intensities.
    //
    
    pReg = (DWORD *) LCD_PALETTE_LSW;                                                        
    *pReg = 0x000005bf;
    pReg = (DWORD *) LCD_PALETTE_MSW;                                                        
    *pReg = 0;

    pReg = (DWORD *) LCD_CONTROL;
    *pReg = ulLcdConfig;

    // clear the video frame buffer:

    UCHAR *pPut = (UCHAR *) VID_MEM_BASE;
    WORD wLoop;

    for(wLoop = 0; wLoop < ((640 * 240) / 4); wLoop++)
    {
        *pPut++ = 0;
    }

    LCDOff();

    // program the frame buffer addresss

    pReg = (DWORD *) LCD_FBADDR;
    *pReg = VID_MEM_BASE >> 28;

    LCDOn();
    LCDBacklightOff();
}

#ifdef DOUBLE_BUFFER

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the windows screen. This is only used
// for testing purposes, and can be deleted.

void CLGrayScreen::MemoryToScreen(void)
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
}

#endif



