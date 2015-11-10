/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pstatbar.hpp - Status bar class definition.
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

#ifndef _PEGSTATUSBAR_
#define _PEGSTATUSBAR_


class PegStatusBar : public PegThing
{
    public:
        PegStatusBar();
        virtual ~PegStatusBar();
        virtual void Draw(void);
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void SetFont(PegFont *pFont);
        virtual PegPrompt *AddTextField(SIGNED iWidth, WORD wId, const PEGCHAR *Text = NULL); 
        virtual void SetTextField(WORD wId, PEGCHAR *Text);
        PegPrompt *GetPrompt(WORD wId);

    protected:
        virtual void SizeToParent(void);
        void CheckDrawDragMap(PegThing *Check = NULL);

        PegFont *mpFont;
        PegPrompt *mpLastPrompt;
        BOOL    mbShowPointer;
};


#endif

