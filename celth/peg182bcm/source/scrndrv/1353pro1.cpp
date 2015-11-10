/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1353pro1.cpp - Profile mode monochrome screen driver for the SED 1353
//      video controller.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1998-2000 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// The driver can be configured to run with a screen that has been rotated
// 90 degrees counter-clockwise or 90 degrees clockwise.
//
// This driver does NOT support RUNTIME_COLOR_CHECK operation.
//
// All available configuration flags are found in the 1353pro1.hpp header file.
//
//      *****************  IMPORTANT  ********************
//
//
// Known Limitations:
// 
// This driver implements off-screen drawing using the native 1-bpp format.
// This means that transparency information is lost if a bitmap is drawn
// to the off-screen buffer. The tradeoff here is that off-screen drawing
// uses less memory, while supporting transparency in off-screen drawing
// would quadruple the memory requirements for the off-screen bitmap.
//
// The vertical resolution of the screen (after rotation) MUST be evenly 
// divisible by eight.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"


#include "peg.hpp"

#if defined(PHARLAP) || defined(PEGSMX)
#define __COLORS        // don't want conio colors
#include <conio.h>      // for _inp and _outp on x86
#endif


UCHAR DefPalette[2*3] = {
0, 0, 0,        // black
255, 255, 255   // white
};

/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new ProMono1353Screen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
ProMono1353Screen::ProMono1353Screen(PegRect &Rect) : PegScreen(Rect)
{
    mdNumColors = 2;  

   #ifdef PEGWIN32
    mhPalette = NULL;
   #endif

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();
    miPitch = (mwVRes + 7) >> 3;

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
    CurrentPtr += miPitch * (mwHRes - 1);

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

    SetPalette(0, 2, DefPalette);
}


/*--------------------------------------------------------------------------*/
// *** This function must be filled in by the developer ***
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *ProMono1353Screen::GetVideoAddress(void)
{
    // Depending on your operating system, you may need to Memmap() or similar
    // to gain access to your video frame buffer. We default to using the 
    // SED1353 evaluation card, which had a hard-coded pre-defined address:

    return (UCHAR *) VID_MEM_BASE;
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
ProMono1353Screen::~ProMono1353Screen()
{
  #ifdef PEGWIN32
    delete mpDisplayBuff;

    UCHAR *pMem = mpScanPointers[0];
    
   #ifdef ROTATE_CCW
    pMem -= (mwVRes >> 3);
    pMem++;
   #else
    pMem -= (mwVRes >> 3) * (mwHRes - 1);
   #endif
    delete pMem;

  #endif

    delete mpScanPointers;
}


/*--------------------------------------------------------------------------*/
void ProMono1353Screen::BeginDraw(PegThing *)
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
void ProMono1353Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        WORD wBytesPerLine = (pMap->wHeight + 7) >> 3;

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
void ProMono1353Screen::EndDraw()
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
void ProMono1353Screen::EndDraw(PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        mbVirtualDraw = FALSE;
        delete [] mpScanPointers;
        mpScanPointers = mpSaveScanPointers;
        miPitch = (mwVRes + 7) >> 3;
        UNLOCK_PEG
    }
}

