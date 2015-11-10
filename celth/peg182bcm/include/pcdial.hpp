/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pcdial.hpp - Interface for the PegCircularDial class
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

#ifndef _PCDIAL_HPP_
#define _PCDIAL_HPP_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegCircularDial : public PegDial
{
    public:
        PegCircularDial(const PegRect& tRect, SIGNED iRefAngle,
                        LONG lValuePerRev, LONG lMinValue, LONG lMaxValue,
                        WORD wStyle = DS_STANDARDSTYLE);
        PegCircularDial(SIGNED iLeft, SIGNED iTop, SIGNED iRefAngle,
                        LONG lValuePerRev, LONG lMinValue, LONG lMaxValue,
                        WORD wStyle = DS_STANDARDSTYLE);
        virtual ~PegCircularDial();

        virtual void Draw();
        virtual void SetLimits(SIGNED iRefAngle, LONG lValuePerRev,
                               LONG lMinValue, LONG lMaxValue);

        SIGNED GetRefAngle() const { return miRefAngle; }
        LONG GetValuePerRev() const { return mlValuePerRev; }
        
    protected:
        virtual void CalcNeedlePos();
        virtual SIGNED ValToAngle(LONG lVal);
        virtual void DrawTicMarks();

        SIGNED miRefAngle;
        LONG mlValuePerRev;
};

#endif // _PCDIAL_HPP_


