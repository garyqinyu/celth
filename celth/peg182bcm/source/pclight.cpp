/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pclight.cpp - Implementation for the PegColorLight class
//
// Author: Jim DeLisle
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

#include "string.h"
#include "peg.hpp"

#if defined(PEG_HMI_GADGETS)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegColorLight::PegColorLight(const PegRect &Size, WORD wNumStates,
    WORD wStyle) : PegLight(Size, wNumStates, wStyle)
{
    mpStateColor = NULL;
    Type(TYPE_CLR_LIGHT);
}

/*--------------------------------------------------------------------------*/
PegColorLight::PegColorLight(SIGNED iLeft, SIGNED iTop, WORD wNumStates,
                             WORD wStyle) :
    PegLight(iLeft, iTop, wNumStates, wStyle)
{
    mpStateColor = NULL;
    Type(TYPE_CLR_LIGHT);
}

/*--------------------------------------------------------------------------*/
PegColorLight::~PegColorLight()
{
    if(mpStateColor)
    {
        delete [] mpStateColor;
    }
}

/*--------------------------------------------------------------------------*/
void PegColorLight::Draw()
{
    if (!mwNumStates || !mpStateColor)
    {
        return;
    }    

    BeginDraw();
    PegColor Color(mpStateColor[mwCurState], mpStateColor[mwCurState], CF_FILL);

    if(mwStyle & LS_RECTANGLE)
    {
        StandardBorder(mpStateColor[mwCurState]);
    }
    else if(mwStyle & LS_CIRCLE)
    {
        if(mwStyle & AF_TRANSPARENT && Parent())
        {
            StandardBorder(Parent()->muColors[PCI_NORMAL]);
        }

        SIGNED iWidth = mClient.Width() / 2;
        SIGNED iHeight = mClient.Height() / 2;
        
        Circle(iWidth + mClient.wLeft, iHeight + mClient.wTop, 
               PEGMIN(iWidth, iHeight), Color, 1);

        PegColor Border(PCLR_SHADOW, PCLR_SHADOW, CF_NONE);
        Circle(iWidth + mClient.wLeft, iHeight + mClient.wTop, 
               PEGMIN(iWidth, iHeight), Border, 1);
    }

    DrawChildren();

    EndDraw();
}

/*--------------------------------------------------------------------------*/
WORD PegColorLight::SetNumStates(WORD wNumStates)
{
    if(wNumStates == mwNumStates)
    {
        return(mwNumStates);
    }


    if(mpStateColor == NULL)
    {
        mpStateColor = new COLORVAL[wNumStates];
        memset(mpStateColor, 0, sizeof(COLORVAL) * wNumStates);
    }
    else
    {
        COLORVAL* pColors = new COLORVAL[wNumStates];

        if(wNumStates > mwNumStates)
        {
            memset(pColors, 0, sizeof(COLORVAL) * wNumStates);
        }

        memcpy(pColors, mpStateColor, sizeof(COLORVAL) * wNumStates);

        delete [] mpStateColor;
        mpStateColor = pColors;
    }

    return(PegLight::SetNumStates(wNumStates));
}

/*--------------------------------------------------------------------------*/
BOOL PegColorLight::SetStateColor(WORD wState, COLORVAL tColor)
{
    BOOL bRetVal = FALSE;

    if(mpStateColor == NULL && mwNumStates)
    {
        mpStateColor = new COLORVAL[mwNumStates];
        memset(mpStateColor, 0, sizeof(COLORVAL) * mwNumStates);
    }

    if(wState < mwNumStates)
    {
        mpStateColor[wState] = tColor;
        bRetVal = TRUE;
    }

    return(bRetVal);
}

#endif // PEG_HMI_GADGETS

