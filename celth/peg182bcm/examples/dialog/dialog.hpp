/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// dialog.hpp - Example dialog window using signals.
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

class DialogWin : public PegDialog
{
	public:
		DialogWin(const PegRect &Rect);
		SIGNED Message(const PegMessage &Mesg);

	private:
		enum MyButtons {
			IDB_HAS_EMAIL = 1,

		};
		PegPrompt *mpEmailPrompt;
		PegString *mpEmailString;
};


