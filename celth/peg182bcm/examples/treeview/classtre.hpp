/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// classtre.hpp - Example using PegTreeView that displays the PEG library 
//                hierarchy.
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-1999 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

class ClassTreeWin : public PegDecoratedWindow
{
	public:
		ClassTreeWin(const PegRect& Rect);
		~ClassTreeWin() {}

	private:
		void InitTreeView();

		PegTreeView* pTree;
};


