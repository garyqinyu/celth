/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ppc823_8.cpp - Linear 8 bit-per-pixel screen driver. This driver is bases
//   on the l8scrn.cpp screen driver template. This driver was created for
//   for the Embedded Planet IceBox developement platform. This platform uses
//   an active (TFT) color 640x480 LCD display planel. 
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
// For the MPC823, video memory is just a portion of DRAM if you are using
// the onboard (built-in) video controller. In this case, the easiest thing
// is to un-comment the line below to allocate the video frame buffer. You
// may need to put this in assembly to insure proper (16-byte) alignment.
/*--------------------------------------------------------------------------*/

#define STATIC_FRAME_BUFFER

#ifdef STATIC_FRAME_BUFFER
#ifdef DOUBLE_BUFFER
DWORD gbVMemory[((PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE) / 2) + 4];
#else
DWORD gbVMemory[((PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE) / 4) + 4];
#endif
#endif


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new MPC823Screen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
MPC823Screen::MPC823Screen(PegRect &Rect) : PegScreen(Rect)
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

    ConfigureController();        // set up controller registers

    SetPalette(0, 232, DefPalette256);
}

/*--------------------------------------------------------------------------*/
// *** This function must be filled in by the developer ***
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *MPC823Screen::GetVideoAddress(void)
{
    #ifdef STATIC_FRAME_BUFFER

    // just return the address of the static array:

    UCHAR PEGFAR *pMem = (UCHAR PEGFAR *) gbVMemory;

    #else

    // for an example, just allocate a buffer in dynamic memory:

    DWORD dSize = mwHRes * mwVRes;
    dSize += 4;         

   #ifdef DOUBLE_BUFFER
    dSize *= 2;         // need two frame buffers if double buffering
   #endif

    UCHAR *pMem = new UCHAR[dSize];

    #endif

    // ensure 16-byte alignment:
    pMem += 15;
    DWORD dAddr = (DWORD) pMem;
    dAddr &= 0xfffffff0L;
    pMem = (UCHAR *) dAddr;
    return pMem; 
}

/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
MPC823Screen::~MPC823Screen()
{
    #ifndef STATIC_FRAME_BUFFER
    delete [] mpScanPointers[0];
    #endif

    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void MPC823Screen::BeginDraw(PegThing *)
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
void MPC823Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        mpScanPointers = new UCHAR PEGFAR *[pMap->wHeight];
        UCHAR PEGFAR *CurrentPtr = pMap->pStart;
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
void MPC823Screen::EndDraw()
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
void MPC823Screen::EndDraw(PegBitmap *pMap)
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
void MPC823Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void MPC823Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    SIGNED iLen = wXEnd - wXStart + 1;

    if (!iLen)
    {
        return;
    }
    while(wWidth-- > 0)
    {
        UCHAR *Put = mpScanPointers[wYPos] + wXStart;
        memset(Put, Color, iLen);
        wYPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MPC823Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
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
void MPC823Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    UCHAR *Put = mpScanPointers[wYPos] + wXStart;

    while (wXStart <= wXEnd)
    {
        *Put ^= 0x0f;
        Put += 2;
        wXStart += 2;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MPC823Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
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
void MPC823Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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
    pMap->uBitsPix = 8;      // 8 bits per pixel

    // fill in the image with our captured info:

    UCHAR *Put = pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        UCHAR *Get = GetStart;
        for (WORD wLoop = 0; wLoop < pMap->wWidth; wLoop++)
        {
            *Put++ = *Get++;
        }
        GetStart += mwHRes;
    }
    Info->SetValid(TRUE);
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MPC823Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
    }
    else
    {
        UCHAR *Get = Getmap->pStart;
        Get += (View.wTop - Where.y) * Getmap->wWidth;
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
	            Get += Getmap->wWidth - View.Width();
	        }
        }
        else
        {
	        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
	        {
	            UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
                memcpy(Put, Get, View.Width());
	            //Get += Getmap->wWidth - View.Width();
	            Get += Getmap->wWidth;
	        }
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void MPC823Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
	                        uVal != 0xff)
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
void MPC823Screen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void MPC823Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
void MPC823Screen::HidePointer(void)
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
void MPC823Screen::SetPointer(PegPoint Where)
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
void MPC823Screen::SetPointerType(UCHAR bType)
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
void MPC823Screen::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *MPC823Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 232;
    return muPalette;
}



/*--------------------------------------------------------------------------*/
// ReadIMMR() returns the current value of the Internal Memory Map Register
// 
/*--------------------------------------------------------------------------*/
#ifdef PEGSMX
extern "C" {
DWORD getIMMR(void);
void  setICACHE(DWORD);
}
#endif

