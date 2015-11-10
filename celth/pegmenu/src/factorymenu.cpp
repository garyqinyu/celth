/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: FactoryMenu
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
#include "factorymenu.hpp"

#include "tvbitmapbutton.hpp"
#include "usertype.hpp"
#include "mainmenu.hpp"
#include "helpwnd.hpp"
#include "tooltip.hpp"
#include "videomenu.hpp"

#include "anim.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
FactoryMenu::FactoryMenu(SIGNED iLeft, SIGNED iTop) : 
    PegWindow(FF_NONE)
{
    PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 49, iTop + 149);
    InitClient();
    AddStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 0, iTop + 100, iLeft + 49, iTop + 149);
    m_pFactoryMenuButtonGeometry = new TVBitmapButton(ChildRect, &gbgeometryBitmap, ID_FactoryMenu_Button_Geometry);
    Add(m_pFactoryMenuButtonGeometry);

    ChildRect.Set(iLeft + 0, iTop + 50, iLeft + 49, iTop + 99);
    m_pFactoryEMenuButtonWhiteBalance = new TVBitmapButton(ChildRect, &gbwhitebalanceBitmap, ID_FactoryMenu_Button_WhiteBalance);
    Add(m_pFactoryEMenuButtonWhiteBalance);

    ChildRect.Set(iLeft + 0, iTop + 0, iLeft + 49, iTop + 49);
    m_pFactoryMenuButtonVideo = new TVBitmapButton(ChildRect, &gbtransparanceBitmap, ID_FactoryMenu_Button_Video);
    Add(m_pFactoryMenuButtonVideo);

    /* WB End Construction */
	m_pFactoryMenuButtonGeometry->SetTipString(LS(SID_FactoryMenu_Prompt_Geometry));
	m_pFactoryEMenuButtonWhiteBalance->SetTipString(LS(SID_FactoryMenu_WhiteBalance));
	m_pFactoryMenuButtonVideo->SetTipString(LS(SID_FactoryMenu_Prompt_FactroySelect));
	
	m_pToolTip = NULL;
	m_nCurrentSubWndID = ID_FactoryMenu_Button_Video;
	Id(FACTORY_MENU_ID);

}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED FactoryMenu::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(ID_FactoryMenu_Button_Geometry, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_FactoryMenu_Button_WhiteBalance, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(ID_FactoryMenu_Button_Video, PSF_CLICKED):
        // Enter your code here:
        break;

	case PM_KEYUP:
		{			
			m_nCurrentSubWndID = Mesg.iData;
			if(ID_FactoryMenu_Button_Video > m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID++;
			}	
			else
			{
				m_nCurrentSubWndID =  (int)ID_FactoryMenu_Button_Geometry;
				Presentation()->MoveFocusTree(m_pFactoryMenuButtonGeometry);
			}
			ProcessUpDownKey();
			break;
		}
	case PM_KEYDOWN:
		{
			m_nCurrentSubWndID = Mesg.iData;
			if(ID_FactoryMenu_Button_Geometry < m_nCurrentSubWndID)
			{
				m_nCurrentSubWndID--;
			}	
			else
			{
				m_nCurrentSubWndID = (int)ID_FactoryMenu_Button_Video;
				Presentation()->MoveFocusTree(m_pFactoryMenuButtonVideo);
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
			DisplayChildWindow(Mesg.iData);
			break;
		}


    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void FactoryMenu::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();

    // Insert custom drawing code here:

    EndDraw();
}

void FactoryMenu::SetToolTipWnd(ToolTip * pToolTip)
{
	m_pToolTip = pToolTip;
}

void FactoryMenu::SetTVHelpWnd(TVHelpWnd * pTVHelpWnd)
{
	m_pTVHelpWnd = pTVHelpWnd;
}

void FactoryMenu::ProcessUpDownKey()
{
	switch(m_nCurrentSubWndID)
	{
   	case ID_FactoryMenu_Button_Geometry:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_FactoryMenu_Prompt_Geometry), NEWMENU_BUTTON_TYPE);
			m_pFactoryMenuButtonGeometry->SetToolTip(m_pToolTip);
			m_pFactoryMenuButtonGeometry->ShowTip();
			break;
		}
	case ID_FactoryMenu_Button_WhiteBalance:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_FactoryMenu_WhiteBalance), NEWMENU_BUTTON_TYPE);
			m_pFactoryEMenuButtonWhiteBalance->SetToolTip(m_pToolTip);
			m_pFactoryEMenuButtonWhiteBalance->ShowTip();
			break;
		}
	case ID_FactoryMenu_Button_Video:
		{
			m_pTVHelpWnd->ShowHelpString(LS(SID_FactoryMenu_Prompt_FactroySelect), NEWMENU_BUTTON_TYPE);
			m_pFactoryMenuButtonVideo->SetToolTip(m_pToolTip);
			m_pFactoryMenuButtonVideo->ShowTip();
			break;
		}

	default:
		{
			//assert(FALSE);
		}
	}

	Presentation()->Draw();
}

void FactoryMenu::DisplayChildWindow(int iSubWndID)
{
	if(ID_FactoryMenu_Button_Geometry> iSubWndID || ID_FactoryMenu_Button_Video < iSubWndID)
	{
		return;
	}

	m_nCurrentSubWndID = iSubWndID;

	SIGNED nTipLeftTopY = mReal.wTop + (ID_FactoryMenu_Button_Video - m_nCurrentSubWndID)  * 50;
	VideoMenu * pVideoMenu = NULL;
	
	switch(iSubWndID)
	{
		case ID_FactoryMenu_Button_Video:
		{
			pVideoMenu = new VideoMenu(mReal.wRight, nTipLeftTopY);
			Presentation()->Add(pVideoMenu);
			pVideoMenu->SetTVHelpWnd(m_pTVHelpWnd);
			pVideoMenu->SetToolTipWnd(m_pToolTip);
			pVideoMenu->ProcessUpDownKey();
			break;
		}
		case ID_FactoryMenu_Button_WhiteBalance:
		{
			break;
		}
		case ID_FactoryMenu_Button_Geometry:
		{
			break;
		}

	default:
		{
			break;
		}
	}
}
