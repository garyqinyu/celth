/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// robframe.cpp
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


#include "peg.hpp"
#include "statbar.hpp"
#include "RobFrame.hpp"
#include "RoboButn.hpp"

#define NUM_FRAMES 17

extern PegBitmap gbRob01Bitmap;
extern PegBitmap gbFrame3Bitmap;
extern PegBitmap gbFrame5Bitmap;
extern PegBitmap gbFrame7Bitmap;
extern PegBitmap gbFrame9Bitmap;
extern PegBitmap gbFrame11Bitmap;
extern PegBitmap gbFrame13Bitmap;
extern PegBitmap gbFrame15Bitmap;
extern PegBitmap gbFrame17Bitmap;
extern PegBitmap gbFrame19Bitmap;
extern UCHAR     RobotPalette[];

PegBitmap *RobotFrames[] = 
{
    &gbFrame3Bitmap,
    &gbFrame5Bitmap,
    &gbFrame7Bitmap,
    &gbFrame9Bitmap,
    &gbFrame11Bitmap,
    &gbFrame13Bitmap,
    &gbFrame15Bitmap,
    &gbFrame17Bitmap,
    &gbFrame19Bitmap,
    &gbFrame17Bitmap,
    &gbFrame15Bitmap,
    &gbFrame13Bitmap,
    &gbFrame11Bitmap,
    &gbFrame9Bitmap,
    &gbFrame7Bitmap,
    &gbFrame5Bitmap,
    &gbFrame3Bitmap
};

/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresentation)
{
	PegRect Rect;
	Rect.Set(0, 0, 399 + (PEG_FRAME_WIDTH * 2), 319 + (PEG_FRAME_WIDTH * 2));
    RobotFrame *pFrame = new RobotFrame(Rect);
    pPresentation->Center(pFrame);
    pPresentation->Add(pFrame);
}


/*--------------------------------------------------------------------------*/
RobotFrame::RobotFrame(PegRect &Rect) : PegAnimationWindow(Rect, 
    &gbRob01Bitmap, RobotFrames, NUM_FRAMES, Rect.wLeft, Rect.wTop + 107,
    RobotPalette, FF_THICK)
{
    PegRect ButtonRect;

    ButtonRect.wRight = mClient.wRight - 10;
    ButtonRect.wTop = mClient.wTop + 80;
    ButtonRect.wLeft = ButtonRect.wRight - 70;
    ButtonRect.wBottom = ButtonRect.wTop + 24;
    Add(new RoboButton(ButtonRect, IDB_RUN, "Run"));

    ButtonRect.Shift(0, 80);
    ButtonRect.wRight -= 10;
    ButtonRect.wLeft += 10;
    ButtonRect.wBottom = ButtonRect.wTop + 18;
    mpXPos = new PegPrompt(ButtonRect, "0.0", 0, FF_RECESSED|TJ_RIGHT);
    mpXPos->SetColor(PCI_NORMAL, BLACK);
    mpXPos->SetColor(PCI_NTEXT, YELLOW);
    Add(mpXPos);

    ButtonRect.Shift(0, 24);
    mpYPos = new PegPrompt(ButtonRect, "0.0", 0, FF_RECESSED|TJ_RIGHT);
    mpYPos->SetColor(PCI_NORMAL, BLACK);
    mpYPos->SetColor(PCI_NTEXT, YELLOW);    
    Add(mpYPos);

    ButtonRect.Shift(0, 24);
    mpZPos = new PegPrompt(ButtonRect, "0.0", 0, FF_RECESSED|TJ_RIGHT);
    mpZPos->SetColor(PCI_NORMAL, BLACK);
    mpZPos->SetColor(PCI_NTEXT, YELLOW);
    Add(mpZPos);

    ButtonRect.Shift(0, 76);
    ButtonRect.wLeft += 10;
    mpWeldPrompt = new PegPrompt(ButtonRect, "Off", 0, FF_RECESSED|TJ_CENTER);
    mpWeldPrompt->SetColor(PCI_NORMAL, BLACK);
    mpWeldPrompt->SetColor(PCI_NTEXT, RED);
    mpWeldPrompt->SetFont(&SysFont);
    Add(mpWeldPrompt);

    ButtonRect.Set(mClient.wLeft, mClient.wTop, mClient.wRight,
        mClient.wTop + 52);

    mpStatBar = new StatusBar(ButtonRect);

    ButtonRect.wLeft = ButtonRect.wRight - 140;
    ButtonRect.wRight -= 5;
    ButtonRect.wTop += 27;
    ButtonRect.wBottom -= 3;

    PegSlider *pSlider = new PegSlider(ButtonRect, 10, 100, IDS_SPEED,
        FF_RAISED, 10);
    pSlider->SetCurrentValue(50);

    mpStatBar->Add(pSlider);
    Add(mpStatBar);

    mbForward = TRUE;
    mbWeldOn = FALSE;
    mbRunning = FALSE;

    RemoveStatus(PSF_SIZEABLE);

    mTestBitmapCount = 0;
}

