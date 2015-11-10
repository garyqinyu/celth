/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pegdial.hpp - Implementation of the PegDial classes
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

#include "peg.hpp"

#if defined(PEG_HMI_GADGETS)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegDial::PegDial(const PegRect& Rect, WORD wStyle) :
    PegThing(Rect, 0, wStyle),
    miCurAngle(0),
    mwNeedleLen(80),
    mlTicFreq(0),
    mwTicLen(10),
    mwAnchorWidth(DS_ANCHORWIDTH),
    mtAnchorColor(DS_ANCHORCOLOR),
    mbValueSet(FALSE)
{
    if (!(wStyle & AF_TRANSPARENT))
    {
        AddStatus(PSF_VIEWPORT);
    }
    
   #if PEG_NUM_COLORS >= 16 
    mtNeedleColor = RED;
   #else
    mtNeedleColor = WHITE;
   #endif

    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
	mtDialColor = muColors[PCI_NORMAL];
    
    if(!(mwStyle & FF_MASK))
    {
        mwStyle |= FF_NONE;
    }

    InitClient();
}

/*--------------------------------------------------------------------------*/
PegDial::PegDial(SIGNED iLeft, SIGNED iTop, WORD wStyle) :
    PegThing(0, wStyle),
    miCurAngle(0),
    mwNeedleLen(80),
    mlTicFreq(0),
    mwTicLen(0),
    mwAnchorWidth(DS_ANCHORWIDTH),
    mtAnchorColor(DS_ANCHORCOLOR),
    mbValueSet(FALSE)
{
    if (!(wStyle & AF_TRANSPARENT))
    {
        AddStatus(PSF_VIEWPORT);
    }

   #if PEG_NUM_COLORS >= 16 
    mtNeedleColor = RED;
   #else
    mtNeedleColor = WHITE;
   #endif

    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
	mtDialColor = muColors[PCI_NORMAL];
    
    if(!(mwStyle & FF_MASK))
    {
        mwStyle |= FF_NONE;
    }

    mReal.Set(iLeft, iTop, iLeft + 100, iTop + 100);

    InitClient();
}

/*--------------------------------------------------------------------------*/
PegDial::~PegDial()
{
}

/*--------------------------------------------------------------------------*/
void PegDial::SetValue(LONG lValue, BOOL bRedraw /*=TRUE*/)
{
    if(mlCurValue == lValue)
    {
        return;
    }
    
    if(lValue < mlMinValue && mlCurValue == mlMinValue)
    {
        return;
    }
    
    if(lValue > mlMaxValue && mlCurValue == mlMaxValue)
    {
        return;
    }
        
    if(lValue >= mlMinValue && lValue <= mlMaxValue)
    {
        mlCurValue = lValue;
    }
    else if(lValue <= mlMinValue)
    {
        mlCurValue = mlMinValue;
    }
    else if(lValue >= mlMaxValue)
    {
        mlCurValue = mlMaxValue;
    }

    if(StatusIs(PSF_VISIBLE) && bRedraw)
    {
		CalcClipAndDraw();
    }
}

/*--------------------------------------------------------------------------*/
LONG PegDial::IncrementValue(LONG lValue, BOOL bRedraw /*=TRUE*/)
{
    LONG lSet = mlCurValue + lValue;
    
    if(lSet <= mlMinValue)
    {
        if(mlCurValue == mlMinValue)
        {
            return(mlCurValue);
        }
        else
        {
            mlCurValue = mlMinValue;
        }
    }    
    else if(lSet >= mlMaxValue)
    {
        if(mlCurValue == mlMaxValue)
        {
            return(mlCurValue);
        }
        else
        {
            mlCurValue = mlMaxValue;
        }
    }
    else
    {
        mlCurValue = lSet;
    }

    if(StatusIs(PSF_VISIBLE) && bRedraw)
    {
		CalcClipAndDraw();
    }

    return mlCurValue;
}