/*--------------------------------------------------------------------------*/
// CreateBitmap: The default version creates an 8-bpp bitmap, so we override
// to create a 1-bpp bitmap.
/*--------------------------------------------------------------------------*/
PegBitmap *ProMono1353Screen::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
{
   PegBitmap *pMap = NULL;

    if (wWidth && wHeight)
    {
        pMap = new PegBitmap;
        pMap->wWidth = wWidth;
        pMap->wHeight = wHeight;
        wHeight += 7;
        wHeight >>= 3;
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
			// fill the whole thing with 0x00:
			memset(pMap->pStart, 0x00, (DWORD) wWidth * (DWORD) wHeight);
			pMap->uFlags = 0;   // raw format
		}
        pMap->uBitsPix = 1;
    }
    return pMap;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ProMono1353Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
    SIGNED wYEnd, PegRect &View, PegColor Color, SIGNED wWidth)
{
    if (wYStart == wYEnd)
    {
        HorizontalLine(View.wLeft, View.wRight, View.wTop, Color.uForeground, wWidth);
        return;
    }
    if (wXStart == wXEnd)
    {
        VerticalLine(View.wTop, View.wBottom, View.wLeft, Color.uForeground, wWidth);
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

    if (View.Contains(wXStart, wYStart) &&
        View.Contains(wXEnd, wYEnd))
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
	                if (wCurx >= View.wLeft &&
	                    wCurx <= View.wRight &&
	                    wpy >= View.wTop &&
	                    wpy <= View.wBottom)
	                {
	                    PlotPointView(wCurx, wpy, Color.uForeground);
	                }
	            }
	
	            for (wpy = wNexty - wWidth / 2;
	                 wpy <= wNexty + wWidth / 2; wpy++)
	            {
	                if (wNextx >= View.wLeft &&
	                    wNextx <= View.wRight &&
	                    wpy >= View.wTop &&
	                    wpy <= View.wBottom)
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
	                if (wpx >= View.wLeft &&
	                    wpx <= View.wRight &&
	                    wCury >= View.wTop &&
	                    wCury <= View.wBottom)
	                {
	                    PlotPointView(wpx, wCury, Color.uForeground);
	                }
	            }
	
	            for (wpx = wNextx - wWidth / 2;
	                 wpx <= wNextx + wWidth / 2; wpx++)
	            {
	                if (wpx >= View.wLeft &&
	                    wpx <= View.wRight &&
	                    wNexty >= View.wTop &&
	                    wNexty <= View.wBottom)
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
void ProMono1353Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR uVal, uShift, uMask;
    COLORVAL uFill;
    UCHAR *Put;

    SIGNED iOffset = miPitch;

   #ifdef ROTATE_CW
    if (mbVirtualDraw)
    {
        iOffset = -miPitch;
    }
   #endif

    while(wWidth-- > 0)
    {
        #ifdef ROTATE_CCW
        uShift = (wYPos & 7);     // 0,0 is lsb 
        #else
        uShift = 7 - (wYPos & 7); // 0,0 is msb
        #endif
        uFill = Color << uShift;
        uMask = ~(0x01 << uShift);

        #ifdef ROTATE_CCW
        Put = mpScanPointers[wXStart] - (wYPos >> 3);
        #else
        Put = mpScanPointers[wXStart] + (wYPos >> 3);
        #endif
        SIGNED iLen = wXEnd - wXStart;

        while(iLen-- >= 0)
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
void ProMono1353Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR *Put;
    UCHAR uVal;
    UCHAR uFill = 0;

    if (Color)
    {
        uFill = 0xff;
    }
    if (wYEnd < wYStart)
    {
        return;
    }
                        
    while(wWidth-- > 0)
    {
        SIGNED iLen = wYEnd - wYStart + 1;

       #ifdef ROTATE_CCW

        Put = mpScanPointers[wXPos] - (wYStart >> 3);

        switch(wYStart & 7)
        {
        case 1:
            if (iLen >= 7)
            {
                uVal = *Put;
                uVal &= 0x01;
                uVal |= uFill & 0xfe;
                *Put-- = uVal;
                iLen -= 7;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXPos, wYStart + iLen, Color);
                }
                wXPos++;
                continue;
            }
            break;

        case 2:
            if (iLen >= 6)
            {
                uVal = *Put;
                uVal &= 0x03;
                uVal |= uFill & 0xfc;
                *Put-- = uVal;
                iLen -= 6;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXPos, wYStart + iLen, Color);
                }
                wXPos++;
                continue;
            }
            break;

        case 3:
            if (iLen >= 5)
            {
                uVal = *Put;
                uVal &= 0x07;
                uVal |= uFill & 0xf8;
                *Put-- = uVal;
                iLen -= 5;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXPos, wYStart + iLen, Color);
                }
                wXPos++;
                continue;
            }
            break;

        case 4:
            if (iLen >= 4)
            {
                uVal = *Put;
                uVal &= 0x0f;
                uVal |= uFill & 0xf0;
                *Put-- = uVal;
                iLen -= 4;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXPos, wYStart + iLen, Color);
                }
                wXPos++;
                continue;
            }
            break;

        case 5:
            if (iLen >= 3)
            {
                uVal = *Put;
                uVal &= 0x1f;
                uVal |= uFill & 0xe0;
                *Put-- = uVal;
                iLen -= 3;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXPos, wYStart + iLen, Color);
                }
                wXPos++;
                continue;
            }
            break;

        case 6:
            if (iLen >= 2)
            {
                uVal = *Put;
                uVal &= 0x3f;
                uVal |= uFill & 0xc0;
                *Put-- = uVal;
                iLen -= 2;
            }
            else
            {
                PlotPointView(wXPos, wYStart, Color);
                wXPos++;
                continue;
            }
            break;

        case 7:
            PlotPointView(wXPos, wYStart, Color);
            Put--;
            iLen--;
            break;

        default:
            break;
        }

        // most compilers seem to do a good job of optimizing 
        // memset to do 32-bit data writes. If your compiler doesn't
        // make the most of your CPU, you might want to re-write this
        // in assembly.

        if (iLen >= 8)
        {
            Put -= (iLen >> 3) - 1;
            memset(Put, uFill, iLen >> 3);
            Put--;
        }
        if (iLen)
        {
            switch(wYEnd & 7)
            {
            case 6:
                uVal = *Put;
                uVal &= 0x80;
                uVal |= uFill & 0x7f;
                *Put = uVal;
                break;
                
            case 5:
                uVal = *Put;
                uVal &= 0xc0;
                uVal |= uFill & 0x3f;
                *Put = uVal;
                break;
                
            case 4:
                uVal = *Put;
                uVal &= 0xe0;
                uVal |= uFill & 0x1f;
                *Put = uVal;
                break;

            case 3:
                uVal = *Put;
                uVal &= 0xf0;
                uVal |= uFill & 0x0f;
                *Put = uVal;
                break;
                
            case 2:
                uVal = *Put;
                uVal &= 0xf8;
                uVal |= uFill & 0x07;
                *Put = uVal;
                break;
                
            case 1:
                uVal = *Put;
                uVal &= 0xfc;
                uVal |= uFill & 0x03;
                *Put = uVal;
                break;
                
            case 0:
                uVal = *Put;
                uVal &= 0xfe;
                uVal |= uFill & 0x01;
                *Put = uVal;
                break;
            }
        }

       #else        // ROTATE CW

        Put = mpScanPointers[wXPos] + (wYStart >> 3);

        switch(wYStart & 7)
        {
        case 1:
            if (iLen >= 7)
            {
                uVal = *Put;
                uVal &= 0x80;
                uVal |= uFill & 0x7f;
                *Put++ = uVal;
                iLen -= 7;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXPos, wYStart + iLen, Color);
                }
                wXPos++;
                continue;
            }
            break;

        case 2:
            if (iLen >= 6)
            {
                uVal = *Put;
                uVal &= 0xc0;
                uVal |= uFill & 0x3f;
                *Put++ = uVal;
                iLen -= 6;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXPos, wYStart + iLen, Color);
                }
                wXPos++;
                continue;
            }
            break;

        case 3:
            if (iLen >= 5)
            {
                uVal = *Put;
                uVal &= 0xe0;
                uVal |= uFill & 0x1f;
                *Put++ = uVal;
                iLen -= 5;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXPos, wYStart + iLen, Color);
                }
                wXPos++;
                continue;
            }
            break;

        case 4:
            if (iLen >= 4)
            {
                uVal = *Put;
                uVal &= 0xf0;
                uVal |= uFill & 0x0f;
                *Put++ = uVal;
                iLen -= 4;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXPos, wYStart + iLen, Color);
                }
                wXPos++;
                continue;
            }
            break;

        case 5:
            if (iLen >= 3)
            {
                uVal = *Put;
                uVal &= 0xf8;
                uVal |= uFill & 0x07;
                *Put++ = uVal;
                iLen -= 3;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXPos, wYStart + iLen, Color);
                }
                wXPos++;
                continue;
            }
            break;

        case 6:
            if (iLen >= 2)
            {
                uVal = *Put;
                uVal &= 0xfc;
                uVal |= uFill & 0x03;
                *Put++ = uVal;
                iLen -= 2;
            }
            else
            {
                PlotPointView(wXPos, wYStart, Color);
                wXPos++;
                continue;
            }
            break;

        case 7:
            PlotPointView(wXPos, wYStart, Color);
            Put++;
            iLen--;
            break;

        default:
            break;
        }

        // most compilers seem to do a good job of optimizing 
        // memset to do 32-bit data writes. If your compiler doesn't
        // make the most of your CPU, you might want to re-write this
        // in assembly.

        if (iLen >= 8)
        {
            memset(Put, uFill, iLen >> 3);
            Put += iLen >> 3;
        }
        if (iLen)
        {
            switch(wYEnd & 7)
            {
            case 6:
                uVal = *Put;
                uVal &= 0x01;
                uVal |= uFill & 0xfe;
                *Put = uVal;
                break;
                
            case 5:
                uVal = *Put;
                uVal &= 0x03;
                uVal |= uFill & 0xfc;
                *Put = uVal;
                break;
                
            case 4:
                uVal = *Put;
                uVal &= 0x07;
                uVal |= uFill & 0xf8;
                *Put = uVal;
                break;

            case 3:
                uVal = *Put;
                uVal &= 0x0f;
                uVal |= uFill & 0xf0;
                *Put = uVal;
                break;
                
            case 2:
                uVal = *Put;
                uVal &= 0x1f;
                uVal |= uFill & 0xe0;
                *Put = uVal;
                break;
                
            case 1:
                uVal = *Put;
                uVal &= 0x3f;
                uVal |= uFill & 0xc0;
                *Put = uVal;
                break;
                
            case 0:
                uVal = *Put;
                uVal &= 0x7f;
                uVal |= uFill & 0x80;
                *Put = uVal;
                break;
            }
        }
       #endif

        wXPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ProMono1353Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
   #ifdef ROTATE_CCW
    UCHAR uVal = 0x01 << (wYPos & 7);
   #else
    UCHAR uVal = 0x80 >> (wYPos & 7);
   #endif

    while (wXStart <= wXEnd)
    {
       #ifdef ROTATE_CCW
        UCHAR *Put = mpScanPointers[wXStart] - (wYPos >> 3);
       #else
        UCHAR *Put = mpScanPointers[wXStart] + (wYPos >> 3);
       #endif
        *Put ^= uVal;
        wXStart += 2;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ProMono1353Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    wYStart += 7;
    wYStart &= 0xfffc;
    #ifdef ROTATE_CCW
    UCHAR *Put = mpScanPointers[wXPos] - (wYStart >> 3);
    #else
    UCHAR *Put = mpScanPointers[wXPos] + (wYStart >> 3);
    #endif
    SIGNED iLen = wYEnd - wYStart;

    if (iLen <= 0)
    {
        return;
    }
    while(wYStart < wYEnd - 7)
    {
        #ifdef ROTATE_CCW
        *Put-- ^= 0x55;
        #else
        *Put++ ^= 0x55;
        #endif
        wYStart += 8;
    }
}

