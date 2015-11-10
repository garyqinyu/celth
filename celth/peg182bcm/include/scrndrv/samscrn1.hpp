
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// samscrn1.hpp - Linear screen driver for 1-bpp (monochrome) operation.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-1998 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// This is a modified version of monoscrn- This version has been tailored for
// use with the SamSung S3C44 ARM system.
// 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _MONOSCREEN_
#define _MONOSCREEN_


/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 240

//#define DOUBLE_BUFFER     // can be turned on for double buffering

#define PlotPointView(x, y, c)\
    {\
    UCHAR *_Put = mpScanPointers[y] + (x >> 3);\
    UCHAR _uShift = ( x & 7);\
    UCHAR _uVal = *_Put & ~(0x80 >> _uShift);\
    _uVal |= c << (7 - _uShift);\
    *_Put = _uVal;\
    }


/* LCD controller register addresses */

#ifndef rLCDCON1

#define CLKVAL (12)
#define rLCDCON1        (*(volatile unsigned *)0x1f00000)
#define rLCDCON2        (*(volatile unsigned *)0x1f00004)
#define rLCDSADDR1      (*(volatile unsigned *)0x1f00008)
#define rLCDSADDR2      (*(volatile unsigned *)0x1f0000c)
#define rREDLUT         (*(volatile unsigned *)0x1f00010)
#define rGREENLUT       (*(volatile unsigned *)0x1f00014)
#define rBLUELUT        (*(volatile unsigned *)0x1f00018)
#define rDP1_2          (*(volatile unsigned *)0x1f0001c)
#define rDP4_7H         (*(volatile unsigned *)0x1f00020)
#define rDP4_7L         (*(volatile unsigned *)0x1f00024)
#define rDP3_5          (*(volatile unsigned *)0x1f00028)
#define rDP2_3          (*(volatile unsigned *)0x1f0002c)
#define rDP3_4          (*(volatile unsigned *)0x1f00030)
#define rDP5_7H         (*(volatile unsigned *)0x1f00034)
#define rDP5_7L         (*(volatile unsigned *)0x1f00038)
#define rDP4_5          (*(volatile unsigned *)0x1f0003c)
#define rDP6_7H         (*(volatile unsigned *)0x1f00040)
#define rDP6_7L         (*(volatile unsigned *)0x1f00044)
#define rLCDCON3        (*(volatile unsigned *)0x1f00048)
#define rDITHMODE       (*(volatile unsigned *)0x1f0004c)

#endif


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class MonoScreen : public PegScreen
{
    public:
        MonoScreen(PegRect &);

        virtual ~MonoScreen();

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
                _pGet += pMap->wWidth * x;
                _pGet += y;
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
            _uVal &= ~(0x80 >> _uShift);
            _uVal |= cVal << (7 - _uShift);
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
};

#endif



