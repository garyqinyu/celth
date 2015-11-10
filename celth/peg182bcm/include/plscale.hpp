/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plscale.hpp - Interface for the PegLinearScale class
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

#ifndef _PLSCALE_HPP_
#define _PLSCALE_HPP_

class PegLinearScale : public PegScale
{
    public:
        PegLinearScale(const PegRect &Size, LONG lMinValue, LONG lMaxValue,
                       LONG lTicFreq = 0, 
                       WORD wStyle = SS_STANDARDSTYLE | FF_RAISED);
        PegLinearScale(SIGNED iLeft, SIGNED iTop, LONG lMinValue,
                       LONG lMaxValue, LONG lTicFreq = 0,
                       WORD wStyle = SS_STANDARDSTYLE | FF_RAISED);
        ~PegLinearScale();

        virtual void Draw();

        LONG GetTicFreq() const { return mlTicFreq; }
        void SetTicFreq(LONG lFreq) { mlTicFreq = lFreq; }

    protected:
        virtual void DrawScale();
        virtual void DrawTicMarks();

        LONG mlTicFreq;
};

#endif // _PLSCALE_HPP_

