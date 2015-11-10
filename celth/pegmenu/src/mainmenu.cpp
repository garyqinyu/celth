/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: MainMenu
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "peg.hpp"
#include "mainmenu.hpp"
/*add by tina, 2003/8/28*/
#include "wbstring.hpp"
#include "tooltip.hpp"
#include "usertype.hpp"
#include "tvbitmapbutton.hpp"
#include "helpwnd.hpp"
#include "soundmenu.hpp"
#include "picturemenu.hpp"
#include "factorymenu.hpp"
#include "superfunction.hpp"
#include "supsetmenu.hpp"

#undef __MAINMENUMASTER  /*for mastership */
#define __MAINMENUMASTER /* for mastership */

#include "anim.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresent)
{
	printf("err0\n");
	pPresent->Screen()->SetPalette(0, 255, PegCustomPalette); /* use custom palette */
	pPresent->SetColor(PCI_NORMAL, NEWMENU_BACKGRANOUD_COLOR);
	printf("error1\n");
	pPresent->Draw();
	printf("error2\n");
	
	/*Set default font size (only English)*/
	PegTextThing::SetDefaultFont(PEG_PROMPT_FONT,&Arial14);
	PegTextThing::SetDefaultFont(PEG_TBUTTON_FONT,&Arial12);

	//gbCurrentLanguage = 1;
	/*
	if(0 == gbCurrentLanguage)
	{
		PegTextThing::SetDefaultFont(PEG_PROMPT_FONT,&Arial14);
		PegTextThing::SetDefaultFont(PEG_TBUTTON_FONT,&Arial12);
	}
	else if(1 == gbCurrentLanguage)
	{
		PegTextThing::SetDefaultFont(PEG_PROMPT_FONT,&heiti14);
		PegTextThing::SetDefaultFont(PEG_TBUTTON_FONT,&heiti12);
	}
	*/

	printf("error3\n");
    MainMenu *pWin = new MainMenu(20, 20);

	printf("error4\n");
	ToolTip * pToolTip = new ToolTip(0, 0, NULL);
	pWin->SetToolTipWnd(pToolTip);

	printf("err111\n");
	TVHelpWnd * pTVHelpWnd = new TVHelpWnd(pWin->mReal.wLeft + 400, pWin->mReal.wTop);
	pWin->SetHelpWnd(pTVHelpWnd);

	/*set translucency*/

	pPresent->Add(pTVHelpWnd);
	pPresent->Add(pToolTip, FALSE);
	pPresent->Add(pWin);
	
	pWin->ProcessUpDownKey();

	PegMessage tmpMsg;
	tmpMsg.pTarget = pToolTip;
	tmpMsg.pSource = pPresent;
	tmpMsg.wType = PM_SHOW;
	pPresent->Message(tmpMsg);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MainMenu::MainMenu(SIGNED iLeft, SIGNED iTop) : 
    PegWindow(FF_NONE)
{
    PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 49, iTop + 249);
    InitClient();
    AddStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 0, iTop + 200, iLeft + 49, iTop + 249);
	m_pMainMenuButton_SupFunction = new TVBitmapButton(ChildRect, &gbsuperfunctionBitmap, ID_MainMenu_Button_SupFunction);
    Add(m_pMainMenuButton_SupFunction);

	ChildRect.Set(iLeft + 0, iTop + 150, iLeft + 49, iTop + 199);
	m_pMainMenuButtonSupSetting = new TVBitmapButton(ChildRect, &gbsupersettingBitmap, ID_MainMenu_Button_SupSetting);
    Add(m_pMainMenuButtonSupSetting);

    ChildRect.Set(iLeft + 0, iTop + 100, iLeft + 49, iTop + 149);
	m_pMainMenuButtonSound = new TVBitmapButton(ChildRect, &gbsoundBitmap, ID_MainMenu_Button_Sound);
    Add(m_pMainMenuButtonSound);

    ChildRect.Set(iLeft + 0, iTop + 50, iLeft + 49, iTop + 99);
    m_pMainMenuButtonPicture = new TVBitmapButton(ChildRect, &gbpictureBitmap, ID_MainMenu_Buttion_Picture);
    Add(m_pMainMenuButtonPicture);

    ChildRect.Set(iLeft + 0, iTop + 0, iLeft + 49, iTop + 49);
	m_pMainMenuButtonFactorySetting = new TVBitmapButton(ChildRect, &gbfactoryBitmap, ID_MainMenu_Button_FactorySetting);
    Add(m_pMainMenuButtonFactorySetting);

    /* WB End Construction */
	m_pMainMenuButtonFactorySetting->SetTipString(LS(SID_MainMenu_Prompt_Factory));
	m_pMainMenuButton_SupFunction->SetTipString(LS(SID_MainMenu_Prompt_SuperFunction));
	m_pMainMenuButtonSupSetting->SetTipString(LS(SID_MainMenu_Prompt_SuperSetting));

	m_pMainMenuButtonSound->SetTipString(LS(SID_MainMenu_Prompt_Sound));
	m_pMainMenuButtonPicture->SetTipString(LS(SID_MainMenu_Prompt_Picture));

	m_pToolTip = NULL;
	m_pTVHelpWnd = NULL;
	m_nCurrentSubWndID = ID_MainMenu_Button_FactorySetting;


	Id(MAIN_MENU_ID);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED MainMenu::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(ID_MainMenu_Button_FactorySetting, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_MainMenu_Button_SupFunction, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_MainMenu_Button_SupSetting, PSF_CLICKED):
        // Enter your code here:
        break;
    case SIGNAL(ID_MainMenu_Button_Sound, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_MainMenu_Buttion_Picture, PSF_CLICKED):
        // Enter your code here:
        break;

	case PM_KEYRIGHT:
		{
			DisplayChildWindow(Mesg.iData);
			break;
		}
	case PM_KEYUP:
		{			
			m_nCurrentSubWndID = Mesg.iData;
			if( ID_MainMenu_Button_FactorySetting < m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID--;
			}
			else
			{
				m_nCurrentSubWndID = (int)ID_MainMenu_Button_SupFunction;
				Presentation()->MoveFocusTree(m_pMainMenuButton_SupFunction);
			}
			ProcessUpDownKey();
			break;
		}
	case PM_KEYDOWN:
		{
			m_nCurrentSubWndID = Mesg.iData;
			if( ID_MainMenu_Button_SupFunction > m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID++;
			}	
			else
			{
				m_nCurrentSubWndID = (int)ID_MainMenu_Button_FactorySetting;
				Presentation()->MoveFocusTree(m_pMainMenuButtonFactorySetting);
			}
			ProcessUpDownKey();
			break;
		}

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MainMenu::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();

    // Insert custom drawing code here:

    EndDraw();
}

