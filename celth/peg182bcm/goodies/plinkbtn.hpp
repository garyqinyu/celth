/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plinkbtn.hpp - A transparent text button that changes color after it
//      has been selected.
//
// Author: Kenneth G. Maxwell
//
// Version: 1.04
//
// Copyright (c) 1997-1998 Swell Software 
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

enum LinkMessage {
    LM_OVERLINK = FIRST_USER_MESSAGE + 1000,
    LM_EXITLINK
};

class PegLinkButton : public PegTextButton
{
    public:
        /*--------------------------------------------------------*/
        // Provide all of the same constructors as PegTextButton.
        // Some of these may be removed if they are not used:
        /*--------------------------------------------------------*/
        PegLinkButton(PegRect &Rect, char *Text, WORD wId);
        PegLinkButton(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, char *Text,
            WORD wId);
        PegLinkButton(SIGNED iLeft, SIGNED iTop, char *Text, WORD wId);

        SIGNED Message(const PegMessage &Mesg);

    private:
};


