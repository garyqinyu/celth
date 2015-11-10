/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pchart.hpp - Chart base class definition.
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

#ifndef _PEGCHART_
#define _PEGCHART_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
struct PegChartPoint
{
    PegPoint mScreenPoint;
    LONG     mlDataX;
    LONG     mlDataY;
    PegChartPoint* mpNext;

    PegChartPoint()
    {
        mpNext = NULL;
        mlDataX = 0;
        mlDataY = 0;
        mScreenPoint.x = 0;
        mScreenPoint.y = 0;
    }

    PegChartPoint(LONG x, LONG y, PegChartPoint* Next = NULL)
    {
        mlDataX = x;
        mlDataY = y;
        mpNext = Next;
        mScreenPoint.x = 0;
        mScreenPoint.y = 0;
    }
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegChart : public PegThing
{
    public:
        PegChart(const PegRect& rect, LONG lMinX = 0, LONG lMaxX = 0,
                 LONG lMinY = 0, LONG lMaxY = 0, WORD wMajorXScale = 0,
			     WORD wMajorYScale = 0);
        virtual ~PegChart() {}

        virtual void Draw();
        virtual SIGNED Message(const PegMessage& Mesg);
        virtual void ParentShift(SIGNED xOffset, SIGNED yOffset);

        virtual void RecalcLayout(BOOL bRedraw = TRUE);
        virtual void RecalcSize(const PegRect& NewRect, BOOL bRedraw = TRUE);

        virtual void MapDataToPoint(PegChartPoint* pPoint);
        virtual void MapPointToData(PegChartPoint* pPoint);

        LONG GetMinX() const { return mlMinX; }
        LONG GetMaxX() const { return mlMaxX; }
        LONG GetMinY() const { return mlMinY; }
        LONG GetMaxY() const { return mlMaxY; }
        WORD GetMajorXScale() const { return mwMajorXScale; }
		WORD GetMinorXScale() const { return mwMinorXScale; }
        WORD GetMajorYScale() const { return mwMajorYScale; }
		WORD GetMinorYScale() const { return mwMinorYScale; }
        WORD GetMajorTicSize() const { return mwMajorTicSize; }
		WORD GetMinorTicSize() const { return mwMinorTicSize; }
        WORD GetXLabelScale() const { return mwXLabelScale; }
        WORD GetYLabelScale() const { return mwYLabelScale; }

        void SetMinX(LONG lData) { mlMinX = lData; }
        void SetMaxX(LONG lData) { mlMaxX = lData; }
        void SetMinY(LONG lData) { mlMinY = lData; }
        void SetMaxY(LONG lData) { mlMaxY = lData; }
    
        void SetMajorXScale(WORD wScale)
        {
            mwMajorXScale = wScale;
            RecalcLayout(FALSE);
        }

        void SetMinorXScale(WORD wScale)
        {
            mwMinorXScale = wScale;
			if(mwMinorXScale > mwMajorXScale)
			{
            	RecalcLayout(FALSE);
			}
        }

        void SetMajorYScale(WORD wScale)
        {
            mwMajorYScale = wScale;
            RecalcLayout(FALSE);
        }

        void SetMinorYScale(WORD wScale)
        {
            mwMinorYScale = wScale;
			if(mwMinorYScale > mwMajorYScale)
			{
            	RecalcLayout(FALSE);
			}
        }

        void SetMajorTicSize(WORD wSize)
        {
            mwMajorTicSize = wSize;
            RecalcLayout(FALSE);
        }

        void SetMinorTicSize(WORD wSize)
        {
            mwMinorTicSize = wSize;
            RecalcLayout(FALSE);
        }

        void SetXLabelScale(WORD wScale)
        {
            mwXLabelScale = wScale;
            RecalcLayout(FALSE);
        }

        void SetYLabelScale(WORD wScale)
        {
            mwYLabelScale = wScale;
            RecalcLayout(FALSE);
        }

        WORD GetExStyle() const { return mwExStyle; }
        void SetExStyle(WORD wStyle)
        { 
            mwExStyle = wStyle;
            RecalcLayout(FALSE);
        }

		WORD GetYLabelWidth() const { return mwYLabelWidth; }
		void SetYLabelWidth(WORD wWidth)
		{
			mwYLabelWidth = wWidth;
			RecalcLayout(FALSE);
		}

		WORD GetXLabelHeight() const { return mwXLabelHeight; }
		void SetXLabelHeight(WORD wHeight)
		{
			mwXLabelHeight = wHeight;
			RecalcLayout(FALSE);
		}

	    PegFont* GetFont() const { return mpFont; }
	    void SetFont(PegFont* pFont) { mpFont = pFont; }

        PegRect GetChartRegion() const { return mChartRegion; }

    protected:
        virtual void DrawXTics();
        virtual void DrawYTics();
        virtual void DrawXGrid();
        virtual void DrawYGrid();
        virtual void DrawXLabels();
        virtual void DrawYLabels();

        LONG mlMinX;
        LONG mlMaxX;
        LONG mlMinY;
        LONG mlMaxY;

        WORD mwMajorXScale;
		WORD mwMinorXScale;
        WORD mwMajorYScale;
		WORD mwMinorYScale;
        WORD mwMajorTicSize;
		WORD mwMinorTicSize;
        WORD mwXLabelScale;
        WORD mwYLabelScale;
		WORD mwXLabelHeight;
		WORD mwYLabelWidth;

        WORD mwExStyle;

	    PegFont* mpFont;

        PegRect mChartRegion;
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegChartLine
{
    public:
        PegChartLine(PegChart* pParent, UCHAR ucID, COLORVAL tColor);
        virtual ~PegChartLine();

        virtual void Draw();

        PegChartPoint* AddPoint(LONG lX, LONG lY);
        PegChartPoint* InsertPoint(PegChartPoint* pPreviousPoint, LONG lX, LONG lY);
        PegChartPoint* RemovePoint(PegChartPoint* pPoint);
        void RecalcLine();
        void ResetLine();
        void MapDataToPoint(PegChartPoint* pPoint) 
        { 
            mpParent->MapDataToPoint(pPoint);
        }

        PegChartLine* GetNext() const { return mpNext; }
        void SetNext(PegChartLine* pLine) { mpNext = pLine; }

        PegChartPoint* GetFirstPoint() const { return mpPoints; }

        COLORVAL GetLineColor() const { return mtColor.uForeground; }
        void SetLineColor(COLORVAL tColor) { mtColor.uForeground = tColor; }
		COLORVAL GetFillColor() const { return mtBkgColor.uForeground; }
		void SetFillColor(COLORVAL tColor) 
		{ 
			mtBkgColor.uForeground = tColor;
		}

        UCHAR GetID() const { return mucID; }

    protected:
        PegChartPoint*  mpPoints;
        PegChart*       mpParent;
        UCHAR           mucID;
        PegColor        mtColor;
		PegColor		mtBkgColor;
        PegChartLine*   mpNext;
};

#endif // _PEGCHART_


