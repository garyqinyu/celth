/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: InputOutputClass
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

#include "peg.hpp"
#include "wbstring.hpp"
#include "inout.hpp"
#include "install.hpp"
#include "anim.hpp"
#include "userfct.hpp"
#include "init_val.h"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
InputOutputClass::InputOutputClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbInoutBitmap, kAutoAdjustBitmapSize, WHITE, FF_NONE) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 263, iTop + 279);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 195, iTop + 240, iLeft + 254, iTop + 267);
    pAV2OutStatus = new PegPrompt(ChildRect, LS(SID_Tuner), AV2OutStatusID);
    pAV2OutStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pAV2OutStatus);

    ChildRect.Set(iLeft + 195, iTop + 200, iLeft + 254, iTop + 227);
    pAV1OutStatus = new PegPrompt(ChildRect, LS(SID_Tuner), AV1OutStatusID);
    pAV1OutStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pAV1OutStatus);

    ChildRect.Set(iLeft + 195, iTop + 150, iLeft + 254, iTop + 177);
    pAV3InStatus = new PegPrompt(ChildRect, LS(SID_CVBS), AV3InStatusID);
    pAV3InStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pAV3InStatus);

    ChildRect.Set(iLeft + 195, iTop + 110, iLeft + 254, iTop + 137);
    pAV2InStatus = new PegPrompt(ChildRect, LS(SID_CVBS), AV2InStatusID);
    pAV2InStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pAV2InStatus);

    ChildRect.Set(iLeft + 13, iTop + 240, iLeft + 182, iTop + 267);
    pAV2OutButton = new TVDecoratedButton(ChildRect, LS(SID_AV2OutButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, AV2OutButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pAV2OutButton->SetColor(PCI_NORMAL, 223);
    pAV2OutButton->SetColor(PCI_NTEXT, BLUE);
    Add(pAV2OutButton);

    ChildRect.Set(iLeft + 13, iTop + 200, iLeft + 182, iTop + 227);
    pAV1OutButton = new TVDecoratedButton(ChildRect, LS(SID_AV1OutButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, AV1OutButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pAV1OutButton->SetColor(PCI_NORMAL, 223);
    pAV1OutButton->SetColor(PCI_NTEXT, BLUE);
    Add(pAV1OutButton);

    ChildRect.Set(iLeft + 13, iTop + 150, iLeft + 182, iTop + 177);
    pAV3InButton = new TVDecoratedButton(ChildRect, LS(SID_AV3InButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, AV3InButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pAV3InButton->SetColor(PCI_NORMAL, 223);
    pAV3InButton->SetColor(PCI_NTEXT, BLUE);
    Add(pAV3InButton);

    ChildRect.Set(iLeft + 13, iTop + 110, iLeft + 182, iTop + 137);
    pAV2InButton = new TVDecoratedButton(ChildRect, LS(SID_AV2InButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, AV2InButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pAV2InButton->SetColor(PCI_NORMAL, 223);
    pAV2InButton->SetColor(PCI_NTEXT, BLUE);
    Add(pAV2InButton);

    ChildRect.Set(iLeft + 13, iTop + 70, iLeft + 182, iTop + 97);
    pAV1InButton = new TVDecoratedButton(ChildRect, LS(SID_AV1InButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, AV1InButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pAV1InButton->SetColor(PCI_NORMAL, 223);
    pAV1InButton->SetColor(PCI_NTEXT, BLUE);
    Add(pAV1InButton);

    ChildRect.Set(iLeft + 195, iTop + 70, iLeft + 254, iTop + 97);
    pAV1InStatus = new PegPrompt(ChildRect, LS(SID_CVBS), AV1InStatusID);
    pAV1InStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pAV1InStatus);

    ChildRect.Set(iLeft + 20, iTop + 16, iLeft + 219, iTop + 45);
    pIOMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_IOMenuWindowTitle), IOMenuWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pIOMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pIOMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pIOMenuWindowTitle);

    /* WB End Construction */
		Id(IO_MENU_ID);

		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pIOMenuWindowTitle->SetFont(&A12RFont);
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pIOMenuWindowTitle->SetFont(&korean);
		}
		else /* default for English & French */
		{
	    pIOMenuWindowTitle->SetFont(&Pronto_16);
		}

		RefreshInOutWindow(); /* initialize window */
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED InputOutputClass::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {

		case SIGNAL(AV1InButtonID, PSF_FOCUS_RECEIVED):
			break;

		case SIGNAL(AV2InButtonID, PSF_FOCUS_RECEIVED):
			break;

		case SIGNAL(AV3InButtonID, PSF_FOCUS_RECEIVED):
			break;

		case SIGNAL(AV1OutButtonID, PSF_FOCUS_RECEIVED):
			break;

		case SIGNAL(AV2OutButtonID, PSF_FOCUS_RECEIVED):
			break;

    case PM_KEYRIGHT:
    case PM_KEYLEFT:
				break;

		case PM_REFRESHWINDOW:
			RefreshInOutWindow();
			break;

		case PM_CLOSE:
				CloseThis();
				break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

void InputOutputClass::CloseThis(void)
{
	InstallMenuClass *pInstallWindow;

	Presentation()->Destroy(this);
	pInstallWindow = (InstallMenuClass*) Presentation()->Find(INSTALL_MENU_ID);
	pInstallWindow->SetBitmap(&gbMainBitmap);
}


void InputOutputClass::RefreshInOutWindow(void)
{

}

