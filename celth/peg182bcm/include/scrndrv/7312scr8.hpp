/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 7312scr8.hpp - 8 bpp screen driver for the Cirrus Logic EP7312 as part 
//                of the Cogent CSB238 development board attached to a
//                Optrex DMG-50944 STFN color panel.
//
// Author: Kenneth G. Maxwell
//         Jim DeLisle
//
// Copyright (c) 1997-2001 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// This driver takes the high road and uses 8bpp for all of the drawing
// operations, then, using double buffering, transfers the 8bpp scratch
// buffer to the 12bpp visible frame buffer by expanding the 8bpp in the
// scratch buffer into the usable 12bpp frame buffer used by the actual
// video hardware.
//
// It seems to be more efficient to do the boundary calculations only once,
// when the buffer is transferred, than for every drawing operation.
//
// This also has the benefit of forcing the use of double buffering. Given
// the amount of RAM in the development board (16MB), the extra buffer
// shouldn't be an issue.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _7312SCREEN8_
#define _7312SCREEN8_

/*--------------------------------------------------------------------------*/
// Defines for running with the Macraigor Wiggler or using the CodeGen
// SmartFirmware. NOTE: Only define 1 of these.
/*--------------------------------------------------------------------------*/
//#define RUNTIME_WIGGLER     1
#define RUNTIME_SF          1

/*--------------------------------------------------------------------------*/
// Defines for the base address of the video buffer depending on what
// mode we're running in.
//
// When running with the Wiggler, we map the video address to physical 
// memory and put the double buffer start at the end of the hardware
// buffer. NOTE: The placement of the double buffer assumes a 320x240x3/2
// video buffer size. If the size of the video buffer should change, the
// start address of the double buffer would have to be moved to prevent
// overlapping data segments.
//
// When running with SmartFirmware, 0xC0000000 is mapped to 0xF7000000,
// and there doesn't seem to be enough room for the double buffer to
// tag along at that location, so we put the double buffer in the
// data segment.
/*--------------------------------------------------------------------------*/
#if defined(RUNTIME_WIGGLER)
#define VIDEO_BUFFER_START  0xC0000000
#define DOUBLE_BUFFER_START 0xC001C200
#else
#define VIDEO_BUFFER_START  0xF7000000
#define DOUBLE_BUFFER_START NULL
#endif

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 240

/*--------------------------------------------------------------------------*/
// Defines for LCD registers
/*--------------------------------------------------------------------------*/
#define LCD_LCDCON      0x800002C0
#define LCD_PALLSW      0x80000580
#define LCD_PALMSW      0x80000540
#define LCD_FBADDR      0x80001000
#define LCD_SYSCON1     0x80000100

/*--------------------------------------------------------------------------*/
// Constant values for setting the LCD_LCDCON register
/*--------------------------------------------------------------------------*/
#define LCD_GSMD2       0x01
#define LCD_GSMD1       0x01
#define LCD_CLOCKRATE   0x02328000      // 36.864 MHz clock rate

/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (UCHAR) c;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class EP7312Screen8 : public PegScreen
{
    public:
        EP7312Screen8(PegRect &);
        virtual ~EP7312Screen8();

        //---------------------------------------------------------
        // Public pure virtual functions in PegScreen:
        //---------------------------------------------------------

        void BeginDraw(PegThing *);
        void BeginDraw(PegThing *, PegBitmap *);
        void EndDraw(void);
        void EndDraw(PegBitmap *);
        void SetPointerType(UCHAR bType);
        void HidePointer(void);
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

    protected:
        
        BOOL mbPointerHidden;
        PegCapture mCapture;
        PegPoint  mLastPointerPos;
    
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

       #ifdef DO_OUTLINE_TEXT
        void DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
            PegFont *Font, SIGNED iCount, PegRect &Rect);
       #endif

        UCHAR  muPalette[256 * 3];

        void MemoryToScreen(void);
};

#endif  // _7312SCREEN8_



