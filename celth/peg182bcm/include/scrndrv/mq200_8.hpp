/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// mq200_8.hpp - PegScreen screen driver for MediaQ 200 video
//      controller using 256 colors (8 bpp).
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
// run in either a PCI or direct connect configuration. If running with a
// PCI configuration, we find the card using PCI protocol. If a direct
// connect (ColdFire, 68K, ARM, other), you must define the register and
// memory addresses.
//
// This driver can also be configured to use double buffering, hardware cursor,
// etc. Please read the comments above each configuration flag.
//
// This driver is meant for use with 256 color systems only. The display can
// be LCD, CRT/TV or a combination. If you are using other than 256 colors,
// you should use the driver created for your color depth.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _MQ200SCRN_
#define _MQ200SCRN_


/*--------------------------------------------------------------------------*/
// Set the default screen resolution for the CRT or LCD
/*--------------------------------------------------------------------------*/

//#define PEG_VIRTUAL_XSIZE 320
//#define PEG_VIRTUAL_YSIZE 240

//#define PEG_VIRTUAL_XSIZE 640
//#define PEG_VIRTUAL_YSIZE 480

//#define PEG_VIRTUAL_XSIZE 800
//#define PEG_VIRTUAL_YSIZE 600

#define PEG_VIRTUAL_XSIZE 1024
#define PEG_VIRTUAL_YSIZE 768

/*--------------------------------------------------------------------------*/
// PCI/Direct connect configuration
/*--------------------------------------------------------------------------*/

#define USE_PCI         // installed in a PCI bus??

#ifdef USE_PCI

#define PEGPCI_MQ200

#else

// In any configuration other than PCI eval card, define the register
// and memory addresses right here:

#define VID_MEM_BASE 0xc00000L    // fill in your address here
#define VID_REG_BASE 0x800000L    // fill in your address here

#endif


/*--------------------------------------------------------------------------*/
// VID_MEM_SIZE- how much video memory is installed on your system?
/*--------------------------------------------------------------------------*/
#define VID_MEM_SIZE       0x200000L     // default to 2M video RAM

/*--------------------------------------------------------------------------*/
// HARDWARE_CURSOR should be turned on
/*--------------------------------------------------------------------------*/
#define HARDWARE_CURSOR                 // use hardware cursor?

/*--------------------------------------------------------------------------*/
// LCD_ACTIVE- Turn this on if you want to use GC2 for LCD
/*--------------------------------------------------------------------------*/

//#define LCD_ACTIVE

#ifdef LCD_ACTIVE
#define LCD_PANEL_TYPE  1 // choose panel type from list in the driver
#endif

/*--------------------------------------------------------------------------*/
// CRT_ACTIVE- Turn this on if you want to use GC1 for CRT
/*--------------------------------------------------------------------------*/

#define CRT_ACTIVE

#ifdef CRT_ACTIVE
#define CRT_FREQUENCY 75        // CRT  refresh rate, HZ, must match driver
                                // table entry for x, y, and refresh rate.
#endif

/*--------------------------------------------------------------------------*/
// DOUBLE_BUFFER provides a nice appearance if the memory is available.
// When this is turned on, PEG does all drawing to a second frame buffer, 
// and then uses the BITBLT engine to transfer the modified
// portion of video memory to the visible frame buffer.
/*--------------------------------------------------------------------------*/
//#define DOUBLE_BUFFER                   // use double buffering?


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
class MQ200Screen : public PegScreen
{
    public:
        MQ200Screen(PegRect &);
        virtual ~MQ200Screen();

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

//        PegBitmap *CreateBitmap(SIGNED wWidth, SIGNED wHeight);

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

       #ifdef LCD_ACTIVE
        void SetupLCD(FP_CONTROL_STRUC *pParams);
       #endif

       #ifdef CRT_ACTIVE
        void SetupCRT(void);
       #endif

        void Delay(SIGNED iMs);
        void CheckIfState(SIGNED iState);
        void WaitEngineFIFO(SIGNED iNum);


       #ifdef HARDWARE_CURSOR
        void ProgramCursorBitmap(PegBitmap *pMap, UCHAR *pPut);
       #endif

        UCHAR *mpVidMemBase;
        UCHAR *mpVidRegBase;
};

#endif



