/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: ToolTip
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifndef _TOOLTIP_HPP
#define _TOOLTIP_HPP

class TVPrompt;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

class ToolTip : public PegWindow
{
    public:
        ToolTip(SIGNED iLeft, SIGNED iTop, const PEGCHAR *Text);
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        enum ChildIds {
        };

		void SetTipString(const PEGCHAR * pcTip);
		SIGNED GetTextWidth(const PEGCHAR * pcTip);
		void DeleteTipGraphics();
		void SetFont(PegFont * pFont);

    private:
		TVPrompt * m_pPromptTip; //tip container
		PegFont * m_pFont;

		PegCapture m_Capture;    //capture previous graphics for restore soon after
};


#endif /*_TOOLTIP_HPP*/