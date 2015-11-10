/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 8106scrn.cpp - Instantiable screen class written for SED8106
//   controller running with 320x240 LCD display.
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
// This is the most difficult of the standard VGA modes, due to the strange
// way the video controller works when in this mode. It is therefore also one
// of the slowest relative to the number of pixels being redrawn. It is
// provided for completness, and could be easily modified to support other
// resolutions.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


#include "stdlib.h"
#include "string.h"
#include "peg.hpp"


#ifdef CADUL
#include <builtin.h>
#else

#define __COLORS        // don't want conio colors
#include <conio.h>

#ifdef PEGQNX
#include "fcntl.h"
#include "mman.h"
#include "unistd.h"
#include "i86.h"
#endif
#endif

#ifdef __BORLANDC__
 #pragma warn -aus   /* turn off warning "identifier is assigned a value that is never used" */
                     /* (this is done to avoid warnings for dummy var) */
#endif

#if defined(PEGSMX) && defined(__BORLANDC__) && defined(FM)
 #pragma inline   /* eliminates warning "Restarting compile using assembly..." */
#endif

#if defined(PEGTNT)
#include "pharlap.h"
#endif

/*----------------------------------------------------------------------------
 This is a dummy variable necessary for proper operation of the
 VGA controller. Reading from the video buffer causes an internal
 register to be loaded. The result of the read is not used, so some
 compilers want to 'optimize out' this seemingly needless instruction.
 Making it a global variable rather than a local variable prevents the
 compiler from discarding these statements. Note that since the value
 stored in this variable does not matter, making it global has no
 adverse effect on re-entrancy.
----------------------------------------------------------------------------*/

UCHAR dummy; 


// These are the values actually programmed into the VGA DAC registers.
// These are 6-bit values, biased to brighten the normal VGA colors slightly.

#ifndef GRAYSCALE

UCHAR DosPal[16*3] = {
    0, 0, 0,     // black
    44, 0, 0,    // red
    0, 44, 0,    // green
    44, 44, 0,   // brown
    0, 0, 44,    // blue
    44, 0, 44,   // magenta
    0, 44, 44,   // cyan
    50, 50, 50,  // lightgray
    32, 32, 32,  // darkgray
    63, 0, 0,    // lightred
    0, 63, 0,    // lightgreen
    63, 63, 0,   // yellow
    0, 0, 63,    // lightblue
    63, 0, 63,   // lightmagenta
    0, 63, 63,   // lightcyan
    63, 63, 63   // white
};

#else

UCHAR DosPal[16*3] = {
    0, 0, 0,     // black
    4, 4, 4,     // red
    8, 8, 8,     // green
    12, 12, 12,     // brown
    16, 16, 16,     // blue
    20, 20, 20,  // magenta
    24, 24, 24,  // cyan
    48, 48, 48,  // lightgray
    18, 18, 18,  // darkgray
    28, 28, 28,    // lightred
    32, 32, 32,    // lightgreen
    36, 36, 36,   // yellow
    44, 44, 44,    // lightblue
    48, 48, 48,   // lightmagenta
    52, 52, 52,   // lightcyan
    63, 63, 63   // white
};

#endif


// These are the 8-bpp RGB colors we will tell the rest of the world
// we are running with:

UCHAR DefPalette[16*3] = {
0, 0, 0,        // black
191, 0, 0,      // red
0, 191, 0,      // green
191, 191, 0,    // brown
0, 0, 191,      // blue
191, 0, 191,    // magenta
0, 191, 191,    // cyan
192, 192, 192,  // lightgray
128, 128, 128,  // darkgray
255, 0, 0,      // lightred
0, 255, 0,      // lightgreen
255, 255, 0,    // yellow
0, 0, 255,      // lightblue
255, 0, 255,    // lightmagenta
0, 255, 255,    // lightcyan
255, 255, 255   // white
};


/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new Peg8106Screen(Rect);
    return pScreen;
}


/*------------------------ Constructor -------------------------------------*/
//
//
/*--------------------------------------------------------------------------*/

