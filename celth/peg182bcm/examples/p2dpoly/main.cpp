/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// main.cpp - Demonstration of using the PegPolygonThing
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

void PegAppInitialize(PegPresentationManager* pPresent)
{
    PolygonWindow* pWindow = new PolygonWindow();
    pPresent->Center(pWindow);
    pPresent->Add(pWindow);
}


