/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// drawwin.hpp - Example overriding a base PEG class
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



class DerivedWin : public PegWindow
{
	public:
		DerivedWin(PegRect &Rect);
		void Draw(void);
};

