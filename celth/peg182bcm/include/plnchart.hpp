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

#ifndef _PEGLINECHART_
#define _PEGLINECHART_

class PegLineChart : public PegChart
{
	public:
	    PegLineChart(const PegRect &Size, LONG lMinX, LONG lMaxX, LONG lMinY,
	                 LONG lMaxY, WORD wXScale = 0, WORD wYScale = 0);
	    virtual ~PegLineChart();
	
	    virtual void Draw();
	    virtual SIGNED Message(const PegMessage& Mesg);
        virtual void Resize(PegRect NewSize);

	    virtual void RecalcSize(const PegRect& NewSize, BOOL bRedraw = TRUE);
	
	    void RecalcLine();
	    void ResetLine();
	    
	    PegChartPoint* AddPoint(LONG lX, LONG lY);
	    PegChartPoint* InsertPoint(PegChartPoint* pPreviousPoint,
            LONG lX, LONG lY);
	    PegChartPoint* RemovePoint(PegChartPoint* pPoint);
	
	    PegChartPoint* GetFirstPoint() const { return mpPoints; }
	
	    COLORVAL GetLineColor() const { return mcLineColor; }
	    COLORVAL SetLineColor(COLORVAL cColor)
	    {
	        COLORVAL cTemp = mcLineColor;
	        mcLineColor = cColor;
	        return cTemp;
	    }
	    
	protected:
	    PegChartPoint* mpPoints;
	    COLORVAL mcLineColor;
};

#endif // _PEGLINECHART_


