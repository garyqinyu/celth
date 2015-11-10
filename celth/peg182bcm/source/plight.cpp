/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plight.cpp - Implementation for the PegLight class
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
PegLight::PegLight(const PegRect &Size, WORD wNumStates, WORD wStyle) :
    PegThing(Size, 0, wStyle)
{
    mwNumStates = wNumStates;
    mwCurState = 0;
    InitClient();
}

/*--------------------------------------------------------------------------*/
PegLight::PegLight(SIGNED iLeft, SIGNED iTop, WORD wNumStates, WORD wStyle) :
    PegThing(0, wStyle)
{
    mwNumStates = wNumStates;
    mwCurState = 0;

    mReal.Set(iLeft, iTop, iLeft + 100, iTop + 100);
    InitClient();
}

/*--------------------------------------------------------------------------*/
PegLight::~PegLight()
{
}


/*--------------------------------------------------------------------------*/
WORD PegLight::IncrementState(BOOL bRedraw /*=TRUE*/)
{
    if(mwCurState == mwNumStates - 1)
    {
        mwCurState = 0;
    }
    else
    {
        mwCurState++;
    }

    if(StatusIs(PSF_VISIBLE) && bRedraw)
    {
        Invalidate();
        Draw();
    }

    return(mwCurState);
}

/*--------------------------------------------------------------------------*/
WORD PegLight::DecrementState(BOOL bRedraw /*=TRUE*/)
{
    if(mwCurState == 0)
    {
        mwCurState = mwNumStates - 1;
    }
    else
    {
        mwCurState--;
    }

    if(StatusIs(PSF_VISIBLE) && bRedraw)
    {
        Invalidate();
        Draw();
    }

    return(mwCurState);
}

/*--------------------------------------------------------------------------*/
WORD PegLight::SetState(WORD wState, BOOL bRedraw /*=TRUE*/)
{
    if(wState < mwNumStates)
    {
        mwCurState = wState;
    }

    if(StatusIs(PSF_VISIBLE) && bRedraw)
    {
        Invalidate();
        Draw();
    }
    
    return(mwCurState);
}

/*--------------------------------------------------------------------------*/
WORD PegLight::SetNumStates(WORD wNumStates)
{
    mwNumStates = wNumStates;

    if(mwCurState >= mwNumStates)
    {
        mwCurState = mwNumStates - 1;
    }
    
    return(mwNumStates);
}

#endif	// PEG_HMI_GADGETS


