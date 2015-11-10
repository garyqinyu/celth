/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: InstallMenuClass
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
class InstallMenuClass : public PegBitmapWindow
{
    public:
        InstallMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            LanguageMenuButtonID = 1,
            IOMenuButtonID,
            ManualTuningMenuButtonID,
            AutosearchMenuButtonID,
            OrganizeMenuButtonID,
            InstallMenuWindowTitleID,
        };

    private:
				void DisplayChildWindow(int WindowID);
				PegDecoratedButton *pLanguageMenuButton;
        PegDecoratedButton *pIOMenuButton;
        PegDecoratedButton *pManualTuningMenuButton;
        PegDecoratedButton *pAutosearchMenuButton;
        PegDecoratedButton *pOrganizeMenuButton;
        PegPrompt *pInstallMenuWindowTitle;
};
