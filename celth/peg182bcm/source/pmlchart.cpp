/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pmlchart.hpp - PegMultiLineChart class definition.
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
/*--------------------------------------------------------------------------*/
PegMultiLineChart::PegMultiLineChart(const PegRect& Rect, LONG lMinX, 
                                     LONG lMaxX, LONG lMinY, LONG lMaxY, 
                                     WORD wXScale /*=0*/,
                                     WORD wYScale /*=0*/) :
    PegChart(Rect, lMinX, lMaxX, lMinY, lMaxY, wXScale, wYScale),
    mpLines(NULL),
    mcLineID(0)
{
    Type(TYPE_MULTI_LINE_CHART);
}

/*--------------------------------------------------------------------------*/
PegMultiLineChart::~PegMultiLineChart()
{
    ResetAllLines();
}

/*--------------------------------------------------------------------------*/
void PegMultiLineChart::Draw()
{
    BeginDraw();

    PegChart::Draw();

    PegChartLine* pLine = mpLines;
    while(pLine)
    {
        pLine->Draw();
        pLine = pLine->GetNext();
    }

    EndDraw();
}

/*--------------------------------------------------------------------------*/
SIGNED PegMultiLineChart::Message(const PegMessage& Mesg)
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
void PegMultiLineChart::Resize(PegRect NewSize)
{
    PegThing::Resize(NewSize);
    RecalcLayout(FALSE);
    PegChartLine* pLine = mpLines;
    while(pLine)
    {
        pLine->RecalcLine();
        pLine = pLine->GetNext();
    }
}


