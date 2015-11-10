/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ptoolbar.hpp - Definition of PegToolBar and PegToolBarPanel classes
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

#ifndef _PEGTOOLBAR_
#define _PEGTOOLBAR_

#define TOOLBAR_PAD_HEIGHT       2
//#define TOOLBAR_PANEL_HEIGHT    22

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegToolBarPanel : public PegThing
{
    public:
        PegToolBarPanel(WORD wID = 0);
        virtual ~PegToolBarPanel();

        //virtual SIGNED Message(const PegMessage& Mesg);
        virtual void Draw();
        virtual void Add(PegThing *Who, BOOL bDraw = TRUE);
        virtual void AddToEnd(PegThing *Who, BOOL bDraw = TRUE);
        virtual PegThing *Remove(PegThing *Who, BOOL bDraw = TRUE);

    protected:
        virtual void PositionChildren();
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegToolBar : public PegThing
{
    public:
        PegToolBar(WORD wID = 0);
        virtual ~PegToolBar();

        virtual SIGNED Message(const PegMessage& Mesg);
        virtual void Draw();

        virtual void AddPanel(PegToolBarPanel* pPanel, BOOL bDraw = TRUE);
        virtual PegThing* RemovePanel(PegThing *Who, BOOL bDraw = TRUE);
        virtual void PositionPanels();

    protected:
        virtual void SizeToParent();
        virtual void SizeToTallestPanel();
        
        SIGNED  miPanelHeight;
};

#endif // _PEGTOOLBAR_

