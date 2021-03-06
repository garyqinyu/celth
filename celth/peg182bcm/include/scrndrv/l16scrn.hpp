/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// l16scrn.hpp - Linear screen driver for 16-bpp (65k color) operation.
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
//  This screen driver is intended for use with 256-color systems that support
//  a linear frame buffer. Example controllers include the SMOS 135xx series
//  and the MPC823.
//
//  This driver can be tested under Win32 by turning on the definition
//  PEGWIN32. You can safely delete all code bracketed by the PEGWIN32
//  definition when moving to your target.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _L16SCREEN_
#define _L16SCREEN_

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480


/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (COLORVAL) c;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class L16Screen : public PegScreen
{
    public:
       #ifndef PEGWIN32
        L16Screen(PegRect &);
       #else
        L16Screen(HWND hwnd, PegRect &);
        SIGNED mwWinRectXOffset;
        SIGNED mwWinRectYOffset;
       #endif

        virtual ~L16Screen();

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
            return((COLORVAL) *(mpScanPointers[y] + x));
        }

        #ifdef PEG_IMAGE_SCALING

        COLORVAL GetBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap)
        {
            COLORVAL *pGet = (COLORVAL *) pMap->pStart;
            pGet += pMap->wWidth * y;
            pGet += x;
            return *pGet;
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            COLORVAL *pPut = (COLORVAL *) pMap->pStart;
            pPut += pMap->wWidth * y;
            pPut += x;
            *pPut = cVal;
        }
        #endif

        PegBitmap *CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans);

        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

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

        void Draw8BitBitmap(const PegPoint Where, const PegRect View,
            const PegBitmap *Getmap);

        COLORVAL  mcHiPalette[256];

       #ifdef PEGWIN32

        void MemoryToScreen(void);
        HWND mHWnd;
        HDC  mHdc;
       #endif
};

#endif



