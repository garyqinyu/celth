/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// PegTerminalWin - Window derived from PegTextBox that emulates a 
//  dumb terminal window.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2000 Swell Software, Inc.
// All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software, Inc.
// is strictly prohibited.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "string.h"
#include "ctype.h"
#include "peg.hpp"

/*--------------------------------------------------------------------------*/
PegTerminalWin::PegTerminalWin(const PegRect &Rect, WORD wId, WORD wStyle,
    PEGCHAR *Text, SIGNED iMaxLines ) :
    PegEditBox(Rect, wId, wStyle, Text, iMaxLines)
{
    Type(TYPE_TERMINAL_WIN);
    mpComPrompt = NULL;
    mpCommand = NULL;
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegTerminalWin::~PegTerminalWin()
{
    if (mpComPrompt)
    {
        delete mpComPrompt;
    }
    if (mpCommand)
    {
        delete mpCommand;
    }
}

/*--------------------------------------------------------------------------*/
SIGNED PegTerminalWin::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_KEY:
        if (Mesg.iData == PK_CR)
        {
            if (Id() && Parent())
            {
                SendCommandToParent();
            }
        }
        else
        {
            PegEditBox::Message(Mesg);
        }
        break;

    default:
        return(PegEditBox::Message(Mesg));
    }
    return (0);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTerminalWin::SendCommandToParent(void)
{
    PegMessage NewMessage(Parent(), PEG_SIGNAL(Id(), PSF_COMMAND));
    NewMessage.pSource = this;

    WORD wLine = mCursor.y;
    WORD wPromptLength = 0;

    if (mpComPrompt)
    {
        // Back up until the line starts with a command prompt:

        wPromptLength = strlen(mpComPrompt);
        PEGCHAR *cBuf = new PEGCHAR[wPromptLength + 2];
        
        while(wLine)
        {
            strncpy(cBuf, GetLineStart(wLine, NULL), wPromptLength);
            cBuf[wPromptLength] = '\0';
            if (!strcmp(cBuf, mpComPrompt))
            {
                break;
            }
            wLine--;
        }
        delete cBuf;
    }

    WORD wStart = GetLineIndex(wLine) + wPromptLength;
    WORD wEnd = GetLineIndex(mCursor.y) + mCursor.x;

    if (mpCommand)
    {
        delete mpCommand;
        mpCommand = NULL;
    }

    if (wEnd > wStart && wEnd != 0xffff)
    {
        mpCommand = new PEGCHAR[wEnd - wStart + 1];
        strncpy(mpCommand, DataGet() + wStart, wEnd - wStart);
        mpCommand[wEnd - wStart] = '\0';   
    }
    NewMessage.pData = mpCommand;
    Parent()->Message(NewMessage);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTerminalWin::SetPrompt(PEGCHAR *pPrompt)
{
    if (mpComPrompt)
    {
        delete mpComPrompt;
        mpComPrompt = NULL;
    }

    if (pPrompt)
    {
        mpComPrompt = new PEGCHAR[strlen(pPrompt) + 1];
        strcpy(mpComPrompt, pPrompt);

        if (!DataGet())
        {
            DataSet(mpComPrompt);
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTerminalWin::Prompt(void)
{
    Append(mpComPrompt);
}

// End of file
