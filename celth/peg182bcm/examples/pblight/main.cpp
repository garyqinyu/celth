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
// Notes: PegBitmapLight example
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

extern PegBitmap gbgreenltBitmap;
extern PegBitmap gbredltBitmap;
extern PegBitmap gbyellowltBitmap;

/*--------------------------------------------------------------------------*/
class LightWindow : public PegDecoratedWindow
{
    public:
        LightWindow(const PegRect& Rect);
        ~LightWindow();

        SIGNED Message(const PegMessage& Mesg);

        PegBitmapLight* mpLight1;
        PegTextButton* mpStart;
        PegTextButton* mpStop;

    private:

        enum ButtonIds
        {
            IDB_START = 1,
            IDB_STOP
        };
};

/*--------------------------------------------------------------------------*/
LightWindow::LightWindow(const PegRect& Rect) :
    PegDecoratedWindow(Rect)
{
    Add(new PegTitle("PegBitmapLight Demo Window"));

    muColors[PCI_NORMAL] = GREEN;
    
    mpLight1 = new PegBitmapLight(mClient.wLeft + 20, mClient.wTop + 20, 3);
    mpLight1->SetStateBitmap(2, &gbredltBitmap);
    mpLight1->SetStateBitmap(1, &gbyellowltBitmap);
    mpLight1->SetStateBitmap(0, &gbgreenltBitmap);
    Add(mpLight1); 
    
    PegRect tRect;
    tRect.Set(mClient.wLeft + 200, mClient.wTop + 40, 
              mClient.wLeft + 300, mClient.wTop + 80);
    mpStart = new PegTextButton(tRect, "Start the Light", IDB_START);
    Add(mpStart);

    tRect.Shift(0, 50);
    mpStop = new PegTextButton(tRect, "Stop the Light", IDB_STOP);
    Add(mpStop);


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
    case SIGNAL(IDB_START, PSF_CLICKED):
        {
            PegWindow::Message(Mesg);
            SetTimer(1, ONE_SECOND, ONE_SECOND);
        } break;

    case PM_HIDE:
    case SIGNAL(IDB_STOP, PSF_CLICKED):
        {
            PegWindow::Message(Mesg);
            KillTimer(1);
        } break;

    case PM_TIMER:
        mpLight1->IncrementState();
        break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager* pPresentation)
{
    PegRect Rect;
    Rect.Set(20, 20, 400, 240);
    LightWindow* pWindow = new LightWindow(Rect);

    pPresentation->Add(pWindow);
}


