/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 1376scr8.hpp - Linear screen driver for 8-bpp (256 color) operation.
//
// Author: Kenneth G. Maxwell & Jim DeLisle
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
// This driver uses the output of the Epson utility program 1376cfg.exe 
// top configure the controller registers. To use this driver, you first
// need to run the 1376cfg.exe utility program (you can download this
// program from the Epson www site). After verifying all settings in the
// dialog boxes, save the configuration in "HAL" (i.e. header file) format.
//
// The output file should be named "1376cfg.h", because this is the file
// that our screen driver is going to include!! If you give the file some
// other name you will not be able to compile the screen driver!
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _1376SCREEN8_
#define _1376SCREEN8_


/*--------------------------------------------------------------------------*/
// USE_PCI 
//
// Turn this on if using the SDU1376 eval card in PCI configuration. In
// this configuration we need to find the card address using run-time
// PCI bios function calls. Otherwise, you simply define the controller
// address below.
/*--------------------------------------------------------------------------*/
//#define USE_PCI




/*--------------------------------------------------------------------------*/
// DOUBLE_BUFFER 
//
// When this is turned on, PEG does all drawing to an offscreen buffer, 
// and then transfers the buffer into the visible frame buffer.
/*--------------------------------------------------------------------------*/
//#define DOUBLE_BUFFER


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*
     1376 register and flag names.

     These register names are produces by the 1376cfg.exe utility program,
     we need to define them so that we can include the output of the 1376
     configuration utility in our driver.
*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define REG_REVISION_CODE              0x00
#define REG_DISP_BUFFER_SIZE           0x01
#define REG_CONFIG_READBACK            0x02
#define REG_BUSCLK_MEMCLK_CONFIG       0x04
#define REG_PCLK_CONFIG                0x05
#define REG_LUT_BLUE_WRITE_DATA        0x08
#define REG_LUT_GREEN_WRITE_DATA       0x09
#define REG_LUT_RED_WRITE_DATA         0x0a
#define REG_LUT_WRITE_ADDR             0x0b
#define REG_LUT_BLUE_READ_DATA         0x0c
#define REG_LUT_GREEN_READ_DATA        0x0d
#define REG_LUT_RED_READ_DATA          0x0e
#define REG_LUT_READ_ADDR              0x0f
#define REG_PANEL_TYPE                 0x10
#define REG_MOD_RATE                   0x11
#define REG_HORIZ_TOTAL                0x12
#define REG_HDP                        0x14
#define REG_HDP_START_POS0             0x16
#define REG_HDP_START_POS1             0x17
#define REG_VERT_TOTAL0                0x18
#define REG_VERT_TOTAL1                0x19
#define REG_VDP0                       0x1c
#define REG_VDP1                       0x1d
#define REG_VDP_START_POS0             0x1e
#define REG_VDP_START_POS1             0x1f
#define REG_HSYNC_PULSE_WIDTH          0x20
#define REG_HSYNC_PULSE_START_POS0     0x22
#define REG_HSYNC_PULSE_START_POS1     0x23
#define REG_VSYNC_PULSE_WIDTH          0x24
#define REG_VSYNC_PULSE_START_POS0     0x26
#define REG_VSYNC_PULSE_START_POS1     0x27 
#define REG_DTFD_GCP_INDEX             0x28
#define REG_DTFD_GCP_DATA              0x2c
#define REG_DISPLAY_MODE               0x70
#define REG_SPECIAL_EFFECTS            0x71
#define REG_MAIN_WIN_DISP_START_ADDR0  0x74
#define REG_MAIN_WIN_DISP_START_ADDR1  0x75
#define REG_MAIN_WIN_DISP_START_ADDR2  0x76
#define REG_MAIN_WIN_ADDR_OFFSET0      0x78
#define REG_MAIN_WIN_ADDR_OFFSET1      0x79
#define REG_SUB_WIN_DISP_START_ADDR0   0x7c
#define REG_SUB_WIN_DISP_START_ADDR1   0x7d
#define REG_SUB_WIN_DISP_START_ADDR2   0x7e
#define REG_SUB_WIN_ADDR_OFFSET0       0x80
#define REG_SUB_WIN_ADDR_OFFSET1       0x81
#define REG_SUB_WIN_X_START_POS0       0x84
#define REG_SUB_WIN_X_START_POS1       0x85
#define REG_SUB_WIN_Y_START_POS0       0x88
#define REG_SUB_WIN_Y_START_POS1       0x89
#define REG_SUB_WIN_X_END_POS0         0x8c
#define REG_SUB_WIN_X_END_POS1         0x8d
#define REG_SUB_WIN_Y_END_POS0         0x90
#define REG_SUB_WIN_Y_END_POS1         0x91
#define REG_POWER_SAVE_CONFIG          0xa0
#define REG_CPU_ACCESS_CONTROL         0xa1
#define REG_SOFTWARE_RESET             0xa2
#define REG_BIG_ENDIAN_SUPPORT         0xa3
#define REG_SCRATCH_PAD0               0xa4
#define REG_SCRATCH_PAD1               0xa5
#define REG_GPIO_CONFIG0               0xa8
#define REG_GPIO_CONFIG1               0xa9
#define REG_GPIO_STATUS_CONTROL0       0xac
#define REG_GPIO_STATUS_CONTROL1       0xad
#define REG_PWM_CV_CLOCK_CONTROL       0xb0
#define REG_PWM_CV_CLOCK_CONFIG        0xb1
#define REG_CV_CLOCK_BURST_LENGTH      0xb2
#define REG_PWM_CLOCK_DUTY_CYCLE       0xb3


