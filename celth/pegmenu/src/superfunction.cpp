/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: SuperFuction
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
#include "superfunction.hpp"

#include "tvbitmapbutton.hpp"
#include "usertype.hpp"
#include "mainmenu.hpp"
#include "helpwnd.hpp"

#include "anim.hpp"
#include "tvselectwi.hpp"

#define LONG_SELECTBUTTON_LENGTH 200
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SuperFuction::SuperFuction(SIGNED iLeft, SIGNED iTop) : 
    PegWindow(FF_NONE)
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 49, iTop + 199);
    InitClient();
    AddStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 0, iTop + 150, iLeft + 49, iTop + 199);
    m_pSupFunctionMenuButtonCA = new TVBitmapButton(ChildRect, &gbtimepreserveBitmap, ID_SupFunctionMenu_Button_CA);
    Add(m_pSupFunctionMenuButtonCA);

    ChildRect.Set(iLeft + 0, iTop + 100, iLeft + 49, iTop + 149);
    m_pSupFunctionMenuButtonCC = new TVBitmapButton(ChildRect, &gbsettimeBitmap, ID_SupFunctionMenu_Button_CC);
    Add(m_pSupFunctionMenuButtonCC);

    ChildRect.Set(iLeft + 0, iTop + 50, iLeft + 49, iTop + 99);
    m_pSupFunctionMenuButtonParentControl = new TVBitmapButton(ChildRect, &gbparentcontrolBitmap,ID_SupFunctionMenu_Button_ParentControl);
    Add(m_pSupFunctionMenuButtonParentControl);

    ChildRect.Set(iLeft + 0, iTop + 0, iLeft + 49, iTop + 49);
    m_pSupFunctionMenuButtonEPG = new TVBitmapButton(ChildRect, &gbepgBitmap, ID_SupFunctionMenu_Button_EPG);   
	Add(m_pSupFunctionMenuButtonEPG);

    /* WB End Construction */
	m_pSupFunctionMenuButtonCA->SetTipString(LS(SID_SuperFuctionMenu_Prompt_CA));
	m_pSupFunctionMenuButtonCC->SetTipString(LS(SID_SuperFuctionMenu_Prompt_CC));
	m_pSupFunctionMenuButtonParentControl->SetTipString(LS(SID_SuperFunctionMenu_Prompt_Parent));
	m_pSupFunctionMenuButtonEPG->SetTipString(LS(SID_SuperFuctionMenu_Prompt_EPG));

	m_pToolTip = NULL;
	m_nCurrentSubWndID = ID_SupFunctionMenu_Button_EPG;
	Id(SUPERFUNCTION_MENU_ID);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED SuperFuction::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(ID_SupFunctionMenu_Button_CA, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_SupFunctionMenu_Button_CC, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_SupFunctionMenu_Button_ParentControl, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_SupFunctionMenu_Button_EPG, PSF_CLICKED):
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
					case ID_SupFunctionMenu_Button_CC:
					{
						int i;
						HorizontalScrollText_t Strings;
						SIGNED nTipLeftTopY = mReal.wBottom - ((int)ID_SupFunctionMenu_Button_CC) * 50+2;
						TvSelectWindowClass * pTestMenu = NULL;
						Strings.numberofString = 2;
						Strings.StringToDisplay[0] = LS(SID_SuperSetMenu_SelectHelp_On);
						Strings.StringToDisplay[1] = LS(SID_SuperSetMenu_SelectHelp_Off);
						for (i = 2; i< max_string; i++)
						{
							Strings.StringToDisplay[i] = NULL;
						}
						pTestMenu = new TvSelectWindowClass(mReal.wRight, nTipLeftTopY, LONG_SELECTBUTTON_LENGTH, Ssb_CloseCaptionChange, kPegMenuSuperFunction, Strings, 0, SUPERFUNCTION_MENU_ID);
						m_pTVHelpWnd->ShowHelpString(LS(SID_SuperFuctionMenu_Prompt_CC), NEWMENU_Select_TYPE);
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
			if(ID_SupFunctionMenu_Button_EPG > m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID++;
			}
			else
			{
				m_nCurrentSubWndID = (int)ID_SupFunctionMenu_Button_CA;
				Presentation()->MoveFocusTree(m_pSupFunctionMenuButtonCA);
			}
			ProcessUpDownKey();
			break;
		}
	case PM_KEYDOWN:
		{
			m_nCurrentSubWndID = Mesg.iData;
			if(ID_SupFunctionMenu_Button_CA < m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID--;
			}
			else
			{
				m_nCurrentSubWndID = (int)ID_SupFunctionMenu_Button_EPG;
				Presentation()->MoveFocusTree(m_pSupFunctionMenuButtonEPG);
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
void SuperFuction::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();

    // Insert custom drawing code here:

    EndDraw();
}

void SuperFuction::SetToolTipWnd(ToolTip * pToolTip)
{
	m_pToolTip = pToolTip;
}

void SuperFuction::SetTVHelpWnd(TVHelpWnd * pTVHelpWnd)
{
	m_pTVHelpWnd = pTVHelpWnd;
}

void SuperFuction::ProcessUpDownKey()
{
  
	switch(m_nCurrentSubWndID)
	{
	case ID_SupFunctionMenu_Button_CA:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_SuperFuctionMenu_Prompt_CA), NEWMENU_BUTTON_TYPE);
			m_pSupFunctionMenuButtonCA->SetToolTip(m_pToolTip);
			m_pSupFunctionMenuButtonCA->ShowTip();
			break;
		}
	case ID_SupFunctionMenu_Button_CC:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_SuperFuctionMenu_Prompt_CC), NEWMENU_BUTTON_TYPE);
			m_pSupFunctionMenuButtonCC->SetToolTip(m_pToolTip);
			m_pSupFunctionMenuButtonCC->ShowTip();
			break;
		}
	case ID_SupFunctionMenu_Button_ParentControl:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_SuperFunctionMenu_Prompt_Parent), NEWMENU_BUTTON_TYPE);
			m_pSupFunctionMenuButtonParentControl->SetToolTip(m_pToolTip);
			m_pSupFunctionMenuButtonParentControl->ShowTip();
			break;
		}
	case ID_SupFunctionMenu_Button_EPG:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_SuperFuctionMenu_Prompt_EPG), NEWMENU_BUTTON_TYPE);
			m_pSupFunctionMenuButtonEPG->SetToolTip(m_pToolTip);
			m_pSupFunctionMenuButtonEPG->ShowTip();
			break;
		}

	default:
		{
			//assert(FALSE);
		}
	}

	Presentation()->Draw();
}
