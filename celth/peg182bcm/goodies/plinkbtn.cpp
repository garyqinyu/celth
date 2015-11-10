/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plinkbtn.cpp - A transparent text button that changes color after it
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

#include "peg.hpp"
#include "plinkbtn.hpp"

/*--------------------------------------------------------------------------*/
// provide all of the same constructors as PegTextButton:
/*--------------------------------------------------------------------------*/

PegLinkButton::PegLinkButton(PegRect &Rect, char *Text, WORD wId) :
    PegTextButton(Rect, Text, wId, AF_ENABLED|AF_TRANSPARENT|FF_NONE)
{
    // put the font you would like to use here:
    //mpFont = &MenuFont;
}


/*--------------------------------------------------------------------------*/
PegLinkButton::PegLinkButton(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, 
    char *Text, WORD wId) : PegTextButton(iLeft, iTop, iWidth, Text, wId,
    AF_ENABLED|AF_TRANSPARENT|FF_NONE)
{
    // put the font you would like to use here:
    //mpFont = &MenuFont;
}

/*--------------------------------------------------------------------------*/
PegLinkButton::PegLinkButton(SIGNED iLeft, SIGNED iTop, char *Text, WORD wId) :
    PegTextButton(iLeft, iTop, Text, wId, AF_ENABLED|AF_TRANSPARENT|FF_NONE)
{
    // put the font you would like to use here:
    //mpFont = &MenuFont;
}


/*--------------------------------------------------------------------------*/
// Override the message handler to do some custom operations:
/*--------------------------------------------------------------------------*/
SIGNED PegLinkButton::Message(const PegMessage &Mesg)
{
    PegMessage NewMessage;

    switch(Mesg.wType)
    {
    case PM_POINTER_ENTER:
        Screen()->SetPointerType(PPT_HAND);
        PegTextButton::Message(Mesg);
        NewMessage.wType = LM_OVERLINK;
        NewMessage.pSource = this;
        Parent()->Message(NewMessage); 
        break;

    case PM_POINTER_EXIT:
        Screen()->SetPointerType(PPT_NORMAL);
        PegTextButton::Message(Mesg);
        NewMessage.wType = LM_EXITLINK;
        NewMessage.pSource = this;
        Parent()->Message(NewMessage);
        break;

    case PM_LBUTTONUP:
        if (Style() & BF_SELECTED)
        {
            SetColor(PCI_NTEXT, RED);   // change color
            Screen()->SetPointerType(PPT_NORMAL);
        }
        PegTextButton::Message(Mesg);
        break;

    default:
        PegTextButton::Message(Mesg);
        break;
    }
    return 1;
}

