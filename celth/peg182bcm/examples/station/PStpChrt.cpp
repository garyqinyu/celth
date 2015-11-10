/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pstpchrt.cpp - Strip chart class definition.
//
// Author: jad
//
// Copyright (c) 1997-1999 Swell Software 
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

#include "Peg.hpp"
#include "PStpChrt.hpp"

/*--------------------------------------------------------------------------*/
PegStripChart::PegStripChart(const PegRect& rect, WORD style, WORD xWidth,
    LONG minY, LONG maxY) : PegWindow(rect, style)
{
    //mwXExtent = xWidth;
    mwClientDataWidth = xWidth;
    mlMinY = minY;
    mlMaxY = maxY;
    //mwDataPerY = 0;
    mpLines = NULL;
    mcLineID = 0;

    InitClient();
    
    // Figure how many elements of data will fit on a screen 
    //mwClientDataWidth = (WORD)(mClient.Width() / mwXExtent);

    // Assign it's type
    Type(TYPE_HMI_STRIPCHART);    
}

/*--------------------------------------------------------------------------*/
PegStripChart::~PegStripChart()
{
    // Delete all my children...but, Rex, I thought he was going to live...
    // damn it...you tell Billy...that...that...that his father is...dead.
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
BYTE PegStripChart::AddLine(const PegColor& color, WORD historySize)
{
    if(mcLineID >= 255)
    {
        return((BYTE) -1);
    }

    if(mpLines == NULL)
    {
        mpLines = new PegStripChartLine(this, mcLineID, color, historySize);
    }
    else
    {
        PegStripChartLine* temp2, *temp = mpLines;
        
        while(temp)
        {
            temp2 = temp;
            temp = temp->GetNext();
        }

        // temp2 should be pointing at the tail
        PegStripChartLine* newLine = new PegStripChartLine(this, mcLineID, 
                                        color, historySize);
        temp2->SetNext(newLine);
    }

    return mcLineID++;
}

/*--------------------------------------------------------------------------*/
BOOL PegStripChart::RemoveLine(BYTE id)
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
            PegStripChartLine* temp = mpLines;
            mpLines = mpLines->GetNext();
            delete temp;
        }

        return TRUE;
    }
    else
    {
        PegStripChartLine* temp2, *temp = mpLines;
        while(temp)
        {
            temp2 = temp;
            temp = temp->GetNext();
            
            // Handle hitting the end of the list and
            // not finding the ID
            if(temp == NULL) continue;

            if(temp->GetID() == id)
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
SIGNED PegStripChart::Message(const PegMessage& msg)
{
    return(PegWindow::Message(msg));
}

/*--------------------------------------------------------------------------*/
WORD PegStripChart::AddData(BYTE id, LONG rawData)
{
    // Normalize the data and add it to the data array
    //WORD normData = NormalizeData(rawData);
    //WORD normData = (SIGNED)ValToPointY(rawData);
    WORD normData = (WORD)rawData;

    PegStripChartLine* temp = mpLines;
    while(temp)
    {
        if(temp->GetID() == id)
        {
            if(temp->AddData(normData))
            {
                return normData;
            }
            else
            {
                break;
            }
        }
        temp = temp->GetNext();
    }
        
    return((WORD) -1);
}

/*--------------------------------------------------------------------------*/
// Based on the minY and maxY, we have to convert the raw data into a
// value that is appropriate to the Y value in relation to the client
// region of the chart.
/*--------------------------------------------------------------------------*/
/*WORD PegStripChart::NormalizeData(LONG rawData)
{
    // We may only have to do this conversion once in the constructor
    // if we don't allow the client region to be dynamically sized.
    mwDataPerY = CalculateYScale();

    if(rawData > mlMaxY)
    {
        return((WORD)mClient.wTop);
    }
    
    if(rawData < mlMinY)
    {
        return((WORD)mClient.wBottom);
    }

    //return((WORD)((rawData / mwDataPerY) + mClient.wTop));
    return((WORD)(((mlMaxY - rawData) / mwDataPerY) + mClient.wTop));
}*/

/*--------------------------------------------------------------------------*/
// Figure out how much data per pixel. In other words, given
// the min and max of y and the client.Height, how large of
// a number do we have to have to move one pixel up or down.
// diff(min, max) / client.Height
// Watch your signs...
// This will introduce disparity in the data if the min/max diff is 
// small.
/*--------------------------------------------------------------------------*/
/*WORD PegStripChart::CalculateYScale()
{
    WORD dSpan = abs(mlMaxY - mlMinY);

    return((WORD)(dSpan / mClient.Height()));    
}*/

/*--------------------------------------------------------------------------*/
SIGNED PegStripChart::ValToPointY(LONG lVal)
{
    SIGNED iHeight = mClient.Height();

    LONG lTotalVal = mlMaxY - mlMinY;
    DOUBLE dStep = (DOUBLE)(iHeight << 3) / (DOUBLE)(lTotalVal - 1);
    LONG lCurStep = (LONG)((lVal - mlMinY) * dStep);

    SIGNED iRetVal = mClient.wBottom - (SIGNED)(lCurStep >> 3);
    return(iRetVal);
}

/*--------------------------------------------------------------------------*/
SIGNED PegStripChart::IndexToPoint(WORD wIndex)
{
    SIGNED iWidth = mClient.Width();

    DOUBLE dStep = (DOUBLE)(iWidth << 3) / (DOUBLE)(mwClientDataWidth - 1);
    WORD wCurStep = (WORD)(wIndex * dStep);

    SIGNED iRetVal = mClient.wLeft + (SIGNED)(wCurStep >> 3);
    return(iRetVal);
}

/*--------------------------------------------------------------------------*/
void PegStripChart::Draw(void)
{
    PegStripChartLine* line = mpLines;
    
    BeginDraw();
    
    // Inefficent, but just see if we get a good line
    PegColor rectColor(BLACK, BLACK, CF_FILL);
    Rectangle(mClient, rectColor, 0);
    SIGNED iMidY = (SIGNED)(((mlMaxY - mlMinY) / 2) + mlMinY);

    while(line)
    {
        PegColor lineColor(LIGHTGREEN, BLACK);
        PegColor agedColor(GREEN, BLACK);

        if(Style() & SC_PAGED)
        {        
            if(Style() & SC_DRAWXAXIS)
            {
                PegColor axisColor(DARKGRAY, BLACK);
                Line(mClient.wLeft, ValToPointY(iMidY), mClient.wRight, ValToPointY(iMidY),
                    axisColor, 1);
            }
        
            INT i;
            for (i = 0; i < line->GetHead(); i++)
            {
                Line(IndexToPoint(i), ValToPointY(line->GetData(i)),
                    IndexToPoint(i + 1), ValToPointY(line->GetData(i + 1)),
                    line->GetColor(), 1);
            }
        
            if(Style() & SC_DRAWAGED)
            {
                Line(IndexToPoint(i), mClient.wTop, 
                     IndexToPoint(i), mClient.wBottom, agedColor, 1);

                // Draw from the present (mwTail) to the right client edge.
                for(i = line->GetTail() + 1; i < mwClientDataWidth; i++)
                {
                    Line(IndexToPoint(i), ValToPointY(line->GetData(i)),
                         IndexToPoint(i + 1), ValToPointY(line->GetData(i + 1)),
                         agedColor, 1);
                }
            }      
        }
        
        line = line->GetNext();
    }

    EndDraw();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegStripChartLine::PegStripChartLine(PegStripChart* parent, BYTE id, 
                                     PegColor color, WORD historySize /*=0*/) :
mpParent(parent), mcID(id), mColor(color), mwHistorySize(historySize), 
mpNext(NULL), mwHead(0), mwTail(0), mwViewHead(0), mwViewTail(0)
{
    WORD dataSize = 0;
    dataSize = mwHistorySize + mpParent->GetClientDataWidth();
    mwRecordLength = mpParent->GetClientDataWidth();
    
    mpData = new WORD[dataSize];
    memset(mpData, 0, dataSize);
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
BOOL PegStripChartLine::AddData(WORD data)
{
    if(++mwHead >= mwRecordLength)
    {
        mwHead = 0;
    }
    
    mpData[mwHead] = data;
    
    if(mpParent->Style() & SC_SCROLLED)
    {

    }
    else if(mpParent->Style() & SC_PAGED)
    {
        if(mwHead == mwRecordLength - 1)
        {
            mwTail = 0;
        }
        else
        {
            mwTail = mwHead + 1;
        }

    }   
    
    return TRUE;
}
