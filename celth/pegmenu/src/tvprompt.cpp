/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// tvprompt.cpp - prompt object
//
// Author: ST ShenZhen MMX Lab
//
// Copyright (c) 
//              All Rights Reserved.
//
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


#include "peg.hpp"
#include "tvprompt.hpp"

/*--------------------------------------------------------------------------*/
TVPrompt::TVPrompt(const PegRect &Rect, const PEGCHAR *Text, WORD wId, WORD wStyle, PegFont *pFont) 
: PegPrompt(Rect, Text, wId, wStyle)
{
    //Type(TYPE_TVPROMPT); 
	if (pFont)
    {
        mpFont = pFont;
    }
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
TVPrompt::~TVPrompt()
{
}

/*--------------------------------------------------------------------------*/
SIGNED TVPrompt::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_CURRENT:
        PegThing::Message(Mesg);
        if (!(Style() & AF_TRANSPARENT))
        {
            Invalidate(mClip);
            Draw();
        }
        break;

    case PM_NONCURRENT:
        PegThing::Message(Mesg);
        if (!(Style() & AF_TRANSPARENT))
        {
            Invalidate(mClip);
            Draw();
        }
        break;

    default:
        PegThing::Message(Mesg);
        break;
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void TVPrompt::Draw(void)
{
    if (!Parent())
    {
        return;
    }
    
	
    BeginDraw();	
	PegColor Color(LIGHTBLUE, LIGHTBLUE, CF_FILL);
	
	//draw border begin
	//
	PegPoint tmpPoint[4];
	tmpPoint[0].x = mReal.wLeft;
	tmpPoint[0].y = (mReal.wBottom + mReal.wTop) / 2;
	tmpPoint[1].x = tmpPoint[0].x + 60;
	tmpPoint[1].y = tmpPoint[0].y - 5;
	tmpPoint[2].x = tmpPoint[1].x;
	tmpPoint[2].y = tmpPoint[0].y + 15;
	tmpPoint[3].x = tmpPoint[0].x;
	tmpPoint[3].y = tmpPoint[0].y;
	
	Screen()->Polygon(this, tmpPoint, 4, Color, 1);

	PegRect rectEllips;
	rectEllips.Set(mReal.wLeft + 20, mReal.wTop, mReal.wRight, mReal.wBottom);
	Screen()->Ellipse(this, rectEllips, Color, 1);
	//

	Color.uForeground = YELLOW;
    Color.uFlags = CF_NONE;   

    // Draw the Text:
    if (mpText)
    {
        PegRect mSave = mClip;
        mClip &= mClient;

        PegPoint Point;
        Point.y = mClient.wTop + ((mClient.Height() - TextHeight(mpText, mpFont)) / 2);
        Point.x = mClient.wLeft + 10;
        Point.x += (mClient.Width() - TextWidth(mpText, mpFont)) / 2;
       
        DrawText(Point, mpText, Color, mpFont);
        mClip = mSave;
    }

    EndDraw();
}

// End of file