/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// proscrn4.hpp - Linear screen driver for 4-bpp (16 color) operation.
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
//  This screen driver is intended for use with 16-color systems that support
//  a linear frame buffer. Example controllers include the SMOS 135xx series
//  and the MPC823.
//
//  This driver is for use with PROFILE_MODE, i.e. the screen has been rotated
//  90 degrees clockwise or CCW.
//
//  This driver can be tested under Win32 by turning on the definition
//  PEGWIN32. You can safely delete all code bracketed by the PEGWIN32
//  definition when moving to your target.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PRO4SCREEN_
#define _PRO4SCREEN_

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


/*--------------------------------------------------------------------------*/
#ifdef ROTATE_CCW
#define PlotPointView(x, y, c)\
{\
    UCHAR *_Put = mpScanPointers[x] - (y >> 1);\
    UCHAR _uVal = *_Put;\
    if (y & 1)\
    {\
        _uVal &= 0x0f;\
        _uVal |= c << 4;\
    }\
    else\
    {\
        _uVal &= 0xf0;\
        _uVal |= c;\
    }\
    *_Put = _uVal;\
}
#else
#define PlotPointView(x, y, c)\
{\
    UCHAR *_Put = mpScanPointers[x] + (y >> 1);\
    UCHAR _uVal = *_Put;\
    if (y & 1)\
    {\
        _uVal &= 0xf0;\
        _uVal |= c;\
    }\
    else\
    {\
        _uVal &= 0x0f;\
        _uVal |= c << 4;\
    }\
    *_Put = _uVal;\
}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Pro4Screen : public PegScreen
{
    public:
       #ifndef PEGWIN32
        Pro4Screen(PegRect &);
       #else
        Pro4Screen(HWND hwnd, PegRect &);
        SIGNED mwWinRectXOffset;
        SIGNED mwWinRectYOffset;
       #endif

        virtual ~Pro4Screen();

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
            #ifdef ROTATE_CCW
            UCHAR *_Get = mpScanPointers[x] - (y >> 1);
            UCHAR _uVal = *_Get;
            if (y & 1)
            {
                _uVal >>= 4;
            }
            else
            {
                _uVal &= 0x0f;
            }
            #else
            UCHAR *_Get = mpScanPointers[x] + (y >> 1);
            UCHAR _uVal = *_Get;
            if (y & 1)
            {
                _uVal &= 0x0f;
            }
            else
            {
                _uVal >>= 4;
            }
            #endif
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
            WORD wBytesPerLine = (pMap->wHeight + 1) >> 1;
            _pGet += wBytesPerLine * x;

           #ifdef ROTATE_CCW
            _pGet -= y >> 1;
            UCHAR uVal = *_pGet;
            if (x & 1)
            {
                uVal >>= 4;
            }
            else
            {
                uVal &= 0x0f;
            }
           #else
            _pGet += y >> 1;
            UCHAR uVal = *_pGet;
            if (x & 1)
            {
                uVal &= 0x0f;
            }
            else
            {
                uVal >>= 4;
            }
           #endif
            return (COLORVAL) uVal;
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            WORD wBytesPerLine = (pMap->wHeight + 1) >> 1;
            COLORVAL *pPut = pMap->pStart;
            pPut += wBytesPerLine * x;

            #ifdef ROTATE_CCW
            pPut -= y >> 1;

            UCHAR uVal = *pPut;
            if (x & 1)
            {
                uVal &= 0x0f;
                uVal |= (UCHAR) cVal << 4;
            }
            else
            {
                uVal &= 0xf0;
                uVal |= cVal;
            }
            #else
            pPut += y >> 1;

            UCHAR uVal = *pPut;
            if (x & 1)
            {
                uVal &= 0xf0;
                uVal |= cVal;
            }
            else
            {
                uVal &= 0x0f;
                uVal |= (UCHAR) cVal << 4;
            }
            #endif
            *pPut = (COLORVAL) uVal;
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
        UCHAR  muPalette[16 * 3];

       #ifdef PEGWIN32

        void MemoryToScreen(void);
        HWND mHWnd;
        HPALETTE mhPalette;
        HDC  mHdc;
        UCHAR *mpDisplayBuff;
       #endif
};

#endif



