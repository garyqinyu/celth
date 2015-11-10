/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pnotebk.cpp - Notebook window class implementation.
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
// Notebook windows have tabs at the top or bottom. The number of tabs
// is defined when the notebook window is constructed. What appears on each
// tab may be either simple text or any PegThing derived object type. 
// Likewise, a PegWindow derived object should be associated with
// each tab. The Notebook window takes care of displaying the correct window
// as each tab is selcted.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "string.h"
#include "peg.hpp"

extern PEGCHAR lsTEST[];

/*--------------------------------------------------------------------------*/
PegNotebook::PegNotebook(const PegRect &Rect, WORD wStyle, UCHAR uNumTabs) :
    PegWindow(Rect, wStyle)
{
    Type(TYPE_NOTEBOOK);
    RemoveStatus(PSF_SIZEABLE);
    muColors[PCI_NORMAL] = PCLR_DIALOG;
    muColors[PCI_SELECTED] = PCLR_DIALOG;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_STEXT] = PCLR_NORMAL_TEXT;
    SetSignals(SIGMASK(PSF_PAGE_SELECT));

    if (Style() & NS_TEXTTABS)
    {
        mpTabStrings = new PEGCHAR *[uNumTabs];
        mpTabClients = NULL;
    }
    else
    {
        mpTabStrings = NULL;
        mpTabClients = new PegThing *[uNumTabs];
    }
    mpPageClients = new PegThing *[uNumTabs];

    for (WORD wLoop = 0; wLoop < uNumTabs; wLoop++)
    {
        if (Style() & NS_TEXTTABS)
        {
            mpTabStrings[wLoop] = NULL;
        }
        else
        {
            mpTabClients[wLoop] = NULL;
        }
        mpPageClients[wLoop] = NULL;
    }
    muNumTabs = uNumTabs;
    mpFont = PegTextThing::GetDefaultFont(PEG_TAB_FONT);
    muSelectedTab = 0;
    muFirstTab = 0;
    mpSpin = NULL;
}



