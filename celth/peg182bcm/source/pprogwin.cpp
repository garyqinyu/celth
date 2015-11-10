/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pprogwin.cpp - Progress Window class definition.
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
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

/*--------------------------------------------------------------------------*/
PegProgressWindow::PegProgressWindow(const PegRect &Rect, const PEGCHAR *Title, 
    const PEGCHAR *Message, WORD wMesgStyle, WORD wProgStyle,
    PegBitmap *pIcon, PegThing *Owner) :
    PegMessageWindow(Rect, Title, Message, wMesgStyle, pIcon, Owner)
{
    mwProgStyle = wProgStyle;
    AddProgBar();
}

/*--------------------------------------------------------------------------*/
PegProgressWindow::PegProgressWindow(const PEGCHAR *Title, const PEGCHAR *Message,
    WORD wOptions, WORD wProgStyle, PegBitmap *pIcon, PegThing *Owner) :
    PegMessageWindow(Title, Message, wOptions, pIcon, Owner)
{
    mwProgStyle = wProgStyle;
    PegRect NewSize = mReal;

    if (Style() & (MW_OK|MW_YES|MW_NO|MW_ABORT|MW_RETRY|MW_CANCEL))
    {
        NewSize.wBottom += 40;
    }
    else
    {
        NewSize.wBottom += 24;
    }
    Resize(NewSize);

    // move all client area buttons down to make room for progress bar:
    PegThing *pButton = First();

    while(pButton)
    {
        if (!pButton->StatusIs(PSF_NONCLIENT))
        {
            if (pButton->Type() == TYPE_TEXTBUTTON)
            {
                NewSize = pButton->mReal;
                NewSize.Shift(0, 40);
                pButton->Resize(NewSize);
            }
        }
        pButton = pButton->Next();
    }
    AddProgBar();
}

/*--------------------------------------------------------------------------*/
PegProgressWindow::~PegProgressWindow()
{
}


/*--------------------------------------------------------------------------*/
void PegProgressWindow::AddProgBar(void)
{
    PegRect ProgRect;

    if (Style() & (MW_OK|MW_YES|MW_NO|MW_ABORT|MW_RETRY|MW_CANCEL))
    {
        ProgRect.Set(mClient.wLeft + 10,mClient.wBottom - 56,
            mClient.wRight - 10, mClient.wBottom - 32);
    }
    else
    {
        ProgRect.Set( mClient.wLeft + 10, mClient.wBottom - 28,
            mClient.wRight - 10, mClient.wBottom - 4);
    }
    mpProgBar = new PegProgressBar(ProgRect, mwProgStyle);
    Add(mpProgBar);
}

