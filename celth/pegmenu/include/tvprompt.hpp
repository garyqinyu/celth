/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// tvprompt.hpp - prompt class definition.
//
// Author: ST ShenZhen MMX Lab
//
// Copyright (c) 
//              All Rights Reserved.
//
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _TVPROMPT_HPP
#define _TVPROMPT_HPP

class TVPrompt : public PegPrompt
{
    public:        
        TVPrompt(const PegRect &Rect, const PEGCHAR *Text, WORD wId = 0,
            WORD wStyle = FF_THIN|TJ_LEFT|AF_TRANSPARENT,
            PegFont *pFont = NULL);

        SIGNED Message(const PegMessage &Mesg);

        virtual ~TVPrompt();
        virtual void Draw(void);
};


#endif /*end _TVPROMPT_HPP*/
