/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pstchart.hpp - Definition for the PegStripChart object
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

#ifndef _PSTCHART_HPP_
#define _PSTCHART_HPP_

// Forward reference
class PegStripChart;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegStripChartLine
{
	public:
	    PegStripChartLine(PegStripChart* pParent, UCHAR ucId, COLORVAL tColor, 
	                      COLORVAL tAgedColor, COLORVAL tFillColor = 0,
                          COLORVAL tLeaderColor = 0); 
	    virtual ~PegStripChartLine();
	
	    BOOL AddData(LONG lData);
	
	    // Accessors
	    PegStripChartLine* GetNext() const { return mpNext; }
	    UCHAR GetID() const { return mcID; }
	    WORD GetHead() const { return mwHead; }
	    WORD GetTail() const { return mwTail; }
	    WORD GetViewHead() const { return mwViewHead; }
	    WORD GetViewTail() const { return mwViewTail; }
	    LONG* GetData() const { return mpData; }
	    LONG GetData(WORD index) { return mpData[index]; }
	    COLORVAL GetColor() const { return mtColor; }
	    COLORVAL GetAgedColor() const { return mtAgedColor; }
		COLORVAL GetFillColor() const { return mtFillColor; }
        COLORVAL GetLeaderColor() const { return mtLeaderColor; }
	    BOOL ScrollFilled() const { return mbScrollFilled; }
	
	    // Mutators
	    void SetNext(PegStripChartLine* pLine) { mpNext = pLine; }
	    COLORVAL SetColor(COLORVAL tColor) { mtColor = tColor; return mtColor; }
	    COLORVAL SetAgedColor(COLORVAL tColor) 
	    {
	        mtAgedColor = tColor;
	        return mtAgedColor; 
	    }
		COLORVAL SetFillColor(COLORVAL tColor)
		{
			mtFillColor = tColor;
			return mtFillColor;
		}
        COLORVAL SetLeaderColor(COLORVAL tColor)
        {
            mtLeaderColor = tColor;
            return mtLeaderColor;
        }
	
	    void SetScrollFilled(BOOL bFilled) { mbScrollFilled = bFilled; }
	    
	private:
	    UCHAR       mcID;
	    WORD        mwHead;
	    WORD        mwTail;
	    WORD        mwViewHead;
	    WORD        mwViewTail;
	    WORD        mwRecordLength;
	    LONG*       mpData;
	    COLORVAL    mtColor;
	    COLORVAL    mtAgedColor;
		COLORVAL	mtFillColor;
        COLORVAL    mtLeaderColor;
	    BOOL        mbScrollFilled;
	    PegStripChartLine* mpNext;
	    PegStripChart* mpParent;
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegStripChart : public PegChart
{
	public:
	    PegStripChart(const PegRect &Size, WORD wSamples, LONG lMinY,
	                  LONG lMaxY, WORD wXScale = 0, WORD wYScale = 0);
	    ~PegStripChart();
	
	    // Overrides
	    void Draw();
	    SIGNED Message(const PegMessage& Mesg);
        virtual void Resize(PegRect NewSize);
	    virtual void RecalcSize(const PegRect& NewSize, BOOL bRedraw = TRUE);
		
	    // returns the indice of the new line
	    UCHAR AddLine(COLORVAL tColor, COLORVAL tAgedColor, 
					  COLORVAL tFillColor = 0);
	    // returns success/failure
	    BOOL RemoveLine(UCHAR ucID);
	
	    // Add new data to a line
	    BOOL AddData(UCHAR ucID, LONG rawData, BOOL bRedraw = TRUE);
	    
	    // Convert raw data to screen point values
	    SIGNED ValToPointY(LONG lVal);
	    SIGNED IndexToPointX(WORD wIndex);
	
		// Line wrappers
		COLORVAL GetLineColor(UCHAR ucID);
		COLORVAL GetLineAgedColor(UCHAR ucID);
		COLORVAL GetLineFillColor(UCHAR ucID);
        COLORVAL GetLineLeaderColor(UCHAR ucID);
	
		void SetLineColor(UCHAR ucID, COLORVAL tColor);
		void SetLineAgedColor(UCHAR ucID, COLORVAL tColor);
		void SetLineFillColor(UCHAR ucID, COLORVAL tColor);
        void SetLineLeaderColor(UCHAR ucID, COLORVAL tColor);
	    
    protected:
	    // Line drawing methods
	    virtual void DrawLine(PegStripChartLine* pLine);
	    virtual void DrawLineHistory(PegStripChartLine* pLine);
		virtual void DrawNewLineData(PegStripChartLine* pLine);
        virtual void DrawInvalidChartRegion();
	
	private:
		PegStripChartLine* GetLineByID(UCHAR ucID);
	
	    UCHAR mcLineID;
	    PegStripChartLine* mpLines;
};

#endif // _PSTCHART_HPP_


