/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// proscrn2.hpp - Profile mode screen driver for 2-bpp (4-color or grays)
//                operation.
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

#ifndef _PRO2SCREEN_
#define _PRO2SCREEN_

#define PEG_PROFILE_MODE        // always leave this turned on

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Select either clockwise or counter-clockwise screen rotation:

//#define ROTATE_CCW          // counter clockwise
#define ROTATE_CW         // clockwise

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 240
#define PEG_VIRTUAL_YSIZE 320


#ifdef ROTATE_CCW
#define PlotPointView(x, y, c)\
    {\
    UCHAR *_Put = mpScanPointers[x] - (y >> 2);\
    UCHAR _uShift = ( y & 3) << 1;\
    UCHAR _uVal = *_Put & ~(0x03 << _uShift);\
    _uVal |= c << _uShift;\
    *_Put = _uVal;\
    }
#else
#define PlotPointView(x, y, c)\
    {\
    UCHAR *_Put = mpScanPointers[x] + (y >> 2);\
    UCHAR _uShift = (3 - (y & 3)) << 1;\
    UCHAR _uVal = *_Put & ~(0x03 << _uShift);\
    _uVal |= c << _uShift;\
    *_Put = _uVal;\
    }
#endif


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Pro2Screen : public PegScreen
{
    public:
       #ifndef PEGWIN32
        Pro2Screen(PegRect &);
       #else
        Pro2Screen(HWND hwnd, PegRect &);
        SIGNED mwWinRectXOffset;
        SIGNED mwWinRectYOffset;
       #endif

        virtual ~Pro2Screen();

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
            UCHAR *_Get = mpScanPointers[x] + (y >> 2);
            UCHAR _uShift = 6 - (( y & 3) << 1);
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
                _pGet += pMap->wWidth * x;
                _pGet += y;
                return *_pGet;
            }
            WORD wBytesPerLine = (pMap->wHeight + 3) >> 2;
            _pGet += wBytesPerLine * x;
            _pGet -= y >> 2;
            UCHAR _uVal = *_pGet;
            UCHAR _uShift = 6 - (( y & 3) << 1);
            _uVal >>= _uShift;
            _uVal &= 3;
            return ((COLORVAL) _uVal);
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            WORD wBytesPerLine = (pMap->wHeight + 3) >> 2;
            COLORVAL *_pPut = pMap->pStart;
            _pPut += wBytesPerLine * x;
            _pPut -= y >> 2;
            UCHAR _uShift = 6 - ((y & 3) << 1);
            UCHAR _uVal = *_pPut;
            _uVal &= ~(0xc0 >> _uShift);
            _uVal |= cVal << (6 - _uShift);
            *_pPut = _uVal;
        }
        #endif

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

        virtual PegBitmap *CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans);

    protected:
        
        SIGNED miPitch;
    
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
        void DrawFastBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void DrawUnalignedBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        UCHAR  muPalette[4 * 3];


       #ifdef PEGWIN32

        void MemoryToScreen(void);
        HWND mHWnd;
        HPALETTE mhPalette;
        HDC  mHdc;
        UCHAR *mpDisplayBuff;
       #endif
};

#endif



