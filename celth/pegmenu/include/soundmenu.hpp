/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: SoundMenu
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _SOUNDMENU_HPP
#define _SOUNDMENU_HPP
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class TVBitmapButton;
class ToolTip;
class TVHelpWnd;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SoundMenu : public PegWindow
{
    public:
        SoundMenu(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        enum ChildIds {
		ID_SoundMenu_Button_Mute = 1,
			ID_SoundMenu_Button_Volume
        };
		void SetToolTipWnd(ToolTip * pToolTip);
		void SetTVHelpWnd(TVHelpWnd * pTVHelpWnd);
		void ProcessUpDownKey();

    private:

        TVBitmapButton *m_pSoundMenuButtonVolume;
        TVBitmapButton *m_pSoundMenuButtonMute;

		ToolTip * m_pToolTip;
		TVHelpWnd * m_pTVHelpWnd;
		int m_nCurrentSubWndID;
};
#endif /*_SOUNDMENU_HPP*/