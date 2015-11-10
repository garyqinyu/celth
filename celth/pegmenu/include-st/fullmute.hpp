/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: FullMuteClass
//  
//  Copyright (c) STMicroelectronics Pvt.Ltd.
//                TVD Application Lab
//                Noida, India.
//  
//  All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "pbitmwin.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class FullMuteClass : public PegBitmapWindow
{
	public:
		FullMuteClass(SIGNED iLeft, SIGNED iTop);
		SIGNED Message(const PegMessage &Mesg);
		enum ChildIds {
			FullMuteID = 1,
		};

	private:
		PegBitmapButton *pFullMuteBitmap;
};
