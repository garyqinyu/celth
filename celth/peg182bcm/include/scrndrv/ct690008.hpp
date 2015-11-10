/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ct690008.hpp - Linear screen driver for 8-bpp (256 color) operation with
//  C&T 69000 controller.
//
// Author: Kenneth G. Maxwell
//         Jim DeLisle
//
// Copyright (c) 1997-2002 Swell Software 
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
// Select CRT-LCD-BOTH
/*--------------------------------------------------------------------------*/

#define CT_LCD_SUPPORT
//#define CT_CRT_SUPPORT

/*--------------------------------------------------------------------------*/
// Select the resolution you want to run in-
/*--------------------------------------------------------------------------*/

#define MODE320         // turn this on for 320x240 mode
//#define MODE640         // turn this on for 640x480 mode
//#define MODE800         // turn this on for 800x600 mode
//#define MODE1024        // turn this on for 1024x768

#if defined(MODE320)
#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 240
#elif defined(MODE640)
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480
#elif defined(MODE800)
#define PEG_VIRTUAL_XSIZE 800
#define PEG_VIRTUAL_YSIZE 600
#elif defined(MODE1024)
#define PEG_VIRTUAL_XSIZE 1024
#define PEG_VIRTUAL_YSIZE 768
#endif

/*--------------------------------------------------------------------------*/
// FRAME_BUFFER_SIZE- calculated size of one complete frame buffer
/*--------------------------------------------------------------------------*/
#define FRAME_BUFFER_SIZE (((PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE) + 4095) & 0xfffff000)

/*--------------------------------------------------------------------------*/
// VID_MEM_SIZE- size of video memory available in bytes
/*--------------------------------------------------------------------------*/
#define VID_MEM_SIZE 0x200000

/*--------------------------------------------------------------------------*/
// USE_VID_MEM_MANAGER- turn this on to provide run-time video memory
// management. This makes it possible to create dynamic PegBitmaps that
// are in video memory, and display them using hardware BitBlt capability.
// This is useful for Sprite or animation display.
/*--------------------------------------------------------------------------*/
#define USE_VID_MEM_MANAGER

/*--------------------------------------------------------------------------*/
// DOUBLE_BUFFER- Define this only if you have enough memory available for
//    two complete frame buffers
/*--------------------------------------------------------------------------*/
#define DOUBLE_BUFFER

/*--------------------------------------------------------------------------*/
// USE_PCI: Define this on if your controller is plugged into a PCI adapter
//    and you want to "find" the controller at power up. Otherwise, you can
//    simply pre- define the controller address below.
/*--------------------------------------------------------------------------*/
#define USE_PCI 

#if defined(USE_PCI)

// When USE_PCI is defined, we look for the video controller signature using
// PCI bios function calls.

#define PEGPCI_CT69000

#else

#if defined(PEGTNT)
#define VID_MEM_BASE  0x13C00000L		// linear address (32-bit flat mode)
#else
#define VID_MEM_BASE 0x3c00000L     /*** SET FOR YOUR HARDWARE!! ***/
#endif
#endif

/*--------------------------------------------------------------------------*/
// Register offsets from VID_MEM_BASE

#define BITBLT_REGS         0x400000
#define ATTRIB_CONTROLLER_INDEX 0x400780
#define ATTRIB_CONTROLLER_DATA  0x400781
#define SEQUENCER_INDEX     0x400788
#define SEQUENCER_DATA      0x400789
#define PIXEL_MASK_REG      0x40078c
#define PALETTE_INDEX       0x400790
#define PALETTE_DATA        0x400791
#define MISC_OUTPUT_REG     0x400784
#define GRAPHICS_INDEX      0x40079c
#define GRAPHICS_DATA       0x40079d
#define FLAT_PANEL_INDEX    0x4007a0
#define CRTC_REG_INDEX      0x4007a8
#define CRTC_REG_DATA       0x4007a9
#define EXTENSION_REG_INDEX 0x4007ac
#define EXTENSION_REG_DATA  0x4007ad
#define FEATURE_CONTROL_REG 0x4007b4
#define STATUS_REG_INDEX    0x4007b4

/*--------------------------------------------------------------------------*/
// Copy start- Used only for Bit-blit operations. Start with upper-left
// corner or lower-right corner.
/*--------------------------------------------------------------------------*/
#define     CS_UPPER_LEFT   0x0000
#define     CS_LOWER_RIGHT  0x0300


/*--------------------------------------------------------------------------*/
// HARDWARE_CURSOR- Turn this on if you have a mouse for input.
/*--------------------------------------------------------------------------*/
//#define HARDWARE_CURSOR


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
        UCHAR  *mpVidMemBase;
        UCHAR  *mpVidRegBase;

        void WriteExtReg(int index, int val);
        UCHAR ReadExtReg(int index);
        void WaitNotBusy(void);
        void WriteFPReg(int index, int val);
        void WriteCRTCReg(int index, int val);
        int ReadCRTCReg(int index);
        UCHAR ReadSeqReg(int index);
        void WriteSeqReg(int index, int val);
        void WriteAttribReg(int rg, int val);
        void WriteGraphicsReg(int index, int val);

       #ifdef DOUBLE_BUFFER
        void MemoryToScreen(void);
       #endif
};

#endif



