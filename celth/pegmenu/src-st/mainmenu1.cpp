/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: PEGMainMenu
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "mainmenu1.hpp"
#include "submenu1.hpp"
#include "wbstring1.hpp"

extern PegFont NewFont;
extern UCHAR gbCurrentLanguage1;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGMainMenu1::PEGMainMenu1(SIGNED iLeft, SIGNED iTop) : 
    PegWindow()
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 399, iTop + 499);
    InitClient();


    ChildRect.Set(iLeft + 90, iTop + 233, iLeft + 308, iTop + 266);
    pMainmenuButton_2 = new PegDecoratedButton(ChildRect, LS1(SID_1), NULL, ID_Button_2, AF_ENABLED);
    Add(pMainmenuButton_2);

    ChildRect.Set(iLeft + 92, iTop + 416, iLeft + 310, iTop + 449);
    pMainmenuButton_3 = new PegDecoratedButton(ChildRect, LS1(SID_2), NULL, ID_Button_3, AF_ENABLED);
    Add(pMainmenuButton_3);

    ChildRect.Set(iLeft + 95, iTop + 72, iLeft + 313, iTop + 105);
    pMainmenuButton_1 = new PegDecoratedButton(ChildRect, LS1(SID_3), NULL, ID_Button_1, AF_ENABLED);
    Add(pMainmenuButton_1);

	
	if (gbCurrentLanguage1 == 1)
	{
			pMainmenuButton_1->SetFont(&NewFont);
			pMainmenuButton_2->SetFont(&NewFont);
			pMainmenuButton_3->SetFont(&NewFont);
	}
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PEGMainMenu1::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(ID_Button_2, PSF_CLICKED):
        // Enter your code here:
		DisplayChildWindow(ID_Button_2);
        break;

    case SIGNAL(ID_Button_3, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_Button_1, PSF_CLICKED):
        // Enter your code here:
        break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

void PEGMainMenu1::DisplayChildWindow(int WindowID)
{
	switch(WindowID)
	{
	case ID_Button_2:	
		PEGSubMenu *pSubWin = new PEGSubMenu(0, 0);
		Presentation()->Center(pSubWin);
		Presentation()->Add(pSubWin);
			break;	
		}
}

