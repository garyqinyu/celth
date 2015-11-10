/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// gauge.cpp - Example gauge control, demonstrates using the
//  off-screen drawing functions.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1993-2002 Swell Software 
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
#include "gauge.hpp"
#include "chart.hpp"
#include "math.h"

extern PegBitmap gbDialBitmap;

#define PI 3.14159265358

/*--------------------------------------------------------------------------*/
// PegAppInitialize- called by the PEG library during program startup.
/*--------------------------------------------------------------------------*/

// The SMX and ThreadX example programs use the gauge object, but provide
// their own initialization function

#if !defined(PEGSMX) && !defined(PEGX) && !defined(PHARLAP)
extern PegBitmap gbPoolBitmap;

void PegAppInitialize(PegPresentationManager *pPresentation)
{
	// create the dialog and add it to PegPresentationManager:
    PegRect PosRect;
    PosRect.Set(10, 10, 80, 50);
    pPresentation->Add(new Chart(PosRect));

	Gauge *pGauge = new Gauge();
	pPresentation->Center(pGauge);
	pPresentation->Add(pGauge);
    pPresentation->SetWallpaper(&gbPoolBitmap);
}

#endif


/*--------------------------------------------------------------------------*/
// Gauge- Animated gauge gadget.
/*--------------------------------------------------------------------------*/
Gauge::Gauge() : PegThing()
{
    mReal.Set(0, 0, gbDialBitmap.wWidth - 1, gbDialBitmap.wHeight - 1);
    mClient = mReal;
    mClip = mReal;
    miCurrent = 0;
    miDir = 2;
    mpBitmap = NULL;
    mbMoveMode = FALSE;
}


/*--------------------------------------------------------------------------*/
// Override the Message Function to start a timer for updating the gauge,
//  also provides "drag-to-position" operation.
/*--------------------------------------------------------------------------*/
SIGNED Gauge::Message(const PegMessage &Mesg)
{
	switch(Mesg.wType)
	{
    case PM_SHOW:
        PegThing::Message(Mesg);
        SetTimer(1, ONE_SECOND / 10, ONE_SECOND / 10);
        break;

    case PM_HIDE:
        PegThing::Message(Mesg);
        Screen()->DestroyBitmap(mpBitmap);
        KillTimer(1);
        break;

    case PM_TIMER:

        miCurrent += miDir;
        if (miCurrent > 100)
        {
            miDir = -miDir;
        }
        else
        {
            if (miCurrent < 0)
            {
                miDir = -miDir;
            }
        }
                
        DrawToBitmap();
        Invalidate();
        Draw();
        break;

    case PM_LBUTTONDOWN:
        if (!mbMoveMode)
        {
            mbMoveMode = TRUE;
            CapturePointer();
            mStartMove = Mesg.Point;
        }
        break;

    case PM_LBUTTONUP:
        if (mbMoveMode)
        {
            mbMoveMode = FALSE;
            ReleasePointer();
            PegRect NewSize = mReal;
            NewSize.Shift(Mesg.Point.x - mStartMove.x, Mesg.Point.y - mStartMove.y);
            Resize(NewSize);
            Parent()->Draw();
        }
        break;

    case PM_RBUTTONDOWN:
        // spawn a new instance of myself:
        Presentation()->Add(new Gauge());
        break;

	default:
		PegThing::Message(Mesg);
		break;
	}
	return 0;
}


/*--------------------------------------------------------------------------*/
// Override the Draw() function to do the off-screen drawing.
/*--------------------------------------------------------------------------*/
void Gauge::Draw(void)
{
    if (!mpBitmap)
    {
        mpBitmap = Screen()->CreateBitmap(gbDialBitmap.wWidth, 
                                          gbDialBitmap.wHeight);
        DrawToBitmap();
    }
    BeginDraw();
    PegPoint Put;
    Put.x = mReal.wLeft;
    Put.y = mReal.wTop;
    Bitmap(Put, mpBitmap);
    EndDraw();
}


/*--------------------------------------------------------------------------*/
// DrawToBitmap- draw the gauge bitmap and the needle indicator into an
//  offscreen bitmap.
/*--------------------------------------------------------------------------*/
void Gauge::DrawToBitmap(void)
{
    PegPoint Put;
    SIGNED x1, y1, x2, y2;
    SIGNED iRadious = (gbDialBitmap.wWidth / 2) - 8;
    
    // Open the bitmap for drawing:

    Screen()->BeginDraw(this, mpBitmap);
    Put.x = Put.y = 0;

    // copy the background bitmap into memory bitmap:
    Bitmap(Put, &gbDialBitmap);

    // find the center:
    x1 = mReal.Width() / 2;
    y1 = mReal.Height() / 2;

    // find the end:
    double angle = ((4.0 * PI) / 5.0) + (((double) miCurrent / 100.0) * PI);
    x2 = x1 + cos(angle) * iRadious;
    y2 = y1 + sin(angle) * iRadious;

    // draw the indicator line:
    PegColor LineColor(RED, RED, CF_NONE);
    Line(x1, y1, x2, y2, LineColor, 2);

    // Close the bitmap for drawing:
    Screen()->EndDraw(mpBitmap);
}

