/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pmenu.cpp - PegMenuBar, PegMenu, and PegMenuItem class definitions.
//
// Author: Kenneth G. Maxwell
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

#include "string.h"
#include "peg.hpp"

//extern PegFont MenuFont;
extern PegBitmap gbMoreBitmap;
extern PegBitmap gbCheckBitmap;
extern PegBitmap gbDotBitmap;

#ifdef PEG_KEYBOARD_SUPPORT

/*--------------------------------------------------------------------------*/
// Support functions for keyboard support. Used by PegMenu and PegMenuBar.
/*--------------------------------------------------------------------------*/

PegMenuButton *MenuFindActiveButton(PegMenuButton *pButton)
{
    while(pButton)
    {
        if (pButton->IsPointerOver())
        {
            break;
        }
        if (pButton->IsSubVisible())
        {
            pButton = (PegMenuButton *) pButton->GetSubMenu()->First();
        }
        else
        {
            pButton = (PegMenuButton *) pButton->Next();
        }
    }
    return pButton;
}


/*--------------------------------------------------------------------------*/
PegMenuButton *MenuFindOwnerButton(PegMenuButton *pMenu, PegMenuButton *pStart)
{
    while(pStart)
    {
        if (pStart->IsSubVisible())
        {
            PegMenuButton *pButton = (PegMenuButton *) pStart->GetSubMenu()->First();

            while(pButton)
            {
                if (pButton == pMenu)
                {
                    return(pStart);
                }
                if (pButton->IsSubVisible())
                {
                    return MenuFindOwnerButton(pMenu, pButton);
                }
                pButton = (PegMenuButton *) pButton->Next();
            }
        }
        pStart = (PegMenuButton *) pStart->Next();
    }
    return pStart;
}

/*--------------------------------------------------------------------------*/
PegMenuButton *MenuNextButton(PegMenuButton *pCurrent)
{
    PegMenuButton *pTest = (PegMenuButton *) pCurrent->Next();

    if (pTest == NULL)
    {
        pTest = (PegMenuButton *) pCurrent->Parent()->First();
    }
    while(pTest)
    {
        if (!(pTest->Style() & BF_SEPARATOR) && (pTest->Style() & AF_ENABLED))
        {
            break;
        }
        pTest = (PegMenuButton *) pTest->Next();
    }
    return pTest;
}

/*--------------------------------------------------------------------------*/
PegMenuButton *MenuPreviousButton(PegMenuButton *pCurrent)
{
    PegMenuButton *pTest;

    if (pCurrent->Previous())
    {
        pTest = (PegMenuButton *) pCurrent->Previous();
    }
    else
    {
        pTest = (PegMenuButton *) pCurrent;
        while(pTest->Next())
        {
            pTest = (PegMenuButton *) pTest->Next();
        }
    }

    while(pTest)
    {
        if (!(pTest->Style() & BF_SEPARATOR) && (pTest->Style() & AF_ENABLED))
        {
            break;
        }
        pTest = (PegMenuButton *) pTest->Previous();
    }
    return pTest;
}


/*--------------------------------------------------------------------------*/
void MenuMoveToButton(PegMenuButton *pCurrent, PegThing *pNext)
{
    PegMenuButton *pTarget = (PegMenuButton *) pNext;

    if (pCurrent)
    {
        pCurrent->Message(PM_POINTER_EXIT);
    }
    pTarget->Message(PM_POINTER_ENTER);
    
    if (pTarget->IsSubVisible())
    {
        PegThing *pTest = pTarget->GetSubMenu()->First();

        while(pTest)
        {
            if (!(pTest->Style() & BF_SEPERATOR) && (pTest->Style() & AF_ENABLED))
            {
                break;
            }
            pTest = pTest->Next();
        }

        if (pTest)
        {
            pTarget->Message(PM_POINTER_EXIT);
            pTest->Message(PM_POINTER_ENTER);
        }
    }
}

#endif

/*--------------------------------------------------------------------------*/
// Start PegMenuBar class implementation
/*--------------------------------------------------------------------------*/

