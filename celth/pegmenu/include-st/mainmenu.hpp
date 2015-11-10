/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: MainMenuClass
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
class MainMenuClass : public PegBitmapWindow
{
    public:
        MainMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            MainMenuWindowTitleID = 1,
            HelpMenuButtonID,
            ServiceMenuButtonID,
            InstallMenuButtonID,
            SoundMenuButtonID,
            PictureMenuButtonID,
        };

    private:
				void DisplayChildWindow(int WindowID);
		    PegPrompt *pMainMenuWindowTitle;
        TVDecoratedButton *pHelpMenuButton;
        TVDecoratedButton *pServiceMenuButton;
        TVDecoratedButton *pInstallMenuButton;
        TVDecoratedButton *pSoundMenuButton;
        TVDecoratedButton *pPictureMenuButton;
};

void PegSetColorTranslucency(PegPresentationManager *pPresent, unsigned char ucTranslucency);