DWORD ReadIMMR()
{
   #ifdef PEGSMX

	return(getIMMR());

   #elif defined(PEGINTEGRITY)

    // For Integrity, the BSP defines IMMR_BASE. If that doesn't seem right,
    // you may use the asm call below, but your application will have to run
    // in kernel space, not as a virtual space application.
    return((DWORD) 0xFF000000L);

   #else

    #if 1
    // this version really reads the immr register. This only works with
    // certain compilers and operating systems.

    asm(" mfspr 3,638 ");   /* return value in gpr 3 */
    #else
   
    // this version just returns a hard-coded value. Must match the
    // actual contents of IMMR!!!

    return ((DWORD) 0xfa200000L);
    #endif
   #endif

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MPC823Screen::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
    DWORD dImmr = ReadIMMR() & 0xffff0000;

    WORD *pPut = (WORD *) (dImmr + COLOR_RAM);
    pPut += iFirst;
    WORD wVal;
    WORD wTemp;
    
    while(iCount > 0)
    {
        wTemp = *pGet++;      // get the 4 bit red value
        wTemp >>= 4;
        wTemp <<= 8;
        wVal = wTemp;

        wTemp = *pGet++;
        wTemp >>= 4;
        wTemp <<= 4;
        wVal |= wTemp;       // OR in the 4 green bits

        wTemp = *pGet++;
        wTemp >>= 4;
        wVal |= wTemp;      // OR in the 4 blue bits

        *pPut++ = wVal;
        iCount--;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void EnableICache( DWORD val )
{
	#ifdef PEGSMX
	setICACHE(val);
	#else
    asm(" mtspr 560,r3 ");      // turn on the instruction cache!!
    #endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MPC823Screen::ConfigureController(void)
{
    DWORD *pReg;
    DWORD dImmr = ReadIMMR() & 0xffff0000;

    EnableICache(0x02000000);   /* turn on the instruction cache */

    // The LCD Data, LOE, and Synch lines are on PORT D. Configure these
    // pins for output, and away we go!!

    WORD *phReg = (WORD *) (dImmr + 0x976); // port D data 
    *phReg = 0x1e02;       

    pReg = (DWORD *) (dImmr + 0x970);   // port D data direction
    *pReg = 0x1fff1fff;

   #ifdef PANEL_TYPE_SHARP_LQ64D
    // Setup LCD clock dividers

    WORD* pClockReg = (WORD*) (dImmr + 0x282);
    WORD wClockData = *pClockReg;
    wClockData = wClockData | 0x04;     // DFLCD / 2, DFALCD / 1 = 25MHz
                                        // on the IPEngine 50MHz board
    *pClockReg = wClockData;
   #endif    

    pReg = (DWORD *) (dImmr + LCD_HORZ_CTRL);
    
    *pReg = (1 << 24) |                 // BO bit == 1
            (4 << 21) |                 // AT == 4
   #ifndef PANEL_TYPE_TFT
            (((PEG_VIRTUAL_XSIZE * 3) >> 3) << 10) |    // Horz Pixel count
   #else
            (PEG_VIRTUAL_XSIZE << 10)|  // Horizontal pixel count
   #endif
            (LCD_HORIZONTAL_WAIT);      // WBL value

    pReg = (DWORD *) (dImmr + LCD_VERT_CONFIG);

    *pReg = (VSYNCH_PULSE_WIDTH << 28)|
            (0x30 << 21)|                // ENAB toggle
            (PEG_VIRTUAL_YSIZE << 11) |
            (LCD_VERTICAL_WAIT);        // WBF value

    pReg = (DWORD *) (dImmr + LCD_BUFFER_A);

   #ifdef DOUBLE_BUFFER
    *pReg = ((DWORD) mpScanPointers[0]) + (mwVRes * mwHRes);
    pReg = (DWORD *) (dImmr + LCD_BUFFER_B);
    *pReg = ((DWORD) mpScanPointers[0]) + (mwVRes * mwHRes);
   #else
    *pReg = (DWORD) mpScanPointers[0];
    pReg = (DWORD *) (dImmr + LCD_BUFFER_B);
    *pReg = (DWORD) mpScanPointers[0];
   #endif

    pReg = (DWORD *) (dImmr + LCD_CONFIG);

   #ifdef PANEL_TYPE_SHARP_LM057QC
    *pReg = ((PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE * 8 / 128) << 17) | // BNUM
            (0x00 << 8)|                // HSYNCH and VSYNCH both active high
            (0x03 << 6)|                // 8 bpp
            (LCD_DATA_POLARITY << 7)|   // data polarity, active hi or low
            (PANEL_TYPE_TFT << 1)|
            5;                          // single panel, color, display on
   #elif defined(PANEL_TYPE_SHARP_LQ64D)
    *pReg = ((PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE * 8 / 128) << 17) | // BNUM
            (0x03 << 8)|                // HSYNCH and VSYNCH both active low
            (0x06 << 4)|                // 8 bpp
            (LCD_DATA_POLARITY << 7)|   // data polarity, active hi or low
            (PANEL_TYPE_TFT << 1)|
            0x05;                       // color, set display on
   #else
    *pReg = ((PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE * 8 / 128) << 17) | // BNUM
            (0x03 << 8)|                // HSYNCH and VSYNCH both active low
            (0x06 << 4)|                // 8 bpp
            (LCD_DATA_POLARITY << 7)|   // data polarity, active hi or low
            (PANEL_TYPE_TFT << 1)|
            5;                          // single panel, color, display on
   #endif
}


#ifdef DOUBLE_BUFFER 

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the windows screen:

void MPC823Screen::MemoryToScreen(void)
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

    SIGNED iTop = Copy.wTop;
    SIGNED iLeft = Copy.wLeft;
    SIGNED iWidth = Copy.Width();
    SIGNED iHeight = Copy.Height();

    UCHAR *pGet = mpScanPointers[iTop] + iLeft;
    UCHAR *pPut = pGet;
    pPut += (mwVRes * mwHRes);  // Put to second (visible) frame buffer

    for (SIGNED iLoop = 0; iLoop < iHeight; iLoop++)
    {
        memcpy(pPut, pGet, iWidth);
        pPut += mwHRes;
        pGet += mwHRes;
    }
}

#endif



