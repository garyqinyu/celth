/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plbscale.hpp - Interface for the PegLinearBitmapScale
//
// Author: Jim DeLisle
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

#ifndef _PLBSCALE_HPP_
#define _PLBSCALE_HPP_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegLinearBitmapScale : public PegScale
{
    public:
        PegLinearBitmapScale(const PegRect &Size, LONG lMinValue,
                             LONG lMaxValue, SIGNED iMinTravelOffset, 
                             SIGNED iMaxTravelOffset, PegBitmap* pBkgBmp, 
                             PegBitmap* pNeedleBmp = NULL, 
                             WORD wStyle = SS_STANDARDSTYLE);
        PegLinearBitmapScale(SIGNED iLeft, SIGNED iTop, LONG lMinValue,
                             LONG lMaxValue, SIGNED iMinTravelOffset, 
                             SIGNED iMaxTravelOffset, PegBitmap* pBkgBmp, 
                             PegBitmap* pNeedleBmp = NULL, 
                             WORD wStyle = SS_STANDARDSTYLE);
        ~PegLinearBitmapScale();

        // Overrides
        virtual void Draw();
        
        PegBitmap* GetBkgBitmap() const { return mpBkgBitmap; }
        void SetBkgBitmap(PegBitmap* pBmp);
        PegBitmap* GetNeedleBitmap() const { return mpNeedleBitmap; }
        void SetNeedleBitmap(PegBitmap* pBmp) { mpNeedleBitmap = pBmp; }
        
        SIGNED GetNeedleOffset() const { return miNeedleOffset; }
        void SetNeedleOffset(SIGNED iOffset) { miNeedleOffset = iOffset; }
        SIGNED GetMinTravelOffset() const { return miMinTravelOffset; }
        void SetMinTravelOffset(SIGNED iOffset)
        {
            miMinTravelOffset = iOffset;
        }
        SIGNED GetMaxTravelOffset() const { return miMaxTravelOffset; }
        void SetMaxTravelOffset(SIGNED iOffset)
        {
            miMaxTravelOffset = iOffset;
        }


    protected:
        virtual void SetTravelPoints();
        virtual void CalcNeedleRect(const PegPoint& tPoint, PegRect& Rect);
        void DrawToComposite();
        void DrawBitmapNeedle();

        PegBitmap* mpBkgBitmap;
        PegBitmap* mpNeedleBitmap;

        PegBitmap* mpCompBitmap;

        SIGNED miNeedleOffset;

        SIGNED miMinTravelOffset;
        SIGNED miMaxTravelOffset;
};

#endif // _PLBSCALE_HPP_


