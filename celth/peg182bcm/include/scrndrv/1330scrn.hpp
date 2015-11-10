/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1330scrn.hpp - Linear screen driver for 1-bpp (monochrome) operation with
//  SED1330 video controller.
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
//  This screen driver is intended for use with monochrome systems that support
//  a linear frame buffer. Example controllers include the SMOS 135xx series,
//  the MPC823, SC300, and many others.
//
//  This driver can be tested under Win32 by turning on the definition
//  PEGWIN32. You can safely delete all code bracketed by the PEGWIN32
//  definition when moving to your target.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _SED1330Screen_
#define _SED1330Screen_

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 240

#define PlotPointView(x, y, c)\
    {\
    UCHAR *_Put = mpScanPointers[y] + (x >> 3);\
    UCHAR _uShift = ( x & 7);\
    UCHAR _uVal = *_Put & ~(0x80 >> _uShift);\
    _uVal |= c << (7 - _uShift);\
    *_Put = _uVal;\
    }

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SED1330Screen : public PegScreen
{
    public:
        SED1330Screen(PegRect &);
        virtual ~SED1330Screen();

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
        void DrawFastBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void DrawUnalignedBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        UCHAR  muPalette[2 * 3];

        // function for double-buffering

        void MemoryToScreen(void);
};

#endif



