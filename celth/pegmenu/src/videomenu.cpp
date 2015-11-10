/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: VideoMenu
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
#include "videomenu.hpp"
#include "tvbitmapbutton.hpp"
#include "usertype.hpp"
#include "factorymenu.hpp"
#include "helpwnd.hpp"
#include "tvprogwi.hpp"
#include "tvselectwi.hpp"


#include "anim.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
VideoMenu::VideoMenu(SIGNED iLeft, SIGNED iTop) : 
    PegWindow(FF_NONE)
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 49, iTop + 349);
    InitClient();
    AddStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 0, iTop + 300, iLeft + 49, iTop + 349);
    m_pVideoMenuButtonAutoFlesh = new TVBitmapButton(ChildRect, &gbmanualBitmap, ID_VideoMenu_Button_AutoFlesh);
    Add(m_pVideoMenuButtonAutoFlesh);

    ChildRect.Set(iLeft + 0, iTop + 250, iLeft + 49, iTop + 299);
    m_pVideoMenuButtonPeaking = new TVBitmapButton(ChildRect, &gbfinetuneBitmap, ID_VideoMenu_Button_Peaking);
    Add(m_pVideoMenuButtonPeaking);

    ChildRect.Set(iLeft + 0, iTop + 200, iLeft + 49, iTop + 249);
    m_pVideoMenuButtonDCI = new TVBitmapButton(ChildRect, &gbautoBitmap, ID_VideoMenu_Button_DCI);
    Add(m_pVideoMenuButtonDCI);

    ChildRect.Set(iLeft + 0, iTop + 150, iLeft + 49, iTop + 199);
    m_pVideoMenuButtonEdgeReplace = new TVBitmapButton(ChildRect, &gbgeomagnetismBitmap, ID_VideoMenu_Button_EdgeReplace);
    Add(m_pVideoMenuButtonEdgeReplace);

    ChildRect.Set(iLeft + 0, iTop + 100, iLeft + 49, iTop + 149);
    m_pVideoMenuButtonGreenBoost = new TVBitmapButton(ChildRect, &gblanguageBitmap, ID_VideoMenu_Button_GreenBoost);
    Add(m_pVideoMenuButtonGreenBoost);

    ChildRect.Set(iLeft + 0, iTop + 50, iLeft + 49, iTop + 99);
    m_pVideoMenuButtonSaturation = new TVBitmapButton(ChildRect, &gbreducenoiseBitmap, ID_VideoMenu_Button_Saturation);
    Add(m_pVideoMenuButtonSaturation);

    ChildRect.Set(iLeft + 0, iTop + 0, iLeft + 49, iTop + 49);
    m_pVideoMenuButtonTint = new TVBitmapButton(ChildRect, &gbcolorBitmap, ID_VideoMenu_Button_Tint);
    Add(m_pVideoMenuButtonTint);

    /* WB End Construction */

	m_pVideoMenuButtonAutoFlesh->SetTipString(LS(SID_VideoMenu_Prompt_AutoFlesh));
	m_pVideoMenuButtonPeaking->SetTipString(LS(SID_VideoMenu_Prompt_Peaking));
	m_pVideoMenuButtonDCI->SetTipString(LS(SID_VideoMenu_Prompt_DCI));
	m_pVideoMenuButtonEdgeReplace->SetTipString(LS(SID_VideoMenu_Prompt_EdgeReplace));
	m_pVideoMenuButtonGreenBoost->SetTipString(LS(SID_VideoMenu_Prompt_GreenBoost));
       m_pVideoMenuButtonSaturation->SetTipString(LS(SID_VideoMenu_Prompt_Saturation)); 
	m_pVideoMenuButtonTint->SetTipString(LS(SID_VideoMenu_Prompt_Tint));

	m_pToolTip = NULL;
	m_nCurrentSubWndID = ID_VideoMenu_Button_Tint;
	Id(VIDEO_MENU_ID);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED VideoMenu::Message(const PegMessage &Mesg)
{
	TVProgressWindowClass * pTVProgressWindow = NULL;
	PegThing *ptDisplayedWindow[MaxNumberOfWindow];
	PegThing *SomeThing = NULL;
	PegThing *pTest;
	int iIndex;

    switch (Mesg.wType)
    {
    case SIGNAL(ID_VideoMenu_Button_AutoFlesh, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_VideoMenu_Button_Peaking, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_VideoMenu_Button_DCI, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_VideoMenu_Button_EdgeReplace, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_VideoMenu_Button_GreenBoost, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_VideoMenu_Button_Saturation, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_VideoMenu_Button_Tint, PSF_CLICKED):
        // Enter your code here:
        break;
	case PM_KEYLEFT:
		{
			Destroy(this);
			FactoryMenu * pFactoryMenu = (FactoryMenu *)Presentation()->Find(FACTORY_MENU_ID);
			pFactoryMenu->ProcessUpDownKey();
			break;
		}
	case PM_KEYRIGHT:
		{

			if(ID_VideoMenu_Button_Peaking == Mesg.iData || \
			   ID_VideoMenu_Button_DCI == Mesg.iData || \
			   ID_VideoMenu_Button_EdgeReplace == Mesg.iData || \
			   ID_VideoMenu_Button_GreenBoost == Mesg.iData || \
			   ID_VideoMenu_Button_Saturation == Mesg.iData || \
			   ID_VideoMenu_Button_Tint == Mesg.iData )
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
				case ID_VideoMenu_Button_Tint:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pVideoMenuButtonTint, \
							Ssb_TintChange, kPegMenuPicture, \
							LS(SID_VideoMenu_Prompt_Tint), RED, FF_RAISED | PS_SHOW_VAL, 0, \
							100, 30, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_Tint), NEWMENU_BAR_TYPE);
						break;
					}
				case ID_VideoMenu_Button_Saturation:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pVideoMenuButtonSaturation, \
							Ssb_ColorChange, kPegMenuPicture, \
							LS(SID_VideoMenu_Prompt_Saturation), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 30, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_Saturation), NEWMENU_BAR_TYPE);
						break;
					}

				case ID_VideoMenu_Button_GreenBoost:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pVideoMenuButtonGreenBoost, \
							Ssb_GreenBoostChange, kPegMenuVideo, \
							LS(SID_VideoMenu_Prompt_GreenBoost), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 30, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_GreenBoost), NEWMENU_BAR_TYPE);
						break;
					}
				case ID_VideoMenu_Button_EdgeReplace:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pVideoMenuButtonEdgeReplace, \
							Ssb_EdgeReplacementChange, kPegMenuVideo, \
							LS(SID_VideoMenu_Prompt_EdgeReplace), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 30, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_EdgeReplace), NEWMENU_BAR_TYPE);
						break;
					}
				case ID_VideoMenu_Button_DCI:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pVideoMenuButtonDCI, \
							Ssb_DCIChange,  kPegMenuVideo, \
							LS(SID_VideoMenu_Prompt_DCI_Short), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 20, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_DCI), NEWMENU_BAR_TYPE);
						break;
					  }
				case ID_VideoMenu_Button_Peaking:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pVideoMenuButtonPeaking, \
							Ssb_PeakingChange, kPegMenuVideo, \
							LS(SID_VideoMenu_Prompt_Peaking), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 20, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_Peaking), NEWMENU_BAR_TYPE);
						break;
					 }

				default:
					break;
				}
				EndDraw();
			}
			else
			{
				switch(Mesg.iData)
				{
					case ID_VideoMenu_Button_AutoFlesh:
					{
						int i;
						HorizontalScrollText_t Strings;
						SIGNED nTipLeftTopY = mReal.wBottom - ((int)ID_VideoMenu_Button_AutoFlesh) * 50;
						TvSelectWindowClass * pTestMenu = NULL;
						Strings.numberofString = 2;
						Strings.StringToDisplay[0] = LS(SID_VideoMenu_AutoFlash_USA);
						Strings.StringToDisplay[1] = LS(SID_VideoMenu_AutoFlash_Japan);
						for (i = 2; i< max_string; i++)
						{
							Strings.StringToDisplay[i] = NULL;
						}
						pTestMenu = new TvSelectWindowClass(mReal.wRight, nTipLeftTopY, 200, Ssb_AutoFleshChange, kPegMenuVideo, Strings, 0, VIDEO_MENU_ID);
						m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_AutoFlesh), NEWMENU_Select_TYPE);
						Presentation()->Add(pTestMenu);
						break;
					}
					default:
						break;
				}
			}
			break;
		}
		
	case PM_KEYUP:
		{			
			m_nCurrentSubWndID = Mesg.iData;
			if(ID_VideoMenu_Button_Tint > m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID++;
			}
			else
			{
				m_nCurrentSubWndID = (int)ID_VideoMenu_Button_AutoFlesh;
				Presentation()->MoveFocusTree(m_pVideoMenuButtonAutoFlesh);
			}
			ProcessUpDownKey();
			break;
		}
	case PM_KEYDOWN:
		{
			m_nCurrentSubWndID = Mesg.iData;
			if(ID_VideoMenu_Button_AutoFlesh< m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID--;
			}
			else
			{
				m_nCurrentSubWndID = (int)ID_VideoMenu_Button_Tint;
				Presentation()->MoveFocusTree(m_pVideoMenuButtonTint);
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
void VideoMenu::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();

    // Insert custom drawing code here:

    EndDraw();
}

void VideoMenu::SetToolTipWnd(ToolTip * pToolTip)
{
	m_pToolTip = pToolTip;
}

void VideoMenu::SetTVHelpWnd(TVHelpWnd * pTVHelpWnd)
{
	m_pTVHelpWnd = pTVHelpWnd;
}

void VideoMenu::ProcessUpDownKey()
{
	switch(m_nCurrentSubWndID)
	{
	case ID_VideoMenu_Button_Tint:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_Tint), NEWMENU_BUTTON_TYPE);
			m_pVideoMenuButtonTint->SetToolTip(m_pToolTip);
			m_pVideoMenuButtonTint->ShowTip();
			break;
		}
	case ID_VideoMenu_Button_Saturation:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_Saturation), NEWMENU_BUTTON_TYPE);
			m_pVideoMenuButtonSaturation->SetToolTip(m_pToolTip);
			m_pVideoMenuButtonSaturation->ShowTip();
			break;
		}
	case ID_VideoMenu_Button_GreenBoost:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_GreenBoost), NEWMENU_BUTTON_TYPE);
			m_pVideoMenuButtonGreenBoost->SetToolTip(m_pToolTip);
			m_pVideoMenuButtonGreenBoost->ShowTip();
			break;
		}
	case ID_VideoMenu_Button_EdgeReplace:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_EdgeReplace), NEWMENU_BUTTON_TYPE);
			m_pVideoMenuButtonEdgeReplace->SetToolTip(m_pToolTip);
			m_pVideoMenuButtonEdgeReplace->ShowTip();
			break;
		}
	case ID_VideoMenu_Button_DCI:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_DCI), NEWMENU_BUTTON_TYPE);
			m_pVideoMenuButtonDCI->SetToolTip(m_pToolTip);
			m_pVideoMenuButtonDCI->ShowTip();
			break;
		}
	case ID_VideoMenu_Button_Peaking:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_Peaking), NEWMENU_BUTTON_TYPE);
			m_pVideoMenuButtonPeaking->SetToolTip(m_pToolTip);
			m_pVideoMenuButtonPeaking->ShowTip();
			break;
		}
	case ID_VideoMenu_Button_AutoFlesh:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_VideoMenu_Prompt_AutoFlesh), NEWMENU_BUTTON_TYPE);
			m_pVideoMenuButtonAutoFlesh->SetToolTip(m_pToolTip);
			m_pVideoMenuButtonAutoFlesh->ShowTip();
			break;
		}
	default:
		{
			//assert(FALSE);
		}
	}

	Presentation()->Draw();
}
