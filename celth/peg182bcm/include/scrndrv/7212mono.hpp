/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 7212mono.hpp - 4-gray screen driver for Cirrus Logic 7212 ARM7tm
//  platform. This is an ARM7 based platform with integrated LCD and
//  touch screen.
//
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
//  This screen driver is very specific to the Cirrus Logic ARM7 ASIC. The
//  LCD controller configuration registers may be different if you are using
//  a different version of the ARM7 processor. Note the the lh77xxxx screen
//  drivers are also for ARM7, but the configuration registers for the LCD
//  controller are much different.
//
//  This driver can be tested under Win32 by turning on the definition
//  PEGWIN32. You can safely delete all code bracketed by the PEGWIN32
//  definition when moving to your target.
//
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _MONO7212SCREEN_
#define _MONO7212SCREEN_

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 240

#define PlotPointView(x, y, c)\
    {\
    UCHAR *_Put = mpScanPointers[y] + (x >> 3);\
    UCHAR _uShift = ( x & 7);\
    UCHAR _uVal = *_Put & ~(0x01 << _uShift);\
    _uVal |= (c << _uShift);\
    *_Put = _uVal;\
    }

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class CLMonoScreen : public PegScreen
{
    public:
       #ifndef PEGWIN32
        CLMonoScreen(PegRect &);
       #else
        CLMonoScreen(HWND hwnd, PegRect &);
        SIGNED mwWinRectXOffset;
        SIGNED mwWinRectYOffset;
       #endif

        virtual ~CLMonoScreen();

        //---------------------------------------------------------
        // Public pure virtual functions in PegScreen:
        //---------------------------------------------------------

        void BeginDraw(PegThing *);
        void BeginDraw(PegThing *, PegBitmap *);
        void EndDraw(void);
        void EndDraw(PegBitmap *);
        void SetPointerType(UCHAR bType);
        void SetPointer(PegPoint);
        void Capture(PegCapture *Info, PegRect &Rect);
        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);
        COLORVAL GetPixelView(SIGNED x, SIGNED y)
        {
            UCHAR *_Get = mpScanPointers[y] + (x >> 3);
            UCHAR _uShift = 7 - (x & 7);
            UCHAR _uVal = *_Get;
            _uVal >>= _uShift;
            _uVal &= 1;
            return ((COLORVAL) _uVal);
        }

        #ifdef PEG_IMAGE_SCALING
        COLORVAL GetBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap)
        {
            COLORVAL *_pGet = pMap->pStart;
            if (pMap->uBitsPix == 8)
            {
                _pGet += pMap->wWidth * y;
                _pGet += x;
                return *_pGet;
            }
            WORD wBytesPerLine = (pMap->wWidth + 7) >> 3;
            _pGet += wBytesPerLine * y;
            _pGet += x >> 3;
            UCHAR _uVal = *_pGet;
            UCHAR _uShift = 7 - (x & 7);
            _uVal >>= _uShift;
            _uVal &= 1;
            return ((COLORVAL) _uVal);
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            WORD wBytesPerLine = (pMap->wWidth + 7) >> 3;
            COLORVAL *_pPut = pMap->pStart;
            _pPut += wBytesPerLine * y;
            _pPut += x >> 3;
            UCHAR _uShift = 7 - (x & 7);
            UCHAR _uVal = *_pPut;
            _uVal &= ~(0x01 << _uShift);
            _uVal |= cVal << _uShift;
            *_pPut = _uVal;
        }
        #endif

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

        virtual PegBitmap *CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans);

    protected:
        
        void HidePointer(void);
        UCHAR *GetVideoAddress(void);
        void ConfigureController(void);

    private:

        //---------------------------------------------------------
        // Protected pure virtual functions in PegScreen:
        //---------------------------------------------------------

        void DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
            PegFont *Font, SIGNED iCount, PegRect &Rect);
        void LineView(SIGNED xStart, SIGNED yStart, SIGNED xEnd, SIGNED yEnd,
            PegRect &Rect, PegColor Color, SIGNED wWidth);
        void BitmapView(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void RectMoveView(PegThing *Caller, const PegRect &View,
            const SIGNED xMove, const SIGNED yMove);

        void DrawRleBitmap(const PegPoint Where, const PegRect View,
            const PegBitmap *Getmap);

        void HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
            COLORVAL Color, SIGNED wWidth);
        void VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
            COLORVAL Color, SIGNED wWidth);
        void HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos);
        void VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos);

        //---------------------------------------------------------
        // End protected pure virtual functions.
        //---------------------------------------------------------

        // functions for drawing to off-screen bitmaps, using linear addressing:
        void DrawFastNativeBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void DrawSlowNativeBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void Draw2ColorBitmap(const PegPoint Where, const PegBitmap* GetMap,
            const PegRect& View);

        UCHAR  muPalette[2 * 3];

       #ifdef PEGWIN32
        void MemoryToScreen(void);
        HWND mHWnd;
        HPALETTE mhPalette;
        HDC  mHdc;
       #endif
};

#endif  // _MONO7211SCREEN_



