/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: SupSettingMenu
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifndef _SUPERSETMENU_HPP
#define _SUPERSETMENU_HPP

class ToolTip;
class TVBitmapButton;
class TVHelpWnd;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SupSettingMenu : public PegWindow
{
    public:
        SupSettingMenu(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        enum ChildIds {
			ID_SupSetMenu_Button_Help = 1,
			ID_SupSetMenu_Button_Input,
            ID_SupSetMenu_Button_Zoom,
        };

		void SetToolTipWnd(ToolTip * pToolTip);
		void SetTVHelpWnd(TVHelpWnd * pTVHelpWnd);
		void ProcessUpDownKey();

    private:
        TVBitmapButton *m_pSupSetMenuButtonHelp;
        TVBitmapButton *m_pSupsetMenuButtonInput;
        TVBitmapButton *m_pSupSetMenuButtonZoom;

		ToolTip * m_pToolTip;
		TVHelpWnd * m_pTVHelpWnd;
		int m_nCurrentSubWndID;
};

#endif /*_SUPERSETMENU_HPP*/