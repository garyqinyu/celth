/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// main.cpp
//
// Author: Jim DeLisle
//
// Date: 09/26/1999
//
// Copyright (c) 1993-1999 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes: Example of using the PegLinearBitmapScale
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

extern PegBitmap gbbluebkgBitmap;
extern PegBitmap gbgreybkgBitmap;
extern PegBitmap gbforddbBitmap;
extern PegBitmap gbsignalBitmap;

class ScaleWindow : public PegDecoratedWindow
{
    public:
        ScaleWindow(const PegRect& Rect);
        ~ScaleWindow();

        SIGNED Message(const PegMessage& Mesg);

    private:
        PegLinearBitmapScale* mpScale1;
        PegLinearBitmapScale* mpScale2;
        PegLinearBitmapScale* mpScale3;
        PegLinearBitmapScale* mpScale4;

        PegPrompt* mpValPrompt;

        LONG mlScale1Val;
        SIGNED miScale1Dir;
};

ScaleWindow::ScaleWindow(const PegRect& Rect) :
    PegDecoratedWindow(Rect)
{
    Add(new PegTitle("PegLinearBitmapScale Example"));

    PegRect WinRect;    
    mlScale1Val = 0;
    miScale1Dir = -1;

    WinRect.Set(50, 50, 125, 250);
    mpScale1 = new PegLinearBitmapScale(WinRect, 0, 100, 120, 23, &gbbluebkgBitmap);
    mpScale1->SetNeedleColor(WHITE);
    mpScale1->Style(mpScale1->Style() & ~SS_POLYNEEDLE);
    mpScale1->Style(mpScale1->Style() | SS_THICKNEEDLE);
    
    WinRect.Set(57, 215, 115, 240);
    mpValPrompt = new PegPrompt(WinRect, "0", 101, FF_RECESSED | TJ_CENTER | TT_COPY);
    mpScale1->Add(mpValPrompt, FALSE);

    WinRect.Set(215, 100, 290, 300);
    mpScale2 = new PegLinearBitmapScale(WinRect, 0, 100, 180, 20, &gbgreybkgBitmap);
    mpScale2->SetNeedleColor(RED);
    mpScale2->Style(mpScale2->Style() & ~SS_FACELEFT);
    mpScale2->Style(mpScale2->Style() & ~SS_BOTTOMTOTOP);


    
    WinRect.Set(135, 50, 205, 300);
    mpScale3 = new PegLinearBitmapScale(WinRect, 0, 100, 167, 15, &gbforddbBitmap);
    mpScale3->Style(mpScale3->Style() & ~SS_POLYNEEDLE);
    mpScale3->Style(mpScale3->Style() | SS_THICKNEEDLE);

    WinRect.Set(215, 50, 415, 90);
    mpScale4 = new PegLinearBitmapScale(WinRect, 0, 100, 3, 196, &gbsignalBitmap);
    mpScale4->Style(SS_BOTTOMTOTOP | SS_FACELEFT | SS_THINNEEDLE | SS_USERTRAVEL);
    
    Add(mpScale1);
    Add(mpScale2);
    Add(mpScale3);
    Add(mpScale4);

}

ScaleWindow::~ScaleWindow()
{
}

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
            mpScale4->SetValue(mlScale1Val);
            
            PEGCHAR szBuff[10];
            _ltoa(mlScale1Val, szBuff, 10);
            mpValPrompt->DataSet(szBuff);
            mpValPrompt->Draw();
            
        } break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }

    return 0;
}

void PegAppInitialize(PegPresentationManager* pPresentation)
{    
    PegRect Rect;
    Rect.Set(20, 20, 500, 350);
    ScaleWindow* pWindow = new ScaleWindow(Rect);
    pPresentation->Add(pWindow);
}

