/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plscale.cpp - Implementation of the PegLinearScale class
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
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#if defined(PEG_HMI_GADGETS)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegLinearScale::PegLinearScale(const PegRect &Size, 
                               LONG lMinValue, LONG lMaxValue, 
                               LONG lTicFreq, WORD wStyle) :
    PegScale(Size, lMinValue, lMaxValue, wStyle),
    mlTicFreq(lTicFreq)
{
    Type(TYPE_LIN_SCALE);
}

/*--------------------------------------------------------------------------*/
PegLinearScale::PegLinearScale(SIGNED iLeft, SIGNED iTop, 
                               LONG lMinValue, LONG lMaxValue,
                               LONG lTicFreq, WORD wStyle) :
    PegScale(iLeft, iTop, lMinValue, lMaxValue, wStyle),
    mlTicFreq(lTicFreq)
{
    Type(TYPE_LIN_SCALE);
}

/*--------------------------------------------------------------------------*/
PegLinearScale::~PegLinearScale()
{
}

/*--------------------------------------------------------------------------*/
void PegLinearScale::Draw()
{
    BeginDraw();

    SetTravelPoints();

    DrawScale();

    if(mwStyle & SS_TICMARKS)
    {
        DrawTicMarks();
    }

    DrawChildren();
    
    DrawNeedle();

    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegLinearScale::DrawScale()
{
    if(mwStyle & AF_TRANSPARENT && Parent())
    {
        StandardBorder(Parent()->muColors[PCI_NORMAL]);
    }
    else
    {
        StandardBorder(muColors[PCI_NORMAL]);
    }

    if(mwStyle & SS_ORIENTVERT)
    {
        SIGNED iStart;
        SIGNED iWidth = mClient.Width();

        if(mwStyle & SS_FACELEFT)
        {
            iStart = mClient.wLeft + (iWidth >> 1) + (iWidth >> 2) - 1;
        }
        else
        {
            iStart = mClient.wLeft + (iWidth >> 2) - 1;
        }

        PegColor tColor(PCLR_LOWLIGHT);
        Line(iStart, miMaxTravel, iStart, miMinTravel, tColor, 1);
        iStart++;
        tColor.uForeground = PCLR_SHADOW;
        Line(iStart, miMaxTravel, iStart, miMinTravel, tColor, 1);
        tColor.uForeground = PCLR_HIGHLIGHT;
        iStart++;
        Line(iStart, miMaxTravel, iStart, miMinTravel, tColor, 1);
    }
    else
    {
        SIGNED iStart;
        SIGNED iHeight = mClient.Height();

        if(mwStyle & SS_FACELEFT)
        {
            iStart = mClient.wTop + (iHeight >> 1) + (iHeight >> 2) - 1;
        }
        else
        {
            iStart = mClient.wTop + (iHeight >> 2) - 1;
        }

        PegColor tColor(PCLR_LOWLIGHT);
        Line(miMinTravel, iStart, miMaxTravel, iStart, tColor, 1);
        tColor.uForeground = PCLR_SHADOW;
        iStart++;
        Line(miMinTravel, iStart, miMaxTravel, iStart, tColor, 1);
        tColor.uForeground = PCLR_HIGHLIGHT;
        iStart++;
        Line(miMinTravel, iStart, miMaxTravel, iStart, tColor, 1);
    }
}

/*--------------------------------------------------------------------------*/
void PegLinearScale::DrawTicMarks()
{
    if(mlTicFreq == 0)
    {
        return;
    }

    PegPoint Point1, Point2;
    LONG lVal = mlMinValue;
    SIGNED iTotal = ((mlMaxValue - mlMinValue) / mlTicFreq) + 1;

    PegColor tColor(PCLR_SHADOW);

    if(mwStyle & SS_ORIENTVERT)
    {
        SIGNED iWidth = mClient.Width() >> 2;

        for(SIGNED i = 0; i < iTotal; i++)
        {
            ValToPoint(lVal, Point1);
            if(mwStyle & SS_FACELEFT)
            {
                Point1.x = mClient.wLeft;
                Point2.x = Point1.x + iWidth;
            }
            else
            {
                Point1.x = mClient.wRight;
                Point2.x = Point1.x - iWidth;
            }

            Point2.y = Point1.y;

            Line(Point1.x, Point1.y, Point2.x, Point2.y, tColor, 1);

            lVal += mlTicFreq;
        }
    }
    else
    {
        SIGNED iHeight = mClient.Height() >> 2;

        for(SIGNED i = 0; i < iTotal; i++)
        {
            ValToPoint(lVal, Point1);
            if(mwStyle & SS_FACELEFT)
            {
                Point1.y = mClient.wTop;
                Point2.y = Point1.y + iHeight;
            }
            else
            {
                Point1.y = mClient.wBottom;
                Point2.y = Point1.y - iHeight;
            }

            Point2.x = Point1.x;

            Line(Point1.x, Point1.y, Point2.x, Point2.y, tColor, 1);

            lVal += mlTicFreq;
        }
    }            
}

#endif	// PEG_HMI_GADGETS

