/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: GeometryMenuClass
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
class GeometryMenuClass : public PegBitmapWindow
{
    public:
        GeometryMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            EWEHTStatusID = 1,
            HBowStatusID,
            HParallelStatusID,
            EWLowerStatusID,
            EWUpperStatusID,
            EWTrapeziumStatusID,
            EWPinCushionStatusID,
            EWWidthStatusID,
            HShiftStatusID,
            HBlankingStatusID,
            VSCorrectionStatusID,
            VAmplitudeStatusID,
            VPositionStatusID,
            VSlopeStatusID,
            VideoMenuWindowTitleID,
            DefaultGeometryButtonID,
            EWEHTButtonID,
            HBowButtonID,
            HParallelButtonID,
            EWLowerButtonID,
            EWUpperButtonID,
            EWTrapeziumButtonID,
            EWPinCushionButtonID,
            EWWidthButtonID,
            HShiftButtonID,
            HBlankingButtonID,
            VSCorrectionButtonID,
            VAmplitudeButtonID,
            VPositionButtonID,
            VSlopeButtonID,
        };

    private:
				void RefreshGeometryValues(void);
        PegPrompt *pEWEHTStatus;
        PegPrompt *pHBowStatus;
        PegPrompt *pHParallelStatus;
        PegPrompt *pEWLowerStatus;
        PegPrompt *pEWUpperStatus;
        PegPrompt *pEWTrapeziumStatus;
        PegPrompt *pEWPinCushionStatus;
        PegPrompt *pEWWidthStatus;
        PegPrompt *pHShiftStatus;
        PegPrompt *pHBlankingStatus;
        PegPrompt *pVSCorrectionStatus;
        PegPrompt *pVAmplitudeStatus;
        PegPrompt *pVPositionStatus;
        PegPrompt *pVSlopeStatus;
        PegPrompt *pGeometryMenuWindowTitle;
        TVDecoratedButton *pDefaultGeometryButton;
        TVDecoratedButton *pEWEHTButton;
        TVDecoratedButton *pHBowButton;
        TVDecoratedButton *pHParallelButton;
        TVDecoratedButton *pEWLowerButton;
        TVDecoratedButton *pEWUpperButton;
        TVDecoratedButton *pEWTrapeziumButton;
        TVDecoratedButton *pEWPinCushionButton;
        TVDecoratedButton *pEWWidthButton;
        TVDecoratedButton *pHShiftButton;
        TVDecoratedButton *pHBlankingButton;
        TVDecoratedButton *pVSCorrectionButton;
        TVDecoratedButton *pVAmplitudeButton;
        TVDecoratedButton *pVPositionButton;
        TVDecoratedButton *pVSlopeButton;
};
