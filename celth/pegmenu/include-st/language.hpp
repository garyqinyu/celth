/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: LanguageMenuClass
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
class LanguageMenuClass : public PegBitmapWindow
{
    public:
        LanguageMenuClass(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            ChineseMenuButtonID = 1,
            KoreanMenuButtonID,
            FrenchMenuButtonID,
            EnglishMenuButtonID,
            LanguageMenuWindowTitleID,
        };

    private:
				void DrawScreenNewLanguage(void);
        PegDecoratedButton *pChineseMenuButton;
        PegDecoratedButton *pKoreanMenuButton;
        PegDecoratedButton *pFrenchMenuButton;
        PegDecoratedButton *pEnglishMenuButton;
        PegPrompt *pLanguageMenuWindowTitle;
};
