/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: TvSelectWindowClass
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
#include "tvselectwi.hpp"
#include "usertype.hpp"
#include "soundmenu.hpp"
#include "superfunction.hpp"
#include "supsetmenu.hpp"
#include "videomenu.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

TvSelectWindowClass::TvSelectWindowClass(SIGNED iLeft, SIGNED iTop, SIGNED iRight, \
										 APCTRL_LinkId_t eEventToNotify, APCTRL_PegState_t eSubStateToNotify, \
										 HorizontalScrollText_t strings, int InitStringNumber, int menuID) : 
    PegWindow()
{
    PegRect ChildRect;
	int iIndex = 0;

    mReal.Set(iLeft, iTop, iLeft + iRight, iTop + 50);
    InitClient();
    SetColor(PCI_NORMAL, LIGHTGRAY);

    StringToDisplay = strings;
	parentMenu = menuID;
	if(InitStringNumber < StringToDisplay.numberofString)
	{
		CurrentStringNumber = InitStringNumber;
	}
	else
	{
		CurrentStringNumber = 0;
	}

	ChildRect.Set(iLeft, iTop, iLeft + iRight, iTop + 50);
    m_pSelectWindowTextButton = new PegTextButton(ChildRect, StringToDisplay.StringToDisplay[CurrentStringNumber], ID_SelectWindow_TextButton, FF_NONE|AF_ENABLED|TJ_CENTER|TT_COPY);
    Add(m_pSelectWindowTextButton);

    /* WB End Construction */
	eEvent = eEventToNotify;
    eSubState = eSubStateToNotify;
	Id(TVSELECTWINDOW_ID);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED TvSelectWindowClass::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
	case PM_KEY:
		if (Mesg.iData == PK_LEFT)
        { /* catch Key Left and send it to parent for handling */
			if(CurrentStringNumber<=0 )
			{
				CurrentStringNumber = StringToDisplay.numberofString -1;
			}
			else
			{
				CurrentStringNumber --;
			}
			m_pSelectWindowTextButton->DataSet(StringToDisplay.StringToDisplay[CurrentStringNumber]);
			m_pSelectWindowTextButton->Invalidate();
			Draw();
			ProcessLeftandRightPress(CurrentStringNumber);
			break;
		}
		else if (Mesg.iData == PK_RIGHT)
        {/* catch Key Right and send it to parent for handling */
			CurrentStringNumber++;
			if(CurrentStringNumber>=StringToDisplay.numberofString )
			{
				CurrentStringNumber = 0;
			}
			m_pSelectWindowTextButton->DataSet(StringToDisplay.StringToDisplay[CurrentStringNumber]);
			m_pSelectWindowTextButton->Invalidate();
			Draw();
			ProcessLeftandRightPress(CurrentStringNumber);
			break;
		}
		else
		{
			/*function realize for up/down and exit*/
			if ((Mesg.iData == PK_LNUP)||(Mesg.iData == PK_LNDN))
			{
				ProcessUpandDownPress(CurrentStringNumber);
			}
			else
			{
				// functions for exit
			}
			Destroy(this);
			BeginDraw();
			switch(parentMenu)
			{
				case SOUND_MENU_ID:
				{
					SoundMenu *SomeThing = (SoundMenu *)Presentation()->Find(parentMenu);
					SomeThing->ProcessUpDownKey();
				}
				break;
				case SUPERFUNCTION_MENU_ID:
				{
					SuperFuction *SomeThing = (SuperFuction *)Presentation()->Find(parentMenu);
					SomeThing->ProcessUpDownKey();
				}
				break;
				case SUPERSETTING_MENU_ID:
				{
					SupSettingMenu *SomeThing = (SupSettingMenu *)Presentation()->Find(parentMenu);
					SomeThing->ProcessUpDownKey();
				}
				break;
				case VIDEO_MENU_ID:
				{
					VideoMenu *SomeThing = (VideoMenu *)Presentation()->Find(parentMenu);
					SomeThing->ProcessUpDownKey();
				}
				break;

				break;
			default:
				break;

			}
			EndDraw();

		} /*end of else*/
		break;
	default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void TvSelectWindowClass::Draw(void)
{
    BeginDraw();
    PegWindow::Draw();

    // Insert custom drawing code here:

    EndDraw();
}

int TvSelectWindowClass::GetCurrentStringNumber(void)
{
	return (CurrentStringNumber);
}

void TvSelectWindowClass::ProcessUpandDownPress(int Number)
{

}

void TvSelectWindowClass::ProcessLeftandRightPress(int Number)
{

}