/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pfbdial.hpp - Interface for the PegFiniteBitmapDial class
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

#ifndef _PFBDIAL_HPP_
#define _PFBDIAL_HPP_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegFiniteBitmapDial : public PegFiniteDial
{
    public:
        PegFiniteBitmapDial(SIGNED iLeft, SIGNED iTop, SIGNED iMinAngle,
            SIGNED iMaxAngle, LONG lMinValue, LONG lMaxValue,
            PegBitmap* pBkgBmp, PegBitmap* pAnchorBitmap = NULL,
            WORD wStyle = DS_STANDARDSTYLE);

        PegFiniteBitmapDial(const PegRect& tRect, SIGNED iMinAngle,
            SIGNED iMaxAngle, LONG lMinValue, LONG lMaxValue,
            PegBitmap* pBkgBmp, PegBitmap* pAnchorBitmap = NULL,
            WORD wStyle = DS_STANDARDSTYLE);

        virtual ~PegFiniteBitmapDial();

        virtual void Draw();

        PegBitmap* GetBkgBitmap() const { return mpBkgBitmap; }
        void SetBkgBitmap(PegBitmap* pBmp);
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

#endif // _PFBDIAL_HPP_