void MainMenu::SetToolTipWnd(ToolTip * pToolTip)
{
	m_pToolTip = pToolTip;
}

void MainMenu::ProcessUpDownKey()
{
	switch(m_nCurrentSubWndID)
	{

	case ID_MainMenu_Button_FactorySetting:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_MainMenu_Prompt_Factory), NEWMENU_BUTTON_TYPE);
			m_pMainMenuButtonFactorySetting->SetToolTip(m_pToolTip);
			m_pMainMenuButtonFactorySetting->ShowTip();			
			break;
		}
	case ID_MainMenu_Button_SupFunction:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_MainMenu_Prompt_SuperFunction), NEWMENU_BUTTON_TYPE);
			m_pMainMenuButton_SupFunction->SetToolTip(m_pToolTip);
			m_pMainMenuButton_SupFunction->ShowTip();
			break;
		}
	case ID_MainMenu_Button_SupSetting:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_MainMenu_Prompt_SuperSetting), NEWMENU_BUTTON_TYPE);
			m_pMainMenuButtonSupSetting->SetToolTip(m_pToolTip);
			m_pMainMenuButtonSupSetting->ShowTip();
			break;
		}
	case ID_MainMenu_Button_Sound:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_MainMenu_Prompt_Sound), NEWMENU_BUTTON_TYPE);
			m_pMainMenuButtonSound->SetToolTip(m_pToolTip);
			m_pMainMenuButtonSound->ShowTip();
			break;
		}
	case ID_MainMenu_Buttion_Picture:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_MainMenu_Prompt_Picture), NEWMENU_BUTTON_TYPE);
			m_pMainMenuButtonPicture->SetToolTip(m_pToolTip);
			m_pMainMenuButtonPicture->ShowTip();
			break;
		}

	default:
		{
			//assert(FALSE);
		}
	}
}

