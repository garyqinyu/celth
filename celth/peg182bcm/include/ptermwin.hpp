/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ptermwin.hpp - Window derived from PegTextBox that emulates a 
//  dumb terminal window.
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

#ifndef _PEGTERMINAL_
#define _PEGTERMINAL_

class PegTerminalWin : public PegEditBox
{
    public:
        PegTerminalWin(const PegRect &Rect, WORD wId, 
            WORD wStyle = FF_RECESSED|EF_EDIT|EF_WRAP,
            PEGCHAR *Text = NULL, SIGNED iMaxLines = 25);
        ~PegTerminalWin();
        SIGNED Message(const PegMessage &Mesg);
        void SetPrompt(PEGCHAR *pPrompt);
        void Prompt(void);

    protected:
        void SendCommandToParent(void);
        PEGCHAR *mpComPrompt;
        PEGCHAR *mpCommand;
};

#endif

// End of file


