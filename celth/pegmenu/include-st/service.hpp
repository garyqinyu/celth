/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: ServiceMenuClass
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
class ServiceMenuClass : public PegBitmapWindow
{
    public:
        ServiceMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            SoundMenuButtonID = 1,
            VideoMenuButtonID,
            GeometryMenuButtonID,
            SetupMenuButtonID,
            SWVersionMenuButtonID,
            ServiceMenuWindowTitleID,
            SWVersionStatusID,
        };

    private:
				void DisplayChildWindow(int WindowID);
        TVDecoratedButton *pSoundServiceMenuButton;
        TVDecoratedButton *pVideoServiceMenuButton;
        TVDecoratedButton *pGeometryMenuButton;
        TVDecoratedButton *pSetupMenuButton;
//        TVDecoratedButton *pSWVersionMenuButton;
        PegPrompt *pSWVersionMenuButton;
        PegPrompt *pServiceMenuWindowTitle;
        PegPrompt *pSWVersionStatus;
};
