/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// lcda400.cpp - Linear 16 bbp driver for Sharp KEV7A400 Eval board. Note
// that this display supports several display types. To select this driver,
// make sure pconfig.hpp is configured to use the lcda400.hpp driver for
// PSCREEN. In the driver header file, select the correct define for the
// display type being used to have the proper configuration data generated
// in the compiled file.
//
// Notes:
//    This driver only supports 16-bit mode.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"

/*--------------------------------------------------------------------------*/
// Driver configuration
/*--------------------------------------------------------------------------*/
// Normally, the video memory is allocated in an area of memory that is
// uncached or located seperately from program data. If the following line
// is left un-commented, a static memory buffer will automatically be
// created and used in cached memory. This may cause some minor problems
// with display artifacts as the cache temporarily holds pixel data

//#define STATIC_VBUF

// If a static memory buffer isn't used, the physical address of the video
// memory buffer should be placed here. With the default Sharp boot code,
// this maps to the end of SDRAM

#define VBUF_ADDR 0xC3C00000    // Logical address
#define PBUF_ADDR 0xDD000000    // Physical address

// Default 256 color palette, used during palette updates for 16-bit driver

extern UCHAR DefPalette256[];

/*--------------------------------------------------------------------------*/
// LCD specific data
/*--------------------------------------------------------------------------*/
// LCD register enumeration list

typedef enum {LCDTIM0, LCDTIM1, LCDTIM2, LCDTIM3, LCDINT, HRSETUP,
    HRCTRL, HRTIM1, HRTIM2, LCDCTRL, LCDBASE} LCD_Types;

// Physical addresses of LCD registers
const unsigned long LCD_initaddrs [LCDBASE + 1] = {
    0x80003000,             // LCD Timing 0 register
    0x80003004,             // LCD Timing 1
    0x80003008,             // LCD Timing 2
    0x8000300C,             // LCD Timing 3
    0x80003024,             // LCD interrupt enable
    0x80001000,             // HR-TFT setup register
    0x80001004,             // HR-TFT control
    0x80001008,             // HR-TFT timing 1
    0x8000100C,             // HR-TFT timing 2
    0x8000301C,             // LCD control
    0x80003010};            // Buffer base address

#if defined(LQ121S1)
const unsigned long LCD_initvals [LCDCTRL + 1] = {
    0x58A880C4,             // LCD Timing 0 register
    0x172B1257,             // LCD Timing 1
    0x031F1800,             // LCD Timing 2
    0x00000000,             // LCD Timing 3
    0x00000000,             // LCD interrupt enable
    0x000031E0,             // HR-TFT setup register
    0x00000000,             // HR-TFT control
    0x00000000,             // HR-TFT timing 1
    0x00000000,             // HR-TFT timing 2
    0x00010929};            // LCD control

// Optimal pixel clock value for the display
#define OPTIMAL_PCLOCK 800*600*66

#elif defined (LQ104V1)
const unsigned long LCD_initvals [LCDCTRL + 1] = {
    0x58A8809C,             // LCD Timing 0 register
    0x1F1709DF,             // LCD Timing 1
    0x027F1801,             // LCD Timing 2
    0x00000000,             // LCD Timing 3
    0x00000000,             // LCD interrupt enable
    0x000027E0,             // HR-TFT setup register
    0x00000000,             // HR-TFT control
    0x00000000,             // HR-TFT timing 1
    0x00000000,             // HR-TFT timing 2
    0x00010939};            // LCD control

#define OPTIMAL_PCLOCK 640*480*66

#elif defined (LQ039Q2)
const unsigned long LCD_initvals [LCDCTRL + 1] = {
    0x140A0C4C,             // LCD Timing 0 register
    0x040404EF,             // LCD Timing 1
    0x013F3000,             // LCD Timing 2
    0x00000000,             // LCD Timing 3
    0x00000000,             // LCD interrupt enable
    0x000033FD,             // HR-TFT setup register
    0x00000003,             // HR-TFT control
    0x0000082D,             // HR-TFT timing 1
    0x000042D0,             // HR-TFT timing 2
    0x00010929};            // LCD control

#define OPTIMAL_PCLOCK 6400000

#else
  ** ERROR - Sharp LH7A400 display type not configured in lcda400.hpp **
#endif

