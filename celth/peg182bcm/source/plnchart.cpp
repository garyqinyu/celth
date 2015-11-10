/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plnchart.hpp - PegLineChart class definition.
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

#if defined(PEG_CHARTING)

/*--------------------------------------------------------------------------*/
PegLineChart::PegLineChart(const PegRect& Rect, LONG lMinX, LONG lMaxX,
						   LONG lMinY, LONG lMaxY, WORD wXScale /*=0*/,
                           WORD wYScale /*=0*/) : 
    PegChart(Rect, lMinX, lMaxX, lMinY, lMaxY, wXScale, wYScale),
    mpPoints(NULL)
{
    Type(TYPE_LINE_CHART);

   #if(PEG_NUM_COLORS >= 16)
    mcLineColor = LIGHTGREEN;
   #else
    mcLineColor = WHITE;
   #endif
}

/*--------------------------------------------------------------------------*/
PegLineChart::~PegLineChart()
{
    ResetLine();
}

/*--------------------------------------------------------------------------*/
void PegLineChart::Draw()
{
    PegColor Color(mcLineColor, mcLineColor, CF_NONE);

    BeginDraw();

    PegChart::Draw();

    PegChartPoint* pPoint = mpPoints;
    PegChartPoint* pNext = NULL;
    
    if(pPoint)
    {
        pNext = pPoint->mpNext;
    }

    while(pNext)
    {
        Line(pPoint->mScreenPoint.x, pPoint->mScreenPoint.y,
             pNext->mScreenPoint.x, pNext->mScreenPoint.y, Color);

        pPoint = pNext;
        pNext = pNext->mpNext;
    }

    EndDraw();
}

/*--------------------------------------------------------------------------*/
SIGNED PegLineChart::Message(const PegMessage& Mesg)
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
        } break;

    default:
        PegChart::Message(Mesg);
        break;
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
void PegLineChart::Resize(PegRect NewSize)
{
    PegThing::Resize(NewSize);
    RecalcLayout(FALSE);
    RecalcLine();
}

/*--------------------------------------------------------------------------*/
void PegLineChart::RecalcSize(const PegRect& NewSize, BOOL bRedraw)
{
    Resize(NewSize);
    if(bRedraw)
    {
        Invalidate(mReal);
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
void PegLineChart::RecalcLine()
{
    PegChartPoint* pPoint = mpPoints;
    while(pPoint)
    {
        MapDataToPoint(pPoint);
        pPoint = pPoint->mpNext;
    }
}

/*--------------------------------------------------------------------------*/
void PegLineChart::ResetLine()
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

/*--------------------------------------------------------------------------*/
PegChartPoint* PegLineChart::AddPoint(LONG lX, LONG lY)
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
    return(pAdd);
}

/*--------------------------------------------------------------------------*/
PegChartPoint* PegLineChart::InsertPoint(PegChartPoint* pPreviousPoint,
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
    return(pAdd);
}

/*--------------------------------------------------------------------------*/
PegChartPoint* PegLineChart::RemovePoint(PegChartPoint* pPoint)
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

    return(pPrevious);
}
 

#endif // defined PEG_CHARTING


