/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// panimwin.cpp: PEG Animation Window class implementation.
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
//    Installs palette for each window when displayed, if palette is non-NULL.
//    Starts timer for animation.
//    Draws animation frames.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

/*--------------------------------------------------------------------------*/
PegAnimationWindow::PegAnimationWindow(const PegRect &Rect,
    PegBitmap *pBackground, PegBitmap **FrameList, UCHAR uNumFrames,
    SIGNED xPos, SIGNED yPos, UCHAR *Palette, WORD wStyle) :
    PegWindow(Rect, wStyle)
{
    muAnimationFrame = 0;
    muNumFrames = uNumFrames;
    mpPalette = Palette;
    mFramePoint.x = xPos;
    mFramePoint.y = yPos;
    mpAnimations = FrameList;
    mpBackground = pBackground;
}

/*--------------------------------------------------------------------------*/
SIGNED PegAnimationWindow::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case PM_TIMER:
        muAnimationFrame++;

        if (muAnimationFrame >= muNumFrames)
        {
            muAnimationFrame = 0;
        }
        DrawAnimationFrame();
        break;

    case PM_SHOW:
        PegWindow::Message(Mesg);

        if (mpPalette)
        {
            Screen()->SetPalette(0, 256, mpPalette);
        }

        break;

    case PM_HIDE:
        PegWindow::Message(Mesg);
        MessageQueue()->KillTimer(this, 1);
        break;

    default:
        return(PegWindow::Message(Mesg));
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
void PegAnimationWindow::Draw(void)
{
    PegPoint Put;
    Put.x = mClient.wLeft;
    Put.y = mClient.wTop;
    BeginDraw();

    if (mpBackground)
    {
        Bitmap(Put, mpBackground);
    }
    DrawChildren();
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegAnimationWindow::Run(WORD wPeriod, UCHAR uFrame)
{
    MessageQueue()->SetTimer(this, 1, wPeriod, wPeriod);
    muAnimationFrame = uFrame;
}

/*--------------------------------------------------------------------------*/
void PegAnimationWindow::Stop(void)
{
    MessageQueue()->KillTimer(this, 1);
}

/*--------------------------------------------------------------------------*/
void PegAnimationWindow::SetFrame(UCHAR uFrame)
{
    muAnimationFrame = uFrame;
    DrawAnimationFrame();
}

/*--------------------------------------------------------------------------*/
void PegAnimationWindow::DrawAnimationFrame(void)
{
    PegBitmap *pNewFrame = mpAnimations[muAnimationFrame];

    PegRect  FillRect;
    PegPoint PutFrame;

    PutFrame.x = mClient.wLeft + mFramePoint.x;
    PutFrame.y = mClient.wTop + mFramePoint.y;

    FillRect.wLeft = PutFrame.x;
    FillRect.wRight = FillRect.wLeft + pNewFrame->wWidth;
    FillRect.wTop = PutFrame.y;
    FillRect.wBottom = FillRect.wTop + pNewFrame->wHeight;
    Invalidate(FillRect);
    BeginDraw();
    Bitmap(PutFrame, pNewFrame);
    EndDraw();
}



