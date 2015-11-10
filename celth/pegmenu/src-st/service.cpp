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

#include "peg.hpp"
#include "wbstring.hpp"
#include "mainmenu.hpp"
#include "service.hpp"
#include "tvdecbut.hpp"
#include "pbitmwin.hpp"
#include "anim.hpp"
#include "setup.hpp"
#include "video.hpp"
#include "geometry.hpp"
#include "svaudio.hpp"

char* ptcSWVersion;
PEGCHAR PegString[6];
char cLoop;
extern PegFont chinesefont;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*-------------------------gbPictBitmap-------------------------------------------------*/
ServiceMenuClass::ServiceMenuClass(SIGNED iLeft, SIGNED iTop) : 
    PegBitmapWindow(&gbMainBitmap, kAutoAdjustBitmapSize, PegBlueTransparent, FF_RAISED) 
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 183, iTop + 269);
    InitClient();
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);
#if 0
    ChildRect.Set(iLeft + 13, iTop + 226, iLeft + 162, iTop + 253);
    pSoundServiceMenuButton = new TVDecoratedButton(ChildRect, LS(SID_SoundServiceMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, SoundMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pSoundServiceMenuButton->SetColor(PCI_NORMAL, 223);
    pSoundServiceMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pSoundServiceMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 186, iLeft + 162, iTop + 213);
    pVideoServiceMenuButton = new TVDecoratedButton(ChildRect, LS(SID_VideoServiceMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, VideoMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pVideoServiceMenuButton->SetColor(PCI_NORMAL, 223);
    pVideoServiceMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pVideoServiceMenuButton);
#endif
    ChildRect.Set(iLeft + 13, iTop + 146, iLeft + 162, iTop + 173);
    pGeometryMenuButton = new TVDecoratedButton(ChildRect, LS(SID_GeometryMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, GeometryMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pGeometryMenuButton->SetColor(PCI_NORMAL, 223);
    pGeometryMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pGeometryMenuButton);

    ChildRect.Set(iLeft + 13, iTop + 106, iLeft + 162, iTop + 133);
    pSetupMenuButton = new TVDecoratedButton(ChildRect, LS(SID_SetupMenuButton), &gbBullet5Bitmap, tAnimationTable, 6, RED, BLUE, SetupMenuButtonID, FF_NONE|AF_ENABLED|AF_TRANSPARENT);
    pSetupMenuButton->SetColor(PCI_NORMAL, 223);
    pSetupMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pSetupMenuButton);
#if 0
    ChildRect.Set(iLeft + 13, iTop + 66, iLeft + 92, iTop + 93);
    pSWVersionMenuButton = new PegPrompt(ChildRect, LS(SID_SWVersionMenuButton), SWVersionMenuButtonID, FF_NONE|AF_TRANSPARENT|TJ_LEFT);
    pSWVersionMenuButton->SetColor(PCI_NORMAL, 223);
    pSWVersionMenuButton->SetColor(PCI_NTEXT, BLUE);
    Add(pSWVersionMenuButton);

		ChildRect.Set(iLeft + 130, iTop + 66, iLeft + 174, iTop + 93);
    pSWVersionStatus = new PegPrompt(ChildRect, LS(SID_VersionNb), SWVersionStatusID, FF_NONE|AF_TRANSPARENT|TJ_RIGHT);
		pSWVersionStatus->SetFont(&Pronto_10);
    pSWVersionStatus->SetColor(PCI_NORMAL, 223);
    pSWVersionStatus->SetColor(PCI_NTEXT, BLUE);
    Add(pSWVersionStatus);
#endif
/*
    ChildRect.Set(iLeft + 20, iTop + 15, iLeft + 160, iTop + 44);
    pServiceMenuWindowTitle = new PegPrompt(ChildRect, LS(SID_ServiceMenuWindowTitle), ServiceMenuWindowTitleID, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    pServiceMenuWindowTitle->SetColor(PCI_NORMAL, 130);
    pServiceMenuWindowTitle->SetColor(PCI_NTEXT, BLUE);
    Add(pServiceMenuWindowTitle); */

    /* WB End Construction */
		Id(SERVICE_MENU_ID);

	if (gbCurrentLanguage == 1) /* language = chinese */
	{
		pSetupMenuButton->SetFont(&chinesefont);
		pGeometryMenuButton->SetFont(&chinesefont);
	}
	else if (gbCurrentLanguage == 0)
	{
		pSetupMenuButton->SetFont(&Pronto_12);
		pGeometryMenuButton->SetFont(&Pronto_12);
	}


#if 0
		if (gbCurrentLanguage == 1) /* language = chinese */
		{
	    pServiceMenuWindowTitle->SetFont(&A12RFont);
/*			pSWVersionMenuButton->SetFont(&A12RFont);*/
		}
		else if (gbCurrentLanguage == 2)  /* language = korean */
		{
	    pServiceMenuWindowTitle->SetFont(&korean);
/*			pSWVersionMenuButton->SetFont(&korean);*/
		}
		else /* default for English & French */
		{
	    pServiceMenuWindowTitle->SetFont(&Pronto_16);
/*			pSWVersionMenuButton->SetFont(&Pronto_10);*/
		}
		for (cLoop = 0 ; cLoop<6 ; cLoop++)
		{
				PegString[cLoop] = *ptcSWVersion++; /* translate to PEGCHAR style */
		}

		pSWVersionStatus->DataSet(PegString); /* update string onthe screen */
		pSWVersionStatus->Draw();
#endif
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED ServiceMenuClass::Message(const PegMessage &Mesg)
{
		MainMenuClass *pMainWindow;

		switch (Mesg.wType)
    {

    case SIGNAL(GeometryMenuButtonID, PSF_CLICKED):
        // Enter your code here:
				DisplayChildWindow(GeometryMenuButtonID);
        break;

    case SIGNAL(SetupMenuButtonID, PSF_CLICKED):
        // Enter your code here:
				DisplayChildWindow(SetupMenuButtonID);
        break;

    case PM_KEYRIGHT:/* right key => enter submenu */
			DisplayChildWindow(Mesg.iData);
			break;

    case PM_KEYLEFT:/* left key => back to previous menu */
    case PM_CLOSE:
		Presentation()->Destroy(this);
		pMainWindow = (MainMenuClass*) Presentation()->Find(MAIN_MENU_ID);
		pMainWindow->SetBitmap(&gbMainBitmap);
		break;

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void ServiceMenuClass::DisplayChildWindow(int WindowID)
{
		SetupMenuClass *pSetupWindow;
		VideoMenuClass *pVideoWindow;
		GeometryMenuClass *pGeometryWindow;
		ServiceAudioMenuClass *pServiceAudioMenuWindow;

		switch(WindowID)
		{
		case GeometryMenuButtonID:
				pGeometryWindow = new GeometryMenuClass(100,40);
				Presentation()->Add(pGeometryWindow);
				break;

		case SetupMenuButtonID:
				pSetupWindow = new SetupMenuClass(280,180);
				Presentation()->Add(pSetupWindow);
				break;

		}
}


