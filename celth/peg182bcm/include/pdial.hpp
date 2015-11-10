/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pegdial.hpp - Base for the PegDial classes
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

#ifndef _PEGDIAL_HPP_
#define _PEGDIAL_HPP_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegDial : public PegThing
{
    public:
        PegDial(const PegRect& Rect, WORD wStyle);
        PegDial(SIGNED iLeft, SIGNED iTop, WORD wStyle);
        virtual ~PegDial();

        virtual void SetValue(LONG lValue, BOOL bRedraw = TRUE);
        virtual LONG IncrementValue(LONG lValue, BOOL bRedraw = TRUE);

        // Accessors and mutators
        LONG GetCurrentValue() const { return mlCurValue; }
        SIGNED GetCurAngle() const { return miCurAngle; }

        WORD GetNeedleLength() const { return mwNeedleLen; }
        void SetNeedleLength(WORD wLen)
        {
            if(wLen <= 100)
            {
                mwNeedleLen = wLen;
            }
        }

        COLORVAL GetNeedleColor() const { return mtNeedleColor; }
        void SetNeedleColor(COLORVAL tColor)
        {
            mtNeedleColor = tColor;
        }

        COLORVAL GetDialColor() const { return mtDialColor; }
        void SetDialColor(COLORVAL tColor)
        {
            mtDialColor = tColor;
        }
        
        COLORVAL GetAnchorColor() const { return mtAnchorColor; }
        void SetAnchorColor(COLORVAL tColor)
        {
            mtAnchorColor = tColor;
        }
        
        WORD GetAnchorWidth() const { return mwAnchorWidth; }
        void SetAnchorWidth(WORD wWidth)
        {
            mwAnchorWidth = wWidth;
        }
                
        LONG GetTicFreq() const { return mlTicFreq; }
        void SetTicFreq(LONG lFreq) { mlTicFreq = lFreq; }
        WORD GetTicLen() const { return mwTicLen; }
        void SetTicLen(WORD wLen)
        {
            if(wLen <= 100)
            {
                mwTicLen = wLen;
            }
        }
        
        SIGNED GetMinAngle() const { return miMinAngle; }
        SIGNED GetMaxAngle() const { return miMaxAngle; }
        LONG GetMinValue() const { return mlMinValue; }
        LONG GetMaxValue() const { return mlMaxValue; }

    protected:

        virtual SIGNED ValToAngle(LONG lVal) = 0;
        virtual void CalcNeedlePos() = 0;
        virtual void DrawTicMarks() = 0;

        virtual void DrawNeedle();
        virtual void DrawAnchor();
        virtual void EraseNeedle();
        virtual void CalcClipAndDraw();
        virtual void DrawDial();
        virtual void CalcTicPos(LONG lVal, PegPoint& tPt1, PegPoint& tPt2);

        LONG mlCurValue;
        SIGNED miCurAngle;
        PegRect mtNeedlePos;
        WORD mwNeedleLen;

        LONG mlTicFreq;
        WORD mwTicLen;
        
        WORD mwAnchorWidth;

        COLORVAL mtNeedleColor;
        COLORVAL mtDialColor;
        COLORVAL mtAnchorColor;
        
        SIGNED miMinAngle;
        SIGNED miMaxAngle;
        LONG mlMinValue;
        LONG mlMaxValue;
        
        BOOL mbValueSet;
};

#endif // _PEGDIAL_HPP_


