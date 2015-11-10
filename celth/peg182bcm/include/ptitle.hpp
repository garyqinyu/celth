/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ptitle.hpp - PegTitle class definition.
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

#ifndef _PEGTITLE_
#define _PEGTITLE_

//#define SYS_BUTTON_SIZE 16                // for fixed size
#define SYS_BUTTON_SIZE PEG_SCROLL_WIDTH    // for adjustable size

class PegTitle : public PegThing, public PegTextThing
{
    public:
        PegTitle(const PEGCHAR *Title, WORD wStyle =
            TF_SYSBUTTON|TF_MINMAXBUTTON|TF_CLOSEBUTTON);
        virtual ~PegTitle();
        virtual void Draw(void);
        virtual SIGNED Message(const PegMessage &Mesg);
        void AssignMenu(PegMenuDescription *pMenu);


        enum TitleButtons {
            IDB_MAX = 1008,
            IDB_MIN,
            IDB_RESTORE
        };

    protected:
            
        virtual void SizeToParent(void);
        PegBitmapButton *mpSysButton;
        PegBitmapButton *mpCloseButton;
        PegBitmapButton *mpMinButton;
        PegBitmapButton *mpMaxButton;
        PegMenu *mpMenu;
        SIGNED miHeight;
};


#endif

