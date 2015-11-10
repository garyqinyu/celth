/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: HalfMuteClass
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
class HalfMuteClass : public PegBitmapWindow
{
	public:
		HalfMuteClass(SIGNED iLeft, SIGNED iTop);
		SIGNED Message(const PegMessage &Mesg);
		enum ChildIds {
			HalfMuteID = 1,
		};

	private:
		PegBitmapButton *pHalfMuteBitmap;
};
