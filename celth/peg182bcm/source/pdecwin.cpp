/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pdecwin.cpp - Base window class definition.
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
// This is the basic window class. It supports the addition of decorations
// such as a border, title, scroll bar, status bar, and menu, in addition
// to any other derived PegThings.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

/*--------------------------------------------------------------------------*/
PegDecoratedWindow::PegDecoratedWindow(const PegRect &Rect, WORD wFlags) :
    PegWindow(Rect, wFlags),
    mpTitle(NULL),
    mpMenuBar(NULL),
    mpStatusBar(NULL),
    mpToolBar(NULL)
{
    Type(TYPE_DECORATED_WIN);
}

/*--------------------------------------------------------------------------*/
PegDecoratedWindow::PegDecoratedWindow(WORD wStyle) :
    PegWindow(wStyle),
    mpTitle(NULL),
    mpMenuBar(NULL),
    mpStatusBar(NULL),
    mpToolBar(NULL)
{
    Type(TYPE_DECORATED_WIN);
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegDecoratedWindow::~PegDecoratedWindow()
{

}

/*--------------------------------------------------------------------------*/
SIGNED PegDecoratedWindow::Message(const PegMessage &Mesg)
{
   #ifdef PEG_KEYBOARD_SUPPORT
    PegMessage NewMessage;
   #endif

    switch(Mesg.wType)
    {
    case PM_CURRENT:
        PegWindow::Message(Mesg);
        if (mpTitle)
        {
            Invalidate(mpTitle->mClip);
            mpTitle->Draw();
        }
        break;

    case PM_NONCURRENT:
        PegWindow::Message(Mesg);
        if (mpTitle)
        {
            Invalidate(mpTitle->mClip);
            mpTitle->Draw();
        }
        break;

   #ifdef PEG_KEYBOARD_SUPPORT
    case PM_KEY_RELEASE:
        if (Mesg.iData == PK_F1 && mpMenuBar)
        {
            NewMessage.wType = PM_KEY;
            NewMessage.iData = PK_MENU_KEY;
            mpMenuBar->Message(NewMessage);
        }
        else
        {
            return (PegWindow::Message(Mesg));
        }
        break;

   #endif

    default:
        return(PegWindow::Message(Mesg));
    }
    return (0);
}

/*--------------------------------------------------------------------------*/
void PegDecoratedWindow::Add(PegThing *What, BOOL bDraw)
{
    switch(What->Type())
    {
    case TYPE_TITLE:
        mpTitle = (PegTitle *) What;

        if(!(What->Parent()))        // not allready added to me?
        {
            mClient.wTop += What->mReal.Height();
        }
        break;

    case TYPE_MENU_BAR:
        mpMenuBar = (PegMenuBar *) What;
        if(!(What->Parent()))        // not allready added to me?
        {
            mClient.wTop += What->mReal.Height();
        }
        break;

    case TYPE_STATUS_BAR:
        mpStatusBar = (PegStatusBar *) What;
        if (!(What->Parent()))
        {
            mClient.wBottom -= What->mReal.Height() - 2;
        }
        break;

    case TYPE_TOOL_BAR:
        mpToolBar = (PegToolBar*) What;
        if(!(What->Parent()))
        {
            mClient.wTop += What->mReal.Height();
        }
        break;

    default:
        break;
    }
    PegWindow::Add(What, bDraw);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegThing *PegDecoratedWindow::Remove(PegThing *What, BOOL bDraw)
{
    PegRect Invalid;
    switch(What->Type())
    {
    case TYPE_TITLE:
        if (What->Parent() == this)
        {
            mpTitle = NULL;
            Invalid = What->mReal;
            Invalid.wBottom = mClient.wTop;
            Invalidate(Invalid);
            mClient.wTop -= What->mReal.Height();
            
        }
        break;

    case TYPE_MENU_BAR:
        if (What->Parent() == this)
        {
            mpMenuBar = NULL;
            Invalid = What->mReal;
            Invalid.wBottom = mClient.wTop;
            Invalidate(Invalid);
            mClient.wTop -= What->mReal.Height();
        }
        break;

    case TYPE_STATUS_BAR:
        if (What->Parent() == this)
        {
            mpStatusBar = NULL;
            mClient.wBottom += What->mReal.Height() - 2;
        }
        break;

    case TYPE_TOOL_BAR:
        if (What->Parent() == this)
        {
            mpToolBar = NULL;
            Invalid = What->mReal;
            Invalid.wBottom = mClient.wTop;
            Invalidate(Invalid);
            mClient.wTop -= What->mReal.Height();
        }
        break;

    default:
        break;
    }
    return(PegWindow::Remove(What, bDraw));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegDecoratedWindow::InitClient(void)
{
    PegWindow::InitClient();

    if (mpTitle)
    {
        mClient.wTop += mpTitle->mReal.Height();
    }
    if (mpMenuBar)
    {
        mClient.wTop += mpMenuBar->mReal.Height();
    }
    if (mpStatusBar)
    {
        mClient.wBottom -= mpStatusBar->mReal.Height();
    }
    if (mpToolBar)
    {
        mClient.wTop += mpToolBar->mReal.Height();
    }
}


/*--------------------------------------------------------------------------*/

// End of file
