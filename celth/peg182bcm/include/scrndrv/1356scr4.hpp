/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1356scr4.hpp - Linear screen driver for 4-bpp (16 color) operation.
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
//  This screen driver is intended for use with 16-color systems that support
//  a linear frame buffer. Example controllers include the SMOS 135xx series
//  and the MPC823.
//
//  This driver can be tested under Win32 by turning on the definition
//  PEGWIN32. You can safely delete all code bracketed by the PEGWIN32
//  definition when moving to your target.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _L4SCREEN_
#define _L4SCREEN_


/*--------------------------------------------------------------------------*/
// Set the default screen resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 640
#define PEG_VIRTUAL_YSIZE 480

/*--------------------------------------------------------------------------*/
// PCI/Direct connect configuration
/*--------------------------------------------------------------------------*/
#define USE_PCI         // installed in a PCI bus??

#ifdef USE_PCI

#define PEGPCI_SED1356

#else

#define VID_MEM_BASE 0xc00000L    // fill in your address here
#define VID_REG_BASE 0x800000L    // fill in your address here
#define BITBLT_REG_BASE 0x900000L // fill in your address here

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
#define VID_MEM_SIZE       0x200000L     // default to 2M video RAM

/*--------------------------------------------------------------------------*/
// The following definition, when turned on, allows bitmaps created at
// run time to be placed directly in video memory. This should only be turned
// on if your video memory is >= 2X the size of a single frame buffer.
//
// DOUBLE_BUFFER would not normally be used at the same time as the
// VID_MEM_MANAGER
/*--------------------------------------------------------------------------*/
#define USE_VID_MEM_MANAGER           // use video memory manager?

/*--------------------------------------------------------------------------*/
// HARDWARE_CURSOR should be turned on
/*--------------------------------------------------------------------------*/
#define HARDWARE_CURSOR                 // use hardware cursor?


/*--------------------------------------------------------------------------*/
// CRT_CURSOR
// The flag affects HARDWARE_CURSOR operation only. This flag, when set,
// causes the driver to write the CRTTV hardware cursor registers.  If this flag
// is cleared some code and overhead are eliminated.
/*--------------------------------------------------------------------------*/

#define CRT_CURSOR

/*--------------------------------------------------------------------------*/
// LCD_CURSOR
// The flag affects HARDWARE_CURSOR operation only. This flag, when set,
// causes the driver to write the LCD hardware cursor registers. If this flag
// is cleared some code and overhead are eliminated.
/*--------------------------------------------------------------------------*/

//#define LCD_CURSOR

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
// Definitions allowing us to pull in the register values generated with
// the 1356cfg.exe utility provided by Sharp:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

