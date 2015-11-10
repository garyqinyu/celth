/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ct69_24.cpp - Linear screen driver for the CT69000 at 24bpp operation.
//
// Author: Kenneth G. Maxwell
//         Jim DeLisle
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
// This driver is intended for systems which have direct, linear (i.e. not
// paged) access to the video frame buffer. This driver supports hardware cursor
// and double-buffering using the controller bit-blit feature.
// 
// The driver supports two different resolutions out of the box.
// These resolutions are:
//
// MODE640:  Configure for 640x480  full color CRT and/or LCD.
// MODE800:  Configure for 800x600  full color CRT and/or LCD.
//
// Any other resolution can also be supported by modifying the timing
// register configuration in the ConfigureController function below.
//
// All available configuration flags are found in the ct69_24.hpp header file.
//
//
// Known Limitations:
//
// The CT69000 supports up to 800x600 resolution at 24bpp
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"

#if defined(USE_PCI)
#include "pegpci.hpp"
#endif

extern UCHAR DefPalette256[];

static void VGA_DELAY();

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new CT6924Screen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
CT6924Screen::CT6924Screen(PegRect &Rect) : PegScreen(Rect)
{
    mdNumColors = 256;

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();

    mpScanPointers = new COLORVAL PEGFAR *[Rect.Height()];
    UCHAR PEGFAR *CurrentPtr = GetVideoAddress();

    for (SIGNED iLoop = 0; iLoop < Rect.Height(); iLoop++)
    {
        mpScanPointers[iLoop] = (COLORVAL *) CurrentPtr;
        CurrentPtr += mwHRes * 3;
    }

    mLastPointerPos.x = Rect.Width() / 2;
    mLastPointerPos.y = Rect.Height() / 2;
    mbPointerHidden = FALSE;
    mwDrawNesting = 0;

    ConfigureController();        // set up controller registers

    //SetPalette(0, 232, DefPalette256);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *CT6924Screen::GetVideoAddress(void)
{
    UCHAR *pMem = NULL;

   #if defined(USE_PCI)
    pMem = PegPCIGetVideoAddress(PEGPCI_VGACLASS, PEGPCI_VENDORID, 
                                 PEGPCI_DEVICEID);
   #else
    pMem = (UCHAR *) VID_MEM_BASE;
   #endif

    mpVidMemBase = pMem;

   #ifdef DOUBLE_BUFFER
    if(pMem)
    {
        pMem += FRAME_BUFFER_SIZE;
    }
   #endif

    return (pMem);
}

/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
CT6924Screen::~CT6924Screen()
{
    delete [] mpScanPointers[0];

    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void CT6924Screen::BeginDraw(PegThing *)
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
void CT6924Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
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
        UCHAR PEGFAR *CurrentPtr = pMap->pStart;
        for (SIGNED iLoop = 0; iLoop < pMap->wHeight; iLoop++)
        {
            mpScanPointers[iLoop] = (COLORVAL *) CurrentPtr;
            CurrentPtr += pMap->wWidth * 3;
        }
        mVirtualRect.Set(0, 0, pMap->wWidth - 1, pMap->wHeight - 1);
        mbVirtualDraw = TRUE;
    }
}


