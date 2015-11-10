/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// chart.hpp - Simple chart
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



/*--------------------------------------------------------------------------*/
class Chart : public PegWindow
{
	public:
		Chart(const PegRect &Rect);
        ~Chart();
		SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        void Update(void);

	private:
        void DrawToBitmap(void);
        void PositionIcon(PegPoint Point);

        PegBitmap *mpBitmap;
        PegIcon *mpIcon;
        WORD mwNumPoints;
        WORD mwMaxPoints;
        SIGNED *mpData;
        SIGNED *mpFirstPoint;
        BOOL mbMoveMode;
        PegPoint mStartMove;
};

