/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: TVProgressWindowClass
//  
//   Copyright (c) STMicroelectronics
//                 ZI de Rousset, BP2
//                 13106 ROUSSET CEDEX
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifndef _TVPROGWI_HPP
#define _TVPROGWI_HPP

#include "tvprobar.hpp"
#include "usertype.hpp"
#include "tempheader.hpp" /* just for virtual test, delete it when there is ap_cmd module in the software*/

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class TVProgressWindowClass : public PegWindow
{
public:
	TVProgressWindowClass(SIGNED iLeft, SIGNED iTop, PegThing* pCaller, \
		APCTRL_LinkId_t eEventToNotify, APCTRL_PegState_t eSubStateToNotify, \
		const PEGCHAR *Text, UCHAR TextColor, WORD wStyle, SIGNED iMin, SIGNED iMax, \
		SIGNED iCurrent, PegThing** pptWindow);

	SIGNED Message(const PegMessage &Mesg);
	enum ChildIds {
		ProgressWindowPromptID = 1,
            ProgressWindowBarID,
	};
	
private:
				void StoreAndSend(short CurrentBarValue);
				PegThing* pCallerObject;
				PegThing* ptHiddenWindow[MaxNumberOfWindow];
				PegPrompt *pProgressWindowPrompt;
				TVProgressBar *pProgressWindowBar;
				APCTRL_LinkId_t eEvent;
				APCTRL_PegState_t eSubState;
};
#endif /*_TVPROGWI_HPP*/