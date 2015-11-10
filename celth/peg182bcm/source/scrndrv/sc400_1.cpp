/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// sc400_1.cpp - PegScreen driver template for 1 bit-per-pixel
//      linear frame buffer access. This driver can be configured
//      to work with any screen resolution, simply by passing in the correct
//      rectangle to the constructor. 
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
// is used ONLY for testing the driver, or for development under WIN32 using
// this driver before the target hardware is ready. These sections of code
// can be removed to improve readability without affecting your target system.
//
// This driver is intended for systems which have direct, linear (i.e. not
// paged) access to the video frame buffer. This driver does not implement
// double buffering, although that could be added.
//
// All available configuration flags are found in the sc400_1.hpp header file.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"

#if defined(CADUL)
#include <builtin.h>

#elif defined(POSIXPEG)
#include <fcntl.h>
//#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
extern   BOOL gbMasterPegProcess;

#if defined(LYNXPEG)
#include <kernel.h>
#include <mem.h>
#include <smem.h>
#endif

#if defined(QNXPEG)
#include "i86.h"
#endif

#else
#define __COLORS        // don't want conio colors
#include <conio.h>
#endif

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
    PegScreen *pScreen = new Sc400MonoScreen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/
Sc400MonoScreen::Sc400MonoScreen(PegRect &Rect) : PegScreen(Rect)
{
    mdNumColors = 2;  

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
}




