/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: PictureMenu
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


#ifndef _PICTUREMENU_HPP
#define _PICTUREMENU_HPP
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class ToolTip;
class TVBitmapButton;
class TVHelpWnd;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PictureMenu : public PegWindow
{
    public:
        PictureMenu(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        enum ChildIds {
            ID_PictureMenu_Button_ColorTemp =1,
            ID_PictureMenu_Button_Color,
            ID_PictureMenu_Button_Sharp,
            ID_PictureMenu_Button_Tint,
            ID_PictureMenu_Button_Contrast,
            ID_PictureMenu_Button_Brightness,
        };
		void SetToolTipWnd(ToolTip * pToolTip);
		void SetTVHelpWnd(TVHelpWnd * pTVHelpWnd);
		void ProcessUpDownKey();

    private:
        TVBitmapButton *m_pPictureMenuButtonColorTemp;
        TVBitmapButton *m_pPictureMenuButtonColor;
        TVBitmapButton *m_pPictureMenuButtonSharp;
        TVBitmapButton *m_pPictureMenuButtonTint;
        TVBitmapButton *m_pPictureMenuButtonContrast;
        TVBitmapButton *m_pPictuerMenuButtonBright;

		ToolTip * m_pToolTip;
		TVHelpWnd * m_pTVHelpWnd;
		int m_nCurrentSubWndID;
};

#endif /*_PICTUREMENU_HPP*/