/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegNotebook::~PegNotebook()
{
    // remove and delete all the PageClients and tab strings:

    for (UCHAR wLoop = 0; wLoop < muNumTabs; wLoop++)
    {
        if (mpPageClients[wLoop] && wLoop != muSelectedTab)
        {
           
            Remove(mpPageClients[wLoop]);
            delete mpPageClients[wLoop];
        }

        if (mpTabStrings)
        {
            if (mpTabStrings[wLoop])
            {
                delete [] mpTabStrings[wLoop];
            }
        }
    }

    if (mpTabStrings)
    {
        delete [] mpTabStrings;
    }

    // if there are tab clients, they have been added to myself and will
    // automatically be deleted. I still need to delete the array of pointers
    // to the tab clients:

    if (mpTabClients)
    {
        delete [] mpTabClients;
    }
    delete [] mpPageClients;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegNotebook::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case PM_SHOW:
        CalculateTabPositions();
        PegWindow::Message(Mesg);
        AddCurrentPageClient();
        break;

   #ifdef PEG_KEYBOARD_SUPPORT
    case PM_GAINED_KEYBOARD:
        PegThing::Message(Mesg);
        RedrawTabs();
        break;

    case PM_LOST_KEYBOARD:
        PegThing::Message(Mesg);
        RedrawTabs();
        break;
   #endif

    case PM_LBUTTONDOWN:
        CheckTabClick(Mesg.Point);
        break;

    case PEG_SIGNAL(NB_SPIN, PSF_SPIN_MORE):
        if (muFirstTab + muVisibleTabs < muNumTabs)
        {
            muFirstTab++;
            RedrawTabs();
        }
        break;

    case PM_SIZE:
        if (mpPageClients[muSelectedTab])
        {
            Remove(mpPageClients[muSelectedTab], FALSE);
        }
        PegWindow::Message(Mesg);
        AddCurrentPageClient(FALSE);

        if (StatusIs(PSF_VISIBLE))
        {
            Parent()->Invalidate(Parent()->mReal);
            Parent()->Draw();
        }
        break;

    case PEG_SIGNAL(NB_SPIN, PSF_SPIN_LESS):
        if (muFirstTab)
        {
            muFirstTab--;
            RedrawTabs();
        }
        break;

   #ifdef PEG_KEYBOARD_SUPPORT
    case PM_KEY:
        switch(Mesg.iData)
        {
        case PK_RIGHT:
            if (muSelectedTab < muNumTabs - 1)
            {
                SelectTab(muSelectedTab + 1);
            }
           #if !defined(PEG_TAB_KEY_SUPPORT)
            else
            {
                return PegThing::Message(Mesg);
            }
           #endif
            break;

        case PK_LEFT:
            if (muSelectedTab)
            {
                SelectTab(muSelectedTab - 1);
            }
           #if !defined(PEG_TAB_KEY_SUPPORT)
            else
            {
                return PegThing::Message(Mesg);
            }
           #endif
            break;

        case PK_LNUP:
            if (!(Style() & NS_TOPTABS))
            {
                if (mpPageClients[muSelectedTab])
                {
                    Presentation()->MoveFocusTree(mpPageClients[muSelectedTab]);
                    RedrawTabs();
                    break;
                }
            }
            else
            {
                if (Presentation()->GetCurrentThing() != this)
                {
                    Presentation()->MoveFocusTree(this);
                }
                else
                {
                    return PegWindow::Message(Mesg);
                }
            }
            break;

        case PK_LNDN:
            if (Style() & NS_TOPTABS)
            {
                if (mpPageClients[muSelectedTab])
                {
                    Presentation()->MoveFocusTree(mpPageClients[muSelectedTab]);
                    RedrawTabs();
                    break;
                }
            }
            else
            {
                if (Presentation()->GetCurrentThing() != this)
                {
                    Presentation()->MoveFocusTree(this);
                }
                else
                {
                    return PegWindow::Message(Mesg);
                }
            }
            return PegWindow::Message(Mesg);

        case PK_TAB:
            if (Presentation()->GetCurrentThing() != this)
            {
                Presentation()->MoveFocusTree(this);
                break;
            }

        default:
            return PegWindow::Message(Mesg);
        }
        break;
   #endif

    default:
        return(PegWindow::Message(Mesg));
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
void PegNotebook::Draw(void)
{
    BeginDraw();
    DrawFrame();
    DrawTabs(FALSE);
    DrawTabs(TRUE);
    DrawChildren();
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegNotebook::DrawTabs(BOOL bSelected)
{
    PegRect TabRect, SaveClip, ClipTabs;
    PegPoint Put;
    UCHAR wLoop;

    TabRect.wLeft = mReal.wLeft + 2;
    TabRect.wRight = TabRect.wLeft + miTabWidth - 1;

    PegColor Color(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    PegColor TextColor(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);

    if (bSelected)
    {
        Color.uBackground = muColors[PCI_SELECTED];
        TextColor.uForeground = muColors[PCI_STEXT];
    }

    if (Style() & NS_TOPTABS)
    {
        TabRect.wTop = mReal.wTop + 1;
        TabRect.wBottom = TabRect.wTop + miTabHeight - 1;
    }
    else
    {
        TabRect.wBottom = mReal.wBottom - 3;
        TabRect.wTop = TabRect.wBottom - miTabHeight + 1;
    }

    ClipTabs = TabRect;
    ClipTabs++;
    ClipTabs.wRight = mReal.wRight - 2;

    if (mpSpin)
    {
        ClipTabs.wRight -= (PEG_SCROLL_WIDTH * 2) + 2;
    }
    SaveClip = mClip;
    mClip &= ClipTabs;

    if (!bSelected)
    {
        TabRect -= 2;
    }

    for (wLoop = muFirstTab; wLoop < muNumTabs && TabRect.wLeft < mClip.wRight;
        wLoop++)
    {
        if ((bSelected && wLoop == muSelectedTab) ||
            (!bSelected && wLoop != muSelectedTab))
        {
            Rectangle(TabRect, Color, 0);
            Color.uForeground = PCLR_HIGHLIGHT;

            if (Style() & NS_TOPTABS)
            {
                Line(TabRect.wLeft + 1, TabRect.wTop - 1,
                    TabRect.wRight + 1, TabRect.wTop - 1, Color);
                Line(TabRect.wLeft - 1, TabRect.wTop + 1,
                    TabRect.wLeft - 1, TabRect.wBottom, Color);
                Line(TabRect.wLeft, TabRect.wTop,
                    TabRect.wLeft, TabRect.wTop, Color);
                Color.uForeground = PCLR_LOWLIGHT;
                Line(TabRect.wRight + 1, TabRect.wTop - 1,
                    TabRect.wRight + 1, TabRect.wBottom, Color);
                Color.uForeground = PCLR_SHADOW;
                Line(TabRect.wRight + 2, TabRect.wTop,
                    TabRect.wRight + 2, TabRect.wBottom, Color);
            }
            else
            {
                Line(TabRect.wLeft - 1, TabRect.wTop,
                    TabRect.wLeft - 1, TabRect.wBottom, Color);
                Color.uForeground = PCLR_LOWLIGHT;
                Line(TabRect.wRight + 1, TabRect.wTop,
                    TabRect.wRight + 1, TabRect.wBottom, Color);
                Line(TabRect.wLeft, TabRect.wBottom + 1,
                    TabRect.wRight + 1, TabRect.wBottom + 1, Color);
                Color.uForeground = PCLR_SHADOW;

                if (!bSelected)
                {
                    Line(TabRect.wRight + 2, TabRect.wTop,
                        TabRect.wRight + 2, TabRect.wBottom + 1, Color);
                }
                else
                {
                    Line(TabRect.wRight + 2, TabRect.wTop + 1,
                        TabRect.wRight + 2, TabRect.wBottom + 1, Color);
                }
                Line(TabRect.wLeft + 1, TabRect.wBottom + 2,
                    TabRect.wRight + 2, TabRect.wBottom + 2, Color);
            }

            if (mpTabStrings)
            {
                if (mpTabStrings[wLoop])
                {
                    Put.x = (TabRect.wLeft + TabRect.wRight) >> 1;
                    Put.y = TabRect.wTop + 1;
                    if (Style() & NS_TOPTABS)
                    {
                        Put.y++;
                    }
                    Put.x -= TextWidth(mpTabStrings[wLoop], mpFont) >> 1;
                    DrawText(Put, mpTabStrings[wLoop], TextColor, mpFont);
                }
            }
            if (bSelected && Presentation()->GetCurrentThing() == this)
            {
                Color.uForeground = PCLR_LOWLIGHT;
                Color.uFlags = CF_NONE;
                TabRect--;
                Rectangle(TabRect, Color, 1);
            }
        }
        TabRect.Shift(miTabWidth, 0);
    }
    mClip = SaveClip;
}

/*--------------------------------------------------------------------------*/
void PegNotebook::RedrawTabs(void)
{
    PegRect Invalid = mReal;
    if (Style() & NS_TOPTABS)
    {
        Invalid.wBottom = Invalid.wTop + miTabHeight;
    }
    else
    {
        Invalid.wTop = Invalid.wBottom - miTabHeight - 2;
    }
    if (!mpTabStrings)
    {
        CalculateTabPositions();
    }
    Invalidate(Invalid);
    Draw();
}

/*--------------------------------------------------------------------------*/
void PegNotebook::DrawFrame(BOOL)
{
    PegColor Color(PCLR_BORDER, muColors[PCI_NORMAL], CF_FILL);
    PegRect Fill = mReal;
    WORD wWidth = 2;

    if (Style() & FF_THICK)
    {
        wWidth = PEG_FRAME_WIDTH;
    }
    
    if (Style() & NS_TOPTABS)
    {
        Fill.wTop = mClient.wTop - wWidth;
        Rectangle(Fill, Color, wWidth);
        Color.Set(PCLR_BORDER, PCLR_BORDER, CF_FILL);
        Fill.wBottom = Fill.wTop - 1;
        Fill.wTop = mReal.wTop;
        Rectangle(Fill, Color, 0);
    }
    else
    {
        Fill.wBottom = mClient.wBottom + wWidth;
        Rectangle(Fill, Color, wWidth);
        Color.Set(PCLR_BORDER, PCLR_BORDER, CF_FILL);
        Fill.wTop = Fill.wBottom + 1;
        Fill.wBottom = mReal.wBottom;
        Rectangle(Fill, Color, 0);        
    }

    Color.uForeground = PCLR_HIGHLIGHT;

    if (Style() & FF_THICK)
    {
        wWidth = PEG_FRAME_WIDTH - 1;
    }
    else
    {
        wWidth = 2;
    }

    if (Style() & NS_TOPTABS)
    {
        // add left edge with top tabs:
        Line(mReal.wLeft + 1, mClient.wTop - wWidth,
            mReal.wLeft + 1, mReal.wBottom, Color);

        // add top edge with top tabs:
        Line(mReal.wLeft + 1, mClient.wTop - wWidth,
            mReal.wRight - 1, mClient.wTop - wWidth, Color);

        Color.uForeground = PCLR_LOWLIGHT;

        // add right edge with top tabs:
        Line(mReal.wRight - 1,
            mClient.wTop - wWidth + 1, mReal.wRight - 1,
            mReal.wBottom - 1, Color);

        // add bottom edge with top tabs:
        Line(mReal.wLeft + 1, mReal.wBottom - 1,
            mReal.wRight - 1, mReal.wBottom - 1, Color);
    }
    else
    {
        // add left edge with bottom tabs:
        Line(mReal.wLeft + 1, mReal.wTop + 1,
            mReal.wLeft + 1, mClient.wBottom + wWidth, Color);

        // add top edge with bottom tabs:
        Line(mReal.wLeft + 1, mReal.wTop + 1,
            mReal.wRight - 1, mReal.wTop + 1, Color);

        Color.uForeground = PCLR_LOWLIGHT;

        // add the right edge with bottom tabs:
        Line(mReal.wRight - 1,
            mReal.wTop + 1, mReal.wRight - 1,
            mClient.wBottom + wWidth, Color);

        // add the bottom edge with bottom tabs:
        Line(mReal.wLeft + 1,
            mClient.wBottom + wWidth - 1, mReal.wRight - 1,
            mClient.wBottom + wWidth - 1, Color);
    }

    // add shadows:

    Color.uForeground = PCLR_SHADOW;

    if (Style() & NS_TOPTABS)
    {
        // add right shadow with top tabs:
        Line(mReal.wRight, mClient.wTop - wWidth,
            mReal.wRight, mReal.wBottom, Color);

        // add the bottom shadow with top tabs:
        Line(mReal.wLeft, mReal.wBottom, mReal.wRight,
            mReal.wBottom, Color);
    }
    else
    {
        // add the right shadow with bottom tabs:
        Line(mReal.wRight,
            mReal.wTop, mReal.wRight,
            mClient.wBottom + wWidth, Color);

        // add the bottom shadow with bottom tabs:
        Line(mReal.wLeft + 1,
            mClient.wBottom + wWidth, mReal.wRight,
            mClient.wBottom + wWidth, Color);
    }

    if (Style() & FF_THICK)
    {
        Color.uForeground = PCLR_LOWLIGHT;

        // underline border top with top tabs:
        Line(mClient.wLeft - 1, mClient.wTop - 1,
            mClient.wRight + 1, mClient.wTop - 1, Color);

        // darken right edge of left border:
        Line(mClient.wLeft - 1, mClient.wTop,
            mClient.wLeft - 1, mClient.wBottom + 1, Color);

        // highlight top of bottom border:
        Color.uForeground = PCLR_HIGHLIGHT;
        Line(mClient.wLeft - 1, mClient.wBottom + 2,
            mClient.wRight + 1, mClient.wBottom + 2, Color);

        // highlight left edge of right border:
        Line(mClient.wRight + 2, mClient.wTop,
            mClient.wRight + 2, mClient.wBottom + 1, Color);
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegNotebook::SelectTab(UCHAR uIndex)
{
    if (uIndex >= muNumTabs)
    {
        return;
    }
    if (muSelectedTab != uIndex)
    {
        if (mpSpin)
        {
            // see if we need to spin this guy into view:

            SIGNED iLeft = mReal.wLeft;
            iLeft += ((SIGNED) uIndex - (SIGNED) muFirstTab) * miTabWidth;
            SIGNED iRight = iLeft + miTabWidth;
            
            if (iRight >= mpSpin->mReal.wLeft)
            {
                muFirstTab = uIndex - ((mpSpin->mReal.wLeft - mReal.wLeft) / miTabWidth) + 1;
            }
            else
            {
                if (iLeft < mReal.wLeft)
                {
                    muFirstTab = uIndex;
                }
            }
        }

        if (mpPageClients[muSelectedTab])
        {
            Remove(mpPageClients[muSelectedTab], FALSE);
        }
        muSelectedTab = uIndex;
        AddCurrentPageClient();

        if (StatusIs(PSF_VISIBLE))
        {
            Invalidate(mReal);
            Draw();

            if (Id())
            {
                PegMessage NewMessage(Parent(), PEG_SIGNAL(Id(), PSF_PAGE_SELECT));
                NewMessage.pSource = this;
                NewMessage.iData = Id();
                NewMessage.lData = muSelectedTab;
                MessageQueue()->Push(NewMessage);
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
BOOL PegNotebook::CheckTabClick(PegPoint Click)
{
    if (Style() & NS_TOPTABS)
    {
        // see if the click was between mReal.wTop and mClient.wTop:

        if (Click.y >= mClient.wTop)
        {
            return FALSE;
        }
    }
    else
    {
        if (Click.y <= mClient.wBottom)
        {
            return FALSE;
        }
    }

    // see if the click was on a tab:

    SIGNED iLeft = mReal.wLeft;
    SIGNED iRight = iLeft + miTabWidth - 1;

    for (UCHAR uLoop = muFirstTab; uLoop < muNumTabs; uLoop++)
    {
        if (Click.x >= iLeft && Click.x <= iRight)
        {
            if (muSelectedTab != uLoop)
            {
                SelectTab(uLoop);
                return TRUE;
            }
            return FALSE;
        }
        iLeft += miTabWidth;
        iRight += miTabWidth;
    }
    return FALSE;
}


/*--------------------------------------------------------------------------*/
void PegNotebook::AddCurrentPageClient(BOOL bDraw)
{
    if (mpPageClients[muSelectedTab])
    {
        PegRect PageRect = mClient;
        PageRect -= 4;

        if (mpPageClients[muSelectedTab]->mReal != PageRect)
        {
            mpPageClients[muSelectedTab]->Resize(PageRect);
        }
        Add(mpPageClients[muSelectedTab], bDraw);
        //if (StatusIs(PSF_VISIBLE))
        //{
        //    Presentation()->MoveFocusTree(mpPageClients[muSelectedTab]);
        //}
    }
    //else
    //{
        if (StatusIs(PSF_VISIBLE))
        {
            Presentation()->MoveFocusTree(this);
        }
    //}
}

/*--------------------------------------------------------------------------*/
void PegNotebook::ResetNumTabs(UCHAR uNum)
{
    PEGCHAR **pOldTabStrings = mpTabStrings;
    PegThing **pOldTabClients = mpTabClients;
    PegThing **pOldPageClients = mpPageClients;
    WORD wLoop;

    if (mpPageClients[muSelectedTab])
    {
        Remove(mpPageClients[muSelectedTab]);
    }

    if (Style() & NS_TEXTTABS)
    {
        mpTabStrings = new PEGCHAR *[uNum];
        mpTabClients = NULL;
    }
    else
    {
        mpTabStrings = NULL;
        mpTabClients = new PegThing *[uNum];
    }
    mpPageClients = new PegThing *[uNum];

    for (wLoop = 0; wLoop < uNum; wLoop++)
    {
        if (Style() & NS_TEXTTABS)
        {
            if (wLoop < muNumTabs)
            {
                mpTabStrings[wLoop] = pOldTabStrings[wLoop];
            }
            else
            {
                mpTabStrings[wLoop] = NULL;
            }
        }
        else
        {
            if (wLoop < muNumTabs)
            {
                mpTabClients[wLoop] = pOldTabClients[wLoop];
            }
            else
            {
                mpTabClients[wLoop] = NULL;
            }
        }
        if (wLoop < muNumTabs)
        {
            mpPageClients[wLoop] = pOldPageClients[wLoop];
        }
        else
        {
            mpPageClients[wLoop] = NULL;
        }
    }

    // remove and delete any leftover PageClients and tab strings:

    while (wLoop < muNumTabs)
    {
        if (pOldPageClients[wLoop])
        {
            delete pOldPageClients[wLoop];
        }

        if (pOldTabStrings)
        {
            if (pOldTabStrings[wLoop])
            {
                delete [] pOldTabStrings[wLoop];
            }
        }
        wLoop++;
    }

    if (pOldTabStrings)
    {
        delete [] pOldTabStrings;
    }
    if (pOldTabClients)
    {
        delete [] pOldTabClients;
    }
    delete pOldPageClients;
    muNumTabs = uNum;
    muSelectedTab = 0;
    muFirstTab = 0;
    CalculateTabPositions();
    AddCurrentPageClient();

    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate(mReal);
    }
}


/*--------------------------------------------------------------------------*/
void PegNotebook::SetTabClient(UCHAR uIndex, PegThing *TabClient)
{
    if (mpTabClients)
    {
        if (uIndex < muNumTabs)
        {
            if (mpTabClients[uIndex])
            {
                Remove(mpTabClients[uIndex]);
                delete mpTabClients[uIndex];
            }
            TabClient->Id(uIndex + 1);
            TabClient->AddStatus(PSF_NONCLIENT);
            mpTabClients[uIndex] = TabClient;
            Add(TabClient);
        }
    }
}

/*--------------------------------------------------------------------------*/
PegThing *PegNotebook::GetTabClient(UCHAR uIndex)
{
    if (mpTabClients)
    {
        if (uIndex < muNumTabs)
        {
            return (mpTabClients[uIndex]);
        }
    }
    return NULL;
}


/*--------------------------------------------------------------------------*/
void PegNotebook::SetPageClient(UCHAR uIndex, PegThing *PageClient)
{
    if (uIndex < muNumTabs)
    {
        if (mpPageClients[uIndex])
        {
            if (PageClient)
            {
                Remove(mpPageClients[uIndex], FALSE);
            }
            else
            {
                Remove(mpPageClients[uIndex]);
            }
            delete mpPageClients[uIndex];
        }
        mpPageClients[uIndex] = PageClient;

        if (uIndex == muSelectedTab && StatusIs(PSF_VISIBLE) && PageClient)
        {
            AddCurrentPageClient(TRUE);
        }
    }
}

/*--------------------------------------------------------------------------*/
PegThing *PegNotebook::GetPageClient(UCHAR uIndex)
{
    if (uIndex < muNumTabs)
    {
        return (mpPageClients[uIndex]);
    }
    return NULL;
}

/*--------------------------------------------------------------------------*/
PegThing *PegNotebook::RemovePageClient(UCHAR uIndex)
{
    PegThing *pReturn = NULL;
    if (uIndex < muNumTabs)
    {
        if (mpPageClients[uIndex])
        {
            pReturn = mpPageClients[uIndex];

            if (uIndex == muSelectedTab)
            {
                Remove(mpPageClients[uIndex]);
            }
            mpPageClients[uIndex] = NULL;
        }
    }
    return pReturn;
}


/*--------------------------------------------------------------------------*/
void PegNotebook::SetTabString(UCHAR uIndex, const PEGCHAR *Text)
{
    if (mpTabStrings)
    {
        if (uIndex < muNumTabs)
        {
            if (mpTabStrings[uIndex])
            {
                delete [] mpTabStrings[uIndex];
                mpTabStrings[uIndex] = NULL;
            }
            if (Text)
            {
                mpTabStrings[uIndex] = new PEGCHAR[strlen(Text) + 1];
                strcpy(mpTabStrings[uIndex], Text);
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
void PegNotebook::CalculateTabPositions(void)
{
    miTabWidth = -1;
    miTabHeight = -1;
    SIGNED iTemp;
    UCHAR uLoop;

    if (mpTabStrings)
    {
        // find the widest string:

        for (uLoop = 0; uLoop < muNumTabs; uLoop++)
        {
            if (mpTabStrings[uLoop])
            {
                iTemp = TextWidth(mpTabStrings[uLoop], mpFont);

                if (iTemp > miTabWidth)
                {
                    miTabWidth = iTemp;
                }
            }
        }
        miTabHeight = TextHeight(lsTEST, mpFont);
    }
    else
    {
        // find the widest and tallest thing:

        for (uLoop = 0; uLoop < muNumTabs; uLoop++)
        {
            if (mpTabClients[uLoop])
            {
                iTemp = mpTabClients[uLoop]->mReal.Width();

                if (iTemp > miTabWidth)
                {
                    miTabWidth = iTemp;
                }

                iTemp = mpTabClients[uLoop]->mReal.Height();

                if (iTemp > miTabHeight)
                {
                    miTabHeight = iTemp;
                }
            }
        }
    }

    // add 12 pixels in width, 6 pixels in height:

    miTabWidth += 12;
    miTabHeight += 6;
    InitClient();

    if (Style() & NS_TOPTABS)
    {
        mClient.wTop = mReal.wTop + miTabHeight;

        if (Style() & FF_THICK)
        {
            mClient.wTop += PEG_FRAME_WIDTH - 1;
        }
        else
        {
            mClient.wTop += 2;
        }
    }
    else
    {
        mClient.wBottom = mReal.wBottom - miTabHeight;

        if (Style() & FF_THICK)
        {
            mClient.wBottom -= PEG_FRAME_WIDTH;
        }
        else
        {
            mClient.wBottom -= 3;
        }
    }

    muVisibleTabs = (UCHAR) ((mClient.Width() - 4) / miTabWidth);

    if (muVisibleTabs < muNumTabs)
    {
        // I need a spin button:

        PegRect SpinRect;

        if (Style() & NS_TOPTABS)
        {
            SpinRect.Set(mReal.wRight - (PEG_SCROLL_WIDTH * 2) - 2,
                     mClient.wTop - PEG_SCROLL_WIDTH - 3,
                     mReal.wRight - 2,
                     mClient.wTop - 3);
        }
        else
        {
            SpinRect.Set(mReal.wRight - (PEG_SCROLL_WIDTH * 2) - 2,
                     mClient.wBottom + 3,
                     mReal.wRight - 2,
                     mClient.wBottom + PEG_SCROLL_WIDTH + 3);
        }
        if (!mpSpin)
        {        
            mpSpin = new PegSpinButton(SpinRect, NB_SPIN, AF_ENABLED);
            mpSpin->AddStatus(PSF_NONCLIENT);
            Add(mpSpin);
        }
        else
        {
            mpSpin->Resize(SpinRect);
        }
        muVisibleTabs = (UCHAR) ((mClient.Width() - 4 - PEG_SCROLL_WIDTH * 2) / miTabWidth);
    }
    else
    {
        if (mpSpin)
        {
            Destroy(mpSpin);
            mpSpin = NULL;
        }
    }

    // if I simply have strings on my tabs, then there is nothing else
    // to do:

    if (mpTabStrings)
    {
        return;
    }    

    // here if I have client things on my tabs, position all of them  

    PegRect PutClient;

    PutClient.wLeft = mReal.wLeft + 6 - ((SIGNED) muFirstTab * miTabWidth);
    PutClient.wRight = PutClient.wLeft + miTabWidth - 13;

    if (Style() & NS_TOPTABS)
    {
        PutClient.wTop = mReal.wTop + 3;
        PutClient.wBottom = PutClient.wTop + miTabHeight - 7;
    }
    else
    {
        PutClient.wBottom = mReal.wBottom - 3;
        PutClient.wTop = PutClient.wBottom - miTabHeight + 7;
    }

    for (uLoop = 0; uLoop < muNumTabs; uLoop++)
    {
        if (mpTabClients[uLoop])
        {
            mpTabClients[uLoop]->Resize(PutClient);
            PutClient.Shift(miTabWidth, 0);
        }
    }
}


/*--------------------------------------------------------------------------*/
void PegNotebook::ResetTabStyle(WORD wStyle)
{
    UCHAR uLoop, uLoop1;

    if (!(wStyle & NS_TEXTTABS) && mpTabStrings)
    {
        // get rid of the tab string pointers:

        for (uLoop = 0; uLoop < muNumTabs; uLoop++)
        {
            if (mpTabStrings[uLoop])
            {
                delete [] mpTabStrings[uLoop];
            }
        }
        delete [] mpTabStrings;
        mpTabStrings = NULL;
        mpTabClients = new PegThing *[muNumTabs];
        for (uLoop1 = 0; uLoop1 < muNumTabs; uLoop1++)
        {
            mpTabClients[uLoop1] = NULL;
        }
    }
    else
    {
        if ((wStyle & NS_TEXTTABS) && mpTabClients)
        {
            // get rid of the tab thing pointers:

            for (uLoop = 0; uLoop < muNumTabs; uLoop++)
            {
                if (mpTabClients[uLoop])
                {
                    Remove(mpTabClients[uLoop]);
                    delete mpTabClients[uLoop];
                }
            }
            delete [] mpTabClients;
            mpTabClients = NULL;
            mpTabStrings = new PEGCHAR *[muNumTabs];

            for (uLoop1 = 0; uLoop1 < muNumTabs; uLoop1++)
            {
                mpTabStrings[uLoop1] = NULL;
            }
        }
    }
}

// End of file
