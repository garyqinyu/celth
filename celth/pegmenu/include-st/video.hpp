/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: VideoMenuClass
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
class VideoMenuClass : public PegBitmapWindow
{
    public:
        VideoMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            LumaChromaDelayStatusID = 1,
            FBDelayStatusID,
            PrescaleRGBStatusID,
            OffsetGStatusID,
            OffsetRStatusID,
            PeakWhiteLimiterStatusID,
            WPBStatusID,
            WPGStatusID,
            WPRStatusID,
            G2AlignStatusID,
            DefaultVideoButtonID,
            LumaChromaDelayButtonID,
            FBDelayButtonID,
            PrescaleRGBButtonID,
            OffsetGButtonID,
            OffsetRButtonID,
            PeakWhiteLimiterButtonID,
            WPBButtonID,
            WPGButtonID,
            WPRButtonID,
            G2AlignButtonID,
            VideoMenuWindowTitleID,
        };

    private:
				void RefreshVideoValues(void);
				PegPrompt *pLumaChromaDelayStatus;
        PegPrompt *pFBDelayStatus;
        PegPrompt *pPrescaleRGBStatus;
        PegPrompt *pOffsetGStatus;
        PegPrompt *pOffsetRStatus;
        PegPrompt *pPeakWhiteLimiterStatus;
        PegPrompt *pWPBStatus;
        PegPrompt *pWPGStatus;
        PegPrompt *pWPRStatus;
        PegPrompt *pG2AlignStatus;
        TVDecoratedButton *pDefaultVideoButton;
        TVDecoratedButton *pLumaChromaDelayButton;
        TVDecoratedButton *pFBDelayButton;
        TVDecoratedButton *pPrescaleRGBButton;
        TVDecoratedButton *pOffsetGButton;
        TVDecoratedButton *pOffsetRButton;
        TVDecoratedButton *pPeakWhiteLimiterButton;
        TVDecoratedButton *pWPBButton;
        TVDecoratedButton *pWPGButton;
        TVDecoratedButton *pWPRButton;
        TVDecoratedButton *pG2AlignButton;
        PegPrompt *pVideoMenuWindowTitle;
};
