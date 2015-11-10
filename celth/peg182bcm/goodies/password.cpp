/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// password.cpp - password entry editor, derived from PegString.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-1999 Swell Software 
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


#include "peg.hpp"
#include "password.hpp"

/*--------------------------------------------------------------------------*/
PWString::PWString(const PegRect &Rect, const PEGCHAR *Text,
            WORD wId, WORD wStyle, SIGNED iLen, PEGCHAR EchoChar)
            : PegString(Rect, NULL, wId, wStyle, iLen)
{
    mEchoChar = EchoChar;
    InitializeText(Text);
}
        
/*--------------------------------------------------------------------------*/
PWString::PWString(SIGNED iLeft, SIGNED iTop, SIGNED iWidth,
            const PEGCHAR *Text, WORD wId, WORD wStyle, SIGNED iLen,
            PEGCHAR EchoChar)
            : PegString(iLeft, iTop, iWidth, NULL, wId, wStyle, iLen)
{
    mEchoChar = EchoChar;
    InitializeText(Text);
}

/*--------------------------------------------------------------------------*/
PWString::PWString(SIGNED iLeft, SIGNED iTop, const PEGCHAR *Text, WORD wId,
             WORD wStyle, SIGNED iLen, PEGCHAR EchoChar)
            : PegString(iLeft, iTop, NULL, wId, wStyle, iLen)
{
    mEchoChar = EchoChar;
    InitializeText(Text);
}

/*--------------------------------------------------------------------------*/
SIGNED PWString::Message(const PegMessage &Mesg)
{
    PegMessage KeyMesg;
    PEGCHAR *cGet = DataGet();
    PEGCHAR *cBuff;

    switch(Mesg.wType)
    {
    case PM_KEY:
        KeyMesg = Mesg;

        switch(Mesg.iData)
        {
        case PK_BACKSPACE:
            if (State.mbEditMode)
            {
                if (miCursor)
                {
                    strcpy(mText + miCursor - 1, mText + miCursor);            
                    PegString::Message(Mesg);
                }
            }
            break;

        case PK_DELETE:
            if (cGet)
            {
                if (State.mbEditMode)
                {
                    if (miCursor < (SIGNED) strlen(cGet))
                    {
                        strcpy(mText + miCursor, mText + miCursor + 1);
                        PegString::Message(Mesg);
                    }
                }
                else
                {
                    if (State.mbFullSelect)
                    {
                        mText[0] = 0;
                        PegString::Message(Mesg);
                    }
                }
            }
            break;

        default:
            if (Mesg.iData >= 0x20 && Mesg.iData <= 0x7e)
            {
                /* for ASCII input, enter the real character
                   in my private string, and pass the EchoChar
                   to the PegString
                */

                if (cGet)
                {
                    cBuff = new PEGCHAR[strlen(mText) + 2];
                    strcpy(cBuff, mText);
                    *(cBuff + miCursor) = (PEGCHAR) Mesg.iData;
                    strcpy(cBuff + miCursor + 1, mText + miCursor);
                    strcpy(mText, cBuff);
                }
                else
                {
                    mText[0] = (PEGCHAR) Mesg.iData;
                    mText[1] = 0;
                }

                KeyMesg.iData = mEchoChar;
            }
            PegString::Message(KeyMesg);
            break;
        }
        break;

    case PM_CURRENT:

        /* Intercept PM_CURRENT to make the cursor appear when the password
           string is first selected, instead of fully selecting the text.
        */

        PegThing::Message(Mesg);
        miCursor = 0;
        State.mbMarked = 0;
        State.mbFullSelect = 0;
        State.mbEditMode = 1;
        State.mbChanged = 0;
        mCursorPos.x = mClient.wLeft + 1;
        mCursorPos.y = mClient.wTop;
        Invalidate(mClient);
        Draw();
        break;

    default:
        return PegString::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR *PWString::DataGet(void)
{
    if (mText[0])
    {
        return mText;
    }
    return NULL;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PWString::InitializeText(const PEGCHAR *Text)
{
    const PEGCHAR *pt = Text;
    SIGNED Index = 0;

    if (Text)
    {
        while(*pt)
        {
            mText[Index] = mEchoChar;
            Index++;
            pt++;
        }
        mText[Index] = 0;
        DataSet(mText);
        strcpy(mText, Text);
    }
    else
    {
        mText[0] = 0;
    }
}