/*--------------------------------------------------------------------------*/
void PegMultiLineChart::RecalcSize(const PegRect& Rect, BOOL bRedraw /*=TRUE*/)
{
    Resize(Rect);

    if(bRedraw)
    {
        Invalidate(mReal);
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
void PegMultiLineChart::DrawNewLineData(PegChartLine* pLine,
										PegChartPoint* pNew,
										PegChartPoint* pPrevious /*=NULL*/)
{
	if(pPrevious == NULL)
	{
		PegChartPoint* pTemp = pLine->GetFirstPoint();
		
		if(pTemp != pNew)
		{
			while(pTemp)
			{
				if(pTemp->mpNext == pNew)
				{
					pPrevious = pTemp;
					break;	
				}

				pTemp = pTemp->mpNext;
			}
		}
	}

	PegRect tRectPrev, tRectNew, tRectNext, tRectDraw;

	if(pPrevious)
	{
		tRectPrev.Set(pPrevious->mScreenPoint.x, pPrevious->mScreenPoint.y,
					  pPrevious->mScreenPoint.x, pPrevious->mScreenPoint.y);

		tRectNew.Set(pNew->mScreenPoint.x, pNew->mScreenPoint.y,
				     pNew->mScreenPoint.x, pNew->mScreenPoint.y);

		tRectDraw = tRectPrev;
		tRectDraw |= tRectNew;
		
		if(pNew->mpNext)
		{
			tRectNext.Set(pNew->mpNext->mScreenPoint.x,
                          pNew->mpNext->mScreenPoint.y,
						  pNew->mpNext->mScreenPoint.x,
                          pNew->mpNext->mScreenPoint.y);

			tRectDraw |= tRectNext;		
		}
	}
	else
	{
		tRectNew.Set(pNew->mScreenPoint.x, pNew->mScreenPoint.y,
				     pNew->mScreenPoint.x, pNew->mScreenPoint.y);

		tRectDraw = tRectNew;

		if(pNew->mpNext)
		{
			tRectNext.Set(pNew->mpNext->mScreenPoint.x, 
                          pNew->mpNext->mScreenPoint.y,
						  pNew->mpNext->mScreenPoint.x, 
                          pNew->mpNext->mScreenPoint.y);

			tRectDraw |= tRectNext;		
		}
	}

	Invalidate(tRectDraw);
	Draw();
}

/*--------------------------------------------------------------------------*/
void PegMultiLineChart::RecalcLine(UCHAR ucID, BOOL bRedraw /*=TRUE*/)
{
    PegChartLine* pLine = GetLineFromID(ucID);
    if(pLine)
    {
        pLine->RecalcLine();

		if(bRedraw)
		{
			Invalidate(mChartRegion);
			Draw();
		}
    }
}

/*--------------------------------------------------------------------------*/
void PegMultiLineChart::ResetLine(UCHAR ucID, BOOL bRedraw /*=TRUE*/)
{
    PegChartLine* pLine = GetLineFromID(ucID);
    if(pLine)
    {
        pLine->ResetLine();

		if(bRedraw)
		{
			Invalidate(mChartRegion);
			Draw();
		}
    }
}

/*--------------------------------------------------------------------------*/
void PegMultiLineChart::ResetAllLines(BOOL bRedraw)
{
    PegChartLine* pLine = mpLines;

    while(pLine)
    {
        pLine->ResetLine();
        pLine = pLine->GetNext();
    }

	if(bRedraw)
	{
		Invalidate(mChartRegion);
		Draw();
	}
}

/*--------------------------------------------------------------------------*/
UCHAR PegMultiLineChart::AddLine(COLORVAL tColor)
{
    if(mcLineID >= 255)
    {
        return((UCHAR) -1);
    }

    if(mpLines == NULL)
    {
        mpLines = new PegChartLine(this, mcLineID, tColor);
    }
    else
    {
        PegChartLine* temp2 = mpLines, *temp = mpLines;
        
        while(temp)
        {
            temp2 = temp;
            temp = temp->GetNext();
        }

        // temp2 should be pointing at the tail
        PegChartLine* newLine = new PegChartLine(this, mcLineID, tColor);
        temp2->SetNext(newLine);
    }

    return mcLineID++;
}

/*--------------------------------------------------------------------------*/
BOOL PegMultiLineChart::RemoveLine(UCHAR ucID)
{
    if(mpLines->GetID() == ucID)
    {
        if(mpLines->GetNext() == NULL)
        {
            delete mpLines;
            mpLines = NULL;
        }
        else
        {
            PegChartLine* temp = mpLines;
            mpLines = mpLines->GetNext();
            delete temp;
        }

        return TRUE;
    }
    else
    {
        PegChartLine* temp2, *temp = mpLines;
        while(temp)
        {
            temp2 = temp;
            temp = temp->GetNext();
            
            // Handle hitting the end of the list and
            // not finding the ID
            if(temp == NULL) continue;

            if(temp->GetID() == ucID)
            {
                temp2->SetNext(temp->GetNext());
                delete temp;

                return TRUE;
            }
        }
    }

    // Didn't find it.
    return FALSE;
}

/*--------------------------------------------------------------------------*/
PegChartPoint* PegMultiLineChart::AddPoint(UCHAR ucID, LONG lX, LONG lY,
										   BOOL bRedraw /*=TRUE*/)
{
    PegChartLine* pLine = GetLineFromID(ucID);
	PegChartPoint* pPoint = NULL;

    if(pLine)
    {
        pPoint = pLine->AddPoint(lX, lY);
    }

	if(bRedraw)
	{
		DrawNewLineData(pLine, pPoint);
	}

    return pPoint;
}

/*--------------------------------------------------------------------------*/
PegChartPoint* PegMultiLineChart::InsertPoint(UCHAR ucID, 
                                              PegChartPoint* pPreviousPoint,
                                              LONG lX, LONG lY,
											  BOOL bRedraw /*=TRUE*/)
{
    PegChartLine* pLine = GetLineFromID(ucID);
	PegChartPoint* pPoint = NULL;

    if(pLine)
    {
        pPoint = pLine->InsertPoint(pPreviousPoint, lX, lY);
    }

	if(bRedraw)
	{
		DrawNewLineData(pLine, pPoint, pPreviousPoint);
	}

    return pPoint;
}

/*--------------------------------------------------------------------------*/
PegChartLine* PegMultiLineChart::GetLineFromID(UCHAR ucID)
{
    PegChartLine* pLine = mpLines;
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


#endif // PEGCL_CHARTING

