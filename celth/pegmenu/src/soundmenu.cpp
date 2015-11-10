/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: SoundMenu
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
#include "soundmenu.hpp"
#include "tvbitmapbutton.hpp"
#include "usertype.hpp"
#include "mainmenu.hpp"
#include "helpwnd.hpp"
#include "tvprogwi.hpp"
#include "tvselectwi.hpp"


#include "anim.hpp"

#define NORMAL_SELECTBUTTON_LENGTH  150

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SoundMenu::SoundMenu(SIGNED iLeft, SIGNED iTop) : 
    PegWindow(FF_NONE)
{
    PegRect ChildRect;

	mReal.Set(iLeft, iTop, iLeft + 49, iTop + 99);
    InitClient();
    AddStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 0, iTop + 50, iLeft + 49, iTop + 99);
	m_pSoundMenuButtonMute = new TVBitmapButton(ChildRect, &gbsoundBitmap, ID_SoundMenu_Button_Mute);
    Add(m_pSoundMenuButtonMute);

    ChildRect.Set(iLeft + 0, iTop + 0, iLeft + 49, iTop + 49);
    m_pSoundMenuButtonVolume = new TVBitmapButton(ChildRect, &gbvolumeBitmap, ID_SoundMenu_Button_Volume);
    Add(m_pSoundMenuButtonVolume);
    /* WB End Construction */

	m_pSoundMenuButtonVolume->SetTipString(LS(SID_SoundMenu_Prompt_Volume));
	m_pSoundMenuButtonMute->SetTipString(LS(SID_SoundMenu_Prompt_Mute));

	m_pToolTip = NULL;
	m_nCurrentSubWndID = ID_SoundMenu_Button_Volume;
	Id(SOUND_MENU_ID);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED SoundMenu::Message(const PegMessage &Mesg)
{
	TVProgressWindowClass * pTVProgressWindow = NULL;

	PegThing *ptDisplayedWindow[MaxNumberOfWindow];
	PegThing *SomeThing = NULL;
	PegThing *pTest;
	int iIndex;

    switch (Mesg.wType)
    {

	case SIGNAL(ID_SoundMenu_Button_Volume, PSF_CLICKED):
        // Enter your code here:
        break;
	case SIGNAL(ID_SoundMenu_Button_Mute, PSF_CLICKED):
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

			if(ID_SoundMenu_Button_Volume == Mesg.iData)
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
				case ID_SoundMenu_Button_Volume:
					{
						pTVProgressWindow = new TVProgressWindowClass(100, 400, m_pSoundMenuButtonVolume, \
							Ssb_VolumeChange, kPegMenuSound, \
							LS(SID_SoundMenu_Prompt_Volume), RED, FF_RAISED|PS_SHOW_VAL, 0, \
							100, 20, ptDisplayedWindow);
						Presentation()->Add(pTVProgressWindow);
						m_pTVHelpWnd->ShowHelpString(LS(SID_SoundMenu_Prompt_Volume), NEWMENU_BAR_TYPE);
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
					case ID_SoundMenu_Button_Mute:
					{
						int i;
						HorizontalScrollText_t Strings;
						SIGNED nTipLeftTopY = mReal.wBottom - ((int)ID_SoundMenu_Button_Mute) * 50;
						TvSelectWindowClass * pTestMenu = NULL;
						Strings.numberofString = 2;
						Strings.StringToDisplay[0] = LS(SID_SoundMenu_Prompt_Mute_On);
						Strings.StringToDisplay[1] = LS(SID_SoundMenu_Prompt_Mute_Off);
						for (i = 2; i< max_string; i++)
						{
							Strings.StringToDisplay[i] = NULL;
						}
						pTestMenu = new TvSelectWindowClass(mReal.wRight, nTipLeftTopY, NORMAL_SELECTBUTTON_LENGTH, Ssb_MuteModeChange, kPegMenuSound, Strings, 0, SOUND_MENU_ID);
						Presentation()->Add(pTestMenu);
						m_pTVHelpWnd->ShowHelpString(LS(SID_SoundMenu_Prompt_Mute), NEWMENU_Select_TYPE );
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
			if(ID_SoundMenu_Button_Volume > m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID++;
			}
			else
			{
				m_nCurrentSubWndID = (int)ID_SoundMenu_Button_Mute;
				Presentation()->MoveFocusTree(m_pSoundMenuButtonMute);
			}
			ProcessUpDownKey();
			break;
		}
	case PM_KEYDOWN:
		{
			m_nCurrentSubWndID = Mesg.iData;
			if(ID_SoundMenu_Button_Mute < m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID--;
			}
			else
			{
				m_nCurrentSubWndID = (int)ID_SoundMenu_Button_Volume;
				Presentation()->MoveFocusTree(m_pSoundMenuButtonVolume);
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
void SoundMenu::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();

    // Insert custom drawing code here:

    EndDraw();
}

void SoundMenu::SetToolTipWnd(ToolTip * pToolTip)
{
	m_pToolTip = pToolTip;
}

void SoundMenu::SetTVHelpWnd(TVHelpWnd * pTVHelpWnd)
{
	m_pTVHelpWnd = pTVHelpWnd;
}

void SoundMenu::ProcessUpDownKey()
{
	switch(m_nCurrentSubWndID)
	{

	case ID_SoundMenu_Button_Volume:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_SoundMenu_Prompt_Volume), NEWMENU_BUTTON_TYPE);
			m_pSoundMenuButtonVolume->SetToolTip(m_pToolTip);
			m_pSoundMenuButtonVolume->ShowTip();
			break;
		}
	case ID_SoundMenu_Button_Mute:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_SoundMenu_Prompt_Mute), NEWMENU_BUTTON_TYPE);
			m_pSoundMenuButtonMute->SetToolTip(m_pToolTip);
			m_pSoundMenuButtonMute->ShowTip();
			break;
		}

	default:
		{
			//assert(FALSE);
		}
	}

	Presentation()->Draw();
}
