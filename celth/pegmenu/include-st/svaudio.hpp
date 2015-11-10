/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: ServiceAudioMenuClass
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

#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "tvprobar.hpp"
#include "agcmsg.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class ServiceAudioMenuClass : public PegBitmapWindow
{
	public:
		ServiceAudioMenuClass(SIGNED iLeft, SIGNED iTop);
		SIGNED Message(const PegMessage &Mesg);
		enum ChildIds {
			ThrSwNICAMBarID,
			PrescNICAMBarID,
			PrescFMBarID,
			PrescSCARTBarID,
			AGCAdjBarID,
			ThrSwNICAMButtonID,
			PrescNICAMButtonID,
			PrescFMButtonID,
			PrescSCARTButtonID,
			AGCAdjButtonID,
			ServiceAudioMenuTitleID,
		};

	private:
		AGCMessageClass  *pAGCMessageWindow;
		TVProgressBar *pThrSwNICAMBar;
		TVProgressBar *pPrescNICAMBar;
		TVProgressBar *pPrescFMBar;
		TVProgressBar *pPrescSCARTBar;
		TVProgressBar *pAGCAdjBar;
		TVDecoratedButton *pThrSwNICAMButton;
		TVDecoratedButton *pPrescNICAMButton;
		TVDecoratedButton *pPrescFMButton;
		TVDecoratedButton *pPrescSCARTButton;
		TVDecoratedButton *pAGCAdjButton;
		PegPrompt *pServiceAudioMenuTitle;
};
