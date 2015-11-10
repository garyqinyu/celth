/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pfdial.cpp - Implementation of the PegFiniteDial class
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
PegFiniteDial::PegFiniteDial(const PegRect& Rect, SIGNED iMinAngle,
                             SIGNED iMaxAngle, LONG lMinValue,
                             LONG lMaxValue, WORD wStyle) :
    PegDial(Rect, wStyle)
{
    Type(TYPE_FDIAL);
    SetLimits(iMinAngle, iMaxAngle, lMinValue, lMaxValue);
}

/*--------------------------------------------------------------------------*/
PegFiniteDial::PegFiniteDial(SIGNED iLeft, SIGNED iTop, SIGNED iMinAngle,
                             SIGNED iMaxAngle, LONG lMinValue, 
                             LONG lMaxValue, WORD wStyle) :
    PegDial(iLeft, iTop, wStyle)
{
    Type(TYPE_FDIAL);
    SetLimits(iMinAngle, iMaxAngle, lMinValue, lMaxValue);
}

/*--------------------------------------------------------------------------*/
PegFiniteDial::~PegFiniteDial()
{
}

/*--------------------------------------------------------------------------*/
void PegFiniteDial::Draw()
{
    BeginDraw();

    if((mwStyle & AF_TRANSPARENT) && Parent())
    {
        StandardBorder(Parent()->muColors[PCI_NORMAL]);
    }
    else 
    {
        StandardBorder(muColors[PCI_NORMAL]);
    }

    // If this is set, we've already calculated the needle pos
    if(!mbValueSet)
    {
    	CalcNeedlePos();
    }
    	
    DrawDial();

    if(mwStyle & DS_TICMARKS)
    {
        DrawTicMarks();
    }

    DrawChildren();

    DrawNeedle();
    DrawAnchor();

	EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegFiniteDial::SetLimits(SIGNED iMinAngle, SIGNED iMaxAngle,
    LONG lMinValue, LONG lMaxValue)
{
    miMinAngle = iMinAngle;
    mlMinValue = lMinValue;
    mlMaxValue = lMaxValue;
    mlCurValue = lMinValue;

    if(iMaxAngle > iMinAngle)
    {
        miMaxAngle = iMaxAngle - 360;
    }
    else
    {
        miMaxAngle = iMaxAngle;
    }

    CalcNeedlePos();
}


/*--------------------------------------------------------------------------*/
void PegFiniteDial::CalcNeedlePos()
{
    mtNeedlePos.wRight = mClient.wLeft + (mClient.Width() >> 1);
    mtNeedlePos.wBottom = mClient.wTop + (mClient.Height() >> 1);

    if(mlCurValue <= mlMinValue)
    {
        miCurAngle = miMinAngle;
    }
    else if(mlCurValue >= mlMaxValue)
    {
        miCurAngle = miMaxAngle;
    }
    else
    {
        miCurAngle = ValToAngle(mlCurValue);
    }

    WORD wRadius = ((PEGMIN((mClient.Width() >> 1), (mClient.Height() >> 1)))
    				* mwNeedleLen) / 100;

    SIGNED iSin, iCos;
    PegLookupSinCos(miCurAngle, &iSin, &iCos);
    mtNeedlePos.wLeft = mtNeedlePos.wRight + (WORD)((iCos * wRadius) >> 10);
    mtNeedlePos.wTop = mtNeedlePos.wBottom - (WORD)((iSin * wRadius) >> 10);
}

/*--------------------------------------------------------------------------*/
SIGNED PegFiniteDial::ValToAngle(LONG lVal)
{
    SIGNED iTotalDeg = miMinAngle - miMaxAngle;
    LONG lTotalVal = mlMaxValue - mlMinValue;
    
	LONG lStep = (LONG)(iTotalDeg << 8) / (lTotalVal - 1);
    
	LONG lCurStep = (LONG)((lVal - mlMinValue) * lStep);    
	SIGNED iAngle = miMinAngle - (SIGNED)(lCurStep >> 8);

    return(iAngle);
}

/*--------------------------------------------------------------------------*/
void PegFiniteDial::DrawTicMarks()
{
    if(!mlTicFreq)
    {
        return;
    }

    PegPoint tPt1, tPt2;
    PegColor Color(PCLR_SHADOW, PCLR_SHADOW);
    for(LONG i = mlMinValue; i <= mlMaxValue; i += mlTicFreq)
    {
        CalcTicPos(i, tPt1, tPt2);
        Line(tPt1.x, tPt1.y, tPt2.x, tPt2.y, Color);
    }
}

#endif	// PEG_HMI_GADGETS

