/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pstchart.cpp - Implementation for the PegStripChart object
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-2001 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "string.h"
#include "peg.hpp"

#if defined(PEG_CHARTING)

/*--------------------------------------------------------------------------*/
PegStripChart::PegStripChart(const PegRect& Rect, WORD wSamples, LONG lMinY, 
                             LONG lMaxY, WORD wXScale /*=0*/,
                             WORD wYScale /*=0*/) : 
    PegChart(Rect, 0L, (LONG)wSamples, lMinY, lMaxY, wXScale, wYScale)
{
    mpLines = NULL;
    mcLineID = 0;

    Type(TYPE_STRIP_CHART);
    
    muColors[PCI_NORMAL] = PCLR_SHADOW;

    mwExStyle = CS_PAGED | CS_DRAWXAXIS | CS_DRAWAGED | CS_DRAWYTICS | 
                CS_DRAWYLABELS | CS_DRAWLEADER;

    RecalcLayout(FALSE);
}

/*--------------------------------------------------------------------------*/
PegStripChart::~PegStripChart()
{
    if(mpLines)
    {
        PegStripChartLine* temp, *temp2;
        temp = mpLines;
        while(temp)
        {
            temp2 = temp->GetNext();
            delete temp;
            temp = temp2;
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegStripChart::Draw(void)
{
	if(!(mwStatus & PSF_VISIBLE))
	{
		return;
	}

    PegStripChartLine* pLine = mpLines;
    
    BeginDraw();
    
	StandardBorder(muColors[PCI_NORMAL]);

    // Put the grids underneath
    if(GetExStyle() & CS_DRAWXGRID)
    {
        DrawXGrid();
    }

    if(GetExStyle() & CS_DRAWYGRID)
    {
        DrawYGrid();
    }

    while(pLine)
    {
        DrawLine(pLine);

        if(GetExStyle() & CS_PAGED && pLine->ScrollFilled())
        {
            DrawLineHistory(pLine);
        }
        
        pLine = pLine->GetNext();
    }

    // The tics and labels on top
    if(GetExStyle() & CS_DRAWXTICS)
    {
        DrawXTics();
    }

    if(GetExStyle() & CS_DRAWYTICS)
    {
        DrawYTics();
    }

    if(GetExStyle() & CS_DRAWXLABELS)
    {
        DrawXLabels();
    }

    if(GetExStyle() & CS_DRAWYLABELS)
    {
        DrawYLabels();
    }

    DrawChildren();

    EndDraw();
}

/*--------------------------------------------------------------------------*/
SIGNED PegStripChart::Message(const PegMessage& Mesg)
{
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
            RecalcLayout(TRUE);
        }

    default:
        return(PegChart::Message(Mesg));
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
void PegStripChart::Resize(PegRect NewSize)
{
    PegThing::Resize(NewSize);
    RecalcLayout(FALSE);
}

/*--------------------------------------------------------------------------*/
void PegStripChart::RecalcSize(const PegRect& NewSize, BOOL bRedraw /*=TRUE*/)
{
    Resize(NewSize);
    if(bRedraw)
    {
        Invalidate(mReal);
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
UCHAR PegStripChart::AddLine(COLORVAL tColor, COLORVAL tAgedColor,
                             COLORVAL tFillColor /*=0*/)
{
    if(mcLineID >= 255)
    {
        return((UCHAR) -1);
    }

    if(mpLines == NULL)
    {
        mpLines = new PegStripChartLine(this, mcLineID, tColor, tAgedColor, 
                                        tFillColor);
    }
    else
    {
        PegStripChartLine* temp2 = mpLines, *temp = mpLines;
        
        while(temp)
        {
            temp2 = temp;
            temp = temp->GetNext();
        }

        // temp2 should be pointing at the tail
        PegStripChartLine* newLine = new PegStripChartLine(this, mcLineID, 
                                        tColor, tAgedColor, tFillColor);
        temp2->SetNext(newLine);
    }

    return mcLineID++;
}

/*--------------------------------------------------------------------------*/
BOOL PegStripChart::RemoveLine(UCHAR id)
{
    if(mpLines->GetID() == id)
    {
        if(mpLines->GetNext() == NULL)
        {
            delete mpLines;
            mpLines = NULL;
        }
        else
        {
            PegStripChartLine* pTemp = mpLines;
            mpLines = mpLines->GetNext();
            delete pTemp;
        }

        return TRUE;
    }
    else
    {
        PegStripChartLine* pTemp2, *pTemp = mpLines;
        while(pTemp)
        {
            pTemp2 = pTemp;
            pTemp = pTemp->GetNext();
            
            // Handle hitting the end of the list and
            // not finding the ID
            if(pTemp == NULL) continue;

            if(pTemp->GetID() == id)
            {
                pTemp2->SetNext(pTemp->GetNext());
                delete pTemp;

                return TRUE;
            }
        }
    }

    // Didn't find it.
    return FALSE;
}

/*--------------------------------------------------------------------------*/
BOOL PegStripChart::AddData(UCHAR ucID, LONG lRawData, BOOL bRedraw /*=TRUE*/)
{
    PegStripChartLine* pLine = mpLines;
    while(pLine)
    {
        if(pLine->GetID() == ucID)
        {
            if(pLine->AddData(lRawData))
            {
				if(bRedraw)
				{
					DrawNewLineData(pLine);
				}

                return(TRUE);
            }
            else
            {
                break;
            }
        }
        pLine = pLine->GetNext();
    }
        
    return(FALSE);
}

/*--------------------------------------------------------------------------*/
SIGNED PegStripChart::ValToPointY(LONG lVal)
{
	 PegChartPoint tPoint(mlMinX, lVal);
	 MapDataToPoint(&tPoint);
	 return(tPoint.mScreenPoint.y);
}

/*--------------------------------------------------------------------------*/
SIGNED PegStripChart::IndexToPointX(WORD wIndex)
{
	 PegChartPoint tPoint((LONG)wIndex, 0);
	 MapDataToPoint(&tPoint);
	 return(tPoint.mScreenPoint.x);
}

/*--------------------------------------------------------------------------*/
COLORVAL PegStripChart::GetLineColor(UCHAR ucID)
{
	PegStripChartLine* pLine = GetLineByID(ucID);

	if(pLine)
	{
		return(pLine->GetColor());
	}
	else
	{
		return((COLORVAL)-1);
	}
}

/*--------------------------------------------------------------------------*/
COLORVAL PegStripChart::GetLineAgedColor(UCHAR ucID)
{
	PegStripChartLine* pLine = GetLineByID(ucID);

	if(pLine)
	{
		return(pLine->GetAgedColor());
	}
	else
	{
		return((COLORVAL)-1);
	}
}

/*--------------------------------------------------------------------------*/
COLORVAL PegStripChart::GetLineFillColor(UCHAR ucID)
{
	PegStripChartLine* pLine = GetLineByID(ucID);

	if(pLine)
	{
		return(pLine->GetFillColor());
	}
	else
	{
		return((COLORVAL)-1);
	}
}

/*--------------------------------------------------------------------------*/
COLORVAL PegStripChart::GetLineLeaderColor(UCHAR ucID)
{
    PegStripChartLine* pLine = GetLineByID(ucID);

    if(pLine)
    {
        return(pLine->GetLeaderColor());
    }
    else
    {
        return((COLORVAL)-1);
    }
}

/*--------------------------------------------------------------------------*/
void PegStripChart::SetLineColor(UCHAR ucID, COLORVAL tColor)
{
	PegStripChartLine* pLine = GetLineByID(ucID);

	if(pLine)
	{
		pLine->SetColor(tColor);
	}
}

/*--------------------------------------------------------------------------*/
void PegStripChart::SetLineAgedColor(UCHAR ucID, COLORVAL tColor)
{
	PegStripChartLine* pLine = GetLineByID(ucID);

	if(pLine)
	{
		pLine->SetAgedColor(tColor);
	}
}

/*--------------------------------------------------------------------------*/
void PegStripChart::SetLineFillColor(UCHAR ucID, COLORVAL tColor)
{
	PegStripChartLine* pLine = GetLineByID(ucID);

	if(pLine)
	{
		pLine->SetFillColor(tColor);
	}
}

/*--------------------------------------------------------------------------*/
void PegStripChart::SetLineLeaderColor(UCHAR ucID, COLORVAL tColor)
{
	PegStripChartLine* pLine = GetLineByID(ucID);

	if(pLine)
	{
		pLine->SetLeaderColor(tColor);
	}
}

/*--------------------------------------------------------------------------*/
void PegStripChart::DrawLine(PegStripChartLine* pLine)
{
	PegPoint tFillPoints[5];
	PegColor tLineColor(pLine->GetColor());
    PegColor tFillColor(pLine->GetFillColor(), pLine->GetFillColor(), CF_FILL);

    PegRect tInvalid = Screen()->GetInvalidRect();
    PegPoint tCur, tNext;

    tNext.x = IndexToPointX(0);
    tNext.y = ValToPointY(pLine->GetData(0));

    SIGNED iZeroY = ValToPointY(0);
    
    if(mwExStyle & CS_PAGED)
    {        
        for(WORD i = 0; i < pLine->GetHead(); i++)
        {
            tCur.x = tNext.x;
            tCur.y = tNext.y;
            tNext.x = IndexToPointX(i + 1);
            tNext.y = ValToPointY(pLine->GetData(i + 1));

            if(tInvalid.Contains(tCur) || tInvalid.Contains(tNext))
            {
                if(mwExStyle & CS_DRAWLINEFILL)
                {
                    tFillPoints[0].x = tCur.x;
                    tFillPoints[0].y = tCur.y;

                    tFillPoints[1].x = tNext.x;
                    tFillPoints[1].y = tNext.y;

                    tFillPoints[2].x = tNext.x;
                    tFillPoints[2].y = iZeroY;

                    tFillPoints[3].x = tCur.x;
                    tFillPoints[3].y = iZeroY;

                    tFillPoints[4].x = tCur.x;
                    tFillPoints[4].y = tCur.y;

                    Screen()->Polygon(this, tFillPoints, 5, tFillColor, 0);
                }

                Line(tCur.x, tCur.y, tNext.x, tNext.y, tLineColor, 1);
            }
        }
    
        if(mwExStyle & CS_DRAWLEADER)
        {
            Line(tNext.x, mChartRegion.wTop + 1, tNext.x, 
                 mChartRegion.wBottom - 1, pLine->GetLeaderColor(), 1);
        }
    }
    else if(mwExStyle & CS_SCROLLED)
    {
        if(!pLine->ScrollFilled())
        {
            for(WORD i = 0; i < pLine->GetHead(); i++)
            {
                tCur.x = tNext.x;
                tCur.y = tNext.y;
                tNext.x = IndexToPointX(i + 1);
                tNext.y = ValToPointY(pLine->GetData(i + 1));

                if(tInvalid.Contains(tCur) || tInvalid.Contains(tNext))
                {
                    if(mwExStyle & CS_DRAWLINEFILL)
                    {
                        tFillPoints[0].x = tCur.x;
                        tFillPoints[0].y = tCur.y;

                        tFillPoints[1].x = tNext.x;
                        tFillPoints[1].y = tNext.y;

                        tFillPoints[2].x = tNext.x;
                        tFillPoints[2].y = iZeroY;

                        tFillPoints[3].x = tCur.x;
                        tFillPoints[3].y = iZeroY;

                        tFillPoints[4].x = tCur.x;
                        tFillPoints[4].y = tCur.y;

                        Screen()->Polygon(this, tFillPoints, 5, tFillColor, 0);
                    }

                    Line(tCur.x, tCur.y, tNext.x, tNext.y, tLineColor, 1);
                }
            }
        }
        else
        {
            SIGNED nIndex = 0;
            SIGNED i;
            
            for(i = pLine->GetTail(); i < mlMaxX - 1; i++, nIndex++)
            {
                if(mwExStyle & CS_DRAWLINEFILL)
                {
                    tFillPoints[0].x = IndexToPointX(nIndex);
                    tFillPoints[0].y = ValToPointY(pLine->GetData(i));

                    tFillPoints[1].x = IndexToPointX(nIndex + 1);
                    tFillPoints[1].y = ValToPointY(pLine->GetData(i + 1));

                    tFillPoints[2].x = tFillPoints[1].x;
                    tFillPoints[2].y = ValToPointY(0);

                    tFillPoints[3].x = tFillPoints[0].x;
                    tFillPoints[3].y = tFillPoints[2].y;

                    tFillPoints[4].x = tFillPoints[0].x;
                    tFillPoints[4].y = tFillPoints[0].y;

                    Screen()->Polygon(this, tFillPoints, 5, tFillColor, 0);
                }

                Line(IndexToPointX(nIndex), ValToPointY(pLine->GetData(i)),
                     IndexToPointX(nIndex + 1), ValToPointY(pLine->GetData(i + 1)),
                     tLineColor, 1);
            }

            if((pLine->GetHead() != mlMaxX - 1) && (pLine->GetHead() != 0))
            {
                if(mwExStyle & CS_DRAWLINEFILL)
                {
                    tFillPoints[0].x = IndexToPointX(nIndex);
                    tFillPoints[0].y = ValToPointY(pLine->GetData(mlMaxX - 1));

                    tFillPoints[1].x = IndexToPointX(nIndex + 1);
                    tFillPoints[1].y = ValToPointY(pLine->GetData(0));

                    tFillPoints[2].x = tFillPoints[1].x;
                    tFillPoints[2].y = ValToPointY(0);

                    tFillPoints[3].x = tFillPoints[0].x;
                    tFillPoints[3].y = tFillPoints[2].y;

                    tFillPoints[4].x = tFillPoints[0].x;
                    tFillPoints[4].y = tFillPoints[0].y;

                    Screen()->Polygon(this, tFillPoints, 5, tFillColor, 0);
                }

                Line(IndexToPointX(nIndex), 
                     ValToPointY(pLine->GetData(mlMaxX - 1)),
                     IndexToPointX(nIndex + 1), 
                     ValToPointY(pLine->GetData(0)),
                     tLineColor, 1);
                
                nIndex++;
            }

            for(i = 0; 
               (i < pLine->GetHead()) && (nIndex < mlMaxX - 1); 
               i++, nIndex++)
            {
                if(mwExStyle & CS_DRAWLINEFILL)
                {
                    tFillPoints[0].x = IndexToPointX(nIndex);
                    tFillPoints[0].y = ValToPointY(pLine->GetData(i));

                    tFillPoints[1].x = IndexToPointX(nIndex + 1);
                    tFillPoints[1].y = ValToPointY(pLine->GetData(i + 1));

                    tFillPoints[2].x = tFillPoints[1].x;
                    tFillPoints[2].y = ValToPointY(0);

                    tFillPoints[3].x = tFillPoints[0].x;
                    tFillPoints[3].y = tFillPoints[2].y;

                    tFillPoints[4].x = tFillPoints[0].x;
                    tFillPoints[4].y = tFillPoints[0].y;

                    Screen()->Polygon(this, tFillPoints, 5, tFillColor, 0);
                }

                Line(IndexToPointX(nIndex), ValToPointY(pLine->GetData(i)),
                     IndexToPointX(nIndex + 1), 
                     ValToPointY(pLine->GetData(i + 1)),
                     tLineColor, 1);
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegStripChart::DrawLineHistory(PegStripChartLine* pLine)
{
    PegColor tAgedColor(pLine->GetAgedColor());
    PegColor tFillColor(pLine->GetFillColor(), pLine->GetFillColor(), CF_FILL);
    PegPoint tFillPoints[5];

    PegRect tInvalid = Screen()->GetInvalidRect();

    PegPoint tCur, tNext;

    tNext.x = IndexToPointX(pLine->GetTail() + 1);
    tNext.y = ValToPointY(pLine->GetData(pLine->GetTail() + 1));

    SIGNED iZeroY = ValToPointY(0);
    
	BeginDraw();
	
    // Draw from the present (mwTail) to the right client edge.
    for(WORD i = pLine->GetTail() + 1; i < mlMaxX - 1; i++)
    {
        tCur.x = tNext.x;
        tCur.y = tNext.y;
        tNext.x = IndexToPointX(i + 1);
        tNext.y = ValToPointY(pLine->GetData(i + 1));

        if(tInvalid.Contains(tCur) || tInvalid.Contains(tNext))
        {
            if(mwExStyle & CS_DRAWLINEFILL)
            {
                tFillPoints[0].x = tCur.x;
                tFillPoints[0].y = tCur.y;

                tFillPoints[1].x = tNext.x;
                tFillPoints[1].y = tNext.y;

                tFillPoints[2].x = tNext.x;
                tFillPoints[2].y = iZeroY;

                tFillPoints[3].x = tCur.x;
                tFillPoints[3].y = iZeroY;

                tFillPoints[4].x = tCur.x;
                tFillPoints[4].y = tCur.y;

                Screen()->Polygon(this, tFillPoints, 5, tFillColor, 0);
            }

            Line(tCur.x, tCur.y, tNext.x, tNext.y, tAgedColor, 1);
        }
    }

	EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegStripChart::DrawNewLineData(PegStripChartLine* pLine)
{
    PegRect DrawRect;
    WORD wIndex;
	if(mwExStyle & CS_PAGED)
	{
		DrawRect = mChartRegion;

		// Narrow down the DrawRect to where the new data was applied
		wIndex = pLine->GetHead();
		if(wIndex != 0)
		{
			DrawRect.wLeft = IndexToPointX(wIndex - 1);
			DrawRect.wRight = IndexToPointX(pLine->GetTail());
			Invalidate(DrawRect);
            BeginDraw();
            DrawInvalidChartRegion();
            EndDraw();
		}
		else
		{
			// Now, invalidate the entire chart region, and
			// only draw the history line.
			Invalidate(mChartRegion);
			DrawLineHistory(pLine);

			// Get rid of the leader, if we have one
			if(mwExStyle & CS_DRAWLEADER)
			{
				DrawRect.wRight = IndexToPointX((WORD)mlMaxX);
				DrawRect.wLeft = IndexToPointX((WORD)(mlMaxX - 2));
				Invalidate(DrawRect);
                BeginDraw();
				DrawInvalidChartRegion();
                EndDraw();
			}
		}	
	}
	else if(mwExStyle & CS_SCROLLED)
	{
        if(!pLine->ScrollFilled())
        {
            DrawRect = mChartRegion;
            wIndex = pLine->GetHead();
            if(wIndex)
            {
                DrawRect.wLeft = IndexToPointX(wIndex - 1);
                DrawRect.wRight = IndexToPointX(wIndex);
                Invalidate(DrawRect);
                Draw();
            }
        }
        else
        {
            // TODO: Make this more efficient. We just can't
            // shift a rectangle, because there may be  grid
            // lines and other lines on the chart that are
            // getting updated at different values. We can't
            // just shift them around, too.
            Invalidate(mChartRegion);
            Draw();
        }
	}
}

/*--------------------------------------------------------------------------*/
void PegStripChart::DrawInvalidChartRegion()
{
    PegRect tInvalid = Screen()->GetInvalidRect();

    // Start with the bottom
    PegColor tColor(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    Rectangle(tInvalid, tColor);

    // Redraw the Y axis line
    if(tInvalid.wLeft == mChartRegion.wLeft)
    {
        if(GetExStyle() & CS_DRAWYTICS && 
                (mwMajorYScale > 0 || mwMinorYScale > 0))
        {
            tColor.uForeground = muColors[PCI_STEXT];
            tColor.uFlags = CF_NONE;
            PegChartPoint tPoint(mlMinX, mlMaxY);
            MapDataToPoint(&tPoint);
            Line(mChartRegion.wLeft, tPoint.mScreenPoint.y, mChartRegion.wLeft,
                    mChartRegion.wBottom, tColor);
        }
    }

    // Draw the grids
    if(GetExStyle() & CS_DRAWXGRID && (mwMajorXScale > 0 || mwMinorXScale > 0))
    {
        DrawXGrid();
    }

    if(GetExStyle() & CS_DRAWYGRID && (mwMajorYScale > 0 || mwMinorYScale > 0))
    {
        DrawYGrid();
    }

    // If the X scale is in the chart region, redraw it
    if(GetExStyle() & CS_DRAWXTICS && (mwMajorXScale > 0 || mwMinorXScale > 0))
    {
        if(GetExStyle() & CS_XAXISONZEROY)
        {
            DrawXTics();

            if(GetExStyle() & CS_DRAWXLABELS && mwXLabelScale > 0)
            {
                DrawXLabels();
            }
        }
        else
        {
            tColor.uForeground = muColors[PCI_STEXT];
            tColor.uFlags = CF_NONE;
            Line(tInvalid.wLeft, mChartRegion.wBottom, tInvalid.wRight,
                    mChartRegion.wBottom, tColor);
        }
    }

    // Draw line history if we have more than one line
    PegStripChartLine* pLine = mpLines;

    if(GetExStyle() & CS_DRAWAGED)
    {
        if(pLine->GetNext())
        {
            while(pLine)
            {
                if(pLine->ScrollFilled())
                {
                    DrawLineHistory(pLine);
                }
                pLine = pLine->GetNext();
            }
        }
    }

    // Draw the lines
    pLine = mpLines;
    while(pLine)
    {
        DrawLine(pLine);
        pLine = pLine->GetNext();
    }
}

/*--------------------------------------------------------------------------*/
PegStripChartLine* PegStripChart::GetLineByID(UCHAR ucID)
{
	PegStripChartLine* pLine = mpLines;

	while(pLine)
	{
		if(pLine->GetID() == ucID)
		{
			return pLine;
		}

		pLine = pLine->GetNext();
	}

	return NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegStripChartLine::PegStripChartLine(PegStripChart* pParent, UCHAR ucId, 
                                     COLORVAL tColor, COLORVAL tAgedColor,
                                     COLORVAL tFillColor /*=0*/,
                                     COLORVAL tLeaderColor /*=0*/) :
	mcID(ucId),
	mwHead(0), 
	mwTail(0), 
	mwViewHead(0), 
	mwViewTail(0),
	mtColor(tColor),
	mtAgedColor(tAgedColor),
	mtFillColor(tFillColor),
    mtLeaderColor(tLeaderColor),
    mbScrollFilled(FALSE),
	mpNext(NULL), 
	mpParent(pParent)
{
    WORD wDataSize = (WORD)mpParent->GetMaxX();
    mwRecordLength = (WORD)mpParent->GetMaxX();
    
    mpData = new LONG[wDataSize];
    memset(mpData, 0, wDataSize * sizeof(LONG));

    if(mtLeaderColor == 0)
    {
        mtLeaderColor = mtColor;
    }
}

/*--------------------------------------------------------------------------*/
PegStripChartLine::~PegStripChartLine()
{
    if(mpData)
    {
        delete [] mpData;
        mpData = NULL;
    }
}

/*--------------------------------------------------------------------------*/
BOOL PegStripChartLine::AddData(LONG lData)
{
    if(++mwHead >= mwRecordLength)
    {
        mwHead = 0;
        //mpParent->SetFilled(TRUE);
        mbScrollFilled = TRUE;
    }

    mpData[mwHead] = lData;

    if(mwHead == mwRecordLength - 1)
    {
        mwTail = 0;
    }
    else
    {
        mwTail = mwHead + 1;
    }

    return TRUE;
}

#endif // PEG_CHARTING

