/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// dbmain.cpp - Example PegDecoratedButtons window
//
// Author: Jim DeLisle
//
// Copyright (c) 1998-2002 Swell Software 
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
#include "dbwin.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager* pPresent)
{
    pPresent->Add(new DecButtonWindow(10, 10));
}
