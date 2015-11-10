/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// l24scrn.hpp - Linear screen driver for 24-bpp operation.
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
//  This driver can be tested under Win32 by turning on the definition
//  PEGWIN32. You can safely delete all code bracketed by the PEGWIN32
//  definition when moving to your target.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _L24SCREEN_
#define _L24SCREEN_

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 240
#define PEG_VIRTUAL_YSIZE 220

/*--------------------------------------------------------------------------*/
// Pick ONLY ONE of the following two #define. Either put the red byte
// to the lowest (first) address, or put the blue byte to the lowest (first)
// address.
/*--------------------------------------------------------------------------*/
//#define RED_BYTE_FIRST
#define BLUE_BYTE_FIRST

#define RedVal(c) (UCHAR)   ((c) >> 16)
#define GreenVal(c) (UCHAR) (((c) >> 8) & 0xff)
#define BlueVal(c)  (UCHAR) ((c) & 0xff)

/*--------------------------------------------------------------------------*/
#ifdef RED_BYTE_FIRST
#define PlotPointView(x, y, c) \
        { \
        UCHAR *_pPut = (UCHAR *) mpScanPointers[y] + (x * 3); \
        *_pPut++ = RedVal(c); \
        *_pPut++ = GreenVal(c); \
        *_pPut = BlueVal(c); \
        }

#else
#define PlotPointView(x, y, c) \
        { \
        UCHAR *_pPut = (UCHAR *) mpScanPointers[y] + (x * 3); \
        *_pPut++ = BlueVal(c); \
        *_pPut++ = GreenVal(c); \
        *_pPut = RedVal(c); \
        }

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class L24Screen : public PegScreen
{
    public:
       #ifndef PEGWIN32
        L24Screen(PegRect &);
       #else
        L24Screen(HWND hwnd, PegRect &);
        SIGNED mwWinRectXOffset;
        SIGNED mwWinRectYOffset;
       #endif

        virtual ~L24Screen();

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
            COLORVAL cVal;
            UCHAR *pGet = (UCHAR *) mpScanPointers[y] + (x * 3);
            cVal = *pGet++;
            cVal <<= 8;
            cVal |= *pGet++;
            cVal <<= 8;
            cVal |= *pGet;
            return cVal;
        }

        #ifdef PEG_IMAGE_SCALING
        COLORVAL GetBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap)
        {
            COLORVAL cVal;
            UCHAR *pGet = pMap->pStart;
            pGet += pMap->wWidth * y * 3;
            pGet += x * 3;
            cVal = *pGet++;
            cVal <<= 8;
            cVal |= *pGet++;
            cVal <<= 8;
            cVal |= *pGet;
            return cVal;
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
           if (pMap->uBitsPix == 24)
           {
            UCHAR *pPut = pMap->pStart;
            pPut += pMap->wWidth * y * 3;
            pPut += x * 3;

           #ifdef RED_BYTE_FIRST
            *pPut++ = RedVal(cVal);
            *pPut++ = GreenVal(cVal);
            *pPut = BlueVal(cVal);
           #else
            *pPut++ = BlueVal(cVal);
            *pPut++ = GreenVal(cVal);
            *pPut = RedVal(cVal);
           #endif
           }
           else
           {
           }
        }
        #endif

        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

        virtual PegBitmap *CreateBitmap(SIGNED wWidth, SIGNED wHeight,
            BOOL bHasTrans = TRUE);

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

        void DrawTrueColorBitmap(const PegPoint Where,
            const PegBitmap *Getmap, const PegRect View);

        void DrawRlePaletteBitmap(const PegPoint Where, const PegRect View,
            const PegBitmap *Getmap);


       #ifdef DO_OUTLINE_TEXT
        void DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
            PegFont *Font, SIGNED iCount, PegRect &Rect);
       #endif

       #if defined(PEGWIN32) || defined(DOUBLE_BUFFER)
        void MemoryToScreen(void);
       #endif

       UCHAR mPalette[256 * 3];  // palette for drawing 8bpp bitmaps


       #ifdef PEGWIN32
        HWND mHWnd;
        HDC  mHdc;
       #endif

};

#endif