void MainMenu::DisplayChildWindow(int iSubWndID)
{
	if(ID_MainMenu_Button_FactorySetting > iSubWndID || ID_MainMenu_Button_SupFunction < iSubWndID)
	{
		return;
	}

	m_nCurrentSubWndID = iSubWndID;

	SIGNED nTipLeftTopY = mReal.wTop + m_nCurrentSubWndID  * 50;
	PictureMenu * pPictureMenu = NULL;
	SoundMenu * pSoundMenu = NULL;
	SupSettingMenu * pSuperSettingMenu = NULL;
	SuperFuction * pSuperFunctionMenu = NULL;
	FactoryMenu * pFactoryMenu = NULL;
	
	switch(iSubWndID)
	{
		case ID_MainMenu_Buttion_Picture:
		{
			pPictureMenu = new PictureMenu(mReal.wRight, nTipLeftTopY);
			Presentation()->Add(pPictureMenu);
			pPictureMenu->SetTVHelpWnd(m_pTVHelpWnd);
			pPictureMenu->SetToolTipWnd(m_pToolTip);
			pPictureMenu->ProcessUpDownKey();
			break;
		}
		case ID_MainMenu_Button_Sound:
		{
			pSoundMenu = new SoundMenu(mReal.wRight, nTipLeftTopY);
			Presentation()->Add(pSoundMenu);
			pSoundMenu->SetTVHelpWnd(m_pTVHelpWnd);
			pSoundMenu->SetToolTipWnd(m_pToolTip);
			pSoundMenu->ProcessUpDownKey();
			break;
		}

		case ID_MainMenu_Button_SupSetting:
		{
			pSuperSettingMenu = new SupSettingMenu(mReal.wRight, nTipLeftTopY);
			Presentation()->Add(pSuperSettingMenu);
			pSuperSettingMenu->SetTVHelpWnd(m_pTVHelpWnd);
			pSuperSettingMenu->SetToolTipWnd(m_pToolTip);
			pSuperSettingMenu->ProcessUpDownKey();
			break;
		}
		case ID_MainMenu_Button_SupFunction:
		{
			pSuperFunctionMenu = new SuperFuction(mReal.wRight, nTipLeftTopY);
			Presentation()->Add(pSuperFunctionMenu);
			pSuperFunctionMenu->SetTVHelpWnd(m_pTVHelpWnd);
			pSuperFunctionMenu->SetToolTipWnd(m_pToolTip);
			pSuperFunctionMenu->ProcessUpDownKey();
			break;
		}
		case ID_MainMenu_Button_FactorySetting:
		{
			pFactoryMenu = new FactoryMenu(mReal.wRight, nTipLeftTopY);
			Presentation()->Add(pFactoryMenu);
			pFactoryMenu->SetTVHelpWnd(m_pTVHelpWnd);
			pFactoryMenu->SetToolTipWnd(m_pToolTip);
			pFactoryMenu->ProcessUpDownKey();
			break;
		}
	default:
		{
			break;
		}
	}
}

void MainMenu::SetHelpWnd(TVHelpWnd * pTVHelpWnd)
{
	m_pTVHelpWnd = pTVHelpWnd;
}