/*--------------------------------------------------------------------------*/
SIGNED RobotFrame::Message(const PegMessage &Mesg)
{
    UCHAR uFrame;
    RoboButton *pButton;

    switch (Mesg.wType)
    {
    case PM_TIMER:
        uFrame = muAnimationFrame;
        PegAnimationWindow::Message(Mesg);
        UpdateRobotPosition(uFrame);
        break;
            
    case SIGNAL(IDB_RUN, PSF_CLICKED):
        {
        mpStatBar->StatusSet("Running- Auto Mode");
        pButton = (RoboButton *) Mesg.pSource;
        pButton->Id(IDB_STOP);
        pButton->DataSet("Stop");
        pButton->Draw();
        PegSlider *pSlider = (PegSlider *) mpStatBar->Find(IDS_SPEED);
        Run(ONE_SECOND * 10 / pSlider->GetCurrentValue(), muAnimationFrame);
        mbRunning = TRUE;
        }
        break;

    case SIGNAL(IDB_STOP, PSF_CLICKED):
        Stop();
        pButton = (RoboButton *) Mesg.pSource;
        pButton->Id(IDB_RUN);
        pButton->DataSet("Run");
        pButton->Draw();
        mpStatBar->StatusSet("Stopped");
        mbRunning = FALSE;
        break;

    case SIGNAL(IDS_SPEED, PSF_SLIDER_CHANGE):
        if (mbRunning)
        {
            Stop();
            Run((WORD) (ONE_SECOND * 10 / Mesg.lData), muAnimationFrame);
        }
        break;

    default:
        PegAnimationWindow::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void RobotFrame::Draw(void)
{
    BeginDraw();
    DrawFrame();
    PegAnimationWindow::Draw();
    DrawTitles();
    EndDraw();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void RobotFrame::DrawTitles(void)
{
	PegPoint Put;
    PegColor TextColor(WHITE, BLACK, CF_NONE);

	Put.x = mClient.wRight - 128;
    Put.y = mClient.wTop + 136;
	ShadowText("Current Position", Put, &SysFont, 114);

    Put.y += 24;
    DrawText(Put, "X Axis:", TextColor, &SysFont);
    Put.y += 24;
    DrawText(Put, "Y Axis:", TextColor, &SysFont);
    Put.y += 24;
    DrawText(Put, "Z Axis:", TextColor, &SysFont);

    Put.y += 52;
    ShadowText("Weld Status", Put, &SysFont, 114);
    Put.y += 24;
    DrawText(Put, "Solenoid", TextColor, &SysFont);
}

/*--------------------------------------------------------------------------*/
// These are just dummy position values, display in the position fields on
// the screen for the purposes of the demo application.
/*--------------------------------------------------------------------------*/
char *RobPos[][3] = {
{"0.0", "0.0", "0.0"},
{"5.2", "0.3", "1.2"},
{"7.6", "0.8", "1.4"},
{"10.7", "1.7", "1.5"},
{"14.2", "1.9",  "2.1"},
{"17.2", "4.6", "2.4"},
{"22.3", "8.5", "2.5"},
{"29.4", "14.7", "2.6"},
{"37.1", "21.8", "2.7"},
{"45.4", "26.3", "2.8"},
{"37.1", "21.8", "2.7"},
{"29.4", "14.7", "2.6"},
{"22.3", "8.5", "2.5"},
{"17.2", "4.6", "2.4"},
{"14.2", "1.9",  "2.1"},
{"10.7", "1.7", "1.5"},
{"7.6", "0.8", "1.4"},
{"5.2", "0.3", "1.2"},
{"0.0", "0.0", "0.0"}
};

/*--------------------------------------------------------------------------*/
void RobotFrame::UpdateRobotPosition(UCHAR uFrame)
{
    mpXPos->DataSet(RobPos[uFrame][0]);
    mpXPos->Draw();
    mpYPos->DataSet(RobPos[uFrame][1]);
    mpYPos->Draw();
    mpZPos->DataSet(RobPos[uFrame][2]);
    mpZPos->Draw();

    if (uFrame == NUM_FRAMES - 1)
    {
        mpWeldPrompt->SetColor(PCI_NTEXT, GREEN);
        mpWeldPrompt->DataSet("On");
        mpWeldPrompt->Draw();
        mbWeldOn = TRUE;
    }
    else
    {
        if (mbWeldOn)
        {
            mbWeldOn = FALSE;
            mpWeldPrompt->SetColor(PCI_NTEXT, RED);
            mpWeldPrompt->DataSet("Off");
            mpWeldPrompt->Draw();
        }
    }

}

/*--------------------------------------------------------------------------*/
void RobotFrame::ShadowText(char *pText, PegPoint Put, PegFont *pFont,
	SIGNED iWidth)
{
	PegColor Color(PCLR_SHADOW, PCLR_SHADOW, CF_NONE);
	DrawText(Put, pText, Color, pFont);
	Color.uForeground = WHITE;
	SIGNED iHeight = TextHeight("H", pFont);

	if (iHeight > 20)
	{
		Put.x -= 2;
		Put.y -= 2;
	}
	else
	{
		Put.x--;
		Put.y--;
	}
	DrawText(Put, pText, Color, pFont);

	if (iWidth >= 0)
	{
		if (!iWidth)	// 0 means equal to text width:
		{
			iWidth = TextWidth(pText, pFont);
		}
		// now draw the shadowed underline:

		Put.y += iHeight;
		Color.uForeground = WHITE;
		Line(Put.x, Put.y, Put.x + iWidth, Put.y, Color, 3);

		Put.y += 3;
		Color.uForeground = PCLR_SHADOW;
		Line(Put.x + 1, Put.y, Put.x + iWidth + 1, Put.y, Color);
		Put.x += iWidth + 1;
		Line(Put.x, Put.y - 3, Put.x, Put.y, Color);
	}
}
