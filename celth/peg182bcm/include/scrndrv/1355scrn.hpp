/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1355scrn.hpp - Linear screen driver for 8-bpp (256 color) operation using
//  SMOS SED1355 ISA video card.
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
//  This screen driver is intended for use with 16-color systems that support
//  a linear frame buffer. Example controllers include the SMOS 135xx series
//  and the MPC823.
//
//  This driver can be tested under Win32 by turning on the definition
//  PEGWIN32. You can safely delete all code bracketed by the PEGWIN32
//  definition when moving to your target.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _SED1355Screen_
#define _SED1355Screen_

/*--------------------------------------------------------------------------*/
// Turn this on if you are using the SDU1355BOC eval card:
/*--------------------------------------------------------------------------*/
#define PC_EVAL_CARD

/*--------------------------------------------------------------------------*/
// Default Screen Size and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480

/*--------------------------------------------------------------------------*/
// DOUBLE_BUFFER: turn this on if you want to draw locally and transfer
// modified portion to video frame buffer
/*--------------------------------------------------------------------------*/
#define DOUBLE_BUFFER


/*--------------------------------------------------------------------------*/
// Miscellaneous Addresses for eval card:

#define VID_MEM_BASE 0xc00000L  // linear address of frame buffer
#define VID_REG_BASE 0xe00000L  // linear address of config regs

#ifdef PC_EVAL_CARD
#define EVAL_CFG_REG 0xf80000L  // Register on 1355BOC to enable 16-bit ISA
#define EVAL_SUSPEND 0xf00000L  // write to disable hardware suspend
#endif


/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (UCHAR) c;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SED1355Screen : public PegScreen
{
    public:
        SED1355Screen(PegRect &, BOOL bDual = TRUE);

        virtual ~SED1355Screen();

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

        /* Functions added for this controller only */

        void DisplayPowerOn(void);
        void DisplayPowerOff(void);

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

        void WriteVidReg(UCHAR uReg, UCHAR uVal);
        UCHAR  muPalette[256 * 3];
        UCHAR  *mpVidMemBase;
        BOOL mbDualMode;

       #ifdef DOUBLE_BUFFER
        void MemoryToScreen(void);
       #endif
};

#endif



