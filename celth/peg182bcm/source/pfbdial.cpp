/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pfbdial.cpp - Implementation of the PegFiniteBitmapDial class
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
PegFiniteBitmapDial::PegFiniteBitmapDial(const PegRect& tRect,
                                         SIGNED iMinAngle, SIGNED iMaxAngle, 
                                         LONG lMinValue, LONG lMaxValue, 
                                         PegBitmap* pBkgBmp,
                                         PegBitmap* pAnchorBmp,
                                         WORD wStyle) :
    PegFiniteDial(tRect, iMinAngle, iMaxAngle, lMinValue, lMaxValue, wStyle),
	mpBkgBitmap(pBkgBmp),
	mpAnchorBitmap(pAnchorBmp),
	mpCompBitmap(NULL)
{
    Type(TYPE_FBM_DIAL);

    UseTrueCOR(TRUE);

    mtCOR.x = 0;
    mtCOR.y = 0;
}

/*--------------------------------------------------------------------------*/
PegFiniteBitmapDial::PegFiniteBitmapDial(SIGNED iLeft, SIGNED iTop,
                                         SIGNED iMinAngle, SIGNED iMaxAngle, 
                                         LONG lMinValue, LONG lMaxValue, 
                                         PegBitmap* pBkgBmp,
                                         PegBitmap* pAnchorBmp,
                                         WORD wStyle) :
    PegFiniteDial(iLeft, iTop, iMinAngle, iMaxAngle, lMinValue,
                  lMaxValue, wStyle),
	mpBkgBitmap(pBkgBmp),
	mpAnchorBitmap(pAnchorBmp),
	mpCompBitmap(NULL)
{
    Type(TYPE_FBM_DIAL);

    UseTrueCOR(TRUE);

    mtCOR.x = 0;
    mtCOR.y = 0;

    if(mpBkgBitmap)
    {
        mReal.wRight = mReal.wLeft + mpBkgBitmap->wWidth - 1;
        mReal.wBottom = mReal.wTop + mpBkgBitmap->wHeight - 1;
        InitClient();
    }

    CalcNeedlePos();
}

/*--------------------------------------------------------------------------*/
PegFiniteBitmapDial::~PegFiniteBitmapDial()
{
	if(mpCompBitmap)
	{
		Screen()->DestroyBitmap(mpCompBitmap);
	}
}

/*--------------------------------------------------------------------------*/
void PegFiniteBitmapDial::Draw()
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

        if(Style() & AF_TRANSPARENT)
        {
            if(Parent() && !(Style() & FF_MASK))
            {
                PegColor tColor(Parent()->GetColor(PCI_NORMAL),
                                Parent()->GetColor(PCI_NORMAL),
                                CF_FILL);
                Rectangle(mReal, tColor, 0);
            }
            else
            {
                StandardBorder(muColors[PCI_NORMAL]);
            }
        }

        DrawChildren();

        if(!mbValueSet)
        {
            CalcNeedlePos();
        }

        DrawNeedle();
        DrawAnchor();

        EndDraw();
	}
	
}


/*--------------------------------------------------------------------------*/
void PegFiniteBitmapDial::SetBkgBitmap(PegBitmap *pMap)
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

        mpCompBitmap->uFlags = 0;
        if(HAS_TRANS(mpBkgBitmap))
        {
            mpCompBitmap->uFlags = BMF_HAS_TRANS;
            mpCompBitmap->dTransColor = mpBkgBitmap->dTransColor;
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegFiniteBitmapDial::UseTrueCOR(BOOL bUse)
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
void PegFiniteBitmapDial::DrawToComposite()
{
    if(!mpCompBitmap)
    {
        mpCompBitmap = Screen()->CreateBitmap(mpBkgBitmap->wWidth,
                                              mpBkgBitmap->wHeight);
        mpCompBitmap->uFlags = 0;
        if(HAS_TRANS(mpBkgBitmap))
        {
            mpCompBitmap->uFlags = BMF_HAS_TRANS;
            mpCompBitmap->dTransColor = mpBkgBitmap->dTransColor;
        }
    }

    if(!mbValueSet)
    {
        CalcNeedlePos();
    }

    Screen()->BeginDraw(this, mpCompBitmap);

    PegPoint tPoint;
    tPoint.x = tPoint.y = 0;

    if(Style() & AF_TRANSPARENT || HAS_TRANS(mpBkgBitmap))
    {
        PegRect tBkgRect = mReal;
        tBkgRect.Shift(-(mReal.wLeft), -(mReal.wTop));
        if(Parent() && ((Style() & FF_MASK) == FF_NONE))
        {
            PegColor tColor(Parent()->GetColor(PCI_NORMAL),
                            Parent()->GetColor(PCI_NORMAL),
                            CF_FILL);
            Rectangle(tBkgRect, tColor, 0);
        }
        else
        {
            PegColor tColor(GetColor(PCI_NORMAL), GetColor(PCI_NORMAL),
                            CF_FILL);
            Rectangle(tBkgRect, tColor);
        }
    }

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
    
    // Shift the mtNeedlePos back to mReal coordinates
    mtNeedlePos.Shift(mReal.wLeft, mReal.wTop);
}

/*--------------------------------------------------------------------------*/
void PegFiniteBitmapDial::SetCOR(SIGNED CORX, SIGNED CORY, 
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
void PegFiniteBitmapDial::CalcNeedlePos()
{
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

		wRadius = ((PEGMAX(mClient.Width(), mClient.Height())) * mwNeedleLen) / 100;
	}

    SIGNED iSin, iCos;
    PegLookupSinCos(miCurAngle, &iSin, &iCos);
    mtNeedlePos.wLeft = mtNeedlePos.wRight + (WORD)((iCos * wRadius) >> 10);
    mtNeedlePos.wTop = mtNeedlePos.wBottom - (WORD)((iSin * wRadius) >> 10);
}

#endif	// PEG_HMI_GADGETS