/*--------------------------------------------------------------------------*/
void CT6924Screen::EndDraw()
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

       #if defined(DOUBLE_BUFFER)
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
void CT6924Screen::EndDraw(PegBitmap *pMap)
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
PegBitmap *CT6924Screen::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
{
   PegBitmap *pMap = NULL;

    if (wWidth && wHeight)
    {
        pMap = new PegBitmap;
        pMap->wWidth = wWidth;
        pMap->wHeight = wHeight;
        pMap->uFlags = BMF_NATIVE;
        pMap->uBitsPix = 24;

        DWORD dSize = (DWORD) wWidth * (DWORD) wHeight * 3;

       #ifdef USE_VID_MEM_MANAGER
        pMap->pStart = AllocBitmap(dSize);

        if (!pMap->pStart)  // out of video memory?
        {
            pMap->pStart = new UCHAR[dSize];    // try local memory
        }
        else
        {
            pMap->uFlags = BMF_SPRITE|BMF_NATIVE;
        }
       #else
        pMap->pStart = new UCHAR[dSize];
       #endif

        if (!pMap->pStart)
        {
            delete pMap;
            return NULL;
        }
		if(bHasTrans)
		{
			memset(pMap->pStart, TRANSPARENCY, dSize);
			pMap->uFlags |= BMF_HAS_TRANS;
		}
		else
		{
	        memset(pMap->pStart, BLACK, dSize);
		}
    }
    return pMap;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void CT6924Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    SIGNED iLen = wXEnd - wXStart + 1;
    SIGNED iLoop;

    UCHAR cRed = RedVal(Color);
    UCHAR cGreen = GreenVal(Color);
    UCHAR cBlue = BlueVal(Color);

    if (!iLen)
    {
        return;
    }
    while(wWidth-- > 0)
    {
        UCHAR *Put = ((UCHAR *) mpScanPointers[wYPos]) + (wXStart * 3);

        for (iLoop = 0; iLoop < iLen; iLoop++)
        {
            #ifdef RED_BYTE_FIRST
            *Put++ = cRed;
            *Put++ = cGreen;
            *Put++ = cBlue;
            #else
            *Put++ = cBlue;
            *Put++ = cGreen;
            *Put++ = cRed;
            #endif
        }
        wYPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR cRed = RedVal(Color);
    UCHAR cGreen = GreenVal(Color);
    UCHAR cBlue = BlueVal(Color);

    while(wYStart <= wYEnd)
    {
        SIGNED lWidth = wWidth;
        UCHAR *Put = (UCHAR *) mpScanPointers[wYStart] + (wXPos * 3);

        while (lWidth-- > 0)
        {
           #ifdef RED_BYTE_FIRST
            *Put++ = cRed;
            *Put++ = cGreen;
            *Put = cBlue;
           #else
            *Put++ = cBlue;
            *Put++ = cGreen;
            *Put = cRed;
           #endif
        }
        wYStart++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    UCHAR *Put = (UCHAR *) mpScanPointers[wYPos] + (wXStart * 3);

    while (wXStart <= wXEnd)
    {
        *Put ^= 0xff;
        Put++;
        *Put ^= 0xff;
        Put++;
        *Put ^= 0xff;
        Put += 4;
        wXStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    while (wYStart <= wYEnd)
    {
        UCHAR *Put = (UCHAR *) mpScanPointers[wYStart] + (wXPos * 3);
        *Put ^= 0xff;
        Put++;
        *Put ^= 0xff;
        Put++;
        *Put ^= 0xff;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void CT6924Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
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
    LONG Size = pMap->wWidth * pMap->wHeight * 3;
    Info->Realloc(Size);

    UCHAR *GetStart = (UCHAR *) mpScanPointers[CaptureRect.wTop] + (CaptureRect.wLeft * 3);

    // make room for the memory bitmap:

    pMap->uFlags = 0;         // raw format
    pMap->uBitsPix = 24;      // 24 bits per pixel

    // fill in the image with our captured info:

    UCHAR *Put = pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        UCHAR *Get = GetStart;
        for (WORD wLoop = 0; wLoop < pMap->wWidth; wLoop++)
        {
            *Put++ = *Get++;
            *Put++ = *Get++;
            *Put++ = *Get++;
        }
        GetStart += mwHRes * 3;
    }
    Info->SetValid(TRUE);
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
    }
    else
    {
        if (Getmap->uBitsPix == 24)
        {
            DrawTrueColorBitmap(Where, Getmap, View);
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
		            UCHAR *Put = (UCHAR *) mpScanPointers[wLine] + (View.wLeft * 3);
		            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
		            {
		                WORD wVal = *Get++;
		                if (wVal != Getmap->dTransColor)
		                {
                            wVal *= 3;
                           #ifdef RED_BYTE_FIRST
		                    *Put++ = DefPalette256[wVal];
		                    *Put++ = DefPalette256[wVal + 1];
		                    *Put++ = DefPalette256[wVal + 2];
                           #else
		                    *Put++ = DefPalette256[wVal + 2];
		                    *Put++ = DefPalette256[wVal + 1];
		                    *Put++ = DefPalette256[wVal];
                           #endif
		                }
                        else
                        {
                            Put += 3;
                        }
		            }
		            Get += Getmap->wWidth - View.Width();
		        }
	        }
	        else
	        {
		        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
		        {
		            UCHAR *Put = (UCHAR *) mpScanPointers[wLine] + (View.wLeft * 3);

		            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
		            {
		                WORD wVal = *Get++;
                        wVal *= 3;
                           #ifdef RED_BYTE_FIRST
		                    *Put++ = DefPalette256[wVal];
		                    *Put++ = DefPalette256[wVal + 1];
		                    *Put++ = DefPalette256[wVal + 2];
                           #else
		                    *Put++ = DefPalette256[wVal + 2];
		                    *Put++ = DefPalette256[wVal + 1];
		                    *Put++ = DefPalette256[wVal];
                           #endif
		            }
		            Get += Getmap->wWidth - View.Width();
		        }
	        }
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::DrawTrueColorBitmap(const PegPoint Where,
    const PegBitmap *Getmap, const PegRect View)
{
    UCHAR *Get = Getmap->pStart;
    Get += (View.wTop - Where.y) * (Getmap->wWidth * 3);
    Get += (View.wLeft - Where.x) * 3;

    #ifdef BLUE_BYTE_FIRST
    if (IS_NATIVE(Getmap))
    {
    #endif

        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            UCHAR *Put = (UCHAR *) mpScanPointers[wLine] + (View.wLeft * 3);

            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
		    {
                *Put++ = *Get++;
                *Put++ = *Get++;
                *Put++ = *Get++;
            }
            Get += (Getmap->wWidth - View.Width()) * 3;
        }

    #ifdef BLUE_BYTE_FIRST
    }
    else
    {
        // here if the bitmap is in R-G-B order but needs to be written out
        // in B-G-R order:
       #if defined(SWAP_24BPP_IMAGE)
        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            UCHAR *Put = (UCHAR *) mpScanPointers[wLine] + (View.wLeft * 3);

            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
		    {
                *Put++ = *Get++;
                *Put++ = *Get++;
                *Put++ = *Get++;
            }
            Get += (Getmap->wWidth - View.Width()) * 3;
        }
       #else
        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            UCHAR *Put = (UCHAR *) mpScanPointers[wLine] + (View.wLeft * 3);

            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
		    {
                *Put++ = *(Get + 2);
                *Put++ = *(Get + 1);
                *Put++ = *Get;
                Get += 3;
            }
            Get += (Getmap->wWidth - View.Width()) * 3;
        }
       #endif
    }
    #endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
    const PegBitmap *Getmap)
{
    UCHAR *Get = Getmap->pStart;
    WORD wVal;
    SIGNED uCount;

    SIGNED wLine = Where.y;

    uCount = 0;

    while (wLine < View.wTop)
    {
        uCount = 0;

        while(uCount < Getmap->wWidth)
        {
            wVal = *Get++;
            if (wVal & 0x80)
            {
                wVal = (wVal & 0x7f) + 1;
                uCount += wVal;
                Get += wVal;
            }
            else
            {
                Get++;
                uCount += wVal + 1;
            }
        }
        wLine++;
    }

    if (HAS_TRANS(Getmap))
    {
	    while (wLine <= View.wBottom)
	    {
	        UCHAR *Put = (UCHAR *) mpScanPointers[wLine] + (Where.x * 3);
	        SIGNED wLoop1 = Where.x;
	
	        while (wLoop1 < Where.x + Getmap->wWidth)
	        {
	            wVal = *Get++;
	
	            if (wVal & 0x80)        // raw packet?
	            {
	                uCount = (wVal & 0x7f) + 1;
	                
	                while (uCount--)
	                {
	                    wVal = *Get++;
	                    if (wLoop1 >= View.wLeft &&
	                        wLoop1 <= View.wRight &&
	                        wVal != Getmap->dTransColor)
	                    {
                            wVal *= 3;
                           #ifdef RED_BYTE_FIRST
                            *Put++ = DefPalette256[wVal];
		                    *Put++ = DefPalette256[wVal + 1];
		                    *Put++ = DefPalette256[wVal + 2];
                           #else
                            *Put++ = DefPalette256[wVal + 2];
		                    *Put++ = DefPalette256[wVal + 1];
		                    *Put++ = DefPalette256[wVal];
                           #endif
	                    }
                        else
                        {
                            Put += 3;
                        }
	                    wLoop1++;
	                }
	            }
	            else
	            {
	                uCount = wVal + 1;
	                wVal = *Get++;
	
	                if (wVal == Getmap->dTransColor)
	                {
	                    wLoop1 += uCount;
	                    Put += uCount * 3;
	                }
	                else
	                {
                        wVal *= 3;

	                    while(uCount--)
	                    {
	                        if (wLoop1 >= View.wLeft &&
	                            wLoop1 <= View.wRight)
	                        {
                               #ifdef RED_BYTE_FIRST
                                *Put++ = DefPalette256[wVal];
	    	                    *Put++ = DefPalette256[wVal + 1];
		                        *Put++ = DefPalette256[wVal + 2];
                               #else
                                *Put++ = DefPalette256[wVal + 2];
	    	                    *Put++ = DefPalette256[wVal + 1];
		                        *Put++ = DefPalette256[wVal];
                               #endif
	                        }
	                        else
	                        {
	                            Put += 3;
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
	        UCHAR *Put = (UCHAR *) mpScanPointers[wLine] + (Where.x * 3);
	        SIGNED wLoop1 = Where.x;
	
	        while (wLoop1 < Where.x + Getmap->wWidth)
	        {
	            wVal = *Get++;
	
	            if (wVal & 0x80)        // raw packet?
	            {
	                uCount = (wVal & 0x7f) + 1;
                
	                while (uCount--)
	                {
	                    wVal = *Get++;
	                    if (wLoop1 >= View.wLeft &&
	                        wLoop1 <= View.wRight &&
                            wVal != Getmap->dTransColor)
	                    {
                            wVal *= 3;
                           #ifdef RED_BYTE_FIRST
                            *Put++ = DefPalette256[wVal];
		                    *Put++ = DefPalette256[wVal + 1];
		                    *Put++ = DefPalette256[wVal + 2];
                           #else
                            *Put++ = DefPalette256[wVal + 2];
		                    *Put++ = DefPalette256[wVal + 1];
		                    *Put++ = DefPalette256[wVal];
                           #endif
	                    }
                        else
                        {
    	                    Put += 3;
                        }
	                    wLoop1++;
	                }
	            }
	            else
	            {
	                uCount = wVal + 1;
	                wVal = *Get++;
                    wVal *= 3;
	
                    while(uCount--)
	                {
	                    if (wLoop1 >= View.wLeft &&
	                        wLoop1 <= View.wRight)
	                    {
                           #ifdef RED_BYTE_FIRST
                            *Put++ = DefPalette256[wVal];
		                    *Put++ = DefPalette256[wVal + 1];
		                    *Put++ = DefPalette256[wVal + 2];
                           #else
                            *Put++ = DefPalette256[wVal + 2];
		                    *Put++ = DefPalette256[wVal + 1];
		                    *Put++ = DefPalette256[wVal];
                           #endif
	                    }
	                    else
	                    {
	                        Put += 3;
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
void CT6924Screen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void CT6924Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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

    UCHAR cRed = RedVal(Color.uForeground);
    UCHAR cGreen = GreenVal(Color.uForeground);
    UCHAR cBlue = BlueVal(Color.uForeground);

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

            UCHAR *Put = (UCHAR *) mpScanPointers[ScanRow] + (wXpos * 3);

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
                       #ifdef RED_BYTE_FIRST
                        *Put++ = cRed;
                        *Put++ = cGreen;
                        *Put++ = cBlue;
                       #else
                        *Put++ = cBlue;
                        *Put++ = cGreen;
                        *Put++ = cRed;
                       #endif

                    }
                    else
                    {
                        if (Color.uFlags & CF_FILL)
                        {
                           #ifdef RED_BYTE_FIRST
                            *Put++ = RedVal(Color.uBackground);
                            *Put++ = GreenVal(Color.uBackground);
                            *Put++ = BlueVal(Color.uBackground);
                           #else
                            *Put++ = BlueVal(Color.uBackground);
                            *Put++ = GreenVal(Color.uBackground);
                            *Put++ = RedVal(Color.uBackground);
                           #endif
                        }
                        else
                        {
                            Put += 3;
                        }
                    }
                }
                else
                {
                    Put += 3;
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

void CT6924Screen::DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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

void CT6924Screen::ShowPointer(BOOL bShow)
{
   #ifdef HARDWARE_CURSOR
    if (bShow)
    {
        WriteExtReg(0xa0, 0x11);
    }
    else
    {
        WriteExtReg(0xa0, 0x00);
    }
   #endif
    PegScreen::ShowPointer(bShow);
}



/*--------------------------------------------------------------------------*/
void CT6924Screen::HidePointer(void)
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
void CT6924Screen::SetPointer(PegPoint Where)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (mbShowPointer)
    {
	   #if !defined(HARDWARE_CURSOR)
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
	   #else
	
	    mLastPointerPos = Where;
	    Where.x -= miCurXOffset;
	    Where.y -= miCurYOffset;
	    WriteExtReg(0xa4, (UCHAR) Where.x);
	    WriteExtReg(0xa5, (UCHAR) (Where.x >> 8));
	    WriteExtReg(0xa6, (UCHAR) Where.y);
	    WriteExtReg(0xa7, (UCHAR) (Where.y >> 8));
	   #endif
    }
   #endif
}


/*--------------------------------------------------------------------------*/
void CT6924Screen::SetPointerType(UCHAR bType)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (bType < NUM_POINTER_TYPES)
    {
       #if !defined(HARDWARE_CURSOR)
        LOCK_PEG
        HidePointer();
       #endif

        mpCurPointer = mpPointers[bType].Bitmap;

       #if defined(HARDWARE_CURSOR)
        // reset the pointer register:
        UCHAR uVal = ReadExtReg(0xa2);
        uVal &= 0xf0;
        uVal |= bType;
        WriteExtReg(0xa2, uVal);
       #endif
        
        miCurXOffset = mpPointers[bType].xOffset;
        miCurYOffset = mpPointers[bType].yOffset;
        SetPointer(mLastPointerPos);

       #if !defined(HARDWARE_CURSOR)
        UNLOCK_PEG
       #endif
    }
   #endif
}


/*--------------------------------------------------------------------------*/
void CT6924Screen::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *CT6924Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = PEG_NUM_COLORS;
    return NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
    UCHAR *pReg = mpVidMemBase + PALETTE_INDEX;

    *pReg = (UCHAR) iFirst;
    VGA_DELAY();

    for (SIGNED iLoop = 0; iLoop < iCount * 3; iLoop++)
    {
        UCHAR uVal = *pGet++;
        //uVal >>= 2;
        *(pReg + 1) = uVal;
        VGA_DELAY();
    }
}

/*--------------------------------------------------------------------------*/
#if defined(MODE640) || defined(MODE800) 
#else

 #error ** You Must Define a Recognized Mode for Conguration **

#endif

typedef struct {
    int iReg;
    int iVal;
} VID_REG_VAL;

/* Sequencer Registers: */

VID_REG_VAL SequenceRegs[] = {
{0x00,  0x03},       // Reset Register
{0x01,  0x01},       // Clock Mode  0x01??
{0x02,  0x0f},       // Map Mask
{0x03,  0x00},       // Char Map
//{0x04,  0x0e},       // Memory Mode   0x02? 0x0e?
{0x04,  0x04},       // Memory Mode   0x02? 0x0e?
//{0x07,  0x11},       // Horz Char Count
{0x07,  0x00},       // Horz Char Count  24
{0x00,  0x03},       // Reset Register
{-1, -1}
};

/* Graphics Registers: */

VID_REG_VAL GraphicsRegs[] = {

{0x00,  0x00},       // Set/Reset
{0x01,  0x00},       // Set/Reset Enable
{0x02,  0x00},       // Color Compare
{0x03,  0x00},       // Data Rotate
{0x04,  0x00},       // Read Plane Select
{0x05,  0x00},       // Mode Register   0x40??
{0x06,  0x05},       // Misc Reg   0x05 ??
{0x07,  0x0f},       // Color Dont Care
{0x08,  0xff},       // Bit Mask
{-1, -1}
};

/* Attrib Registers: */


VID_REG_VAL AttribRegs[] = {

{0x00,  0x00},       // ColorData00
{0x01,  0x00},       // ColorData01 24
{0x02,  0x02},       // ColorData02
{0x03,  0x02},       // ColorData03	24
{0x04,  0x04},       // ColorData04
{0x05,  0x05},       // ColorData05
{0x06,  0x06},       // ColorData06
{0x07,  0x07},       // ColorData07
{0x08,  0x08},       // ColorData08
{0x09,  0x09},       // ColorData09
{0x0a,  0x0a},       // ColorData0a
{0x0b,  0x0b},       // ColorData0b
{0x0c,  0x0c},       // ColorData0c
{0x0d,  0x0d},       // ColorData0d
{0x0e,  0x0e},       // ColorData0e
{0x0f,  0x0f},       // ColorData0f
{0x10,  0x01},       // Mode Control  0x10? 0x01?
{0x11,  0x00},       // Overscan Color
{0x12,  0x0f},       // Memory Plane En
{0x13,  0x00},       // Horz Pix Panning
{0x14,  0x00},       // Color Select
{-1, -1}
};

/* Extension Registers: (common) */

VID_REG_VAL ExtensionRegs[] = {

{0xd0,  0x0a},       // Powerdown Control- kill all!
{0x08,  0x03},       // Host Bus Config
{0x09,  0x01},       // I/O Control
{0x0a,  0x02},       // Frame Buff Mapping 0x00
{0x0b,  0x01},       // PCI Burst Support
{0x0e,  0x00},       // Frame Buff Page
{0x20,  0x20},       // BitBlt Config
{0x40,  0x03},       // Mem Access Control  0x02
{0x60,  0x00},       // Video Pin Control
{0x61,  0x00},       // DPMS Sync
{0x62,  0x00},       // GPIO Control
{0x63,  0xb6},       // GPIO Pin Data
{0x67,  0x00},       // Pin Tri-State
{0x70,  0xfe},       // Config Pins 0
{0x71,  0xff},       // Config Pins 1
{0x80,  0x80},       // Pipeline Config 0
{0x81,  0x06},       // Pipeline Config 1 0x00
{0x82,  0x00},       // Pipeline Config 2
{0xa0,  0x00},       // turn off hardware cursor
{0xc0,  0x7d},       // DotClk0 VCO M-Div Low
{0xc1,  0x07},       // DotClk0 VCO N-Div Low
{0xc2,  0x00},       // DotClk0 VCO M-N-Div High
{0xc3,  0x34},       // DotClk0 Divisor Select
{0xc4,  0x55},       // DotClk1 VCO M-Div Low
{0xc5,  0x09},       // DotClk1 VCO N-Div Low
{0xc6,  0x00},       // DotClk1 VCO M-N-Div High
{0xc7,  0x24},       // DotClk1 Divisor Select

#if defined(MODE640)
{0xc8,  0x7d},       // DotClk2 VCO M-Div Low
{0xc9,  0x07},       // DotClk2 VCO N-Div Low
{0xca,  0x00},       // DotClk2 VCO M-N-Div High
{0xcb,  0x34},       // DotClk2 Divisor Select
#elif defined(MODE800)
{0xc8,  0x7d},       // DotClk2 VCO M-Div Low
{0xc9,  0x07},       // DotClk2 VCO N-Div Low
{0xca,  0x00},       // DotClk2 VCO M-N-Div High
{0xcb,  0x24},       // DotClk2 Divisor Select
#endif

{0xcc,  0x38},       // MemClk VCO M-Divisor
{0xcd,  0x03},       // MemClk VCO N-Divisor
{0xce,  0x90},       // MemClk VCO Divisor Select
{0xcf,  0x06},       // Clock Config Reg
{0xd0,  0x0f},       // Powerdown Control
{0xd1,  0x01},       // Power Conserve
{-1, -1}
};


/* Flat Panel Registers: */

VID_REG_VAL FlatPanelRegs[] = {

#ifdef CT_LCD_SUPPORT
{0x01,  0x02},       // FP Mode
#else
{0x01,  0x01},       // CRT Mode
#endif

{0x02,  0x00},       // FP Mode Control
{0x03,  0x08},       // FP DotClock Src
{0x04,  0x81},       // Power Seq Delay
{0x05,  0x21},       // Power Down Ctrl 1
{0x06,  0x03},       // FP Power Down Ctrl
{0x08,  0xcc},       // FP Pin Polarity
{0x0a,  0x08},       // Prog Output Drive
{0x0b,  0x01},       // Pin Control 1
{0x0c,  0x00},       // Pin Control 2
{0x0f,  0x02},       // Activity Timer
{0x10,  0x0c},       // FP Format 0
{0x11,  0xe0},       // FP Format 1
{0x12,  0x50},       // FP Format 2
{0x13,  0x00},       // FP Format 3
{0x16,  0x03},       // FRC Option Select
{0x17,  0xbd},       // Polynomial FRC
{0x18,  0x00},       // Text Mode Ctrl
{0x19,  0x88},       // Blink Rate
{0x1a,  0x00},       // STN Buffering
{0x1e,  0x80},       // ACDCLK Control

#if defined(MODE640)
{0x20,  0x4f},       // Horz Size LSB
{0x21,  0x51},       // Horz Sync Start LSB
{0x22,  0x1d},       // Horz Sync End
{0x23,  0x5e},       // Horz Total LSB
{0x24,  0x50},       // Hsynch Delay LSB
{0x25,  0x00},       // Horz Overflow 1
{0x26,  0x00},       // Horz Overflow 2
{0x27,  0x0b},       // HSynch Width
{0x30,  0xdf},       // Vert Size LSB
{0x31,  0xe8},       // Vert Synch LSB
{0x32,  0x0c},       // Vert Synch End
{0x33,  0x0b},       // Vert Total LSB
{0x34,  0x00},       // VSynch Delay LSB
{0x35,  0x11},       // Vert Overflow 1
{0x36,  0x02},       // Vert Overflow 2
#elif defined(MODE800)
{0x20,  0x63},       // Horz Size LSB
{0x21,  0x66},       // Horz Sync Start LSB
{0x22,  0x1d},       // Horz Sync End
{0x23,  0x7e},       // Horz Total LSB
{0x24,  0x64},       // Hsynch Delay LSB
{0x25,  0x00},       // Horz Overflow 1
{0x26,  0x00},       // Horz Overflow 2
{0x27,  0x0b},       // HSynch Width
{0x30,  0x57},       // Vert Size LSB
{0x31,  0xd0},       // Vert Synch LSB
{0x32,  0x0c},       // Vert Synch End
{0x33,  0xe0},       // Vert Total LSB
{0x34,  0x00},       // VSynch Delay LSB
{0x35,  0x22},       // Vert Overflow 1
{0x36,  0x02},       // Vert Overflow 2
#endif
{0x37,  0x80},       // VSynch Disable
{0x40,  0x83},       // Horz Compensation
{0x41,  0x00},       // Horz Stretching
{0x48,  0x13},       // Vert Compensation
{0x70,  0x00},       // TMED Red Seed
{0x71,  0x55},       // TMED Green Seed
{0x72,  0xaa},       // TMED Blue Seed
{0x73,  0x00},       // TMED Control
{0x74,  0x5f},       // TMED2 Shift
{-1, -1}
};



VID_REG_VAL CRTCRegs[] = {

#if defined(MODE640)
{0x00,  0x5f},       // H Cols
{0x01,  0x4f},       // H Disp End
{0x02,  0x50},       // Start H Blank
{0x03,  0x82},       // End H Blank
{0x04,  0x54},       // H Sync Start  24
{0x05,  0x80},       // H Sync End    24
{0x06,  0x0b},       // V Total
{0x07,  0x3e},       // Overflow
{0x08,  0x00},       // Initial Row
{0x09,  0x40},       // Max Row
#elif defined(MODE800)
{0x00,  0x7f},       // H Cols
{0x01,  0x63},       // H Disp End
{0x02,  0x64},       // Start H Blank
{0x03,  0x82},       // End H Blank
{0x04,  0x6b},       // H Sync Start
{0x05,  0x1b},       // H Sync End
{0x06,  0x72},       // V Total
{0x07,  0xf0},       // Overflow
{0x08,  0x00},       // Initial Row
{0x09,  0x60},       // Max Row
#endif

{0x0a,  0x00},       // Cursor Start
{0x0b,  0x00},       // Cursor End
{0x0c,  0x00},       // Start MSB
{0x0d,  0x00},       // Start LSB
{0x0e,  0x00},       // Cursor Loc MSB
{0x0f,  0x00},       // Cursor Loc LSB

#if defined(MODE640)
{0x10,  0xea},       // Vert Sync Start
{0x11,  0x8c},       // Vert Sync End
{0x12,  0xdf},       // Vert Disp End
{0x13,  0xf0},       // Row Offset orig 24
{0x14,  0x00},       // Addr Mode / Underline Addr  0x00
{0x15,  0xe7},       // Start VBlank
{0x16,  0x04},       // End VBlank
#elif defined(MODE800)
{0x10,  0x58},       // Vert Sync Start
{0x11,  0x8c},       // Vert Sync End
{0x12,  0x57},       // Vert Disp End
{0x13,  0x2c},       // Row Offset
{0x14,  0x00},       // Addr Mode / Underline Addr  0x00
{0x15,  0x58},       // Start VBlank
{0x16,  0x72},       // End VBlank
#endif
{0x17,  0xe3},       // CRTC Mode 
{0x18,  0xff},       // Line Compare
{0x30,  0x02},       // Ex Vert Total
#if defined(MODE640)
{0x31,  0x01},       // Ex Vert Disp End
{0x32,  0x01},       // Ex Vert Sync Start
{0x33,  0x01},       // Ex Vert Blank Start
#elif defined(MODE800)
{0x31,  0x02},       // Ex Vert Disp End
{0x32,  0x02},       // Ex Vert Sync Start
{0x33,  0x02},       // Ex Vert Blank Start
#endif
{0x38,  0x00},       // Ex Horz Total
#if defined(MODE640)
{0x3c,  0x40},       // Ex Horz Blank End
#elif defined(MODE800)
{0x3c,  0x80},       // Ex Horz Blank End
#endif
{0x40,  0x00},       // Ex Start Addr
#if defined(MODE640)
{0x41,  0x00},       // Ex Offset
#elif defined(MODE800)
{0x41,  0x01},       // Ex Offset
#endif
{0x70,  0x4f},       // Interlace Control
{0x71,  0x00},       // NTSC Output Control  0x00 ??
{-1, -1}
};

/*--------------------------------------------------------------------------*/
void VGA_DELAY(void)
{
    #ifdef __WATCOMC__
    for (WORD wLoop = 0; wLoop < 200; wLoop++)
    {
        dummy++;
    }
    #else
    for (WORD wLoop = 0; wLoop < 200; wLoop++)
    {
        wLoop++;
    }
    #endif
}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::WriteExtReg(int index, int val)
{
    UCHAR *pMem = mpVidMemBase + EXTENSION_REG_INDEX;
    *pMem++ = index;
    *pMem = val;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR CT6924Screen::ReadExtReg(int index)
{
    UCHAR *pMem = mpVidMemBase + EXTENSION_REG_INDEX;
    *pMem++ = index;
    return (*pMem);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::WriteFPReg(int index, int val)
{
    UCHAR *pMem = mpVidMemBase + FLAT_PANEL_INDEX;
    *pMem++ = index;
    *pMem = val;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::WriteCRTCReg(int index, int val)
{
    UCHAR *pMem = mpVidMemBase + CRTC_REG_INDEX;
    *pMem++ = index;
    *pMem = val;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int CT6924Screen::ReadCRTCReg(int index)
{ 
    UCHAR *pMem = mpVidMemBase + CRTC_REG_INDEX;
    *pMem++ = index;
    return(*pMem);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR CT6924Screen::ReadSeqReg(int index)
{
    UCHAR *pMem = mpVidMemBase + SEQUENCER_INDEX;
    *pMem++ = index;
    return(*pMem);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::WriteSeqReg(int index, int val)
{
    UCHAR *pMem = mpVidMemBase + SEQUENCER_INDEX;
    *pMem++ = index;
    *pMem = val;
}

UCHAR guDummyStatusVal;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::WriteAttribReg(int rg, int val)
{
    UCHAR *pMem;
    // read from the status register to put the attribute controller
    // index/data flip-flop back into index mode:

    pMem = mpVidMemBase + STATUS_REG_INDEX;
    guDummyStatusVal = *pMem;
    
    // now write the index, then the data,
    //* then index|0x20 to restart controller    

    pMem = mpVidMemBase + ATTRIB_CONTROLLER_INDEX;
    *pMem = rg;
    *pMem = val;
    *pMem = (rg | 0x20); // restart attrib controller
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::WriteGraphicsReg(int index, int val)
{
    UCHAR *pMem = mpVidMemBase + GRAPHICS_INDEX;
    *pMem++ = index;
    *pMem = val;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::WaitNotBusy(void)
{
    volatile DWORD *pReg = (DWORD *) (mpVidMemBase + BITBLT_REGS);
    pReg += 4;
    DWORD dVal = *pReg;

    while(dVal & 0x80000000)
    {
        dVal = *pReg;
    }
}


#ifdef HARDWARE_CURSOR
UCHAR CursorPalette[6] = {0, 0, 0, 0xff, 0xff, 0xff};
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::ConfigureController(void)
{
    UCHAR uVal;
    UCHAR *pReg;
    VID_REG_VAL *pRegVal;

    WriteSeqReg(0, 0x00);   // synchronous reset sequencer

    // Miscellaneous Output Register

    pReg = mpVidMemBase + MISC_OUTPUT_REG;

    #if defined(MODE640)
    *pReg = 0xeb;
    #else
    *pReg = 0x2b;
    #endif

    pReg = mpVidMemBase + FEATURE_CONTROL_REG;

    #if defined(MODE640)
    *pReg = 0x01;         // 640x480
    #else
    *pReg = 0x00;         // 800x600 or above
    #endif
    VGA_DELAY();

    // setup the extension registers:

    pRegVal = ExtensionRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteExtReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }

    // setup the flat panel registers:

    pRegVal = FlatPanelRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteFPReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }

    // setup the attribute controller:

    pRegVal = AttribRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteAttribReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }
//    WriteAttribReg(0x20, AttribRegs[0].iVal);

    // setup the crt controller regs:

    uVal = ReadCRTCReg(0x11);
    uVal &= 0x7f;
    WriteCRTCReg(0x11, uVal);

    pRegVal = CRTCRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteCRTCReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }

    // setup the graphics registers:

    pRegVal = GraphicsRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteGraphicsReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }

    pReg = mpVidMemBase + PIXEL_MASK_REG;
    *pReg = 0xff;

    //WriteExtReg(0xd0,  0x0f);   // Powerdown Control, turn on

    // setup the sequencer regs:

    WriteSeqReg(0, 0x03);   // restart sequencer

    pRegVal = SequenceRegs;

    while(pRegVal->iReg >= 0)
    {
        WriteSeqReg(pRegVal->iReg, pRegVal->iVal);
        pRegVal++;
    }

    //SetPalette(0, 232, DefPalette256);

    #if defined(MODE640)
    HorizontalLine(0, 639, 0, RED, 480);
    #elif defined(MODE800)
    HorizontalLine(0, 799, 0, BLACK, 600);
    #endif


   #if defined(PEG_MOUSE_SUPPORT) && defined(HARDWARE_CURSOR)
    UCHAR  AndVal, XorVal, uMask, uRow, uCol, uByteCount, uSrcVal;
    UCHAR *pMem = mpVidMemBase;
    WORD wLoop;
    DWORD dOffset = FRAME_BUFFER_SIZE;
     
    #ifdef DOUBLE_BUFFER
    dOffset += FRAME_BUFFER_SIZE;
    #endif

    pMem += dOffset;

    WriteExtReg(0xa1, 0);
    WriteExtReg(0xa2, (dOffset >> 12) << 4);
    WriteExtReg(0xa3, dOffset >> 16);

    SIGNED Pos = mwHRes / 2;

    WriteExtReg(0xa4, Pos);
    WriteExtReg(0xa5, Pos >> 8);

    Pos = (mwVRes / 2);

    WriteExtReg(0xa6, Pos);
    WriteExtReg(0xa7, (Pos >> 8));


    for (wLoop = 0; wLoop < 5000; wLoop++)
    {
        VGA_DELAY();
    }

    for (wLoop = 0; wLoop < SYSTEM_POINTER_TYPES; wLoop++)
    {
        UCHAR *pPut = pMem;
        UCHAR *pPut1 = pPut + 8;
        uByteCount = 0;

        PegBitmap *pPointerMap = mpPointers[wLoop].Bitmap;

        UCHAR *pGet = pPointerMap->pStart;

        for (uRow = 0; uRow < 32; uRow++)
        {
            AndVal = XorVal = 0;
            uMask = 0x80;
            for (uCol = 0; uCol < 32; uCol++)
            {
                // re-encode the bitmap as 2-bpp
                if (uCol < pPointerMap->wWidth &&
                    uRow < pPointerMap->wHeight)
                {
                    uSrcVal = *pGet++;
                }
                else
                {
                    uSrcVal = TRANSPARENCY;
                }
                switch(uSrcVal)
                {
                case 0x0F:
                    XorVal |= uMask;
                    break;

                case TRANSPARENCY:
                    AndVal |= uMask;
                    break;

                default:
                    break;
                }
                uMask >>= 1;

                if (!uMask)
                {
                    *pPut++ = AndVal;
                    *pPut1++ = XorVal;
                    uMask = 0x80;
                    AndVal = XorVal = 0;
                    uByteCount++;

                    if (uByteCount == 8)
                    {
                        pPut += 8;
                        pPut1 += 8;
                        uByteCount = 0;
                    }
                }
            }
        }
        pMem += 256;        // where to start next cursor
    }

    // Write the cursor 1 color registers:

    uSrcVal = ReadExtReg(0x80);     // read pipeline config 0
    WriteExtReg(0x80, uSrcVal | 0x01);  // set bit 0 to enable alt pal regs

    SetPalette(0, 2, CursorPalette);

    SetPalette(4, 2, CursorPalette);

    WriteExtReg(0x80, uSrcVal | 0x10);     // put bit 0 back for normal palette
                                           // and turn on hardware cursor

    // OK, turn on the hardware cursor
    SetPointerType(0);
    WriteExtReg(0xa0, 0x11);

   #endif
}
 
#ifdef DOUBLE_BUFFER
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CT6924Screen::MemoryToScreen(void)
{
    if (!miInvalidCount)
    {
        return;
    }

    PegRect Copy;
    Copy.Set(0, 0, mwHRes - 1, mwVRes - 1);
    Copy &= mInvalid;

    SIGNED iWidth = Copy.Width();

    if (iWidth <= 0)
    {
        return;
    }

    iWidth *= 3;

    SIGNED iHeight = Copy.Height();
    if (iHeight <= 0)
    {
        return;
    }

    WaitNotBusy();

    // BR00
    DWORD *pReg = (DWORD *) (mpVidMemBase + BITBLT_REGS);
    WORD wAdjHRes = mwHRes * 3;
    *pReg = ((DWORD) wAdjHRes << 16) | wAdjHRes;

    // BR04
    *(pReg + 4) = 0xcc;     // SRCCPY ROP

    // BR07
    DWORD dOffset = Copy.wTop;
    dOffset *= wAdjHRes;
    dOffset += (Copy.wLeft * 3);
    *(pReg + 6) = (dOffset + FRAME_BUFFER_SIZE);
    *(pReg + 7) = dOffset;
    
    // BR08
    dOffset = iHeight;
    dOffset <<= 16;
    dOffset |= iWidth;
    *(pReg + 8) = dOffset;      // start the copy!
}

#endif



