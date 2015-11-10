/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// svgascrn.hpp - PegScreen class definition for running DOS SVGA.
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
//    This version of PegScreen is used for the Win32 development
//    environment. This version implements a 640x480x16 color PegScreen
//    under Win32. This resolution was chosen in order to be compatible
//    with the DOS screen and demo application. Other resolutions and/or
//    color depths could be easily be created using this module as an
//    example.
//
//
// This class defines all of the methods which must be supported in 
// instantiated screen classes. Users may of course add additional members
// in custom derived classes, but only those members found here will be used
// by PEG.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


#define VID_MEM_BASE 0xA0000000L;
#define USE_BIOS

/*--------------------------------------------------------------------------*/
// This driver only supports 1024x768 resolution. The following definition,
// SUPERVGA, tells the rest of the library what resolution we are running in.
/*--------------------------------------------------------------------------*/
#define SUPERVGA    

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 1024
#define PEG_VIRTUAL_YSIZE 768


/*--------------------------------------------------------------------------*/
extern DWORD old_bank;

#define PlotPointView(x, y, c)\
{\
    LONG lOffset = ((LONG) y << 10) + x;\
    DWORD bank;\
    bank = lOffset >> BitsShift;\
    if (bank != old_bank)\
    {\
        old_bank = bank;\
	    union REGS regs;\
    	regs.x.dx = bank;\
        regs.x.ax = 0x4f05;\
    	regs.x.bx = 0;\
        int86 (0x10, &regs, &regs);\
    }\
    UCHAR PEGFAR *Put = mpVidMem + (lOffset & AddrMask);\
    *Put = c;\
}
   
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegDosScreen : public PegScreen
{
    public:
        PegDosScreen(PegRect &);
        virtual ~PegDosScreen();

        //---------------------------------------------------------
        // Public pure virtual functions in PegScreen:
        //---------------------------------------------------------

        void BeginDraw(PegThing *);
        void EndDraw(void);
        void SetPointerType(UCHAR bType);
        void SetPointer(PegPoint);
        void Capture(PegCapture *Info, PegRect &Rect);
        COLORVAL GetPixelView(SIGNED x, SIGNED y);
        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(WORD *pPutSize);
        void ResetPalette(void);
        //inline void PlotPointView(SIGNED x, SIGNED y, UCHAR Color);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

    protected:

        void HidePointer(void);

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
            UCHAR Color, SIGNED wWidth);
        void VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
            UCHAR Color, SIGNED wWidth);
        void HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos);
        void VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos);

        //---------------------------------------------------------
        // End protected pure virtual functions.
        //---------------------------------------------------------


        void SetVidPage(LONG lOffset);

        BOOL GetModeData(SIGNED mode);
        void GetModeInfo(void);
        void GetModeList(void); 

        UCHAR far *mpVidMem;

        UCHAR mPalette[256*3];
        WORD  mwPalSize;

        PegCapture mCapture;
        PegPoint   mLastPointerPos;
        BOOL mbPointerHidden;
};


typedef struct {
int ModeAttributes;
char WinAAttributes;
char WinBAttributes;
int WinGranularity;
int WinSize;
int WinASegment;
int WinBSetment;
long WinFuncPtr;
int BytesPerScanLine;
int XResolution;
int YResolution;
char XCharSize;
char yCharSize;
char NumberOfPlanes;
char BitsPerPixel;
char NumberOfBanks;
char MemoryModel;
char BankSize;
char NumberOfImagePages;
char Reserved;
char RedMaskSize;
char RedFieldPosition;
char GreenMaskSize;
char GreenFieldPosition;
char BlueMaskSize;
char BlueFieldPosition;
char RsvdMaskSize;
char RsvdFieldPosition;
char DirectColorModeInfo;
char Filler[216];
} SvgaAttribs;


typedef struct
{
    char signature[4];
    int version;
    long int far *OEM_string;
    char capabilities[4];
    long int far *Modes;
    int memory_size;
    char reserved[512];
} MODELIST;

