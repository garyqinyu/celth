/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// l4scrn.hpp - Linear screen driver for 4-bpp (16 color) operation.
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

#ifndef _DRAGONSCREEN_
#define _DRAGONSCREEN_

#define TESTBOARD_1             // proprietary test board??

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
#define PEG_VIRTUAL_XSIZE 240
#define PEG_VIRTUAL_YSIZE 320

/*--------------------------------------------------------------------------*/
// Dragonball LCD registers

#if 1
#define LCD_REG_BASE  		0xfffffa00L
#define LCD_SSA                 ((volatile unsigned long  *)(0xFFFFFA00))
#define LCD_VPW                 ((volatile unsigned char  *)(0xFFFFFA05))
#define LCD_XMAX                ((volatile unsigned char  *)(0xFFFFFA08))
#define LCD_YMAX                ((volatile unsigned short *)(0xFFFFFA0A))
#define LCD_CXP                 ((volatile unsigned short *)(0xFFFFFA18))
#define LCD_CYP                 ((volatile unsigned short *)(0xFFFFFA1A))
#define LCD_CWCH                ((volatile unsigned short *)(0xFFFFFA1C))
#define LCD_BLKC                ((volatile unsigned char  *)(0xFFFFFA1F))
#define LCD_PICF                ((volatile unsigned char  *)(0xFFFFFA20))
#define LCD_POLCF               ((volatile unsigned char  *)(0xFFFFFA21))
#define LCD_ACDRC               ((volatile unsigned char  *)(0xFFFFFA23))
#define LCD_PXCD                ((volatile unsigned char  *)(0xFFFFFA25))
#define LCD_CKCON               ((volatile unsigned char  *)(0xFFFFFA27))
#define LCD_POSR                ((volatile unsigned char  *)(0xFFFFFA2D))
#define LCD_FRCM                ((volatile unsigned char  *)(0xFFFFFA31))
#define LCD_LRRA                ((volatile unsigned char  *)(0xFFFFFA29))
#define LCD_GPMR             	((volatile unsigned char  *)(0xFFFFFA32))
#define LCD_PWMR                ((volatile unsigned short *)(0xFFFFFA36))

#ifdef TESTBOARD_1

#define REG_PBDIR               ((volatile UCHAR  *)(0xFFFFF408))
#define REG_PBDATA              ((volatile UCHAR  *)(0xFFFFF409))
#define REG_PBPUEN              ((volatile UCHAR  *)(0xFFFFF40A))
#define REG_PBSEL               ((volatile UCHAR  *)(0xFFFFF40B))

#define REG_PFDIR    ((UCHAR *) 0xFFFFF428)
#define REG_PFDATA   ((UCHAR *) 0xFFFFF429)
#define REG_PFPUEN   ((UCHAR *) 0xFFFFF42A)
#define REG_PFSEL    ((UCHAR *) 0xFFFFF42B)

#define REG_PGDIR    ((UCHAR *) 0xFFFFF430)
#define REG_PGDATA   ((UCHAR *) 0xFFFFF431)
#define REG_PGPUEN   ((UCHAR *) 0xFFFFF432)
#define REG_PGSEL    ((UCHAR *) 0xFFFFF433)

#define REG_PCSEL    ((UCHAR *) 0xFFFFF413)
#endif

#else

#define LCD_REG_BASE  0xfffffa00L
#define LCD_SSA      (LCD_REG_BASE + 0x00)
#define LCD_VPW      (LCD_REG_BASE + 0x05)
#define LCD_XMAX     (LCD_REG_BASE + 0x08)
#define LCD_YMAX     (LCD_REG_BASE + 0x0a)
#define LCD_CXP      (LCD_REG_BASE + 0x18)
#define LCD_CYP      (LCD_REG_BASE + 0x1a)
#define LCD_CWCH     (LCD_REG_BASE + 0x1c)
#define LCD_BLKC     (LCD_REG_BASE + 0x1f)
#define LCD_PICF     (LCD_REG_BASE + 0x20)
#define LCD_POLCF    (LCD_REG_BASE + 0x21)
#define LCD_ACDRC    (LCD_REG_BASE + 0x23)
#define LCD_PXCD     (LCD_REG_BASE + 0x25)
#define LCD_CKCON    (LCD_REG_BASE + 0x27)
#define LCD_RRA      (LCD_REG_BASE + 0x29)
#define LCD_POSR     (LCD_REG_BASE + 0x2D)
#define LCD_FRCM     (LCD_REG_BASE + 0x31)
#define LCD_GPMR     (LCD_REG_BASE + 0x33)
#define LCD_PWMR     (LCD_REG_BASE + 0x36)

#ifdef TESTBOARD_1

#define REG_PFDIR    ((UCHAR *) 0xFFFFF428)
#define REG_PFDATA   ((UCHAR *) 0xFFFFF429)
#define REG_PFPUEN   ((UCHAR *) 0xFFFFF42A)
#define REG_PFSEL    ((UCHAR *) 0xFFFFF42B)

#define REG_PGDIR    ((UCHAR *) 0xFFFFF430)
#define REG_PGDATA   ((UCHAR *) 0xFFFFF431)
#define REG_PGPUEN   ((UCHAR *) 0xFFFFF432)
#define REG_PGSEL    ((UCHAR *) 0xFFFFF433)

#define REG_PCSEL    ((UCHAR *) 0xFFFFF413)

#endif

#endif      // If 1 if

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
class Dragon4Screen : public PegScreen
{
    public:
        Dragon4Screen(PegRect &);
        virtual ~Dragon4Screen();

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

};

#endif



