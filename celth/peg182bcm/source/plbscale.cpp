/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plbscale.cpp - Implementation of the PegLinearBitmapScale class
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
PegLinearBitmapScale::PegLinearBitmapScale(const PegRect &Size,
                                           LONG lMinValue, LONG lMaxValue,
                                           SIGNED iMinTravelOffset,
                                           SIGNED iMaxTravelOffset,
                                           PegBitmap* pBkgBmp,
                                           PegBitmap* pNeedleBmp,
                                           WORD wStyle) :
    PegScale(Size, lMinValue, lMaxValue, wStyle|FF_NONE|SS_USERTRAVEL),
    mpBkgBitmap(pBkgBmp),
    mpNeedleBitmap(pNeedleBmp),
    mpCompBitmap(NULL),
    miNeedleOffset(0),
    miMinTravelOffset(iMinTravelOffset),
    miMaxTravelOffset(iMaxTravelOffset)
{
    Type(TYPE_LIN_BM_SCALE);
}

/*--------------------------------------------------------------------------*/
PegLinearBitmapScale::PegLinearBitmapScale(SIGNED iLeft, SIGNED iTop,
                                           LONG lMinValue, LONG lMaxValue,
                                           SIGNED iMinTravelOffset,
                                           SIGNED iMaxTravelOffset,
                                           PegBitmap* pBkgBmp,
                                           PegBitmap* pNeedleBmp,
                                           WORD wStyle) :
    PegScale(iLeft, iTop, lMinValue, lMaxValue, wStyle|FF_NONE|SS_USERTRAVEL),
    mpBkgBitmap(pBkgBmp),
    mpNeedleBitmap(pNeedleBmp),
    mpCompBitmap(NULL),
    miNeedleOffset(0),
    miMinTravelOffset(iMinTravelOffset),
    miMaxTravelOffset(iMaxTravelOffset)
{
    Type(TYPE_LIN_BM_SCALE);
    
    if(mpBkgBitmap)
    {
        mReal.wRight = mReal.wLeft + mpBkgBitmap->wWidth - 1;
        mReal.wBottom = mReal.wTop + mpBkgBitmap->wHeight - 1;
        InitClient();
    }
}

/*--------------------------------------------------------------------------*/
PegLinearBitmapScale::~PegLinearBitmapScale()
{
	if(mpCompBitmap)
	{
		Screen()->DestroyBitmap(mpCompBitmap);
	}
}

