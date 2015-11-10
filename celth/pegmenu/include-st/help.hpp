/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: HelpMenuClass
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

#include "pbitmwin.hpp"
#include "tvdecbut.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class HelpMenuClass : public PegBitmapWindow
{
    public:
        HelpMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            HelpMenuWindowTitleID = 1,
            CancelStatusID,
            MixStatusID,
            RevealStatusID,
            SubtitleStatusID,
            ColorLinksStatusID,
            TxtZoomStatusID,
            TwbStatusID,
            AVToggleStatusID,
            ZoomStatusID,
            TeletextStatusID,
            PrgPPrgMStatusID,
            OKStatusID,
            StandbyIndexStatusID,
            DigitStatusID,
            SoundStatusID,
            LeftRightStatusID,
            MenuStatusID,
            UpDownStatusID,
            HelpStatusID,
            HelpSelectionButtonID,
						RCBitmapID,
        };

    private:
				void DisplayHelp(void);
        PegPrompt *pCancelStatus;
        PegPrompt *pMixStatus;
        PegPrompt *pRevealStatus;
        PegPrompt *pSubtitleStatus;
        PegPrompt *pColorLinksStatus;
        PegPrompt *pTxtZoomStatus;
        PegPrompt *pTwbStatus;
        PegPrompt *pAVToogleStatus;
        PegPrompt *pZoomSubcodeStatus;
        PegPrompt *pTeletextStatus;
        PegPrompt *pPrgPPrgMStatus;
        PegPrompt *pOKStatus;
        PegPrompt *pStandbyIndexStatus;
        PegPrompt *pDigitStatus;
        PegPrompt *pSoundStatus;
        PegPrompt *pLeftRightStatus;
        PegPrompt *pMenuStatus;
        PegPrompt *pUpDownStatus;
        PegPrompt *pHelpStatus;
        TVDecoratedButton *pHelpSelectionButton;
				PegIcon* pRCBitmap;
				PegIcon* pLeftBitmap;
				PegIcon* pRightBitmap;
};
