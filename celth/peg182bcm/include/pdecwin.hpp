/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pdecwin.hpp - Decorated window class.
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

#ifndef _PEGDECWIN_
#define _PEGDECWIN_

/*--------------------------------------------------------------------------*/
// window style flags

class PegDecoratedWindow : public PegWindow
{
    public:
        PegDecoratedWindow(const PegRect &Rect, WORD wStyle = FF_THICK);
        PegDecoratedWindow(WORD wStyle = FF_THICK);
        virtual ~PegDecoratedWindow();
        virtual void Add(PegThing *What, BOOL bDraw = TRUE);
        virtual PegThing *Remove(PegThing *Who, BOOL bDraw = TRUE);
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void InitClient(void);
        PegStatusBar* StatusBar(void) { return mpStatusBar; }
        PegMenuBar*   MenuBar(void) { return mpMenuBar; }
        PegToolBar*   ToolBar(void) { return mpToolBar; }

        const PEGCHAR *Title(void)
        {
            if (mpTitle)
            {
                return (mpTitle->DataGet());
            }
            return NULL;
        }

        PegTitle *TitleObject(void)
        {
            return mpTitle;
        }

        void SetTitle(PEGCHAR *Text)
        {
            if (mpTitle)
            {
                mpTitle->DataSet(Text);
            }
        }

    protected:
        PegTitle*      mpTitle;
        PegMenuBar*    mpMenuBar;
        PegStatusBar*  mpStatusBar;
        PegToolBar*    mpToolBar;
};


#endif