/*--------------------------------------------------------------------------*/
void ProMono1353Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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

    while (CaptureRect.wTop & 7)
    {
        CaptureRect.wTop--;
    }

    while ((CaptureRect.wBottom & 7) != 7)
    {
        CaptureRect.wBottom++;
    }

    Info->SetPos(CaptureRect);
    LONG Size = (pMap->wWidth * pMap->wHeight) >> 3;
    Info->Realloc(Size);

   #ifdef ROTATE_CCW
    UCHAR *GetStart = mpScanPointers[CaptureRect.wLeft] - (CaptureRect.wBottom >> 3);
   #else
    UCHAR *GetStart = mpScanPointers[CaptureRect.wLeft] + (CaptureRect.wTop >> 3);
   #endif

    // make room for the memory bitmap:

    pMap->uFlags = 0;       // raw format
    pMap->uBitsPix = 1;     // 1 bits per pixel

    // fill in the image with our captured info:

    UCHAR *Put = pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wWidth; wLine++)
    {
        memcpy(Put, GetStart, pMap->wHeight >> 3);
        #ifdef ROTATE_CCW
        GetStart += miPitch;
        #else
        GetStart -= miPitch;
        #endif
        Put += pMap->wHeight >> 3;
    }
    Info->SetValid(TRUE);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ProMono1353Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uVal1, uPix;
    UCHAR *Get, *Put;

    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
    }
    else
    {
        if (Getmap->uBitsPix == 1)
        {
            #ifdef ROTATE_CCW
            if (((Where.y + Getmap->wHeight - 1)  & 7) != 7)
            #else
            if (Where.y & 7)
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
            Get = Getmap->pStart;
            Get += (View.wLeft - Where.x) * Getmap->wHeight;

            #ifdef ROTATE_CCW
            Get += (Where.y + Getmap->wHeight - 1 - View.wBottom);
            #else
            Get += View.wTop - Where.y;
            #endif

            for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
            {
               #ifdef ROTATE_CCW
                Put = mpScanPointers[wCol] - (View.wBottom >> 3);
                uPix = View.wBottom & 7;
                uVal = *Put;

                for (SIGNED wRow = View.wBottom; wRow >= View.wTop; wRow--)
                {
                    uVal1 = *Get++;

                    if (uVal1 != 0xff)
                    {
                        uVal &= ~(1 << uPix);
                        uVal |= uVal1 << uPix;
                    }
                    if (!uPix)
                    {
                        *Put++ = uVal;
                        uVal = *Put;
                        uPix = 7;
                    }
                    else
                    {
                        uPix--;
                    }
                }
                if (uPix != 7)
                {
                    *Put = uVal;
                }

               #else        // rotate CW

                Put = mpScanPointers[wCol] + (View.wTop >> 3);
                uPix = 7 - (View.wTop & 7);
                uVal = *Put;

                for (SIGNED wRow = View.wTop; wRow <= View.wBottom; wRow++)
                {
                    uVal1 = *Get++;

                    if (uVal1 != 0xff)
                    {
                        uVal &= ~(0x01 << uPix);
                        uVal |= uVal1 << uPix;
                    }
                    if (!uPix)
                    {
                        *Put++ = uVal;
                        uVal = *Put;
                        uPix = 7;
                    }
                    else
                    {
                        uPix--;
                    }
                }
                if (uPix != 7)
                {
                    *Put = uVal;
                }

               #endif

                Get += Getmap->wHeight - iHeight;
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
// here for an aligned 1 bpp bitmap, no shifting required.
/*--------------------------------------------------------------------------*/
void ProMono1353Screen::DrawFastBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    // always padded to nearest full byte per scan line:

    UCHAR uVal;
    WORD wBytesPerLine = (Getmap->wHeight + 7) >> 3;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wLeft - Where.x) * wBytesPerLine;

   #ifdef ROTATE_CCW
    GetStart += (Where.y + Getmap->wHeight - 1 - View.wBottom) >> 3;
   #else
    GetStart += (View.wTop - Where.y) >> 3;
   #endif

    for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
    {
        UCHAR *Get = GetStart;
        SIGNED iCount = View.Height();

       #ifdef ROTATE_CCW
        UCHAR *Put = mpScanPointers[wCol] - (View.wBottom >> 3);

        switch(View.wBottom & 7)
        {
        case 0:
            uVal = *Put;
            uVal &= 0xfe;
            uVal |= *Get++ & 0x01;
            *Put++ = uVal;
            iCount--;
            break;

        case 1:
            uVal = *Put;
            uVal &= 0xfc;
            uVal |= *Get++ & 0x03;
            *Put++ = uVal;
            iCount -= 2;
            break;

        case 2:
            uVal = *Put;
            uVal &= 0xf8;
            uVal |= *Get++ & 0x07;
            *Put++ = uVal;
            iCount -= 3;
            break;

        case 3:
            uVal = *Put;
            uVal &= 0xf0;
            uVal |= *Get++ & 0x0f;
            *Put++ = uVal;
            iCount -= 4;
            break;

        case 4:
            uVal = *Put;
            uVal &= 0xe0;
            uVal |= *Get++ & 0x1f;
            *Put++ = uVal;
            iCount -= 5;
            break;

        case 5:
            uVal = *Put;
            uVal &= 0xc0;
            uVal |= *Get++ & 0x3f;
            *Put++ = uVal;
            iCount -= 6;
            break;

        case 6:
            uVal = *Put;
            uVal &= 0x80;
            uVal |= *Get++ & 0x7f;
            *Put++ = uVal;
            iCount -= 7;
            break;

        case 7:
            break;
        }

        if (iCount >= 8)
        {
            SIGNED iTemp = iCount >> 3;       
            // copy 8 pixels at a time:
            memcpy(Put, Get, iTemp);
            Put += iTemp;
            Get += iTemp;
            iCount -= iTemp << 3;
        }

        if (iCount > 0)
        {
            switch (View.wTop & 7)
            {
            case 7:
                uVal = *Put;
                uVal &= 0x7f;
                uVal |= *Get & 0x80;
                *Put = uVal;
                break;

            case 6:
                uVal = *Put;
                uVal &= 0x3f;
                uVal |= *Get & 0xc0;
                *Put = uVal;
                break;

            case 5:
                uVal = *Put;
                uVal &= 0x1f;
                uVal |= *Get & 0xe0;
                *Put = uVal;
                break;

            case 4:
                uVal = *Put;
                uVal &= 0x0f;
                uVal |= *Get & 0xf0;
                *Put = uVal;
                break;

            case 3:
                uVal = *Put;
                uVal &= 0x07;
                uVal |= *Get & 0xf8;
                *Put = uVal;
                break;

            case 2:
                uVal = *Put;
                uVal &= 0x03;
                uVal |= *Get & 0xfc;
                *Put = uVal;
                break;

            case 1:
                uVal = *Put;
                uVal &= 0x01;
                uVal |= *Get & 0xfe;
                *Put = uVal;
                break;

            case 0:
                // should never get here!
                break;
            }
        }
       #else

        UCHAR *Put = mpScanPointers[wCol] + (View.wTop >> 3);

        switch(View.wTop & 7)
        {
        case 0:
            break;

        case 1:
            uVal = *Put;
            uVal &= 0x80;
            uVal |= *Get++ & 0x7f;
            *Put++ = uVal;
            iCount -= 7;
            break;

        case 2:
            uVal = *Put;
            uVal &= 0xc0;
            uVal |= *Get++ & 0x3f;
            *Put++ = uVal;
            iCount -= 6;
            break;

        case 3:
            uVal = *Put;
            uVal &= 0xe0;
            uVal |= *Get++ & 0x1f;
            *Put++ = uVal;
            iCount -= 5;
            break;

        case 4:
            uVal = *Put;
            uVal &= 0xf0;
            uVal |= *Get++ & 0x0f;
            *Put++ = uVal;
            iCount -= 4;
            break;

        case 5:
            uVal = *Put;
            uVal &= 0xf8;
            uVal |= *Get++ & 0x07;
            *Put++ = uVal;
            iCount -= 3;
            break;

        case 6:
            uVal = *Put;
            uVal &= 0xfc;
            uVal |= *Get++ & 0x03;
            *Put++ = uVal;
            iCount -= 2;
            break;

        case 7:
            uVal = *Put;
            uVal &= 0xfe;
            uVal |= *Get++ & 0x01;
            *Put++ = uVal;
            iCount--;
            break;
        }

        if (iCount >= 8)
        {
            SIGNED iTemp = iCount >> 3;       
            // copy 8 pixels at a time:
            memcpy(Put, Get, iTemp);
            Put += iTemp;
            Get += iTemp;
            iCount -= iTemp << 3;
        }

        if (iCount > 0)
        {
            switch (View.wBottom & 7)
            {
            case 7:
                break;

            case 6:
                uVal = *Put;
                uVal &= 0x01;
                uVal |= *Get & 0xfe;
                *Put = uVal;
                break;

            case 5:
                uVal = *Put;
                uVal &= 0x03;
                uVal |= *Get & 0xfc;
                *Put = uVal;
                break;

            case 4:
                uVal = *Put;
                uVal &= 0x07;
                uVal |= *Get & 0xf8;
                *Put = uVal;
                break;

            case 3:
                uVal = *Put;
                uVal &= 0x0f;
                uVal |= *Get & 0xf0;
                *Put = uVal;
                break;

            case 2:
                uVal = *Put;
                uVal &= 0x1f;
                uVal |= *Get & 0xe0;
                *Put = uVal;
                break;

            case 1:
                uVal = *Put;
                uVal &= 0x3f;
                uVal |= *Get & 0xc0;
                *Put = uVal;
                break;

            case 0:
                uVal = *Put;
                uVal &= 0x7f;
                uVal |= *Get & 0x80;
                *Put = uVal;
                break;
            }
        }
       #endif

        GetStart += wBytesPerLine;
    }
}

/*--------------------------------------------------------------------------*/
// here for a misaligned monochrome bitmap, bit shifting required.
/*--------------------------------------------------------------------------*/
void ProMono1353Screen::DrawUnalignedBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uiPix, uoPix, uVal1;
    SIGNED iCount;
    UCHAR *Get;

    // always padded to nearest full byte per scan line:

    WORD wBytesPerLine = (Getmap->wHeight + 7) >> 3;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wLeft - Where.x) * wBytesPerLine;

   #ifdef ROTATE_CCW
    GetStart += (Where.y + Getmap->wHeight - 1 - View.wBottom) >> 3;
   #else
    GetStart += (View.wTop - Where.y) >> 3;
   #endif

    for (SIGNED wCol = View.wLeft; wCol <= View.wRight; wCol++)
    {
        Get = GetStart;
        uVal1 = *Get++;
        iCount = View.Height();

       #ifdef ROTATE_CCW
        UCHAR *Put = mpScanPointers[wCol] - (View.wBottom >> 3);

        uiPix = ((Where.y + Getmap->wHeight - 1) - View.wBottom) & 7;
        uVal1 <<= uiPix;        // shift input pixels out the msb

        uoPix = View.wBottom & 7;
        uVal = *Put;
        uVal >>= uoPix + 1;

        while (iCount-- > 0)
        {
            uVal <<= 1;
            uVal |= uVal1 >> 7;

            if (!uoPix)
            {
                uoPix = 8;
                *Put++ = uVal;
                uVal = 0;
            }
            uoPix--;
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

        if (uoPix != 7)
        {
            uVal1 = *Put;
            uVal1 &= 0xff >> (7 - uoPix);
            uVal <<= uoPix + 1;
            uVal1 |= uVal;
            *Put = uVal1;
        }

       #else    // ROTATE_CW

        UCHAR *Put = mpScanPointers[wCol] + (View.wTop >> 3);

        uiPix = (View.wTop - Where.y) & 7;
        uVal1 <<= uiPix;        // shift input pixels out the msb

        uoPix = View.wTop & 7;
        uVal = *Put;
        uVal >>= 8 - uoPix;

        while (iCount-- > 0)
        {
            uVal <<= 1;
            uVal |= uVal1 >> 7;

            if (uoPix == 7)
            {
                uoPix = 0;
                *Put++ = uVal;
                uVal = 0;
            }
            else
            {
                uoPix++;
            }
            if (uiPix == 7)
            {
                uiPix = 0;
                uVal1 = *Get++;
            }
            else
            {
                uiPix++;
                uVal1 <<= 1;
            }
        }

        if (uoPix)
        {
            uVal1 = *Put;
            uVal1 &= 0xff >> uoPix;
            uVal <<= 8 - uoPix;
            uVal1 |= uVal;
            *Put = uVal1;
        }
       #endif
        GetStart += wBytesPerLine;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ProMono1353Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
void ProMono1353Screen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void ProMono1353Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
void ProMono1353Screen::HidePointer(void)
{
   #ifdef PEG_MOUSE_SUPPORT
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
void ProMono1353Screen::SetPointer(PegPoint Where)
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
   #endif
}

/*--------------------------------------------------------------------------*/
void ProMono1353Screen::SetPointerType(UCHAR bType)
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
void ProMono1353Screen::ResetPalette(void)
{
    SetPalette(0, 2, DefPalette);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *ProMono1353Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 2;
    return muPalette;
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ProMono1353Screen::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *pGet)
{
    // in monochrome mode there is not palette to set....

    ReadVidReg(2);
    UCHAR *pPut = muPalette;

    for (WORD loop = iFirst; loop < iNum; loop++)
    {
        *pPut++ = *pGet++;
        *pPut++ = *pGet++;
        *pPut++ = *pGet++;
	}
}

/*--------------------------------------------------------------------------*/
// WriteVidReg-- Default to x86 development platform. This function may
// need to change based on your target hardware.
/*--------------------------------------------------------------------------*/
void ProMono1353Screen::WriteVidReg(UCHAR uReg, UCHAR uVal)
{
#if 0               // Use this version for SDU1353 eval card in PC-ISA slot

    POUTB(VID_IO_BASE, uReg);
    POUTB(VID_IO_BASE + 1, uVal);

#else               // Use this version for direct mapped (like PPC, RISC)
                    
    UCHAR *pReg = (UCHAR *) VID_IO_BASE;
    *pReg = uReg;
    *(pReg + 1) = uVal;

#endif
}

UCHAR ProMono1353Screen::ReadVidReg(UCHAR uReg)
{
#if 0               // Use this version for SDU1353 eval card in PC-ISA slot

    return(PINB(VID_IO_BASE + uReg));

#else               // Use this version for direct mapped (like PPC, RISC)

    UCHAR *pReg = (UCHAR *) VID_IO_BASE;
    return *(pReg + uReg);

#endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ProMono1353Screen::ConfigureController(void)
{
    WriteVidReg(0x00, 0);     // normal operation    

    WriteVidReg(0x01, 0x25);  // disp off, single panel, !LCDE, 16-bit mem,
                              // 8 data bits to LCD         

    WriteVidReg(0x02, (PEG_VIRTUAL_YSIZE / 16) - 1);  // 320 / 16 - 1 = 19 decimal


    WriteVidReg(0x03, 0x04);  // !PS, !LUT bypass, monochrome (B/W) mode

    WriteVidReg(0x04, PEG_VIRTUAL_XSIZE - 1);  // 240-1 lines

    WriteVidReg(0x05, 0);     // wf, hi order line count
    WriteVidReg(0x06, 0);     // screen1 start address LSB
    WriteVidReg(0x07, 0);     // screen1 start address MSB
    WriteVidReg(0x08, 0);     // screen2 start address LSB
    WriteVidReg(0x09, 0x00);  // screen2 start address MSB
    WriteVidReg(0x0a, PEG_VIRTUAL_XSIZE - 1);  // screen 1 line count, same as screen0
    WriteVidReg(0x0b, 0);
    WriteVidReg(0x0c, 0x00);     // horizontal non-display period
    WriteVidReg(0x0d, 0);     // pitch adjustment register

    // read from BASE + 2
    ReadVidReg(2);

    HorizontalLine(0, mwHRes - 1, 0, BLACK, mwVRes);    // clear the screen

    WriteVidReg(0x01, 0xa5);                            // turn display on

    // read from BASE + 2

    ReadVidReg(2);
}