struct SED1356RegEntry {
    WORD wIndex;
    UCHAR uVal;
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
struct HAL_STRUCT {
    char *IdString;
    WORD wEndian;
    WORD wSizeOf;
    WORD wFlags;
    SED1356RegEntry rInitVals[128];
    WORD wSkip[3];
    DWORD dSkip[2];
    WORD wSkip1[8];
    BOOL  bFilter;
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1356 Register Offsets:

#define REG_REV_CODE                0x00
#define REG_MISC                    0x01              
#define REG_GPIO_CONFIG             0x04          
#define REG_GPIO_CTRL               0x08          
#define REG_MEM_CLOCK_CFG           0x10          
#define REG_LCD_PCLK_CFG            0x14          
#define REG_CRTTV_PCLK_CFG          0x18      
#define REG_MPCLK_CFG               0x1c      
#define REG_CPU2MEM_WAIT_SEL        0x1e      
#define REG_MEM_CFG                 0x20      
#define REG_DRAM_REFRESH            0x21      
#define REG_DRAM_TIMINGS_CTRL0      0x2a      
#define REG_DRAM_TIMINGS_CTRL1      0x2b      
#define REG_PANEL_TYPE              0x30
#define REG_MOD_RATE                0x31      
#define REG_LCD_HDP                 0x32      
#define REG_LCD_HNDP                0x34      
#define REG_TFT_FPLINE_START        0x35      
#define REG_TFT_FPLINE_PULSE        0x36      
#define REG_LCD_VDP0                0x38      
#define REG_LCD_VDP1                0x39      
#define REG_LCD_VNDP                0x3a      
#define REG_TFT_FPFRAME_START       0x3b      
#define REG_TFT_FPFRAME_PULSE       0x3c      
#define REG_LCD_DISPLAY_MODE        0x40      
#define REG_LCD_MISC                0x41      
#define REG_LCD_START_ADDR0         0x42      
#define REG_LCD_START_ADDR1         0x43      
#define REG_LCD_START_ADDR2         0x44      
#define REG_LCD_MEM_ADDR_OFFSET0    0x46
#define REG_LCD_MEM_ADDR_OFFSET1    0x47  
#define REG_LCD_PIXEL_PANNING       0x48  
#define REG_LCD_FIFO_HIGH_THRESHOLD 0x4a  
#define REG_LCD_FIFO_LOW_THRESHOLD  0x4b  
#define REG_CRTTV_HDP               0x50  
#define REG_CRTTV_HNDP              0x52  
#define REG_CRTTV_HRTC_START        0x53  
#define REG_CRT_HRTC_PULSE          0x54  
#define REG_CRTTV_VDP0              0x56  
#define REG_CRTTV_VDP1              0x57  
#define REG_CRTTV_VNDP              0x58  
#define REG_CRTTV_VRTC_START        0x59  
#define REG_CRT_VRTC_PULSE          0x5a  
#define REG_TV_OUTPUT_CTRL          0x5b  
#define REG_CRTTV_DISPLAY_MODE      0x60 
#define REG_CRTTV_START_ADDR0       0x62  
#define REG_CRTTV_START_ADDR1       0x63  
#define REG_CRTTV_START_ADDR2       0x64  
#define REG_CRTTV_MEM_ADDR_OFFSET0  0x66  
#define REG_CRTTV_MEM_ADDR_OFFSET1  0x67  
#define REG_CRTTV_PIXEL_PANNING     0x68  
#define REG_CRTTV_FIFO_HIGH_THRESHOLD 0x6a
#define REG_CRTTV_FIFO_LOW_THRESHOLD  0x6b
#define REG_LCD_INK_CURS_CTRL       0x70  
#define REG_LCD_INK_CURS_START_ADDR 0x71  
#define REG_LCD_CURSOR_X_POS0       0x72  
#define REG_LCD_CURSOR_X_POS1       0x73  
#define REG_LCD_CURSOR_Y_POS0       0x74  
#define REG_LCD_CURSOR_Y_POS1       0x75  
#define REG_LCD_INK_CURS_BLUE0      0x76  
#define REG_LCD_INK_CURS_GREEN0     0x77  
#define REG_LCD_INK_CURS_RED0       0x78  
#define REG_LCD_INK_CURS_BLUE1      0x7a  
#define REG_LCD_INK_CURS_GREEN1     0x7b  
#define REG_LCD_INK_CURS_RED1       0x7c  
#define REG_LCD_INK_CURS_FIFO       0x7e  
#define REG_CRTTV_INK_CURS_CTRL     0x80  
#define REG_CRTTV_INK_CURS_START_ADDR 0x81 
#define REG_CRTTV_CURSOR_X_POS0     0x82  
#define REG_CRTTV_CURSOR_X_POS1     0x83  
#define REG_CRTTV_CURSOR_Y_POS0     0x84  
#define REG_CRTTV_CURSOR_Y_POS1     0x85  
#define REG_CRTTV_INK_CURS_BLUE0    0x86  
#define REG_CRTTV_INK_CURS_GREEN0   0x87  
#define REG_CRTTV_INK_CURS_RED0     0x88  
#define REG_CRTTV_INK_CURS_BLUE1    0x8a  
#define REG_CRTTV_INK_CURS_GREEN1   0x8b  
#define REG_CRTTV_INK_CURS_RED1     0x8c  
#define REG_CRTTV_INK_CURS_FIFO     0x8e  
#define REG_BITBLT_CTRL0            0x100  
#define REG_BITBLT_CTRL1            0x101  
#define REG_BITBLT_ROP_CODE         0x102  
#define REG_BITBLT_OPERATION        0x103  
#define REG_BITBLT_SRC_START_ADDR0  0x104  
#define REG_BITBLT_SRC_START_ADDR1  0x105  
#define REG_BITBLT_SRC_START_ADDR2  0x106  
#define REG_BITBLT_DEST_START_ADDR0 0x108  
#define REG_BITBLT_DEST_START_ADDR1 0x109  
#define REG_BITBLT_DEST_START_ADDR2 0x10a  
#define REG_BITBLT_MEM_ADDR_OFFSET0 0x10c  
#define REG_BITBLT_MEM_ADDR_OFFSET1 0x10d  
#define REG_BITBLT_WIDTH0           0x110  
#define REG_BITBLT_WIDTH1           0x111  
#define REG_BITBLT_HEIGHT0          0x112  
#define REG_BITBLT_HEIGHT1          0x113  
#define REG_BITBLT_BACKGND_COLOR0   0x114  
#define REG_BITBLT_BACKGND_COLOR1   0x115  
#define REG_BITBLT_FOREGND_COLOR0   0x118  
#define REG_BITBLT_FOREGND_COLOR1   0x119  
#define REG_LUT_MODE                0x1e0  
#define REG_LUT_ADDR                0x1e2  
#define REG_LUT_DATA                0x1e4  
#define REG_PWR_SAVE_CFG            0x1f0  
#define REG_PWR_SAVE_STATUS         0x1f1  
#define REG_CPU2MEM_WATCHDOG        0x1f4  
#define REG_DISPLAY_MODE            0x1fc          
#define FINISHED_REG_CFG            0x5555             

#define fVALID_LCD 1
#define fVALID_CRT 2

/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c)\
{\
    UCHAR *_Put = mpScanPointers[y] + (x >> 1);\
    UCHAR _uVal = *_Put;\
    if (x & 1)\
    {\
        _uVal &= 0xf0;\
        _uVal |= c;\
    }\
    else\
    {\
        _uVal &= 0x0f;\
        _uVal |= c << 4;\
    }\
    *_Put = _uVal;\
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SED1356Screen : public PegScreen
{
    public:
        SED1356Screen(PegRect &);

        virtual ~SED1356Screen();

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
            UCHAR *_Get = mpScanPointers[y] + (x >> 1);
            UCHAR _uVal = *_Get;
            if (x & 1)
            {
                _uVal &= 0x0f;
            }
            else
            {
                _uVal >>= 4;
            }
            return ((COLORVAL) _uVal);

        }

        #ifdef PEG_IMAGE_SCALING

        COLORVAL GetBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap)
        {
            COLORVAL *_pGet = pMap->pStart;

            if (pMap->uBitsPix == 8)
            {
                _pGet += pMap->wWidth * y;
                _pGet += x;
                return *_pGet;
            }

            WORD wBytesPerLine = (pMap->wWidth + 1) >> 1;
            _pGet += wBytesPerLine * y;
            _pGet += x >> 1;
            UCHAR uVal = *_pGet;
            if (x & 1)
            {
                uVal &= 0x0f;
            }
            else
            {
                uVal >>= 4;
            }
            return (COLORVAL) uVal;
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            WORD wBytesPerLine = (pMap->wWidth + 1) >> 1;
            COLORVAL *_pPut = pMap->pStart;
            _pPut += wBytesPerLine * y;
            _pPut += x >> 1;

            UCHAR uVal = *_pPut;
            if (x & 1)
            {
                uVal &= 0xf0;
                uVal |= (UCHAR) cVal;
            }
            else
            {
                uVal &= 0x0f;
                uVal |= (UCHAR) cVal << 4;
            }
            *_pPut = (COLORVAL) uVal;
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
		UCHAR muPalette[256*3];

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
        void Draw16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void DrawUnaligned16ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);

       #ifdef DOUBLE_BUFFER
        void MemoryToScreen(void);
       #endif

        UCHAR *mpVidMemBase;
        UCHAR *mpVidRegBase;
        UCHAR *mpBitBlitRegs;
};

#endif



