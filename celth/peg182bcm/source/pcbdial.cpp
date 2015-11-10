/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pcbdial.cpp - Implementation of the PegCircularBitmapDial class
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
PegCircularBitmapDial::PegCircularBitmapDial(const PegRect& tRect,
                                             SIGNED iRefAngle,
                                             LONG lValuePerRev,
                                             LONG lMinValue, 
                                             LONG lMaxValue, 
                                             PegBitmap* pBkgBitmap,
                                             PegBitmap* pAnchorBitmap,
                                             WORD wStyle) :
    PegCircularDial(tRect, iRefAngle, lValuePerRev, lMinValue, lMaxValue,
                    wStyle),
    mpBkgBitmap(pBkgBitmap),
    mpAnchorBitmap(pAnchorBitmap),
    mpCompBitmap(NULL)
{
    Type(TYPE_CBM_DIAL);
    UseTrueCOR(TRUE);
    mtCOR.x = 0;
    mtCOR.y = 0;
}


/*--------------------------------------------------------------------------*/
PegCircularBitmapDial::PegCircularBitmapDial(SIGNED iLeft, SIGNED iTop,
                                             SIGNED iRefAngle,
                                             LONG lValuePerRev,
                                             LONG lMinValue, 
                                             LONG lMaxValue, 
                                             PegBitmap* pBkgBitmap,
                                             PegBitmap* pAnchorBitmap,
                                             WORD wStyle) :
    PegCircularDial(iLeft, iTop, iRefAngle, lValuePerRev, lMinValue, lMaxValue,
                    wStyle),
    mpBkgBitmap(pBkgBitmap),
    mpAnchorBitmap(pAnchorBitmap),
    mpCompBitmap(NULL)
{
    Type(TYPE_CBM_DIAL);
    UseTrueCOR(TRUE);
    mtCOR.x = 0;
    mtCOR.y = 0;

    if(mpBkgBitmap)
    {
        mReal.wRight = mReal.wLeft + mpBkgBitmap->wWidth - 1;
        mReal.wBottom = mReal.wTop + mpBkgBitmap->wHeight - 1;
    }

    CalcNeedlePos();
}

/*--------------------------------------------------------------------------*/
PegCircularBitmapDial::~PegCircularBitmapDial()
{
    if(mpCompBitmap)
    {
        Screen()->DestroyBitmap(mpCompBitmap);
    }
}

/*--------------------------------------------------------------------------*/
void PegCircularBitmapDial::Draw()
{
	if(mpBkgBitmap)
	{
	    DrawToComposite();

		BeginDraw();

		PegPoint tPoint;
		tPoint.x = mReal.wLeft;
		tPoint.y = mReal.wTop;
		Bitmap(tPoint, mpCompBitmap);
        
        DrawChildren();       
        
        EndDraw();
	}
	else
	{
		BeginDraw();

        DrawChildren();

        if(!mbValueSet)
        {
            CalcNeedlePos();
        }

        DrawNeedle();

        EndDraw();
	}
	
}

/*--------------------------------------------------------------------------*/
void PegCircularBitmapDial::UseTrueCOR(BOOL bUse)
{
    if(bUse)
    {
        mwStyle &= ~DS_USERCOR;
        mwStyle |= DS_RECTCOR;
    }
    else
    {
        mwStyle &= ~DS_RECTCOR;
        mwStyle |= DS_USERCOR;
    }
}

/*--------------------------------------------------------------------------*/
void PegCircularBitmapDial::DrawToComposite()
{
    if(!mpCompBitmap)
    {
        mpCompBitmap = Screen()->CreateBitmap(mpBkgBitmap->wWidth,
                                              mpBkgBitmap->wHeight);
    }

    if(!mbValueSet)
    {
        CalcNeedlePos();
    }

    PegRect tOldClip = mClip;
    mClip.Shift(-(mReal.wLeft), -(mReal.wTop));

    Screen()->BeginDraw(this, mpCompBitmap);

    PegPoint tPoint;
    tPoint.x = tPoint.y = 0;

    Bitmap(tPoint, mpBkgBitmap);
        
    // Shift to draw into the coordinates of the offscreen bitmap
    mtNeedlePos.Shift(-(mReal.wLeft), -(mReal.wTop));

    DrawNeedle();

    if(mpAnchorBitmap)
    {
        if(mwStyle & DS_RECTCOR)
        {
            tPoint.x = mtNeedlePos.wRight - (mpAnchorBitmap->wWidth >> 1);
            tPoint.y = mtNeedlePos.wBottom - (mpAnchorBitmap->wHeight >> 1);
            Bitmap(tPoint, mpAnchorBitmap);
        }
    }
    else
    {
        DrawAnchor();
    }
    
    Screen()->EndDraw(mpCompBitmap);
    
    mClip = tOldClip;

    // Shift the mtNeedlePos back to mReal coordinates
    mtNeedlePos.Shift(mReal.wLeft, mReal.wTop);
}

/*--------------------------------------------------------------------------*/
void PegCircularBitmapDial::SetCOR(SIGNED CORX, SIGNED CORY, 
                                 BOOL bRedraw /*=FALSE*/)
{
    mtCOR.x = CORX;
    mtCOR.y = CORY;

    if(StatusIs(PSF_VISIBLE) && bRedraw)
    {
        Invalidate();
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
void PegCircularBitmapDial::CalcNeedlePos()
{
    LONG lCurVal = mlCurValue;

    while(lCurVal >= (mlMinValue + mlValuePerRev))
    {
        lCurVal -= mlValuePerRev;
    }

    miCurAngle = ValToAngle(lCurVal);

    WORD wRadius;

	if(mwStyle & DS_RECTCOR)
	{
		mtNeedlePos.wRight = mClient.wLeft + (mClient.Width() >> 1);
		mtNeedlePos.wBottom = mClient.wTop + (mClient.Height() >> 1);

		wRadius = ((PEGMIN((mClient.Width() >> 1), (mClient.Height() >> 1)))
                  * mwNeedleLen) / 100;
	}
	else
	{
		mtNeedlePos.wRight = mClient.wLeft + mtCOR.x;
		mtNeedlePos.wBottom = mClient.wTop + mtCOR.y;

		wRadius = ((PEGMAX(mClient.Width(), mClient.Height())) 
                  * mwNeedleLen) / 100;
	}

    SIGNED iSin, iCos;
    PegLookupSinCos(miCurAngle, &iSin, &iCos);
    mtNeedlePos.wLeft = mtNeedlePos.wRight + (WORD)((iCos * wRadius) >> 10);
    mtNeedlePos.wTop = mtNeedlePos.wBottom - (WORD)((iSin * wRadius) >> 10);
}

#endif	// PEG_HMI_GADGETS


