/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pfdial.hpp - Interface for the PegFiniteDial class
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

#ifndef _PFDIAL_HPP_
#define _PFDIAL_HPP_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegFiniteDial : public PegDial
{
    public:
        PegFiniteDial(const PegRect &Size, SIGNED iMinAngle, 
            SIGNED iMaxAngle, LONG lMinValue, LONG lMaxValue,
            WORD wStyle = DS_STANDARDSTYLE);

        PegFiniteDial(SIGNED iLeft, SIGNED iTop, SIGNED iMinAngle,
            SIGNED iMaxAngle, LONG lMinValue, LONG lMaxValue,
            WORD wStyle = DS_STANDARDSTYLE);

        virtual ~PegFiniteDial();

        virtual void Draw();
        virtual void SetLimits(SIGNED iMinAngle, SIGNED iMaxAngle,
                               LONG lMinValue, LONG lMaxValue);
    protected:
        virtual void CalcNeedlePos();
        virtual SIGNED ValToAngle(LONG lVal);
        virtual void DrawTicMarks();
};

#endif // _PFDIAL_HPP_


