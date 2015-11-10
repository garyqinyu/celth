/*--------------------------------------------------------------------------*/
// lcda400.hpp - Linear screen driver for 16-bpp (65k color) operation. Used
//               with the KEV7A400 EVB and the following Sharp LCDs:
//                         LQ039Q2 (3.9" HR-TFT QVGA display)
//                         LQ121S1 (12.1" TFT SVGA display)
//                         LQ
//
// Copyright 2002 Sharp Microelectronics of the Americas
//
// Notes:
//
//  This screen driver is intended for Sharp LH7A400 SOCs with the
//  LQ121S1 LCD screen. The LCD and driver are configured for the following:
//      64Kcolors (no color palette)
//      800x600 resolution
//      Automatic configuration of the LCD controller registers and timing
//         based on the SOC clock settings
//
//  This driver is based on the PEG video driver template (pscreen).
//
/*--------------------------------------------------------------------------*/

#ifndef _SHARP_LCDA400_
#define _SHARP_LCDA400_

// The pconfig file is pulled in to get the display type
#include "pconfig.hpp"

// Pick only one of the following displays
#define LQ121S1      // 12.1" TFT 800x600 backlit display
//#define LQ104V1      // 10.4" TFT 640x480 backlit display
//#define LQ039Q2      // 3.9" HR-TFT 320x340 frontlit display

// To enable double buffer support, enable the following define. Double
// buffer support will improve display update performance and may reduce
// visible flicker and draw effects on the display by drawing to an off
// screen buffer and then blitting the image to the display
//#define DOUBLE_BUFFER // Double buffering is not presently supported

/*--------------------------------------------------------------------------*/
// Default resolution
/*--------------------------------------------------------------------------*/
#if defined (LQ121S1)
#define PEG_VIRTUAL_XSIZE 800
#define PEG_VIRTUAL_YSIZE 600

#elif defined (LQ104V1)
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480

#elif defined (LQ039Q2)
#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 240

#else
  ** ERROR - Sharp LH7A400 display type not configured in lcda400.hpp **
#endif

/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (COLORVAL) c;

/*--------------------------------------------------------------------------*/
// Class description
/*--------------------------------------------------------------------------*/
class LQ121S1_16 : public PegScreen
{
    public:
        // Constructor - inits display, sets up class data
        LQ121S1_16(PegRect &);

        // Deletes dynamics allocated in constructor, display stays active
        virtual ~LQ121S1_16();

        /*------------------------------------------------------------------*/
        // Mandatory overrided virtual functions in PegScreen
        /*------------------------------------------------------------------*/
        void BeginDraw(PegThing *pThing);
        void EndDraw();
        void BeginDraw(PegThing *Caller, PegBitmap *pMap);
        void EndDraw(PegBitmap *pMap);
        void SetPointerType(UCHAR bType);
        void HidePointer(void);
        void SetPointer(PegPoint Where);
        void Capture(PegCapture *Info, PegRect &Rect);
        void  SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *pGet);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

#ifdef PEG_IMAGE_SCALING
        COLORVAL GetBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap)
        {
            COLORVAL *_pGet = (COLORVAL *) pMap->pStart;
            _pGet += pMap->wWidth * y;
            _pGet += x;
            return *_pGet;
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL c)
        {
            COLORVAL *_pPut = (COLORVAL *) pMap->pStart;
            _pPut += pMap->wWidth * y;
            _pPut += x;
            *_pPut = c;
        }
#endif
        /*------------------------------------------------------------------*/
        // End mandatory virtual function overrides
        /*------------------------------------------------------------------*/

        /*------------------------------------------------------------------*/
        // Optional overrided virtual functions in PegScreen
        /*------------------------------------------------------------------*/
        PegBitmap *CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans = TRUE);

        /*------------------------------------------------------------------*/
        // End optional virtual function overrides
        /*------------------------------------------------------------------*/

        // Updates the pixel clock from the system bus clock, should be
        // called whenever the CPU or bus speed is changed
        unsigned long UpdatePixelClock(void);

    protected:    
        // None

    private:
        /*------------------------------------------------------------------*/
        // Mandatory overridden protected virtual functions
        /*------------------------------------------------------------------*/
        void DrawTextView(PegPoint Put, const PEGCHAR *Text,
            PegColor &Color, PegFont *pFont, SIGNED iLen, PegRect &View);
        void LineView(SIGNED xStart, SIGNED yStart,
            SIGNED xEnd, SIGNED yEnd,  PegRect &View, PegColor Color,
            SIGNED iWidth);
        void BitmapView(const PegPoint Where,
            const PegBitmap *pMap, const PegRect &View);
        void RectMoveView(PegThing *Caller, const PegRect &View,
            const SIGNED xMove, const SIGNED yMove);
        void HorizontalLine(SIGNED xStart, SIGNED xEnd,
            SIGNED y, COLORVAL cColor, SIGNED iWidth);
        void VerticalLine(SIGNED yStart, SIGNED yEnd,
            SIGNED x, COLORVAL cColor, SIGNED iWidth);
        void HorizontalLineXOR(SIGNED xs, SIGNED xe, SIGNED y);
        void VerticalLineXOR(SIGNED ys, SIGNED ye, SIGNED x);
        COLORVAL GetPixelView(SIGNED x, SIGNED y)
        {
            return((COLORVAL) *(mpScanPointers[y] + x));
        }
        /*------------------------------------------------------------------*/
        // End mandatory protected virtual function overrides
        /*------------------------------------------------------------------*/

        // RLE map
        void DrawRleBitmap(const PegPoint Where, const PegRect View,
            const PegBitmap *Getmap);

        // Draw an 8-bit image to the window
        void Draw8BitBitmap(const PegPoint Where, const PegRect View,
            const PegBitmap *Getmap);

        // Local pointer maintainence data
        BOOL mbPointerHidden;
        PegCapture mCapture;
        PegPoint  mLastPointerPos;

        // 8-bit to 16-bit color conversion palette
        COLORVAL  mcHiPalette[256];

#ifdef DOUBLE_BUFFER
        void MemoryToScreen (void);
#endif

        // Returns a pointer to the frame buffer used for the display
        UCHAR *GetVideoAddress(void);

        // Returns the optimal pixel clock divider for the LCD controller
        unsigned long lcd_get_divider (void);

        // Returns the speed in MHZ of the bus clock (HCLK)
        unsigned long csc_get_hclk (void);

        // Used by constructor to initialize the LCD controller
        void ConfigureController(void);
};

#endif
