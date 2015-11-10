/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: HelpWnd
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _HELPWND_HPP
#define _HELPWND_HPP


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class TVHelpWnd : public PegWindow
{
    public:
        TVHelpWnd(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
		~TVHelpWnd();
        void Draw(void);
        enum ChildIds {
            ID_Text_HelpString = 1,
        };

    void ShowHelpString(const PEGCHAR * pcHelpString, int iType = 0);
	void SetFont(PegFont * pFont);
    private:
        PegTextBox *m_pTextHelpString;
		PEGCHAR *   m_pcText;
		PegPrompt * m_pPromptTitle;
};

#endif /*_HELPWND_HPP*/