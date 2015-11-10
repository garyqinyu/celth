/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: StringDisplayClass
//  
//   Copyright (c) STMicroelectronics
//                 Display & TV Division
//                 SW Application Lab.
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "pbitmwin.hpp"
#include "peg.hpp"

typedef struct
{ unsigned char ucTableSize;
	PEGCHAR* pTextString1;
	PEGCHAR* pTextString2;
	PEGCHAR* pTextString3;
}DisplayedString_t;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PopupWindowClass : public PegBitmapWindow
{
    public:
				PopupWindowClass(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, SIGNED iHeight, PegBitmap* pBitmap, UCHAR BitmapStyle , UCHAR FillColor, WORD wStyle, DisplayedString_t* psStringToDisplay, UCHAR TextColor, UCHAR uPopUpWindowTimeOut, int iPopupID);
				SIGNED Message(const PegMessage &Mesg);
				void SetFont(PegFont* pFont);
        virtual ~PopupWindowClass() {}
        enum ChildIds {
            String1PromptID = 1,
            String2PromptID,
            String3PromptID,
        };
				
    private:
				PegPrompt* pString1;
				PegPrompt* pString2;
				PegPrompt* pString3;
				unsigned char mTimout;
};
