/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ct69_24.hpp - Linear screen driver for the CT69000 at 24-bpp operation.
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
// The CT69000 supports up to 800x600 resolution at 24bpp
//
// Due to size limitations of the on board video memory (2MB), the
// hardware can not support double buffering at 800x600 resolution, where
// each buffer takes up approximately 1.4MB of memory.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _CT6924SCREEN_
#define _CT6924SCREEN_

/*--------------------------------------------------------------------------*/
// Select CRT-LCD-BOTH
/*--------------------------------------------------------------------------*/

#define CT_LCD_SUPPORT
//#define CT_CRT_SUPPORT

/*--------------------------------------------------------------------------*/
// Select the resolution you want to run in-
/*--------------------------------------------------------------------------*/

//#define MODE640         // turn this on for 640x480 mode
#define MODE800         // turn this on for 800x600 mode

#if defined(MODE640)
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480
#elif defined(MODE800)
#define PEG_VIRTUAL_XSIZE 800
#define PEG_VIRTUAL_YSIZE 600
#endif

/*--------------------------------------------------------------------------*/
// USE_PCI: Define this on if your controller is plugged into a PCI adapter
//    and you want to "find" the controller at power up. Otherwise, you can
//    simply pre- define the controller address below.
/*--------------------------------------------------------------------------*/
#define USE_PCI 

/*--------------------------------------------------------------------------*/
// FRAME_BUFFER_SIZE- calculated size of one complete frame buffer
/*--------------------------------------------------------------------------*/
#define FRAME_BUFFER_SIZE (((PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE * 3) + 4095) & 0xfffff000)

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
// two complete frame buffers. (Only available when running at 640x480
// resolution due to the size of the on board video memory).
/*--------------------------------------------------------------------------*/
//#define DOUBLE_BUFFER

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#if defined(USE_PCI)

// When USE_PCI is defined, we look for the video controller signature using
// PCI bios function calls.
//#define PCI_CLASS       0x03
//#define CHIPS_VENDOR_ID   0x102c
//#define CHIPS_DEVICE_ID   0x00c0

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
/*--------------------------------------------------------------------------*/
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
#define HARDWARE_CURSOR

/*--------------------------------------------------------------------------*/
// Pick ONLY ONE of the following two #define. Either put the red byte
// to the lowest (first) address, or put the blue byte to the lowest (first)
// address.
/*--------------------------------------------------------------------------*/
//#define RED_BYTE_FIRST
#define BLUE_BYTE_FIRST

#define RedVal(c) (UCHAR)   (c >> 16)
#define GreenVal(c) (UCHAR) ((c >> 8) & 0xff)
#define BlueVal(c)  (UCHAR) (c & 0xff)

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
// If 24bpp bitmaps look like BGR instead of RGB, define the following
/*--------------------------------------------------------------------------*/
#define SWAP_24BPP_IMAGE

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class CT6924Screen : public PegScreen
{
    public:
        CT6924Screen(PegRect &);

        virtual ~CT6924Screen();

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

        virtual PegBitmap *CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans);

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

       #ifdef DO_OUTLINE_TEXT
        void DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
            PegFont *Font, SIGNED iCount, PegRect &Rect);
       #endif

       #if defined(DOUBLE_BUFFER)
        void MemoryToScreen(void);
       #endif

        UCHAR*  mpVidMemBase;
        UCHAR*  mpVidRegBase;

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
};

#endif  // _CT6924SCREEN_



