/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pmlmsgwin.hpp - Multiline popup message window class definition.
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-2002 Swell Software 
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

#ifndef _PEGMLMSGWIN_
#define _PEGMLMSGWIN_

/*--------------------------------------------------------------------------*/

class PegMLMessageWindow : public PegWindow
{
    public:

        PegMLMessageWindow(const PegRect &Rect, const PEGCHAR *Title, 
            const PEGCHAR *Message = NULL, WORD wStyle = MW_OK|FF_RAISED,
            PegBitmap *pIcon = NULL, PegThing *Owner = NULL);
            
        PegMLMessageWindow(const PEGCHAR *Title, const PEGCHAR *Message = NULL,
            WORD wStyle = MW_OK|FF_RAISED, PegBitmap *pIcon = NULL,
            PegThing *Owner = NULL);

        virtual ~PegMLMessageWindow();

        virtual void Draw();
        virtual SIGNED Message(const PegMessage& Mesg);

    protected:
        void CalcButtonWidth();
        void CalcButtonTop();
        void AddButtons();

        PegTitle *mpTitle;
        PegThing *mpOwner;
        PegBitmap *mpIcon;
        PegTextBox* mpTextBox;
        SIGNED miButtonWidth;
        SIGNED miButtonTop;
        SIGNED miOneButtonWidth;
};

#endif  // _PEGMLMSGWIN_


