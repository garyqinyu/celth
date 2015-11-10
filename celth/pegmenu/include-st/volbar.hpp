/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: VolumeBarClass
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
#include "tvprobar.hpp"

#define kVolumeBarTimeOut  5

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class VolumeBarClass : public PegBitmapWindow
{
	public:
		VolumeBarClass(SIGNED iLeft, SIGNED iTop);
		SIGNED Message(const PegMessage &Mesg);
		enum ChildIds {
			VolumeBarID = 1,
		};

	private:
		TVProgressBar *pVolumeBar;
		PegIcon *pVolumeIcon;
		int iVolumeBarTimeOut;
};
