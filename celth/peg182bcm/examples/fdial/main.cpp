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
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

class DialWindow : public PegDecoratedWindow
{
    public:
        DialWindow(const PegRect& Rect);
        ~DialWindow();

        SIGNED Message(const PegMessage& Mesg);

    private:
        PegDial* mpDial1;
        PegDial* mpDial2;
		PegDial* mpDial3;

        SIGNED miDial1Val;
        SIGNED miDial2Val;
        SIGNED miDial1Dir;
        SIGNED miDial2Dir;
        SIGNED miDial3Val;
};

DialWindow::DialWindow(const PegRect& Rect) :
    PegDecoratedWindow(Rect)
{
    Add(new PegTitle("PegDial Example"));

    miDial1Dir = 1;
    miDial2Dir = 1;
    miDial1Val = 0;
    miDial2Val = 0;
    miDial3Val = 0;

    PegRect WinRect;
    
    WinRect.Set(50, 50, 200, 200);
    mpDial1 = new PegFiniteDial(WinRect, 180, 0, -50, 100, AF_TRANSPARENT | DS_STANDARDSTYLE);
    //mpDial1 = new PegFiniteDial(50, 50, 180, 0, -50, 100, AF_TRANSPARENT | DS_STANDARDSTYLE);
    //mpDial1 = new PegCircularDial(WinRect, 270, 360, 0, 720, DS_STANDARDSTYLE);
    mpDial1->SetTicFreq(10);
    mpDial1->SetTicLen(10);
	mpDial1->SetDialColor(CYAN);
    mpDial1->RemoveStatus(PSF_VIEWPORT);

    WinRect.Shift(160, 0);
    mpDial2 = new PegFiniteDial(WinRect, 225, 315, 0, 300, FF_RAISED);
    //mpDial2 = new PegFiniteDial(210, 50, 225, 315, 0, 300, FF_RAISED);
    mpDial2->Style(mpDial2->Style() | DS_THICKNEEDLE | DS_TICMARKS | DS_RECTCOR | DS_CLOCKWISE);
    mpDial2->SetColor(PCI_NORMAL, DARKGRAY);
    mpDial2->SetNeedleColor(BLUE);
    mpDial2->SetTicFreq(50);
    mpDial2->SetTicLen(20);
    
	WinRect.Shift(160, 0);
    //mpDial3 = new PegFiniteDial(WinRect, 225, 315, 0, 300, FF_RECESSED);
    //mpDial3 = new PegFiniteDial(370, 50, 225, 315, 0, 300, FF_RECESSED);
    mpDial3 = new PegCircularDial(WinRect, 270, 300, 0, 900, FF_RECESSED);
    mpDial3->Style(mpDial3->Style() | DS_POLYNEEDLE | DS_TICMARKS | DS_RECTCOR | DS_CLOCKWISE);
    mpDial3->SetColor(PCI_NORMAL, LIGHTGRAY);
	mpDial3->SetDialColor(GREEN);
    mpDial3->SetNeedleColor(YELLOW);
    mpDial3->SetTicFreq(10);
    mpDial3->SetTicLen(20);
	    
    Add(mpDial1);
    PegPrompt* pPrompt = new PegPrompt(105, 210, 40, "0", 101, FF_RECESSED | TJ_CENTER | TT_COPY);
    pPrompt->SetColor(PCI_NTEXT, RED);
    Add(pPrompt);
    //mpDial1->Add(pPrompt);
        
    Add(mpDial2);
    pPrompt = new PegPrompt(265, 210, 40, "0", 102, FF_RECESSED | TJ_CENTER | TT_COPY);
    pPrompt->SetColor(PCI_NTEXT, BLUE);
    Add(pPrompt);
    //mpDial2->Add(pPrompt);

    pPrompt = new PegPrompt(425, 210, 40, "0", 103, FF_RECESSED | TJ_CENTER | TT_COPY);
    pPrompt->SetColor(PCI_NTEXT, GREEN);
    Add(pPrompt);
	Add(mpDial3);
}

DialWindow::~DialWindow()
{
}

SIGNED DialWindow::Message(const PegMessage& Mesg)
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
            if(miDial1Val < -50)
            {
                miDial1Dir = 1;
            }
            else if(miDial1Val > 100)
            {
                miDial1Dir = -1;
            }
            miDial1Val += miDial1Dir;
            mpDial1->SetValue(miDial1Val);

            PEGCHAR szTemp[10];
            PegPrompt* pPrompt = (PegPrompt*)Find(101);
            if(pPrompt)
            {
                ltoa(miDial1Val, szTemp, 10);
                pPrompt->DataSet(szTemp);
                pPrompt->Draw();
            }

            if(miDial2Val < 0)
            {
                miDial2Dir = 1;
            }
            else if(miDial2Val > 300)
            {
                miDial2Dir = -1;
            }
            miDial2Val += miDial2Dir;
            mpDial2->SetValue(miDial2Val);
			mpDial3->SetValue(++miDial3Val);

            pPrompt = (PegPrompt*)Find(102);
            if(pPrompt)
            {
                ltoa(miDial2Val, szTemp, 10);
                pPrompt->DataSet(szTemp);
                pPrompt->Draw();
            }

            pPrompt = (PegPrompt*)Find(103);
            if(pPrompt)
            {
                ltoa(miDial3Val, szTemp, 10);
                pPrompt->DataSet(szTemp);
                pPrompt->Draw();
            }

        } break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }

    return 0;
}

void PegAppInitialize(PegPresentationManager* pPresentation)
{    
    PegRect Rect;
    Rect.Set(20, 20, 560, 250);
    DialWindow* pWindow = new DialWindow(Rect);
    pPresentation->Add(pWindow);
}

