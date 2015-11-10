/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: SupSettingMenu
//  
//   Copyright (c) ST microelectronics
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "wbstring.hpp"
#include "supsetmenu.hpp"

#include "tvbitmapbutton.hpp"
#include "usertype.hpp"
#include "mainmenu.hpp"
#include "helpwnd.hpp"
#include "tvprogwi.hpp"
#include "anim.hpp"
#include "tvselectwi.hpp"

#define NORMAL_SELECTBUTTON_LENGTH 150
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SupSettingMenu::SupSettingMenu(SIGNED iLeft, SIGNED iTop) : 
    PegWindow(FF_NONE)
{
    PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 49, iTop + 149);
    InitClient();
    AddStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 0, iTop + 100, iLeft + 49, iTop + 149);
    m_pSupSetMenuButtonHelp = new TVBitmapButton(ChildRect, &gbhelpbigBitmap, ID_SupSetMenu_Button_Help);
    Add(m_pSupSetMenuButtonHelp);

    ChildRect.Set(iLeft + 0, iTop + 50, iLeft + 49, iTop + 99);
    m_pSupsetMenuButtonInput = new TVBitmapButton(ChildRect, &gbprogramBitmap, ID_SupSetMenu_Button_Input);
    Add(m_pSupsetMenuButtonInput);

    ChildRect.Set(iLeft + 0, iTop + 0, iLeft + 49, iTop + 49);
    m_pSupSetMenuButtonZoom = new TVBitmapButton(ChildRect, &gbCRTBitmap, ID_SupSetMenu_Button_Zoom);
    Add(m_pSupSetMenuButtonZoom);

    /* WB End Construction */

	m_pSupSetMenuButtonHelp->SetTipString(LS(SID_SuperSettingMenu_Prompt_Help));
	m_pSupSetMenuButtonZoom->SetTipString(LS(SID_SuperSettingMenu_Prompt_Zoom));
	m_pSupsetMenuButtonInput->SetTipString(LS(SID_SuperSettingMenu_Prompt_Input));

	m_pToolTip = NULL;
	m_nCurrentSubWndID = ID_SupSetMenu_Button_Zoom;
	Id(SUPERSETTING_MENU_ID);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED SupSettingMenu::Message(const PegMessage &Mesg)
{
	TVProgressWindowClass * pTVProgressWindow = NULL;
	PegMessage tmpMessage;
	PegThing *SomeThing = NULL;

	switch (Mesg.wType)
    {
    case SIGNAL(ID_SupSetMenu_Button_Help, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_SupSetMenu_Button_Zoom, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_SupSetMenu_Button_Input, PSF_CLICKED):
        // Enter your code here:
        break;

	case PM_KEYLEFT:
		{
			Destroy(this);
			MainMenu * pMainMenu = (MainMenu *)Presentation()->Find(MAIN_MENU_ID);
			pMainMenu->ProcessUpDownKey();
			break;
		}
	case PM_KEYRIGHT:
		{
				switch(Mesg.iData)
				{
					case ID_SupSetMenu_Button_Input:
					{
						int i;
						HorizontalScrollText_t Strings;
						SIGNED nTipLeftTopY = mReal.wBottom - ((int)ID_SupSetMenu_Button_Input) * 50+2;
						TvSelectWindowClass * pTestMenu = NULL;
						Strings.numberofString = 3;
						Strings.StringToDisplay[0] = LS(SID_SuperSettingMenu_Input_Tuner);
						Strings.StringToDisplay[1] = LS(SID_SuperSettingMenu_Input_CVBS);
						Strings.StringToDisplay[2] = LS(SID_SuperSettingMenu_Input_YCrCb);
						for (i = 3; i< max_string; i++)
						{
							Strings.StringToDisplay[i] = NULL;
						}
						pTestMenu = new TvSelectWindowClass(mReal.wRight, nTipLeftTopY, NORMAL_SELECTBUTTON_LENGTH, Ssb_InputChange, kPegMenuSuperSetting,Strings, 0, SUPERSETTING_MENU_ID);
						m_pTVHelpWnd->ShowHelpString(LS(SID_SuperSettingMenu_Prompt_Input), NEWMENU_Select_TYPE);
						Presentation()->Add(pTestMenu);
						break;
					}
					case ID_SupSetMenu_Button_Zoom:
					{
						int i;
						HorizontalScrollText_t Strings;
						SIGNED nTipLeftTopY = mReal.wBottom - ((int)ID_SupSetMenu_Button_Zoom) * 50+2;
						TvSelectWindowClass * pTestMenu = NULL;
						Strings.numberofString = 3;
						Strings.StringToDisplay[0] = LS(SID_SuperSetMenu_Zoom_43);
						Strings.StringToDisplay[1] = LS(SID_SuperSetMenu_Zoom_169);
						Strings.StringToDisplay[2] = LS(SID_SuperSetMenu_Zoom_Panorama);
					for (i = 3; i< max_string; i++)
						{
							Strings.StringToDisplay[i] = NULL;
						}
						pTestMenu = new TvSelectWindowClass(mReal.wRight, nTipLeftTopY, NORMAL_SELECTBUTTON_LENGTH, Ssb_ZoomModeChange, kPegMenuSuperSetting,Strings, 0, SUPERSETTING_MENU_ID);
						m_pTVHelpWnd->ShowHelpString(LS(SID_SuperSettingMenu_Prompt_Zoom), NEWMENU_Select_TYPE);
						Presentation()->Add(pTestMenu);
						break;
					}
					case ID_SupSetMenu_Button_Help:
					{
						int i;
						HorizontalScrollText_t Strings;
						SIGNED nTipLeftTopY = mReal.wBottom - ((int)ID_SupSetMenu_Button_Help) * 50+2;
						TvSelectWindowClass * pTestMenu = NULL;
						Strings.numberofString = 2;
						Strings.StringToDisplay[0] = LS(SID_SuperSetMenu_SelectHelp_On);
						Strings.StringToDisplay[1] = LS(SID_SuperSetMenu_SelectHelp_Off);
						for (i = 2; i< max_string; i++)
						{
							Strings.StringToDisplay[i] = NULL;
						}
						pTestMenu = new TvSelectWindowClass(mReal.wRight, nTipLeftTopY, NORMAL_SELECTBUTTON_LENGTH, Ssb_HelpModeChange, kPegMenuSuperSetting, Strings, 0, SUPERSETTING_MENU_ID);
						m_pTVHelpWnd->ShowHelpString(LS(SID_SuperSettingMenu_Prompt_Help), NEWMENU_Select_TYPE);
						Presentation()->Add(pTestMenu);
						break;
					}
					default:
						break;
				}
			break;
		}
			
	case PM_KEYUP:
		{			
			m_nCurrentSubWndID = Mesg.iData;
			if(ID_SupSetMenu_Button_Zoom > m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID++;
			}
			else
			{
				m_nCurrentSubWndID = (int)ID_SupSetMenu_Button_Help;
				Presentation()->MoveFocusTree(m_pSupSetMenuButtonHelp);
			}
			ProcessUpDownKey();
			break;
		}
	case PM_KEYDOWN:
		{
			m_nCurrentSubWndID = Mesg.iData;
			if(ID_SupSetMenu_Button_Help < m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID--;
			}
			else
			{
				m_nCurrentSubWndID = (int)ID_SupSetMenu_Button_Zoom;
				Presentation()->MoveFocusTree(m_pSupSetMenuButtonZoom);
			}
			ProcessUpDownKey();
			break;
		}

	case PM_REFRESHHELPWND:
		{
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
void SupSettingMenu::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();

    // Insert custom drawing code here:

    EndDraw();
}

void SupSettingMenu::SetToolTipWnd(ToolTip * pToolTip)
{
	m_pToolTip = pToolTip;
}

void SupSettingMenu::SetTVHelpWnd(TVHelpWnd * pTVHelpWnd)
{
	m_pTVHelpWnd = pTVHelpWnd;
}

void SupSettingMenu::ProcessUpDownKey()
{
	switch(m_nCurrentSubWndID)
	{
	case ID_SupSetMenu_Button_Zoom:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_SuperSettingMenu_Prompt_Zoom), NEWMENU_BUTTON_TYPE);
			m_pSupSetMenuButtonZoom->SetToolTip(m_pToolTip);
			m_pSupSetMenuButtonZoom->ShowTip();
			break;
		}

	case ID_SupSetMenu_Button_Input:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_SuperSettingMenu_Prompt_Input), NEWMENU_BUTTON_TYPE);
			m_pSupsetMenuButtonInput->SetToolTip(m_pToolTip);
			m_pSupsetMenuButtonInput->ShowTip();
			break;
		}
	case ID_SupSetMenu_Button_Help:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_SuperSettingMenu_Prompt_Help), NEWMENU_BUTTON_TYPE);
			m_pSupSetMenuButtonHelp->SetToolTip(m_pToolTip);
			m_pSupSetMenuButtonHelp->ShowTip();
			break;
		}

	default:
		{
			//assert(FALSE);
		}
	}

	Presentation()->Draw();
}


