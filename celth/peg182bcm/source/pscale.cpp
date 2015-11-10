/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pscale.cpp - Base for the PegScale classes
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
PegScale::PegScale(const PegRect &Size, 
                   LONG lMinValue, LONG lMaxValue, WORD wStyle) :
    PegThing(Size, 0, wStyle),
    mbValueSet(FALSE)
{
    AddStatus(PSF_VIEWPORT);

    mlMinValue = lMinValue;
    mlMaxValue = lMaxValue;
    mlCurValue = lMinValue;

   #if PEG_NUM_COLORS >= 16
    mtNeedleColor = RED;
   #else
    mtNeedleColor = WHITE;
   #endif    
    
    muColors[PCI_NORMAL] = PCLR_DIALOG;

    if(!(mwStyle & FF_MASK))
    {
        mwStyle |= FF_NONE;
    }

    InitClient();
    
    // Init the needle position
    ValToPoint(mlCurValue, mtNeedlePoint);
}

/*--------------------------------------------------------------------------*/
PegScale::PegScale(SIGNED iLeft, SIGNED iTop, LONG lMinValue,
                   LONG lMaxValue, WORD wStyle) :
    PegThing(0, wStyle),
    mbValueSet(FALSE)
{
    AddStatus(PSF_VIEWPORT);

    mlMinValue = lMinValue;
    mlMaxValue = lMaxValue;
    mlCurValue = lMinValue;

   #if PEG_NUM_COLORS >= 16
    mtNeedleColor = RED;
   #else
    mtNeedleColor = WHITE;
   #endif    
    
    muColors[PCI_NORMAL] = PCLR_DIALOG;

    if(!(mwStyle & FF_MASK))
    {
        mwStyle |= FF_NONE;
    }

    mReal.Set(iLeft, iTop, iLeft + 100, iTop + 150);

    InitClient();
    
    // Init the needle position
    ValToPoint(mlCurValue, mtNeedlePoint);
}

/*--------------------------------------------------------------------------*/
PegScale::~PegScale()
{
}


/*--------------------------------------------------------------------------*/
void PegScale::SetValue(LONG lVal, BOOL bRedraw /*=TRUE*/)
{
    if(mlCurValue == lVal)
    {
        return;
    }
    
    if(lVal < mlMinValue && mlCurValue == mlMinValue)
    {
        return;
    }
    
    if(lVal > mlMaxValue && mlCurValue == mlMaxValue)
    {
        return;
    }
        
    if(lVal >= mlMinValue && lVal <= mlMaxValue)
    {
        mlCurValue = lVal;
    }
    else if(lVal <= mlMinValue)
    {
        mlCurValue = mlMinValue;
    }
    else if(lVal >= mlMaxValue)
    {
        mlCurValue = mlMaxValue;
    }
    
    if(StatusIs(PSF_VISIBLE) && bRedraw)
    {
        CalcClipAndDraw();
    }
}

/*--------------------------------------------------------------------------*/
LONG PegScale::IncrementValue(LONG lVal, BOOL bRedraw /*=TRUE*/)
{
    LONG lSet = mlCurValue + lVal;
    
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

    return(mlCurValue);
}

/*--------------------------------------------------------------------------*/
void PegScale::SetMinValue(LONG lVal, BOOL bRedraw /*=TRUE*/)
{
    if(mlMinValue == lVal)
    {
        return;
    }
    
    mlMinValue = lVal;

    if(StatusIs(PSF_VISIBLE) && bRedraw)
    {
        CalcClipAndDraw();
    }
}

/*--------------------------------------------------------------------------*/
void PegScale::SetMaxValue(LONG lVal, BOOL bRedraw /*=TRUE*/)
{
    if(mlMaxValue == lVal)
    {
        return;
    }
    
    mlMaxValue = lVal;

    if(StatusIs(PSF_VISIBLE) && bRedraw)
    {
        CalcClipAndDraw();
    }
}

