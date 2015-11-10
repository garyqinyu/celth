/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plight.hpp - Base for the PegLight classes
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

#ifndef _PEGLIGHT_HPP_
#define _PEGLIGHT_HPP_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegLight : public PegThing
{
    public:
        PegLight(const PegRect &Size, WORD wNumStates, WORD wStyle);
        PegLight(SIGNED iLeft, SIGNED iTop, WORD wNumStates,
                 WORD wStyle);
        virtual ~PegLight();

        virtual WORD IncrementState(BOOL bRedraw = TRUE);
        virtual WORD DecrementState(BOOL bRedraw = TRUE);        
        virtual WORD SetState(WORD wState, BOOL bRedraw = TRUE);
        
        WORD GetCurrentState() const { return mwCurState; }
        WORD GetNumStates() const { return mwNumStates; }
        virtual WORD SetNumStates(WORD wNumStates);

    protected:
        WORD mwNumStates;
        WORD mwCurState;
};

#endif // _PEGLIGHT_HPP_


