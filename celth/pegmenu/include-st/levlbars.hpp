/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: STMonoBarClass
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
#include "tvdecbut.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class STWideGainBarClass : public PegBitmapWindow
{
  public:
    STWideGainBarClass(SIGNED iLeft, SIGNED iTop);
    SIGNED Message(const PegMessage &Mesg);
    enum ChildIds {
      STWideGainBarWindowID = 1,
      STWideGainBarID,
			STWideGainButtonID,
    };

  private:
    TVDecoratedButton *pSTWideGainButton;
		TVProgressBar *pSTWideGainBar;
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SRS3DBarsClass : public PegBitmapWindow
{
	public:
		SRS3DBarsClass(SIGNED iLeft, SIGNED iTop);
		SIGNED Message(const PegMessage &Mesg);
		enum ChildIds {
			SRS3DBarsWindowID = 1,
			SRS3DSpaceBarID,
			SRS3DCentreBarID,
			SRS3DSpaceButtonID,
			SRS3DCentreButtonID,
		};

	private:
		TVDecoratedButton *pSRS3DSpaceButton;
		TVDecoratedButton *pSRS3DCentreButton;
		TVProgressBar *pSRS3DSpaceBar;
		TVProgressBar *pSRS3DCentreBar;
};
