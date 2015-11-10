/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pspin.cpp - PegSpinButton class.
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

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"

extern PegBitmap gbUpArrowBitmap;
extern PegBitmap gbDownArrowBitmap;
extern PegBitmap gbLeftArrowBitmap;
extern PegBitmap gbMoreBitmap;


/*--------------------------------------------------------------------------*/
// Note: PegSpinButton uses the non-ANSI "C" function "_ltoa" to convert the
// spin value to a string. Most compilers used to date (MetaWare, Borland,
// Microsoft, Green Hills, Watcom, and Keil) support this function in their
// run-time library. If your compiler does not support the "_ltoa" function,
// you should turn on the define USE_PEG_LTOA in peg.hpp.
//
// Why don't we use the ANSI sprintf? Many run-time implementations of sprintf
// are non-reentrant, or otherwise forbidden from use on embedded platforms
// due to stack requirements or code size. Sprintf is often a very large
// function!
/*--------------------------------------------------------------------------*/


/*--------------------- SPIN BUTTON CLASS ----------------------------------*/
/*--------------------------------------------------------------------------*/
PegSpinButton::PegSpinButton(SIGNED iLeft, SIGNED iTop, WORD wId, WORD wStyle) :
    PegThing(wId, wStyle)
{
    if (wStyle & SB_VERTICAL)
    {
        mReal.Set(iLeft, iTop, iLeft + PEG_SCROLL_WIDTH - 1,
            iTop + (PEG_SCROLL_WIDTH * 2) - 1);
    }
    else
    {
        mReal.Set(iLeft, iTop, iLeft + (PEG_SCROLL_WIDTH * 2) - 1,
            iTop + PEG_SCROLL_WIDTH - 1);
    }
    mpSlave = NULL;
    mlMin = mlMax = miOutputWidth = 0;
    AddSpinButtons();
}

/*--------------------------------------------------------------------------*/
PegSpinButton::PegSpinButton(SIGNED iLeft, SIGNED iTop, PegTextThing *client,
    LONG lMin, LONG lMax, SIGNED iStep, WORD wId, WORD wStyle) : PegThing(wId, wStyle)
{
    if (wStyle & SB_VERTICAL)
    {
        mReal.Set(iLeft, iTop, iLeft + PEG_SCROLL_WIDTH - 1,
            iTop + (PEG_SCROLL_WIDTH * 2) - 1);
    }
    else
    {
        mReal.Set(iLeft, iTop, iLeft + (PEG_SCROLL_WIDTH * 2) - 1,
            iTop + PEG_SCROLL_WIDTH - 1);
    }
    mpSlave = client;
    mlMin = lMin;
    mlMax = lMax;
    miStep = iStep;
    miOutputWidth = 0;

    if (client)
    {
        client->SetCopyMode();
    }
        
    AddSpinButtons();
}

/*--------------------------------------------------------------------------*/
PegSpinButton::PegSpinButton(const PegRect &Rect, WORD wId, WORD wStyle) :
    PegThing(Rect, wId, wStyle)
{
    mpSlave = NULL;
    mlMin = mlMax = miStep = 0;
    miOutputWidth = 0;
    AddSpinButtons();
}


