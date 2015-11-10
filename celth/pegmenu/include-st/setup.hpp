/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: SetupMenuClass
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

#include "steditinput.hpp"
#include "pbitmwin.hpp"
#include "tvprobar.hpp"
#include "tvdecbut.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SetupMenuClass : public PegBitmapWindow
{
    public:
        SetupMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            CRTTypeStatusID = 1,
            OKButtonID,
            CancelButtonID,
            CRTButtonID,
            I2C3ButtonID,
            I2C0ButtonID,
            SetupMenuWindowTitleID,
            LabelModulationID,
            LabelSymbolrateID,
            LabelFrequencyID,
        };

    private:
				void BackToPreviousMenu(void);
				void RefreshSetupWindow(void);
				PegPrompt *pCRTTypeStatus;
	PegPrompt *LabelFrequency;
	PegPrompt *LabelSymbolrate;
	PegPrompt *LabelModulation;
	TVDecoratedButton *pOKbutton;
	TVDecoratedButton *pCancelbutton;
	MutilSelectString *EditModulation;
	stEditInput *EditSymbolrate;
	stEditInput *EditFrequency;
        PegPrompt *pSetupMenuWindowTitle;
};
