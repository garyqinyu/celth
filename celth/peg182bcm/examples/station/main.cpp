/*--------------------------------------------------------------------------*/
// main.cpp - Station example using peg HMI and chart classes
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-2002 Swell Software, Inc. 
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

SIGNED giChartData[] = { 100, 100, 100, 100, 100, 100, 125, 150, 175, 200, 
                         150, 100, 50, 100, 125, 100, 100, 100 };

SIGNED giBukData[] = { 100, 100, 100, 100, 100, 100, 100, 100, 125, 100, 
                       100, 120, 140, 160, 180, 200, 150, 100, 50, 125, 100,
                       100, 125, 150, 125, 100, 100, 100, 100 };


extern PegBitmap gbbpBitmap;
extern PegBitmap gbpulseBitmap;
extern PegBitmap gbrateBitmap;

/*--------------------------------------------------------------------------*/
class StationWindow : public PegWindow
{
    public:
        StationWindow(const PegRect& Rect, PEGCHAR* pTitle);
        ~StationWindow() {}

        SIGNED Message(const PegMessage& Mesg);

        PegPrompt* mpTitle;

        PegColorLight* mpPulseLight;
        PegColorLight* mpCrashLight;
        PegStripChart* mpChart;
        PegLinearBitmapScale* mpPulse;
        PegLinearBitmapScale* mpRate;
        PegFiniteBitmapDial* mpBP;

        WORD mwIndex;
        UCHAR mcID;

        BOOL mbCrashWindow;
        BOOL mbBukWindow;

        SIGNED nCount;
        LONG lData;
        SIGNED iDataDir;
        WORD wDataLimit;
};

/*--------------------------------------------------------------------------*/
StationWindow::StationWindow(const PegRect& Rect, PEGCHAR* pTitle) :
    PegWindow(Rect, FF_THIN)
{
    muColors[PCI_NORMAL] = BLACK;
    muColors[PCI_SELECTED] = BLUE;
    muColors[PCI_NTEXT] = WHITE;
    muColors[PCI_STEXT] = YELLOW;

    mbCrashWindow = FALSE;
    mbBukWindow = FALSE;

    nCount = 0;
    lData = 0;
    iDataDir = 1;
    wDataLimit = 40;

    PegRect WinRect;
    WinRect = mClient;
    WinRect.wBottom = WinRect.wTop + 20;
    WinRect.wRight = WinRect.wLeft + 250;
    mpTitle = new PegPrompt(WinRect, pTitle, 0, FF_NONE | AF_TRANSPARENT | TJ_LEFT);
    mpTitle->muColors[PCI_NTEXT] = WHITE;
    Add(mpTitle);

    WinRect.wTop = WinRect.wBottom;
    WinRect.wBottom = mClient.wBottom;
    WinRect.wRight = WinRect.wLeft + 250;
    mpChart = new PegStripChart(WinRect, 100, 0, 200, 0, 0);
    mpChart->SetExStyle(CS_PAGED | CS_DRAWAGED);
    mcID = mpChart->AddLine(LIGHTGREEN, 100);
    mwIndex = 0;
    Add(mpChart);

    WinRect.wTop = mClient.wTop + 20;
    WinRect.wLeft = WinRect.wRight + 10;
    WinRect.wRight = WinRect.wLeft + 20;
    WinRect.wBottom = WinRect.wTop + 20;
    mpPulseLight = new PegColorLight(WinRect, FF_THIN, 2);
    mpPulseLight->SetStateColor(0, BLACK);
    mpPulseLight->SetStateColor(1, LIGHTGREEN);
    Add(mpPulseLight);

    WinRect.wBottom = mClient.wBottom - 20;
    WinRect.wTop = WinRect.wBottom - 20;
    mpCrashLight = new PegColorLight(WinRect,FF_THIN,  2);
    mpCrashLight->SetStateColor(0, BLACK);
    mpCrashLight->SetStateColor(1, LIGHTRED);
    Add(mpCrashLight);

    WinRect.wLeft = WinRect.wRight + 10;
    WinRect.wTop = mClient.wTop + 10;
    WinRect.wRight = WinRect.wLeft + 150 - 1;
    WinRect.wBottom = WinRect.wTop + 40 - 1;
    //mpPulse = new PegLinearBitmapScale(WinRect, SS_BOTTOMTOTOP | SS_USERTRAVEL | SS_POLYNEEDLE,
    //                                   -200, 200, 6, 194, &gbpulseBitmap);
    mpPulse = new PegLinearBitmapScale(WinRect, -200, 200, 6, 194, &gbpulseBitmap, NULL,
                                       SS_BOTTOMTOTOP | SS_USERTRAVEL | SS_POLYNEEDLE);
    Add(mpPulse);

    WinRect.wTop = WinRect.wBottom + 10;
    WinRect.wBottom = WinRect.wTop + 40;
    //mpRate = new PegLinearBitmapScale(WinRect, SS_BOTTOMTOTOP | SS_USERTRAVEL | SS_FACELEFT | SS_POLYNEEDLE,
    //                                  -200, 200, 6, 194, &gbrateBitmap);
    mpRate = new PegLinearBitmapScale(WinRect, -200, 200, 6, 194, &gbrateBitmap, NULL,
                                      SS_BOTTOMTOTOP | SS_USERTRAVEL | SS_FACELEFT | SS_POLYNEEDLE);

    Add(mpRate);

    WinRect.wRight = mClient.wRight;
    WinRect.wTop = mClient.wTop;
    WinRect.wBottom = WinRect.wTop + 100 - 1;
    WinRect.wLeft = WinRect.wRight - 100 + 1;
    //mpBP = new PegFiniteBitmapDial(WinRect, DS_STANDARDSTYLE, 225, 315, 0, 120, &gbbpBitmap);
    mpBP = new PegFiniteBitmapDial(WinRect, 225, 315, 0, 120, &gbbpBitmap, NULL);
    mpBP->Style(mpBP->Style() & ~DS_TICMARKS);
    mpBP->Style(mpBP->Style() & ~DS_THINNEEDLE);
    mpBP->Style(mpBP->Style() | DS_THICKNEEDLE);
    Add(mpBP);
}