/*--------------------------------------------------------------------------*/
PegSpinButton::PegSpinButton(const PegRect &Rect, PegTextThing *client,
    LONG lMin, LONG lMax, SIGNED iStep, WORD wId, WORD wStyle) :
    PegThing(Rect, wId, wStyle)
{
    mpSlave = client;
    mlMin = lMin;
    mlMax = lMax;
    miStep = iStep;
    miOutputWidth = 0;

    if (client)
    {
        client->SetCopyMode();
    }

    AddSpinButtons();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpinButton::AddSpinButtons(void)
{
    Type(TYPE_SPINBUTTON);
    mClient = mReal;
    PegRect ChildRect = mReal;
    WORD wEnable = Style() & AF_ENABLED;

    if (Style() & SB_VERTICAL)
    {
        ChildRect.wBottom = ChildRect.wTop + mReal.Height() / 2;
        Add(new PegBitmapButton(ChildRect, &gbUpArrowBitmap,
            IDB_UPBUTTON, wEnable|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT));
        ChildRect.wTop = ChildRect.wBottom + 1;
        ChildRect.wBottom = mReal.wBottom;
        Add(new PegBitmapButton(ChildRect, &gbDownArrowBitmap,
            IDB_DOWNBUTTON, wEnable|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT));
    }
    else
    {
        ChildRect.wRight = ChildRect.wLeft + mReal.Width() / 2;
        Add(new PegBitmapButton(ChildRect, &gbLeftArrowBitmap,
            IDB_LEFTBUTTON, wEnable|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT));
        ChildRect.wLeft = ChildRect.wRight + 1;
        ChildRect.wRight = mReal.wRight;
        Add(new PegBitmapButton(ChildRect, &gbMoreBitmap,
            IDB_RIGHTBUTTON, wEnable|BF_FULLBORDER|BF_DOWNACTION|BF_REPEAT));
    }
    SetSignals(SIGMASK(PSF_SPIN_MORE)|SIGMASK(PSF_SPIN_LESS));
}

/*--------------------------------------------------------------------------*/
SIGNED PegSpinButton::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case PEG_SIGNAL(IDB_UPBUTTON, PSF_CLICKED):
    case PEG_SIGNAL(IDB_RIGHTBUTTON, PSF_CLICKED):
        if (mpSlave)
        {
            UpdateSlave(PSF_SPIN_MORE);
        }
        CheckSendSignal(PSF_SPIN_MORE);
        break;

    case PEG_SIGNAL(IDB_DOWNBUTTON, PSF_CLICKED):
    case PEG_SIGNAL(IDB_LEFTBUTTON, PSF_CLICKED):
        if (mpSlave)
        {
            UpdateSlave(PSF_SPIN_LESS);
        }
        CheckSendSignal(PSF_SPIN_LESS);
        break;

    default:
        PegThing::Message(Mesg);
    }
    return(0);
}

/*--------------------------------------------------------------------------*/
void PegSpinButton::UpdateSlave(WORD wSpinType)
{
    PEGCHAR cTemp[34];
    LONG lVal = atol(mpSlave->DataGet());

    if (wSpinType == PSF_SPIN_LESS)
    {
        lVal -= miStep;
    }
    else
    {
        lVal += miStep;
    }
    if (lVal < mlMin)
    {
        lVal = mlMin;
    }
    if (lVal > mlMax)
    {
        lVal = mlMax;
    }

    _ltoa(lVal, cTemp, 10);

    if (miOutputWidth > 0)
    {
        SIGNED wWidth = (SIGNED) strlen(cTemp);
        if (wWidth < miOutputWidth)
        {
            PEGCHAR cTemp1[34];

           #ifdef PEG_UNICODE
            PEGCHAR cDummy[] = {'0','0','0','0','0','0','0','0','0','0','0','0', 0};
            strcpy(cTemp1, cDummy);
           #else
            strcpy(cTemp1, "000000000000000");
           #endif

            strcpy(cTemp1 + miOutputWidth - wWidth, cTemp);
            mpSlave->DataSet(cTemp1);
        }
        else
        {
            mpSlave->DataSet(cTemp);
        }
    }
    else
    {
        mpSlave->DataSet(cTemp);
    }

    #ifdef __HIGHC__
    #pragma OffWarn(480)
    #endif

    PegPrompt *pTemp = (PegPrompt *) mpSlave;

    #ifdef __HIGHC__
    #pragma PopWarn()
    #endif

    pTemp->Draw();
}

/*--------------------------------------------------------------------------*/
void PegSpinButton::SetLimits(LONG lMin, LONG lMax, SIGNED iStep)
{
    mlMin = lMin;
    mlMax = lMax;
    miStep = iStep;

}

// End of file
