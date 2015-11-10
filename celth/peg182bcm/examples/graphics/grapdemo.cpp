/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// grapdemo.cpp - Extended graphics demo.
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
#include "grapdemo.hpp"


/*--------------------------------------------------------------------------*/
// PegAppInitialize- called by the PEG library during program startup.
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresentation)
{
	// create the dialog and add it to PegPresentationManager:

	PegRect Rect;
	Rect.Set(0, 0, 200, 200);
	PegWindow *pWin = new GraphicsWin(Rect);
	pPresentation->Center(pWin);
	pPresentation->Add(pWin);

    Rect= pWin->mReal;
    Rect.Shift(20, 20);
    pWin = new ArcWin(Rect);
    pPresentation->AddToEnd(pWin);

    Rect.Shift(20, 20);
    pWin = new CircleWin(Rect);
    pPresentation->Add(pWin);

    Rect.Shift(20, 20);
    pWin = new EllipseWin(Rect);
    pPresentation->Add(pWin);
}

/*--------------------------------------------------------------------------*/
// GraphicsWin- Just a PegWindow with an overridden Draw() function. 
/*--------------------------------------------------------------------------*/
GraphicsWin::GraphicsWin(const PegRect &Rect) :
    PegDecoratedWindow(Rect)
{
    Add(new PegTitle("pLine, Polygon, Circle"));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GraphicsWin::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();
    PegRect OldClip = mClip;
    mClip &= mClient;

    PegPoint PolyPoints[10];

    PegColor PolyColor(LIGHTGRAY, LIGHTGRAY, CF_FILL);
    Line(mClient.wLeft, mClient.wTop, mClient.wRight, mClient.wBottom,
        PolyColor, 10);
    Line(mClient.wLeft, mClient.wBottom, mClient.wRight, mClient.wTop,
        PolyColor, 10);

    SIGNED Radius = mClient.Width() / 6;
    SIGNED iStep = mClient.Height() / 3;

    Screen()->PatternLine(this, mClient.wLeft, mClient.wTop + iStep, mClient.wRight,
        mClient.wBottom - iStep, PolyColor, 4, 0xf0f0f0f0);
    Screen()->PatternLine(this, mClient.wLeft, mClient.wBottom - iStep, mClient.wRight,
        mClient.wTop + iStep, PolyColor, 4, 0xf0f0f0f0);

    SIGNED xCenter = (mClient.wLeft + mClient.wRight) / 2;
    SIGNED yCenter = (mClient.wTop + mClient.wBottom) / 2;

    PolyPoints[0].x = xCenter - Radius;
    PolyPoints[0].y = mClient.wBottom - 1;
    PolyPoints[1].x = xCenter;
    PolyPoints[1].y = mClient.wBottom - iStep - 1;
    PolyPoints[2].x = xCenter + Radius;
    PolyPoints[2].y = mClient.wBottom - 1;
    PolyPoints[3] = PolyPoints[0];
    PolyColor.Set(GREEN, GREEN, CF_FILL);
    Screen()->Polygon(this, PolyPoints, 4, PolyColor);

    PolyPoints[0].y = mClient.wTop + 1;
    PolyPoints[1].y = mClient.wTop + iStep + 1;
    PolyPoints[2].y = mClient.wTop + 1;
    PolyPoints[3] = PolyPoints[0];
    PolyColor.Set(GREEN, GREEN, CF_FILL);
    Screen()->Polygon(this, PolyPoints, 4, PolyColor);

    Radius = mClient.Width() / 5;
    iStep = Radius / 3;

    PolyColor.Set(RED, RED, CF_FILL);
    PolyPoints[0].x = xCenter - iStep;
    PolyPoints[0].y = yCenter - Radius;
    PolyPoints[1].x = xCenter + iStep;
    PolyPoints[1].y = yCenter - Radius;
    PolyPoints[2].x = xCenter + Radius;
    PolyPoints[2].y = yCenter - iStep;
    PolyPoints[3].x = xCenter + Radius;
    PolyPoints[3].y = yCenter + iStep;
    PolyPoints[4].x = xCenter + iStep;
    PolyPoints[4].y = yCenter + Radius;
    PolyPoints[5].x = xCenter - iStep;
    PolyPoints[5].y = yCenter + Radius;
    PolyPoints[6].x = xCenter - Radius;
    PolyPoints[6].y = yCenter + iStep;
    PolyPoints[7].x = xCenter - Radius;
    PolyPoints[7].y = yCenter - iStep;
    PolyPoints[8] = PolyPoints[0];
    Screen()->Polygon(this, PolyPoints, 9, PolyColor);

    Radius = mClient.Width() / 8;
    PolyColor.Set(WHITE, BLACK, CF_FILL);
    Screen()->Circle(this, xCenter, yCenter, Radius, PolyColor, 1 + Radius / 8);
    mClip = OldClip;
    EndDraw();
}


