/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 7212mono.cpp - PegScreen driver for 1 bit-per-pixel operation on the
//      Cirrus Logic ARM7 development platform. This screen driver was
//      created by copying the mono screen driver template, and adding in
//      the video controller configuration code. Any other color depth or
//      resolution can easily be created following this procedure.        
//
// Author: Kenneth G. Maxwell & Jim DeLisle
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"
#include "cl7212.h"

UCHAR DefPalette[2*3] = {
0, 0, 0,        // black
255, 255, 255   // white
};

/*--------------------------------------------------------------------------*/



#ifdef PEG_RUNTIME_COLOR_CHECK

// Extra bitmaps for monochrome mode. These bitmaps are only installed when
// run-time color depth determination is used, and the screen driver is
// monochrome. The screen driver must replace the above bitmaps with
// those shown below.


ROMDATA UCHAR ucMonoSysButtonBitmap[] = {
0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,
0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,
0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,
0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,
0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,
0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,
0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,
0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
};


ROMDATA UCHAR ucMonoMixPatternBitmap[] = {
0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00,
0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f,
0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00,
0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f, 0x00, 0x0f,
};

ROMDATA UCHAR ucMonoHandBitmap[] = {
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x00,0x00,0x0f,0x00,0x00,0x0f,0x00,0x00,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x00,0x00,0x0f,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0xff,
0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x0f,0x00,0x00,0x0f,0x00,0x00,0x0f,0x00,0x00,0x0f,0x00,0x00,
0x00,0x0f,0x00,0x00,0xff,0xff,0x00,0x0f,0x0f,0x00,0x0f,0x0f,0x00,0x0f,0x00,0x00,0x0f,0x00,0x00,
0xff,0x00,0x0f,0x00,0x00,0xff,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x0f,0x00,0x00,
0xff,0x00,0x0f,0x0f,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
0xff,0xff,0x00,0x0f,0x0f,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
0xff,0xff,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
0xff,0xff,0xff,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
0xff,0xff,0xff,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0xff,
0xff,0xff,0xff,0xff,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0xff,
0xff,0xff,0xff,0xff,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0xff,
0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
};

extern PegBitmap gbSysButtonBitmap;
extern PegBitmap gbMixPatternBitmap;
extern PegBitmap gbHandBitmap;

COLORVAL MonoColors[16] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1};

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
    PegScreen *pScreen = new CLMonoScreen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
#ifdef PEGWIN32
CLMonoScreen::CLMonoScreen(HWND hWnd, PegRect &Rect) : PegScreen(Rect)
#else
CLMonoScreen::CLMonoScreen(PegRect &Rect) : PegScreen(Rect)
#endif
{
    mdNumColors = 2;  

   #ifdef PEGWIN32
    mhPalette = NULL;
   #endif

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();

    mpScanPointers = new UCHAR PEGFAR *[Rect.Height()];

    UCHAR PEGFAR *CurrentPtr = GetVideoAddress();

    for (SIGNED iLoop = 0; iLoop < Rect.Height(); iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += mwHRes >> 3;
    }

    mLastPointerPos.x = Rect.Width() / 2;
    mLastPointerPos.y = Rect.Height() / 2;
    mbPointerHidden = FALSE;
    mwDrawNesting = 0;

    ConfigureController();        // set up controller registers

    SetPalette(0, 2, DefPalette);

    #ifdef PEG_RUNTIME_COLOR_CHECK

    BLACK = MonoColors[0];
    RED = MonoColors[1];        
    GREEN = MonoColors[2];
    BROWN = MonoColors[3];        
    BLUE = MonoColors[4];        
    MAGENTA = MonoColors[5];
    CYAN = MonoColors[6];
    LIGHTGRAY = MonoColors[7];  
    DARKGRAY = MonoColors[8];   
    LIGHTRED = MonoColors[9];   
    LIGHTGREEN = MonoColors[10]; 
    YELLOW = MonoColors[11];
    LIGHTBLUE = MonoColors[12];  
    LIGHTMAGENTA = MonoColors[13];
    LIGHTCYAN = MonoColors[14];  
    WHITE = MonoColors[15];

    gbSysButtonBitmap.pStart = (UCHAR *) ucMonoSysButtonBitmap;
    gbMixPatternBitmap.pStart = (UCHAR *) ucMonoMixPatternBitmap;
    gbHandBitmap.pStart = (UCHAR *) ucMonoHandBitmap;

    #endif

#ifdef PEGWIN32

   // Some setup stuff for the BitBlitting function:
   mHWnd = hWnd;
   RECT lSize;
   ::GetClientRect(mHWnd, &lSize);

   mwWinRectXOffset = (lSize.right - mwHRes) / 2;
   mwWinRectYOffset = (lSize.bottom -mwVRes) / 2;
#endif

}




