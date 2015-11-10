/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pcdial.cpp - Implementation of the PegCircularDial class
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
PegCircularDial::PegCircularDial(const PegRect& tRect, SIGNED iRefAngle,
                                 LONG lValuePerRev, LONG lMinValue,
                                 LONG lMaxValue, WORD wStyle) :
    PegDial(tRect, wStyle)
{
    Type(TYPE_CDIAL);
    SetLimits(iRefAngle, lValuePerRev, lMinValue, lMaxValue);
}


/*--------------------------------------------------------------------------*/
PegCircularDial::PegCircularDial(SIGNED iLeft, SIGNED iTop, SIGNED iRefAngle,
                                 LONG lValuePerRev, LONG lMinValue,
                                 LONG lMaxValue, WORD wStyle) :
    PegDial(iLeft, iTop, wStyle)
{
    Type(TYPE_CDIAL);
    SetLimits(iRefAngle, lValuePerRev, lMinValue, lMaxValue);
}

/*--------------------------------------------------------------------------*/
PegCircularDial::~PegCircularDial()
{
}

/*--------------------------------------------------------------------------*/
void PegCircularDial::Draw()
{
    BeginDraw();

    if(mwStyle & AF_TRANSPARENT && Parent() && (!(mwStyle & FF_NONE)))
    {
        StandardBorder(Parent()->muColors[PCI_NORMAL]);
    }
    else if(!(mwStyle & FF_NONE))
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
void PegCircularDial::SetLimits(SIGNED iRefAngle, LONG lValuePerRev,
                                LONG lMinValue, LONG lMaxValue)
{
    miRefAngle = iRefAngle;
    mlValuePerRev = lValuePerRev;
    mlMinValue = lMinValue;
    mlMaxValue = lMaxValue;
    mlCurValue = lMinValue;

    CalcNeedlePos();
}

/*--------------------------------------------------------------------------*/
void PegCircularDial::CalcNeedlePos()
{
    mtNeedlePos.wRight = mClient.wLeft + (mClient.Width() >> 1);
    mtNeedlePos.wBottom = mClient.wTop + (mClient.Height() >> 1);

    LONG lCurVal = mlCurValue;

    while(lCurVal >= (mlMinValue + mlValuePerRev))
    {
        lCurVal -= mlValuePerRev;
    }

    miCurAngle = ValToAngle(lCurVal);

    WORD wRadius = ((PEGMIN((mClient.Width() >> 1), (mClient.Height() >> 1)))
    				* mwNeedleLen) / 100;

    SIGNED iSin, iCos;
    PegLookupSinCos(miCurAngle, &iSin, &iCos);
    mtNeedlePos.wLeft = mtNeedlePos.wRight + (WORD)((iCos * wRadius) >> 10);
    mtNeedlePos.wTop = mtNeedlePos.wBottom - (WORD)((iSin * wRadius) >> 10);
}

/*--------------------------------------------------------------------------*/
SIGNED PegCircularDial::ValToAngle(LONG lVal)
{
    SIGNED iTotalDeg = 360;
    LONG lTotalVal = mlValuePerRev;

    LONG lStep = (LONG)(iTotalDeg << 8) / (lTotalVal - 1);

    LONG lCurStep = (LONG)((lVal - mlMinValue) * lStep);
    SIGNED iAngle = miRefAngle - (SIGNED)(lCurStep >> 8);

    return(iAngle);
}

/*--------------------------------------------------------------------------*/
void PegCircularDial::DrawTicMarks()
{
    if(!mlTicFreq)
    {
        return;
    }

    PegPoint tPt1, tPt2;
    PegColor Color(PCLR_SHADOW, PCLR_SHADOW);

    CalcTicPos(mlMinValue, tPt1, tPt2);
    Line(tPt1.x, tPt1.y, tPt2.x, tPt2.y, Color, 3);

    for(LONG i = mlMinValue + mlTicFreq; i <= (mlMinValue + mlValuePerRev);
        i += mlTicFreq)
    {
        CalcTicPos(i, tPt1, tPt2);

        Line(tPt1.x, tPt1.y, tPt2.x, tPt2.y, Color);
    }
}

#endif  // PEG_HMI_GADGETS

