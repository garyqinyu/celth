/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: PictureMenu
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
#include "picturemenu.hpp"

#include "tvbitmapbutton.hpp"
#include "usertype.hpp"
#include "mainmenu.hpp"
#include "helpwnd.hpp"
#include "tooltip.hpp"
#include "tvprogwi.hpp"

#include "anim.hpp"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PictureMenu::PictureMenu(SIGNED iLeft, SIGNED iTop) : 
    PegWindow(FF_NONE)
{
    PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 49, iTop + 299);
    InitClient();
    AddStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 0, iTop + 250, iLeft + 49, iTop + 299);
    m_pPictureMenuButtonColorTemp = new TVBitmapButton(ChildRect, &gbcolortempBitmap, ID_PictureMenu_Button_ColorTemp);
    Add(m_pPictureMenuButtonColorTemp);

    ChildRect.Set(iLeft + 0, iTop + 200, iLeft + 49, iTop + 249);
    m_pPictureMenuButtonColor = new TVBitmapButton(ChildRect, &gbreducenoiseBitmap, ID_PictureMenu_Button_Color);
    Add(m_pPictureMenuButtonColor);

    ChildRect.Set(iLeft + 0, iTop + 150, iLeft + 49, iTop + 199);
    m_pPictureMenuButtonSharp = new TVBitmapButton(ChildRect, &gbsharpnessBitmap, ID_PictureMenu_Button_Sharp);
    Add(m_pPictureMenuButtonSharp);

    ChildRect.Set(iLeft + 0, iTop + 100, iLeft + 49, iTop + 149);
    m_pPictureMenuButtonTint = new TVBitmapButton(ChildRect, &gbcolorBitmap, ID_PictureMenu_Button_Tint);
    Add(m_pPictureMenuButtonTint);

    ChildRect.Set(iLeft + 0, iTop + 50, iLeft + 49, iTop + 99);
    m_pPictureMenuButtonContrast = new TVBitmapButton(ChildRect, &gbcontrastBitmap, ID_PictureMenu_Button_Contrast);
    Add(m_pPictureMenuButtonContrast);

    ChildRect.Set(iLeft + 0, iTop + 0, iLeft + 49, iTop + 49);
    m_pPictuerMenuButtonBright = new TVBitmapButton(ChildRect, &gbbrightnessBitmap, ID_PictureMenu_Button_Brightness);
    Add(m_pPictuerMenuButtonBright);

    /* WB End Construction */
	m_pPictureMenuButtonColorTemp->SetTipString(LS(SID_PictureMenu_Prompt_ColorTemp));
	m_pPictureMenuButtonColor->SetTipString(LS(SID_PictureMenu_Prompt_Color));
    m_pPictureMenuButtonSharp->SetTipString(LS(SID_PictureMenu_Prompt_Sharpness)); 
	m_pPictureMenuButtonTint->SetTipString(LS(SID_PictureMenu_Prompt_Tint));
	m_pPictureMenuButtonContrast->SetTipString(LS(SID_PictureMenu_Prompt_Contrast));
	m_pPictuerMenuButtonBright->SetTipString(LS(SID_PictureMenu_Prompt_Brightness));
	m_pToolTip = NULL;
	m_nCurrentSubWndID = ID_PictureMenu_Button_Brightness;
	Id(PICTURE_MENU_ID);

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PictureMenu::Message(const PegMessage &Mesg)
{
	TVProgressWindowClass * pTVProgressWindow = NULL;
	PegThing *ptDisplayedWindow[MaxNumberOfWindow];
	PegThing *SomeThing = NULL;
	PegThing *pTest;
	int iIndex;

    switch (Mesg.wType)
    {
    case SIGNAL(ID_PictureMenu_Button_ColorTemp, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_PictureMenu_Button_Color, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_PictureMenu_Button_Sharp, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_PictureMenu_Button_Tint, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_PictureMenu_Button_Contrast, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_PictureMenu_Button_Brightness, PSF_CLICKED):
        // Enter your code here:
        break;
	case PM_KEYUP:
		{			
			m_nCurrentSubWndID = Mesg.iData;
			if(ID_PictureMenu_Button_Brightness > m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID++;
			}	
			else
			{
				m_nCurrentSubWndID = (int)ID_PictureMenu_Button_ColorTemp;
				Presentation()->MoveFocusTree(m_pPictureMenuButtonColorTemp);
			}
			ProcessUpDownKey();
			break;
		}
	case PM_KEYDOWN:
		{
			m_nCurrentSubWndID = Mesg.iData;
			if(ID_PictureMenu_Button_ColorTemp < m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID--;
			}
			else
			{
				m_nCurrentSubWndID = (int)ID_PictureMenu_Button_Brightness;
				Presentation()->MoveFocusTree(m_pPictuerMenuButtonBright);
			}
			ProcessUpDownKey();
			break;
		}
	case PM_KEYLEFT:
		{
			Destroy(this);
			MainMenu * pMainMenu = (MainMenu *)Presentation()->Find(MAIN_MENU_ID);
			pMainMenu->ProcessUpDownKey();
			break;
		}
	case PM_KEYRIGHT:
		{
			if(ID_PictureMenu_Button_ColorTemp == Mesg.iData || \
			   ID_PictureMenu_Button_Color == Mesg.iData || \
			   ID_PictureMenu_Button_Tint == Mesg.iData || \
			   ID_PictureMenu_Button_Contrast == Mesg.iData || \
			   ID_PictureMenu_Button_Sharp == Mesg.iData || \
			   ID_PictureMenu_Button_Brightness == Mesg.iData)
			{
				/* right key equals OK, so enter next menu level */
				BeginDraw();
				/* Delete all pop up already displayed to clean the screen */
				for(iIndex = FIRST_POPUP_ID; iIndex < LAST_USER_ID; iIndex++)
				{
					SomeThing = PegThing::Presentation()->Find(iIndex);
					if(SomeThing!=NULL)
					{
						PegThing::Presentation()->Destroy(SomeThing);
					}
				}
				/*hide all the menus displayed on screen except the help window*/
				pTest = PegPresentationManager::Presentation()->First();
				iIndex = 0;
				while(pTest) /* construct the current displayed window pointer table */
				{
					if(HELP_ID == pTest->Id())
					{					
						pTest = PegPresentationManager::Presentation()->Next();
						continue;
					}
					else
					{
						ptDisplayedWindow[iIndex++] = pTest;
						PegPresentationManager::Presentation()->Remove(pTest);
						pTest = PegPresentationManager::Presentation()->First();
					}
				}
				ptDisplayedWindow[iIndex] = NULL; /* reset last pointer */
				
				switch(Mesg.iData)
				{
				case ID_PictureMenu_Button_Brightness:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pPictuerMenuButtonBright, \
							Ssb_BrightnessChange, kPegMenuPicture, \
							LS(SID_PictureMenu_Prompt_Brightness), RED, FF_RAISED | PS_SHOW_VAL, 0, \
							100, 30, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_Brightness), NEWMENU_BAR_TYPE);
						break;
					}
				case ID_PictureMenu_Button_Contrast:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pPictureMenuButtonContrast, \
							Ssb_ContrastChange, kPegMenuPicture, \
							LS(SID_PictureMenu_Prompt_Contrast), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 30, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_Contrast), NEWMENU_BAR_TYPE);
						break;
					}

				case ID_PictureMenu_Button_Tint:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pPictureMenuButtonTint, \
							Ssb_TintChange, kPegMenuPicture, \
							LS(SID_PictureMenu_Prompt_Tint), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 30, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_Tint), NEWMENU_BAR_TYPE);
						break;
					}
				case ID_PictureMenu_Button_ColorTemp:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pPictureMenuButtonColorTemp, \
							Ssb_ColorTemperatureChange, kPegMenuPicture, \
							LS(SID_PictureMenu_Prompt_ColorTemp), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 30, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_ColorTemp), NEWMENU_BAR_TYPE);
						break;
					}
				case ID_PictureMenu_Button_Color:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pPictureMenuButtonColor, \
							Ssb_ColorChange, kPegMenuPicture, \
							LS(SID_PictureMenu_Prompt_Color), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 20, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_Color), NEWMENU_BAR_TYPE);
						break;
					  }
				case ID_PictureMenu_Button_Sharp:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pPictureMenuButtonSharp, \
							Ssb_SharpnessChange, kPegMenuPicture, \
							LS(SID_PictureMenu_Prompt_Sharpness), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 20, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_Sharpness), NEWMENU_BAR_TYPE);
						break;
					 }

				default:
					break;
				}
				EndDraw();
			}
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
void PictureMenu::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();

    // Insert custom drawing code here:

    EndDraw();
}


