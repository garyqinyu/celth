/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 8106scrn.hpp - Instantiable screen driver for 320x240 LCD using SED8106
//  controller.
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

#ifndef _8106_SCREEN_
#define _8106_SCREEN_

/*--------------------------------------------------------------------------*/
// Default Screen Size
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480


/*--------------------------------------------------------------------------*/
#define INVERT_COLORS


/*--------------------------------------------------------------------------*/
// The following instructs the driver to return to text mode when the PEG
// application exits. This can be useful when running in protected mode. If
// you don't care about returning to text mode, turn this off to save pulling
// in the mode-3 text font.
/*--------------------------------------------------------------------------*/

#define RESTORE_TEXT_ON_EXIT

/*--------------------------------------------------------------------------*/
// If USE_BIOS is defined, the 8106 screen driver defaults to using
// the REAL MODE address of the video memory. If USE_BIOS is not defined,
// the 8106 screen driver defaults to using the flat-mode linear address.
// You can change this by modifying the pre-processor logic contained in 
// the 8106Screen constructor.
/*--------------------------------------------------------------------------*/

#define VID_MEM_BASE 0xA0000L  // linear address (32-bit flat mode)
#define VID_MEM_SIZE 0x10000L  // in bytes



/*--------------------------------------------------------------------------*/
// The following macro is used to write to the registers on the video
// controller. This may have to be changed depending on the environment.

#define WriteVidReg(index, val) {POUTB(0x3CE, index); POUTB(0x3cf, val);}
#define ReadVidReg(index) PINB(index)

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
class Peg8106Screen : public PegScreen
{
    public:
        Peg8106Screen(PegRect &);
        virtual ~Peg8106Screen();

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
        COLORVAL GetPixelView(SIGNED wXPos, SIGNED wYPos);
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


        WORD mwBytesPerRow;
        UCHAR PEGFAR *mpVidMem;

        //---------------------------------------------------------
        // a few extra things for running in VGA mode without using
        // any BIOS traps:
        //---------------------------------------------------------

        void HidePointer(void);
        void ConfigureController(void);

        #ifdef RESTORE_TEXT_ON_EXIT
         void ConfigureMode3(void);
         void SetMode3(void);
         void CopyTextFont(void);
         UCHAR *mpTextModeFont;
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



