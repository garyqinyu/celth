/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// p2dpoly.hpp - Peg2DPolygon definition file. This is an abstraction up
//               for having self drawing polygon things that can act a bit
//               like a sprite.
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-2001 Swell Software 
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

#ifndef _PEG2DPOLYGON_
#define _PEG2DPOLYGON_

class Peg2DPolygon : public PegThing
{
    public:
        Peg2DPolygon(const PegRect& tRect, PegPoint* pPoints,
                     WORD wNumPoints, WORD wId = 0, WORD wStyle = FF_NONE);
        virtual ~Peg2DPolygon();

        // Overrides
        void Draw();
        void ParentShift(SIGNED iXOffset, SIGNED iYOffset);
        void Resize(PegRect tRect);

        // Accessors
        WORD GetNumPoints() const { return mwNumPoints; }
        BOOL GetFill() const { return mbFilled; }
        SIGNED GetCurAngle() const { return miTheta; }
        SIGNED GetLineWidth() const { return miLineWidth; }
        
        // Mutators
        void SetFill(BOOL bFill) { mbFilled = bFill; }
        void SetCurAngle(SIGNED iTheta);
        void SetLineWidth(SIGNED iLineWidth) { miLineWidth = iLineWidth; }

    protected:
        void MapPolygonToReal();

        WORD        mwNumPoints;
        PegPoint*   mpPoints;
        PegPoint*   mpRotPoints;
        BOOL        mbFilled;
        SIGNED      miTheta;
        BOOL        mbCopy;
        SIGNED      miLineWidth;
};

#endif // _PEG2DPOLYGON_