PegMenuBar::PegMenuBar(PegMenuDescription *pDesc) : PegThing()
{
    PegFont *pFont = DEF_MENU_FONT;
    mReal.Set(0, 0, 100, pFont->uHeight + 6);
    mClient.Set(0, 0, 100, pFont->uHeight + 6);
    Type(TYPE_MENU_BAR);
    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    AddStatus(PSF_SIZEABLE | PSF_MOVEABLE | PSF_NONCLIENT);

    if (pDesc)
    {
        while (pDesc->wId || pDesc->wStyle)
        {
            Add(new PegMenuButton(pDesc));
            pDesc++;
        }
    }
    mbFlyoverMode = 0;
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegMenuBar::~PegMenuBar()
{

}


/*--------------------------------------------------------------------------*/
SIGNED PegMenuBar::Message(const PegMessage &Mesg)
{
    PegThing *Child;
    PegMenuButton *pmb;

    switch(Mesg.wType)
    {
    case PM_SHOW:
        Child = First();

        while(Child)
        {
            if (Child->Type() == TYPE_MENU_BUTTON)
            {
                pmb = (PegMenuButton *) Child;
                pmb->SetOwner(Parent());
            }
            Child = Child->Next();
        }

        // fall through to PM_PARENTSIZED:

        
    case PM_PARENTSIZED:
        SizeToParent();
        PositionButtons();
        PegThing::Message(Mesg);
        break;

    case PM_NONCURRENT:
        PegThing::Message(Mesg);
        CloseSiblings(NULL);
        mbFlyoverMode = 0;
       #ifdef PEG_KEYBOARD_SUPPORT
        pmb = MenuFindActiveButton((PegMenuButton *) First());
        if (pmb)
        {
            pmb->Message(PM_POINTER_EXIT);
        }
       #endif
        break;

   #ifdef PEG_KEYBOARD_SUPPORT
    case PM_KEY:
        MenuKeyHandler(Mesg.iData);
        break;
   #endif

    default:
        PegThing::Message(Mesg);
        break;
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void PegMenuBar::Draw(void)
{
    BeginDraw();
    PegColor Color(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    Rectangle(mReal, Color, 0);
    PegThing::Draw();
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegMenuBar::SizeToParent(void)
{
    if (!Parent())
    {
        return;
    }

    PegWindow *pWin = (PegWindow *) Parent();

    PegRect NewSize = Parent()->mReal;

    if (pWin->Style() & FF_THICK)
    {
        NewSize.wTop += PEG_FRAME_WIDTH - 1;
        NewSize.wLeft += PEG_FRAME_WIDTH - 1;
        NewSize.wRight -= PEG_FRAME_WIDTH - 2;
    }
    else
    {
        if (!(pWin->Style() & FF_NONE))
        {
            NewSize.wTop += 2;
            NewSize.wLeft += 2;
            NewSize.wRight -= 2;
        }
    }

    // look for a title, if there is one I need to be under it:

    PegThing *pTest = Parent()->First();

    while(pTest)
    {
        if (pTest->Type() == TYPE_TITLE)
        {
            NewSize.wTop += pTest->mReal.Height();
            break;
        }
        pTest = pTest->Next();
    }

    PegFont *pFont = DEF_MENU_FONT;
    NewSize.wBottom = NewSize.wTop + pFont->uHeight + 6;
    PegMessage NewMessage(PM_SIZE);
    NewMessage.Rect = NewSize;
    Message(NewMessage);
}

/*--------------------------------------------------------------------------*/
void PegMenuBar::PositionButtons(void)
{
    PegRect Put;
    PegRect Current;
    Put.wTop = mReal.wTop + 2;
    Put.wLeft = mReal.wLeft + 2;
    Put.wRight = Put.wLeft;
    PegMessage NewMessage;

    // I have to figure out where my children go!

    PegMenuButton *pButton = (PegMenuButton *) First();

    while(pButton)
    {
        Put.wLeft = Put.wRight + 4;
        Current = pButton->GetMinSize(TYPE_MENU_BAR);
        Put.wRight = Put.wLeft + Current.Width();
        Put.wBottom = Put.wTop + Current.Height() - 1;
        NewMessage.wType = PM_SIZE;
        NewMessage.Rect = Put;
        pButton->Message(NewMessage);
        pButton = (PegMenuButton *) pButton->Next();
    }
}

/*--------------------------------------------------------------------------*/
void PegMenuBar::CloseSiblings(PegMenuButton *NotMe)
{
    PegMenuButton *pButton = (PegMenuButton *) First();

    while(pButton)
    {
        if (pButton != NotMe)
        {
            pButton->CloseMenu();
        }
        pButton = (PegMenuButton *) pButton->Next();
    }
}


/*--------------------------------------------------------------------------*/
PegMenuButton *PegMenuBar::FindButton(const PEGCHAR *Who)
{
    PegMenuButton *pTest = (PegMenuButton *) First();
    PegMenuButton *pFound;

    while(pTest)
    {
        if (pTest->DataGet())
        {
            if (!strcmp(pTest->DataGet(), Who))
            {
                return pTest;
            }
            else
            {
                pFound = pTest->FindButton(Who);

                if (pFound)
                {
                    return pFound;
                }
            }
        }
        pTest = (PegMenuButton *) pTest->Next();
    }
    return NULL;
}


#ifdef PEG_KEYBOARD_SUPPORT

/*--------------------------------------------------------------------------*/
void PegMenuBar::MenuKeyHandler(SIGNED iKey)
{
    PegMenuButton *pButton = MenuFindActiveButton((PegMenuButton *) First());
    PegMenuButton *pButton1;

    if (iKey == PK_MENU_KEY)
    {
        if (First())
        {
            if (pButton)
            {
                pButton->Message(PM_POINTER_EXIT);
            }
            Presentation()->MoveFocusTree(this);
            First()->Message(PM_POINTER_ENTER);
        }
        return;
    }
        
    if (!pButton)
    {
        return;
    }

    switch(iKey)
    {
    case PK_RIGHT:
        if (pButton->Parent() == this)
        {
            if (MenuNextButton(pButton))
            {
                MenuMoveToButton(pButton, MenuNextButton(pButton));
            }
        }
        else
        {
            if (pButton->IsSubVisible())
            {
                MenuMoveToButton(pButton, pButton->GetSubMenu()->First());
            }
            else
            {
                pButton1 = MenuFindOwnerButton(pButton,
                    (PegMenuButton *) First());
                while(pButton1 && !MenuNextButton(pButton1))
                {   
                    pButton1 = MenuFindOwnerButton(pButton1,
                        (PegMenuButton *) First());
                }
                if (pButton1)
                {
                    MenuMoveToButton(pButton, MenuNextButton(pButton1));
                }
            }
        }
        break;
            
    case PK_LNDN:
        if (MenuNextButton(pButton))
        {
            pButton->Message(PM_POINTER_EXIT);
            MenuNextButton(pButton)->Message(PM_POINTER_ENTER);
        }
        break;

    case PK_LEFT:
        if (pButton->Parent() == this)
        {
            if (MenuPreviousButton(pButton))
            {
                MenuMoveToButton(pButton, MenuPreviousButton(pButton));
            }
        }
        else
        {
            pButton1 = MenuFindOwnerButton(pButton, (PegMenuButton *) First());
            if (pButton1)
            {
                pButton->Message(PM_POINTER_EXIT);
                pButton1->Message(PM_POINTER_ENTER);

                if (pButton1->Parent() == this && MenuPreviousButton(pButton1))
                {
                    MenuMoveToButton(pButton1, MenuPreviousButton(pButton1));
                }
                else
                {
                    pButton1->Message(PM_LBUTTONDOWN);
                }
            }
        }
        break;

    case PK_LNUP:
        if (MenuPreviousButton(pButton))
        {
            pButton->Message(PM_POINTER_EXIT);
            MenuPreviousButton(pButton)->Message(PM_POINTER_ENTER);
        }
        break;

    case PK_CR:
        pButton->Message(PM_LBUTTONDOWN);
        
        if (pButton->Parent() == this && pButton->mbSubVisible)
        {
            pButton->Message(PM_POINTER_EXIT);
            pButton->mpSubMenu->First()->Message(PM_POINTER_ENTER);
        }
        break;

    case PK_ESC:
        pButton->Message(PM_POINTER_EXIT);
        Presentation()->MoveFocusTree(Parent());
        break;

    default:
        break;
    }
}


#endif


/******************  Peg Menu class definition  *****************************/

/*--------------------------------------------------------------------------*/
// Peg menu: A visible container object for PegMenuButtons
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
PegMenu::PegMenu(PegMenuDescription *pDesc, BOOL bPopup) : PegThing()
{
    mReal.Set(0, 0, 100, 20);
    mClient = mReal;
    mClient -= 2;
    Type(TYPE_MENU);
    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    AddStatus(PSF_SIZEABLE|PSF_MOVEABLE|PSF_NONCLIENT|PSF_VIEWPORT);

    if (!bPopup)
    {
        RemoveStatus(PSF_ACCEPTS_FOCUS);
    }

    if (pDesc)
    {
        while (pDesc->wId || pDesc->wStyle)
        {
            Add(new PegMenuButton(pDesc));
            pDesc++;
        }
    }
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegMenu::~PegMenu()
{

}


/*--------------------------------------------------------------------------*/
SIGNED PegMenu::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
        PositionButtons();
        PegThing::Message(Mesg);
        break;

    // The following two messages, PM_CURRENT and PM_NONCURRENT, only occur
    // if this menu is a pop-up menu, i.e. it was not spawned by a PegMenuBar

    case PM_CURRENT:
        PegThing::Message(Mesg);
        {
        PegMenuButton *pTarget = (PegMenuButton *) First();
        while(pTarget)
        {
            if (!(pTarget->Style() & BF_SEPERATOR) && (pTarget->Style() & AF_ENABLED))
            {
                break;
            }
            pTarget = (PegMenuButton *) pTarget->Next();
        }
        if (pTarget)
        {
            pTarget->Message(PM_POINTER_ENTER);
        }
        }
        break;

    case PM_NONCURRENT:
        PegThing::Message(Mesg);
        {
        PegMenuButton *pButton = (PegMenuButton *) First();
        while(pButton)
        {
            pButton->CloseMenu();
            pButton = (PegMenuButton *) pButton->Next();
        }
        }
        Parent()->Remove(this);
        break;

    case PM_KEY:
        #ifdef PEG_KEYBOARD_SUPPORT
        MenuKeyHandler(Mesg.iData);
        #endif
        break;

    default:
        PegThing::Message(Mesg);
        break;
    }
    return 0;
}

#ifndef PEG_RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegMenu::Draw(void)
{
    BeginDraw();

   #if (PEG_NUM_COLORS >= 4)
   
    PegColor Color(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    Rectangle(mReal, Color, 0);

    // outline myself:
    Color.uForeground = PCLR_HIGHLIGHT;
    Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wLeft + 1,
        mReal.wBottom, Color);
    Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wRight - 1,
        mReal.wTop + 1, Color);
    Color.uForeground = PCLR_LOWLIGHT;
    Line(mReal.wLeft + 1, mReal.wBottom - 1, mReal.wRight - 1,
        mReal.wBottom - 1, Color);
    Line(mReal.wRight - 1, mReal.wTop + 1, mReal.wRight - 1,
        mReal.wBottom - 1, Color);
    Color.uForeground = PCLR_SHADOW;
    Line(mReal.wLeft, mReal.wBottom, mReal.wRight,
        mReal.wBottom, Color);
    Line(mReal.wRight, mReal.wTop, mReal.wRight,
        mReal.wBottom, Color);

   #else
    // here for monochrome:

    PegColor Color(PCLR_SHADOW, muColors[PCI_NORMAL], CF_FILL);
    Rectangle(mReal, Color, 1);

   #endif
    PegThing::Draw();
    EndDraw();
}

#else   // here for RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegMenu::Draw(void)
{
    BeginDraw();

    if (NumColors() >= 4)
    {
	    PegColor Color(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
	    Rectangle(mReal, Color, 0);
	
	    // outline myself:
	    Color.uForeground = PCLR_HIGHLIGHT;
	    Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wLeft + 1,
	        mReal.wBottom, Color);
	    Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wRight - 1,
	        mReal.wTop + 1, Color);
	    Color.uForeground = PCLR_LOWLIGHT;
	    Line(mReal.wLeft + 1, mReal.wBottom - 1, mReal.wRight - 1,
	        mReal.wBottom - 1, Color);
	    Line(mReal.wRight - 1, mReal.wTop + 1, mReal.wRight - 1,
	        mReal.wBottom - 1, Color);
	    Color.uForeground = PCLR_SHADOW;
	    Line(mReal.wLeft, mReal.wBottom, mReal.wRight,
	        mReal.wBottom, Color);
	    Line(mReal.wRight, mReal.wTop, mReal.wRight,
	        mReal.wBottom, Color);
    }
    else
    {
        PegColor Color(PCLR_SHADOW, muColors[PCI_NORMAL], CF_FILL);
        Rectangle(mReal, Color, 1);
    }
    PegThing::Draw();
    EndDraw();
}

#endif  // RUNTIME_COLOR_CHECK if

/*--------------------------------------------------------------------------*/
PegRect PegMenu::GetMinSize(void)
{
    PegRect SizeRect;
    PegRect CurrentSize;
    SizeRect.Set(0, 0, 0, 0);

    PegMenuButton *pButton = (PegMenuButton *) First();

    while(pButton)
    {
        CurrentSize = pButton->GetMinSize(TYPE_MENU);
        if (CurrentSize.Width() > SizeRect.Width())
        {
            SizeRect.wRight += CurrentSize.Width() - SizeRect.Width();
        }
        SizeRect.wBottom += CurrentSize.Height();
        pButton = (PegMenuButton *) pButton->Next();
    }
    SizeRect.wBottom += 6;
    SizeRect.wRight += 6;
    return SizeRect;
}

/*--------------------------------------------------------------------------*/
PegMenuButton *PegMenu::FindButton(const PEGCHAR *Who)
{
    PegMenuButton *pTest = (PegMenuButton *) First();
    PegMenuButton *pFound;

    while(pTest)
    {
        if (pTest->DataGet())
        {
            if (!strcmp(pTest->DataGet(), Who))
            {
                return pTest;
            }
            else
            {
                pFound = pTest->FindButton(Who);
                if (pFound)
                {
                    return pFound;
                }
            }
        }
        pTest = (PegMenuButton *) pTest->Next();
    }

    return NULL;
}


/*--------------------------------------------------------------------------*/
void PegMenu::PositionButtons(void)
{
    PegRect Put;
    PegRect Current;
    Put.wLeft = mReal.wLeft + 2;
    Put.wRight = mReal.wRight - 2;
    Put.wBottom = mReal.wTop + 1;
    PegMessage NewMessage;

    // I have to figure out where my children go!

    PegMenuButton *pButton = (PegMenuButton *) First();

    while(pButton)
    {
        Put.wTop = Put.wBottom + 1;
        Current = pButton->GetMinSize(TYPE_MENU);
        Put.wBottom = Put.wTop + Current.Height() - 1;
        NewMessage.wType = PM_SIZE;
        NewMessage.Rect = Put;
        pButton->Message(NewMessage);
        pButton = (PegMenuButton *) pButton->Next();
    }
}


/*--------------------------------------------------------------------------*/
void PegMenu::CloseSiblings(PegMenuButton *NotMe)
{
    PegMenuButton *pButton = (PegMenuButton *) First();

    while(pButton)
    {
        if (pButton != NotMe)
        {
            pButton->CloseMenu();

            // If we're a popup menu, and NotMe is not the
            // first button, and we've just gotten focus,
            // the first button still thinks that it has 
            // the pointer (see PegMenu::Message-PM_SHOW)
            // because it never received a PM_POINTER_EXIT.
            // So, if there is another button on the menu that
            // thinks it has the pointer, send it a PM_POINTER_EXIT
            // message after it has closed it's submenu (it it had one).
            if ((pButton->Parent()->Type() == TYPE_MENU) && 
                (pButton->IsPointerOver()))
            {
                    pButton->Message(PM_POINTER_EXIT);
            }
        }
        pButton = (PegMenuButton *) pButton->Next();
    }
}


/*--------------------------------------------------------------------------*/
void PegMenu::SetOwner(PegThing *pOwner)
{
    PegMenuButton *pmb;
    PegThing *Child = First();
    
    while(Child)
    {
        if (Child->Type() == TYPE_MENU_BUTTON)
        {
            pmb = (PegMenuButton *) Child;
            pmb->SetOwner(pOwner);
        }
        Child = Child->Next();
    }
}


#ifdef PEG_KEYBOARD_SUPPORT

void PegMenu::MenuKeyHandler(SIGNED iKey)
{
    PegMenuButton *pButton1;
    PegMenuButton *pButton = MenuFindActiveButton((PegMenuButton *) First());

    if (!pButton)
    {
        return;
    }
        
    switch(iKey)
    {
    case PK_RIGHT:
        if (pButton->IsSubVisible())
        {
            MenuMoveToButton(pButton, pButton->GetSubMenu()->First());
        }
        break;

    case PK_LEFT:
        pButton1 = MenuFindOwnerButton(pButton, (PegMenuButton *) First());
        if (pButton1)
        {
            pButton->Message(PM_POINTER_EXIT);
            pButton1->Message(PM_POINTER_ENTER);
            pButton1->Message(PM_LBUTTONDOWN);
        }
        break;

    case PK_LNDN:
        if (MenuNextButton(pButton))
        {
            pButton->Message(PM_POINTER_EXIT);
            MenuNextButton(pButton)->Message(PM_POINTER_ENTER);
        }
        break;

    case PK_LNUP:
        if (MenuPreviousButton(pButton))
        {
            pButton->Message(PM_POINTER_EXIT);
            MenuPreviousButton(pButton)->Message(PM_POINTER_ENTER);
        }
        break;

    case PK_CR:
        pButton->Message(PM_LBUTTONDOWN);
        break;

    case PK_ESC:
        pButton->Message(PM_POINTER_EXIT);
        Parent()->Remove(this);
        if (pButton->GetOwner())
        {
            Presentation()->MoveFocusTree(pButton->GetOwner());
        }
        break;

    default:
        break;
    }
}

#endif

/******************         Peg Menu Button     *****************************/

/*--------------------------------------------------------------------------*/
PegMenuButton::PegMenuButton(PegMenuDescription *pDesc) :
    PegThing(pDesc->wId, pDesc->wStyle),
    PegTextThing(pDesc->wStyle, PEG_MENU_FONT)
{
    mReal.Set(0, 0, 100, 18);
    mClient.Set(0, 0, 100, 18);
    Type(TYPE_MENU_BUTTON);
    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;

    DataSet(pDesc->pText);
    mpOwner = NULL;

    if (pDesc->pSubMenu)
    {
        mpSubMenu = new PegMenu(pDesc->pSubMenu);
    }
    else
    {
        mpSubMenu = NULL;

        if (pDesc->wStyle & BF_DOTABLE)
        {
            SetSignals(SIGMASK(PSF_DOT_ON));
        }
        else
        {
            if (pDesc->wStyle & BF_CHECKABLE)
            {
                SetSignals(SIGMASK(PSF_CHECK_ON)|SIGMASK(PSF_CHECK_OFF));
            }
            else
            {
                if (pDesc->wStyle & AF_ENABLED)
                {
                    SetSignals(SIGMASK(PSF_CLICKED));
                }
                else
                {
                    RemoveStatus(PSF_SELECTABLE);
                }
            }
        }
    }
    AddStatus(PSF_SIZEABLE | PSF_NONCLIENT);
    RemoveStatus(PSF_ACCEPTS_FOCUS);

    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    muColors[PCI_SELECTED] = PCLR_ACTIVE_TITLE;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_STEXT] = PCLR_HIGH_TEXT;

    mbPointerOver = FALSE;
    mbSubVisible = FALSE;
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegMenuButton::~PegMenuButton()
{
    if (mpSubMenu && !mpSubMenu->StatusIs(PSF_VISIBLE))
    {
        Destroy(mpSubMenu);
    }
}

/*--------------------------------------------------------------------------*/
SIGNED PegMenuButton::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
        PegThing::Message(Mesg);
        break;

    case PM_LBUTTONDOWN:
        if (Parent()->Type() == TYPE_MENU_BAR)
        {
            PegMenuBar *pBar = (PegMenuBar *) Parent();
            if (pBar->InFlyoverMode())
            {
                pBar->SetFlyoverMode(FALSE);
            }
            else
            {
                pBar->SetFlyoverMode(TRUE);
            }
            Invalidate(mReal);
            Draw();
        }
        if (mpSubMenu)
        {
            if (!mbSubVisible)
            {
                if (Parent()->Type() == TYPE_MENU_BAR)
                {
                    AddSubMenu(TRUE);
                }
                else
                {
                    AddSubMenu(FALSE);
                }
            }
            // A touch support test was here to help in the case that
            // the touch screen is poorly debounced. However, it causes
            // a problem when the touch is working correctly. Better to
            // debounce correctly.

           //#if !defined(PEG_TOUCH_SUPPORT)
            else
            {
                CloseMenu();
            }
           //#endif
        }
        else
        {
            SendOwnerMessage();
        }
        break;

    case PM_LBUTTONUP:
        if (!mpSubMenu)
        {
            SendOwnerMessage();
        }
        break;

    case PM_HIDE:
        PegThing::Message(Mesg);
        mbPointerOver = FALSE;
        CloseMenu();
        break;

    case PM_POINTER_ENTER:
        mbPointerOver = TRUE;
        Invalidate(mReal);
        Draw();

        if (Parent()->Type() == TYPE_MENU_BAR)
        {
            PegMenuBar *pBar = (PegMenuBar *) Parent();            
            if (pBar->InFlyoverMode())
            {
                pBar->CloseSiblings(this);

                if (mpSubMenu && !mbSubVisible)
                {
                    AddSubMenu(TRUE);
                }
            }
        }
        else
        {
            PegMenu *pBar = (PegMenu *) Parent();
            pBar->CloseSiblings(this);

            if (mpSubMenu && !mbSubVisible)
            {
                AddSubMenu(FALSE);
            }
        }
        break;

    case PM_POINTER_EXIT:
        mbPointerOver = FALSE;

        if (!mbSubVisible)
        {
            Invalidate(mReal);
            Draw();
        }
        break;

   #ifdef PEG_KEYBOARD_SUPPORT
    case PM_KEY:
        break;
   #endif

    default:
        PegThing::Message(Mesg);
        break;
    }
    return 0;
}