/*--------------------------------------------------------------------------*/
// *** This function must be filled in by the developer ***
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *Sc400MonoScreen::GetVideoAddress(void)
{
    //DWORD dSize = mwHRes / 8 * mwVRes;

    UCHAR *pMem = (UCHAR *) VID_MEM_BASE;
    return pMem; 
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
Sc400MonoScreen::~Sc400MonoScreen()
{
    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
void Sc400MonoScreen::BeginDraw(PegThing *)
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
void Sc400MonoScreen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
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
void Sc400MonoScreen::EndDraw()
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
void Sc400MonoScreen::EndDraw(PegBitmap *pMap)
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
PegBitmap *Sc400MonoScreen::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
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
void Sc400MonoScreen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
void Sc400MonoScreen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
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
                uVal &= 0x80;
                uVal |= uFill & 0x7F;
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
                uVal &= 0xc0;
                uVal |= uFill & 0x3f;
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
                uVal &= 0xe0;
                uVal |= uFill & 0x1f;
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
                uVal &= 0xf0;
                uVal |= uFill & 0x0f;
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
                uVal &= 0xf8;
                uVal |= uFill & 0x07;
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
                uVal &= 0xfc;
                uVal |= uFill & 0x03;
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
        wYPos++;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Sc400MonoScreen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    UCHAR uVal, uShift, uMask;
    COLORVAL uFill;
    UCHAR *Put;

    while(wWidth--)
    {
        uShift = (wXPos & 7);
        uFill = Color << (7 - uShift);
        uMask = 0x80 >> uShift;
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
void Sc400MonoScreen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
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
void Sc400MonoScreen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    UCHAR uVal = 0x80 >> (wXPos & 7);

    while (wYStart <= wYEnd)
    {
        UCHAR *Put = mpScanPointers[wYStart] + (wXPos >> 3);
        *Put ^= uVal;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void Sc400MonoScreen::Capture(PegCapture *Info, PegRect &CaptureRect)
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

    pMap->uFlags = 0;       // raw format
    pMap->uBitsPix = 1;     // 2 bits per pixel

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
void Sc400MonoScreen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
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
            if (Where.x & 7)
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

                        uVal &= ~(0x80 >> uPix);
                        uVal |= uVal1 << (7 - uPix);
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
// here for an aligned 1 bpp bitmap, no shifting required.
/*--------------------------------------------------------------------------*/
void Sc400MonoScreen::DrawFastBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    // always padded to nearest full byte per scan line:

    UCHAR uVal;
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
        GetStart += wBytesPerLine;
    }
}

/*--------------------------------------------------------------------------*/
// here for a misaligned monochrome bitmap, bit shifting required.
/*--------------------------------------------------------------------------*/
void Sc400MonoScreen::DrawUnalignedBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    UCHAR uVal, uiPix, uoPix, uVal1;
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
        UCHAR *Put = mpScanPointers[wLine] + (View.wLeft >> 3);

        uiPix = (View.wLeft - Where.x) & 7;
        uVal1 <<= uiPix;

        uoPix = View.wLeft & 7;
        uVal = *Put;
        uVal &= 0xff << (8 - uoPix);
        uVal >>= 7 - uoPix;

        while (iCount-- > 0)
        {
            uVal |= uVal1 >> 7;
            uoPix++;

            if (uoPix == 8)
            {
                uoPix =0;
                *Put++ = uVal;
                uVal = 0;
            }
            else
            {
                uVal <<= 1;
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

        if (uoPix)
        {
            uVal1 = *Put;
            uVal1 &= 0xff >> uoPix;
            uVal1 |= uVal << (7 - uoPix);
            *Put = uVal1;
        }
        GetStart += wBytesPerLine;
    }
}




/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Sc400MonoScreen::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
void Sc400MonoScreen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void Sc400MonoScreen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
void Sc400MonoScreen::HidePointer(void)
{
#ifdef PEG_MOUSE_SUPPORT
    if (mbShowPointer)
    {
        PegPresentationManager *pp = PegThing::Presentation();
        Restore(pp, &mCapture, TRUE);
        mCapture.SetValid(FALSE);
    }
#endif
}

/*--------------------------------------------------------------------------*/
void Sc400MonoScreen::SetPointer(PegPoint Where)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (mbShowPointer)
    {
        LOCK_PEG
        HidePointer();
        mLastPointerPos = Where;

	    PegPresentationManager *pp = PegThing::Presentation();
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
void Sc400MonoScreen::SetPointerType(UCHAR bType)
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
void Sc400MonoScreen::ResetPalette(void)
{
    SetPalette(0, 2, DefPalette);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *Sc400MonoScreen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 2;
    return muPalette;
}

void VGA_DELAY(void)
{
}


void WriteVgaReg(int index, int val)
{
    VGA_DELAY();
    POUTB(0x3d4, index);
    VGA_DELAY();
    POUTB(0x3d5, val);
    VGA_DELAY();
}

int ReadVgaReg(int index)
{ 
    VGA_DELAY();
    POUTB(0x3d4, index);
    VGA_DELAY();
    return(PINB(0x3d5));
}

int ReadVidReg(int index)
{
    VGA_DELAY();
    POUTB(0x3ce, index);
    VGA_DELAY();
    return PINB(0x3cf);
}

UCHAR ReadSeqReg(int index)
{
    VGA_DELAY();
    POUTB(0x3c4, index);
    VGA_DELAY();
    return((UCHAR) PINB(0x3c5));
}

void WriteSeqReg(int index, int val)
{
    VGA_DELAY();
    POUTB(0x3c4, index);
    VGA_DELAY();
    POUTB(0x3c5, val);
    VGA_DELAY();
}

void WriteAttribReg(int rg, int val)
{
    PINB(0x3da);
    PINB(0x3ba);
    POUTB(0x3c0, rg);        // kills attrib controller
    VGA_DELAY();
    POUTB(0x3c0, val);
    VGA_DELAY();
    POUTB(0x3c0, rg | 0x20); // restart attrib controller
    VGA_DELAY();
}

void WriteCSCReg(int rg, int val)
{
    POUTB(0x22, rg);
    VGA_DELAY();
    POUTB(0x23, val);
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Sc400MonoScreen::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *pGet)
{
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Sc400MonoScreen::ConfigureController(void)
{
    WriteCSCReg(0xdd, 0x06);    // enable controller, CGA mode, linear
    WriteCSCReg(0xde, 0x00);
    WriteCSCReg(0xe0, 0x00);
    WriteCSCReg(0xe1, 0x00);
    WriteCSCReg(0xe2, 0x00);

    WriteVgaReg(0x0a, 0x00);
    WriteVgaReg(0x0b, 0x00);
    WriteVgaReg(0x0c, 0x00);
    WriteVgaReg(0x0d, 0x00);
    WriteVgaReg(0x0e, 0x00);
    WriteVgaReg(0x0f, 0x00);
    WriteVgaReg(0x30, 44);       // (320 / 8) - 2
    WriteVgaReg(0x31, 39);
    WriteVgaReg(0x32, 42);
    WriteVgaReg(0x33, 39);
    WriteVgaReg(0x34, 0x00);
    WriteVgaReg(0x35, 0x00);
    WriteVgaReg(0x36, 0x10);
    WriteVgaReg(0x37, 0x29);
    WriteVgaReg(0x38, 0x29);
    WriteVgaReg(0x39, 0x01);
    WriteVgaReg(0x3b, 0x00);
    WriteVgaReg(0x3c, 0x00);
    WriteVgaReg(0x3d, 0x00);
    WriteVgaReg(0x3e, 40);
    WriteVgaReg(0x3f, 0);
    WriteVgaReg(0x40, 8);
    WriteVgaReg(0x41, 0x80);
    WriteVgaReg(0x42, 0x00);
    WriteVgaReg(0x43, 0x20);
    WriteVgaReg(0x4c, 0x00);    // slowest possible shift clockw


    WriteVgaReg(0x4D, VID_MEM_BASE >> 16);
    WriteVgaReg(0x4e, VID_MEM_BASE >> 16);
    WriteVgaReg(0x4f, 0x80);
    WriteVgaReg(0x50, 0x92);
    POUTB(0x3d8, 0x0b);
}

#ifdef DOUBLE_BUFFER

#if 0       // Don't compile this example code!!

The following code is provided as an example of how to do double buffering.
You will need to modify this to meet your hardware requirements.

void Sc400MonoScreen::MemoryToScreen(void)
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

