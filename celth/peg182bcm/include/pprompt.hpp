/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pprompt.hpp - simple prompt class definition.
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

#ifndef _PEGPROMPT_
#define _PEGPROMPT_

class PegPrompt : public PegThing, public PegTextThing
{
    public:
        PegPrompt(const PegRect &Rect, const PEGCHAR *Text, WORD wId = 0,
            WORD wStyle = FF_NONE|TJ_LEFT|AF_TRANSPARENT);

        PegPrompt(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, 
            const PEGCHAR *Text, WORD wId = 0,
            WORD wStyle = FF_NONE|TJ_LEFT|AF_TRANSPARENT,
            PegFont *pFont = NULL);
        
        PegPrompt(SIGNED iLeft, SIGNED iTop, const PEGCHAR *Text, WORD wId = 0,
            WORD wStyle = FF_NONE|TJ_LEFT|AF_TRANSPARENT,
            PegFont *pFont = NULL);

        SIGNED Message(const PegMessage &Mesg);

        virtual ~PegPrompt();
        virtual void Draw(void);
        virtual void DataSet(const PEGCHAR *Text);
        virtual void SetFont(PegFont *Font);
};


#endif

