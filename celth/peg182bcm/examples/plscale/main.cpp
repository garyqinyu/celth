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
// Notes: Example of using the PegLinearScale
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager*);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class ScaleWindow : public PegDecoratedWindow
{
    public:
        ScaleWindow(const PegRect& Rect);
        ~ScaleWindow();

        SIGNED Message(const PegMessage& Mesg);

    private:
        PegLinearScale* mpScale1;
        PegLinearScale* mpScale2;
        PegLinearScale* mpScale3;
        PegLinearScale* mpScale4;
        PegLinearScale* mpScale5;
        PegLinearScale* mpScale6;

        LONG mlScale1Val;
        SIGNED miScale1Dir;
};

/*--------------------------------------------------------------------------*/
ScaleWindow::ScaleWindow(const PegRect& Rect) :
    PegDecoratedWindow(Rect)
{
    Id(100);

    Add(new PegTitle("PegLinearScale Example"));

    PegRect WinRect;    
    WinRect.Set(50, 50, 100, 200);
    mpScale1 = new PegLinearScale(WinRect, 0, 100, 10, AF_TRANSPARENT | SS_STANDARDSTYLE);
    mlScale1Val = 0;
    miScale1Dir = -1;

    WinRect.Set(110, 50, 200, 200);
    mpScale2 = new PegLinearScale(WinRect, 0, 100, 10, FF_THIN | SS_STANDARDSTYLE);
    mpScale2->Style(mpScale2->Style() & ~SS_FACELEFT);

   #if PEG_NUM_COLORS >= 16
    mpScale2->SetNeedleColor(BLUE);
   #endif

    WinRect.Set(210, 50, 280, 300);
    mpScale3 = new PegLinearScale(WinRect, 0, 100, 10, FF_THIN | SS_STANDARDSTYLE);
    mpScale3->SetNeedleColor(CYAN);
    mpScale3->Style(mpScale3->Style() & ~SS_BOTTOMTOTOP);
    
    WinRect.Set(290, 50, 320, 310);
    mpScale4 = new PegLinearScale(WinRect, -100, 100, 10);
    mpScale4->Style(mpScale4->Style() & ~SS_FACELEFT);
    mpScale4->SetValue(0);
   #if PEG_NUM_COLORS >= 16
    mpScale4->SetNeedleColor(YELLOW);
   #endif

    WinRect.Set(50, 210, 200, 260);
    mpScale5 = new PegLinearScale(WinRect, 0, 100, 10);
    mpScale5->Style(mpScale5->Style() & ~SS_ORIENTVERT);
   #if PEG_NUM_COLORS >= 16
    mpScale5->SetNeedleColor(GREEN);
   #endif
    
    WinRect.Set(50, 270, 200, 320);
    mpScale6 = new PegLinearScale(WinRect, 0, 100, 5, FF_THIN | SS_STANDARDSTYLE);
    mpScale6->Style(mpScale6->Style() & ~SS_ORIENTVERT);
    mpScale6->Style(mpScale6->Style() & ~SS_FACELEFT);
    mpScale6->Style(mpScale6->Style() & ~SS_BOTTOMTOTOP);
   #if PEG_NUM_COLORS >= 16
    mpScale6->SetNeedleColor(MAGENTA);
   #endif

    Add(mpScale1);
    Add(mpScale2);
    Add(mpScale3);
    Add(mpScale4);
    Add(mpScale5);
    Add(mpScale6);
}

/*--------------------------------------------------------------------------*/
ScaleWindow::~ScaleWindow()
{
}

/*--------------------------------------------------------------------------*/
SIGNED ScaleWindow::Message(const PegMessage& Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
        PegWindow::Message(Mesg);
        SetTimer(1, ONE_SECOND / 10, ONE_SECOND / 10);
        break;

    case PM_HIDE:
        PegWindow::Message(Mesg);
        KillTimer(1);
        break;

    case PM_TIMER:
        {
            if(mlScale1Val <= 0)
            {
                miScale1Dir = 1;
            }
            else if(mlScale1Val >= 100)
            {
                miScale1Dir = -1;
            }
            mlScale1Val += miScale1Dir;
            mpScale1->SetValue(mlScale1Val);
            mpScale2->SetValue(mlScale1Val);
            mpScale3->SetValue(mlScale1Val);
            //mpScale4->SetValue(mlScale1Val);
            mpScale5->SetValue(mlScale1Val);
            mpScale6->SetValue(mlScale1Val);
        } break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager* pPresentation)
{    
    PegRect Rect;
    Rect.Set(20, 20, 350, 350);
    ScaleWindow* pWindow = new ScaleWindow(Rect);
    pPresentation->Center(pWindow);
    pPresentation->Add(pWindow);
}