/*--------------------------------------------------------------------------*/
void PegLinearBitmapScale::Draw()
{
    if(!mpCompBitmap)
    {
        mpCompBitmap = Screen()->CreateBitmap(mpBkgBitmap->wWidth,
                                              mpBkgBitmap->wHeight);
    }

    DrawToComposite();

    BeginDraw();

    PegPoint tPoint;
    tPoint.x = mReal.wLeft;
    tPoint.y = mReal.wTop;

    Bitmap(tPoint, mpCompBitmap);

    if(!mpNeedleBitmap)
    {
        DrawNeedle();
    }

    DrawChildren();

    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegLinearBitmapScale::SetTravelPoints()
{
    if(mwStyle & SS_USERTRAVEL)
    {
        if(mwStyle & SS_ORIENTVERT)
        {
            miMinTravel = mClient.wTop + miMinTravelOffset;
            miMaxTravel = mClient.wTop + miMaxTravelOffset;
        }
        else
        {
            miMinTravel = mClient.wLeft + miMinTravelOffset;
            miMaxTravel = mClient.wLeft + miMaxTravelOffset;
        }
    }
    else
    {
        PegScale::SetTravelPoints();
    }
}

void PegLinearBitmapScale::SetBkgBitmap(PegBitmap *pMap)
{
    mpBkgBitmap = pMap;

    if (mpCompBitmap)
    {
        Screen()->DestroyBitmap(mpCompBitmap);
        mpCompBitmap = NULL;
    }

    if(mpBkgBitmap)
    {
        mpCompBitmap = Screen()->CreateBitmap(mpBkgBitmap->wWidth,
                                              mpBkgBitmap->wHeight);
        if (pMap->wWidth != mReal.Width() ||
            pMap->wHeight != mReal.Height())
        {
            PegRect Size = mReal;
            Size.wRight = Size.wLeft + pMap->wWidth - 1;
            Size.wBottom = Size.wTop + pMap->wHeight - 1;
            Resize(Size);
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegLinearBitmapScale::CalcNeedleRect(const PegPoint& tPoint,
                                          PegRect& tRect)
{
    if(mpNeedleBitmap == NULL)
    {
        PegScale::CalcNeedleRect(tPoint, tRect);
    }
    else
    {
        if(mwStyle & SS_ORIENTVERT)
        {
            if(mwStyle & SS_FACELEFT)
            {
                tRect.wLeft = mClient.wRight - mpNeedleBitmap->wWidth + 1;
                tRect.wTop = tPoint.y - (mpNeedleBitmap->wHeight >> 1) -
                             miNeedleOffset;
                tRect.wRight = mClient.wRight;
                tRect.wBottom = tRect.wTop + mpNeedleBitmap->wHeight - 1;
            }
            else
            {
                tRect.wLeft = mClient.wLeft;
                tRect.wTop = tPoint.y - (mpNeedleBitmap->wHeight >> 1) -
                             miNeedleOffset;
                tRect.wRight = mClient.wLeft + mpNeedleBitmap->wWidth - 1;
                tRect.wBottom = tRect.wTop + mpNeedleBitmap->wHeight - 1;
            }
        }
        else // horizontal
        {
            if(mwStyle & SS_FACETOP)
            {
                tRect.wLeft = tPoint.x - (mpNeedleBitmap->wWidth >> 1) -
                              miNeedleOffset;
                tRect.wTop = mClient.wBottom - mpNeedleBitmap->wHeight + 1;
                tRect.wRight = tRect.wLeft + mpNeedleBitmap->wWidth - 1;
                tRect.wBottom = mClient.wBottom;
            }
            else
            {
                tRect.wLeft = tPoint.x - (mpNeedleBitmap->wWidth >> 1) -
                              miNeedleOffset;
                tRect.wTop = mClient.wTop;
                tRect.wRight = tRect.wLeft + mpNeedleBitmap->wWidth - 1;
                tRect.wBottom = tRect.wTop + mpNeedleBitmap->wHeight - 1;
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegLinearBitmapScale::DrawToComposite()
{
    Screen()->BeginDraw(this, mpCompBitmap);

    PegPoint tPoint;
    tPoint.x = tPoint.y = 0;

    Bitmap(tPoint, mpBkgBitmap);

    if(mpNeedleBitmap)
    {
        DrawBitmapNeedle();
    }

    Screen()->EndDraw(mpCompBitmap);
}

/*--------------------------------------------------------------------------*/
void PegLinearBitmapScale::DrawBitmapNeedle()
{
    PegPoint tDrawPoint;
    tDrawPoint.x = mtNeedlePoint.x;
    tDrawPoint.y = mtNeedlePoint.y;

    if(mwStyle & SS_ORIENTVERT)
    {
        if(mwStyle & SS_FACELEFT)
        {
            tDrawPoint.x = mClient.wRight - mpNeedleBitmap->wWidth + 1;
        }
        else
        {
            tDrawPoint.x = mClient.wLeft + mpNeedleBitmap->wWidth - 1;
        }

        tDrawPoint.y -= (mpNeedleBitmap->wHeight >> 1) + miNeedleOffset;
    }
    else
    {
        if(mwStyle & SS_FACETOP)
        {
            tDrawPoint.y = mClient.wBottom - mpNeedleBitmap->wHeight + 1;
        }
        else
        {
            tDrawPoint.y = mClient.wTop + mpNeedleBitmap->wHeight - 1;
        }

        tDrawPoint.x -= (mpNeedleBitmap->wWidth >> 1) + miNeedleOffset;
    }
    
    tDrawPoint.x += (-(mReal.wLeft));
    tDrawPoint.y += (-(mReal.wTop));

    Bitmap(tDrawPoint, mpNeedleBitmap);
}

#endif	// PEG_HMI_GADGETS


