/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ct545_8.hpp - Linear screen driver for 8-bpp (256 color) operation with
//  C&T 65545 controller.
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

#ifndef _CT_8SCREEN_
#define _CT_8SCREEN_

/*--------------------------------------------------------------------------*/
// Select the resolution you want to run in-
/*--------------------------------------------------------------------------*/

//#define MODE320         // turn this on for 320x240 mode
//#define MODE640           // turn this on for 640x480 mode
#define MODE800         // turn this on for 800x600 mode

#if defined(MODE320)
#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 240
#elif defined(MODE640)
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480
#elif defined(MODE800)
#define PEG_VIRTUAL_XSIZE 800
#define PEG_VIRTUAL_YSIZE 600
#endif


#define LINEAR_ADDRESSING     // use extended memory or VGA address?
//#define DOUBLE_BUFFER

#ifdef LINEAR_ADDRESSING

// if using LINEAR addressing, we can take advantage of hardware cursor
// and double-buffering for flicker-free drawing, assuming there is 
// extra video memory available:

//#define DOUBLE_BUFFER           		// use double buffering

#if defined(PHARLAP) && defined(PEGTNT)
#define VID_MEM_BASE  0x13C00000L		// linear address (32-bit flat mode)
#else
#define VID_MEM_BASE 0x3c00000L
#endif
#define CURSOR_MEM_BASE (VID_MEM_BASE + (100 * 1024))
//#define HARDWARE_CURSOR

#else  // here if using non-linear VGA addressing:

#define VID_MEM_BASE 0xA0000L   // VGA address (32-bit flat mode)

#endif

// hardware cursor register addresses:

#define HC_COLOR0   0xa7d0
#define HC_COLOR1   0xa7d2
#define HC_XOFFSET  0xafd0
#define HC_YOFFSET  0xafd2
#define HC_ADDR_HI  0xb3d2
#define HC_ADDR_LO  0xb3d0
#define HC_ENABLE   0xa3d0


// BITBLIT register definitions:

#define BB_PITCH_REG 0x83d0
#define BB_CONTROL   0x93d0
#define BB_SRC_REG   0x97d0
#define BB_DEST_REG  0x9bd0
#define BB_COMMAND   0x9fd0
#define BB_STAT_REG  0x93d2

#ifdef DOUBLE_BUFFER
#define VID_MEM_SIZE (256 * 1024)  // use 256 K for two frame buffers
#else
#define VID_MEM_SIZE (128 * 1024)  // use 128 K, just one frame buffer
#endif


/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (UCHAR) c;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class CTLinear8Screen : public PegScreen
{
    public:
       #ifndef PEGWIN32
        CTLinear8Screen(PegRect &);
       #else
        CTLinear8Screen(HWND hwnd, PegRect &);
        SIGNED mwWinRectXOffset;
        SIGNED mwWinRectYOffset;
       #endif

        virtual ~CTLinear8Screen();

        //---------------------------------------------------------
        // Public pure virtual functions in PegScreen:
        //---------------------------------------------------------

        void BeginDraw(PegThing *);
        void BeginDraw(PegThing *, PegBitmap *);
        void EndDraw(void);
        void EndDraw(PegBitmap *);
        void ShowPointer(BOOL bShow);
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

       #ifdef DO_OUTLINE_TEXT
        void DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
            PegFont *Font, SIGNED iCount, PegRect &Rect);
       #endif

       UCHAR  muPalette[256 * 3];

       void MemoryToScreen(void);
};

#endif



