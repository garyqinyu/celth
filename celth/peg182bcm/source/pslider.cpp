/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pslider.cpp - Slider control class.
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

#include "stdlib.h"
#include "peg.hpp"

/*--------------------------------------------------------------------------*/
PegSlider::PegSlider(const PegRect &Rect, SIGNED iMin, SIGNED iMax,
    WORD wId, WORD wStyle, SIGNED iScale) : PegThing(Rect, wId, wStyle)
{
    Type(TYPE_SLIDER);
    AddStatus(PSF_SIZEABLE|PSF_TAB_STOP);

    SetSignals(SIGMASK(PSF_SLIDER_CHANGE));
    miMin = iMin;
    miMax = iMax;
    miScale = iScale;
    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;

    InitClient();

    mpSlideButton = new PegSlideButton(iMin, iMax, iScale, wStyle);
    Add(mpSlideButton);
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegSlider::~PegSlider()
{
}

/*--------------------------------------------------------------------------*/
SIGNED PegSlider::Message(const PegMessage &Mesg)
{
    PegMessage NewMessage;
    BOOL bHorizontal = FALSE;

    switch(Mesg.wType)
    {
    case PM_SLIDER_DRAG:
        if (Id() && GetSignals() & SIGMASK(PSF_SLIDER_CHANGE))
        {
            NewMessage.wType = PEG_SIGNAL(Id(), PSF_SLIDER_CHANGE);
            NewMessage.pTarget = Parent();
            NewMessage.pSource = this;
            NewMessage.iData = Id();
            NewMessage.lData = Mesg.lData;
            Parent()->Message(NewMessage);
        }
        break;

    case PM_LBUTTONDOWN:
        {
            NewMessage = Mesg;
            NewMessage.Point.x = mpSlideButton->mReal.wLeft;
            NewMessage.Point.y = mpSlideButton->mReal.wTop;
            mpSlideButton->Message(NewMessage);
            NewMessage = Mesg;
            NewMessage.wType = PM_POINTER_MOVE;
            mpSlideButton->Message(NewMessage);
        }
        break;

   #if defined(PEG_DRAW_FOCUS)
    case PM_CURRENT:
        PegThing::Message(Mesg);
        mpSlideButton->Invalidate();
        mpSlideButton->Draw();
        break;

    case PM_NONCURRENT:
        PegThing::Message(Mesg);
        mpSlideButton->Invalidate();
        mpSlideButton->Draw();
        break;
   #endif

   #ifdef PEG_KEYBOARD_SUPPORT
    case PM_KEY:
        if (mClient.Width() >= mClient.Height())
        {
            bHorizontal = TRUE;
        }
        switch(Mesg.iData)
        {
        case PK_LEFT:
            if (bHorizontal)
            {
                mpSlideButton->KeyMove(-1);
                break;
            }
            else
            {
                return PegThing::Message(Mesg);
            }

        case PK_LNUP:
            if (!bHorizontal)
            {
                mpSlideButton->KeyMove(-1);
                break;
            }
            else
            {
                return PegThing::Message(Mesg);
            }

        case PK_RIGHT:
            if (bHorizontal)
            {
                mpSlideButton->KeyMove(1);
                break;
            }
            else
            {
                return PegThing::Message(Mesg);
            }

        case PK_LNDN:
            if (!bHorizontal)
            {
                mpSlideButton->KeyMove(1);
                break;
            }
            else
            {
                return PegThing::Message(Mesg);
            }

        default:
            return PegThing::Message(Mesg);
        }
        break;
    #endif

    default:
        PegThing::Message(Mesg);
        break;
    }
    return(0);
}


/*--------------------------------------------------------------------------*/
void PegSlider::Draw(void)
{
    if (!(StatusIs(PSF_VISIBLE)))
    {
        return;
    }

    BeginDraw();

    StandardBorder(muColors[PCI_NORMAL]);

    if (miScale != -1)
    {
        DrawScale();
    }

    PegThing::Draw();
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegSlider::DrawScale(void)
{
    PegColor Color(muColors[PCI_NTEXT], 0, 0);
    SIGNED iSpace, iTickStep, iPos, iTickPos;
    SIGNED iNumTicks = 0;

    if (miScale)
    {
        iNumTicks = (abs(miMax - miMin) / miScale) + 1;
    }

    if (mClient.Width() >= mClient.Height())
    {
        // horizontal
        iSpace = mClient.Width() >> 1;
        iTickStep = ((mClient.Width() << 3)  - (iSpace * 2));
        iTickPos = mClient.wBottom - (mClient.Height() / 6);
        iPos = (mClient.wLeft << 3) + iSpace;
        iSpace = iPos >> 3;        // save copy for later
        
        if (iNumTicks > 1)
        {
            iTickStep /= iNumTicks - 1;

            while(iNumTicks--)
            {
                Line(iPos >> 3, iTickPos, iPos >> 3,
                    mClient.wBottom, Color);
                iPos += iTickStep;
            }

            iPos -= iTickStep;
            iPos >>= 3;
        }
        else
        {
            iPos = mClient.wRight - (mClient.Width() >> 4);
        }
        iTickPos = (mClient.wTop + iTickPos) >> 1;
        iTickPos--;
        iSpace -= 3;
        iPos += 3;

        Color.uForeground = PCLR_LOWLIGHT;
        Line(iSpace, iTickPos, iPos, iTickPos, Color);
        iTickPos++;
        Color.uForeground = PCLR_SHADOW;
        Line(iSpace, iTickPos, iPos, iTickPos, Color);
        iTickPos++;
        Color.uForeground = PCLR_HIGHLIGHT;
        Line(iSpace, iTickPos, iPos, iTickPos, Color);
    }
    else
    {
        iSpace = mClient.Height() >> 1;
        iTickStep = ((mClient.Height() << 3) - (iSpace * 2));
        iTickPos = mClient.wLeft + (mClient.Width() / 6);
        iPos = (mClient.wTop << 3) + iSpace;
        iSpace = iPos >> 3;
        
        if (iNumTicks > 1)
        {
            iTickStep /= iNumTicks - 1;

            while(iNumTicks--)
            {
                Line(mClient.wLeft, iPos >> 3,
                    iTickPos, iPos >> 3, Color);
                iPos += iTickStep;
            }
        
            iPos -= iTickStep;
            iPos >>= 3;
        }
        else
        {
            iPos = mClient.wBottom - (mClient.Height() >> 4);
        }
        iTickPos = (iTickPos + mClient.wRight) >> 1;
        iTickPos--;

        iSpace -= 3;
        iPos += 3;

        Color.uForeground = PCLR_LOWLIGHT;
        Line(iTickPos, iSpace, iTickPos, iPos, Color);
        iTickPos++;
        Color.uForeground = PCLR_SHADOW;
        Line(iTickPos, iSpace, iTickPos, iPos, Color);
        iTickPos++;
        Color.uForeground = PCLR_HIGHLIGHT;
        Line(iTickPos, iSpace, iTickPos, iPos, Color);
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlider::SetMinValue(SIGNED iMin)
{
    miMin = iMin;
    mpSlideButton->SetMinValue(iMin);

    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate();
        Draw();
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlider::SetMaxValue(SIGNED iMax)
{
    miMax = iMax;
    mpSlideButton->SetMaxValue(iMax);

    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate();
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlider::Reset(SIGNED iMin, SIGNED iMax, SIGNED iNew)
{
    miMin = iMin;
    miMax = iMax;
    mpSlideButton->Reset(iMin, iMax, iNew);
    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate();
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlider::Resize(PegRect NewSize)
{
    PegThing::Resize(NewSize);
    mpSlideButton->ParentShift(0, 0);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegSlideButton::PegSlideButton(SIGNED iMin, SIGNED iMax, SIGNED iScale,
    WORD wFlags):PegThing(0, wFlags)
{
    miMin = iMin;
    miMax = iMax;
    miScale = iScale;
    miCurrent = miMin;
    AddStatus(PSF_SELECTABLE);
    mbDragMode = FALSE;
    miMinPos = miMaxPos = 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegSlideButton::~PegSlideButton()
{
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSlideButton::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
        // figure out how big I should be:
        SetSize();

    case PM_HIDE:
        PegThing::Message(Mesg);
        mbDragMode = FALSE;
        break;
        
    case PM_LBUTTONDOWN:
        if (!mbDragMode)
        {
            CapturePointer();
            SetDragLimits();
            mbDragMode = TRUE;
            mBeginDrag = Mesg.Point;
        }
        break;

    case PM_LBUTTONUP:
        if (mbDragMode)
        {
            ReleasePointer();
            mbDragMode = FALSE;
        }
        break;
    
    case PM_POINTER_MOVE:
        if (mbDragMode)
        {
            SIGNED iOldCurrent = miCurrent;
            UpdatePosition(Mesg.Point);

            if (miCurrent != iOldCurrent && Parent())
            {
                PegMessage NewMessage;
                NewMessage.wType = PM_SLIDER_DRAG;
                NewMessage.lData = miCurrent;
                Parent()->Message(NewMessage);
            }
        }
        break;

   #ifdef PEG_KEYBOARD_SUPPORT
    case PM_KEY:
        Parent()->Message(Mesg);
        break;
   #endif
    
    default:
        PegThing::Message(Mesg);
        break;        
    }
    return(0);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::Draw(void)
{
    BeginDraw();
    PegColor Color(PCLR_SHADOW, PCLR_LOWLIGHT, 0);

    #ifdef PEG_DRAW_FOCUS
    if (Parent()->StatusIs(PSF_CURRENT))
    {
        Color.uFlags = CF_FILL;
    }
    #endif

    Rectangle(mReal, Color, 1);

    #ifdef PEG_DRAW_FOCUS
    if (Parent()->StatusIs(PSF_CURRENT))
    {
        EndDraw();
        return;
    }
    #endif

    Color.uForeground = PCLR_LOWLIGHT;

    if (Parent()->mClient.Width() >= Parent()->mClient.Height())
    {
        Line(mReal.wLeft + 1, mReal.wTop + 1,
            mReal.wLeft + 1, mReal.wBottom - 1, Color);
        Line(mReal.wRight - 1, mReal.wTop + 1,
            mReal.wRight -1, mReal.wBottom - 1, Color);
        Color.uForeground = PCLR_BUTTON_FACE;
        Line(mReal.wLeft + 2, mReal.wTop + 1,
            mReal.wLeft + 2, mReal.wBottom - 1, Color);
        Line(mReal.wRight - 2, mReal.wTop + 1,
            mReal.wRight -2, mReal.wBottom - 1, Color);
        Color.uForeground = PCLR_HIGHLIGHT;
        Line(mReal.wLeft + 3, mReal.wTop + 1,
            mReal.wLeft + 3, mReal.wBottom - 1, Color, 2);
    }
    else
    {
        Line(mReal.wLeft + 1, mReal.wTop + 1,
            mReal.wRight - 1, mReal.wTop + 1, Color);
        Line(mReal.wLeft + 1, mReal.wBottom - 1,
            mReal.wRight - 1, mReal.wBottom - 1, Color);
        Color.uForeground = PCLR_BUTTON_FACE;
        Line(mReal.wLeft + 1, mReal.wTop + 2,
            mReal.wRight - 1, mReal.wTop + 2, Color);
        Line(mReal.wLeft + 1, mReal.wBottom - 2,
            mReal.wRight - 1, mReal.wBottom - 2, Color);
        Color.uForeground = PCLR_HIGHLIGHT;
        Line(mReal.wLeft + 1, mReal.wTop + 3,
            mReal.wRight - 1, mReal.wTop + 3, Color, 2);
    }
    EndDraw();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::ParentShift(SIGNED, SIGNED)
{
    SetSize();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::SetMinValue(SIGNED iMin)
{
    miMin = iMin;
    RecalcCurrentPos(FALSE);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::SetMaxValue(SIGNED iMax)
{
    miMax = iMax;
    RecalcCurrentPos(FALSE);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::SetCurrentValue(SIGNED iNewVal, BOOL bDraw)
{
    miCurrent = iNewVal;
    RecalcCurrentPos(bDraw);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::Reset(SIGNED iMin, SIGNED iMax, SIGNED iNew)
{
    miMin = iMin;
    miMax = iMax;
    miCurrent = iNew;
    RecalcCurrentPos(FALSE);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::UpdatePosition(PegPoint NewPoint)
{
    SIGNED Delta;
    BOOL bVertical = TRUE;
    LONG dVal = miCurrent;

    if (Parent()->mClient.Width() >= Parent()->mClient.Height())
    {
        bVertical = FALSE;
    }

    if (Parent()->Style() & SF_SNAP)
    {
        CheckSnap(NewPoint, bVertical);
        return;
    }

    if (!bVertical)
    {
        Delta = NewPoint.x - mBeginDrag.x;

        if (Delta)
        {
            UpdateHorzPos(Delta, TRUE);
 	        dVal = miMax - miMin;
            dVal *= mReal.wLeft - miMinPos;
            dVal /= miMaxPos - miMinPos - 7;
   	        dVal += miMin;
        }
    }
    else
    {
        Delta = NewPoint.y - mBeginDrag.y;

        if (Delta)
        {
            UpdateVertPos(Delta, TRUE);
  	        dVal  = miMax - miMin;
            dVal *= mReal.wTop - miMinPos;
            dVal /= miMaxPos - miMinPos - 7;
   	        dVal += miMin;
        }
    }
    miCurrent = (SIGNED) dVal;
    mBeginDrag = NewPoint;
}

#ifdef PEG_KEYBOARD_SUPPORT
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::KeyMove(SIGNED iSign)
{
    SIGNED OldVal = miCurrent;
    LONG lStep = 0;
    LONG dVal;
    BOOL bVertical = TRUE;

    if (Parent()->mClient.Width() >= Parent()->mClient.Height())
    {
        bVertical = FALSE;
    }

    dVal = miMax - miMin;

    if (Parent()->Style() & SF_SNAP)
    {
        // calculate value change, then snap to position:

        if (miMax < miMin)
        {
            iSign = -iSign;
        }
        miCurrent += miScale * iSign;
        miCurrent = CheckValLimits(miCurrent);

        if (miCurrent != OldVal)
        {
            RecalcCurrentPos(TRUE);
        }
    }                
    else
    {

	    if (!bVertical)
        {
            lStep = Parent()->mClient.Width() / 20;
            UpdateHorzPos(lStep * iSign, TRUE);
		    dVal *= mReal.wLeft - miMinPos;
        }
	    else
	    {
            lStep = Parent()->mClient.Height() / 20;
            UpdateVertPos(lStep * iSign, TRUE);
		    dVal *= mReal.wTop - miMinPos;
        }
        dVal /= miMaxPos - miMinPos - 7;
        dVal += miMin;
        miCurrent = (SIGNED) dVal;
    }

	if (miCurrent != OldVal)
	{
		if (Parent())
		{
		    PegMessage NewMessage;
			NewMessage.wType = PM_SLIDER_DRAG;
			NewMessage.lData = miCurrent;
			Parent()->Message(NewMessage);
        }
    }
}

#endif


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::CheckSnap(PegPoint NewPoint, BOOL bAmVertical)
{
    SIGNED Delta;
    PegRect mPos = mReal;
    if (bAmVertical)
    {
        Delta = NewPoint.y - ((mPos.wTop + mPos.wBottom) / 2);
        mPos.Shift(0, Delta);
    }
    else
    {
        Delta = NewPoint.x - ((mPos.wLeft + mPos.wRight) / 2);
        mPos.Shift(Delta, 0);
    }
    LONG lVal = miMax - miMin;
    SIGNED iNumTicks = (abs(lVal) / miScale);
    LONG lStep;

    if (iNumTicks)
    {
        lStep = lVal / iNumTicks;
    }
    else
    {
        lStep = lVal;
    }

    if (bAmVertical)
    {
        lVal *= mPos.wTop - miMinPos;
    }
    else
    {
        lVal *= mPos.wLeft - miMinPos;
    }

    lVal /= miMaxPos - miMinPos - 7;
    lVal += miMin + abs(lStep / 2);
    LONG lCount = lVal / lStep;
    lVal = lStep * lCount;
    lVal = CheckValLimits((SIGNED) lVal);

    if (miCurrent != (SIGNED) lVal)
    {
        miCurrent = (SIGNED) lVal;
        RecalcCurrentPos(TRUE);
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSlideButton::CheckValLimits(SIGNED iVal)
{
    if (miMax >= miMin)
    {
        if (iVal > miMax)
        {
            iVal = miMax;
        }
        if (iVal < miMin)
        {
            iVal = miMin;
        }
    }
    else
    {
        if (iVal < miMax)
        {
            iVal = miMax;
        }
        if (iVal > miMin)
        {
            iVal = miMin;
        }
    }
    return iVal;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::UpdateHorzPos(SIGNED Delta, BOOL bDraw)
{
	if (Delta > 0)
	{
	    if (mReal.wRight + Delta > miMaxPos)
	    {
	        Delta = miMaxPos - mReal.wRight;
	    }
	}
	else
	{
	    if (mReal.wLeft + Delta < miMinPos)
	    {
	        Delta = miMinPos - mReal.wLeft;
	    }
	}
	
	if (Delta)
	{
        if (bDraw)
        {
            PegRect NewSize = mReal;
            NewSize.Shift(Delta, 0);
            Resize(NewSize);

            if (Parent()->Style() & AF_TRANSPARENT)
            {
                Parent()->Parent()->Draw();
            }
            else
            {
                Parent()->Draw();
            }
        }
        else
        {
		    mReal.Shift(Delta, 0);
		    mClient.Shift(Delta, 0);
		    mClip = mReal & Parent()->mClip;
        }
	}
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::UpdateVertPos(SIGNED Delta, BOOL bDraw)
{
	if (Delta > 0)
	{
	    if (mReal.wBottom + Delta > miMaxPos)
	    {
	        Delta = miMaxPos - mReal.wBottom;
	    }
	}
	else
	{
	    if (mReal.wTop + Delta < miMinPos)
	    {
	        Delta = miMinPos - mReal.wTop;
	    }
	}
	
	if (Delta)
	{
        if (bDraw)
        {
            PegRect NewSize = mReal;
            NewSize.Shift(0, Delta);
            Resize(NewSize);
            if (Parent()->Style() & AF_TRANSPARENT)
            {
                Parent()->Parent()->Draw();
            }
            else
            {
                Parent()->Draw();
            }
        }
        else
        {
		    mReal.Shift(0, Delta);
		    mClient.Shift(0, Delta);
		    mClip = mReal & Parent()->mClip;
        }
	}
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::RecalcCurrentPos(BOOL bDraw)
{
    SIGNED Delta;
    LONG lVal = miCurrent - miMin;
    lVal <<= 8;
    if (miMax != miMin)
    {
        lVal /= (miMax - miMin);     // total span
    }
    lVal *= miMaxPos - miMinPos - 7;
    lVal >>= 8;
    lVal += miMinPos;

    if (Parent()->mClient.Width() >= Parent()->mClient.Height())
    {
        if ((SIGNED) lVal != mReal.wLeft)
        {
            Delta = (SIGNED) lVal - mReal.wLeft;
            UpdateHorzPos(Delta, bDraw);
        }
    }
    else
    {
        if ((SIGNED) lVal != mReal.wTop)
        {
            Delta = (SIGNED) lVal - mReal.wTop;
            UpdateVertPos(Delta, bDraw);
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::SetSize()
{
    if (!Parent())
    {
        return;
    }

    SetDragLimits();

    if (Parent()->mClient.Width() >= Parent()->mClient.Height())
    {
        mReal.wLeft = miMinPos;
        mReal.wRight = mReal.wLeft + 7;
        mReal.wTop = Parent()->mClient.wTop + 2;
        mReal.wBottom = Parent()->mClient.wBottom - (Parent()->mClient.Height() / 6) - 2;
    }
    else
    {
        mReal.wTop = miMinPos;
        mReal.wTop += Parent()->mClient.Height() >> 4;
        mReal.wBottom = mReal.wTop + 7;
        mReal.wRight = Parent()->mClient.wRight - 2;
        mReal.wLeft = Parent()->mClient.wLeft + (Parent()->mClient.Width() / 6) + 3;
    }

    RecalcCurrentPos(FALSE);
    mClient = mReal;
    mClip = mReal & Parent()->mClip;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSlideButton::SetDragLimits(void)
{
    if (Parent()->mClient.Width() >= Parent()->mClient.Height())
    {
        miMinPos = Parent()->mClient.wLeft - 3;
        miMinPos += Parent()->mClient.Width() >> 4;
        miMaxPos = Parent()->mClient.wRight + 3;
        miMaxPos -= Parent()->mClient.Width() >> 4;
    }
    else
    {
        miMinPos = Parent()->mClient.wTop - 3;
        miMinPos += Parent()->mClient.Height() >> 4;
        miMaxPos = Parent()->mClient.wBottom + 3;
        miMaxPos -= Parent()->mClient.Height() >> 4;
    }
}


// End of file