Peg8106Screen::Peg8106Screen(PegRect &Rect) : PegScreen(Rect)
{
    mdNumColors = 16;
    mwBytesPerRow = 80;

  #if defined(PEGSMX)

   #if defined(_32BIT) && defined(_FLAT)
    mpVidMem = (UCHAR *) VID_MEM_BASE;
   #elif defined(PME16)
    word graphVidSel;
    if(pmiAllocLD(1, &graphVidSel))
    {
        mwHRes = 0;     // indicate failure by returning
        mwVRes = 0;     // null screen resolution
        return;
    }
    pmiSetBaseAddr(graphVidSel, VID_MEM_BASE);
    pmiSetLimit(graphVidSel, VID_MEM_SIZE - 1);
    pmiSetRights(graphVidSel, 0x92, 0);
    mpVidMem = (UCHAR PEGFAR *) MK_FP(graphVidSel, 0);

   #elif defined(REALMODE)
    mpVidMem = (UCHAR PEGFAR *) (VID_MEM_BASE << 12);

   #else
    #error Define VID_MEM_BASE for smx target environment (vgascrn.hpp)
   #endif

  #elif defined(PHARLAP)

    // PHARLAP always runs in 32-bit protected mode.

    #ifndef PEGTNT      // running with ETS?
     mpVidMem = (UCHAR *) VID_MEM_BASE;
    #else               // running with TNT?
     mpVidMem = (UCHAR *) VirtualAlloc(NULL, VID_MEM_SIZE, MEM_RESERVE,
        PAGE_NOACCESS);
     _dx_map_pgsn((LPVOID) mpVidMem, VID_MEM_SIZE, VID_MEM_BASE);
    #endif


  #elif defined(CADUL) || defined(__HIGHC__)

    // CAD-UL and HIGHC are also generally used for protected mode
    mpVidMem = (UCHAR *) VID_MEM_BASE;

  #elif defined(PEGQNX)     // using Watcom/QNX

    int fd = shm_open("Physical", O_RDWR, 0777);
    int myerror = errno;
    mpVidMem = (UCHAR PEGFAR *) mmap(0, 64*1024,
		PROT_READ | PROT_WRITE |PROT_NOCACHE, MAP_SHARED, fd, VID_MEM_BASE);

  #else

    // default to REAL MODE addressing:
    mpVidMem = (UCHAR PEGFAR *) (VID_MEM_BASE << 12);

  #endif

   #if defined(__WATCOMC__) && !defined(PEGQNX) // Watcom real mode compiler bug fix

    mpScanPointers = new UCHAR *[Rect.Height()];
   #else
    mpScanPointers = new UCHAR PEGFAR *[Rect.Height()];
   #endif

    UCHAR PEGFAR *CurrentPtr = mpVidMem;

    for (SIGNED iLoop = 0; iLoop < Rect.Height(); iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += mwBytesPerRow;
    }

    mLastPointerPos.x = Rect.Width() / 2;
    mLastPointerPos.y = Rect.Height() / 2;
    mbPointerHidden = FALSE;
    mwDrawNesting = 0;

    ConfigureController();
}

/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
Peg8106Screen::~Peg8106Screen()
{
    delete [] mpScanPointers;

    // when running without a BIOS, there is no way to easily configure
    // the controller for text mode, since the text mode font is usually
    // stored in the BIOS ROM.

   #if defined(RESTORE_TEXT_ON_EXIT)
    ConfigureMode3();
   #endif
}

/*--------------------------------------------------------------------------*/
void Peg8106Screen::BeginDraw(PegThing *)
{
    LOCK_PEG
   #ifdef PEG_MOUSE_SUPPORT
    if (!mwDrawNesting)
    {
        if (miInvalidCount)
        {
            if (mInvalid.Overlap(mCapture.Pos()))
            {
                HidePointer();
                mbPointerHidden = TRUE;
            }
        }
    }
   #endif

    mwDrawNesting++;
}

/*--------------------------------------------------------------------------*/
void Peg8106Screen::BeginDraw(PegThing *, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG
    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        #ifdef __WATCOMC__
         mpScanPointers = (UCHAR PEGFAR **) new UCHAR *[pMap->wHeight];
        #else
         mpScanPointers = new UCHAR PEGFAR *[pMap->wHeight];
        #endif
        UCHAR PEGFAR *CurrentPtr = pMap->pStart;
        for (WORD wLoop = 0; wLoop < pMap->wHeight; wLoop++)
        {
            mpScanPointers[wLoop] = CurrentPtr;
            CurrentPtr += pMap->wWidth;
        }
        mVirtualRect.Set(0, 0, pMap->wWidth - 1, pMap->wHeight - 1);
        mbVirtualDraw = TRUE;
    }
}