#ifndef PEG_RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegMenuButton::Draw(void)
{
    BeginDraw();
    PegColor Color;
    PegPoint Put;

    if (mwStyle & BF_SEPERATOR)
    {
        int Center = (mReal.wTop + mReal.wBottom) / 2;
        Color.Set(PCLR_LOWLIGHT);
        Line(mReal.wLeft, Center, mReal.wRight, Center, Color);
        Color.Set(PCLR_HIGHLIGHT);
        Line(mReal.wLeft, Center + 1, mReal.wRight, Center + 1,
             Color);
    }
    else
    {
        Put.x = mReal.wLeft;
        if (Parent()->Type() == TYPE_MENU)
        {
            Put.x += MENU_CHECK_WIDTH * 2;
        }
        Put.y = mReal.wTop + 1;

        if (Parent()->Type() == TYPE_MENU)
        {
           #if (PEG_NUM_COLORS >= 4)
            if (mwStyle & AF_ENABLED)
            {
                if (mbPointerOver)
                {
                    Color.Set(muColors[PCI_STEXT],
                              muColors[PCI_SELECTED], CF_FILL);
                }
                else
                {
                    Color.Set(muColors[PCI_NTEXT],
                              muColors[PCI_NORMAL], CF_FILL);
                }
            }
            else
            {
                Color.Set(PCLR_LOWLIGHT, muColors[PCI_NORMAL], CF_FILL);
            }

            Rectangle(mReal, Color, 0);
           #else
            Color.Set(muColors[PCI_NTEXT],
                      muColors[PCI_NORMAL], CF_FILL);

            if (mbPointerOver && (mwStyle & AF_ENABLED))
            {
                Rectangle(mReal, Color, 2);
            }
            else
            {
                Rectangle(mReal, Color, 0);
            }
           #endif
        }
        else
        {
            // here if I am on the menu bar:

           #if (PEG_NUM_COLORS >= 4)

            Color.Set(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
            Rectangle(mReal, Color, 0);
            PegMenuBar *pBar = (PegMenuBar *) Parent();

            if (mbPointerOver && (mwStyle & AF_ENABLED) || mbSubVisible)
            {
                if (pBar->InFlyoverMode())
                {
                    Color.uForeground = PCLR_LOWLIGHT;
                }
                else
                {
                    Color.uForeground = PCLR_HIGHLIGHT;
                }
                Line(mReal.wLeft, mReal.wTop, mReal.wRight,
                    mReal.wTop, Color);
                Line(mReal.wLeft, mReal.wTop, mReal.wLeft,
                    mReal.wBottom, Color);

                if (pBar->InFlyoverMode())
                {
                    Color.uForeground = PCLR_HIGHLIGHT;
                }
                else
                {
                    Color.uForeground = PCLR_LOWLIGHT;
                }
                Line(mReal.wLeft + 1, mReal.wBottom,
                    mReal.wRight, mReal.wBottom, Color);
                Line(mReal.wRight, mReal.wTop, mReal.wRight,
                    mReal.wBottom, Color);
            }

            if (mwStyle & AF_ENABLED)
            {
                Color.uForeground = muColors[PCI_NTEXT];
            }
            else
            {
                Color.uForeground = PCLR_LOWLIGHT;
            }

           #else

            // here for monochrome
            
            Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);

            if (mbPointerOver && (mwStyle & AF_ENABLED) || mbSubVisible)
            {
                Rectangle(mReal, Color, 1);
            }
            else
            {
                Rectangle(mReal, Color, 0);
            }
           #endif
            Put.x += (mReal.Width() - TextWidth(mpText, mpFont)) / 2;
        }

        // finally put the text on top:

        Color.uFlags = CF_NONE;
        DrawText(Put, mpText, Color, mpFont, mwStrLen);
    }

    if (Parent()->Type() == TYPE_MENU && mpSubMenu)
    {
        Put.x = mReal.wRight - gbMoreBitmap.wWidth * 2;
        Put.y = mReal.wTop;
        Put.y += (mReal.Height() - gbMoreBitmap.wHeight) / 2;
        Bitmap(Put, &gbMoreBitmap);
    }
    else
    {
        if (mwStyle & BF_CHECKED)
        {
            // draw the check mark on my left:
            Put.x = mReal.wLeft + MENU_CHECK_WIDTH / 2;
            Put.y = mReal.wTop;
            Put.y += (mReal.Height() - gbCheckBitmap.wHeight) / 2;
            Bitmap(Put, &gbCheckBitmap);
        }
        else
        {
            if (mwStyle & BF_DOTTED)
            {
                Put.x = mReal.wLeft + MENU_CHECK_WIDTH / 2;
                Put.y = mReal.wTop;
                Put.y += (mReal.Height() - gbDotBitmap.wHeight) / 2;
                Bitmap(Put, &gbDotBitmap);
            }
        }
    }

   #if (PEG_NUM_COLORS == 2)

    if (!(mwStyle & AF_ENABLED))
    {
        Line(mReal.wLeft + 2, Put.y + mReal.Height() / 2,
             mReal.wRight - 2, Put.y + mReal.Height() / 2, Color);
    }
   #endif
    EndDraw();
}

