/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pmlchart.hpp - Multi line chart class definition 
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

#ifndef _PEGMULTILINECHART_
#define _PEGMULTILINECHART_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegMultiLineChart : public PegChart
{
	public:
	    PegMultiLineChart(const PegRect &Size, LONG lMinX, LONG lMaxX,
	                      LONG lMinY, LONG lMaxY, WORD wXScale = 0,
	                      WORD wYScale = 0);
	    virtual ~PegMultiLineChart();
	
	    virtual void Draw();
	    virtual SIGNED Message(const PegMessage &Mesg);

        virtual void Resize(PegRect NewSize);
	    virtual void RecalcSize(const PegRect &NewSize, BOOL bRedraw = TRUE);
		void DrawNewLineData(PegChartLine* pLine, PegChartPoint* pNew, 
							 PegChartPoint* pPrevious = NULL);
	    
	    void RecalcLine(UCHAR ucID, BOOL bRedraw = TRUE);
	    void ResetLine(UCHAR ucID, BOOL bRedraw = TRUE);
	    void ResetAllLines(BOOL bRedraw = TRUE);
	
	    UCHAR AddLine(COLORVAL tColor);
	    BOOL RemoveLine(UCHAR ucID);
	
	    PegChartPoint* AddPoint(UCHAR ucID, LONG lX, LONG lY, BOOL bRedraw = TRUE);
	    PegChartPoint* InsertPoint(UCHAR ucID, PegChartPoint* pPreviousPoint,
	                               LONG lX, LONG lY, BOOL bRedraw = TRUE);
	    PegChartPoint* RemovePoint(UCHAR ucID, PegChartPoint* pPoint, 
	                               BOOL bRedraw = TRUE);
	
	    PegChartLine* GetFirstLine() const { return mpLines; }
	    PegChartLine* GetLineFromID(UCHAR ucID);
	
	protected:
		PegChartLine* mpLines;
	    UCHAR mcLineID;
};

#endif // _PEGMULTILINECHART_


