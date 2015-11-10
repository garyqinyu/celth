/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plinkbtn.hpp - A transparent text button that changes color after it
//      has been selected.
//
// Author: Kenneth G. Maxwell
//
// Version: 1.04
//
// Copyright (c) 1997-1998 Swell Software 
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

class ClockPrompt : public PegPrompt
{
    public:
        ClockPrompt::ClockPrompt(PegRect &Rect, WORD wStyle);
        SIGNED Message(const PegMessage &Mesg);

    private:
        void UpdateTimeDisplay(void);
        LONG mlOldTime;
};


