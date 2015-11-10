/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// w32scrn.hpp - PegScreen class definition for running as a Win32 app.
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
//    This version of PegScreen is used for the Win32 development
//    environment. This version implements a 640x480x16 color PegScreen
//    under Win32. This resolution was chosen in order to be compatible
//    with the DOS screen and demo application. Other resolutions and/or
//    color depths could be easily be created using this module as an
//    example.
//
//
// This class defines all of the methods which must be supported in 
// instantiated screen classes. Users may of course add additional members
// in custom derived classes, but only those members found here will be used
// by PEG.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if !defined(_PEGWINSCREEN_)
#define _PEGWINSCREEN_


/*--------------------------------------------------------------------------*/
// In the WIN32 environment, you can configure PEG to maintain
// a fixed virtual screen size (the most common case), or to re-size the
// virtual screen to match the client area of the MS Windows window client
// area.
//
// Most embedded platforms have a fixed screen size, and true emulation
// is best served by turning off SIZE_TO_WIN_CLIENT.
//
// As described in the programming manual, the WIN32 environment can be 
// configured for any screen resolution. The settings below configure PEG
// for VGA emulation. These values can be changed to emulate any target screen
// size.
/*--------------------------------------------------------------------------*/

#define SIZE_TO_WIN_CLIENT

#define DO_OUTLINE_TEXT

/*--------------------------------------------------------------------------*/
// Define the screen X-Y resolution.
/*--------------------------------------------------------------------------*/

#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480

#define USE_VID_MEM_MGR         // for testing purposes only

#define GRAY_PALETTE_INDEX 232  // index for drawing grayscale images (simulation)


#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (UCHAR) c;

typedef struct 
{
    BITMAPINFOHEADER bmhead;
    RGBQUAD  ColorTable[256];
} BMHEAD;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegWinScreen : public PegScreen
{
    public:

        PegWinScreen(HWND hwnd, PegRect &);
        virtual ~PegWinScreen();

        //---------------------------------------------------------
        // Public pure virtual functions in PegScreen:
        //---------------------------------------------------------

        void BeginDraw(PegThing *);
        void BeginDraw(PegThing *, PegBitmap *);
        void EndDraw(void);
        void EndDraw(PegBitmap *);
        void SetPointerType(UCHAR bType);
        void SetPointer(PegPoint) {};
        void Capture(PegCapture *Info, PegRect &Rect);
        COLORVAL GetPixelView(SIGNED x, SIGNED y)
        {
            return ((COLORVAL) *(mpScanPointers[y] + x));
        }
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

        void SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pPal);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

        // a couple of extensions for the MS Windows environment:

        SIGNED mwWinRectXOffset;
        SIGNED mwWinRectYOffset;
        SIGNED mwWinRectMouseXOffset;
        SIGNED mwWinRectMouseYOffset;

        HWND GetHWnd(void) {return mHWnd;}
        /*
        virtual void SetNumColors(DWORD dNumColors, BOOL bGrayscale)
        {
            mdNumColors = dNumColors;
            mbGrayScale = bGrayscale;
        }
        */

       #ifndef SIZE_TO_WIN_CLIENT

        void SetNewWindowsWinSize(WORD wWidth, WORD wHeight)
        {
            mwWinRectXOffset = (wWidth - mwHRes) / 2;
            mwWinRectYOffset = (wHeight - mwVRes) / 2;            
        }

       #else

        void SetNewWindowsWinSize(WORD wWidth, WORD wHeight);

       #endif

    protected:

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
        virtual void HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos);
        virtual void VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos);


        //---------------------------------------------------------
        // End protected pure virtual functions.
        //---------------------------------------------------------

       #ifdef DO_OUTLINE_TEXT

        void DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
            PegFont *Font, SIGNED iCount, PegRect &Rect);

       #endif

        void HidePointer(void) {};
        virtual void MemoryToScreen(void);

        UCHAR  muPalette[256 * 3];
        COLORVAL *mpVidMem;
        HWND mHWnd;
        HPALETTE mhPalette;
        HDC  mHdc;

    private:

        void DrawMonochromeBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void DrawFourGrayBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void Draw16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);

        void Draw16GrayBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);

        void Draw256ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void DrawHiColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void DrawTrueColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
};

#endif

