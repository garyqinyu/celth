/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// chart.cpp - Simple chart
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2002 Swell Software 
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

#include <stdlib.h>		// for rand

#include "peg.hpp"
#include "chart.hpp"


/*--------------------------------------------------------------------------*/
// Chart- Continuous chart gadget.
/*--------------------------------------------------------------------------*/
Chart::Chart(const PegRect &Size) : PegWindow(Size, FF_RECESSED)
{
    SetColor(PCI_NORMAL, BLACK);
    mwMaxPoints = mClient.Width() / 4;
    mpData = new SIGNED[mwMaxPoints + 1];
    *mpData = rand() % mClient.Height();
    *(mpData + 1) = rand() % mClient.Height();
    mwNumPoints = 2; 
    mpFirstPoint = mpData;
    mpBitmap = Screen()->CreateBitmap(mClient.Width(), mClient.Height());
    mbMoveMode = FALSE;
    SetTimer(1, 1, 1);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Chart::~Chart()
{
    delete mpData;
    delete mpBitmap->pStart;
    delete mpBitmap;
}


/*--------------------------------------------------------------------------*/
// Override the Message Function to start a timer for updating the guage,
//  also provides "drag-to-position" operation.
/*--------------------------------------------------------------------------*/

extern PegBitmap gbGraphIconBitmap;

SIGNED Chart::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_POINTER_MOVE:
        if (mbMoveMode)
        {
            PositionIcon(Mesg.Point);
        }
        break;

    case PM_LBUTTONDOWN:
        if (!mbMoveMode)
        {
            mbMoveMode = TRUE;
            mpIcon = new PegIcon(&gbGraphIconBitmap);
            //mpIcon->AddStatus(PSF_VIEWPORT);
            PositionIcon(Mesg.Point);
            Presentation()->Add(mpIcon);
            CapturePointer();
            mStartMove = Mesg.Point;
        }
        break;

    case PM_LBUTTONUP:
        if (mbMoveMode)
        {
            mbMoveMode = FALSE;
            ReleasePointer();
            Destroy(mpIcon);
            PegRect NewSize = mReal;
            NewSize.Shift(Mesg.Point.x - mStartMove.x, Mesg.Point.y - mStartMove.y);
            Resize(NewSize);
            Parent()->Draw();
        }
        break;

    case PM_TIMER:
        Update();
        break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
// Override the Draw() function to do the off-screen drawing.
/*--------------------------------------------------------------------------*/
void Chart::Draw(void)
{
    BeginDraw();
    DrawFrame(FALSE);
    PegPoint Put;
    Put.x = mClient.wLeft;
    Put.y = mClient.wTop;
    Bitmap(Put, mpBitmap);
    EndDraw();
}


/*--------------------------------------------------------------------------*/
// DrawToBitmap- draw the guage bitmap and the needle indicator into an
//  offscreen bitmap.
/*--------------------------------------------------------------------------*/
void Chart::DrawToBitmap(void)
{
    PegRect  BitmapRect;
    WORD wIndex = 0;
    PegColor Color(LIGHTGREEN, BLACK, CF_FILL);
    SIGNED xPos, y1, y2;
    SIGNED *pEnd = mpData + mwMaxPoints;

    // Open the bitmap for drawing:

    Screen()->BeginDraw(this, mpBitmap);

    // fill the bitmap with BLACK:

    BitmapRect.Set(0, 0, mpBitmap->wWidth, mpBitmap->wHeight);
    Rectangle(BitmapRect, Color, 0);

    // draw the chart lines:

    SIGNED *pGet = mpFirstPoint;
    y1 = *pGet++;
    xPos = 0;

    while(wIndex < mwNumPoints - 1)
    {
        if (pGet >= pEnd)
        {
            pGet = mpData;
        }
        y2 = *pGet++;
        Line(xPos, y1, xPos + 4, y2, Color, 1);
        y1 = y2;
        xPos += 4;
        wIndex++;
    }

    // Close the bitmap for drawing:
    Screen()->EndDraw(mpBitmap);
}

/*--------------------------------------------------------------------------*/
void Chart::Update(void)
{
    SIGNED NewVal = rand() % mClient.Height();

    if (mwNumPoints < mwMaxPoints)
    {
        *(mpData + mwNumPoints) = NewVal;
        mwNumPoints++;
    }
    else
    {
        *mpFirstPoint = NewVal;
        SIGNED *pEnd = mpData + mwMaxPoints - 1;
        if (mpFirstPoint < pEnd)
        {
            mpFirstPoint++;
        }
        else
        {
            mpFirstPoint = mpData;
        }
    }

    DrawToBitmap();
    Invalidate();
    Draw();
}

/*--------------------------------------------------------------------------*/
void Chart::PositionIcon(PegPoint Point)
{
    PegRect Size = mpIcon->mReal;
    Size.MoveTo(Point.x, Point.y);
    mpIcon->Resize(Size);
    Presentation()->Draw();
}
