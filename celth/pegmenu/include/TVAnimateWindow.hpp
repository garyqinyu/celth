/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// tvanimatewindow.hpp - TVAnimateWindow class definition.
//
// Author: 
//
// Copyright (c) STMicroelectronics
//              All Rights Reserved.
//
// Unauthorized redistrition of this source code, in whole or part,
// without the express written permission of STMicroelectronics
// is strictly prohibited.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef TVANIMATEWINDOW_HPP
#define TVANIMATEWINDOW_HPP

#include "usertype.hpp"

class TVAnimateWindow : public PegAnimationWindow  
{
public:	
	TVAnimateWindow(PegThing* pCaller, PegThing** pptWindow, const PegRect &Rect, 
		PegBitmap *pBackground, PegBitmap **FrameList, UCHAR uNumFrames, SIGNED xPos, 
		SIGNED yPos, UCHAR *Palette = NULL,  WORD wStyle = FF_NONE);
	virtual ~TVAnimateWindow();
	SIGNED Message(const PegMessage &Mesg);

	virtual void SetAnimateFrames(PegBitmap ** FrameList, UCHAR uNumFrames);

protected:
	void DrawTVAnimationFrame(void);

private:
	PegThing* pCallerObject;
	PegThing* ptHiddenWindow[MaxNumberOfWindow];
};

#endif