#else   // here for runtime color checking

/*--------------------------------------------------------------------------*/
void PegMenuButton::Draw(void)
{
    BeginDraw();
    PegColor Color;
    PegPoint Put;

    if (mwStyle & BF_SEPERATOR)
    {
        int Center = (mReal.wTop + mReal.wBottom) / 2;
        Color.Set(PCLR_LOWLIGHT);
        Line(mReal.wLeft, Center, mReal.wRight, Center, Color);
        Color.Set(PCLR_HIGHLIGHT);
        Line(mReal.wLeft, Center + 1, mReal.wRight, Center + 1,
             Color);
    }
    else
    {
        Put.x = mReal.wLeft;
        if (Parent()->Type() == TYPE_MENU)
        {
            Put.x += MENU_CHECK_WIDTH * 2;
        }
        Put.y = mReal.wTop + 1;

        if (Parent()->Type() == TYPE_MENU)
        {
            if (NumColors() >= 4)
            {
	            if (mwStyle & AF_ENABLED)
	            {
	                if (mbPointerOver)
	                {
	                    Color.Set(muColors[PCI_STEXT],
                                  muColors[PCI_SELECTED], CF_FILL);
	                }
	                else
	                {
	                    Color.Set(muColors[PCI_NTEXT],
                                  muColors[PCI_NORMAL], CF_FILL);
	                }
	            }
	            else
	            {
	                Color.Set(PCLR_LOWLIGHT, muColors[PCI_NORMAL], CF_FILL);
	            }
	
	            Rectangle(mReal, Color, 0);
            }
            else
            {
                Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);

                if (mbPointerOver && (mwStyle & AF_ENABLED))
                {
                    Rectangle(mReal, Color, 2);
                }
                else
                {
                    Rectangle(mReal, Color, 0);
                }
            }
        }
        else
        {
            // here if I am on the menu bar:

            if (NumColors() >= 4)
            {
	            Color.Set(muColors[PCI_NORMAL],
                          muColors[PCI_NORMAL], CF_FILL);
	            Rectangle(mReal, Color, 0);
	            PegMenuBar *pBar = (PegMenuBar *) Parent();
	
	            if (mbPointerOver && (mwStyle & AF_ENABLED) || mbSubVisible)
	            {
	                if (pBar->InFlyoverMode())
	                {
	                    Color.uForeground = PCLR_LOWLIGHT;
	                }
	                else
	                {
	                    Color.uForeground = PCLR_HIGHLIGHT;
	                }
	                Line(mReal.wLeft, mReal.wTop, mReal.wRight,
	                    mReal.wTop, Color);
	                Line(mReal.wLeft, mReal.wTop, mReal.wLeft,
	                    mReal.wBottom, Color);
	
	                if (pBar->InFlyoverMode())
	                {
	                    Color.uForeground = PCLR_HIGHLIGHT;
	                }
	                else
	                {
	                    Color.uForeground = PCLR_LOWLIGHT;
	                }
	                Line(mReal.wLeft + 1, mReal.wBottom,
	                    mReal.wRight, mReal.wBottom, Color);
	                Line(mReal.wRight, mReal.wTop, mReal.wRight,
	                    mReal.wBottom, Color);
	            }
	
	            if (mwStyle & AF_ENABLED)
	            {
	                Color.uForeground = muColors[PCI_NTEXT];
	            }
	            else
	            {
	                Color.uForeground = PCLR_LOWLIGHT;
	            }
            }
            else
            {
	            // here for monochrome
	            
	            Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
	
	            if (mbPointerOver && (mwStyle & AF_ENABLED) || mbSubVisible)
	            {
	                Rectangle(mReal, Color, 1);
	            }
	            else
	            {
	                Rectangle(mReal, Color, 0);
	            }
            }
            Put.x += (mReal.Width() - TextWidth(mpText, mpFont)) / 2;
        }

        // finally put the text on top:

        Color.uFlags = CF_NONE;
        DrawText(Put, mpText, Color, mpFont, mwStrLen);
    }

    if (Parent()->Type() == TYPE_MENU && mpSubMenu)
    {
        Put.x = mReal.wRight - gbMoreBitmap.wWidth * 2;
        Put.y = mReal.wTop;
        Put.y += (mReal.Height() - gbMoreBitmap.wHeight) / 2;
        Bitmap(Put, &gbMoreBitmap);
    }
    else
    {
        if (mwStyle & BF_CHECKED)
        {
            // draw the check mark on my left:
            Put.x = mReal.wLeft + MENU_CHECK_WIDTH / 2;
            Put.y = mReal.wTop;
            Put.y += (mReal.Height() - gbCheckBitmap.wHeight) / 2;
            Bitmap(Put, &gbCheckBitmap);
        }
        else
        {
            if (mwStyle & BF_DOTTED)
            {
                Put.x = mReal.wLeft + MENU_CHECK_WIDTH / 2;
                Put.y = mReal.wTop;
                Put.y += (mReal.Height() - gbDotBitmap.wHeight) / 2;
                Bitmap(Put, &gbDotBitmap);
            }
        }
    }

    if (NumColors() < 4)
    {
        if (!(mwStyle & AF_ENABLED))
        {
            Line(mReal.wLeft + 2, Put.y + mReal.Height() / 2,
                 mReal.wRight - 2, Put.y + mReal.Height() / 2, Color);
        }
    }
    EndDraw();
}

