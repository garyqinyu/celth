/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pblight.cpp - Implementation for the PegBitmapLight class
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
PegBitmapLight::PegBitmapLight(const PegRect& Rect, WORD wNumStates) :
    PegLight(Rect, wNumStates, FF_NONE)
{
    Type(TYPE_BM_LIGHT);
    mpStateBitmap = NULL;
}

/*--------------------------------------------------------------------------*/
PegBitmapLight::PegBitmapLight(SIGNED iLeft, SIGNED iTop, WORD wNumStates) :
    PegLight(iLeft, iTop, wNumStates, FF_NONE)
{
    Type(TYPE_BM_LIGHT);
    mpStateBitmap = NULL;
}

/*--------------------------------------------------------------------------*/
PegBitmapLight::~PegBitmapLight()
{
    if(mpStateBitmap)
    {
        delete [] mpStateBitmap;
    }
}

/*--------------------------------------------------------------------------*/
void PegBitmapLight::Draw()
{
    if (!mwNumStates || !mpStateBitmap)
    {
        return;
    }
    BeginDraw();

    PegPoint Point;
    Point.x = mReal.wLeft;
    Point.y = mReal.wTop;

    Bitmap(Point, mpStateBitmap[mwCurState]);
    DrawChildren();

    EndDraw();
}

/*--------------------------------------------------------------------------*/
WORD PegBitmapLight::SetNumStates(WORD wNumStates)
{
    if(mwNumStates == wNumStates)
    {
        return(mwNumStates);
    }

    if(!mpStateBitmap)
    {
        mpStateBitmap = new PegBitmap*[wNumStates];
        memset(mpStateBitmap, 0, sizeof(PegBitmap*) * wNumStates);
    }
    else
    {
        PegBitmap** pBitmaps = new PegBitmap*[wNumStates];

        if(wNumStates > mwNumStates)
        {
            memset(pBitmaps, 0, sizeof(PegBitmap*) * wNumStates);
        }

        memcpy(pBitmaps, mpStateBitmap, sizeof(PegBitmap*) * wNumStates);

        delete [] mpStateBitmap;
        mpStateBitmap = pBitmaps;
    }

    return(PegLight::SetNumStates(wNumStates));
}

/*--------------------------------------------------------------------------*/
BOOL PegBitmapLight::SetStateBitmap(WORD wState, PegBitmap* pBmp)
{
    BOOL bRetVal = FALSE;

    if(mpStateBitmap == NULL && mwNumStates)
    {
        mpStateBitmap = new PegBitmap*[mwNumStates];
        memset(mpStateBitmap, 0, sizeof(PegBitmap*) * mwNumStates);

        PegRect tNewSize;
        tNewSize.Set(mReal.wLeft, mReal.wTop, 
                     mReal.wLeft + pBmp->wWidth - 1,
                     mReal.wTop + pBmp->wHeight - 1);
        Resize(tNewSize);
    }

    if(wState < mwNumStates)
    {
        mpStateBitmap[wState] = pBmp;
        bRetVal = TRUE;
    }

    return(bRetVal);
}


#endif // PEG_HMI_GADGETS

