/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1354scrn.hpp - PegScreen screen driver for Seiko-Epson 1354 video
//      controller using 256 colors in LUT mode.
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
// This driver has several build options. First, the driver is set up to 
// run in either an ISA or direct connect configuration. If running with an
// ISA configuration (like the SDU1354BOC eval card), a few things are 
// required that are not required when running in a direct connect environment
// such as an ARM, 68K, or similar environment. Choose the ISA environment
// by turning on the definition ISA_BOARD in this file.
//
// If running with a custom direct-connect, you also need to set the base
// address for video memory and video register access. These are also defined
// in this header file.
//
//
// This driver can also be configured to use double buffering. Double buffering
// can be done in local memory or in video memory, again controlled with
// #defines.
//
//
// This driver is meant for use with 256 color systems only. The display can
// be LCD, CRT/TV or a combination. If you are using other than 256 colors,
// you should use the driver created for your color depth.
//
// Limitations:
// 
// This driver was developed and tested using the SED1354BOC eval card. It
// has been verified with both a 640x480 CRT and with a 320x240 LCD. All
// other configurations will likely require slight modification of the
// ConfigureController() function (found in the 1354scrn.cpp file).
//
//                  ***     IMPORTANT     ***
//
// Please read the SED1354 documentation to insure that your controller is
// configured correctly for your display before calling Swell Software.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _SED1354SCRN_
#define _SED1354SCRN_


/*--------------------------------------------------------------------------*/
// ISA_BOARD
//
// Turn this on to run with 1354BOC eval card in a PC environment. For all
// other configurations, turn this off.
/*--------------------------------------------------------------------------*/

//#define ISA_BOARD       // On for 1354 eval card, else turn off

/*--------------------------------------------------------------------------*/
// CRT_MODE
//
// The driver currently supports CRT mode or LCD mode. Tested resolutions
// include 640x480x256 CRT and 320x240x256 LCD. Please verify the register
// configuration with your display specifications for all other display types.
/*--------------------------------------------------------------------------*/

//#define CRT_MODE        // On for CRT, Off for LCD

/*--------------------------------------------------------------------------*/
// External RAMDAC support
//
// If an external RAMDAC is being used, the palette programming works
// differently. Also, the PEG definition EIGHT_BIT_PACKED_PIXEL should be
// turned off if an external RAMDAC is being used.
//
// For LCD panales, the normal configuration is to use the internal LUT. In
// this configuration, you should turn off EXTERNAL_RAMDAC and turn on 
// EIGHT_BIT_PACKED_PIXEL in the file \peg\include\peg.hpp (now pconfig.hpp)
//
// The default is to assume you are using an external RAMDAC if running
// with a CRT, else assume you are using the internal LUT. You can change
// this as required to match your system.
/*--------------------------------------------------------------------------*/

#ifdef CRT_MODE

#define EXTERNAL_RAMDAC

#endif


/*--------------------------------------------------------------------------*/
// Set the default screen resolution and color depth
/*--------------------------------------------------------------------------*/

#ifdef CRT_MODE

// default CRT resolution    

#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480

#else

// default LCD resolution

#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 240

#endif

/*--------------------------------------------------------------------------*/
// PCI/Direct connect configuration
/*--------------------------------------------------------------------------*/

#ifdef  ISA_BOARD       // installed in an ISA system (eval card)??

#define VID_REG_BASE 0xc00000L    
#define VID_MEM_BASE 0xe00000L    

#else

//#define VID_MEM_BASE 0x????    // fill in your address here
//#define VID_REG_BASE 0x????    // fill in your address here
#define VID_REG_BASE 0x60000000
#define VID_MEM_BASE 0x60200000

#endif


/*--------------------------------------------------------------------------*/
// VID_MEM_SIZE- how much video memory is installed on your system?
/*--------------------------------------------------------------------------*/

#define VID_MEM_SIZE       0x200000     // default to 2M video RAM

/*--------------------------------------------------------------------------*/
// The following definition, when turned on, allows bitmaps created at
// run time to be placed directly in video memory. This should only be turned
// on if your video memory is >= 2X the size of a single frame buffer.
//
// DOUBLE_BUFFER would not normally be used at the same time as the
// VID_MEM_MANAGER
/*--------------------------------------------------------------------------*/
//#define USE_VID_MEM_MANAGER           // use video memory manager?

/*--------------------------------------------------------------------------*/
// HARDWARE_CURSOR is not supported by 1354 controller, leave off
/*--------------------------------------------------------------------------*/
//#define HARDWARE_CURSOR                 // use hardware cursor?

/*--------------------------------------------------------------------------*/
// DOUBLE_BUFFER provides a nice appearance if the memory is available.
// When this is turned on, PEG does all drawing to a second frame buffer, 
// and then uses the 1356 BITBLT engine to transfer the modified
// portion of video memory to the visible frame buffer.
// 
// DOUBLE_BUFFER would not normally be used at the same time as the
// VID_MEM_MANAGER
/*--------------------------------------------------------------------------*/
//#define DOUBLE_BUFFER                   // use double buffering?

/*--------------------------------------------------------------------------*/
// LOCAL_BUFFERING only has an effect when DOUBLE_BUFFER is turned on. When
// LOCAL_BUFFERING is turned on, the second frame buffer is maintained in
// local memory. This option helps performance when the video memory
// connection is slow, for example in an ISA system.
//
// This may take up quite a bit of local memory. However, if
// LOCAL_BUFFER is turned off and DOUBLE_BUFFER is turned on, you must have
// at least 2X frame buffer size video memory installed or the two
// buffers will overlap creating an ugly display.
/*--------------------------------------------------------------------------*/

#define LOCAL_BUFFERING                 // double buffer locally?

#ifndef DOUBLE_BUFFER
#undef LOCAL_BUFFERING
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//
//                  START OF PEGSCREEN CLASS DEFINITION
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define PlotPointView(x, y, c)\
    *(mpScanPointers[y] + x) = (UCHAR) c;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SED1354Screen : public PegScreen
{
    public:
        SED1354Screen(PegRect &);
        virtual ~SED1354Screen();

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
        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

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
        void ConfigureController(void);
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

        UCHAR *mpVidMemBase;
};

#endif



