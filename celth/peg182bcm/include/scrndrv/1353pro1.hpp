
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1353pro1.hpp - Profile mode screen driver for SED1353 running in 
//  monochrome (black and white) color depth.
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
//  This driver was created from the proscrn1 screen driver template. To this
//  we have added the 1353 specific configuration code.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


#ifndef _PRO1353SCREEN_
#define _PRO1353SCREEN_

#define PEG_PROFILE_MODE        // always leave this turned on

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Select either clockwise or counter-clockwise screen rotation:

#define ROTATE_CCW          // counter clockwise
//#define ROTATE_CW         // clockwise

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/

#define PEG_VIRTUAL_XSIZE 240
#define PEG_VIRTUAL_YSIZE 320


/*--------------------------------------------------------------------------*/
// Address of Video Frame Buffer and Video Controller Registers. Note
// that these definitions are highly dependant on hardware and operating
// system. We are using the SED1353 Eval Card in an x86 environment for
// testing.
/*--------------------------------------------------------------------------*/

#define VID_MEM_BASE 0xD0000L  // linear address (32-bit flat mode)
#define VID_IO_BASE  0x310




#ifdef ROTATE_CCW
#define PlotPointView(x, y, c)\
    {\
    UCHAR *_Put = mpScanPointers[x] - (y >> 3);\
    UCHAR _uShift = ( y & 7);\
    UCHAR _uVal = *_Put & ~(0x01 << _uShift);\
    _uVal |= c << _uShift;\
    *_Put = _uVal;\
    }
#else
#define PlotPointView(x, y, c)\
    {\
    UCHAR *_Put = mpScanPointers[x] + (y >> 3);\
    UCHAR _uShift = 7 - (y & 7);\
    UCHAR _uVal = *_Put & ~(0x01 << _uShift);\
    _uVal |= c << _uShift;\
    *_Put = _uVal;\
    }
#endif


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class ProMono1353Screen : public PegScreen
{
    public:
        ProMono1353Screen(PegRect &);

        virtual ~ProMono1353Screen();

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
            UCHAR *_Get = mpScanPointers[x] + (y >> 3);
            UCHAR _uShift = 7 - (y & 7);
            UCHAR _uVal = *_Get;
            _uVal >>= _uShift;
            _uVal &= 1;
            return ((COLORVAL) _uVal);
        }

        #ifdef PEG_IMAGE_SCALING
        COLORVAL GetBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap)
        {
            WORD wBytesPerLine = (pMap->wHeight + 7) >> 3;
            COLORVAL *_pGet = pMap->pStart;
            _pGet += wBytesPerLine * x;
            _pGet += y >> 3;
            UCHAR _uVal = *_pGet;
            UCHAR _uShift = 7 - (y & 7);
            _uVal >>= _uShift;
            _uVal &= 1;
            return ((COLORVAL) _uVal);
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            WORD wBytesPerLine = (pMap->wHeight + 7) >> 3;
            COLORVAL *_pPut = pMap->pStart;
            _pPut += wBytesPerLine * x;
            _pPut += y >> 3;
            UCHAR _uShift = 7 - (y & 7);
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
        UCHAR  muPalette[2 * 3];

        void WriteVidReg(UCHAR uReg, UCHAR uVal);
        UCHAR ReadVidReg(UCHAR uReg);

};

#endif



