/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pvprompt.hpp - Vertical PegPrompt class definition.
//
// Author: Kenneth G. Maxwell
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

#ifndef _PEGVPROMPT_
#define _PEGVPROMPT_

class PegVPrompt : public PegPrompt
{
    public:
        PegVPrompt(const PegRect &Rect, const PEGCHAR *Text, WORD wId = 0,
            WORD wStyle = FF_NONE|AF_TRANSPARENT);
        virtual void Draw(void);
};


#endif