#endif  // runtime color checking if

/*--------------------------------------------------------------------------*/
void PegMenuButton::AddSubMenu(BOOL bVertical)
{
    PegMessage NewMessage;
    NewMessage.Rect = mpSubMenu->GetMinSize();

    if (bVertical)
    {
        NewMessage.Rect.MoveTo(mReal.wLeft, mReal.wBottom + 2);
    }
    else
    {
        NewMessage.Rect.MoveTo(mReal.wRight, mReal.wTop - 4);
    }

    // do some quick vertical positioning checks:

    if (NewMessage.Rect.wBottom >= Presentation()->mReal.wBottom)
    {
        if (bVertical)
        {
            if (NewMessage.Rect.Height() < mReal.wTop - 1)
            {
                NewMessage.Rect.Shift(0,
                    -(mReal.Height() + NewMessage.Rect.Height() + 2));
            }
        }
        else
        {
            NewMessage.Rect.Shift(0,
                Presentation()->mReal.wBottom - NewMessage.Rect.wBottom - 1);
        }
    }

    // do some quick horizontal positioning checks:

    if (NewMessage.Rect.wRight > Presentation()->mReal.wRight)
    {
        if (bVertical)
        {
            NewMessage.Rect.Shift(Presentation()->mReal.wRight -
                NewMessage.Rect.wRight, 0);
        }
        else
        {
            NewMessage.Rect.Shift(
            -(mReal.Width() + NewMessage.Rect.Width() + 1), 0);
        }
    }

    NewMessage.wType = PM_SIZE;
    mpSubMenu->Message(NewMessage);
    Presentation()->Add(mpSubMenu);
    mbSubVisible = TRUE;
}

