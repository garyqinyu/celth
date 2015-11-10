/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// sc400_2.hpp - Linear screen driver for 2-bpp (4 color or grayscale)
// operation with SC400 internal video controller.
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
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _SC400_2SCREEN_
#define _SC400_2SCREEN_

// default to 1/4 VGA screen size:

#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 240

#define VID_MEM_BASE 0xA0000
//#define VID_MEM_BASE 0xB8000


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
class SC400_2Screen : public PegScreen
{
    public:
        SC400_2Screen(PegRect &);
        ~SC400_2Screen();

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
                _pGet += pMap->wWidth * x;
                _pGet += y;
                return *_pGet;
            }
            WORD wBytesPerLine = (pMap->wWidth + 3) >> 2;
//            COLORVAL *_pGet = pMap->pStart;
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
            COLORVAL *pPut = pMap->pStart;
            pPut += wBytesPerLine * y;
            pPut += x >> 2;
            UCHAR _uShift = 6 - ((x & 3) << 1);
            UCHAR _uVal = *pPut;
            _uVal &= ~(0xc0 >> _uShift);
            _uVal |= cVal << (6 - _uShift);
            *pPut = _uVal;
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
};

#endif