void PictureMenu::SetToolTipWnd(ToolTip * pToolTip)
{
	m_pToolTip = pToolTip;
}

void PictureMenu::SetTVHelpWnd(TVHelpWnd * pTVHelpWnd)
{
	m_pTVHelpWnd = pTVHelpWnd;
}

void PictureMenu::ProcessUpDownKey()
{
	switch(m_nCurrentSubWndID)
	{
	case ID_PictureMenu_Button_Brightness:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_Brightness), NEWMENU_BUTTON_TYPE);
			m_pPictuerMenuButtonBright->SetToolTip(m_pToolTip);
			m_pPictuerMenuButtonBright->ShowTip();
			break;
		}
	case ID_PictureMenu_Button_Contrast:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_Contrast), NEWMENU_BUTTON_TYPE);
			m_pPictureMenuButtonContrast->SetToolTip(m_pToolTip);
			m_pPictureMenuButtonContrast->ShowTip();
			break;
		}
	case ID_PictureMenu_Button_Tint:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_Tint), NEWMENU_BUTTON_TYPE);
			m_pPictureMenuButtonTint->SetToolTip(m_pToolTip);
			m_pPictureMenuButtonTint->ShowTip();
			break;
		}
	case ID_PictureMenu_Button_Sharp:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_Sharpness), NEWMENU_BUTTON_TYPE);
			m_pPictureMenuButtonSharp->SetToolTip(m_pToolTip);
			m_pPictureMenuButtonSharp->ShowTip();
			break;
		}
	case ID_PictureMenu_Button_Color:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_Color), NEWMENU_BUTTON_TYPE);
			m_pPictureMenuButtonColor->SetToolTip(m_pToolTip);
			m_pPictureMenuButtonColor->ShowTip();
			break;
		}
	case ID_PictureMenu_Button_ColorTemp:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_PictureMenu_Prompt_ColorTemp), NEWMENU_BUTTON_TYPE);
			m_pPictureMenuButtonColorTemp->SetToolTip(m_pToolTip);
			m_pPictureMenuButtonColorTemp->ShowTip();
			break;
		}

	default:
		{
			//assert(FALSE);
		}
	}

	Presentation()->Draw();
}
