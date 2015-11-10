/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ptoolbar.cpp - Implementation of PegToolBar and PegToolBarPanel classes
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-2001 Swell Software 
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

#include "peg.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegToolBarPanel::PegToolBarPanel(WORD wID /*=0*/) : PegThing(wID)
{
    Type(TYPE_TOOL_BAR_PANEL);
    mReal.Set(0, 0, 100, 22 + TOOLBAR_PAD_HEIGHT);
    mClient.Set(0, 0, 100, 22 + TOOLBAR_PAD_HEIGHT);
    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
}

/*--------------------------------------------------------------------------*/
PegToolBarPanel::~PegToolBarPanel()
{
}


/*--------------------------------------------------------------------------*/
void PegToolBarPanel::Draw()
{
    BeginDraw();

    PegColor Color(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    Rectangle(mReal, Color, 1);
    
    Color.uForeground = PCLR_HIGHLIGHT;
    Line(mReal.wLeft, mReal.wTop, mReal.wRight, mReal.wTop, Color);
    Line(mReal.wLeft, mReal.wTop, mReal.wLeft, mReal.wBottom - 1, Color);
    Line(mReal.wLeft + 3, mReal.wTop + 2, mReal.wLeft + 3, mReal.wBottom - 3, 
         Color);
    Line(mReal.wLeft + 6, mReal.wTop + 2, mReal.wLeft + 6, mReal.wBottom - 3, 
         Color);

    Color.uForeground = PCLR_LOWLIGHT;
    Line(mReal.wLeft, mReal.wBottom, mReal.wRight, mReal.wBottom, Color);
    Line(mReal.wRight, mReal.wBottom, mReal.wRight, mReal.wTop + 1, Color);
    Line(mReal.wLeft + 4, mReal.wTop + 3, mReal.wLeft + 4, mReal.wBottom - 2, 
         Color);
    Line(mReal.wLeft + 7, mReal.wTop + 3, mReal.wLeft + 7, mReal.wBottom - 2, 
         Color);

    DrawChildren();

    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegToolBarPanel::Add(PegThing *Who, BOOL bDraw /*=TRUE*/)
{
    PegThing::Add(Who, bDraw);
    PositionChildren();
}

/*--------------------------------------------------------------------------*/
void PegToolBarPanel::AddToEnd(PegThing *Who, BOOL bDraw /*=TRUE*/)
{
    PegThing::Add(Who, bDraw);
    PositionChildren();
}

/*--------------------------------------------------------------------------*/
PegThing* PegToolBarPanel::Remove(PegThing* Who, BOOL bDraw /*=TRUE*/)
{
    PegThing* pThing = PegThing::Remove(Who, bDraw);
    PositionChildren();
    return(pThing);
}

/*--------------------------------------------------------------------------*/
void PegToolBarPanel::PositionChildren()
{
    // This also resizes the panel to just fit it's children
    PegRect tNewSize = mReal;
    PegRect tPanelSize = mReal;
    tPanelSize.wRight = tPanelSize.wLeft + 9;

    tNewSize.wLeft += 10;
    tNewSize.wTop += TOOLBAR_PAD_HEIGHT;
    tNewSize.wBottom -= TOOLBAR_PAD_HEIGHT;

    SIGNED iPanelHeight = tNewSize.Height();

    PegThing* pThing = First();
    while(pThing)
    {
        tNewSize.wRight = tNewSize.wLeft + pThing->mReal.Width();
        tNewSize.wBottom = tNewSize.wTop + pThing->mReal.Height() - 1;

        if((tNewSize.Height() + TOOLBAR_PAD_HEIGHT) > iPanelHeight)
        {
            iPanelHeight = tNewSize.Height() + TOOLBAR_PAD_HEIGHT;
        }

        if(tNewSize.wRight > tPanelSize.wRight)
        {
            tPanelSize.wRight = tNewSize.wRight + TOOLBAR_PAD_HEIGHT + 1;
        }

        pThing->Resize(tNewSize);

        tNewSize.wLeft = tNewSize.wRight + 1;

        pThing = pThing->Next();
    }

    if(tPanelSize.Height() != iPanelHeight)
    {
        tPanelSize.wBottom = tPanelSize.wTop + iPanelHeight;
    }
    Resize(tPanelSize);

    /*if(Parent())
    {
        if(Parent()->Type() == TYPE_TOOL_BAR)
        {
            ((PegToolBar*)Parent())->PositionPanels();
        }
    }
    */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegToolBar::PegToolBar(WORD wID /*=0*/) : 
    PegThing(wID),
    miPanelHeight(0) 
{
    Type(TYPE_TOOL_BAR);
    mReal.Set(0, 0, 100, miPanelHeight + (TOOLBAR_PAD_HEIGHT * 2));
    mClient.Set(0, 0, 100, miPanelHeight + (TOOLBAR_PAD_HEIGHT * 2));
    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    AddStatus(PSF_SIZEABLE | PSF_NONCLIENT);
}

/*--------------------------------------------------------------------------*/
PegToolBar::~PegToolBar()
{
}

/*--------------------------------------------------------------------------*/
SIGNED PegToolBar::Message(const PegMessage& Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
    case PM_PARENTSIZED:
        SizeToParent();
        PegThing::Message(Mesg);
        break;

    default:
        return(PegThing::Message(Mesg));
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
void PegToolBar::Draw()
{
    if (!Parent())
    {
        return;
    }

    BeginDraw();
    
    PegColor Color(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    Rectangle(mReal, Color, 1);
    
    Color.uForeground = PCLR_LOWLIGHT;
    Line(mReal.wLeft, mReal.wTop, mReal.wRight - 1, mReal.wTop, Color);
    Line(mReal.wLeft, mReal.wBottom, mReal.wRight - 1, mReal.wBottom, Color);

    PegThing::Draw();
    
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegToolBar::AddPanel(PegToolBarPanel* pPanel, BOOL bDraw /*=TRUE*/)
{
    PegThing* pThing = First();
    while(pThing)
    {
        if(pThing == pPanel)
        {
            return;
        }
        pThing = pThing->Next();
    }

    AddToEnd(pPanel, FALSE);

    PositionPanels();

    if(bDraw && (mwStatus & PSF_VISIBLE))
    {
        Invalidate(mClip);
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
PegThing* PegToolBar::RemovePanel(PegThing *Who, BOOL bDraw /*=TRUE*/)
{
    PegThing* pThing = PegThing::Remove(Who);

    PositionPanels();

    if(bDraw && (mwStatus & PSF_VISIBLE))
    {
        Invalidate(mClip);
        Draw();
    }

    return(pThing);
}

/*--------------------------------------------------------------------------*/
void PegToolBar::SizeToParent()
{
    if (!Parent())
    {
        return;
    }

    PegWindow *pWin = (PegWindow *) Parent();

    PegRect tNewSize = Parent()->mReal;

    if (pWin->Style() & FF_THICK)
    {
        tNewSize.wTop += PEG_FRAME_WIDTH - 1;
        tNewSize.wLeft += PEG_FRAME_WIDTH - 1;
        tNewSize.wRight -= PEG_FRAME_WIDTH - 2;
    }
    else
    {
        if (!(pWin->Style() & FF_NONE))
        {
            tNewSize.wTop += 1;
            tNewSize.wLeft += 1;
            tNewSize.wRight -= 1;
        }
    }

    // look for a title, if there is one I need to be under it:

    PegThing *pTest = Parent()->First();

    while(pTest)
    {
        if (pTest->Type() == TYPE_TITLE)
        {
            tNewSize.wTop += pTest->mReal.Height();
            break;
        }
        pTest = pTest->Next();
    }

    // Ditto for a menu
    pTest = Parent()->First();
    while(pTest)
    {
        if(pTest->Type() == TYPE_MENU_BAR)
        {
            tNewSize.wTop += pTest->mReal.Height();
            break;
        }
        pTest = pTest->Next();
    }

    tNewSize.wBottom = tNewSize.wTop + miPanelHeight;
    Resize(tNewSize);
}

/*--------------------------------------------------------------------------*/
void PegToolBar::PositionPanels()
{
    SizeToTallestPanel();

    PegRect tNewRect = mClient;
    PegThing* pThing = First();
    while(pThing)
    {
        tNewRect.wRight = tNewRect.wLeft + pThing->mReal.Width() - 1;
        pThing->Resize(tNewRect);
        tNewRect.wLeft = tNewRect.wRight + 1;
        pThing = pThing->Next();
    }
}

/*--------------------------------------------------------------------------*/
void PegToolBar::SizeToTallestPanel()
{
    if(mpParent)
    {
        if(mpParent->StatusIs(PSF_VISIBLE))
        {
            return;
        }
    }

    PegThing* pTest = First();

    SIGNED iOldPanelHeight = miPanelHeight;

    while(pTest)
    {
        if(pTest->mReal.Height() > miPanelHeight)
        {
            miPanelHeight = pTest->mReal.Height();
        }

        pTest = pTest->Next();
    }

    if(iOldPanelHeight != miPanelHeight)
    {
        PegRect tNewSize = mReal;
        tNewSize.wBottom = tNewSize.wTop + miPanelHeight;
        Resize(tNewSize);
    }
}

