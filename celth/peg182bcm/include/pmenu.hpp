/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pmenu.hpp - Definition of PegMenu, PegMenu, and PegMenuButton classes.
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

#ifndef _PEGMENU_
#define _PEGMENU_

#define MENU_CHECK_WIDTH 8

/*--------------------------------------------------------------------------*/
struct PegMenuDescription
{
    const PEGCHAR *pText;
    WORD wId;
    WORD wStyle;
    PegMenuDescription *pSubMenu;
};

class PegMenuButton;        // forward reference

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegMenu : public PegThing
{
    public:
        PegMenu(PegMenuDescription *pDesc, BOOL bPopup = FALSE);
        virtual ~PegMenu();
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void Draw(void);
        PegRect GetMinSize(void);
        void CloseSiblings(PegMenuButton *NotMe);
        PegMenuButton *FindButton(const PEGCHAR *Who);
        void SetOwner(PegThing *Who);

       #ifdef PEG_KEYBOARD_SUPPORT
        virtual void MenuKeyHandler(SIGNED iKey);
       #endif

    protected:
        void PositionButtons(void);
};




/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegMenuBar : public PegThing
{
    public:
        PegMenuBar(PegMenuDescription *pDesc);
        virtual ~PegMenuBar();
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void Draw(void);
        virtual void SetFlyoverMode(BOOL bState) {mbFlyoverMode = bState;}
        virtual BOOL InFlyoverMode(void) {return mbFlyoverMode;}
        virtual void CloseSiblings(PegMenuButton *NotMe);
        PegMenuButton *FindButton(const PEGCHAR *Who);

    protected:
        virtual void SizeToParent(void);
        virtual void PositionButtons(void);

       #ifdef PEG_KEYBOARD_SUPPORT
        virtual void MenuKeyHandler(SIGNED iKey);
       #endif

        BOOL mbFlyoverMode;
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegMenuButton : public PegThing, public PegTextThing
{
    friend class PegMenuBar;

    public:
        PegMenuButton(PegMenuDescription *pDesc);
        virtual ~PegMenuButton();
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void Draw(void);
        virtual PegRect GetMinSize(UCHAR uType);
        virtual void CloseMenu(void);
        virtual void SetDotted(BOOL State);
        virtual void SetChecked(BOOL State);
        virtual void SetEnabled(BOOL State);
        PegMenuButton *FindButton(const PEGCHAR *Who);
        PegThing *Find(WORD wId, BOOL bRecursive = TRUE);
        virtual void SetOwner(PegThing *Who);
        PegThing *GetOwner(void) {return mpOwner;}
        PegMenu *GetSubMenu(void) {return mpSubMenu;}
        void SetSubMenu(PegMenu *pMenu);
        virtual BOOL IsSeperator( )
        {
            if (mwStyle & BF_SEPERATOR)
            {
                return TRUE;
            }
            return FALSE;
        }

        virtual BOOL IsChecked(void)
        {
            if (mwStyle & BF_CHECKED)
            {
                return TRUE;
            }
            return FALSE;
        }

        virtual BOOL IsDotted(void)
        {
            if (mwStyle & BF_DOTTED)
            {
                return TRUE;
            }
            return FALSE;
        }

        BOOL IsSubVisible(void) {return mbSubVisible;}
        BOOL IsPointerOver(void) {return mbPointerOver;}

    protected:
        void AddSubMenu(BOOL bVertical);
        void RemoveSiblingDots(void);
        void SendOwnerMessage(void);

        PegMenu *mpSubMenu;
        PegThing *mpOwner;
        BOOL mbSubVisible;
        BOOL mbPointerOver;
};

#endif

