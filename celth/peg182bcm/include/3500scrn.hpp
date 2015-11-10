/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// 3500scrn.hpp - screen driver STV3500
//
// Author: Kenneth G. Maxwell
//				 Modified by STMicroelectronics TVD application Lab
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _ST3500SCREEN_
#define _ST3500SCREEN_

#include "sv_peg.h"

/*--------------------------------------------------------------------------*/
// Default resolution and color depth
/*--------------------------------------------------------------------------*/
/*#define PEG_VIRTUAL_XSIZE 320
#define PEG_VIRTUAL_YSIZE 480*/

                    
/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (UCHAR) c;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class STV3500Screen : public PegScreen
{
    public:
        STV3500Screen(PegRect &, SVPEG_InitParams_t);

        virtual ~STV3500Screen();

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

        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

    protected:
        
        BOOL mbPointerHidden;
        PegCapture mCapture;
        PegPoint  mLastPointerPos;
    
				STOSD_RegionHandle_t	RegionOsdHandle;
				STOSD_Bitmap_t				RegionOsdBitmapBuffer;
				STOSD_Bitmap_t				SmallBitmap;

				UCHAR *GetVideoAddress(void);
        void ConfigureController(void);

				int PEG_VIRTUAL_XSIZE;
				int PEG_VIRTUAL_YSIZE;

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

				void InitScanPointers(void);
        #ifdef PEG_DOUBLE_BUFFER
        void MemoryToScreen(void);
        #endif
};

#endif