/*--------------------------------------------------------------------------*/
SIGNED StationWindow::Message(const PegMessage& Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
        PegWindow::Message(Mesg);
        SetTimer(1, 1, 1);//ONE_SECOND / 10, ONE_SECOND / 10);

        // If it's the Buk window, scroll the strip chart...just for fun.
        if(mbBukWindow)
        {
            mpChart->SetExStyle(CS_SCROLLED);
        }
        break;

    case PM_HIDE:
        PegWindow::Message(Mesg);
        KillTimer(1);
        break;

    case PM_TIMER:
        {
            // Update the chart
            if(!mbCrashWindow)
            {
                if(!mbBukWindow)
                {
                    if(mwIndex >= 18)
                    {
                        mwIndex = 0;
                    }
                    mpChart->AddData(mcID, giChartData[mwIndex]);

                    // Update the pulse light
                    if(giChartData[mwIndex] >= 150)
                    {
                        mpPulseLight->SetState(1);
                    }
                    else
                    {
                        mpPulseLight->SetState(0);
                    }
                    mwIndex++;

                    // Set up the pulse scale
                    if(lData >= wDataLimit)
                    {
                        iDataDir = -1;
                    }
                    else if(lData <= wDataLimit - 120)
                    {
                        iDataDir = 1;
                    }
                    lData += iDataDir;

                    mpPulse->SetValue(lData);
                    mpRate->SetValue(lData);
                    mpBP->SetValue((lData + 200) / 2);
                }
                else
                {
                    if(mwIndex >= 29)
                    {
                        mwIndex = 0;
                    }
                    
                    mpChart->AddData(mcID, giBukData[mwIndex]);

                    // Update the pulse light
                    if(giBukData[mwIndex] >= 150)
                    {
                        mpPulseLight->SetState(1);
                    }
                    else
                    {
                        mpPulseLight->SetState(0);
                    }
                    mwIndex++;
                    
                    if(lData >= wDataLimit - 50)
                    {
                        iDataDir = -1;
                    }
                    else if(lData <= wDataLimit - 200)
                    {
                        iDataDir = 2;
                    }
                    lData += iDataDir;

                    mpPulse->SetValue(lData);
                    mpRate->SetValue(lData);
                    mpBP->SetValue((lData + 200) / 2);
                }
            }

            // Update the crash light, if I'm a crash window
            if(mbCrashWindow)
            {
                if(nCount % 4 == 0)
                {
                    mpCrashLight->IncrementState();
                    nCount = 0;
                }

                nCount++;

                mpChart->AddData(mcID, 100);
                mpPulse->SetValue(-175);
                mpRate->SetValue(-175);
                mpBP->SetValue(0);
            }

            //Invalidate();
            //Draw();

        } break;

    default:
        return(PegWindow::Message(Mesg));
    }

    return(0);
}

/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager* pPresentation)
{
    PegRect Rect;
    Rect.Set(10, 10, 600, 439);
    PegDecoratedWindow* pWindow = new PegDecoratedWindow(Rect);
    pWindow->Add(new PegTitle("Peg Station Example"));

    Rect = pWindow->mClient;
    Rect.wBottom = Rect.wTop + 100;
    pWindow->Add(new StationWindow(Rect, "Room 627 - McCullaugh, P."));

    Rect.wTop = Rect.wBottom;
    Rect.wBottom += 100;
    StationWindow* pStatWindow = new StationWindow(Rect, "Room 629 - Spongen, N.");
    pStatWindow->mbCrashWindow = TRUE;
    pWindow->Add(pStatWindow);

    Rect.wTop = Rect.wBottom;
    Rect.wBottom = Rect.wTop + 100;
    pWindow->Add(new StationWindow(Rect, "Room 631 - Colwell, H."));

    Rect.wTop = Rect.wBottom;
    Rect.wBottom = Rect.wTop + 100;
    pStatWindow = new StationWindow(Rect, "Room 633 - Bukowski, C.");
    pStatWindow->mbBukWindow = TRUE;
    pWindow->Add(pStatWindow);

    pPresentation->Add(pWindow);
}

