/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pcbdial.hpp - Interface for the PegCircularBitmapDial class
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

#ifndef _PCBDIAL_HPP_
#define _PCBDIAL_HPP_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegCircularBitmapDial : public PegCircularDial
{
	public:
        PegCircularBitmapDial(const PegRect& Rect, SIGNED iRefAngle, 
							  LONG lValuePerRev, LONG lMinValue,
                              LONG lMaxValue, PegBitmap* pBkgBitmap,
                              PegBitmap* pAnchorBitmap = NULL,
                              WORD wStyle = DS_STANDARDSTYLE);
        PegCircularBitmapDial(SIGNED iLeft, SIGNED iTop, SIGNED iRefAngle, 
							  LONG lValuePerRev, LONG lMinValue,
                              LONG lMaxValue, PegBitmap* pBkgBitmap,
                              PegBitmap* pAnchorBitmap = NULL,
                              WORD wStyle = DS_STANDARDSTYLE);
        virtual ~PegCircularBitmapDial();

        // Overrides
        virtual void Draw();

        PegBitmap* GetBkgBitmap() const { return mpBkgBitmap; }
        void SetBkgBitmap(PegBitmap* pBmp) { mpBkgBitmap = pBmp; }
        PegBitmap* GetAnchorBitmap() const { return mpAnchorBitmap; }
        void SetAnchorBitmap(PegBitmap* pBmp) { mpAnchorBitmap = pBmp; }

        void UseTrueCOR(BOOL bUse);
        void SetCOR(SIGNED CORX, SIGNED CORY, BOOL bRedraw = FALSE);
        SIGNED GetCORX() const { return mtCOR.x; }
        SIGNED GetCORY() const { return mtCOR.y; }

    protected:
        virtual void CalcNeedlePos();
        void DrawToComposite();

        PegBitmap* mpBkgBitmap;
        PegBitmap* mpAnchorBitmap;
        PegBitmap* mpCompBitmap;

        PegPoint   mtCOR;
};

#endif // _PCBDIAL_HPP_

