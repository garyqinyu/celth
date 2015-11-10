/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// RoboButton.cpp: Custom button class.
//
// Author: Ken Maxwell
//
// Copyright (c) 1997-2002, Swell Software, Inc.
//
// Notes:
//
// Overidden Draw() function, for wide borders and unique style.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "RoboButn.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
RoboButton::RoboButton(PegRect &Rect, WORD wId, char *Text) :
	PegButton(Rect, wId)
{
	mpText = Text;
	mClient = mReal;
	mClient -= 3;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void RoboButton::Draw(void)
{
	PegColor Color;
	BeginDraw();

	if (Style() & BF_SELECTED)
	{
		Color.uForeground = BLACK;
	}
	else
	{
		Color.uForeground = LIGHTGRAY;
	}

	// draw the top:
	Line(mReal.wLeft, mReal.wTop, mReal.wRight, mReal.wTop,
		Color, 3);

	// draw the left:
	Line(mReal.wLeft, mReal.wTop, mReal.wLeft, mReal.wBottom,
		Color, 3);

	if (Style() & BF_SELECTED)
	{
		Color.uForeground = LIGHTGRAY;
	}
	else
	{
		Color.uForeground = BLACK;
	}
	// draw the right shadow:
	Line(mReal.wRight, mReal.wTop, mReal.wRight,
		mReal.wBottom - 2, Color, 1);
	Line(mReal.wRight - 1, mReal.wTop + 1, mReal.wRight - 1,
		mReal.wBottom - 2, Color, 1);
	Line(mReal.wRight - 2, mReal.wTop + 2, mReal.wRight - 2,
		mReal.wBottom - 2,	Color, 1);

	// draw the bottom shadow:
	Line(mReal.wLeft, mReal.wBottom, mReal.wRight,
		mReal.wBottom, Color, 1);
	Line(mReal.wLeft + 1, mReal.wBottom - 1, mReal.wRight,
		mReal.wBottom - 1, Color, 1);
	Line(mReal.wLeft + 2, mReal.wBottom - 2, mReal.wRight,
		mReal.wBottom - 2,	Color, 1);

	// fill in the button client area:

	Color.Set(LIGHTRED, DARKGRAY, CF_FILL);
	
	Rectangle(mClient, Color);

	// draw the text centered:

	PegPoint Put;
	Put.x = (mClient.wLeft + mClient.wRight) >> 1;
	Put.x -= TextWidth(mpText, &SysFont) >> 1;
	Put.y = mClient.wTop + 1;

	if (Style() & BF_SELECTED)
	{
		Put.x++;
		Put.y++;
	}

	Color.Set(WHITE, BLACK, CF_NONE);
	DrawText(Put, mpText, Color, &SysFont);
	EndDraw();
}

