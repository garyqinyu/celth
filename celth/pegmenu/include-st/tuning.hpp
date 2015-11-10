/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: TuningMenuClass
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
class TuningMenuClass : public PegBitmapWindow
{
    public:
        TuningMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            MHzStatusID = 1,
            RFStandardStatusID,
            VideoStandardStatusID,
            FrequencyStatusID,
            CountryStatusID,
            PrgNumberStatusID,
            StoreButtonID,
            RFStandardButtonID,
            VideoStandardButtonID,
            FrequencyButtonID,
            CountryButtonID,
            PrgNumberButtonID,
            TuningMenuWindowTitleID,
        };

    private:
				void GoBack(void);
				void RefreshTuningWindow(void);
        PegPrompt *pMHzStatus;
        PegPrompt *pRFStandardStatus;
        PegPrompt *pVideoStandardStatus;
        PegPrompt *pFrequencyStatus;
        PegPrompt *pCountryStatus;
        PegPrompt *pPrgNumberStatus;
        TVDecoratedButton *pStoreButton;
        TVDecoratedButton *pRFStandardButton;
        TVDecoratedButton *pVideoStandardButton;
        TVDecoratedButton *pFrequencyButton;
        TVDecoratedButton *pCountryButton;
        TVDecoratedButton *pPrgNumberButton;
        PegPrompt *pTuningMenuWindowTitle;
};
