/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// cl54xpci.hpp - Instantiable DOS screen driver for Cirrus Logic video
//    controller using PCI bus video memory access
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
// This driver can easily be adapted to any system using a Cirrus Logic
// 54xx video controller, including non-x86 platforms. As long the the
// video memory is directly accessable to the driver, without paging,
// this driver can be used.
//
// You are free to use these examples when creating PegScreen classes
// for your own target(s).
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _CL54PCI_
#define _CL54PCI_


/*--------------------------------------------------------------------------*/
// Select the resolution you want to run in-
/*--------------------------------------------------------------------------*/
#define MODE640         // turn this on for 640x480 mode
//#define MODE1024        // turn this on for 1024x768

#if defined(MODE640)
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480
#elif defined(MODE1024)
#define PEG_VIRTUAL_XSIZE 1024
#define PEG_VIRTUAL_YSIZE 768
#endif

/*--------------------------------------------------------------------------*/
// VID_MEM_SIZE- how much video memory is installed on your system?
/*--------------------------------------------------------------------------*/
#define VID_MEM_SIZE       0x100000     // default to 1M video RAM

/*--------------------------------------------------------------------------*/
// The following definition, when turned on, allows bitmaps created at
// run time to be placed directly in video memory. This is not support
// in this driver, and USE_VID_MEM_MANAGER should be turned off
/*--------------------------------------------------------------------------*/
//#define USE_VID_MEM_MANAGER           // use video memory manager?

/*--------------------------------------------------------------------------*/
// HARDWARE_CURSOR should be turned on
/*--------------------------------------------------------------------------*/
#define HARDWARE_CURSOR                 // use hardware cursor?

/*--------------------------------------------------------------------------*/
// DOUBLE_BUFFER provides a nice appearance if the memory is available.
// When this is turned on, PEG does all drawing to a second frame buffer, 
// and then uses the 5430/40 BITBLT engine to transfer the modified
// portion of video memory to the visible frame buffer.
/*--------------------------------------------------------------------------*/
//#define DOUBLE_BUFFER                   // use double buffering?

/*--------------------------------------------------------------------------*/
// LOCAL_BUFFERING only has an effect when DOUBLE_BUFFER is turned on. When
// LOCAL_BUFFERING is turned on, the second frame buffer is maintained in
// local memory. This takes up quite a bit of local memory. However, if
// LOCAL_BUFFER is turned off, you must have at least 2X frame buffer size +
// 16K video memory installed, or the two buffers will overlap creating
// an ugly display.
/*--------------------------------------------------------------------------*/
#define LOCAL_BUFFERING                 // double buffer locally?


#ifndef DOUBLE_BUFFER
#undef LOCAL_BUFFERING
#endif

/*--------------------------------------------------------------------------*/
// USE_PCI: Define this on if your controller is plugged into a PCI adapter
//    and you want to "find" the controller at power up. Otherwise, you can
//    simply pre- define the controller address below.
/*--------------------------------------------------------------------------*/
#define USE_PCI 

#if defined(USE_PCI)

#define PEGPCI_CL54XX

#else

#if defined(PEGTNT)
#define VID_MEM_BASE  0x13C00000L		// linear address (32-bit flat mode)
#else
#define VID_MEM_BASE 0x3c00000L     /*** SET FOR YOUR HARDWARE!! ***/
#endif
#endif

/*--------------------------------------------------------------------------*/
#if defined(_MSC_VER)
#define WriteVidReg(index, val) { _outp(0x3CE, index); _outp(0x3cf, val);}
#define ReadVidReg(index) _inp(index)
#else
#define WriteVidReg(index, val) { outp(0x3CE, index); outp(0x3cf, val);}
#define ReadVidReg(index) inp(index)
#endif

/*--------------------------------------------------------------------------*/

#define PlotPointView(x, y, c)\
    *(mpScanPointers[y] + x) = (UCHAR) c;



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SVGAScreen : public PegScreen
{
    public:
        SVGAScreen(PegRect &);
        virtual ~SVGAScreen();

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
        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

        COLORVAL GetPixel(SIGNED wXPos, SIGNED wYPos);



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

    protected:

        void HidePointer(void);
        void ConfigureSVGA(void);
        void Configure1024_768_256(void);
        void Configure640_480_256(void);
        UCHAR PEGFAR *GetVideoAddress(void);
        UCHAR mPalette[256*3];

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

        DWORD *mpVidMemBase;

};

#endif