/*--------------------------------------------------------------------------*/
void Peg8106Screen::EndDraw(void)
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
void Peg8106Screen::EndDraw(PegBitmap *)
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
void Peg8106Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
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
	                PlotPointView(wCurx, wpy, (UCHAR) Color.uForeground);
	            }
	
	            for (wpy = wNexty - wWidth / 2;
	                 wpy <= wNexty + wWidth / 2; wpy++)
	            {
	                PlotPointView(wNextx, wpy, (UCHAR) Color.uForeground);
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
	                PlotPointView(wpx, wCury, (UCHAR) Color.uForeground);
	            }
	
	            for (wpx = wNextx - wWidth / 2;
	                 wpx <= wNextx + wWidth / 2; wpx++)
	            {
	                PlotPointView(wpx, wNexty, (UCHAR) Color.uForeground);
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
	                    PlotPointView(wCurx, wpy, (UCHAR) Color.uForeground);
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
	                    PlotPointView(wNextx, wpy, (UCHAR) Color.uForeground);
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
	                    PlotPointView(wpx, wCury, (UCHAR) Color.uForeground);
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
	                    PlotPointView(wpx, wNexty, (UCHAR) Color.uForeground);
	                }
	            }
	        }
	    }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Peg8106Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    if (mbVirtualDraw)
    {
        LinearHLine(wXStart, wXEnd, wYPos, Color, wWidth);
        return;
    }

    UCHAR uMask;
    UCHAR PEGFAR *PutStart = mpScanPointers[wYPos] + (wXStart / 8);
    UCHAR PEGFAR *Address = PutStart;

    WriteVidReg(5, 2);            // write mode 2

    while (wWidth-- > 0)
    {
        SIGNED wLength = wXEnd - wXStart + 1;

        if (wXStart & 0x07)            // not a byte aligned Start?
        {
            SIGNED cPixThisByte = 8 - (wXStart & 7);

            if (cPixThisByte >= wLength)
            {
                cPixThisByte = wLength;
            }
            uMask = 0xff >> (8 - cPixThisByte);
            uMask <<= 8 - cPixThisByte - (wXStart & 7);

            WriteVidReg(8, uMask);
            dummy = *Address;        // load internal register
            *Address++ = (UCHAR) Color;        // mask in partial byte
            wLength -= cPixThisByte;
        }

        WriteVidReg(8, 0xff);
    
        while (wLength >= 8)
        {
            *Address++ = (UCHAR) Color;
            wLength -= 8;
        }

        if (wLength)
        {
            // finish the last few pixels:
            uMask = 0xff << (8 - wLength);
            WriteVidReg(8, uMask);
            dummy = *Address;        // load internal register
            *Address++ = (UCHAR) Color;        // mask in partial byte
        }

        if (wWidth)
        {
            PutStart += mwBytesPerRow;
            Address = PutStart;
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Peg8106Screen::LinearHLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
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
void Peg8106Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    if (mbVirtualDraw)
    {
        LinearVLine(wYStart, wYEnd, wXPos, Color, wWidth);
        return;
    }
    UCHAR uMask;
    WriteVidReg(5, 2);            // write mode 2
    SIGNED wLength = wYEnd - wYStart + 1;

    while (wWidth > 0)
    {
        UCHAR PEGFAR *Put = mpScanPointers[wYStart] + (wXPos / 8);

        SIGNED cPixThisByte = 8 - (wXPos & 7);

        if (cPixThisByte >= wWidth)
        {
            cPixThisByte = wWidth;
        }
        uMask = 0xff >> (8 - cPixThisByte);
        uMask <<= 8 - cPixThisByte - (wXPos & 7);

        WriteVidReg(8, uMask);

        for (SIGNED wLoop = 0; wLoop < wLength; wLoop++)
        {
            dummy = *Put;        // load internal register
            *Put = (UCHAR) Color;        // mask in partial byte
            Put += mwBytesPerRow;
        }
        wXPos += cPixThisByte;
        wWidth -= cPixThisByte;
    }
    WriteVidReg(8, 0xff);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Peg8106Screen::LinearVLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
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
void Peg8106Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    UCHAR Src;

    while (wXStart <= wXEnd)
    {
        Src = (UCHAR) GetPixelView(wXStart, wYPos);
         Src ^= 7;
        PlotPointView(wXStart, wYPos, Src);
        wXStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Peg8106Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    UCHAR Src;

    while (wYStart <= wYEnd)
    {
        Src = (UCHAR) GetPixelView(wXPos, wYStart);
        Src ^= 7;
        PlotPointView(wXPos, wYStart, Src);
        wYStart += 2;
    }
}


/*--------------------------------------------------------------------------*/
void Peg8106Screen::HidePointer(void)
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
void Peg8106Screen::SetPointer(PegPoint Where)
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
void Peg8106Screen::SetPointerType(UCHAR bType)
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
void Peg8106Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
{
    if (CaptureRect.wLeft < 0)
    {
        CaptureRect.wLeft = 0;
    }

    if (CaptureRect.wTop < 0)
    {
        CaptureRect.wTop = 0;        
    }
    Info->SetPos(CaptureRect);
    PegBitmap *pMap = Info->Bitmap();
    Info->Realloc(pMap->wWidth * pMap->wHeight);

    // make sure evenly aligned:

    SIGNED iGetWidth = pMap->wWidth;
    SIGNED iStartX = CaptureRect.wLeft & 0xfff8;
    SIGNED iSkipBits = CaptureRect.wLeft - iStartX;
    iGetWidth += iSkipBits;
    iGetWidth += 8 - (iGetWidth & 7);
    SIGNED iByteWidth = iGetWidth >> 3;

    // get some memory for each bit plane:

    WORD  wSize = (WORD) (iByteWidth * pMap->wHeight);
    UCHAR *RedPlane = new UCHAR[wSize * 4];
    UCHAR *GreenPlane = RedPlane + wSize;
    UCHAR *BluePlane = GreenPlane + wSize;
    UCHAR *IntensePlane = BluePlane + wSize;

    UCHAR PEGFAR *GetStart = mpScanPointers[CaptureRect.wTop] + (iStartX >> 3);

    WriteVidReg(5, 0);            // select read mode 0

    // read the red bits:

    WriteVidReg(04, 0);
    UCHAR PEGFAR *Get = GetStart;
    SIGNED index = 0;
    WORD wLine;
    SIGNED wLoop;

    for (wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        UCHAR PEGFAR *GetRed = Get;
        for (wLoop = 0; wLoop < iByteWidth; wLoop++)
        {
            RedPlane[index++] = *GetRed++;
        }
        Get += mwBytesPerRow;
    }

    // read the green bits:

    WriteVidReg(04, 1);
    Get = GetStart;
    index = 0;

    for (wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        UCHAR PEGFAR *GetGreen = Get;
        for (wLoop = 0; wLoop < iByteWidth; wLoop++)
        {
            GreenPlane[index++] = *GetGreen++;
        }
        Get += mwBytesPerRow;
    }

    // read the blue bits:

    WriteVidReg(04, 2);
    Get = GetStart;
    index = 0;

    for (wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        UCHAR PEGFAR *GetBlue = Get;
        for (wLoop = 0; wLoop < iByteWidth; wLoop++)
        {
            BluePlane[index++] = *GetBlue++;
        }
        Get += mwBytesPerRow;
    }

    // read the intensity bits:

    WriteVidReg(04, 3);
    Get = GetStart;
    index = 0;

    for (wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        UCHAR PEGFAR *GetIntense = Get;
        for (wLoop = 0; wLoop < iByteWidth; wLoop++)
        {
            IntensePlane[index++] = *GetIntense++;
        }
        Get += mwBytesPerRow;
    }

    // make room for the memory bitmap:

    pMap->uFlags = 0;       // raw format
    pMap->uBitsPix = 8;

    // fill in the image with our captured info:

    index = 0;
    int Mask = 0x80;
    UCHAR *Put = pMap->pStart;

    for (wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        for (wLoop = 0; wLoop < iGetWidth; wLoop++)
        {

            if (wLoop >= iSkipBits &&
                wLoop < (SIGNED) pMap->wWidth + iSkipBits)
            {
                UCHAR uVal = 0;
                if (RedPlane[index] & Mask)
                {
                    uVal |= 1;
                }
                if (BluePlane[index] & Mask)
                {
                    uVal |= 4;
                }
                if (GreenPlane[index] & Mask)
                {
                    uVal |= 2;
                }
                if (IntensePlane[index] & Mask)
                {
                    uVal |= 8;
                }
                *Put++ = uVal;
            }
            Mask >>= 1;
            if (!Mask)
            {
                index++;
                Mask = 0x80;
            }
        }
    }

    delete [] RedPlane;
    Info->SetValid(TRUE);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Peg8106Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    if (mbVirtualDraw)
    {
        LinearBitmap(Where, Getmap, View);
        return;
    }
    WriteVidReg(5, 2);        // write mode 2

    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
    }
    else
    {
        UCHAR PEGFAR *Get = Getmap->pStart;
        Get += (View.wTop - Where.y) * Getmap->wWidth;
        Get += View.wLeft - Where.x;

        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            UCHAR uMask = 0x80 >> (View.wLeft % 8);
            UCHAR PEGFAR *Put = mpScanPointers[wLine] + (View.wLeft >> 3);

            for (SIGNED wPix = View.wLeft; wPix <= View.wRight; wPix++)
            {
                UCHAR uVal = *Get++;
    
                if (uVal != 0xff)
                {
                    WriteVidReg(8, uMask);
                    dummy = *Put;
                    *Put = uVal;
                }
                uMask >>= 1;

                if (!uMask)
                {
                    uMask = 0x80;
                    Put++;
                }
            }
            Get += Getmap->wWidth - View.Width();
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Peg8106Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
    const PegBitmap *Getmap)
{
    UCHAR PEGFAR *Get = Getmap->pStart;
    UCHAR uVal;
    SIGNED uCount;

    SIGNED wLine = Where.y;

    uCount = 0;

    while (wLine < View.wTop)
    {
        uCount = 0;

        while(uCount < (SIGNED) Getmap->wWidth)
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
        UCHAR PEGFAR *Put = mpScanPointers[wLine] + (wLoop1 >> 3);
        UCHAR uMask = 0x80 >> (wLoop1 % 8);

        while (wLoop1 < Where.x + (SIGNED) Getmap->wWidth)
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
                        WriteVidReg(8, uMask);
                        dummy = *Put;
                        *Put = uVal;
                    }
                    wLoop1++;
                    uMask >>= 1;
                    if (!uMask)
                    {
                        uMask = 0x80;
                        Put++;
                    }
                }
            }
            else
            {
                uCount = uVal + 1;
                uVal = *Get++;

                if (uVal == 0xff)
                {
                    wLoop1 += uCount;
                    uMask = 0x80 >> (wLoop1 % 8);
                    Put += wLoop1 / 8;
                }
                else
                {
                    while(uCount--)
                    {
                        if (wLoop1 >= View.wLeft &&
                            wLoop1 <= View.wRight)
                        {
                            WriteVidReg(8, uMask);
                            dummy = *Put;
                            *Put = uVal;
                        }
                        wLoop1++;
                        uMask >>= 1;
                        if (!uMask)
                        {
                            uMask = 0x80;
                            Put++;
                        }
                    }
                }
            }
        }
        wLine++;
    }
}               


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Peg8106Screen::LinearBitmap(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    if (IS_RLE(Getmap))
    {
        LinearRleBitmap(Where, View, Getmap);
    }
    else
    {
        UCHAR *Get = Getmap->pStart;
        Get += (View.wTop - Where.y) * Getmap->wWidth;
        Get += View.wLeft - Where.x;

        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            UCHAR *Put = mpScanPointers[wLine] + View.wLeft;
            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
            {
                UCHAR uVal = *Get++;

                if (uVal != 0xff)
                {
                    *Put = uVal;
                }
                Put++;
            }
            Get += Getmap->wWidth - View.Width();
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Peg8106Screen::LinearRleBitmap(const PegPoint Where, const PegRect View,
    const PegBitmap *Getmap)
{
    UCHAR *Get = Getmap->pStart;
    UCHAR uVal;
    WORD wCount;

    SIGNED wLine = Where.y;

    while (wLine < View.wTop)
    {
        wCount = 0;

        while(wCount < Getmap->wWidth)
        {
            uVal = *Get++;
            if (uVal & 0x80)
            {
                uVal = (uVal & 0x7f) + 1;
                wCount += uVal;
                Get += uVal;
            }
            else
            {
                Get++;
                wCount += uVal + 1;
            }
        }
        wLine++;
    }

    while (wLine <= View.wBottom)
    {
        UCHAR *Put = mpScanPointers[wLine] + Where.x;
        SIGNED wLoop1 = Where.x;

        while (wLoop1 < Where.x + (SIGNED) Getmap->wWidth)
        {
            uVal = *Get++;

            if (uVal & 0x80)        // raw packet?
            {
                wCount = (uVal & 0x7f) + 1;
                
                while (wCount--)
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
                wCount = uVal + 1;
                uVal = *Get++;

                if (uVal == 0xff)
                {
                    wLoop1 += wCount;
                    Put += wCount;
                }
                else
                {
                    while(wCount--)
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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Peg8106Screen::RectMoveView(PegThing *Caller, const PegRect &View,
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
void Peg8106Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
    PegFont *Font, SIGNED iCount, PegRect &Rect)
{
    
    if (mbVirtualDraw)
    {
        LinearDrawText(Where, Text, Color, Font, iCount, Rect);
        return;
    }

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
                        PlotPointView(wXpos, ScanRow, (UCHAR) Color.uForeground);
                    }
                    else
                    {
                        if (Color.uFlags & CF_FILL)
                        {
                            PlotPointView(wXpos, ScanRow, (UCHAR) Color.uBackground);
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
/*--------------------------------------------------------------------------*/
void Peg8106Screen::LinearDrawText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
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
            iCharWidth = (WORD) Font->pOffsets;
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

// Microsoft optimizes out the repeated reads of the same address, so 
// we disable their optimizer here.

#if defined (_MSC_VER)
#pragma optimize("", off)
#endif
/*--------------------------------------------------------------------------*/
COLORVAL Peg8106Screen::GetPixelView(SIGNED wXPos, SIGNED wYPos)
{
    char RedPlane;
    char GreenPlane;
    char BluePlane;
    char IntensePlane;

    UCHAR PEGFAR *Get = mpScanPointers[wYPos] + (wXPos >> 3);

    WriteVidReg(5, 0);            // select read mode 0

    WriteVidReg(04, 0);
    RedPlane = *Get;
    WriteVidReg(04, 1);
    GreenPlane = *Get;
    WriteVidReg(04, 2);
    BluePlane = *Get;
    WriteVidReg(04, 3);
    IntensePlane = *Get;

    int Mask = 0x80 >> (wXPos & 0x07);
    UCHAR uVal = 0;
    if (RedPlane & Mask)
    {
        uVal |= 1;
    }
    if (BluePlane & Mask)
    {
        uVal |= 2;
    }
    if (GreenPlane & Mask)
    {
        uVal |= 4;
    }
    if (IntensePlane & Mask)
    {
        uVal |= 8;
    }
    return uVal;
}

#if defined (_MSC_VER)
#pragma optimize("", on)
#endif


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
void Peg8106Screen::ResetPalette(void)
{
    SetPalette(0, 16, DosPal);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *Peg8106Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = 16;
    return DefPalette;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Peg8106Screen::SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get)
{
    POUTB(0x3c8, iFirst);
    VGA_DELAY();

    for (SIGNED iLoop = 0; iLoop < iNum * 3; iLoop++)
    {
        #ifdef INVERT_COLORS
         POUTB(0x3c9, *Get++ ^ 0x3f);
        #else
         POUTB(0x3c9, *Get++);
        #endif
        VGA_DELAY();
    }
}

void WriteAuxReg(int index, int val)
{
    VGA_DELAY();
    POUTB(0x3de, index);
    VGA_DELAY();
    POUTB(0x3df, val);
    VGA_DELAY();
}

int ReadAuxReg(int index)
{
    VGA_DELAY();
    POUTB(0x3de, index);
    VGA_DELAY();
    return (PINB(0x3df));
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


/*--------------------------------------------------------------------------*/
void Peg8106Screen::ConfigureController(void)
{
    #if defined(PEGX)
    TX_INTERRUPT_SAVE_AREA
    #endif

    int temp;
    UCHAR *GetColor = DosPal;
    WORD wLoop;

    #if defined(PEGSMX)
    DISABLE();
    #elif defined(PEGX)
    TX_DISABLE
    #endif

    // Enable the 8106:
    POUTB(0x3c3, 0x01);         // Enable the 8106

    // Auxilliary Registers:

    WriteAuxReg(0xde, 0x1a);        // Enable Aux Regs
    ReadAuxReg(0xde);               // Enable Aux Regs

    WriteAuxReg(0x0b, 0x01);        // LCD enable
    WriteAuxReg(0x00, 0x01);        // B Regs enable
    WriteVgaReg(0x01, mwBytesPerRow);
    WriteVgaReg(0x10, 0);
    WriteVgaReg(0x11, 0);
    WriteVgaReg(0x12, mwVRes >> 1);
    WriteVgaReg(0x15, 0x00);
    WriteAuxReg(0x00, 0x00);           // B Regs disable
    WriteAuxReg(0x01, 0x01);
    WriteAuxReg(0x02, 0x12);
    WriteAuxReg(0x03, 0x00);
    WriteAuxReg(0x05, 0x01);
    WriteAuxReg(0x06, 0);
    WriteAuxReg(0x07, 0x40);
    WriteAuxReg(0x0d, 0x0);
    WriteAuxReg(0x1b, 0);
    WriteAuxReg(0x1c, 0);

    WriteAuxReg(0xde, 00);          // Disable Aux Regs

    // write the palette:

    PINB(0x3da);         // read the status register
    VGA_DELAY();

    for (wLoop = 0; wLoop < 16; wLoop++)
    {
        VGA_DELAY();
        POUTB(0x3c0, wLoop);     // stop the attribute controller
        VGA_DELAY();
        POUTB(0x3c0, wLoop);
        VGA_DELAY();
    }
    VGA_DELAY();
    POUTB(0x3c0, wLoop);
    VGA_DELAY();

    POUTB(0x3c0, 0);     // stop the attribute controller
    VGA_DELAY();
    
    POUTB(0x3c8, 0);
    VGA_DELAY();

    GetColor = DosPal;
    for (wLoop = 0; wLoop < 16 * 3; wLoop++)
    {
       #ifdef INVERT_COLORS
        POUTB(0x3c9, *GetColor++ ^ 0x3f);
       #else
        POUTB(0x3c9, *GetColor++);
       #endif
        VGA_DELAY();
    }

    VGA_DELAY();
    PINB(0x3ba);
    VGA_DELAY();
    PINB(0x3da);         // read the status register
    VGA_DELAY();
    WriteAttribReg(0x10, 0xa1);
    WriteAttribReg(0x11, 0x00);
    WriteAttribReg(0x12, 0x0f);
    WriteAttribReg(0x13, 0x00);
    WriteAttribReg(0x14, 0x00);

    WriteSeqReg(0, 0x01);
    VGA_DELAY();
    POUTB(0x3c2, 0xe3);
    VGA_DELAY();

    temp = ReadVgaReg(0x11);
    temp &= 0x7f;
    WriteVgaReg(0x11, temp);

    WriteVgaReg(0x00, 0x5f);
    WriteVgaReg(0x01, 0x4f);
    WriteVgaReg(0x02, 0x50);
    WriteVgaReg(0x03, 0x82);
    WriteVgaReg(0x04, 0x54);
    WriteVgaReg(0x05, 0x80); 
    WriteVgaReg(0x06, 0x0b); 
    WriteVgaReg(0x07, 0x3e); 
    WriteVgaReg(0x08, 0); 
    WriteVgaReg(0x09, 0x40); 
    WriteVgaReg(0x0a, 0); 
    WriteVgaReg(0x0b, 0); 
    WriteVgaReg(0x0c, 0); 
    WriteVgaReg(0x0d, 0); 
    WriteVgaReg(0x0e, 0); 
    WriteVgaReg(0x0f, 0); 
    WriteVgaReg(0x10, 0xea); 
    WriteVgaReg(0x11, 0x8c); 
    WriteVgaReg(0x12, 0xdf); 
    WriteVgaReg(0x13, 0x28); 
    WriteVgaReg(0x14, 0x00); 
    WriteVgaReg(0x15, 0xe7); 
    WriteVgaReg(0x16, 0x04); 
    WriteVgaReg(0x17, 0xe3); 
    WriteVgaReg(0x18, 0xff); 
    
    WriteSeqReg(0, 0x03);
    WriteSeqReg(0x01, 0x01);
    WriteSeqReg(0x02, 0x0f);
    WriteSeqReg(0x03, 00);
    WriteSeqReg(0x04, 0x06);

    WriteVidReg(0, 0);
    WriteVidReg(0x01, 0);
    WriteVidReg(0x02, 0);
    WriteVidReg(0x03, 0);
    WriteVidReg(0x04, 0);
    WriteVidReg(0x05, 0);
    WriteVidReg(0x06, 5);
    WriteVidReg(0x07, 0x0f);
    
    VGA_DELAY();
    POUTB(0x3c6, 0xff);
    VGA_DELAY();

    // clear the screen:

    HorizontalLine(0, mwHRes - 1, 0, BLACK, mwVRes);

    #if defined(PEGSMX)
    ENABLE();
    #elif defined(PEGX)
    TX_RESTORE
    #endif
}

#if defined(RESTORE_TEXT_ON_EXIT)

#include "mod3font.hpp"

#define TEXT_SEG  0xb8000L

/*--------------------------------------------------------------------------*/
void Peg8106Screen::ConfigureMode3(void)
{
    WORD wLoop;
    UCHAR PEGFAR *pPut1;
    UCHAR PEGFAR *pPut2;
    UCHAR PEGFAR *pPut3;

    const UCHAR PEGFAR *pGet;

  #if defined(PEGSMX)

   #if defined(_32BIT) && defined(_FLAT)
    pPut1 = (UCHAR *) VID_MEM_BASE;
    pPut2 = (UCHAR *) (VID_MEM_BASE + 0x18000);

   #else
    pPut1= (UCHAR PEGFAR *) (VID_MEM_BASE << 12);
    pPut2= (UCHAR PEGFAR *) (TEXT_SEG << 12);
   #endif

  #elif defined(PHARLAP)

    // PHARLAP always runs in 32-bit protected mode:

    pPut1 = (UCHAR *) VID_MEM_BASE;
    pPut2 = (UCHAR *) TEXT_SEG;

  #elif defined(__HIGHC__)

    // if no target RTOS, and using HIGHC, probably building for 
    // 32-bit standalone:

    pPut1 = (UCHAR *) VID_MEM_BASE;
    pPut2 = (UCHAR *) TEXT_SEG;

  #elif defined(PEGQNX)     // using Watcom/QNX

    pPut1 = (UCHAR PEGFAR *) mpScanPointers[0];

    int fd = shm_open("Physical", O_RDWR, 0777);
    pPut2 = (UCHAR PEGFAR *) mmap(0, 64*1024,
		PROT_READ | PROT_WRITE |PROT_NOCACHE, MAP_SHARED, fd, TEXT_SEG);

  #else

    // default to REAL MODE addressing:

    pPut1= (UCHAR PEGFAR *) (VID_MEM_BASE << 12);
    pPut2= (UCHAR PEGFAR *) (TEXT_SEG << 12);

  #endif

    // clear the screen:

    HorizontalLine(0, mwHRes - 1, 0, BLACK, mwVRes);

    WriteAuxReg(0xde, 0x1a);        // Enable Aux Regs
    ReadAuxReg(0xde);               // Enable Aux Regs

    WriteAuxReg(0x0b, 0x01);        // LCD enable
    WriteAuxReg(0x00, 0x01);        // B Regs enable
    WriteAuxReg(0x00, 0x00);        // B Regs disable
    WriteAuxReg(0x03, 0x00);        // Full Power
    WriteAuxReg(0x04, 0x00);        // clear scratch
    WriteAuxReg(0x05, 0x00);        // disable secondary memory
    WriteAuxReg(0x09, 0x00);        // no sprites
    WriteAuxReg(0x0a, 0x00);        // clear scratch reg
    WriteAuxReg(0x1a, 0x00);        // clear scratch reg
    WriteAuxReg(0x1b, 0x00);        // clear scratch reg
    WriteAuxReg(0x1c, 0x00);        // clear scratch reg
    WriteAuxReg(0x1d, 0x00);        // clear scratch reg

    WriteVidReg(0x06, 0x04); 

    POUTB(0x3c2, 0x67);      // Misc Output, select clock

    WriteSeqReg(0, 0);
    WriteSeqReg(1, 1);
    WriteSeqReg(2, 3);
    WriteSeqReg(3, 0);
    WriteSeqReg(4, 2);
    WriteSeqReg(0, 3);

    WriteVgaReg(0x11, 0x0c);
    WriteVgaReg(0x00, 0x5f);
    WriteVgaReg(0x01, 0x4f);
    WriteVgaReg(0x02, 0x50);
    WriteVgaReg(0x03, 0x82);
    WriteVgaReg(0x04, 0x55);
    WriteVgaReg(0x05, 0x81);
    WriteVgaReg(0x06, 0xbf);
    WriteVgaReg(0x07, 0x1d);
    WriteVgaReg(0x08, 0x00);
    WriteVgaReg(0x09, 0x47);
    WriteVgaReg(0x0a, 0x06);
    WriteVgaReg(0x0b, 0x07);
    WriteVgaReg(0x0c, 0x00);
    WriteVgaReg(0x0d, 0x00);
    WriteVgaReg(0x0e, 0x09);
    WriteVgaReg(0x0f, 0x10);
    WriteVgaReg(0x10, 0x9c);
    WriteVgaReg(0x11, 0x8e);
    WriteVgaReg(0x12, 0xef);
    WriteVgaReg(0x13, 0x28);
    WriteVgaReg(0x14, 0x1f);
    WriteVgaReg(0x15, 0x96);
    WriteVgaReg(0x16, 0xb9);
    WriteVgaReg(0x17, 0xa3);
    WriteVgaReg(0x18, 0xff);

    WriteVidReg(0, 0);
    WriteVidReg(1, 0);
    WriteVidReg(2, 0);
    WriteVidReg(3, 0);
    WriteVidReg(4, 0);
    WriteVidReg(5, 0x10);
    WriteVidReg(6, 0x0e);
    WriteVidReg(7, 0);
    WriteVidReg(8, 0xff);

    // Load the font:

    WriteSeqReg(0x02, 0x04);
    WriteSeqReg(0x04, 0x07);
    WriteVidReg(0x05, 0x00);
    WriteVidReg(0x06, 0x04);

    pGet = Mode3Font;

    for (wLoop = 0; wLoop < 128; wLoop++)
    {
        for (WORD wLoop1 = 0; wLoop1 < 16; wLoop1++)
        {
            *pPut1++ = *pGet++;
        }
        pPut1 += 16;
    }

    WriteSeqReg(0x02, 0x03);
    WriteSeqReg(0x04, 0x02);
    WriteVidReg(0x05, 0x10);
    WriteVidReg(0x06, 0x0e);

    // program the palette:

    POUTB(0x3c6, 0xff);
    POUTB(0x3c8, 0);
    pGet = DacVals;

    for (wLoop = 0; wLoop < 16 * 12; wLoop++)
    {
        POUTB(0x3c9, *pGet++);
    }

    // program attribute controller:
    
    VGA_DELAY();
    PINB(0x3da);         // read the status register
    VGA_DELAY();

    WriteAttribReg(0, 0);        
    WriteAttribReg(1, 1);        
    WriteAttribReg(2, 2);        
    WriteAttribReg(3, 3);        
    WriteAttribReg(4, 4);        
    WriteAttribReg(5, 5);        
    WriteAttribReg(6, 0x14);        
    WriteAttribReg(7, 7);        
    WriteAttribReg(8, 0x38);        
    WriteAttribReg(9, 0x39);        
    WriteAttribReg(0x0a, 0x3a);        
    WriteAttribReg(0x0b, 0x3b);        
    WriteAttribReg(0x0c, 0x3c);        
    WriteAttribReg(0x0d, 0x3d);        
    WriteAttribReg(0x0e, 0x3e);        
    WriteAttribReg(0x0f, 0x3f);        
    WriteAttribReg(0x10, 0x0c);        
    WriteAttribReg(0x11, 0x00);        
    WriteAttribReg(0x12, 0x0f);        
    WriteAttribReg(0x13, 0x00);        
    WriteAttribReg(0x14, 0x00);        

    // fill planes 0 & 1 with 0x20 and 0x07

    pPut3 = pPut2;
    
    for (wLoop = 0; wLoop < (16 * 1024); wLoop++)
    {
        *pPut2++ = 0x20;
        *pPut2++ = 0x07;
    }
}

#endif  // RESTORE_TEXT_ON_EXIT if

// end of file


