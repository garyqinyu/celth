/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pscale.hpp - Base for the PegScale classes
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

#ifndef _PSCALE_HPP_
#define _PSCALE_HPP_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegScale : public PegThing
{
    public:
        PegScale(const PegRect &Size, LONG lMinValue, LONG lMaxValue,
                 WORD wStyle);
        PegScale(SIGNED iLeft, SIGNED iTop, LONG lMinValue, LONG lMaxValue,
                 WORD wStyle);
        virtual ~PegScale();

        virtual void SetValue(LONG lVal, BOOL bRedraw = TRUE);
        virtual LONG IncrementValue(LONG lVal, BOOL bRedraw = TRUE);
        LONG GetCurrentValue() const { return mlCurValue; }

        COLORVAL GetNeedleColor() const { return mtNeedleColor; }
        void SetNeedleColor(COLORVAL tColor)
        {
            mtNeedleColor = tColor;
        }


        LONG GetMinValue() const { return mlMinValue; }
        void SetMinValue(LONG lVal, BOOL bRedraw = TRUE);
        LONG GetMaxValue() const { return mlMaxValue; }
        void SetMaxValue(LONG lVal, BOOL bRedraw = TRUE);

    protected:
        virtual void DrawNeedle();
        virtual void ValToPoint(LONG lVal, PegPoint& tPoint);
        virtual void SetTravelPoints();
        virtual void CalcClipAndDraw();
        virtual void CalcNeedleRect(const PegPoint& tPoint, PegRect& Rect);

        LONG mlCurValue;
        LONG mlMinValue;
        LONG mlMaxValue;

        SIGNED miMinTravel;
        SIGNED miMaxTravel;

        COLORVAL mtNeedleColor;
        
        PegPoint mtNeedlePoint;
        BOOL     mbValueSet;
};

#endif // _PSCALE_HPP_


