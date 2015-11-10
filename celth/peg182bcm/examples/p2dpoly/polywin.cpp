/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// polywin.cpp - Demonstration of using the PegPolygonThing
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
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#include "polywin.hpp"

/*--------------------------------------------------------------------------*/
static PegPoint gtPolyPoints[] = { {47, 24}, {12, 12}, {24, 22},
                                   {24, 23}, {31, 19}, {31, 27}, 
                                   {24, 25}, {12, 35} };

/*--------------------------------------------------------------------------*/
PolygonWindow::PolygonWindow() : PegDecoratedWindow()
{
    mReal.Set(0, 0, 400, 300);
    InitClient();

    SetColor(PCI_NORMAL, BLUE);

    Add(new PegTitle("Peg2DPolygon Example"));

    PegRect tRect;
    tRect.Set(mClient.wLeft + 10, mClient.wTop + 10, mClient.wLeft + 57,
              mClient.wTop + 57);
    mpPolygon = new Peg2DPolygon(tRect, &gtPolyPoints[0], 8,
                                 101, FF_NONE);
    mpPolygon->SetColor(PCI_NORMAL, BLUE);
    mpPolygon->SetColor(PCI_NTEXT, WHITE);
    Add(mpPolygon);

    miMoveX = 1;
    miMoveY = 1;
    miMoveAmount = 3;
    miRotDir = 1;
    miCurAngle = 0;
}

/*--------------------------------------------------------------------------*/
SIGNED PolygonWindow::Message(const PegMessage& Mesg)
{
    switch(Mesg.wType)
    {
        case PM_SHOW:
            {
                PegDecoratedWindow::Message(Mesg);
                SetTimer(1, 1, 1);
            } break;

        case PM_HIDE:
            {
                PegDecoratedWindow::Message(Mesg);
                KillTimer(1);
            } break;

        case PM_TIMER:
            {
                MovePolygon();
            } break;

        default:
            return(PegDecoratedWindow::Message(Mesg));
    }

    return(0);
}

/*--------------------------------------------------------------------------*/
void PolygonWindow::MovePolygon()
{
    PegRect tMoveRect = mpPolygon->mReal;

    if(tMoveRect.wLeft < mClient.wLeft)
    {
        miMoveX = 1;
    }

    if(tMoveRect.wRight > mClient.wRight)
    {
        miMoveX = -1;
    }

    if(tMoveRect.wTop < mClient.wTop)
    {
        miMoveY = 1;
    }

    if(tMoveRect.wBottom > mClient.wBottom)
    {
        miMoveY = -1;
    }

    miRotDir = miMoveAmount * (miMoveX * miMoveY);

    tMoveRect.Shift((miMoveX * miMoveAmount), (miMoveY * miMoveAmount));
    
    miCurAngle += miRotDir;

    mpPolygon->Resize(tMoveRect);
    mpPolygon->SetCurAngle(miCurAngle);
    
    Draw();
}

