/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pchart.hpp - Chart base class implementation.
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#ifndef USE_PEG_LTOA
#include "stdlib.h"
//#else
//#include "Pegtypes.hpp"
#endif

//#include "Pegtypes.hpp"

#if defined(PEG_CHARTING)

extern PEGCHAR lsTEST[];

/*--------------------------------------------------------------------------*/
PegChart::PegChart(const PegRect& rect, LONG lMinX, LONG lMaxX, LONG lMinY, 
                   LONG lMaxY, WORD wXScale, WORD wYScale) : 
    PegThing(rect),
    mlMinX(lMinX),
    mlMaxX(lMaxX),
    mlMinY(lMinY),
    mlMaxY(lMaxY),
    mwMajorXScale(wXScale),
	mwMinorXScale(0),
    mwMajorYScale(wYScale),
	mwMinorYScale(0),
    mwMajorTicSize(12),
	mwMinorTicSize(8),
    mwXLabelScale(wXScale),
    mwYLabelScale(wYScale),
	mwXLabelHeight(0),
	mwYLabelWidth(0),
	mpFont(&MenuFont)
{
    InitClient();
    mChartRegion = mClient;

    muColors[PCI_NORMAL] = BLACK;
   #if(PEG_NUM_COLORS >= 4)
    muColors[PCI_NTEXT] = DARKGRAY;
   #else
    muColors[PCI_NTEXT] = WHITE;
   #endif
    muColors[PCI_STEXT] = WHITE;

	AddStatus(PSF_VIEWPORT);
}

/*--------------------------------------------------------------------------*/
void PegChart::Draw()
{
    BeginDraw();

    StandardBorder(muColors[PCI_NORMAL]);

    if(mwExStyle & CS_DRAWXGRID && (mwMajorXScale > 0 || mwMinorXScale > 0))
    {
        DrawXGrid();
    }

    if(mwExStyle & CS_DRAWYGRID && (mwMajorYScale > 0 || mwMinorYScale > 0))
    {
        DrawYGrid();
    }

    if(mwExStyle & CS_DRAWXTICS && (mwMajorXScale > 0 || mwMinorXScale > 0))
    {
        DrawXTics();
    }

    if(mwExStyle & CS_DRAWYTICS && (mwMajorYScale > 0 || mwMinorYScale > 0))
    {
        DrawYTics();
    }

    if(mwExStyle & CS_DRAWXLABELS && mwXLabelScale > 0)
    {
        DrawXLabels();
    }

    if(mwExStyle & CS_DRAWYLABELS && mwYLabelScale > 0)
    {
        DrawYLabels();
    }

    DrawChildren();
	
    EndDraw();
}

/*--------------------------------------------------------------------------*/
SIGNED PegChart::Message(const PegMessage& Mesg)
{
    SIGNED Result = 0;
    
    switch(Mesg.wType)
    {
    case PM_PARENTSIZED:
    case PM_SHOW:
        {
            PegThing::Message(Mesg);
            if(GetExStyle() & CS_AUTOSIZE)
            {
                RecalcSize(Parent()->mClient, FALSE);
            }

        } break;

    case PM_SIZE:
    case PM_MOVE:
        {
            PegThing::Message(Mesg);
            RecalcLayout(TRUE);
        } break;
    
    default:
        return(PegThing::Message(Mesg));
    }

    return Result;
}

/*--------------------------------------------------------------------------*/
void PegChart::ParentShift(SIGNED xOffset, SIGNED yOffset)
{
    mChartRegion.Shift(xOffset, yOffset);

    PegThing::ParentShift(xOffset, yOffset);
}