/*--------------------------------------------------------------------------*/
// Allocate screen buffer as normal data. The screen buffer should be
// asligned on a 32 byte boundary.
/*--------------------------------------------------------------------------*/

#ifdef STATIC_VBUF
__align (32) COLORVAL gbVMemory[(PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE *
   sizeof (COLORVAL))];
#endif

/*--------------------------------------------------------------------------*/
void LQ121S1_16::BeginDraw(PegThing *pThing)
{
    // Suppress warning
    (void)pThing;

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
void LQ121S1_16::EndDraw()
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
void LQ121S1_16::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
   // Suppress warning for unused variable
   (void)Caller;

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
void LQ121S1_16::EndDraw(PegBitmap *pMap)
{
   // Suppress warning for unused variable
   (void)pMap;

    if (mbVirtualDraw)
    {
        mbVirtualDraw = FALSE;
        delete [] mpScanPointers;
        mpScanPointers = mpSaveScanPointers;
        UNLOCK_PEG
    }
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::SetPointerType(UCHAR bType)
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

#else
   // Suppress warning for unused variable
   (void)bType;
#endif
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::HidePointer(void)
{
#ifdef PEG_MOUSE_SUPPORT
    PegThing *pt = NULL;
    PegPresentationManager *pp = pt->Presentation();
    Restore(pp, &mCapture, TRUE);
    mCapture.SetValid(FALSE);
#endif
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::SetPointer(PegPoint Where)
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

#else
   // Suppress warning for unused variable
   (void)Where;
#endif
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::Capture(PegCapture *Info, PegRect &Rect)
{
    PegBitmap *pMap = Info->Bitmap();

    if (Rect.wLeft < 0)
    {
        Rect.wLeft = 0;
    }

    if (Rect.wTop < 0)
    {
        Rect.wTop = 0;
    }

    Info->SetPos(Rect);
    LONG Size = pMap->wWidth * pMap->wHeight * sizeof (COLORVAL);
    Info->Realloc(Size);

    COLORVAL *GetStart = mpScanPointers[Rect.wTop] + Rect.wLeft;

    // make room for the memory bitmap:

    pMap->uFlags = 0;         // raw format
    pMap->uBitsPix = sizeof (COLORVAL) * 8; // 8 or 16 bits per pixel

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
void LQ121S1_16::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
    // Convert the 8-bit color palette to a 16-bit color palette for use
    // with the bitmap draw functions

    COLORVAL *pPut = mcHiPalette;
    COLORVAL c;

    pPut += iFirst;

    while(iCount--)
    {
        c = (*pGet++ & 0xF8) << (10-3);    // 5 bits red
        c = c | ((*pGet++ & 0xF8) << 5-3); // 5 bits green
        c = c | ((*pGet++ & 0xF8) >> 3);   // 5 bits blue
       *pPut++ = c;                        // save the Hi-c equiv
    }
}

/*--------------------------------------------------------------------------*/
UCHAR *LQ121S1_16::GetPalette(DWORD *pPutSize)
{
    *pPutSize = mdNumColors;
    return NULL;
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::DrawTextView(PegPoint Put, const PEGCHAR *Text,
            PegColor &Color, PegFont *pFont, SIGNED iLen, PegRect &View)
{
#ifdef PEG_UNICODE
    PEGCHAR PEGFAR *pCurrentChar = (PEGCHAR PEGFAR *) Text;
    PegFont *pFontStart = pFont;

#else
    UCHAR PEGFAR *pCurrentChar = (UCHAR PEGFAR *) Text;
#endif

    UCHAR PEGFAR *pGetData;
    UCHAR PEGFAR *pGetDataBase;
    WORD  wIndex;
    WORD  wBitOffset;
    SIGNED  wXpos = Put.x;
    WORD cVal = *pCurrentChar++;
    SIGNED iCharWidth;

#ifdef DO_OUTLINE_TEXT
    if (IS_OUTLINE(pFont))
    {
        DrawOutlineText(Put, Text, Color, pFont, iLen, View);
        return;
    }
#endif

    while(cVal && wXpos <= View.wRight)
    {
        if (iLen == 0)
        {
            return;
        }
        iLen--;

#ifdef PEG_UNICODE
        pFont = pFontStart;

        while(Font)
        {
            if (cVal >= pFont->wFirstChar &&
                cVal <= pFont->wLastChar)
            {
                break;
            }
            pFont = pFont->pNext;
        }
        if (!pFont)                 // this font doesn't contain this glyph?
        {
            cVal = *pCurrentChar++; // just skip to next char
            continue;               
        }

        wIndex = cVal - (WORD) pFont->wFirstChar;

        if (IS_VARWIDTH(pFont))
        {
            wBitOffset = pFont->pOffsets[wIndex];
            iCharWidth = pFont->pOffsets[wIndex+1] - wBitOffset;
        }
        else
        {
            iCharWidth = (SIGNED) pFont->pOffsets;
            wBitOffset = iCharWidth * wIndex;
        }

#else

        wIndex = cVal - (WORD) pFont->wFirstChar;
        wBitOffset = pFont->pOffsets[wIndex];
        iCharWidth = pFont->pOffsets[wIndex+1] - wBitOffset;

#endif

        if (wXpos + iCharWidth > View.wRight)
        {
            iCharWidth = View.wRight - wXpos + 1;
        }

        WORD ByteOffset = wBitOffset / 8;
        pGetDataBase = pFont->pData + ByteOffset;
        pGetDataBase += (View.wTop - Put.y) * pFont->wBytesPerLine;

        for (SIGNED ScanRow = View.wTop; ScanRow <= View.wBottom; ScanRow++)
        {
            pGetData = pGetDataBase;
            UCHAR InMask = 0x80 >> (wBitOffset & 7);
            WORD wBitsOutput = 0;
            UCHAR cData;

#ifdef PEG_UNICODE
            if (ScanRow - Put.y < pFont->uHeight)
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
                    if (ScanRow - Put.y < pFont->uHeight)
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

                if (wXpos >= View.wLeft)
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
                if (wXpos > View.wRight)
                {
                    break;
                }
            }
            pGetDataBase += pFont->wBytesPerLine;
            wXpos -= iCharWidth;
        }
        wXpos += iCharWidth;
        cVal = *pCurrentChar++;
    }
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::LineView(SIGNED xStart, SIGNED yStart,
            SIGNED xEnd, SIGNED yEnd,  PegRect &View, PegColor Color,
            SIGNED iWidth)
{
    if (yStart == yEnd)
    {
        HorizontalLine(View.wLeft, View.wRight, View.wTop, Color.uForeground, iWidth);
        return;
    }
    if (xStart == xEnd)
    {
        VerticalLine(View.wTop, View.wBottom, View.wLeft, Color.uForeground, iWidth);
        return;
    }

    SIGNED dx = abs(xEnd - xStart);
    SIGNED dy = abs(yEnd - yStart);

    if (((dx >= dy && (xStart > xEnd)) ||
        ((dy > dx) && yStart > yEnd)))
    {
        PEGSWAP(xEnd, xStart);
        PEGSWAP(yEnd, yStart);
    }

    SIGNED y_sign = ((int) yEnd - (int) yStart) / dy;
    SIGNED x_sign = ((int) xEnd - (int) xStart) / dx;
    SIGNED decision;

    SIGNED wCurx, wCury, wNextx, wNexty, wpy, wpx;

    if (View.Contains(xStart, yStart) &&
        View.Contains(xEnd, yEnd))
    {
	    if (dx >= dy)
	    {
	        for (wCurx = xStart, wCury = yStart, wNextx = xEnd,
	             wNexty = yEnd, decision = (dx >> 1);
	             wCurx <= wNextx; wCurx++, wNextx--, decision += dy)
	        {
	            if (decision >= dx)
	            {
	                decision -= dx;
	                wCury += y_sign;
	                wNexty -= y_sign;
	            }
	            for (wpy = wCury - iWidth / 2;
	                 wpy <= wCury + iWidth / 2; wpy++)
	            {
                    PlotPointView(wCurx, wpy, Color.uForeground);
	            }
	
	            for (wpy = wNexty - iWidth / 2;
	                 wpy <= wNexty + iWidth / 2; wpy++)
	            {
                    PlotPointView(wNextx, wpy, Color.uForeground);
	            }
	        }
	    }
	    else
	    {
	        for (wCurx = xStart, wCury = yStart, wNextx = xEnd,
	                wNexty = yEnd, decision = (dy >> 1);
	            wCury <= wNexty; wCury++, wNexty--, decision += dx)
	        {
	            if (decision >= dy)
	            {
	                decision -= dy;
	                wCurx += x_sign;
	                wNextx -= x_sign;
	            }
	            for (wpx = wCurx - iWidth / 2;
	                 wpx <= wCurx + iWidth / 2; wpx++)
	            {
                    PlotPointView(wpx, wCury, Color.uForeground);
	            }
	
	            for (wpx = wNextx - iWidth / 2;
	                 wpx <= wNextx + iWidth / 2; wpx++)
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
	        for (wCurx = xStart, wCury = yStart, wNextx = xEnd,
	             wNexty = yEnd, decision = (dx >> 1);
	             wCurx <= wNextx; wCurx++, wNextx--, decision += dy)
	        {
	            if (decision >= dx)
	            {
	                decision -= dx;
	                wCury += y_sign;
	                wNexty -= y_sign;
	            }
	            for (wpy = wCury - iWidth / 2;
	                 wpy <= wCury + iWidth / 2; wpy++)
	            {
	                if (wCurx >= View.wLeft &&
	                    wCurx <= View.wRight &&
	                    wpy >= View.wTop &&
	                    wpy <= View.wBottom)
	                {
	                    PlotPointView(wCurx, wpy, Color.uForeground);
	                }
	            }
	
	            for (wpy = wNexty - iWidth / 2;
	                 wpy <= wNexty + iWidth / 2; wpy++)
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
	        for (wCurx = xStart, wCury = yStart, wNextx = xEnd,
	                wNexty = yEnd, decision = (dy >> 1);
	            wCury <= wNexty; wCury++, wNexty--, decision += dx)
	        {
	            if (decision >= dy)
	            {
	                decision -= dy;
	                wCurx += x_sign;
	                wNextx -= x_sign;
	            }
	            for (wpx = wCurx - iWidth / 2;
	                 wpx <= wCurx + iWidth / 2; wpx++)
	            {
	                if (wpx >= View.wLeft &&
	                    wpx <= View.wRight &&
	                    wCury >= View.wTop &&
	                    wCury <= View.wBottom)
	                {
	                    PlotPointView(wpx, wCury, Color.uForeground);
	                }
	            }
	
	            for (wpx = wNextx - iWidth / 2;
	                 wpx <= wNextx + iWidth / 2; wpx++)
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
void LQ121S1_16::BitmapView(const PegPoint Where,
            const PegBitmap *pMap, const PegRect &View)
{
    if (IS_RLE(pMap))
    {
        DrawRleBitmap(Where, View, pMap);
    }
    else
    {
        if (pMap->uBitsPix == 8)
        {
            Draw8BitBitmap(Where, View, pMap);
        }
        else
        {
            COLORVAL *Get = (COLORVAL *) pMap->pStart;
            Get += (View.wTop - Where.y) * pMap->wWidth;
            Get += View.wLeft - Where.x;

            if (HAS_TRANS(pMap))
            {
                for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
                {
                    COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
                    for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
                    {
                        COLORVAL uVal = *Get++;

                        if (uVal != pMap->dTransColor)
                        {
                            *Put = uVal;
                        }
                        Put++;
                    }
                    Get += pMap->wWidth - View.Width();
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
                    Get += pMap->wWidth - View.Width();
                }
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::RectMoveView(PegThing *Caller, const PegRect &View,
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
void LQ121S1_16::HorizontalLine(SIGNED xStart, SIGNED xEnd,
            SIGNED y, COLORVAL cColor, SIGNED iWidth)
{
    SIGNED iLen = xEnd - xStart + 1;

    if (iLen <= 0)
    {
        return;
    }
    while(iWidth-- > 0)
    {
        COLORVAL *Put = mpScanPointers[y] + xStart;
        SIGNED iLen1 = iLen;

        while(iLen1--)
        {
            *Put++ = cColor;
        }
        y++;
    }
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::VerticalLine(SIGNED yStart, SIGNED yEnd,
            SIGNED x, COLORVAL cColor, SIGNED iWidth)
{
    while(yStart <= yEnd)
    {
        SIGNED lWidth = iWidth;
        COLORVAL *Put = mpScanPointers[yStart] + x;

        while (lWidth-- > 0)
        {
            *Put++ = cColor;
        }
        yStart++;
    }
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::HorizontalLineXOR(SIGNED xs, SIGNED xe, SIGNED y)
{
    COLORVAL *Put = mpScanPointers[y] + xs;

    while (xs <= xe)
    {
        *Put ^= 0xffff;
        Put += 2;
        xs += 2;
    }
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::VerticalLineXOR(SIGNED ys, SIGNED ye, SIGNED x)
{
    COLORVAL uVal;

    while (ys <= ye)
    {
        COLORVAL *Put = mpScanPointers[ys] + x;
        uVal = *Put;
        uVal ^= 0xffff;
        *Put = uVal;
        ys += 2;
    }
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::DrawRleBitmap(const PegPoint Where, const PegRect View,
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
void LQ121S1_16::Draw8BitBitmap(const PegPoint Where, const PegRect View,
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
PegBitmap *LQ121S1_16::CreateBitmap(SIGNED wWidth, SIGNED wHeight,
    BOOL bHasTrans)
{
   PegBitmap *pMap = NULL;

    if (wWidth && wHeight)
    {
        pMap = new PegBitmap;
        pMap->wWidth = wWidth;
        pMap->wHeight = wHeight;
        pMap->dTransColor = TRANSPARENCY;

        if (bHasTrans)
        {
            pMap->uFlags = BMF_HAS_TRANS;
        }
        else
        {
            pMap->uFlags = 0;
        }

        pMap->uBitsPix = 16;

        DWORD dSize = (DWORD) wWidth * (DWORD) wHeight * sizeof(COLORVAL);

        pMap->pStart = (UCHAR PEGFAR *) new COLORVAL[dSize];

        if (!pMap->pStart)
        {
            delete pMap;
            return NULL;
        }
        if (bHasTrans)
        {
            memset(pMap->pStart, TRANSPARENCY, dSize);
        }
        else
        {
            memset(pMap->pStart, 0, dSize);
        }


    }
    return pMap;
}
 
#ifdef DOUBLE_BUFFER

/*--------------------------------------------------------------------------*/
void LQ121S1_16::MemoryToScreen(void)
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

/*--------------------------------------------------------------------------*/
// Return a pointer to the video buffer
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *LQ121S1_16::GetVideoAddress(void)
{
    UCHAR *pMem;

#ifdef STATIC_VBUF
    pMem = (UCHAR PEGFAR *) gbVMemory;

#else
    pMem = (UCHAR PEGFAR *) VBUF_ADDR;
#endif

    return pMem; 
}

/*--------------------------------------------------------------------------*/
unsigned long LQ121S1_16::csc_get_hclk (void)
{
    unsigned long m1;
    unsigned long m2;
    unsigned long p;
    unsigned long ps;
    unsigned long clkset;
    unsigned long cpumhz;

    // Get CLKSET register to determine bus speed
    clkset = * (unsigned long *) 0x80000420;

	m1 = (clkset >>  7) & 0x0F;
	m2 = (clkset >> 11) & 0x1F;
	p  = (clkset >>  2) & 0x1F;
	ps = (clkset >> 18) & 0x03;

	cpumhz = (((((m2 + 2) * 14745600) >> ps) / (p + 2)) * (m1 + 2));

    return (cpumhz / ((clkset & 0x3) + 1));
}

/*--------------------------------------------------------------------------*/
unsigned long LQ121S1_16::lcd_get_divider (void)
 {
    unsigned long div = 1;
    unsigned long lcd_clock;
    unsigned long lcd_diff1, lcd_diff2;
    unsigned long ahb_speed;

    // Get AHB bus speed
    ahb_speed = (unsigned long) csc_get_hclk ();
    lcd_clock = ahb_speed / div;

    // Try to get the best divider just under the nominal frequency
    while (lcd_clock > OPTIMAL_PCLOCK)
    {
       div++;
       lcd_clock = ahb_speed / div;
    }

    if (div > 1)
    {
       // Determine if 'div' or 'div - 1' is closer to nominal frequency
       if (lcd_clock > OPTIMAL_PCLOCK)
       {
          lcd_diff1 = lcd_clock - OPTIMAL_PCLOCK;
       }
       else
       {
          lcd_diff1 = OPTIMAL_PCLOCK - lcd_clock;
       }

       lcd_clock = ahb_speed / (div - 1);
       if (lcd_clock > OPTIMAL_PCLOCK)
       {
          lcd_diff2 = lcd_clock - OPTIMAL_PCLOCK;
       }
       else
       {
          lcd_diff2 = OPTIMAL_PCLOCK - lcd_clock;
       }

       // If diff1 is greater than diff2, then 'div + 1' is the
       // closest divider.
       if ((lcd_diff1 > lcd_diff2) && (div > 1))
       {
          div--;
       }
    }

    return div;
}

/*--------------------------------------------------------------------------*/
unsigned long LQ121S1_16::UpdatePixelClock (void)
{
   unsigned long t2, busdiv;

   // Get the timing 2 register (with clock divider)
   t2 = * (unsigned long *) LCD_initaddrs [LCDTIM2];

   // Mask out divider and bypass bits
   t2 = t2 & 0xFBFFFFE0;

   // Compute an optimal pixel clock divider from the bus clock speed
   busdiv = lcd_get_divider ();
   if (busdiv == 1)
   {
      // Bypass pixel divider, bus speed may be too slow for the display
      busdiv = 0x04000000;
   }
   else
   {
      // Use panel clock divisor (/2 should be programmed as 0x0 with the
      // bypass pixel divider logic disabled
      busdiv = busdiv - 2;
   }

   // Update timing 2 register with new values
   * (unsigned long *) LCD_initaddrs [LCDTIM2] = t2 | busdiv;

   return busdiv;
}

/*--------------------------------------------------------------------------*/
void LQ121S1_16::ConfigureController(void)
{
    int i;
    COLORVAL *dptr;

    // Disable LCD controller
    * (unsigned long *) (LCD_initaddrs [LCDCTRL]) = 0x0;

    // Enable the 8 high data lines for the LCD controller so all 16 bits
    // are enabled to the LCD.
    * (unsigned long *) (0x80000E2C) = (* (unsigned long *) (0x80000E2C) |
       0x3);

    // Initialize LCD controller registers (except for control register)
    for (i = LCDTIM0; i <= HRTIM2; i++)
    {
        * (unsigned long *) (LCD_initaddrs [i]) = LCD_initvals [i];
    }

     // Set the pixel clock rate
     (void) UpdatePixelClock ();

     // Enable the LCD controller
    * (unsigned long *) (LCD_initaddrs [LCDCTRL]) = LCD_initvals [LCDCTRL];

    // Turn on the LCD backlight, enable LCD power, and turn on the LCD
    // data buffers (via the CPLD)
    * (unsigned short *) (0x2000000C) = (* (unsigned short *) (0x2000000C) |
        0x1F);

#ifdef STATIC_VBUF
    // If using the STATIC_VBUF define, the gbVMemory pointer will be a
    // logical address to the frame memory buffer. The LCD controller
    // requires a physical address to be used for the frame memory buffer
    // address. The following define (VIRTUAL_TO_PHYSICAL) needs to
    // contain the actual function name used to perform this conversion
    #define VIRTUAL_TO_PHYSICAL LH7A400_cp15_map_virtual_to_physical

    // Set frame buffer address
    * (unsigned long *) (LCD_initaddrs [LCDBASE]) = (unsigned long *)
        VIRTUAL_TO_PHYSICAL ((void *) GetVideoAddress());

#else
    // Set frame buffer address
    * (unsigned long *) (LCD_initaddrs [LCDBASE]) =
        (unsigned long) PBUF_ADDR;
#endif

    // Clear display memory
    dptr = (COLORVAL *) GetVideoAddress();
    for (i = 0; i < (PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE); i++)
    {
       *dptr++ = 0;
    }
}

/*--------------------------------------------------------------------------*/
LQ121S1_16::LQ121S1_16(PegRect &Rect) : PegScreen(Rect)
{    
    mdNumColors = PEG_NUM_COLORS;

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();

    mpScanPointers = new COLORVAL PEGFAR *[Rect.Height()];
    COLORVAL *CurrentPtr = (COLORVAL *) GetVideoAddress();

    SetPalette(0, 232, DefPalette256);

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
LQ121S1_16::~LQ121S1_16()
{
    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/
PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new LQ121S1_16(Rect);
    return pScreen;
}



