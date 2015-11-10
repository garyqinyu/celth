/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pclight.hpp - Interface for the PegColorLight class
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

#ifndef _PCLIGHT_HPP_
#define _PCLIGHT_HPP_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegColorLight : public PegLight
{
    public:
        PegColorLight(const PegRect& Rect, WORD wNumStates,
                      WORD wStyle = FF_RAISED|LS_RECTANGLE);
        PegColorLight(SIGNED iLeft, SIGNED iTop, WORD wNumStates,
                      WORD wStyle = FF_RAISED|LS_RECTANGLE);
        virtual ~PegColorLight();

        // Overrides
        virtual void Draw();
        virtual WORD SetNumStates(WORD wNumStates);

        BOOL SetStateColor(WORD wState, COLORVAL tColor);

    protected:
        COLORVAL* mpStateColor;
};

#endif // _PCLIGHT_HPP_


