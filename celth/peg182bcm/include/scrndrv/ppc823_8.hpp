/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ppc823_8.hpp - MPC823 screen driver for 8-bpp color TFT (256 color)
// operation.
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

#ifndef _MPC823SCRN_
#define _MPC823SCRN_


/*--------------------------------------------------------------------------*/
// Pick your panel type here. If your panel is not shown, don't despair!
// There are only a few things that change in the controller configuration
// depending on panel type. Simply read the user's manual, chapter 16, and
// program the registers to match your panel specs. The default values
// below will get you close for most panels.
//
/*--------------------------------------------------------------------------*/

//#define PANEL_TYPE_NEC_65BLMO   // VGA TFT Color on Embedded Planet IceBox

//#define PANEL_TYPE_SHARP_LQ104V // VGA TFT Color

//#define PANEL_TYPE_SHARP_LM057QC // QVGA passive color

#define PANEL_TYPE_SHARP_LQ64D  // QVGA TFT color


/*--------------------------------------------------------------------------*/
#if defined(PANEL_TYPE_NEC_65BLMO)

#define PEG_VIRTUAL_XSIZE    640
#define PEG_VIRTUAL_YSIZE    480
#define PANEL_TYPE_TFT         1     
#define LCD_DATA_POLARITY      0    // Usually 1 for Sharp, 0 for NEC
#define LCD_HORIZONTAL_WAIT 0x90    // Wait Between Lines, in shift clocks.
#define LCD_VERTICAL_WAIT   0x21    // Wait between frames, in line scan

#elif defined(PANEL_TYPE_SHARP_LQ104V)

#define PEG_VIRTUAL_XSIZE    640
#define PEG_VIRTUAL_YSIZE    481
#define PANEL_TYPE_TFT         1 
#define LCD_DATA_POLARITY      1    // Usually 1 for Sharp, 0 for NEC
#define LCD_HORIZONTAL_WAIT 0x90    // Wait Between Lines, in shift clocks.
#define LCD_VERTICAL_WAIT   0x20    // Wait between frames, in line scan

#elif defined(PANEL_TYPE_SHARP_LM057QC)

#define PEG_VIRTUAL_XSIZE    320
#define PEG_VIRTUAL_YSIZE    240
#define PANEL_TYPE_TFT         0 
#define LCD_DATA_POLARITY      0    // High for this panel
#define LCD_HORIZONTAL_WAIT 0x50    // Wait Between Lines, in shift clocks.
#define LCD_VERTICAL_WAIT   0x10    // Wait between frames, in line scan

#elif defined(PANEL_TYPE_SHARP_LQ64D)

#define PEG_VIRTUAL_XSIZE    640
#define PEG_VIRTUAL_YSIZE    480
#define PANEL_TYPE_TFT         1 
#define LCD_DATA_POLARITY      0    // Usually 1 for Sharp, 0 for NEC
#define LCD_HORIZONTAL_WAIT 0x63    // Wait Between Lines, in shift clocks.
#define LCD_VERTICAL_WAIT   0x1E    // Wait between frames, in line scan

#else

/*--------------------------------------------------------------------------*/
//
// Note: If your panel is not pre-defined above, you can modify the
// default definition below to work with your panel.
//
// Default: VGA resolution, TFT, color panel
//
/*--------------------------------------------------------------------------*/

#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480

#define PANEL_TYPE_TFT     1     
#define LCD_DATA_POLARITY  1        // Usually 1 for Sharp, 0 for NEC

#define LCD_HORIZONTAL_WAIT 0x90    // Wait Between Lines, in shift clocks.
                                    // Make this bigger/smaller to move
                                    // the image right/left


#define LCD_VERTICAL_WAIT   0x21    // Wait between frames, in line scan
                                    // periods. May be adjusted for vertical
                                    // centering.

#endif          // End of pre-defined panel definitions

#ifdef PANEL_TYPE_TFT
#define VSYNCH_PULSE_WIDTH  0x04    // vertical sych pulse width, TFT only
#else
#define VSYNCH_PULSE_WIDTH  0
#endif



/*--------------------------------------------------------------------------*/
// Double-Buffering: Not used for most PPC applications, but available
/*--------------------------------------------------------------------------*/

//#define DOUBLE_BUFFER     // turn on to use double buffering

/*--------------------------------------------------------------------------*/
//     NO FURTHER USER-LEVEL OPTIONS BEYOND THIS POINT
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
// On-Board 823 Register Offsets from IMMR:


#define LCD_CONFIG      0x840
#define LCD_HORZ_CTRL   0x844
#define LCD_VERT_CONFIG 0x848
#define LCD_BUFFER_A    0x850
#define LCD_BUFFER_B    0x854
#define LCD_STATUS      0x858
#define COLOR_RAM       0xe00

/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (UCHAR) c;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class MPC823Screen : public PegScreen
{
    public:
        MPC823Screen(PegRect &);

        virtual ~MPC823Screen();

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
            COLORVAL *pGet = pMap->pStart;
            pGet += pMap->wWidth * y;
            pGet += x;
            return *pGet;
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            COLORVAL *pPut = pMap->pStart;
            pPut += pMap->wWidth * y;
            pPut += x;
            *pPut = cVal;
        }
        #endif

        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

        PegPoint GetLastPointerPos(void) {return mLastPointerPos;}

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

       #ifdef DOUBLE_BUFFER
        void MemoryToScreen(void);
       #endif

        UCHAR  muPalette[256 * 3];
};

#endif