/*--------------------------------------------------------------------------*/
void PegChart::RecalcLayout(BOOL bRedraw /*=TRUE*/)
{
    mChartRegion = mClient;
    
    if(mwExStyle & CS_DRAWXTICS)
    {
        mChartRegion.wBottom -= mwMajorTicSize;
    }

    if(mwExStyle & CS_DRAWYTICS)
    {
        mChartRegion.wLeft += mwMajorTicSize;

		if(mwExStyle & CS_DUALYTICS)
		{
			mChartRegion.wRight -= mwMajorTicSize;
		}
    }

    if (mwExStyle & CS_DRAWYLABELS)
    {
		SIGNED iSkipHeight = TextHeight(lsTEST, mpFont);
		iSkipHeight++;
		iSkipHeight >>= 1;
		mChartRegion.wTop += iSkipHeight;

		if(!(mwExStyle & CS_DRAWXLABELS))
		{
			mChartRegion.wBottom -= iSkipHeight;
		}
        
        if(mwYLabelWidth == 0)
		{
			PEGCHAR cBuffer[20];
			_ltoa(mlMinY, cBuffer, 10);
			SIGNED iMinWidth = TextWidth(cBuffer, mpFont);
			//_ltoa(mlMinY,cBuffer,10);
			_ltoa(mlMaxY, cBuffer, 10);
			SIGNED iMaxWidth = TextWidth(cBuffer, mpFont);
			/*_ltoa(mlMinY,cBuffer,10);*/
			/*_ltoa(mlMaxY,cBuffer,10);*/
			mChartRegion.wLeft += 
                iMinWidth > iMaxWidth ? iMinWidth : iMaxWidth;
            mChartRegion.wLeft += 2;

            if(mwExStyle & CS_DUALYLABELS)
            {
                mChartRegion.wRight -= 
                    iMinWidth > iMaxWidth ? iMinWidth : iMaxWidth;
                mChartRegion.wRight -= 2;
            }
		}
		else
		{
			mChartRegion.wLeft += mwYLabelWidth;
		}
    }

    if(mwExStyle & CS_DRAWXLABELS)
    {
		PEGCHAR cBuffer[20];
       _ltoa(mlMaxX, cBuffer, 10);
		SIGNED iSkipWidth = TextWidth(cBuffer, mpFont);
/*	_ltoa(mlMaxX,cBuffer,10);*/
        iSkipWidth++;
        iSkipWidth >>= 1;

        mChartRegion.wRight -= iSkipWidth;

        if(!(mwExStyle & CS_DRAWYLABELS))
        {
            _ltoa(mlMinX, cBuffer, 10);
            iSkipWidth = TextWidth(cBuffer, mpFont);
            iSkipWidth++;
            iSkipWidth >>= 1;
            mChartRegion.wLeft += iSkipWidth;
        }

        if(!(mwExStyle & CS_XAXISONZEROY))
        {
            if(mwXLabelHeight == 0)
            {
                SIGNED iLabelHeight = TextHeight(cBuffer, mpFont);
                mChartRegion.wBottom -= iLabelHeight;
            }
            else
            {
                mChartRegion.wBottom -= mwXLabelHeight;
            }
        }
    }

    if(bRedraw)
    {
        Invalidate();
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
void PegChart::RecalcSize(const PegRect& NewSize, BOOL bRedraw /*=TRUE*/)
{
	 PegThing::Resize(NewSize);

	 RecalcLayout(bRedraw);
}

/*--------------------------------------------------------------------------*/
void PegChart::MapDataToPoint(PegChartPoint* pPoint)
{
    LONG lVal = pPoint->mlDataX - mlMinX;
    lVal <<= 8;
    if (mlMaxX != mlMinX)
    {
        lVal /= (mlMaxX - mlMinX);
    }
    lVal *= (mChartRegion.wRight - mChartRegion.wLeft - 1);
    lVal >>= 8;
    pPoint->mScreenPoint.x = (SIGNED)(lVal + mChartRegion.wLeft);
    
    lVal = pPoint->mlDataY - mlMinY;
    lVal <<= 8;

    if (mlMaxY > mlMinY)
    {
        lVal /= (mlMaxY - mlMinY);
    }
    lVal *= (mChartRegion.wBottom - mChartRegion.wTop - 1);
    lVal >>= 8;
    pPoint->mScreenPoint.y = (SIGNED)(mChartRegion.wBottom - lVal);
}

/*--------------------------------------------------------------------------*/
void PegChart::MapPointToData(PegChartPoint* pPoint)
{
    LONG lPoint = pPoint->mScreenPoint.x - mChartRegion.wLeft;
    lPoint <<= 8;
    if (mChartRegion.wLeft < mChartRegion.wRight)
    {
        lPoint /= mChartRegion.Width();
    }
    lPoint *= (mlMaxX - mlMinX);
    lPoint >>= 8;
    pPoint->mlDataX = lPoint;

    lPoint = mChartRegion.wBottom - pPoint->mScreenPoint.y;
    lPoint <<= 8;

    if (mChartRegion.wTop < mChartRegion.wBottom)
    {
        lPoint /= mChartRegion.Height();
    }
    lPoint *= (mlMaxY - mlMinY);
    lPoint >>= 8;
    pPoint->mlDataY = lPoint;
}

/*--------------------------------------------------------------------------*/
void PegChart::DrawXTics()
{
    PegColor tColor(muColors[PCI_STEXT], 0, 0);

	PegChartPoint tPoint(mlMinX, 0);

	SIGNED iTicTop;
	if(mwExStyle & CS_XAXISONZEROY)
	{
		MapDataToPoint(&tPoint);
		iTicTop = tPoint.mScreenPoint.y;
	}
	else
	{
		iTicTop = (SIGNED)mChartRegion.wBottom;
	}

	if(mwMinorXScale > 0)
	{
		for(LONG li = mlMinX; li <= mlMaxX; li += (LONG)mwMinorXScale)
		{
			tPoint.mlDataX = li;
			MapDataToPoint(&tPoint);
			Line(tPoint.mScreenPoint.x, iTicTop,
				 tPoint.mScreenPoint.x, iTicTop + mwMinorTicSize,
				 tColor, 1);
		}
	}

	if(mwMajorXScale > 0)
	{
		for(LONG li = mlMinX; li <= mlMaxX; li += (LONG)mwMajorXScale)
		{
			tPoint.mlDataX = li;
			MapDataToPoint(&tPoint);
			Line(tPoint.mScreenPoint.x, iTicTop,
				 tPoint.mScreenPoint.x, iTicTop + mwMajorTicSize,
				 tColor, 1);
		}
	}

	Line(mChartRegion.wLeft, iTicTop, tPoint.mScreenPoint.x, iTicTop,
		 tColor, 1);
}

/*--------------------------------------------------------------------------*/
void PegChart::DrawYTics()
{
	PegColor tColor(muColors[PCI_STEXT], 0, 0);

	PegChartPoint tPoint(mlMinX, mlMinY);

	SIGNED iTicRight = (SIGNED)mChartRegion.wLeft;
	SIGNED iTicLeft = (SIGNED)mChartRegion.wRight;

	if(mwMinorYScale > 0)
	{
		for(LONG li = mlMinY; li <= mlMaxY; li += mwMinorYScale)
		{
			tPoint.mlDataY = li;
			MapDataToPoint(&tPoint);
			Line(iTicRight, tPoint.mScreenPoint.y, 
				 iTicRight - mwMinorTicSize, tPoint.mScreenPoint.y,
				 tColor, 1);

			if(mwExStyle & CS_DUALYTICS)
			{
				Line(iTicLeft, tPoint.mScreenPoint.y, 
					 iTicLeft + mwMajorTicSize, tPoint.mScreenPoint.y,
					 tColor, 1);
			}
		}
	}

	if(mwMajorYScale > 0)
	{
		for(LONG li = mlMinY; li <= mlMaxY; li += mwMajorYScale)
		{
			tPoint.mlDataY = li;
			MapDataToPoint(&tPoint);
			Line(iTicRight, tPoint.mScreenPoint.y, 
				 iTicRight - mwMajorTicSize, tPoint.mScreenPoint.y,
				 tColor, 1);
			
			if(mwExStyle & CS_DUALYTICS)
			{
				Line(iTicLeft, tPoint.mScreenPoint.y,
					 iTicLeft + mwMajorTicSize, tPoint.mScreenPoint.y,
					 tColor, 1);
			}
		}
	}
	
	Line(mChartRegion.wLeft, tPoint.mScreenPoint.y, mChartRegion.wLeft,
		 mChartRegion.wBottom, tColor, 1);

	if(mwExStyle & CS_DUALYTICS)
	{
		Line(mChartRegion.wRight, tPoint.mScreenPoint.y, mChartRegion.wRight,
			 mChartRegion.wBottom, tColor, 1);
	}
}

/*--------------------------------------------------------------------------*/
void PegChart::DrawXGrid()
{
    PegChartPoint tPoint(mlMinX, mlMinY);
    PegColor tColor(muColors[PCI_NTEXT], 0, 0);

    PegChartPoint tTop(mlMinX, mlMaxY);
    MapDataToPoint(&tTop);

    PegRect tTest, tInvalid = Screen()->GetInvalidRect();

    LONG li;
    WORD wCurScale = mwMinorXScale;
    for(WORD wI = 0; wI < 2; wI++)
    {
        if(wI == 1)
        {
            wCurScale = mwMajorXScale;
        }

        if(wCurScale)
        {
            li = mlMinX;
            if(mwMajorYScale > 0 || mwMinorYScale > 0)
            {
                li += (LONG)wCurScale;
            }

            for(; li <= mlMaxX; li += (LONG)wCurScale)
            {
                tPoint.mlDataX = li;
                MapDataToPoint(&tPoint);
                tTest.Set(tPoint.mScreenPoint.x, tTop.mScreenPoint.y,
                          tPoint.mScreenPoint.x, mChartRegion.wBottom);
                if(tTest.Overlap(tInvalid))
                {
                    Line(tPoint.mScreenPoint.x, tTop.mScreenPoint.y, 
                         tPoint.mScreenPoint.x, mChartRegion.wBottom,
                         tColor, 1);
                }
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegChart::DrawYGrid()
{
    PegChartPoint tPoint(mlMinX, mlMinY);
    PegColor tColor(muColors[PCI_NTEXT], 0, 0);

    PegChartPoint tRight(mlMaxX, mlMinY);
    MapDataToPoint(&tRight);

    PegRect tTest, tInvalid = Screen()->GetInvalidRect();

    LONG li;
    WORD wCurScale = mwMinorYScale;
    for(WORD wI = 0; wI < 2; wI++)
    {
        if(wI == 1)
        {
            wCurScale = mwMajorYScale;
        }

        if(wCurScale)
        {
            li = mlMinY;
            if(mwMajorXScale > 0 || mwMinorXScale > 0 &&
                    (!(mwExStyle & CS_XAXISONZEROY)))
            {
                li += (LONG)wCurScale;
            }
            for(; li <= mlMaxY; li += wCurScale)
            {
                tPoint.mlDataY = li;

                MapDataToPoint(&tPoint);
                tTest.Set(mChartRegion.wLeft, tPoint.mScreenPoint.y,
                          tRight.mScreenPoint.x, tPoint.mScreenPoint.y);
                if(tTest.Overlap(tInvalid))
                {
                    Line(mChartRegion.wLeft, tPoint.mScreenPoint.y,
                         tRight.mScreenPoint.x, tPoint.mScreenPoint.y,
                         tColor, 1);
                }
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegChart::DrawXLabels()
{
    PegColor Color(muColors[PCI_STEXT], muColors[PCI_NORMAL]);
    //PegColor FillColor(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);

    PegPoint tLeftTop;
    PegRect tTest, tInvalid = Screen()->GetInvalidRect();
    SIGNED nHeight = TextHeight(lsTEST, mpFont);
    PegRect tFillRect;

   	if(mwExStyle & CS_XAXISONZEROY)
	{
		PegChartPoint tPoint(mlMinX, 0);
		MapDataToPoint(&tPoint);
		tLeftTop.y = tPoint.mScreenPoint.y + mwMajorTicSize + 2;
	}
	else
	{
		tLeftTop.y = mChartRegion.wBottom + mwMajorTicSize + 2;
    }

    tFillRect.Set(mChartRegion.wLeft + 1, tLeftTop.y,
                  mReal.wRight, tLeftTop.y + nHeight);

    // If we're not overlapping any of the invalid region,
    // just return
    if(!tFillRect.Overlap(tInvalid))
    {
        return;
    }
    
    //Rectangle(tFillRect, FillColor, 0);

    SIGNED iNumLabels = 0;
    SIGNED iLabelStep = 0;
    PEGCHAR szBuff[20];
    szBuff[0] = '\0';

    if (mwXLabelScale)
    {
	    iNumLabels = (SIGNED) (((mlMaxX - mlMinX) / mwXLabelScale) + 1);
    }
	
    if (iNumLabels > 1)
    {
        iLabelStep = (mChartRegion.Width() << 3) / (iNumLabels - 1);
    }
    SIGNED iXPos = (mChartRegion.wLeft << 3);
    SIGNED i = 0;

	if(mwExStyle & CS_XAXISONZEROY && mwExStyle & CS_DRAWYTICS)
	{
		// Skip the first label so it doesn't collide with the Y axis
		iXPos += iLabelStep;
		i++;
		iNumLabels--;
	}

    LONG lData;
    SIGNED nWidth; 

	while(iNumLabels--)
    {
        lData = (LONG)(mlMinX + (mwXLabelScale * i));
        _ltoa(lData, szBuff, 10);
        nWidth = TextWidth(szBuff, mpFont);

        if(nWidth & 0x01)
        {
            nWidth++;
        }

        tLeftTop.x = (iXPos >> 3) - (nWidth / 2);
        //tLeftTop.y = mReal.wBottom - nHeight;
        
        if(tLeftTop.x < mReal.wLeft)
        {
            tLeftTop.x = mReal.wLeft + 1;
        }
        if((tLeftTop.x + nWidth) > mReal.wRight)
        {
            tLeftTop.x = mReal.wRight - nWidth - 1;
        }

        tTest.Set(tLeftTop.x, tLeftTop.y, tLeftTop.x + nWidth,
                  tLeftTop.y + nHeight);
        if(tTest.Overlap(tInvalid))
        {
            DrawText(tLeftTop, szBuff, Color, mpFont);
        }
        
        iXPos += iLabelStep;
        i++;
    }        
}

/*--------------------------------------------------------------------------*/
void PegChart::DrawYLabels()
{
    PegColor Color(muColors[PCI_STEXT], muColors[PCI_NORMAL]);
    PegColor FillColor(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);

    PegRect tTest, tInvalid = Screen()->GetInvalidRect();

    // Draw 1 rect to erase all the stuff at once
    PegRect tFillRect, tRightFillRect;
    tFillRect.Set(mReal.wLeft, mReal.wTop, 
                  mChartRegion.wLeft - mwMajorTicSize - 1,
                  mChartRegion.wBottom);

    // If we're not overlapping any of the invalid region,
    // just return
    if(!(mwExStyle & CS_DUALYLABELS))
    {
        if(!tFillRect.Overlap(tInvalid))
        {
            return;
        }
        else
        {
            Rectangle(tFillRect, FillColor, 0);
        }
    }
    else
    {
        tRightFillRect.Set(mChartRegion.wRight + mwMajorTicSize + 1,
                mReal.wTop, mReal.wRight, mReal.wBottom);
        if(tRightFillRect.Overlap(tInvalid) ||
                tFillRect.Overlap(tInvalid))
        {
            Rectangle(tFillRect, FillColor, 0);
            Rectangle(tRightFillRect, FillColor, 0);
        }
        else
        {
            return;
        }
    }

    PegPoint tLeftTop, tRightTop;
    PEGCHAR szBuff[20];
    szBuff[0] = '\0';
    SIGNED iNumTicks = 0;
    SIGNED iTickStep = 0;

    if (mwYLabelScale)
    { 
        iNumTicks = (SIGNED) (((mlMaxY - mlMinY) / mwYLabelScale) + 1);
    }

    if (iNumTicks > 1)
    {
        iTickStep = (mChartRegion.Height() << 3) / (iNumTicks - 1);
    }
    SIGNED iYPos = (mChartRegion.wBottom << 3);
    
    LONG lData;
    SIGNED i = 0;
    SIGNED nWidth, nHeight = TextHeight(lsTEST, mpFont);

    while(iNumTicks--)
    {
        lData = (LONG)(mlMinY + (mwYLabelScale * i));
        /*_ltoa(lData, szBuff, 10);*/
        nWidth = TextWidth(szBuff, mpFont);
	_ltoa(lData,szBuff,10);
        tLeftTop.y = (iYPos >> 3) - (nHeight / 2);
        
        if(tLeftTop.y < mReal.wTop)
        {
            tLeftTop.y = mReal.wTop + 1;
        }
        if((tLeftTop.y + nHeight) > mReal.wBottom)
        {
            tLeftTop.y = mReal.wBottom - nHeight - 1;
        }
        
		tLeftTop.x = mChartRegion.wLeft - mwMajorTicSize - nWidth - 2;

        tTest.Set(tLeftTop.x, tLeftTop.y, tLeftTop.x + nWidth,
                  tLeftTop.y + nHeight);
        if(tTest.Overlap(tInvalid))
        {
            DrawText(tLeftTop, szBuff, Color, mpFont);
        }

        if(mwExStyle & CS_DUALYLABELS)
        {
            tRightTop.x = mChartRegion.wRight + mwMajorTicSize + 2;
            tRightTop.y = tLeftTop.y;
            tTest.Set(tRightTop.x, tRightTop.y, tRightTop.x + nWidth,
                      tRightTop.y + nHeight);
            if(tTest.Overlap(tInvalid))
            {
                DrawText(tRightTop, szBuff, Color, mpFont);
            }
        }

        iYPos -= iTickStep;
        i++;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegChartLine::PegChartLine(PegChart* pParent, UCHAR ucID, COLORVAL tColor)
{
    mpParent = pParent;
    mucID = ucID;
    mtColor.uForeground = tColor;
	mtBkgColor.uForeground = tColor;
	mtBkgColor.uBackground = tColor;
	mtBkgColor.uFlags = CF_FILL;
    mpNext = NULL;
    mpPoints = NULL;
}

/*--------------------------------------------------------------------------*/
PegChartLine::~PegChartLine()
{
    ResetLine();
}

/*--------------------------------------------------------------------------*/
void PegChartLine::Draw()
{
    PegChartPoint* pPoint = mpPoints;
    PegChartPoint* pNext = NULL;
	BOOL bLineFill = mpParent->GetExStyle() & CS_DRAWLINEFILL;
	PegChartPoint tZeroPointLeft(mpParent->GetMinX(), 0);
	PegChartPoint tZeroPointRight(mpParent->GetMinX(), 0);
	PegPoint tFillPoints[5];

    if(pPoint)
    {
        pNext = pPoint->mpNext;
    }

    while(pNext)
    {
        mpParent->Line(pPoint->mScreenPoint.x, pPoint->mScreenPoint.y,
                       pNext->mScreenPoint.x, pNext->mScreenPoint.y, mtColor);

		if(bLineFill)
		{
			MapDataToPoint(&tZeroPointLeft);
			MapDataToPoint(&tZeroPointRight);

			tFillPoints[0].x = pPoint->mScreenPoint.x;
			tFillPoints[0].y = pPoint->mScreenPoint.y;

			tFillPoints[1].x = pNext->mScreenPoint.x;
			tFillPoints[1].y = pNext->mScreenPoint.y;

			tFillPoints[2].x = tZeroPointRight.mScreenPoint.x;
			tFillPoints[2].y = tZeroPointRight.mScreenPoint.y;

			tFillPoints[3].x = tZeroPointLeft.mScreenPoint.x;
			tFillPoints[3].y = tZeroPointLeft.mScreenPoint.y;

			tFillPoints[4].x = pPoint->mScreenPoint.x;
			tFillPoints[4].y = pPoint->mScreenPoint.y;

			mpParent->Screen()->Polygon(mpParent, tFillPoints,
										5, mtBkgColor);
		}
		
        pPoint = pNext;
        pNext = pNext->mpNext;
    }
}

/*--------------------------------------------------------------------------*/
PegChartPoint* PegChartLine::AddPoint(LONG lX, LONG lY)
{
    PegChartPoint* pAdd = mpPoints;
    PegChartPoint* pPrevious = pAdd;
    
    while(pAdd)
    {
        pPrevious = pAdd;
        pAdd = pAdd->mpNext;
    }

    pAdd = new PegChartPoint(lX, lY);

    if(mpPoints == NULL)
    {
        mpPoints = pAdd;
    }
    else
    {
        pPrevious->mpNext = pAdd;
    }
    
    MapDataToPoint(pAdd);
    return pAdd;
}

/*--------------------------------------------------------------------------*/
PegChartPoint* PegChartLine::InsertPoint(PegChartPoint* pPreviousPoint,
                                         LONG lX, LONG lY)
{
    if(mpPoints == NULL)
    {
        mpPoints = new PegChartPoint(lX, lY);
        return mpPoints;
    }

    PegChartPoint* pPoint = mpPoints;
    PegChartPoint* pPrevious = pPoint;

    while(pPoint && pPrevious != pPreviousPoint)
    {
        pPrevious = pPoint;
        pPoint = pPoint->mpNext;
    }

    PegChartPoint* pAdd = new PegChartPoint(lX, lY);

    if(pPrevious)
    {
        pPrevious->mpNext = pAdd;
    }

    pAdd->mpNext = pPoint;
    
    MapDataToPoint(pAdd);
    return pAdd;
}

/*--------------------------------------------------------------------------*/
PegChartPoint* PegChartLine::RemovePoint(PegChartPoint* pPoint)
{
    if(mpPoints == NULL)
    {
        return mpPoints;
    }

    PegChartPoint* pDelete = mpPoints;
    PegChartPoint* pPrevious = pDelete;
    while(pDelete && pDelete != pPoint)
    {
        pPrevious = pDelete;
        pDelete = pDelete->mpNext;
    }

    if(pPrevious)
    {
        if(pDelete)
        {
            pPrevious->mpNext = pDelete->mpNext;
        }
    }

    if(pDelete)
    {
        delete pDelete;
    }

    return pPrevious;
}

/*--------------------------------------------------------------------------*/
void PegChartLine::RecalcLine()
{
    PegChartPoint* pPoint = mpPoints;
    while(pPoint)
    {
        MapDataToPoint(pPoint);
        pPoint = pPoint->mpNext;
    }
}

/*--------------------------------------------------------------------------*/
void PegChartLine::ResetLine()
{
    PegChartPoint* pDelete = mpPoints;
    PegChartPoint* pNext;
    
    while(pDelete)
    {
        pNext = pDelete->mpNext;
        delete pDelete;
        pDelete = pNext;
    }

    mpPoints = NULL;
}

#endif // PEG_CHARTING


