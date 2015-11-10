/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vgascrn.hpp - Instantiable DOS screen driver for standard VGA mode 12,
//    which is 640x480x16 colors.
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
// This instance of PegScreen is used when building the DOS version of PEG.
// PEG as delivered supports Win32, Win16, and DOS for development purposes.
//
// You are free to use these examples when creating PegScreen classes
// for your own target(s).
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGVGASCRN_
#define _PEGVGASCRN_

/*--------------------------------------------------------------------------*/
//
// Define the screen X-Y resolution.
// This driver only supports generic VGA (640x480).
//
/*--------------------------------------------------------------------------*/

#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480


/*--------------------------------------------------------------------------*/
// 
// The following define, USE_BIOS, instructs this screen driver to use the
// PC BIOS to configure the video controller and set up the color palette.
//
// The USE_BIOS definition can only be used when running on x86 targets in 
// real mode, under DOS or SMX.
//
// If you are running on an x86 in real mode and have a BIOS, this is the
// most reliable method since not all video cards are truly VGA register set
// compatible.
//
// If you are NOT running on an x86 target, or your system does not have
// a BIOS, or if you are running in one of the x86 protected modes,
// you should turn this definition off. When this definition
// is turned off, the screen driver configures the VGA register set directly.
//
// If you have a custom screen driver, this definition has no effect.
//
/*--------------------------------------------------------------------------*/

#define USE_BIOS    // turn this on to use VGA BIOS for configuration

/*--------------------------------------------------------------------------*/
// If you are not using the PC BIOS, but you still want to return to text
// mode when your PEG application terminates, turn on RESTORE_TEXT_ON_EXIT.
// Otherwise, leave this off to save memory.
/*--------------------------------------------------------------------------*/
#ifndef USE_BIOS
#define RESTORE_TEXT_ON_EXIT
#endif

/*--------------------------------------------------------------------------*/
// VGAScreen- derived instantiable screen class. The class below is
// used for 640x480x16 color VGA mode.
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
// If USE_BIOS is defined, the VGA screen driver defaults to using
// the REAL MODE address of the video memory. If USE_BIOS is not defined,
// the VGA screen driver defaults to using the flat-mode linear address.
// You can change this by modifying the pre-processor logic contained in 
// the VGAScreen constructor.
/*--------------------------------------------------------------------------*/

#define VID_MEM_BASE 0xA0000L  // linear address (32-bit flat mode)
#define VID_MEM_SIZE 0x10000L  // in bytes



/*--------------------------------------------------------------------------*/
// The following macro is used to write to the registers on the video
// controller. This may have to be changed depending on the environment.

#define WriteVidReg(index, val) {POUTB(0x3CE, index); POUTB(0x3cf, val);}

/*--------------------------------------------------------------------------*/
// PlotPointView- Write one pixel at x,y, with color c. This function is
// provided by all screen drivers, and is provided as a macro.
/*--------------------------------------------------------------------------*/
extern UCHAR dummy;

#define PlotPointView(x, y, c)\
{\
    if (mbVirtualDraw)\
    {\
        *(mpScanPointers[y] + x) = c;\
    }\
    else\
    {\
        UCHAR _uMask = 0x80 >> (x % 8);\
        UCHAR PEGFAR *_Address = mpScanPointers[y] + (x >> 3);\
        WriteVidReg(8, _uMask);\
        WriteVidReg(5, 2);\
        dummy = *_Address;\
        *_Address = c;\
    }\
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class VGAScreen : public PegScreen
{
    public:
        VGAScreen(PegRect &);
        virtual ~VGAScreen();

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
        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);
        COLORVAL GetPixelView(SIGNED wXPos, SIGNED wYPos);

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

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------


    protected:


        WORD mwBytesPerRow;
        UCHAR PEGFAR *mpVidMem;
        void HidePointer(void);

        //---------------------------------------------------------
        // a few extra things for running in VGA mode without using
        // any BIOS traps:
        //---------------------------------------------------------
       #ifndef USE_BIOS

        void ConfigureVGA(void);

        #ifdef RESTORE_TEXT_ON_EXIT
         void ConfigureMode3(void);
         void SetMode3(void);
         void CopyTextFont(void);
         UCHAR *mpTextModeFont;
        #endif
       #endif

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

        void Draw16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
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

        // functions for drawing to off-screen bitmaps, using linear addressing:

        void LinearVLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
            COLORVAL Color, SIGNED wWidth);
        void LinearHLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
            COLORVAL Color, SIGNED wWidth);
        void LinearDrawText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
            PegFont *Font, SIGNED iCount, PegRect &Rect);
        void LinearRleBitmap(const PegPoint Where, const PegRect View,
            const PegBitmap *Getmap);
        void LinearBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
};

#endif