/*--------------------------------------------------------------------------*/
// *** This function must be filled in by the developer ***
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *CLMonoScreen::GetVideoAddress(void)
{
    return (UCHAR *) VID_MEM_BASE;
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
CLMonoScreen::~CLMonoScreen()
{
    #ifdef PEGWIN32

    delete [] mpScanPointers[0];

    #endif

    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void CLMonoScreen::BeginDraw(PegThing *)
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
void CLMonoScreen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        WORD wBytesPerLine = (pMap->wWidth + 7) >> 3;

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
void CLMonoScreen::EndDraw()
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

       #else

        #ifdef DOUBLE_BUFFER

         MemoryToScreen();

        #endif

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
void CLMonoScreen::EndDraw(PegBitmap *pMap)
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
PegBitmap *CLMonoScreen::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
{
   PegBitmap *pMap = NULL;

    if (wWidth && wHeight)
    {
        pMap = new PegBitmap;
        pMap->wWidth = wWidth;
        pMap->wHeight = wHeight;
        wWidth += 7;
        wWidth >>= 3;
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
        pMap->uBitsPix = 1;
    }
    return pMap;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLMonoScreen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void CLMonoScreen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR *Put;
    UCHAR uVal;
    UCHAR uFill = 0;

    if (Color)
    {
        uFill = 0xff;
    }
    if (wXEnd < wXStart)
    {
        return;
    }
                        
    while(wWidth-- > 0)
    {
        SIGNED iLen = wXEnd - wXStart + 1;
        Put = mpScanPointers[wYPos] + (wXStart >> 3);

        switch(wXStart & 7)
        {
        case 1:
            if (iLen >= 7)
            {
                uVal = *Put;
                uVal &= 0x01;
                uVal |= uFill & 0xfe;
                *Put++ = uVal;
                iLen -= 7;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXStart + iLen, wYPos, Color);
                }
                wYPos++;
                continue;
            }
            break;

        case 2:
            if (iLen >= 6)
            {
                uVal = *Put;
                uVal &= 0x03;
                uVal |= uFill & 0xfc;
                *Put++ = uVal;
                iLen -= 6;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXStart + iLen, wYPos, Color);
                }
                wYPos++;
                continue;
            }
            break;

        case 3:
            if (iLen >= 5)
            {
                uVal = *Put;
                uVal &= 0x07;
                uVal |= uFill & 0xf8;
                *Put++ = uVal;
                iLen -= 5;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXStart + iLen, wYPos, Color);
                }
                wYPos++;
                continue;
            }
            break;

        case 4:
            if (iLen >= 4)
            {
                uVal = *Put;
                uVal &= 0x0f;
                uVal |= uFill & 0xf0;
                *Put++ = uVal;
                iLen -= 4;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXStart + iLen, wYPos, Color);
                }
                wYPos++;
                continue;
            }
            break;

        case 5:
            if (iLen >= 3)
            {
                uVal = *Put;
                uVal &= 0x1f;
                uVal |= uFill & 0xe0;
                *Put++ = uVal;
                iLen -= 3;
            }
            else
            {
                while(iLen-- > 0)
                {
                    PlotPointView(wXStart + iLen, wYPos, Color);
                }
                wYPos++;
                continue;
            }
            break;

        case 6:
            if (iLen >= 2)
            {
                uVal = *Put;
                uVal &= 0x3f;
                uVal |= uFill & 0xc0;
                *Put++ = uVal;
                iLen -= 2;
            }
            else
            {
                PlotPointView(wXStart, wYPos, Color);
                wYPos++;
                continue;
            }
            break;

        case 7:
            PlotPointView(wXStart, wYPos, Color);
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

        if (iLen > 0)
        {
            memset(Put, uFill, iLen >> 3);
            Put += iLen >> 3;

            switch(wXEnd & 7)
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
        wYPos++;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLMonoScreen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR uVal, uShift, uMask;
    COLORVAL uFill;
    UCHAR *Put;

    while(wWidth--)
    {
        uShift = (wXPos & 7);
        uFill = Color << uShift;
        uMask = 0x01 << uShift;
        Put = mpScanPointers[wYStart] + (wXPos >> 3);
        SIGNED iLen = wYEnd - wYStart;

        while(iLen-- >= 0)
        {
            uVal = *Put;
            uVal &= ~uMask;
            uVal |= uFill;
            *Put = uVal;
            Put += mwHRes >> 3;
        }
        wXPos++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLMonoScreen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    wXStart += 7;
    wXStart &= 0xfffc;
    UCHAR *Put = mpScanPointers[wYPos] + (wXStart >> 3);
    SIGNED iLen = wXEnd - wXStart;

    if (!iLen)
    {
        return;
    }
    while(wXStart < wXEnd - 7)
    {
        *Put++ ^= 0x55;
        wXStart += 8;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLMonoScreen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    UCHAR uVal = 0x01 << (wXPos & 7);

    while (wYStart <= wYEnd)
    {
        UCHAR *Put = mpScanPointers[wYStart] + (wXPos >> 3);
        *Put ^= uVal;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void CLMonoScreen::Capture(PegCapture *Info, PegRect &CaptureRect)
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

    while (CaptureRect.wLeft & 7)
    {
        CaptureRect.wLeft--;
    }

    while ((CaptureRect.wRight & 7) != 7)
    {
        CaptureRect.wRight++;
    }

    Info->SetPos(CaptureRect);
    LONG Size = (pMap->wWidth * pMap->wHeight) >> 3;
    Info->Realloc(Size);

    UCHAR *GetStart = mpScanPointers[CaptureRect.wTop] + (CaptureRect.wLeft >> 3);

    // make room for the memory bitmap:

    pMap->uFlags = BMF_NATIVE;  // native format
    pMap->uBitsPix = 1;         // 1 bits per pixel

    // fill in the image with our captured info:

    UCHAR *Put = pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        memcpy(Put, GetStart, pMap->wWidth >> 3);
        GetStart += mwHRes >> 3;
        Put += pMap->wWidth >> 3;
    }
    Info->SetValid(TRUE);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CLMonoScreen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
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
            if (Getmap->uFlags & BMF_NATIVE)
            {
                //if ((View.wLeft & 7) == (Where.x & 7))
                //{
                //    DrawFastNativeBitmap(Where, Getmap, View);
                //}
                //else
                //{
                    DrawSlowNativeBitmap(Where, Getmap, View);
                //}
            }
            else
            {
                Draw2ColorBitmap(Where, Getmap, View);
            }
            return;
        }
        else
        {
            // here for a source bitmap of 8-bpp:

            SIGNED iWidth = View.Width();
            Get = Getmap->pStart;
            Get += (View.wTop - Where.y) * Getmap->wWidth;
            Get += View.wLeft - Where.x;

            for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
            {
                Put = mpScanPointers[wLine] + (View.wLeft >> 3);
                uVal = *Put;
                uPix = View.wLeft & 7;

                for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
                {
                    uVal1 = *Get++;

                    if (uVal1 != 0xff)
                    {
                        #ifdef PEG_RUNTIME_COLOR_CHECK
                        uVal1 = (UCHAR) MonoColors[uVal1 & 0x0f];
                        #endif

                        uVal &= ~(0x01 << uPix);
                        uVal |= (uVal1 << uPix);
                    }
                    
                    if (uPix == 7)
                    {
                        *Put++ = uVal;
                        uVal = *Put;
                        uPix = 0;
                    }
                    else
                    {
                        uPix++;
                    }
                }

                if (uPix)
                {
                    *Put = uVal;
                }
                
                Get += Getmap->wWidth - iWidth;
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
// here for an aligned 1 bpp bitmap, no shifting required in native
// bit-swapped format.
/*--------------------------------------------------------------------------*/
void CLMonoScreen::DrawFastNativeBitmap(const PegPoint Where, 
                                        const PegBitmap *Getmap,
                                        const PegRect &View)
{
    UCHAR uVal;
    UCHAR uVal1;
    UCHAR uMask;
    WORD wBytesPerLine = (Getmap->wWidth + 7) >> 3;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 3;

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        UCHAR *Get = GetStart;
        UCHAR *Put = mpScanPointers[wLine] + (View.wLeft >> 3);
        SIGNED iCount = View.Width();
        

        switch(View.wLeft & 7)
        {
        case 0:
            break;

        case 1:
            if (iCount >= 7)
            {
                uVal = *Put;
                uVal &= 0x01;
                uVal |= *Get++ & 0xfe;
                *Put++ = uVal;
                iCount -= 7;
            }
            else
            {
                uVal1 = *Get;
                uVal = *Put;
                uMask = 0x02;
                while(iCount--)
                {
                    uVal &= ~uMask;
                    uVal |= uVal1 & uMask;
                    uMask <<= 1;
                }
                *Put = uVal;
            }
            break;

        case 2:
            if (iCount >= 6)
            {
                uVal = *Put;
                uVal &= 0x03;
                uVal |= *Get++ & 0xfc;
                *Put++ = uVal;
                iCount -= 6;
            }
            else
            {
                uVal1 = *Get;
                uVal = *Put;
                uMask = 0x04;

                while(iCount--)
                {
                    uVal &= ~uMask;
                    uVal |= uVal1 & uMask;
                    uMask <<= 1;
                }
                *Put = uVal;
            }
            break;

        case 3:
            if (iCount >= 5)
            {
                uVal = *Put;
                uVal &= 0x07;
                uVal |= *Get++ & 0xf8;
                *Put++ = uVal;
                iCount -= 5;
            }
            else
            {
                uVal1 = *Get;
                uVal = *Put;
                uMask = 0x08;
                while(iCount--)
                {
                    uVal &= ~uMask;
                    uVal |= uVal1 & uMask;
                    uMask <<= 1;
                }
                *Put = uVal;
            }            
            break;

        case 4:
            if (iCount >= 4)
            {
                uVal = *Put;
                uVal &= 0x0f;
                uVal |= *Get++ & 0xf0;
                *Put++ = uVal;
                iCount -= 4;
            }
            else
            {
                uVal1 = *Get;
                uVal = *Put;
                uMask = 0x10;
                while(iCount--)
                {
                    uVal &= ~uMask;
                    uVal |= uVal1 & uMask;
                    uMask <<= 1;
                }
                *Put = uVal;                
            }
            break;

        case 5:
            if (iCount >= 3)
            {
                uVal = *Put;
                uVal &= 0x1f;
                uVal |= *Get++ & 0xe0;
                *Put++ = uVal;
                iCount -= 3;
            }
            else
            {
                uVal1 = *Get;
                uVal = *Put;
                uMask = 0x20;
                while (iCount--)
                {
                    uVal &= ~uMask;
                    uVal |= uVal1 & uMask;
                    uMask <<= 1;
                }
                *Put = uVal;
            }
            break;

        case 6:
            if (iCount >= 2)
            {
                uVal = *Put;
                uVal &= 0x3f;
                uVal |= *Get++ & 0xc0;
                *Put++ = uVal;
                iCount -= 2;
            }
            else
            {
                uVal = *Get;
                PlotPointView(View.wLeft, wLine, uVal >> 6);
                PlotPointView(View.wLeft + 1, wLine, uVal >> 7);
                iCount = 0;
            }
            break;

        case 7:
            uVal = *Put;
            uVal &= 0x7f;
            uVal |= *Get++ & 0x80;
            *Put++ = uVal;
            iCount--;
            break;
        }

        if (iCount > 0)
        {       
            // copy 8 pixels at a time:
            memcpy(Put, Get, iCount >> 3);
            Put += iCount >> 3;
            Get += iCount >> 3;

            switch (View.wRight & 7)
            {
            case 7:
                break;

            case 6:
                uVal = *Put;
                uVal &= 0x80;
                uVal |= *Get & 0x7f;
                *Put = uVal;
                break;

            case 5:
                uVal = *Put;
                uVal &= 0xc0;
                uVal |= *Get & 0x3f;
                *Put = uVal;
                break;

            case 4:
                uVal = *Put;
                uVal &= 0xe0;
                uVal |= *Get & 0x1f;
                *Put = uVal;
                break;

            case 3:
                uVal = *Put;
                uVal &= 0xf0;
                uVal |= *Get & 0x0f;
                *Put = uVal;
                break;

            case 2:
                uVal = *Put;
                uVal &= 0xf8;
                uVal |= *Get & 0x07;
                *Put = uVal;
                break;

            case 1:
                uVal = *Put;
                uVal &= 0xfc;
                uVal |= *Get & 0x03;
                *Put = uVal;
                break;

            case 0:
                uVal = *Put;
                uVal &= 0xfe;
                uVal |= *Get & 0x01;
                *Put = uVal;
                break;
            }
        }
        GetStart += wBytesPerLine;
    }
}

/*--------------------------------------------------------------------------*/
// here for a misaligned monochrome bitmap, bit shifting required in native
// bit-swapped format.
/*--------------------------------------------------------------------------*/
void CLMonoScreen::DrawSlowNativeBitmap(const PegPoint Where, 
                                        const PegBitmap *Getmap,
                                        const PegRect &View)
{
    WORD wBytesPerLine = (Getmap->wWidth + 7) >> 3;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 3;

    UCHAR uInPix = ((View.wLeft - Where.x) & 7);
    UCHAR uOutPix = (View.wLeft & 7);

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        UCHAR *Get = GetStart;
        UCHAR *Put = mpScanPointers[wLine] + (View.wLeft >> 3);
        SIGNED iCount = View.Width();

        UCHAR uOutVal = *Put;   // read dest value
        uOutVal <<= 8 - uOutPix;    // pre-shift the output value
        UCHAR uOutCount = uOutPix;
        UCHAR uInCount = uInPix;

        UCHAR uInVal = *Get++;
        uInVal >>= uInPix;

        while(iCount-- > 0)
        {
            uOutVal >>= 1;
            uOutVal |= uInVal << 7;

            uOutCount++;

            if (uOutCount == 8)
            {
                *Put++ = uOutVal;
                uOutVal = *Put;
                uOutCount = 0;
            }

            uInCount++;
            if (uInCount == 8)
            {
                uInVal = *Get++;
                uInCount = 0;
            }
            else
            {
                uInVal >>= 1;
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
// Here to draw 2 color, non-native (big endian), non-RLE formatted bitmap
/*--------------------------------------------------------------------------*/
void CLMonoScreen::Draw2ColorBitmap(const PegPoint Where,
                                    const PegBitmap *Getmap,
                                    const PegRect& View)
{
    WORD wBytesPerLine = (Getmap->wWidth + 7) >> 3;
    UCHAR *GetStart = Getmap->pStart;
    GetStart += (View.wTop - Where.y) * wBytesPerLine;
    GetStart += (View.wLeft - Where.x) >> 3;

    UCHAR uInPix = ((View.wLeft - Where.x) & 7);
    UCHAR uOutPix = (View.wLeft & 7);

    for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
    {
        UCHAR *Get = GetStart;
        UCHAR *Put = mpScanPointers[wLine] + (View.wLeft >> 3);
        SIGNED iCount = View.Width();

        UCHAR uOutVal = *Put;   // read dest value
        uOutVal <<= 8 - uOutPix;    // pre-shift the output value
        UCHAR uOutCount = uOutPix;
        UCHAR uInCount = uInPix;

        UCHAR uInVal = *Get++;
        uInVal <<= uInPix;

        while(iCount-- > 0)
        {
            uOutVal >>= 1;
            uOutVal |= uInVal & 0x80;

            uOutCount++;

            if (uOutCount == 8)
            {
                *Put++ = uOutVal;
                uOutCount = 0;
                uOutVal = 0;
            }

            uInCount++;
            if (uInCount == 8)
            {
                uInVal = *Get++;
                uInCount = 0;
            }
            else
            {
                uInVal <<= 1;
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
void CLMonoScreen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
void CLMonoScreen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void CLMonoScreen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
void CLMonoScreen::HidePointer(void)
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
void CLMonoScreen::SetPointer(PegPoint Where)
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
void CLMonoScreen::SetPointerType(UCHAR bType)
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
void CLMonoScreen::ResetPalette(void)
{
    SetPalette(0, 2, DefPalette);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *CLMonoScreen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 2;
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
void CLMonoScreen::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *pGet)
{
/*   #ifdef PEGWIN32

    BMhead.bmhead.biSize = sizeof(BITMAPINFOHEADER);
    BMhead.bmhead.biWidth = mwHRes;
    BMhead.bmhead.biHeight = -mwVRes;
    BMhead.bmhead.biPlanes = 1;
    BMhead.bmhead.biBitCount = 1;
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
      ** Error- the SetPalette function must be filled in **
   #endif
*/
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
void CLMonoScreen::ConfigureController(void)
{
   #ifdef PEGWIN32
   #else
    DWORD *pReg;
    unsigned long ulLcdConfig, ulLineLength, ulVideoBufferSize;

    //
    // Determine the value to be programmed into the LCD controller in the
    // CL-PS7111 to properly drive the ALPS LCD panel at 640x240, 1 bit per
    // pixel, at 80Hz.

    ulVideoBufferSize = ((mwHRes * mwVRes) / 128) - 1;
    ulLineLength = (mwHRes / 16) - 1;

    ulLcdConfig = LCD_AC_PRESCALE      |
                  LCD_PIX_PRESCALE     |
                  (ulLineLength << 13) |
                  ulVideoBufferSize;
                  
    //
    // Configure the palette to be a one-to-one mapping of the pixel values to
    // the available LCD pixel intensities.
    // Since we're in monochrome, we're only using the 2 ls nibbles
    // of the lsw
    
    pReg = (DWORD *) LCD_PALETTE_LSW;                                                        
    *pReg = 0x0000000f;
    pReg = (DWORD *) LCD_PALETTE_MSW;                                                        
    *pReg = 0;

    pReg = (DWORD *) LCD_CONTROL;
    *pReg = ulLcdConfig;

    // clear the video frame buffer:

    UCHAR *pPut = (UCHAR *) VID_MEM_BASE;
    WORD wLoop;

    for(wLoop = 0; wLoop < ((mwHRes * mwVRes) / 8); wLoop++)
    {
        *pPut++ = 0;
    }


    LCDOn();
    // LCDBacklightOn();
    LCDBacklightOff();

 #endif
}

#ifdef PEGWIN32

/*--------------------------------------------------------------------------*/
// function to blast our memory out to the windows screen. This is only used
// for testing purposes, and can be deleted.

void CLMonoScreen::MemoryToScreen(void)
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
        mpScanPointers[0], (const struct tagBITMAPINFO *) &BMhead,
        DIB_RGB_COLORS, SRCCOPY);

    SelectObject(mHdc, hOldPal);
}

#else

#ifdef DOUBLE_BUFFER

#if 0       // Don't compile this example code!!

The following code is provided as an example of how to do double buffering.
You will need to modify this to meet your hardware requirements.

void CLMonoScreen::MemoryToScreen(void)
{


    // If there is no invalid area, there is nothing to copy:

    if (!miInvalidCount)
    {
        return;
    }

    // Initialize 'area to copy' to the whole screen:

    PegRect Copy;
    Copy.wTop = 0;
    Copy.wLeft = 0;
    Copy.wRight = mwHRes - 1;
    Copy.wBottom = mwVRes - 1;

    // now reduce it to the area that has been modified:

    Copy &= mInvalid;

    // Since we are using 1-bpp, we need to make the left edge start
    // on pixel modulo 0, and the right edge end on pixel modulo 7, so
    // that we are copying full bytes:

    Copy.wLeft &= 0xfff8;       // even align left edge
    Copy.wRight |= 7;           // end on a full byte

    WORD wBytesPerLine = Copy.Width() / 8;
    WORD wPitch = mwHRes >> 3;

    UCHAR *pSource = mpScanPointers[Copy.wTop] + (Copy.wLeft >> 3);
    UCHAR *pDest = OUTPUT_FRAME_BUFFER + 
                   (Copy.wTop * wPitch) +
                   (Copy.wLeft >> 3);

    // Now go into a loop, copying bytes from the source (local memory)
    // to the destination (video frame buffer).

    for (WORD wLoop = Copy.wTop; wLoop <= Copy.wBottom; wLoop++)
    {
        // Copy wBytesPerLine from source to target. This may take
        // some work depending on the setup of your video controller.

        // memcpy(pDest, pSource, wBytesPerLine);
        pSource += wPitch;
        pDest += wPitch;

    }

}

#endif      // the if 0 if
#endif      // the DOUBLE_BUFFER if
#endif      // the WIN32 if

