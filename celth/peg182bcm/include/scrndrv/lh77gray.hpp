/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// lh77gray.hpp - Linear screen driver for 2-bpp grayscale operation on the
//  Sharp/ARM LH77790 development platform.
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
//  This driver can be tested under Win32 by turning on the definition
//  PEGWIN32. You can safely delete all code bracketed by the PEGWIN32
//  definition when moving to your target.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _GRAYSCREEN_
#define _GRAYSCREEN_


/*--------------------------------------------------------------------------*/
// Set the default screen resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 240


#define PlotPointView(x, y, c)\
    {\
    UCHAR *_Put = mpScanPointers[y] + (x >> 2);\
    UCHAR _uShift = ( x & 3) << 1;\
    UCHAR _uVal = *_Put & ~(0xc0 >> _uShift);\
    _uVal |= c << (6 - _uShift);\
    *_Put = _uVal;\
    }

/*********** EIG LCD PANEL REGISTER INITIALIZATION DEFINES ************/
	  
/* CPU ClOCK CONTROL REGISTER (CPU clock frequency). */
#define  CLK_PWR_BASE			  0xFFFFAC00

/* Peripheral Clock Select Register */

#define  PER_CLK_SEL_REG          (CLK_PWR_BASE + 0x4)	


/*** LCD CONTROLLER REGISTERS ***/
#define  LCD_BASE                 0xFFFF1400
#define  LCD_BIT_CTRL             0xFFFFa414

/* LCD MODE REGISTER */
#define  LCD_MODE_REG             LCD_BASE
#define  LCD_MODE_REG_RESET       0x00000000 /*LCD Ctrlr NOT actv*/

/* LCD HORIZONTAL DISPLAY BYTE COUNT Register */
#define  LCD_BYTE_CNT_REG         (LCD_BASE + 0x4)

/* LCD HORIZONTAL SYNC PULSE WIDTH REgister     LCD_CP1W  IN BOOK */
#define  LCD_SYNC_PULSE_REG       (LCD_BASE + 0x8)

/* LCD VERTICAL LINE COUNT (VLC) LSB Register lcd_duty */
#define  LCD_S1_VLC_LSB_REG       (LCD_BASE + 0xC)

/* LCD VERTICAL LINE COUNT (VLC) MSB Register */
#define  LCD_S1_VLC_MSB_REG       (LCD_BASE + 0x10)

/* LCD SCREEN1 DISPLAY START ADDRESS (SAD) Registers[0-3] */
#define  LCD_S1_SAD0_REG          (LCD_BASE + 0x14)
#define  LCD_S1_SAD1_REG          (LCD_BASE + 0x18)
#define  LCD_S1_SAD2_REG          (LCD_BASE + 0x1C)
#define  LCD_S1_SAD3_REG          (LCD_BASE + 0x20)

/* LCD Screen2 display start address (SAD) Registers[0-3] */
#define  LCD_S2_SAD0_REG          (LCD_BASE + 0x24)
#define  LCD_S2_SAD0_REG_MASK     0x000000FF
#define  LCD_S2_SAD0_REG_RESET    0x00000000 

#define  LCD_S2_SAD1_REG          (LCD_BASE + 0x28)
#define  LCD_S2_SAD2_REG          (LCD_BASE + 0x2C)
#define  LCD_S2_SAD3_REG          (LCD_BASE + 0x30)

/* LCD Screen1 vertical line count registers[0-3]
 * (Used only when panel is divided into two screens, and screen
 *  two is by default the total scan lines (SLT) minus screen 1
 *  scan lines (SL1), so screen2 VLC = SLT - SL1.
 */
#define  LCD_S2_VLC_LSB_REG       (LCD_BASE + 0x34)
#define  LCD_S2_VLC_MSB_REG       (LCD_BASE + 0x38)

/* LCD AdDRESS PITCH ADJUSTING REGISTer */
#define  LCD_ADDR_P_ADJ_REG       (LCD_BASE + 0x3C)

/* LCD GrEY STEP CONVERSION CODE (CC) Register */
#define  LCD_GRAY1_REG          (LCD_BASE + 0x40)
#define LCD_GRAY1_REG_RESET     0x00000077

/* LCD GrEY STEP DISPLAY DATA (DD) REgister */
#define  LCD_GRAY2_REG          (LCD_BASE + 0x44)
#define  LCD_GRAY2_REG_RESET    0x000000aa 

/* LCD ClOCK FREQUENCY DIVIDER (CLK_DIV) Register */
#define  LCD_CLK_DIV_REG          (LCD_BASE + 0x48)

/* LCD MClock Width Registers [0-1] */
#define  LCD_MCLK_LSB_REG         (LCD_BASE + 0x4C)
#define  LCD_MCLK_MSB_REG         (LCD_BASE + 0x50)

#define LCD_MAX_BYTES_LINE	 0x4F    /* 320 pixels/4bits=80 bytes (0-79) */
#define LCD_CLK_ENABLE 	  0x40
//#define LCD_CP1W_VALUE    0x08
#define LCD_CP1W_VALUE    0x01
#define LCD_MODE_DSPL_GRAY_ON  0x8B 
#define LCD_VERT_LINES_LSB	 0xEF	 /* max vertical line=239 (0-239=240) */
#define LCD_VERT_LINES_MSB	 0x00	 /* MSB is zero */

#define LCD_FRAME_BUFFER_SIZE (320 * 240 / 4)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class LHGrayScreen : public PegScreen
{
    public:
       #ifndef PEGWIN32
        LHGrayScreen(PegRect &);
       #else
        LHGrayScreen(HWND hwnd, PegRect &);
        SIGNED mwWinRectXOffset;
        SIGNED mwWinRectYOffset;
       #endif

        virtual ~LHGrayScreen();

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
            UCHAR *_Get = mpScanPointers[y] + (x >> 2);
            UCHAR _uShift = 6 - (( x & 3) << 1);
            UCHAR _uVal = *_Get;
            _uVal >>= _uShift;
            _uVal &= 3;
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

            WORD wBytesPerLine = (pMap->wWidth + 3) >> 2;
            _pGet += wBytesPerLine * y;
            _pGet += x >> 2;
            UCHAR _uVal = *_pGet;
            UCHAR _uShift = 6 - (( x & 3) << 1);
            _uVal >>= _uShift;
            _uVal &= 3;
            return ((COLORVAL) _uVal);
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            WORD wBytesPerLine = (pMap->wWidth + 3) >> 2;
            COLORVAL *_pPut = pMap->pStart;
            _pPut += wBytesPerLine * y;
            _pPut += x >> 2;
            UCHAR _uShift = 6 - ((x & 3) << 1);
            UCHAR _uVal = *_pPut;
            _uVal &= ~(0x03 << _uShift);
            _uVal |= cVal << _uShift;
            *_pPut = _uVal;
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

        // functions for drawing to off-screen bitmaps, using linear addressing:
        void DrawFast4ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void DrawUnaligned4ColorBitmap(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);

        void SetFrameBuffer(DWORD);

        UCHAR  muPalette[2 * 3];

       #ifdef PEGWIN32

        void MemoryToScreen(void);
        HWND mHWnd;
        HPALETTE mhPalette;
        HDC  mHdc;
       #endif
};

#endif



