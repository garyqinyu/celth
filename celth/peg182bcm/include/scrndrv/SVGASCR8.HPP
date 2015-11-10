/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// svgascr8.hpp - Generic 8 bpp (256-color) SVGA screen driver.
//
// Author: Jeremy M. Dalmer (from l8scrn)
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
//  This screen driver is intended for use in 80386-compatible systems
//  running in 32-bit flat protected mode.  VESA BIOS Extension 2.0 and a
//  linear frame buffer must be present.
//
//  WORD must be a 16-bit quantity and DWORD must be a 32-bit quantity.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _GENERIC_SVGA_SCREEN_
#define _GENERIC_SVGA_SCREEN_

#ifdef DPMI_API_DJGPP
# include <dpmi.h>
#endif

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480

/*--------------------------------------------------------------------------*/
// Enable double buffering.  Available video memory must be at least two
// pages, unless LOCAL_BUFFERING is also enabled.
/*--------------------------------------------------------------------------*/
#define DOUBLE_BUFFER

/*--------------------------------------------------------------------------*/
// Double buffer locally.  DOUBLE_BUFFER must also be enabled.
/*--------------------------------------------------------------------------*/
#define LOCAL_BUFFERING

/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (UCHAR) c;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class GenericSvgaScreen : public PegScreen
{
	public:
		GenericSvgaScreen(PegRect &);
        virtual ~GenericSvgaScreen();

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
            COLORVAL *_pGet = pMap->pStart;
            _pGet += pMap->wWidth * y;
            _pGet += x;
            return *_pGet;
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            COLORVAL *_pPut = pMap->pStart;
            _pPut += pMap->wWidth * y;
            _pPut += x;
            *_pPut = cVal;
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

	   #ifdef DOUBLE_BUFFER
		void MemoryToScreen();
	   # ifdef LOCAL_BUFFERING
		UCHAR* mpTargetMem;
	   # endif
	   #endif

        //---------------------------------------------------------
        // VBE and protected mode stuff.
        //---------------------------------------------------------

		// Allocates conventional memory for the VBE mode info.
		// Ascertains the VBE mode info conventional memory address.
		// Invokes VBE function 01h to return the VBE mode information.
		// Ascertains the frame buffer device memory physical address.
		// Deallocates the VBE mode info conventional memory.
		// Allocates a mapping from the frame buffer device memory physical address to the frame buffer device memory address.
		// Returns a near pointer to the frame buffer device memory.
		UCHAR* GetFrameBufferAddress( int frameBufferBytes );

		// Deallocates the mapping from the frame buffer device memory physical address to the frame buffer device memory address.
		void DeallocateFrameBufferMapping();

		// Allocates conventional memory for the palette update.
		void AllocatePaletteUpdateMemory( int paletteUpdateEntries );

		// Ascertains the palette update conventional memory address.
		// Invokes VBE function 09h to set the palette data.
		// Deallocates the palette update conventional memory.
		void UpdatePalette( WORD iFirst, WORD iCount );

		// Invokes VBE function 02h to set the VBE mode.
		void SetMode();

		// Invokes VBE function 08h to set the DAC palette format.
		void SetPaletteFormat();

		WORD mMode;	// VBE mode number

	   #ifdef DPMI_API_DJGPP
		// The mapping from the frame buffer device memory physical address to the frame buffer device memory address.
		__dpmi_meminfo mFrameBufferDevMeminfo;

		// The palette update conventional memory selector;
		int mPaletteUpdateConvSelector;
	   #endif
};

#endif
