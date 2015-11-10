/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// main.cpp
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-2002 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes: PegLight example
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

/*--------------------------------------------------------------------------*/
class LightWindow : public PegDecoratedWindow
{
    public:
        LightWindow(const PegRect& Rect);
        ~LightWindow();

        SIGNED Message(const PegMessage& Mesg);

        PegColorLight* mpLight1;
        PegColorLight* mpLight2;

        SIGNED mnState1;
        SIGNED mnState2;
};

/*--------------------------------------------------------------------------*/
LightWindow::LightWindow(const PegRect& Rect) :
    PegDecoratedWindow(Rect),
    mnState1(0),
    mnState2(0)
{
    Add(new PegTitle("PegLight Demo Window"));

    PegRect LightRect = mClient;
    LightRect.wTop += 20;
    LightRect.wLeft += 20;
    LightRect.wRight = LightRect.wLeft + 100;
    LightRect.wBottom = LightRect.wTop + 100;

    mpLight1 = new PegColorLight(LightRect, 3, LS_CIRCLE);
    mpLight1->SetStateColor(0, RED);
    mpLight1->SetStateColor(1, BLUE);
    mpLight1->SetStateColor(2, GREEN);
    Add(mpLight1);
    
    LightRect.Shift(120, 0);
    
    mpLight2 = new PegColorLight(LightRect, 4);
    mpLight2->SetStateColor(0, CYAN);
    mpLight2->SetStateColor(1, BLUE);
    mpLight2->SetStateColor(2, GREEN);
    mpLight2->SetStateColor(3, LIGHTGREEN);
    Add(mpLight2);
}

/*--------------------------------------------------------------------------*/
LightWindow::~LightWindow()
{
}

/*--------------------------------------------------------------------------*/
SIGNED LightWindow::Message(const PegMessage& Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
        PegWindow::Message(Mesg);
        SetTimer(1, ONE_SECOND, ONE_SECOND);
        break;

    case PM_HIDE:
        PegWindow::Message(Mesg);
        KillTimer(1);
        break;

    case PM_TIMER:
        {
            mpLight1->IncrementState();
            mpLight2->IncrementState();

        } break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }

    return 0;
}


void PegAppInitialize(PegPresentationManager* pPresentation)
{
    PegRect Rect;
    Rect.Set(20, 20, 300, 200);
    LightWindow* pWindow = new LightWindow(Rect);

    pPresentation->Add(pWindow);
}


