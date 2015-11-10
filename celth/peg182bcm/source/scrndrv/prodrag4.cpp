/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// prodrag4.cpp - PegScreen driver for DragonBall processor running in 
//  4 bit-per-pixel gray scale mode.
//
//  This driver is for use when the physical display screen has been
//  rotated 90 degrees clockwise or CCW.
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
// There are conditional sections of code delineated by
//  #ifdef TESTBOARD_1. These sections of software are specific
//  to the customer's test board that was used to develope this driver.
//  you should check these sections of code and modify them as required
//  to work with your system.
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


/*--------------------------------------------------------------------------*/
// The frame buffer. Default is to simply place this buffer
// wherever the linker decides it should go.
/*--------------------------------------------------------------------------*/

UCHAR gbFrameBuffer[PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE / 2];

/*--------------------------------------------------------------------------*/
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


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/

PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new Dragon4Screen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
Dragon4Screen::Dragon4Screen(PegRect &Rect) : PegScreen(Rect)
{
    mdNumColors = 16;  

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();
    miPitch = (mwVRes + 1) >> 1;

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

    SetPalette(0, 16, GrayPalette);
}


/*--------------------------------------------------------------------------*/
// *** This function must be filled in by the developer ***
/*--------------------------------------------------------------------------*/
UCHAR *Dragon4Screen::GetVideoAddress(void)
{
    return gbFrameBuffer;
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
Dragon4Screen::~Dragon4Screen()
{
    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void Dragon4Screen::BeginDraw(PegThing *)
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
void Dragon4Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
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
void Dragon4Screen::EndDraw()
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
void Dragon4Screen::EndDraw(PegBitmap *pMap)
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
PegBitmap *Dragon4Screen::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
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
        // fill the whole thing with BLACK:
		if(bHasTrans)
		{
			memset(pMap->pStart, TRANSPARENCY, (DWORD) wWidth * (DWORD) wHeight);
			pMap->uFlags = BMF_HAS_TRANS;
		}
		else
		{
			memset(pMap->pStart, BLACK, (DWORD) wWidth * (DWORD) wHeight);
			pMap->uFlags = 0;
		}
        pMap->uBitsPix = 4;
    }
    return pMap;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Dragon4Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void Dragon4Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
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
void Dragon4Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
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
void Dragon4Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
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
void Dragon4Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
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
void Dragon4Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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
void Dragon4Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
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
void Dragon4Screen::DrawFastBitmap(const PegPoint Where, const PegBitmap *Getmap,
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
void Dragon4Screen::DrawUnalignedBitmap(const PegPoint Where, const PegBitmap *Getmap,
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
void Dragon4Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
void Dragon4Screen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void Dragon4Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
void Dragon4Screen::HidePointer(void)
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
void Dragon4Screen::SetPointer(PegPoint Where)
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
void Dragon4Screen::SetPointerType(UCHAR bType)
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
void Dragon4Screen::ResetPalette(void)
{
    SetPalette(0, 16, GrayPalette);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *Dragon4Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 16;
    return muPalette;
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Dragon4Screen::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *pGet)
{
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Dragon4Screen::ConfigureController(void)
{
    WORD  *pwReg;
    UCHAR *pbReg = (UCHAR *) LCD_CKCON;
    DWORD *pdReg = (DWORD *) LCD_REG_BASE;

    *pbReg = 0x00;                      // turn off display
    

    *pdReg = (DWORD) &gbFrameBuffer[0];     // base address of frame buffer

    pbReg = (UCHAR *) LCD_VPW;
    *pbReg = PEG_VIRTUAL_YSIZE / 4;         // virtual page width (words)

    pwReg = (WORD *) LCD_XMAX;
    *pwReg = PEG_VIRTUAL_YSIZE;             // Screen Width Register

    pwReg = (WORD *) LCD_YMAX;
    *pwReg = PEG_VIRTUAL_XSIZE - 1;         // Screen Height Register

    // disable the cursor:

    pwReg = (WORD *) LCD_CXP;
    *pwReg = 0;                             // Cursor X (disabled)

    pwReg = (WORD *) LCD_CYP;
    *pwReg = 0;                             // Cursor Y

    pwReg = (WORD *) LCD_CWCH;
    *pwReg = 0x0101;                        // Cursor width and height

    pbReg = (UCHAR *) LCD_PICF;
    *pbReg = 0x0a;                      // bus width and mode

    pbReg = (UCHAR *) LCD_POLCF;
    *pbReg = 0x00;

    pbReg = (UCHAR *) LCD_ACDRC;
    *pbReg = 0x00;

    pbReg = (UCHAR *) LCD_PXCD;
    *pbReg = 0x00;                      // pixel clock divisor

    pbReg = (UCHAR *) LCD_POSR;
    *pbReg = 0x00;                      // panning offset

    pbReg = (UCHAR *) LCD_RRA;
    *pbReg = 0x48;                      // refresh rate adjustment

    pbReg = (UCHAR *) LCD_FRCM;
    *pbReg = 0xb9;

    pbReg = (UCHAR *) LCD_GPMR;
    *pbReg = 0x84;


    #ifdef TESTBOARD_1

    /* setup LCD power bits and power down panel */
    *REG_PFDATA |= 0x80;
    *REG_PFDIR  |= 0x80;
    *REG_PFSEL  |= 0x80;
    *REG_PBDATA |= 0x08;
    *REG_PBDIR  |= 0x08;
    *REG_PBSEL  |= 0x08;

    /* setup LCD panel enable line and disable */
    *REG_PFSEL      |=  0x01;   // bit 0 = LCD enable
    *REG_PFDIR      |=  0x01;
    *REG_PFDATA     &= ~0x01;   // bit 7 active high
    
    /* set up LCD interface signals */
    *REG_PCSEL       =  0x00;

    #endif

    pbReg = (UCHAR *) LCD_CKCON;
    *pbReg = 0x06;    // set 16 bit bus, DMA wait states
    *pbReg = 0x86;    // Enable the LCD controller

	#ifdef TESTBOARD_1
    *REG_PFDATA   |= 0x01;    // activate LCD
	#endif

//    HorizontalLine(0, 239, 0, 0, 160);
//    HorizontalLine(0, 239, 160, 15, 150);
}

#ifdef DOUBLE_BUFFER

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the windows screen. This is only used
// for testing purposes, and can be deleted.

void Dragon4Screen::MemoryToScreen(void)
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
