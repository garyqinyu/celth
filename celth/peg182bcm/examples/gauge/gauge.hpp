/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// gauge.hpp - Example gauge control, demonstrates using the
//  off-screen drawing functions.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1993-2002 Swell Software 
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

class Gauge : public PegThing
{
	public:
		Gauge();
		SIGNED Message(const PegMessage &Mesg);
        void Draw(void);

	private:
        PegBitmap *mpBitmap;
        SIGNED miCurrent;
        SIGNED miDir;
        BOOL mbMoveMode;
        PegPoint mStartMove;

        void DrawToBitmap(void);
};

