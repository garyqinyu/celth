/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 13a0scr8.hpp - PegScreen screen driver for Seiko-Epson S1D13A03/4/5
//      controllers using 256 colors.
//
// Author: Jim DeLisle
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
// This driver has several build options. First, the driver is set up to 
// run in either a PCI or direct connect configuration. If running with a
// PCI configuration (like the S5U13A04BOOB/BOOC eval card), we find the card
// using PCI protocol. If a direct connect (ColdFire, 68K, ARM, other),
// you must define the register and memory addresses.
//
// This driver can also be configured to use double buffering, hardware cursor,
// etc. Please read the comments above each configuration flag.
//
// This driver is meant for use with 256 color systems only. The display can
// be LCD only. If you are using other than 256 colors,
// you should use the driver created for your color depth.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _S1D13A0SCRN_
#define _S1D13A0SCRN_


/*--------------------------------------------------------------------------*/
// Set the default screen resolution
/*--------------------------------------------------------------------------*/
#define MODE320
//#define MODE640
//#define MODE800

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


/*--------------------------------------------------------------------------*/
// PCI/Direct connect configuration
/*--------------------------------------------------------------------------*/

//#define USE_PCI         // installed on a PCI bus??

#ifdef USE_PCI

#define PEGPCI_S1D13A04

#else

#define VID_MEM_BASE 0xc00000L    // fill in your address here
#define VID_REG_BASE 0x800000L    // fill in your address here
#define BITBLT_REG_BASE 0x800000L // fill in your address here

#endif


/*--------------------------------------------------------------------------*/
// MEMCPY_NOT_SUPPORTED-
//
// This definition tells the screen driver not to use the (fast) memcpy
// function, but to instead use the (slow) byte-loop operation to move
// video memory blocks.
/*--------------------------------------------------------------------------*/
//#define MEMCPY_NOT_SUPPORTED

/*--------------------------------------------------------------------------*/
// VID_MEM_SIZE- how much video memory is installed on your system?
/*--------------------------------------------------------------------------*/
#define VID_MEM_SIZE       0x28000L     // 160KB of onboard video memory

/*--------------------------------------------------------------------------*/
// DOUBLE_BUFFER provides a nice appearance if the memory is available.
// When this is turned on, PEG does all drawing to a second frame buffer, 
// and then uses the 13A04 BITBLT engine to transfer the modified
// portion of video memory to the visible frame buffer.
// 
// DOUBLE_BUFFER would not normally be used at the same time as the
// VID_MEM_MANAGER
/*--------------------------------------------------------------------------*/
#define DOUBLE_BUFFER                   // use double buffering?

/*--------------------------------------------------------------------------*/
// LOCAL_BUFFERING only has an effect when DOUBLE_BUFFER is turned on. When
// LOCAL_BUFFERING is turned on, the second frame buffer is maintained in
// local memory. This option helps performance when the video memory
// connection is very slow, for example in an ISA system.
//
// This may take up quite a bit of local memory. However, if
// LOCAL_BUFFER is turned off and DOUBLE_BUFFER is turned on, you must have
// at least 2X frame buffer size + 16K video memory installed or the two
// buffers will overlap creating an ugly display.
/*--------------------------------------------------------------------------*/
//#define LOCAL_BUFFERING                 // double buffer locally?

#ifndef DOUBLE_BUFFER
#undef LOCAL_BUFFERING
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// S1D13A04 BITBLT Register Offsets:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define REG_BB_CONTROL              0x8000
#define REG_BB_STATUS               0x8004
#define REG_BB_COMMAND              0x8008
#define REG_BB_SRC_START_ADDR       0x800c
#define REG_BB_DEST_START_ADDR      0x8010
#define REG_BB_MEM_ADDR_OFFSET      0x8014
#define REG_BB_WIDTH                0x8018
#define REG_BB_HEIGHT               0x801c
#define REG_BB_BG_COLOR             0x8020
#define REG_BB_FG_COLOR             0x8024
#define REG_BB_DATA                 0x10000

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
class S1D13A04Screen : public PegScreen
{
    public:
        S1D13A04Screen(PegRect &);
        virtual ~S1D13A04Screen();

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
        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

        PegBitmap *CreateBitmap(SIGNED wWidth, SIGNED wHeight);

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

        void ConfigureController(void);
        void WriteVidReg(UCHAR ucIndex, DWORD dwValue);
        void WriteBlitReg(WORD wIndex, DWORD dwValue);
        UCHAR PEGFAR *GetVideoAddress(void);

        UCHAR mPalette[256*3];
        BOOL mbPointerHidden;
        PegPoint  mLastPointerPos;
        PegCapture mCapture;


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
        DWORD *mpVidRegBase;
        DWORD *mpBitBlitRegs;
};

#endif // _S1D13A0SCRN_

/*--------------------------------------------------------------------------*/
/*
$Workfile: 13a0scr8.hpp $
$Author: Ken $
$Date: 11/20/:2 12:42p $
$Revision: 3 $
$Log: /peg/include/13a0scr8.hpp $
// 
// 3     11/20/:2 12:42p Ken
// Changed GRAYSCALE definition
// 
// 2     11/19/:2 3:09p Jarret
// 
// 1     8/15/:2 4:25p Jim
// Initial checkin
*/
/*--------------------------------------------------------------------------*/

// End of file



