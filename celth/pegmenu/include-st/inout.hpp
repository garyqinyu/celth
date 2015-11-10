/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: InputOutputClass
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

#include "tvdecbut.hpp"
#include "pbitmwin.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class InputOutputClass : public PegBitmapWindow
{
    public:
        InputOutputClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            AV2OutStatusID = 1,
            AV1OutStatusID,
            AV3InStatusID,
            AV2InStatusID,
            AV1InStatusID,
            AV2OutButtonID,
            AV1OutButtonID,
            AV3InButtonID,
            AV2InButtonID,
            AV1InButtonID,
            IOMenuWindowTitleID,
        };

    private:
				void CloseThis(void);
				void RefreshInOutWindow(void);
        PegPrompt *pIOMenuWindowTitle;
        PegPrompt *pAV2OutStatus;
        PegPrompt *pAV1OutStatus;
        PegPrompt *pAV3InStatus;
        PegPrompt *pAV2InStatus;
        PegPrompt *pAV1InStatus;
        TVDecoratedButton *pAV2OutButton;
        TVDecoratedButton *pAV1OutButton;
        TVDecoratedButton *pAV3InButton;
        TVDecoratedButton *pAV2InButton;
        TVDecoratedButton *pAV1InButton;
};
