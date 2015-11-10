/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ppthing.cpp - Peg2DPolygon implementation file. This is an abstraction
//               up for having self drawing polygon things that can act a bit
//               like a sprite.
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

#include <string.h>     // for memcpy

#include "peg.hpp"

#if defined(PEG_FULL_GRAPHICS)

/*--------------------------------------------------------------------------*/
Peg2DPolygon::Peg2DPolygon(const PegRect& tRect, PegPoint* pPoints,
                           WORD wNumPoints, WORD wId /*=0*/,
                           WORD wStyle /*=FF_NONE*/) :
    PegThing(tRect, wId, wStyle),
    mwNumPoints(wNumPoints),
    mpPoints(NULL),
    mpRotPoints(NULL),
    mbFilled(FALSE),
    miTheta(0),
    miLineWidth(1)
{
    mbCopy = (BOOL)(wStyle & TT_COPY);

    if(mbCopy && wNumPoints && pPoints)
    {
        mpPoints = new PegPoint[mwNumPoints];
        memcpy(mpPoints, pPoints, (sizeof(PegPoint) * mwNumPoints));
    }
    else if(!mbCopy && wNumPoints && pPoints)
    {
        mpPoints = pPoints;
    }
    
    MapPolygonToReal();

    if(mpPoints)
    {
        mpRotPoints = new PegPoint[mwNumPoints];
        memcpy(mpRotPoints, mpPoints, (sizeof(PegPoint) * mwNumPoints));
    }
}

/*--------------------------------------------------------------------------*/
Peg2DPolygon::~Peg2DPolygon()
{
    if(mbCopy && mpPoints)
    {
        delete [] mpPoints;
    }
    
    if(mpRotPoints)
    {
        delete [] mpRotPoints;
    }
}

/*--------------------------------------------------------------------------*/
void Peg2DPolygon::Draw()
{
    BeginDraw();
    
    PegColor tColor;
    tColor.uForeground = muColors[PCI_NTEXT];
    if(Style() & AF_TRANSPARENT && Parent())
    {
        tColor.uBackground = Parent()->muColors[PCI_NORMAL];
    }
    else
    {
        tColor.uBackground = muColors[PCI_NORMAL];
    }
    
    if((Style() & AF_TRANSPARENT) != AF_TRANSPARENT ||
       (Style() & FF_MASK) != FF_NONE)
    {
        StandardBorder(tColor.uBackground);
    }
    
    if(mbFilled)
    {
        tColor.uFlags = CF_FILL;
        tColor.uBackground = muColors[PCI_STEXT];
    }
    else
    {
        tColor.uFlags = CF_NONE;
    }
    
    Screen()->Polygon(this, mpRotPoints, mwNumPoints, tColor, miLineWidth);
    
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void Peg2DPolygon::ParentShift(SIGNED iXOffset, SIGNED iYOffset)
{
    for(SIGNED i = 0; i < mwNumPoints; i++)
    {
        mpPoints[i].x += iXOffset;
        mpPoints[i].y += iYOffset;
        mpRotPoints[i].x += iXOffset;
        mpRotPoints[i].y += iYOffset;
    }
    
    PegThing::ParentShift(iXOffset, iYOffset);
}

/*--------------------------------------------------------------------------*/
void Peg2DPolygon::Resize(PegRect tRect)
{
    SIGNED iXShift = tRect.wLeft - mReal.wLeft;
    SIGNED iYShift = tRect.wTop - mReal.wTop;
    
    for(SIGNED i = 0; i < mwNumPoints; i++)
    {
        mpPoints[i].x += iXShift;
        mpPoints[i].y += iYShift;
        mpRotPoints[i].x += iXShift;
        mpRotPoints[i].y += iYShift;
    }

    PegThing::Resize(tRect);
}

/*--------------------------------------------------------------------------*/
void Peg2DPolygon::SetCurAngle(SIGNED iTheta)
{
    if(miTheta == iTheta)
    {
        return;
    }
    else
    {
        miTheta = iTheta;
    }

    SIGNED iCenterX = (SIGNED)(mReal.wLeft + (mReal.Width() >> 1));
    SIGNED iCenterY = (SIGNED)(mReal.wTop + (mReal.Height() >> 1));
    SIGNED iOrgX, iOrgY;

    SIGNED iSin, iCos;
    PegLookupSinCos(iTheta, &iSin, &iCos);
    
    for(SIGNED i = 0; i < mwNumPoints; i++)
    {
        iOrgX = mpPoints[i].x - iCenterX;
        iOrgY = mpPoints[i].y - iCenterY;
        mpRotPoints[i].x = (((iOrgX * iCos) >> 10) - 
                            ((iOrgY * iSin) >> 10) * -1) + iCenterX;
        mpRotPoints[i].y = (((iOrgY * iCos) >> 10) +
                            ((iOrgX * iSin) >> 10) * -1) + iCenterY;;
    }

    if (StatusIs(PSF_VISIBLE))
    {    
        Invalidate(mReal);
    }
}

/*--------------------------------------------------------------------------*/
void Peg2DPolygon::MapPolygonToReal()
{
    for(SIGNED i = 0; i < mwNumPoints; i++)
    {
        mpPoints[i].x += mReal.wLeft;
        mpPoints[i].y += mReal.wTop;
    }
}

#endif  // PEG_FULL_GRAPHICS

