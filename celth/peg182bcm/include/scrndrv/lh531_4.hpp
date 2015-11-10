/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// l4scrn.hpp - Linear screen driver for 4-bpp (16 color) operation.
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
//  This driver can be tested under Win32 by turning on the definition
//  PEGWIN32. You can safely delete all code bracketed by the PEGWIN32
//  definition when moving to your target.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _SHARP531_
#define _SHARP531_


/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480

/*--------------------------------------------------------------------------*/
// LCD register addresses
/*--------------------------------------------------------------------------*/
#define LCD_Timing0     0xFFFF2000
#define LCD_Timing1     0xFFFF2004
#define LCD_Timing2     0xFFFF2008
#define LCD_Timing3     0xFFFF200C
#define LCD_UPBASE      0xFFFF2010
#define LCD_LPBASE      0xFFFF2014
#define LCD_INTREnable  0xFFFF2018
#define LCD_Control     0xFFFF201C
#define LCD_Status      0xFFFF2020
#define LCD_Interrupt   0xFFFF2024
#define LCD_UPCURR      0xFFFF2028
#define LCD_LPCURR      0xFFFF202c
#define DMTNHRTFTSetup   0xFFFF2800
#define DMTNHRTFTControl 0xFFFF2804
#define DMTNHRTFTTiming1 0xFFFF2808
#define DMTNHRTFTTiming2 0xFFFF280C


/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c)\
{\
    UCHAR *_Put = mpScanPointers[y] + (x >> 1);\
    UCHAR _uVal = *_Put;\
    if (x & 1)\
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


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Sharp531Screen : public PegScreen
{
    public:
        Sharp531Screen(PegRect &);

        virtual ~Sharp531Screen();

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
            UCHAR *_Get = mpScanPointers[y] + (x >> 1);
            UCHAR _uVal = *_Get;
            if (x & 1)
            {
                _uVal &= 0x0f;
            }
            else
            {
                _uVal >>= 4;
            }
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

            WORD wBytesPerLine = (pMap->wWidth + 1) >> 1;
            _pGet += wBytesPerLine * y;
            _pGet += x >> 1;
            UCHAR uVal = *_pGet;
            if (x & 1)
            {
                uVal &= 0x0f;
            }
            else
            {
                uVal >>= 4;
            }
            return (COLORVAL) uVal;
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            WORD wBytesPerLine = (pMap->wWidth + 1) >> 1;
            COLORVAL *_pPut = pMap->pStart;
            _pPut += wBytesPerLine * y;
            _pPut += x >> 1;

            UCHAR uVal = *_pPut;
            if (x & 1)
            {
                uVal &= 0xf0;
                uVal |= (UCHAR) cVal;
            }
            else
            {
                uVal &= 0x0f;
                uVal |= (UCHAR) cVal << 4;
            }
            *_pPut = (COLORVAL) uVal;
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
        void Draw16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void DrawUnaligned16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);

        UCHAR  muPalette[16 * 3];
};

#endif