/*--------------------------------------------------------------------------*/
// ArcWin- Just a PegWindow with an overridden Draw() function. Draws an
// arc in the window client area.
/*--------------------------------------------------------------------------*/
ArcWin::ArcWin(const PegRect &Rect) :
    PegDecoratedWindow(Rect)
{
    Add(new PegTitle("Arcs"));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ArcWin::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();
    PegRect OldClip = mClip;
    mClip &= mClient;

    SIGNED xc, yc, r;

    xc = (mClient.wLeft + mClient.wRight) / 2;
    yc = (mClient.wTop  + mClient.wBottom) / 2;

    if (mClient.Width() >= mClient.Height())
    {
        r = mClient.Height() / 3;
    }
    else
    {
        r = mClient.Width() / 3;
    }

    PegColor ArcColor(RED, GREEN, CF_FILL);

    Screen()->Arc(this, xc, yc, r, -180, 45, ArcColor, 4);

    mClip = OldClip;
    EndDraw();
}

/*--------------------------------------------------------------------------*/
// CircleWin: Overrides the draw function to draw some circles.
/*--------------------------------------------------------------------------*/
CircleWin::CircleWin(const PegRect &Rect) :
    PegDecoratedWindow(Rect)
{
    Add(new PegTitle("Circles"));
    SetColor(PCI_NORMAL, BLACK);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CircleWin::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();
    PegRect OldClip = mClip;
    mClip &= mClient;
    PegColor p(BLACK,WHITE); 

    SIGNED xc, yc, r;

    xc = (mClient.wLeft + mClient.wRight) / 2;
    yc = (mClient.wTop  + mClient.wBottom) / 2;
    r = mClient.Width() / 3;

    PegColor Color(RED, WHITE, CF_FILL);

    Screen()->Circle(this, xc, yc, r, Color, 3);

    xc -= r / 4;
    yc -= r / 4;

    Color.uFlags = CF_FILL;
    Color.uForeground = WHITE;
    //Screen()->Circle(this, xc, yc, r / 4, Color, 3);
    Circle(xc, yc, 20,p,1); 

    yc += r / 2;

    Color.uFlags = CF_FILL;
    Color.uForeground = BLACK;
    Color.uBackground = WHITE;
    //Screen()->Circle(this, xc, yc, r / 4, Color, 0);
    Circle(xc, yc, 40,p,1); 

    xc += r / 2;

    Color.uFlags = CF_NONE;
    Color.uForeground = CYAN;
    Color.uBackground = WHITE;
    //Screen()->Circle(this, xc, yc, r / 4, Color, 4);
    Circle(xc, yc, 80,p,1); 

    yc -= r / 2;

    Color.uFlags = CF_NONE;
    Color.uForeground = BLUE;
    Color.uBackground = WHITE;
    //Screen()->Circle(this, xc, yc, r / 4, Color, 1);




    mClip = OldClip;
    EndDraw();
}


/*--------------------------------------------------------------------------*/
// EllipseWin: Override the Draw function to draw some ellipses.
/*--------------------------------------------------------------------------*/
EllipseWin::EllipseWin(const PegRect &Rect) :
    PegDecoratedWindow(Rect)
{
    Add(new PegTitle("Ellipses"));
    SetColor(PCI_NORMAL, BLACK);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void EllipseWin::Draw(void)
{
    PegColor Color;

    BeginDraw();
    PegWindow::Draw();
    PegRect OldClip = mClip;
    mClip &= mClient;

    PegRect EllipseRect;

    EllipseRect = mClient;
    EllipseRect.wRight -= mClient.Width() / 2;
    EllipseRect.wBottom -= mClient.Height() / 2;
    Color.Set(WHITE, RED, CF_FILL);
    Screen()->Ellipse(this, EllipseRect, Color, 2);

    EllipseRect = mClient;
    EllipseRect.wLeft += mClient.Width() / 2;
    EllipseRect.wBottom -= mClient.Height() / 2;
    Color.Set(BLUE, BLUE, CF_FILL);
    Screen()->Ellipse(this, EllipseRect, Color, 0);

    EllipseRect = mClient;
    EllipseRect.wLeft += mClient.Width() / 2;
    EllipseRect.wTop += mClient.Height() / 2;
    Color.Set(BLUE, CYAN, CF_FILL);
    Screen()->Ellipse(this, EllipseRect, Color, 4);

    EllipseRect = mClient;
    EllipseRect.wRight -= mClient.Width() / 2;
    EllipseRect.wTop += mClient.Height() / 2;
    Color.Set(DARKGRAY, LIGHTGRAY, CF_FILL);
    Screen()->Ellipse(this, EllipseRect, Color, 1);

    EllipseRect = mClient;
    SIGNED iTemp = mClient.Width() / 4;
    EllipseRect.wRight -= iTemp;
    EllipseRect.wLeft += iTemp;
    iTemp = mClient.Height() / 4;
    EllipseRect.wTop += iTemp;
    EllipseRect.wBottom -= iTemp;

    Color.Set(RED, GREEN, CF_NONE);
    Screen()->Ellipse(this, EllipseRect, Color, 5);

    mClip = OldClip;
    EndDraw();
}