/*--------------------------------------------------------------------------*/
void PegMenuButton::SendOwnerMessage(void)
{
    if (mpSubMenu || !mpOwner || Parent()->Type() == TYPE_MENU_BAR)
    {
        return;
    }
    Presentation()->MoveFocusTree(mpOwner);
            
    if (mwStyle & BF_CHECKABLE)
    {
        if (mwStyle & BF_CHECKED)
        {
            SetChecked(FALSE);
        }
        else
        {
            SetChecked(TRUE);
        }
    }
    else
    {
        if (mwStyle & BF_DOTABLE)
        {
            RemoveSiblingDots();
            SetDotted(TRUE);
        }
        else
        {
            if (Id())
            {
                PegMessage NewMessage(mpOwner, PEG_SIGNAL(Id(), PSF_CLICKED));
                NewMessage.iData = Id();
                NewMessage.pSource = this;
                MessageQueue()->Push(NewMessage);
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
PegRect PegMenuButton::GetMinSize(UCHAR uType)
{
    PegRect Return;
    Return.wTop = 0;
    Return.wLeft = 0;
    Return.wRight = TextWidth(mpText, mpFont) + 8;

    if (uType == TYPE_MENU)
    {
         Return.wRight += MENU_CHECK_WIDTH * 4;
    }

    if (!(mwStyle & BF_SEPERATOR))
    {
        Return.wBottom = TextHeight(mpText, mpFont) + 2;
    }
    else
    {
        Return.wBottom = 6;
    }
    return Return;
}


/*--------------------------------------------------------------------------*/
void PegMenuButton::RemoveSiblingDots(void)
{
    PegMenuButton *Look = (PegMenuButton *) Parent()->First();
    PegMenuButton *Start = Look;

    while(Look != this)
    {
        if (Look->IsSeperator())
        {
            Start = Look;
        }
        Look = (PegMenuButton *) Look->Next();
    }

    Look = (PegMenuButton *) Look->Next();

    while(Look)
    {
        if (Look->IsSeperator())
        {
            break;
        }
        Look = (PegMenuButton *) Look->Next();
    }

    while (Start != Look)
    {
        if (Start != this)
        {
            Start->SetDotted(FALSE);
        }
        Start = (PegMenuButton *) Start->Next();
    }

}

/*--------------------------------------------------------------------------*/
void PegMenuButton::SetDotted(BOOL State)
{
    if (State)
    {
        RemoveSiblingDots();
        mwStyle |= BF_DOTTED;
    }
    else
    {
        mwStyle &= ~BF_DOTTED;
    }
    if (Id())
    {
        PegMessage NewMessage;
        NewMessage.pTarget = mpOwner;
        NewMessage.pSource = this;
        NewMessage.iData = Id();
        NewMessage.wType = 0;

        if (mwStyle & BF_DOTTED)
        {
            if (GetSignals() & SIGMASK(PSF_DOT_ON))
            {
                NewMessage.wType = PEG_SIGNAL(Id(), PSF_DOT_ON);
            }
        }
        else
        {
            if (GetSignals() & SIGMASK(PSF_DOT_OFF))
            {
                NewMessage.wType = PEG_SIGNAL(Id(), PSF_DOT_OFF);
            }
        }

        if (NewMessage.wType)
        {
            MessageQueue()->Push(NewMessage);
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegMenuButton::SetChecked(BOOL State)
{
    if (State)
    {
        mwStyle |= BF_CHECKED;
    }
    else
    {
        mwStyle &= ~BF_CHECKED;
    }

    if (Id())
    {
        PegMessage NewMessage;
        NewMessage.pTarget = mpOwner;
        NewMessage.pSource = this;
        NewMessage.iData = Id();
        NewMessage.wType = 0;

        if (mwStyle & BF_CHECKED)
        {
            if (GetSignals() & SIGMASK(PSF_CHECK_ON))
            {
                NewMessage.wType = PEG_SIGNAL(Id(), PSF_CHECK_ON);
            }
        }
        else
        {
            if (GetSignals() & SIGMASK(PSF_CHECK_OFF))
            {
                NewMessage.wType = PEG_SIGNAL(Id(), PSF_CHECK_OFF);
            }
        }

        if (NewMessage.wType)
        {
            MessageQueue()->Push(NewMessage);
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegMenuButton::SetEnabled(BOOL State)
{
    if (State)
    {
        mwStyle |= AF_ENABLED;
        AddStatus(PSF_SELECTABLE);
    }
    else
    {
        mwStyle &= ~AF_ENABLED;
        RemoveStatus(PSF_SELECTABLE);
    }
}


/*--------------------------------------------------------------------------*/
void PegMenuButton::CloseMenu(void)
{
    if (mbSubVisible)
    {
        mbSubVisible = FALSE;
        if (mpSubMenu->Parent())
        {
            mpSubMenu->Parent()->Remove(mpSubMenu);
            Invalidate(mReal);
            Draw();
        }
    }
}

/*--------------------------------------------------------------------------*/
PegMenuButton *PegMenuButton::FindButton(const PEGCHAR *Who)
{
    if (!mpSubMenu)
    {
        return NULL;
    }

    return (mpSubMenu->FindButton(Who));
}

/*--------------------------------------------------------------------------*/
PegThing *PegMenuButton::Find(WORD wId, BOOL bRecursive)
{
    if (!mpSubMenu)
    {
        return (NULL);
    }
    return(mpSubMenu->Find(wId, bRecursive));
}

/*--------------------------------------------------------------------------*/
void PegMenuButton::SetSubMenu(PegMenu *pMenu)
{
    if (mpSubMenu)
    {
        Destroy(mpSubMenu);
    }
    mpSubMenu = pMenu;
    mpSubMenu->SetOwner(mpOwner);
}

/*--------------------------------------------------------------------------*/
void PegMenuButton::SetOwner(PegThing *pOwner)
{
    mpOwner = pOwner;

    if (mpSubMenu)
    {
        mpSubMenu->SetOwner(pOwner);
    }
}


// End of file
