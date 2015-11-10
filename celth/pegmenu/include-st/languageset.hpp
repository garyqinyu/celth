/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: PictureMenuClass
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
class languagesetMenuClass : public PegBitmapWindow
{
    public:
        languagesetMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            BrightnessBarID = 1,
            ContrastBarID,
            ColorBarID,
            SharpnessBarID,
            PicturePlusMenuButtonID,
            SharpnessButtonID,
            ColorButtonID,
            ContrastButtonID,
            BrightnessButtonID,
            PictureWindowTitleID,
        };

    private:
				void DisplayChildWindow(int WindowID);
				void RefreshPictureWindow(void);
				void DrawScreenNewLanguage(void);
				#if 0
	TVProgressBar *pBrightnessBar;
        TVProgressBar *pContrastBar;
        TVProgressBar *pColorBar;
        TVProgressBar *pSharpnessBar;
        TVDecoratedButton *pPicturePlusMenuButton;
        TVDecoratedButton *pSharpnessButton;
        TVDecoratedButton *pColorButton;
		#endif
        TVDecoratedButton *pContrastButton;
        TVDecoratedButton *pBrightnessButton;
        PegPrompt *pPictureMenuWindowTitle;
};
