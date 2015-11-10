/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// l2scrn.hpp - Linear screen driver for 2-bpp (4 color or grayscale) operation.
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
//  This screen driver is intended for use with 4-color systems that support
//  a linear frame buffer. Example controllers include the SMOS 135xx series,
//  the MPC823, SC300, and many others.
//
//  This driver can be tested under Win32 by turning on the definition
//  PEGWIN32. You can safely delete all code bracketed by the PEGWIN32
//  definition when moving to your target.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _L2SCREEN_
#define _L2SCREEN_

// default to 1/4 VGA screen size:

#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480

#define PlotPointView(x, y, c)\
    {\
    UCHAR *_Put = mpScanPointers[y] + (x >> 2);\
    UCHAR _uShift = ( x & 3) << 1;\
    UCHAR _uVal = *_Put & ~(0xc0 >> _uShift);\
    _uVal |= c << (6 - _uShift);\
    *_Put = _uVal;\
    }


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class L2Screen : public PegScreen
{
    public:
       #ifndef PEGWIN32
        L2Screen(PegRect &);
       #else
        L2Screen(HWND hwnd, PegRect &);
        SIGNED mwWinRectXOffset;
        SIGNED mwWinRectYOffset;
       #endif

        virtual ~L2Screen();

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

        COLORVAL GetPixelView(SIGNED x, SIGNED y)
        {
            UCHAR *_Get = mpScanPointers[y] + (x >> 2);
            UCHAR _uShift = 6 - (( x & 3) << 1);
            UCHAR _uVal = *_Get;
            _uVal >>= _uShift;
            _uVal &= 3;
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

            WORD wBytesPerLine = (pMap->wWidth + 3) >> 2;
            _pGet += wBytesPerLine * y;
            _pGet += x >> 2;
            UCHAR _uVal = *_pGet;
            UCHAR _uShift = 6 - (( x & 3) << 1);
            _uVal >>= _uShift;
            _uVal &= 3;
            return ((COLORVAL) _uVal);
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            WORD wBytesPerLine = (pMap->wWidth + 3) >> 2;
            COLORVAL *_pPut = pMap->pStart;
            _pPut += wBytesPerLine * y;
            _pPut += x >> 2;
            UCHAR _uShift = 6 - ((x & 3) << 1);
            UCHAR _uVal = *_pPut;
            _uVal &= ~(0x03 << _uShift);
            _uVal |= cVal << _uShift;
            *_pPut = _uVal;
        }
        #endif

        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

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
        void DrawFast4ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void DrawUnaligned4ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        UCHAR  muPalette[4 * 3];

        SIGNED miPitch;

       #ifdef PEGWIN32

        void MemoryToScreen(void);
        UCHAR FAR *mpSecondBuffer;
        HWND mHWnd;
        HPALETTE mhPalette;
        HDC  mHdc;
       #endif
};

#endif



