/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: FactoryMenu
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifndef _FACTORYMENU_HPP
#define _FACTORYMENU_HPP

class ToolTip;
class TVBitmapButton;
class TVHelpWnd;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class FactoryMenu : public PegWindow
{
    public:
        FactoryMenu(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        enum ChildIds {
            ID_FactoryMenu_Button_Geometry = 1,
            ID_FactoryMenu_Button_WhiteBalance,
            ID_FactoryMenu_Button_Video,
        };
		void SetToolTipWnd(ToolTip * pToolTip);
		void SetTVHelpWnd(TVHelpWnd * pTVHelpWnd);
		void ProcessUpDownKey();
		void DisplayChildWindow(int iSubWndID = 1);

    private:
        TVBitmapButton *m_pFactoryMenuButtonGeometry;
        TVBitmapButton *m_pFactoryEMenuButtonWhiteBalance;
        TVBitmapButton *m_pFactoryMenuButtonVideo;

		ToolTip * m_pToolTip;
		TVHelpWnd * m_pTVHelpWnd;
		int m_nCurrentSubWndID;
};
#endif /*_FACTORYMENU_HPP*/