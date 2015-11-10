/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: PicturePlusMenuClass
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
#include "tvprobar.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PicturePlusMenuClass : public PegBitmapWindow
{
    public:
        PicturePlusMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            ContrastEnhStatusID = 1,
			IfpLevelStatusID,
            DigitalScanStatusID,
			IfpLevelButtonID,
            CTIStatusID,
            GreenBoostStatusID,
            RotationBarID,
            ChromaCoringStatusID,
            CombFilterStatusID,
            NRStatusID,
            DigitalScanButtonID,
            CTIButtonID,
            GreenBoostButtonID,
            RotationButtonID,
            ChromaCoringButtonID,
            CombFilterButtonID,
            ContrastEnhButtonID,
            NRButtonID,
            TintBarID,
            TintButtonID,
            AutoFleshStatusID,
            AutoFleshButtonID,
            PicturePlusWindowTitleID,
        };

    private:
				void RefreshPicturePlusWindow(void);
        PegPrompt *pPicturePlusMenuWindowTitle;
        PegPrompt *pContrastEnhStatus;
        PegPrompt *pDigitalScanStatus;
        PegPrompt *pCTIStatus;
        PegPrompt *pGreenBoostStatus;
        PegPrompt *pChromaCoringStatus;
        PegPrompt *pCombFilterStatus;
        PegPrompt *pNRStatus;
        PegPrompt *pAutoFleshStatus;
		PegPrompt *pIfpLevelStatus;
		TVDecoratedButton *pIfPLevelButton;
        TVDecoratedButton *pDigitalScanButton;
        TVDecoratedButton *pCTIButton;
        TVDecoratedButton *pGreenBoostButton;
        TVDecoratedButton *pRotationButton;
        TVDecoratedButton *pChromaCoringButton;
        TVDecoratedButton *pCombFilterButton;
        TVDecoratedButton *pContrastEnhButton;
        TVDecoratedButton *pNRButton;
        TVDecoratedButton *pTintButton;
        TVDecoratedButton *pAutoFleshButton;
        TVProgressBar *pTintBar;
        TVProgressBar *pRotationBar;
};
