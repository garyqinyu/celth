/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pblight.hpp - Interface for the PegBitmapLight class
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

#ifndef _PBLIGHT_HPP_
#define _PBLIGHT_HPP_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegBitmapLight : public PegLight
{
    public:
        PegBitmapLight(const PegRect &Size, WORD wNumStates);
        PegBitmapLight(SIGNED iLeft, SIGNED iTop, WORD wNumStates);
        virtual ~PegBitmapLight();

        // Overrides
        virtual void Draw();
        virtual WORD SetNumStates(WORD wNumStates);

        BOOL SetStateBitmap(WORD wState, PegBitmap* pBmp);

    protected:
        PegBitmap** mpStateBitmap;
};

#endif // _PBLIGHT_HPP_

