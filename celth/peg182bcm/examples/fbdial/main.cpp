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

extern PegBitmap gbdialbkg3Bitmap;
extern PegBitmap gbdialbkg2Bitmap;
extern PegBitmap gbdialbkg4Bitmap;
extern PegBitmap gbAbFluidBitmap;
extern PegBitmap gbDialAnchor1Bitmap;

#if defined(PEG_UNICODE)
static PEGCHAR gsWindowTitle[] = {'P', 'e', 'g', 'B', 'i', 't', 'm', 'a', 'p',
                                  'D', 'i', 'a', 'l', ' ', 'E', 'x', 'a', 'm',
                                  'p', 'l', 'e', '\0'};
static PEGCHAR gsChangeColor[] = {'C', 'h', 'a', 'n', 'g', 'e', ' ', 'C', 'o',
                                  'l', 'o', 'r', '\0'};
static PEGCHAR gsZero[] = {'0', '\0'};
#else
static PEGCHAR gsWindowTitle[] = "PegBitmapDial Example";
static PEGCHAR gsChangeColor[] = "Change Color";
static PEGCHAR gsZero[] = "0";
#endif

class DialWindow : public PegDecoratedWindow
{
    public:
        DialWindow(const PegRect& Rect);
        ~DialWindow();

        SIGNED Message(const PegMessage& Mesg);

        enum ChildID
        {
            IDB_CHANGECOLOR = 1
        };

    private:
        PegDial* mpDial1;
        PegFiniteBitmapDial* mpDial2;
        PegFiniteBitmapDial* mpDial3;

        SIGNED miDial1Val;
        SIGNED miDial2Val;
        SIGNED miDial1Dir;
        SIGNED miDial2Dir;
};

DialWindow::DialWindow(const PegRect& Rect) :
    PegDecoratedWindow(Rect)
{
    Add(new PegTitle(gsWindowTitle));

    miDial1Dir = 1;
    miDial2Dir = 1;
    miDial1Val = 0;
    miDial2Val = 0;

    PegRect WinRect;
    
    WinRect.Set(50, 50, 199, 199);
    mpDial1 = new PegFiniteBitmapDial(WinRect, 180, 0, 0, 100,
                                      &gbdialbkg2Bitmap);

    WinRect.Shift(160, 0);
    mpDial2 = new PegFiniteBitmapDial(WinRect, 225, 315, -25, 125,
                                      &gbdialbkg3Bitmap,
                                      &gbDialAnchor1Bitmap);
    mpDial2->Style((mpDial2->Style() | DS_POLYNEEDLE) & ~DS_THINNEEDLE); 
    mpDial2->SetNeedleColor(BLUE);
    
    Add(mpDial1);
    Add(new PegPrompt(105, 210, 40, gsZero, 101, FF_RECESSED | TJ_RIGHT |
                      TT_COPY));
    Add(mpDial2);
    Add(new PegPrompt(265, 210, 40, gsZero, 102, FF_RECESSED | TJ_RIGHT |
                      TT_COPY));

    WinRect.Shift(160, 0);
    mpDial3 = new PegFiniteBitmapDial(WinRect, 180, 90, 0, 100,
                                      &gbdialbkg4Bitmap, NULL,
                                      0);
    mpDial3->Style(FF_NONE | AF_TRANSPARENT | DS_THICKNEEDLE | DS_USERCOR |
                   DS_CLOCKWISE);
    mpDial3->SetCOR(123, 123);
    mpDial3->SetNeedleLength(65);
    mpDial3->SetColor(PCI_NORMAL, BLUE);
    Add(mpDial3);

    SetColor(PCI_NORMAL, GREEN);

    Add(new PegPrompt(425, 210, 40, gsZero, 101, FF_RECESSED | TJ_RIGHT |
                      TT_COPY));

    WinRect.Set(245, 240, 330, 270);
    Add(new PegTextButton(WinRect, gsChangeColor, IDB_CHANGECOLOR));
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
       #if defined(X11LYNX)
        SetTimer(1, 2, 2);
       #else
        SetTimer(1, ONE_SECOND / 10, ONE_SECOND / 10);
       #endif
        break;

    case PM_HIDE:
        PegWindow::Message(Mesg);
        KillTimer(1);
        break;

    case PM_TIMER:
        {
            if(miDial1Val <= 0)
            {
                miDial1Dir = 1;
            }
            else if(miDial1Val >= 100)
            {
                miDial1Dir = -1;
            }
            miDial1Val += miDial1Dir;
            mpDial1->SetValue(miDial1Val);
            mpDial3->SetValue(miDial1Val);

            if(miDial2Val <= -25)
            {
                miDial2Dir = 1;
            }
            else if(miDial2Val >= 125 )
            {
                miDial2Dir = -1;
            }
            miDial2Val += miDial2Dir;
            mpDial2->SetValue(miDial2Val);

            PEGCHAR szTemp[10];
            PegThing* pTest = First();
            while(pTest)
            {
                if(pTest->Id() == 101 || pTest->Id() == 102)
                {
                    PegPrompt* pPrompt = (PegPrompt*) pTest;
                    if(pPrompt->Id() == 101)
                    {
                        ltoa(miDial1Val, szTemp, 10);
                    }
                    else
                    {
                        ltoa(miDial2Val, szTemp, 10);
                    }

                    pPrompt->DataSet(szTemp);
                    pPrompt->Draw();                    
                }
                pTest = pTest->Next();
            }

        } break;

    case SIGNAL(IDB_CHANGECOLOR, PSF_CLICKED):
        {
            COLORVAL tNewColor = GetColor(PCI_NORMAL);
            if(++tNewColor > WHITE)
            {
                tNewColor = BLACK;
            }
            SetColor(PCI_NORMAL, tNewColor);
            Invalidate();
            Draw();
        } break;

    default:
        return(PegDecoratedWindow::Message(Mesg));
    }

    return 0;
}

void PegAppInitialize(PegPresentationManager* pPresentation)
{    
    //pPresentation->SetWallpaper(&gbAbFluidBitmap);
    
    PegRect Rect;
    Rect.Set(20, 20, 550, 280);
    DialWindow* pWindow = new DialWindow(Rect);
    pPresentation->Add(pWindow);
}

