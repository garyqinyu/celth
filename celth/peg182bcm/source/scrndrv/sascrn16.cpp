/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// sascrn16.cpp - Linear 16 bit-per-pixel screen driver for Intel StrongARM
//   SA1110 development board.
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
// All available configuration flags are found in the L16Screen.hpp header file.
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
// For many embedded controllers, the video RAM is just any section of local
// memory. In that case, the easiest thing is to un-comment the line below
// to allocate the video frame buffer
/*--------------------------------------------------------------------------*/

UCHAR gbVMemory[(PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE * 2) + 512 + 16];


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new SAScreen16(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
#ifdef PEGWIN32
SAScreen16::SAScreen16(HWND hWnd, PegRect &Rect) : PegScreen(Rect)
#else
SAScreen16::SAScreen16(PegRect &Rect) : PegScreen(Rect)
#endif
{
    mdNumColors = 65535;

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();

    mpScanPointers = new COLORVAL PEGFAR *[Rect.Height()];
    COLORVAL *CurrentPtr = (COLORVAL *) GetVideoAddress();

    mpPaletteRam = (WORD *) CurrentPtr;
    SetPalette(0, 232, DefPalette256);

    // The first 32 bytes are dummy palette data:

    CurrentPtr += 16;

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
}

/*--------------------------------------------------------------------------*/
// *** This function must be filled in by the developer ***
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *SAScreen16::GetVideoAddress(void)
{
    // just return the address of the static array:
    UCHAR *pMem = (UCHAR PEGFAR *) gbVMemory;

    // ensure 16-byte alignment:

    pMem += 15;

    DWORD dAddr = (DWORD) pMem;
    dAddr &= 0xfffffff0L;
    dAddr |= 0xc0000000L;       // not cached
    pMem = (UCHAR *) dAddr;
    return pMem; 
}

/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
SAScreen16::~SAScreen16()
{
    delete mpScanPointers;
}


/*--------------------------------------------------------------------------*/
// CreateBitmap- Override default version to create 16-bpp bitmap.
/*--------------------------------------------------------------------------*/
PegBitmap *SAScreen16::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
{
   PegBitmap *pMap = NULL;

    if (wWidth && wHeight)
    {
        pMap = new PegBitmap;
        pMap->wWidth = wWidth;
        pMap->wHeight = wHeight;
        pMap->uFlags = BMF_HAS_TRANS;
        pMap->uBitsPix = 16;

        DWORD dSize = (DWORD) wWidth * (DWORD) wHeight;

       #ifdef USE_VID_MEM_MANAGER
        pMap->pStart = AllocBitmap(dSize * sizeof(COLORVAL));

        if (!pMap->pStart)  // out of video memory?
        {
            pMap->pStart = (UCHAR PEGFAR *) new COLORVAL[dSize];    // try local memory
        }
        else
        {
            pMap->uFlags = BMF_HAS_TRANS|BMF_SPRITE;
        }
       #else
        pMap->pStart = (UCHAR PEGFAR *) new COLORVAL[dSize];
       #endif

        if (!pMap->pStart)
        {
            delete pMap;
            return NULL;
        }
		if(bHasTrans)
		{
			memset(pMap->pStart, TRANSPARENCY, dSize * sizeof(COLORVAL));
			pMap->uFlags |= BMF_HAS_TRANS;
		}
		else
		{
			// fill the whole thing with BLACK:
			memset(pMap->pStart, BLACK, dSize * sizeof(COLORVAL));
		}
    }
    return pMap;
}

/*--------------------------------------------------------------------------*/
void SAScreen16::BeginDraw(PegThing *)
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
void SAScreen16::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        mpScanPointers = new COLORVAL PEGFAR *[pMap->wHeight];
        COLORVAL PEGFAR *CurrentPtr = (COLORVAL PEGFAR *) pMap->pStart;
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
void SAScreen16::EndDraw()
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
void SAScreen16::EndDraw(PegBitmap *pMap)
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
void SAScreen16::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void SAScreen16::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    SIGNED iLen = wXEnd - wXStart + 1;

    if (iLen <= 0)
    {
        return;
    }
    while(wWidth-- > 0)
    {
        COLORVAL *Put = mpScanPointers[wYPos] + wXStart;
        SIGNED iLen1 = iLen;

        while(iLen1--)
        {
            *Put++ = Color;
        }
        wYPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SAScreen16::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    while(wYStart <= wYEnd)
    {
        SIGNED lWidth = wWidth;
        COLORVAL *Put = mpScanPointers[wYStart] + wXPos;

        while (lWidth-- > 0)
        {
            *Put++ = Color;
        }
        wYStart++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SAScreen16::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    COLORVAL *Put = mpScanPointers[wYPos] + wXStart;

    while (wXStart <= wXEnd)
    {
        *Put ^= 0xffff;
        Put += 2;
        wXStart += 2;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SAScreen16::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    COLORVAL uVal;

    while (wYStart <= wYEnd)
    {
        COLORVAL *Put = mpScanPointers[wYStart] + wXPos;
        uVal = *Put;
        uVal ^= 0xffff;
        *Put = uVal;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void SAScreen16::Capture(PegCapture *Info, PegRect &CaptureRect)
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
    LONG Size = pMap->wWidth * pMap->wHeight * 2;
    Info->Realloc(Size);

    COLORVAL *GetStart = mpScanPointers[CaptureRect.wTop] + CaptureRect.wLeft;

    // make room for the memory bitmap:

    pMap->uFlags = 0;         // raw format
    pMap->uBitsPix = 16;      // 16 bits per pixel

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
/*--------------------------------------------------------------------------*/
void SAScreen16::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
    }
    else
    {
        if (Getmap->uBitsPix == 8)
        {
            Draw8BitBitmap(Where, View, Getmap);
        }
        else
        {
            COLORVAL *Get = (COLORVAL *) Getmap->pStart;
            Get += (View.wTop - Where.y) * Getmap->wWidth;
            Get += View.wLeft - Where.x;

            if (HAS_TRANS(Getmap))
            {
                for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
                {
                    COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
                    for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
                    {
                        COLORVAL uVal = *Get++;

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
                    COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
                    for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
                    {
                        *Put++ = *Get++;
                    }
                    Get += Getmap->wWidth - View.Width();
                }
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SAScreen16::Draw8BitBitmap(const PegPoint Where, const PegRect View,
    const PegBitmap *Getmap)
{
    
    UCHAR *Get = Getmap->pStart;
    Get += (View.wTop - Where.y) * Getmap->wWidth;
    Get += View.wLeft - Where.x;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
        for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
        {
            UCHAR uVal = *Get++;
            if (uVal != 0xff)
            {
                *Put = mcHiPalette[uVal];
            }
            Put++;
        }
        Get += Getmap->wWidth - View.Width();
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SAScreen16::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
        COLORVAL *Put = mpScanPointers[wLine] + Where.x;
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
                        *Put = mcHiPalette[uVal];
                    }
                    Put++;
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
                    Put += uCount;
                }
                else
                {
                    while(uCount--)
                    {
                        if (wLoop1 >= View.wLeft &&
                            wLoop1 <= View.wRight)
                        {
                            *Put++ = mcHiPalette[uVal];
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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SAScreen16::RectMoveView(PegThing *Caller, const PegRect &View,
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
void SAScreen16::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
                        *Put = Color.uForeground;
                    }
                    else
                    {
                        if (Color.uFlags & CF_FILL)
                        {
                            *Put = Color.uBackground;
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
void SAScreen16::HidePointer(void)
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
void SAScreen16::SetPointer(PegPoint Where)
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
void SAScreen16::SetPointerType(UCHAR bType)
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
void SAScreen16::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *SAScreen16::GetPalette(DWORD *pPutSize)
{
    *pPutSize = mdNumColors;
    return NULL;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SAScreen16::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
    // The default operation of the SetPalette function in this 16-bit driver
    // is to convert the standard 256 color palette to HiColor 16-bit values.
    // This allows 8-bit bitmaps to be displayed in native format by simply
    // looking up the correct 5-6-5 color value corresponding to each 8-bit
    // palette index. In other words, the 16-bit Hi-Color driver can easily
    // emulate 8-bit operation when required, while still having 65535 colors
    // available.

    COLORVAL *pPut = mcHiPalette;

    pPut += iFirst;

   #if 1
    // Use this version for 5-6-5 operation:

    while(iCount--)
    {
        COLORVAL cVal = *pGet++ >> 3;   // 5 bits RED
        cVal <<= 6;                     // make room for GREEN
        cVal |= *pGet++ >> 2;           // 6 bits GREEN
        cVal <<= 5;                     // make room for BLUE
        cVal |= *pGet++ >> 3;           // keep 5 bits BLUE
       *pPut++ = cVal;                  // save the Hi-Color equivalent
    }
   #else

    // Use this version for 5-5-5 operation

    while(iCount--)
    {
        COLORVAL cVal = *pGet++ >> 3;   // 5 bits RED
        cVal <<= 5;                     // make room for GREEN
        cVal |= *pGet++ >> 3;           // 5 bits GREEN
        cVal <<= 5;                     // make room for BLUE
        cVal |= *pGet++ >> 3;           // keep 5 bits BLUE
       *pPut++ = cVal;                  // save the Hi-Color equivalent
    }
   #endif   

    pPut = mpPaletteRam;

    for (WORD wLoop = 0; wLoop < 31; wLoop++)
    {
        *pPut++ = 0x2000;   // 16 bpp
    }

}

#define LCD_LCCR0 0xb0100000
#define LCD_LCSR  0xb0100004
#define LCD_DBAR1 0xb0100010
#define LCD_DCAR1 0xb0100014
#define LCD_DBAR2 0xb0100018
#define LCD_DCAR2 0xb010001c
#define LCD_LCCR1 0xb0100020
#define LCD_LCCR2 0xb0100024
#define LCD_LCCR3 0xb0100028

#define LCD_HSYNCH_WIDTH 0x04
#define LCD_EOL_WAIT     0x08  // End of line wait
#define LCD_BOL_WAIT     0x3c  // Beg of line wait

#define LCD_VSYNCH_WIDTH 1
#define LCD_EOF_WAIT     0  // End of Frame wait
#define LCD_BOF_WAIT     3  // Beg of Frame wait

#define LCD_PCD 0x11    // Pixel clock divider
#define LCD_ACB 0       // AC Bias Pin Frequency
#define LCD_VSP 0       // VSynch polarity
#define LCD_HSP 0       // HScych polarity
#define LCD_PCP 0       // Pixel clock polarity
#define LCD_OEP 0       // Output Enable polarity

#define LCD_PDD 0       // Palette DMA delay
#define LCD_DPD 0       // double pixel data
#define LCD_BLE 0       // Big/Little Endian, 1 = Big
#define LCD_PAS 1       // Passive/Active, 1 = Active
#define LCD_ERM 1       // Error mask
#define LCD_BAM 1       // Base Address update mask
#define LCD_LDM 1       // disable done mask
#define LCD_SDS 0       // 0= single, 1 = dual panel
#define LCD_CMS 0       // 0= color, 1 = monochrome

#define PLAT_BCR                (0x12000000)
#define BCR_DB1111              (0x00A074E2)
#define BCR_DB1110              (0x00A07410)
//#define BCR_LCD_ON  		0x800
#define BCR_LCD_ON  		0xa00
//#define BCR_LCD_ON  		0x800
//#define RED_LED_ON			(-0x2000)
#define RED_LED_ON 0

#define GPIOREGBASE             0x90040000
#define GPLR                    0x00
#define GPDR                    0x04
#define GPSR                    0x08
#define GPCR                    0x0C
#define GRER                    0x10
#define GFER                    0x14
#define GEDR                    0x18
#define GAFR                    0x1C

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SAScreen16::ConfigureController(void)
{
    *(DWORD *)(GPIOREGBASE + GPDR) |= 0x3fc;
    *(DWORD *)(GPIOREGBASE + GAFR) |= 0x3fc;

    LONG *plReg = (LONG *) PLAT_BCR;
    LONG bcr_value =  BCR_DB1110;    // set to BCR_DB1111 if 1111 board present
    *plReg = bcr_value;

    bcr_value |= 0x2000;	     // turn red LED off
    *plReg = bcr_value;

    
    bcr_value &= ~0x2000;	     // turn red LED on
    *plReg = bcr_value;
    
    bcr_value |= BCR_LCD_ON;
    *plReg = bcr_value;

    DWORD *pReg = (DWORD *) LCD_LCCR0;
    *pReg = (LCD_PDD << 12) |
            (LCD_DPD << 9) |
            (LCD_BLE << 8) |
            (LCD_PAS << 7) |
            (LCD_ERM << 5) |
            (LCD_BAM << 4) |
            (LCD_LDM << 3) |
            (LCD_SDS << 2) |
            (LCD_CMS << 1);    // disable the LCD

    pReg = (DWORD *) LCD_LCSR;      // status register
    *pReg = 0xffffffffL;                     // clear all error bits by writing 1's

    pReg  = (DWORD *) LCD_LCCR1;
    *pReg = (PEG_VIRTUAL_XSIZE - 16) | 
            (LCD_HSYNCH_WIDTH << 10) | 
            (LCD_EOL_WAIT << 16) |
            (LCD_BOL_WAIT << 24);

    pReg  = (DWORD *) LCD_LCCR2;
    *pReg = (PEG_VIRTUAL_YSIZE - 1) |
            (LCD_VSYNCH_WIDTH << 10) |
            (LCD_EOF_WAIT << 16) |
            (LCD_BOF_WAIT << 24);

    pReg  = (DWORD *) LCD_LCCR3;
    *pReg = (LCD_PCD) |
            (LCD_ACB << 8) |
            (LCD_VSP << 20) |
            (LCD_HSP << 21) |
            (LCD_PCP << 22) |
            (LCD_OEP << 23);

    pReg  = (DWORD *) LCD_DBAR1;
    *pReg = (DWORD) mpPaletteRam;

    pReg  = (DWORD *) LCD_LCCR0;
    *pReg = (LCD_PDD << 12) |
            (LCD_DPD << 9) |
            (LCD_BLE << 8) |
            (LCD_PAS << 7) |
            (LCD_ERM << 5) |
            (LCD_BAM << 4) |
            (LCD_LDM << 3) |
            (LCD_SDS << 2) |
            (LCD_CMS << 1) | 1;

    // clear the screen:

    HorizontalLine(0, mwHRes - 1, 0, BLACK, mwVRes);

    HorizontalLine(0, mwHRes - 1, 0,   WHITE, 40);
    HorizontalLine(0, mwHRes - 1, 40,  RED, 40);
    HorizontalLine(0, mwHRes - 1, 80,  GREEN, 40);
    HorizontalLine(0, mwHRes - 1, 120, BLUE, 40);
}
 
#ifdef DOUBLE_BUFFER

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the windows screen:

void SAScreen16::MemoryToScreen(void)
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



