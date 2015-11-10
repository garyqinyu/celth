/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: SuperFuction
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifndef _SUPERFUNCTION_HPP
#define _SUPERFUNCTION_HPP

class ToolTip;
class TVBitmapButton;
class TVHelpWnd;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SuperFuction : public PegWindow
{
    public:
        SuperFuction(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        enum ChildIds {
            ID_SupFunctionMenu_Button_CA = 1,
            ID_SupFunctionMenu_Button_CC,
            ID_SupFunctionMenu_Button_ParentControl,
            ID_SupFunctionMenu_Button_EPG,
        };
		void SetToolTipWnd(ToolTip * pToolTip);
		void SetTVHelpWnd(TVHelpWnd * pTVHelpWnd);
		void ProcessUpDownKey();

    private:
        TVBitmapButton *m_pSupFunctionMenuButtonCA;
        TVBitmapButton *m_pSupFunctionMenuButtonCC;
        TVBitmapButton *m_pSupFunctionMenuButtonParentControl;
        TVBitmapButton *m_pSupFunctionMenuButtonEPG;

		ToolTip * m_pToolTip;
		TVHelpWnd * m_pTVHelpWnd;
		int m_nCurrentSubWndID;
};

#endif /*_SUPERFUNCTION_HPP*/