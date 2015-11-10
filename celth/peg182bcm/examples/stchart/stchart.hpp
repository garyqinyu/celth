/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// stchart.hpp - PegStripChart example.
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

/*--------------------------------------------------------------------------*/
class StripChartWin : public PegDecoratedWindow
{
	public:
   		StripChartWin(const PegRect& Rect);
		~StripChartWin() {}

		virtual SIGNED Message(const PegMessage& Mesg);

		PegStripChart* pChart;
		UCHAR mucID;
};

