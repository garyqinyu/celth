/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// password.hpp - Password entry field.
//
// Author: Kenneth G. Maxwell
//
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


#ifndef _PEGPASSWD_
#define _PEGPASSWD_

#define MAX_PASSWORD   120

class PWString : public PegString
{
    public:
        PWString(const PegRect &Rect, const PEGCHAR *Text = NULL,
            WORD wId = 0, WORD wStyle = FF_RECESSED|AF_ENABLED|EF_EDIT,
            SIGNED iLen = MAX_PASSWORD - 1, PEGCHAR EchoChar = '*');
        
        PWString(SIGNED iLeft, SIGNED iTop, SIGNED iWidth,
            const PEGCHAR *Text = NULL, WORD wId = 0, 
            WORD wStyle = FF_RECESSED|AF_ENABLED|EF_EDIT,
            SIGNED iLen = MAX_PASSWORD - 1, PEGCHAR EchoChar = '*');

        PWString(SIGNED iLeft, SIGNED iTop, const PEGCHAR *Text = NULL,
            WORD wId = 0, WORD wStyle = FF_RECESSED|AF_ENABLED|EF_EDIT,
            SIGNED iLen = MAX_PASSWORD - 1, PEGCHAR EchoChar = '*');

        SIGNED Message(const PegMessage &Mesg);
        PEGCHAR *DataGet(void);

    private:

        void InitializeText(const PEGCHAR *Text);

        PEGCHAR mEchoChar;
        PEGCHAR mText[120];     // max password is 119 chars !!!!

};

#endif