#define MAX_GCP_DATA    32
#define MAX_REG         100
#define HIGHEST_REG     REG_PWM_CLOCK_DUTY_CYCLE
#define FINISHED_REG_CFG 0xffff

#define fVALID_LCD         0x00000001
#define fFIXED_CLKI        0x00000002
#define fFIXED_CLKI2       0x00000004
#define fFIXED_DIVMCLK     0x00000008
#define fFIXED_DIVPCLK     0x00000010

typedef struct
{
    WORD index;
    WORD val;
} RegVal;


/*--------------------------------------------------------------------------*/
// HalStruct- This is the structure produced by the 1376cfg.exe utility
// program.
/*--------------------------------------------------------------------------*/
typedef struct 
{
   char  szIdString[16];
   WORD  wDetectEndian;
   WORD  wSize;

   DWORD dwFlags;
   RegVal Regs[MAX_REG];
   UCHAR  GcpData[MAX_GCP_DATA];

   DWORD dwClkI;                    /* Input Clock Frequency (in kHz) */
   DWORD dwClkI2;                   /* Input Clock 2 Frequency (in kHz) */
   DWORD dwBusClk;                  /* Bus Clock Frequency (in kHz) */
   DWORD dwRegisterAddress;         /* Starting address of registers */
   DWORD dwDisplayMemoryAddress;    /* Starting address of display buffer memory */
   WORD  wPanelFrameRate;           /* Desired panel frame rate */
   WORD  wLcdPowerdownTime;         /* Time before LCD loses power (in milliseconds) */
   WORD  wLcdPowerupTime;           /* Time before LCD gets power (in milliseconds) */
   WORD  wReserved0;                /* */
   WORD  wReserved1;                /* */
   WORD  wReserved2;                /* */

} HAL_STRUCT;



/*--------------------------------------------------------------------------*/
// Default LCD resolution. Since there is only 80k of memory on the 
// SED1376, 320x240 is a resonable resolution at 8bpp.
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 240

/*--------------------------------------------------------------------------*/
// Where do we map to video memory and registers
/*--------------------------------------------------------------------------*/

#ifdef USE_PCI

#define PEGPCI_SED1376

#else

// If NOT running in PCI environment (i.e. now you are running on on your
// embedded target??), just define the controller addresses:

#define VID_MEM_BASE    0xF00000L    
#define VID_REG_BASE    (VID_MEM_BASE + 0x20000)  // Regs = mem + 2Meg?    

#endif

/*--------------------------------------------------------------------------*/
// VID_MEM_SIZE- how much video memory is installed on your system?
/*--------------------------------------------------------------------------*/
#define VID_MEM_SIZE    0x14000     // default to 80k video RAM

/*--------------------------------------------------------------------------*/
// ID of the SED1376 card
/*--------------------------------------------------------------------------*/
#define SED1376ID		0x24

/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (UCHAR) c;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SED1376Screen8 : public PegScreen
{
    public:
        SED1376Screen8(PegRect &);
        virtual ~SED1376Screen8();

        #ifdef PEGWIN32
        SIGNED mwWinRectXOffset;
        SIGNED mwWinRectYOffset;
        #endif

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

        #ifdef PEG_IMAGE_SCALING
        COLORVAL GetBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap)
        {
            COLORVAL PEGFAR *pGet = pMap->pStart;
            pGet += pMap->wWidth * y;
            pGet += x;
            return *pGet;
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            COLORVAL PEGFAR *pPut = pMap->pStart;
            pPut += pMap->wWidth * y;
            pPut += x;
            *pPut = cVal;
        }
        #endif

        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

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

       #ifdef DO_OUTLINE_TEXT
        void DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
            PegFont *Font, SIGNED iCount, PegRect &Rect);
       #endif

        UCHAR  muPalette[256 * 3];

        UCHAR *mpVidMemBase;
        UCHAR *mpVidRegBase;

       #if defined(DOUBLE_BUFFER)
		void MemoryToScreen(void);
	   #endif
};

#endif	// _1376SCR8_





