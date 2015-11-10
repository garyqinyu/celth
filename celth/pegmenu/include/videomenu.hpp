/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: videomenu
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _VIDEO_HPP
#define _VIDEO_HPP
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class TVBitmapButton;
class ToolTip;
class TVHelpWnd;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class VideoMenu : public PegWindow
{
    public:
        VideoMenu(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        enum ChildIds {
            ID_VideoMenu_Button_AutoFlesh = 1,
            ID_VideoMenu_Button_Peaking,
            ID_VideoMenu_Button_DCI,
            ID_VideoMenu_Button_EdgeReplace,
            ID_VideoMenu_Button_GreenBoost,
            ID_VideoMenu_Button_Saturation,
            ID_VideoMenu_Button_Tint
        };
		void SetToolTipWnd(ToolTip * pToolTip);
		void SetTVHelpWnd(TVHelpWnd * pTVHelpWnd);
		void ProcessUpDownKey();

    private:
        TVBitmapButton *m_pVideoMenuButtonAutoFlesh;
        TVBitmapButton *m_pVideoMenuButtonPeaking;
        TVBitmapButton *m_pVideoMenuButtonDCI;
        TVBitmapButton *m_pVideoMenuButtonEdgeReplace;
        TVBitmapButton *m_pVideoMenuButtonGreenBoost;
        TVBitmapButton *m_pVideoMenuButtonSaturation;
        TVBitmapButton *m_pVideoMenuButtonTint;

		ToolTip * m_pToolTip;
		TVHelpWnd * m_pTVHelpWnd;
		int m_nCurrentSubWndID;
};
#endif /*_VIDEO_HPP*/