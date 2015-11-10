/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// svgascr8.cpp - Generic 8 bpp (256-color) SVGA screen driver.  This driver
//   can be configured to work with screen resolutions 640x480, 800x600, and
//   1024x768, simply by passing in the correct rectangle to the constructor.
//
// Author: Jeremy M. Dalmer (from l8scrn)
//
// Copyright (c) 1997-2002 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// All available configuration flags are found in the svgascr8.hpp header file.
//
//      *****************  IMPORTANT  ********************
//
// Known Limitations:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#ifdef DPMI_API_DJGPP
# include <sys\farptr.h>
# include <sys\nearptr.h>
#endif

#include <stdlib.h>
#include <string.h>

extern UCHAR DefPalette256[];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    return new GenericSvgaScreen(Rect);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
GenericSvgaScreen::GenericSvgaScreen(PegRect &Rect) : PegScreen(Rect)
{
	mdNumColors = 256;

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();
    WORD wPitch = mwHRes;

    ConfigureController();        // set up controller registers

    SetPalette(0, 232, DefPalette256);

    mpScanPointers = new UCHAR *[Rect.Height()];
    UCHAR *CurrentPtr = GetVideoAddress();

    for (SIGNED iLoop = 0; iLoop < Rect.Height(); iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += wPitch;
    }

    mLastPointerPos.x = Rect.Width() / 2;
    mLastPointerPos.y = Rect.Height() / 2;
    mbPointerHidden = FALSE;
    mwDrawNesting = 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *GenericSvgaScreen::GetVideoAddress(void)
{
   #if defined(DOUBLE_BUFFER) && !defined(LOCAL_BUFFERING)
	UCHAR* pMem = GetFrameBufferAddress( mwHRes * mwVRes * 2 );
   #else
	UCHAR* pMem = GetFrameBufferAddress( mwHRes * mwVRes );
   #endif

   #ifdef DPMI_API_DJGPP
	int ret = __djgpp_nearptr_enable();
	if (ret != 0)
	{
        return NULL;
	}
	else
    {
        return NULL;
	}

	__djgpp_nearptr_disable();

   #endif

   #ifdef DOUBLE_BUFFER
   # ifdef LOCAL_BUFFERING
	mpTargetMem = pMem;
	pMem = new UCHAR[mwHRes * mwVRes];
   # else
	pMem += mwHRes * mwVRes;
   # endif
   #endif

	return pMem; 
}

/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
GenericSvgaScreen::~GenericSvgaScreen()
{
	DeallocateFrameBufferMapping();

   #ifdef LOCAL_BUFFERING
	delete mpScanPointers[0];
   #endif

    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void GenericSvgaScreen::BeginDraw(PegThing *)
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
void GenericSvgaScreen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        mpScanPointers = new UCHAR *[pMap->wHeight];
        UCHAR *CurrentPtr = pMap->pStart;
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
void GenericSvgaScreen::EndDraw()
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
void GenericSvgaScreen::EndDraw(PegBitmap *pMap)
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
void GenericSvgaScreen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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

	if (wWidth == 1 && Rect.Contains( wXStart, wYStart ) && Rect.Contains( wXEnd, wYEnd ))
	{
		// fast loop for line segments of width 1 that lie entirely inside the screen window

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

				PlotPointView(wCurx, wCury, Color.uForeground);
				PlotPointView(wNextx, wNexty, Color.uForeground);
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

				PlotPointView(wCurx, wCury, Color.uForeground);
				PlotPointView(wNextx, wNexty, Color.uForeground);
			}
		}
	}
	else
	{
		// slow loop for line segments with width > 1 or having an endpoint outside the screen window

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
void GenericSvgaScreen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
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
void GenericSvgaScreen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
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
void GenericSvgaScreen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
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
void GenericSvgaScreen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
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
void GenericSvgaScreen::Capture(PegCapture *Info, PegRect &CaptureRect)
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
void GenericSvgaScreen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
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
	
	                if (uVal != (UCHAR) Getmap->dTransColor)
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
	            Get += Getmap->wWidth;
	        }
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GenericSvgaScreen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
	                        uVal != (UCHAR) Getmap->dTransColor)
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
	
	                if (uVal == (UCHAR) Getmap->dTransColor)
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
                            uVal != (UCHAR) Getmap->dTransColor)
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
void GenericSvgaScreen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void GenericSvgaScreen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
    PegFont *Font, SIGNED iCount, PegRect &Rect)
{
   #ifdef PEG_UNICODE
    PEGCHAR  *pCurrentChar = (PEGCHAR  *) Text;
    PegFont *pFontStart = Font;
   #else
    UCHAR  *pCurrentChar = (UCHAR  *) Text;
   #endif

    UCHAR  *pGetData;
    UCHAR  *pGetDataBase;
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

void GenericSvgaScreen::DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
    PegFont *Font, SIGNED iCount, PegRect &Rect)
{
    UCHAR *pCurrentChar = (UCHAR *) Text;
    UCHAR *pGetData;
    UCHAR *pGetDataBase;
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
            UCHAR *Put = mpScanPointers[ScanRow] + wXpos;

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
                        *Put = (UCHAR) Color.uForeground;
                        break;
                    
                    case 2:
                        *Put = (UCHAR) PCLR_OUTLINE;
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
void GenericSvgaScreen::HidePointer(void)
{
   #ifdef PEG_MOUSE_SUPPORT
    PegThing *pt = NULL;
    PegPresentationManager *pp = pt->Presentation();
    Restore(pp, &mCapture, TRUE);
    mCapture.SetValid(FALSE);
   #endif
}

/*--------------------------------------------------------------------------*/
void GenericSvgaScreen::SetPointer(PegPoint Where)
{
   #ifdef PEG_MOUSE_SUPPORT
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


/*--------------------------------------------------------------------------*/
void GenericSvgaScreen::SetPointerType(UCHAR bType)
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
void GenericSvgaScreen::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *GenericSvgaScreen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 232;
    return muPalette;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GenericSvgaScreen::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
   #ifdef DPMI_API_DJGPP
	AllocatePaletteUpdateMemory( iCount );

	int old_ns = _fargetsel();
	_farsetsel( mPaletteUpdateConvSelector );
	
	// Fill the palette update.
	// Palette update format is Blue, Green, Red, Alignment.
	// It seems that strange things happen when the Alignment byte isn't set to zero.
	pGet += 3 * iFirst;
	int offset = iFirst * 4;
	for (int i = 0; i < iCount; i++, pGet += 3)
	{
		_farnspokeb( offset++, *(pGet + 2) );
		_farnspokeb( offset++, *(pGet + 1) );
		_farnspokeb( offset++, *pGet );
		_farnspokeb( offset++, 0x00 );
	}

	_farsetsel( old_ns );

	UpdatePalette( iFirst, iCount );

   #endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GenericSvgaScreen::ConfigureController(void)
{
	// [8:0] gives the mode number
	// [13:9] is reserved and must be 0
	// [14] = 1 to use linear/flat frame buffer
	// [15] = 0 to clear display memory
	mMode = 0x4000;

	if (mwHRes == 640)	// mwVRes == 480, or else resolution is not supported!
	{
		// mode number 101h
		mMode |= 0x0101;
	}
	else if (mwHRes == 800)	// mwVRes == 600, or else resolution is not supported!
	{
		// mode number 103h
		mMode |= 0x0103;
	}
	else	// mwHRes == 1024 && mwVRes == 768, or else resolution is not supported!
	{
		// mode number 105h
		mMode |= 0x0105;
	}

	SetMode();
	SetPaletteFormat();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifdef DOUBLE_BUFFER
void GenericSvgaScreen::MemoryToScreen()
{
	if (!miInvalidCount)
	{
		return;
	}

	SIGNED iRowLoop;
    UCHAR *pGet;
    UCHAR *pPut;

    PegRect Copy;
    Copy.Set(0, 0, mwHRes - 1, mwVRes - 1);
    Copy &= mInvalid;

    SIGNED iWidth = Copy.Width();

    if (iWidth <= 0)
    {
        return;
    }

    SIGNED iHeight = Copy.Height();
    if (iHeight <= 0)
    {
        return;
    }

    pGet = mpScanPointers[Copy.wTop] + Copy.wLeft;

   #ifdef LOCAL_BUFFERING
	pPut = mpTargetMem + (pGet - mpScanPointers[0]);
   #else
	pPut = pGet - (mwVRes * mwHRes);
   #endif

   #ifdef DPMI_API_DJGPP
	__djgpp_nearptr_enable();
   #endif

	for (iRowLoop = 0; iRowLoop < iHeight; iRowLoop++)
    {
        memcpy( pPut, pGet, iWidth );
        pPut += mwHRes;
        pGet += mwHRes;
    }

   #ifdef DPMI_API_DJGPP
	__djgpp_nearptr_disable();
   #endif
}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR* GenericSvgaScreen::GetFrameBufferAddress( int frameBufferBytes )
{
#ifdef DPMI_API_DJGPP
	// Allocates conventional memory for the VBE mode info.
	int vmi_conv_selector;
	int ret = __dpmi_allocate_dos_memory( 256, &vmi_conv_selector );

	if (ret == -1)
	{
		return NULL;        // failure!
	}

	// Ascertains the VBE mode info conventional memory address.

	unsigned long vmi_conv_addr;

	ret = __dpmi_get_segment_base_address( vmi_conv_selector, &vmi_conv_addr );

	if (ret == -1)
	{
		return NULL
    }

	// Invokes VBE function 01h to return the VBE mode information.

	__dpmi_regs regs;
	regs.x.ax = 0x4F01;
	regs.x.cx = mMode;
	regs.x.es = (vmi_conv_addr >> 4);
	regs.x.di = 0x0000;
	ret = __dpmi_simulate_real_mode_interrupt( 0x0010, &regs );


	// Ascertains the frame buffer device memory physical address.

	mFrameBufferDevMeminfo.address = _farpeekl( vmi_conv_selector, 40 );
    
	// Deallocates the VBE mode info conventional memory.

	ret = __dpmi_free_dos_memory( vmi_conv_selector );

	// Allocates a mapping from the frame buffer device memory physical address to the frame buffer device memory address.

	mFrameBufferDevMeminfo.size = frameBufferBytes;

	ret = __dpmi_physical_address_mapping( &mFrameBufferDevMeminfo );

	// Returns a near pointer to the frame buffer device memory.

	return (UCHAR*) (mFrameBufferDevMeminfo.address + __djgpp_conventional_base);
#endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifdef DPMI_API_DJGPP

void GenericSvgaScreen::DeallocateFrameBufferMapping()
{
	LOG_APPEND0( "\nEntered DeallocateFrameBufferMapping().\n" );

	// Deallocates the mapping from the frame buffer device memory physical address to the frame buffer device memory address.
	int	ret = __dpmi_free_physical_address_mapping( &mFrameBufferDevMeminfo );
}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifdef DPMI_API_DJGPP
void GenericSvgaScreen::AllocatePaletteUpdateMemory( int paletteUpdateEntries )
{
	// Allocates conventional memory for the palette update.
	// [Each palette update entry consumes 4 bytes.]

	int ret = __dpmi_allocate_dos_memory( paletteUpdateEntries * 4, &mPaletteUpdateConvSelector );

}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifdef DPMI_API_DJGPP
void GenericSvgaScreen::UpdatePalette( WORD iFirst, WORD iCount )
{
	// Ascertains the palette update conventional memory address.

	unsigned long pu_conv_addr;
	int ret = __dpmi_get_segment_base_address( mPaletteUpdateConvSelector, &pu_conv_addr );

	// Invokes VBE function 09h to set the palette data.

	__dpmi_regs regs;
	regs.x.ax = 0x4F09;
	regs.x.cx = iCount;
	regs.x.dx = iFirst;
	regs.x.es = pu_conv_addr >> 4;
	regs.x.di = 0x0000;
	ret = __dpmi_simulate_real_mode_interrupt( 0x0010, &regs );

	// Deallocates the palette update conventional memory.

	ret = __dpmi_free_dos_memory( mPaletteUpdateConvSelector );
}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifdef DPMI_API_DJGPP
void GenericSvgaScreen::SetMode()
{
	// Invokes VBE function 02h to set the VBE mode.

	__dpmi_regs regs;
	regs.x.ax = 0x4F02;
	regs.x.bx = mMode;

	int ret =  __dpmi_simulate_real_mode_interrupt( 0x0010, &regs );

}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifdef DPMI_API_DJGPP
void GenericSvgaScreen::SetPaletteFormat()
{
	// Invokes VBE function 08h to set the DAC palette format.

	__dpmi_regs regs;
	regs.x.ax = 0x4F08;
	regs.x.bx = 0x0800;
	int ret = __dpmi_simulate_real_mode_interrupt( 0x0010, &regs );

}
#endif