/*--------------------------------------------------------------------------*/
void PegDial::DrawNeedle()
{
    //BeginDraw();

    PegColor Color(mtNeedleColor, mtNeedleColor, CF_FILL);

    if(mwStyle & DS_THINNEEDLE)
    {
        Line(mtNeedlePos.wLeft, mtNeedlePos.wTop, mtNeedlePos.wRight,
             mtNeedlePos.wBottom, Color);
    }
    else if(mwStyle & DS_THICKNEEDLE)
    {
        Line(mtNeedlePos.wLeft, mtNeedlePos.wTop, mtNeedlePos.wRight,
             mtNeedlePos.wBottom, Color, DS_THICKNEEDLEWIDTH);
    }
    else if(mwStyle & DS_POLYNEEDLE)
    {
	    WORD wRadius;

		if(mwStyle & DS_RECTCOR)
		{
			wRadius = ((PEGMIN((mClient.Width() >> 1), (mClient.Height() >> 1)))
    				    * mwNeedleLen) / 100;
		}
		else
		{
			wRadius = ((PEGMAX(mClient.Width(), mClient.Height())) * mwNeedleLen) / 100;

		}

		WORD wInnerRadius = (WORD)(wRadius / 3);

		PegPoint tPolyPoints[5];
		tPolyPoints[0].x = mtNeedlePos.wRight;
		tPolyPoints[0].y = mtNeedlePos.wBottom;
		tPolyPoints[4].x = tPolyPoints[0].x;
		tPolyPoints[4].y = tPolyPoints[0].y;
		tPolyPoints[2].x = mtNeedlePos.wLeft;
		tPolyPoints[2].y = mtNeedlePos.wTop;

		SIGNED iSin, iCos;
		SIGNED iAngle = miCurAngle - 15;
		PegLookupSinCos(iAngle, &iSin, &iCos);
		tPolyPoints[1].x = mtNeedlePos.wRight + (WORD)((iCos * wInnerRadius) >> 10);
		tPolyPoints[1].y = mtNeedlePos.wBottom - (WORD)((iSin * wInnerRadius) >> 10);

		iAngle = miCurAngle + 15;
		PegLookupSinCos(iAngle, &iSin, &iCos);
		tPolyPoints[3].x = mtNeedlePos.wRight + (WORD)((iCos * wInnerRadius) >> 10);
		tPolyPoints[3].y = mtNeedlePos.wBottom - (WORD)((iSin * wInnerRadius) >> 10);

		Color.uForeground = PCLR_SHADOW;
		Screen()->Polygon(this, tPolyPoints, 5, Color, 1);
    }

    //EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegDial::DrawAnchor()
{
    PegColor tColor(mtAnchorColor, mtAnchorColor, CF_FILL);
    Circle(mtNeedlePos.wRight, mtNeedlePos.wBottom, mwAnchorWidth, tColor, 1);
}

/*--------------------------------------------------------------------------*/
void PegDial::EraseNeedle()
{
    BeginDraw();

    PegColor Color(mtDialColor, mtDialColor, CF_FILL);

    if(mwStyle & DS_THINNEEDLE)
    {
        Line(mtNeedlePos.wLeft, mtNeedlePos.wTop, mtNeedlePos.wRight,
             mtNeedlePos.wBottom, Color);
    }
    else if(mwStyle & DS_THICKNEEDLE)
    {
        Line(mtNeedlePos.wLeft, mtNeedlePos.wTop, mtNeedlePos.wRight,
             mtNeedlePos.wBottom, Color, DS_THICKNEEDLEWIDTH);
    }
    else if(mwStyle & DS_POLYNEEDLE)
    {
    }

    Color.Set(mtAnchorColor, mtAnchorColor, CF_FILL);
    Circle(mtNeedlePos.wRight, mtNeedlePos.wBottom, mwAnchorWidth, Color, 1);

    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegDial::CalcClipAndDraw()
{
    PegRect tOldClip = mClip;
	mClip = mtNeedlePos;

    // Normal the clip rect
    if(mClip.wTop > mClip.wBottom)
    {
        PEGSWAP(mClip.wTop, mClip.wBottom);
    }
    if(mClip.wLeft > mClip.wRight)
    {
        PEGSWAP(mClip.wLeft, mClip.wRight);
    }

    CalcNeedlePos();

    // We can't normal mtNeedlePos, so we'll copy it
    // out and normalize the result.
    PegRect tNewRect = mtNeedlePos;

    if(tNewRect.wTop > tNewRect.wBottom)
    {
        PEGSWAP(tNewRect.wTop, tNewRect.wBottom);
    }
    if(tNewRect.wLeft > tNewRect.wRight)
    {
        PEGSWAP(tNewRect.wLeft, tNewRect.wRight);
    }
    
    // If the new position of the needle is the same,
    // don't do anything.
    if(mClip == tNewRect)
    {
        mClip = tOldClip;
        return;
    }
    
    // Here's why we normaled. If these 2 rects aren't
    // normaled, we can get some unusable results.
    mClip |= tNewRect;
    
    // Pad the rectangle a bit to handle a thick line
	if(mwStyle & DS_THICKNEEDLE)
	{
		mClip += DS_THICKNEEDLEWIDTH;
	}
	else if(mwStyle & DS_POLYNEEDLE)
	{
		mClip += 10;
	}
	else // DS_THINNEEDLE
	{
		mClip += 2;
	}

	// Changing the value caused this draw. The ::Draw method
    // will handle this a little differently. Notably, it
    // won't do another CalcNeedlePos...
    mbValueSet = TRUE;
    
    Invalidate(mClip);
    Draw();
    
    mbValueSet = FALSE;
    
    mClip = tOldClip;
}

/*--------------------------------------------------------------------------*/
void PegDial::DrawDial()
{
    SIGNED iRadius = PEGMIN((mClient.Width() >> 1), (mClient.Height() >> 1));
    PegPoint Point;
    Point.x = mClient.wLeft + (mClient.Width() >> 1);
    Point.y = mClient.wTop + (mClient.Height() >> 1);

    PegColor Color(PCLR_SHADOW, PCLR_SHADOW);
    Circle(Point.x, Point.y, iRadius, Color, 1);

    Color.Set(PCLR_HIGHLIGHT, PCLR_HIGHLIGHT);
    Circle(Point.x, Point.y, --iRadius, Color, 1);
    Circle(Point.x, Point.y, --iRadius, Color, 1);

    Color.Set(PCLR_LOWLIGHT, mtDialColor, CF_FILL);
    Circle(Point.x, Point.y, --iRadius, Color, 1);
}

/*--------------------------------------------------------------------------*/
void PegDial::CalcTicPos(LONG lVal, PegPoint& tPt1, PegPoint& tPt2)
{
    if(lVal < mlMinValue || lVal > mlMaxValue)
    {
        return;
    }

    SIGNED iAngle = ValToAngle(lVal);

    SIGNED iRadius = (SIGNED)(PEGMIN(((mClient.Width() >> 1) - 4), 
                                  ((mClient.Height() >> 1) - 4)));  
                                  // the 4 is from drawing the dial

    PegPoint tCenter;
    tCenter.x = (mClient.wLeft + (mClient.Width() >> 1));
    tCenter.y = (mClient.wTop + (mClient.Height() >> 1));

    SIGNED iSin, iCos;
    PegLookupSinCos(iAngle, &iSin, &iCos);
    tPt1.x = tCenter.x + (WORD)((iCos * iRadius) >> 10);
    tPt1.y = tCenter.y - (WORD)((iSin * iRadius) >> 10);

	iRadius -= ((iRadius * mwTicLen) / 100);

    tPt2.x = tCenter.x + (WORD)((iCos * iRadius) >> 10);
    tPt2.y = tCenter.y - (WORD)((iSin * iRadius) >> 10);
}

#endif	// PEG_HMI_GADGETS


