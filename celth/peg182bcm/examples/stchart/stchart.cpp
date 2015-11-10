/*--------------------------------------------------------------------------*/
// stchart.cpp - PegStripChart example
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-2002 Swell Software, Inc. 
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

SIGNED giChartData[] = { 100, 100, 100, 100, 100, 100, 125, 150, 175, 200, 
                         150, 100, 50, 100, 125, 100, 100, 100 };

SIGNED giBukData[] = { 100, 100, 100, 100, 100, 100, 100, 100, 125, 100, 
                       100, 120, 140, 160, 180, 200, 150, 100, 50, 125, 100,
                       100, 125, 150, 125, 100, 100, 100, 100 };

/*--------------------------------------------------------------------------*/
class StripChartWindow : public PegDecoratedWindow
{
    public:
        StripChartWindow(const PegRect& Rect, PEGCHAR* pTitle);
        ~StripChartWindow() {}

        SIGNED Message(const PegMessage& Mesg);

        PegStripChart* mpChart;
        PegStripChart* mpChart2;
        
        WORD mwIndex;
        UCHAR mcID;
        UCHAR mcSin;
        UCHAR mcID2;
        UCHAR mcSin2;
};

StripChartWindow::StripChartWindow(const PegRect& Rect, PEGCHAR* pTitle) :
    PegDecoratedWindow(Rect)
{
    if(pTitle)
    {
        Add(new PegTitle(pTitle));
    }

    PegRect ChartRect = mClient;
    ChartRect.wBottom = mClient.Height() / 2 - 2;
    mpChart = new PegStripChart(ChartRect, 130, -100, 700, 10, 100);
    mpChart->SetExStyle(mpChart->GetExStyle() | CS_DRAWYGRID | CS_DRAWXGRID | 
                        CS_DRAWXTICS | CS_DRAWYLABELS | CS_SCROLLED |
						//CS_DUALYTICS | CS_DRAWLINEFILL | CS_DRAWXLABELS |
						CS_DUALYTICS | CS_DRAWXLABELS |
                        CS_DUALYLABELS);
    mpChart->SetExStyle(mpChart->GetExStyle() & ~CS_PAGED);
    Add(mpChart);
    mcSin = mpChart->AddLine(CYAN, BLUE, MAGENTA);
    mcID = mpChart->AddLine(LIGHTGREEN, GREEN, RED);
    mpChart->SetYLabelScale(200);

    ChartRect.wTop = ChartRect.wBottom + 4;
    ChartRect.wBottom = mClient.wBottom;
    mpChart2 = new PegStripChart(ChartRect, 130, -200, 600, 10, 100);
    mpChart2->SetExStyle(mpChart2->GetExStyle() | CS_DRAWAGED |
						 //CS_XAXISONZEROY | CS_DRAWXTICS | CS_DRAWXLABELS |
						 CS_XAXISONZEROY | CS_DRAWXTICS | CS_DRAWXLABELS);
						 //CS_DRAWLINEFILL);
	
	Add(mpChart2);
    mcSin2 = mpChart2->AddLine(LIGHTBLUE, BLUE, CYAN);
    mcID2 = mpChart2->AddLine(LIGHTGREEN, GREEN, GREEN);
}

SIGNED StripChartWindow::Message(const PegMessage& Mesg)
{
    static SIGNED iIndex = 0, j = 0;
    static SIGNED iAngle = 0;

    switch(Mesg.wType)
    {
    case PM_SHOW:
        PegWindow::Message(Mesg);
        SetTimer(1, ONE_SECOND, ONE_SECOND / 10);
        break;

    case PM_HIDE:
        PegWindow::Message(Mesg);
        KillTimer(1);
        break;

    case PM_TIMER:
        {
            if(iIndex++ > 17)
            {
                iIndex = 0;
            }

            mpChart->AddData(mcID, giChartData[iIndex]);

            if(j++ > 28)
            {
                j = 0;
            }
            mpChart2->AddData(mcID2, giBukData[j]);

            if((iAngle += 10) > 350)
            {
                iAngle = 0;
            }

           	SIGNED iSin, iCos;
			PegLookupSinCos(iAngle, &iSin, &iCos);
			mpChart->AddData(mcSin, (LONG)((iSin * 125) >> 10) + 400);
			mpChart2->AddData(mcSin2, (LONG)((iCos * 125) >> 10) + 400);
        }

    default:
        {
            return(PegWindow::Message(Mesg));            
        }        
    }

    return 0;
}

void PegAppInitialize(PegPresentationManager* pPresentation)
{
    PegRect Rect;
    Rect.Set(10, 10, 630, 470);

    StripChartWindow* pWindow = new StripChartWindow(Rect, "PegStripChart Example");
    pPresentation->Add(pWindow);

    /*Rect.Set(20, 20, 200, 200);
    PegDecoratedWindow* pDecWin = new PegDecoratedWindow(Rect);
    pDecWin->Add(new PegTitle("Cover Up Window"));
    pPresentation->Add(pDecWin);
    */
}
