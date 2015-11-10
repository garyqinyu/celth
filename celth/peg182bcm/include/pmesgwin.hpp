/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pmesgwin.hpp - Popup message window class definition.
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

#ifndef _PEGMESGWIN_
#define _PEGMESGWIN_

/*--------------------------------------------------------------------------*/

class PegMessageWindow : public PegWindow, public PegTextThing
{
    public:

        PegMessageWindow(const PegRect &Rect, const PEGCHAR *Title, 
            const PEGCHAR *Message = NULL, WORD wStyle = MW_OK|FF_RAISED,
            PegBitmap *pIcon = NULL, PegThing *Owner = NULL);
            
        PegMessageWindow(const PEGCHAR *Title, const PEGCHAR *Message = NULL,
            WORD wStyle = MW_OK|FF_RAISED, PegBitmap *pIcon = NULL,
            PegThing *Owner = NULL);

        virtual ~PegMessageWindow();

        virtual void Draw(void);
        virtual SIGNED Message(const PegMessage &Mesg);


    protected:
        void CalcButtonWidth(void);
        void AddButtons(void);

        PegTitle *mpTitle;
        PegThing *mpOwner;
        PegBitmap *mpIcon;
        SIGNED miButtonWidth;
        SIGNED miOneButtonWidth;
        SIGNED miMessageWidth;
};

#endif

