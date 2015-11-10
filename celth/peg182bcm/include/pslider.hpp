/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pslider.hpp - Slider control class definition.
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGSLIDER_
#define _PEGSLIDER_


/*--------------------------------------------------------------------------*/
class PegSlideButton : public PegThing
{
    public:
        PegSlideButton(SIGNED iMin, SIGNED iMax, SIGNED iScale, 
            WORD wFlags = 0);
        ~PegSlideButton();
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        void ParentShift(SIGNED xShift, SIGNED yShift);

        void SetMinValue(SIGNED iMin);
        void SetMaxValue(SIGNED iMax);
        void SetCurrentValue(SIGNED iNewVal, BOOL bDraw = FALSE);
        SIGNED GetCurrentValue(void) {return miCurrent;}
        void Reset(SIGNED iMin, SIGNED iMax, SIGNED iNew);
        void KeyMove(SIGNED iDirection);
        
    protected:
        void SetSize(void);
        void UpdatePosition(PegPoint Point);
        void RecalcCurrentPos(BOOL bDraw);
        void UpdateVertPos(SIGNED Delta, BOOL bDraw);
        void UpdateHorzPos(SIGNED Delta, BOOL bDraw);
        void SetDragLimits(void);
        void CheckSnap(PegPoint DragPos, BOOL bVertical);
        SIGNED CheckValLimits(SIGNED iVal);

        BOOL  mbDragMode;
        SIGNED miMin;
        SIGNED miMax;
        SIGNED miScale;
        SIGNED miCurrent;
        SIGNED miMinPos;
        SIGNED miMaxPos;
        PegPoint mBeginDrag;
};


/*--------------------------------------------------------------------------*/
class PegSlider : public PegThing
{
    public:
        PegSlider(const PegRect &Rect, SIGNED iMin, SIGNED iMax,
            WORD wId = 0, WORD wStyle = FF_RAISED,
            SIGNED iScale = -1);
        ~PegSlider();
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void Draw(void);
        virtual void Resize(PegRect NewSize);

        void SetMinValue(SIGNED iMin);
        void SetMaxValue(SIGNED iMax);
        void SetScale(SIGNED iScale) {miScale = iScale;}
        SIGNED GetMinValue(void) {return miMin;}
        SIGNED GetMaxValue(void) {return miMax;}
        SIGNED GetScale(void)    {return miScale;}
        void SetCurrentValue(SIGNED iNewVal, BOOL bDraw = FALSE)
        {
            mpSlideButton->SetCurrentValue(iNewVal, bDraw);
        }
        SIGNED GetCurrentValue(void)
        {
            return mpSlideButton->GetCurrentValue();
        }
        void Reset(SIGNED iMin, SIGNED iMax, SIGNED iNew);
        
    protected:
        void DrawScale(void);
        PegSlideButton *mpSlideButton;
        SIGNED miMin;
        SIGNED miMax;
        SIGNED miScale;
};

#endif

