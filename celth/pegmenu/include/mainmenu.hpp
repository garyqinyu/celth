/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: MainMenu
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _MAINMENU_HPP
#define _MAINMENU_HPP

class ToolTip;
class TVBitmapButton;
class TVHelpWnd;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class MainMenu : public PegWindow
{
    public:
        MainMenu(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        enum ChildIds {
			ID_MainMenu_Button_FactorySetting = 0,
			ID_MainMenu_Buttion_Picture,
			ID_MainMenu_Button_Sound,
			ID_MainMenu_Button_SupSetting,
			ID_MainMenu_Button_SupFunction,
            
        };

		void SetToolTipWnd(ToolTip * pToolTip);
		void ProcessUpDownKey();
		void DisplayChildWindow(int iSubWndID = 1);
		void SetHelpWnd(TVHelpWnd * pTVHelpWnd);

    private:
        TVBitmapButton *m_pMainMenuButtonFactorySetting;
        TVBitmapButton *m_pMainMenuButton_SupFunction;
        TVBitmapButton *m_pMainMenuButtonSupSetting;
        TVBitmapButton *m_pMainMenuButtonSound;
        TVBitmapButton *m_pMainMenuButtonPicture;

		ToolTip * m_pToolTip;
		int m_nCurrentSubWndID;
		TVHelpWnd * m_pTVHelpWnd;
};

/*extern void* PegAppInitialize(PegPresentationManager *pPresent);*/

#endif /*_MAINMENU_HPP*/