/*--------------------------------------------------------------------------*/
void PegScale::DrawNeedle()
{
    SIGNED iWidth = mClient.Width();
    SIGNED iHeight = mClient.Height();
    //PegColor tColor(mtNeedleColor, mtNeedleColor, CF_FILL);
	PegColor tColor(PCLR_SHADOW, mtNeedleColor, CF_FILL);

    if(mwStyle & SS_POLYNEEDLE)
    {
	    PegPoint tPoint[4];
	    
	    if(!mbValueSet)
	    {
            ValToPoint(mlCurValue, tPoint[1]);
        }
        else
        {
            tPoint[1].x = mtNeedlePoint.x;
            tPoint[1].y = mtNeedlePoint.y;
        }
        
        if(mwStyle & SS_ORIENTVERT)
        {
            SIGNED iNeedleHeight = iHeight >> 3;
            
            if(mwStyle & SS_FACELEFT)
            {
                tPoint[1].x = mClient.wLeft + (iWidth >> 2);
                tPoint[2].x = tPoint[1].x + (iWidth >> 3);
                tPoint[2].y = tPoint[1].y;
                Line(tPoint[1].x, tPoint[1].y, tPoint[2].x, tPoint[2].y, tColor, 1);

                tPoint[1].x += (iWidth >> 1) + (iWidth >> 3);
                tPoint[1].y -= (iHeight >> 4);
            }
            else
            {
                tPoint[1].x = mClient.wRight - (iWidth >> 2);
                tPoint[2].x = tPoint[1].x - (iWidth >> 3);
                tPoint[2].y = tPoint[1].y;
                Line(tPoint[1].x, tPoint[1].y, tPoint[2].x, tPoint[2].y, tColor, 1);

                tPoint[1].x -= ((iWidth >> 1) + (iWidth >> 3));
                tPoint[1].y -= (iHeight >> 4);
            }

            tPoint[0].x = tPoint[1].x;
            tPoint[0].y = tPoint[1].y + iNeedleHeight;
            tPoint[3].x = tPoint[0].x;
            tPoint[3].y = tPoint[0].y;
            
            Screen()->Polygon(this, tPoint, 4, tColor, 1);
        }
        else        // oriented horizontally
        {
            SIGNED iNeedleWidth = iWidth >> 3;
            
            if(mwStyle & SS_FACETOP)
            {
                tPoint[1].y = mClient.wTop + (iHeight >> 2);
                tPoint[2].y = tPoint[1].y + (iHeight >> 3);
                tPoint[2].x = tPoint[1].x;
                Line(tPoint[1].x, tPoint[1].y, tPoint[2].x, tPoint[2].y, tColor, 1);

                tPoint[1].y += ((iHeight >> 1) + (iHeight >> 3));
                tPoint[1].x -= (iWidth >> 4);
            }
            else
            {
                tPoint[1].y = mClient.wBottom - (iHeight >> 2);
                tPoint[2].y = tPoint[1].y - (iHeight >> 3);
                tPoint[2].x = tPoint[1].x;
                Line(tPoint[1].x, tPoint[1].y, tPoint[2].x, tPoint[2].y, tColor, 1);

                tPoint[1].y -= ((iHeight >> 1) + (iHeight >> 3));
                tPoint[1].x -= (iWidth >> 4);
            }

            tPoint[0].y = tPoint[1].y;
            tPoint[0].x = tPoint[1].x + iNeedleWidth;
            tPoint[3].x = tPoint[0].x;
            tPoint[3].y = tPoint[0].y;
            
            Screen()->Polygon(this, tPoint, 4, tColor, 1);
        }
    }
    else if(mwStyle & SS_THICKNEEDLE || mwStyle & SS_THINNEEDLE)
    {
    	PegPoint tPoint1, tPoint2;
        tColor.uForeground = mtNeedleColor;
	    
	    if(!mbValueSet)
	    {
	        ValToPoint(mlCurValue, tPoint1);
	    }
	    else
	    {
	        tPoint1.x = mtNeedlePoint.x;
	        tPoint1.y = mtNeedlePoint.y;
	    }
	            
        if(mwStyle & SS_ORIENTVERT)
        {
            if(mwStyle & SS_FACELEFT)
            {
                tPoint1.x = mClient.wLeft + (iWidth >> 2);
                tPoint2.x = tPoint1.x + (iWidth >> 1) + (iWidth >> 2) 
                            - (iWidth >> 3);
            }
            else
            {
                tPoint1.x = mClient.wRight - (iWidth >> 2);
                tPoint2.x = tPoint1.x - ((iWidth >> 1) + (iWidth >> 2)
                            - (iWidth >> 3));
            }

            tPoint2.y = tPoint1.y;

            Line(tPoint1.x, tPoint1.y, tPoint2.x, tPoint2.y, tColor, 1);
            
            if(mwStyle & SS_THICKNEEDLE)
            {
                tPoint1.y--;
                tPoint2.y--;
                Line(tPoint1.x, tPoint1.y, tPoint2.x, tPoint2.y, tColor, 1);
                tPoint1.y += 2;
                tPoint2.y += 2;
                Line(tPoint1.x, tPoint1.y, tPoint2.x, tPoint2.y, tColor, 1);
                tPoint1.y -= 3;
                tPoint2.y -= 3;
                if(mwStyle & SS_FACELEFT)
                {
                    tPoint1.x += (iWidth >> 4);
                }
                else
                {
                    tPoint1.x -= (iWidth >> 4);
                }
                Line(tPoint1.x, tPoint1.y, tPoint2.x, tPoint2.y, tColor, 1);
                tPoint1.y += 4;
                tPoint2.y += 4;
                Line(tPoint1.x, tPoint1.y, tPoint2.x, tPoint2.y, tColor, 1);
            }
        }
        else
        {
            if(mwStyle & SS_FACELEFT)
            {
                tPoint1.y = mClient.wTop + (iHeight >> 2);
                tPoint2.y = tPoint1.y + (iHeight >> 1) + (iHeight >> 2)
                            - (iHeight >> 3);
            }
            else
            {
                tPoint1.y = mClient.wBottom - (iHeight >> 2);
                tPoint2.y = tPoint1.y - (iHeight >> 1) + (iHeight >> 2)
                            - (iHeight >> 3);
            }

            tPoint2.x = tPoint1.x;

            Line(tPoint1.x, tPoint1.y, tPoint2.x, tPoint2.y, tColor, 1);

            if(mwStyle & SS_THICKNEEDLE)
            {
                tPoint1.x--;
                tPoint2.x--;
                Line(tPoint1.x, tPoint1.y, tPoint2.x, tPoint2.y, tColor, 1);
                tPoint1.x += 2;
                tPoint2.x += 2;
                Line(tPoint1.x, tPoint1.y, tPoint2.x, tPoint2.y, tColor, 1);
                if(mwStyle & SS_FACELEFT)
                {
                    tPoint1.y -= (iHeight >> 4);
                }
                else
                {
                    tPoint1.y += (iHeight >> 4);
                }
                tPoint1.x -= 3;
                tPoint2.x -= 3;
                Line(tPoint1.x, tPoint1.y, tPoint2.x, tPoint2.y, tColor, 1);
                tPoint1.x += 4;
                tPoint2.x += 4;
                Line(tPoint1.x, tPoint1.y, tPoint2.x, tPoint2.y, tColor, 1);
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegScale::ValToPoint(LONG lVal, PegPoint& tPoint)
{
    SetTravelPoints();
    
    if(mwStyle & SS_ORIENTVERT)
    {
        LONG lTotalTravel = (LONG)(miMinTravel - miMaxTravel);
        LONG lTotalVal = mlMaxValue - mlMinValue;
    
        LONG lStep = (lTotalTravel << 8) / (lTotalVal - 1);
        LONG lCurStep = (lVal - mlMinValue) * lStep;

        if(mwStyle & SS_BOTTOMTOTOP)
        {
            tPoint.y = miMinTravel - (SIGNED)(lCurStep >> 8);
            tPoint.x = 0;
        }
        else
        {
            tPoint.y = miMaxTravel + (SIGNED)(lCurStep >> 8);
            tPoint.x = 0;
        }
    }
    else
    {
        LONG lTotalTravel = (LONG)(miMaxTravel - miMinTravel);
        LONG lTotalVal = mlMaxValue - mlMinValue;
    
        LONG lStep = (LONG)(lTotalTravel << 8) / (lTotalVal - 1);
        LONG lCurStep = (lVal - mlMinValue) * lStep;

        if(mwStyle & SS_BOTTOMTOTOP)
        {
            tPoint.x = miMinTravel + (SIGNED)(lCurStep >> 8);
            tPoint.y = 0;
        }
        else
        {
            tPoint.x = miMaxTravel - (SIGNED)(lCurStep >> 8);
            tPoint.y = 0;
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegScale::SetTravelPoints()
{
    if(mwStyle & SS_ORIENTVERT)
    {
        miMinTravel = mClient.wBottom - (mClient.Height() >> 4);
        miMaxTravel = mClient.wTop + (mClient.Height() >> 4);
    }
    else
    {
        miMinTravel = mClient.wLeft + (mClient.Width() >> 4);
        miMaxTravel = mClient.wRight - (mClient.Width() >> 4);
    }
}

/*--------------------------------------------------------------------------*/
void PegScale::CalcClipAndDraw()
{
    // Only invalidate the area that needs to be redrawn
    PegRect tOldRect;
    CalcNeedleRect(mtNeedlePoint, tOldRect);
    
    PegRect tNewRect;
    PegPoint tNewPoint;
    ValToPoint(mlCurValue, tNewPoint);
    
    if(mtNeedlePoint.x == tNewPoint.x &&
       mtNeedlePoint.y == tNewPoint.y)
    {
        // Even though the data may have changed
        // the needle didn't move, so don't do
        // anything.
        return;
    }
    
    CalcNeedleRect(tNewPoint, tNewRect);
    
    PegRect tOldClip = mClip;
    
    tNewRect |= tOldRect;
    mClip = tNewRect;
    
    // Update the member needle position
    mtNeedlePoint.x = tNewPoint.x;
    mtNeedlePoint.y = tNewPoint.y;
    
    // Tell the drawing functions that we've already 
    // calculated the needle position and have put it
    // in mtNeedlePoint.
    mbValueSet = TRUE;
    
    Invalidate(mClip);
    Draw();
    
    mbValueSet = FALSE;
    
    mClip = tOldClip;
}

/*--------------------------------------------------------------------------*/
void PegScale::CalcNeedleRect(const PegPoint& tPoint, PegRect& tRect)
{
    SIGNED iWidth = mClient.Width();
    SIGNED iHeight = mClient.Height();
    
    if(mwStyle & SS_POLYNEEDLE)
    {
	    if(mwStyle & SS_ORIENTVERT)
        {
            if(mwStyle & SS_FACELEFT)
            {
                tRect.wLeft = mClient.wLeft + (iWidth >> 2);
                tRect.wRight = tRect.wLeft + ((iWidth >> 1) + (iWidth >> 2) -
                               (iWidth >> 3));                
            }
            else
            {
                tRect.wRight = mClient.wRight - (iWidth >> 2);
                tRect.wLeft = tRect.wRight - ((iWidth >> 1) + (iWidth >> 2) -
                              (iWidth >> 3));
            }

            tRect.wTop = tPoint.y - (iHeight >> 4) - 2;
            tRect.wBottom = tPoint.y + (iHeight >> 4) + 2;
        }
        else    // oriented horizontally
        {
            if(mwStyle & SS_FACETOP)
            {
                tRect.wTop = mClient.wTop + (iHeight >> 2);
                tRect.wBottom = tRect.wTop + ((iHeight >> 1) + (iHeight >> 2) -
                                (iHeight >> 3));
            }
            else
            {
                tRect.wBottom = mClient.wBottom - (iHeight >> 2);
                tRect.wTop = tRect.wBottom - ((iHeight >> 1) + (iHeight >> 2) -
                             (iHeight >> 3));
            }
            
            tRect.wLeft = tPoint.x - (iWidth >> 4) - 2;
            tRect.wRight = tPoint.x + (iWidth >> 4) + 2;
        }
    }
    else if(mwStyle & SS_THICKNEEDLE || mwStyle & SS_THINNEEDLE)
    {
        if(mwStyle & SS_ORIENTVERT)
        {
            if(mwStyle & SS_FACELEFT)
            {
                tRect.wLeft = mClient.wLeft + (iWidth >> 2);
                tRect.wRight = tRect.wLeft + ((iWidth >> 1) + (iWidth >> 2) -
                               (iWidth >> 3)); 
            }
            else
            {
                tRect.wRight = mClient.wRight - (iWidth >> 2);
                tRect.wLeft = tRect.wRight - ((iWidth >> 1) + (iWidth >> 2) -
                              (iWidth >> 3));
            }
            
            tRect.wTop = tPoint.y - 3;
            tRect.wBottom = tPoint.y + 3;
        }
        else    // oriented horizontally
        {
            if(mwStyle & SS_FACETOP)
            {
                tRect.wTop = mClient.wTop + (iHeight >> 2);
                tRect.wBottom = tRect.wTop + ((iHeight >> 1) + (iHeight >> 2) -
                                (iHeight >> 3));
            }
            else
            {
                tRect.wBottom = mClient.wBottom - (iHeight >> 2);
                tRect.wTop = tRect.wBottom - ((iHeight >> 1) + (iHeight >> 2) -
                             (iHeight >> 3));
            }
            
            tRect.wLeft = tPoint.x - 3;
            tRect.wRight = tPoint.x + 3;
        }
    }
}

#endif	// PEG_HMI_GADGETS


