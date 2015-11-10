/*
#include "assert.h"
#include "osal.h"
#include "handle.h"*/
#include "peg.hpp"
#include "pegidle.hpp"
/*#include "sv_peg.h"*/
#include "usertype.hpp"
#include "mainmenu.hpp"
#include "mainmenu1.hpp"
#include "l8brcmscrn.hpp"

#define PEGSCREEN L8BrcmScreen

/*--------------------------------------------------------------------------*/
// PegIdleFunction- 
//
// This function gets called by PegPresentationManager when there is nothing
// left for PEG to do. For running with Windows, we return to the Windows
// message loop until a new Windows message arrives.
/*--------------------------------------------------------------------------*/
void PegIdleFunction(void)
{
#if 0

	PegThing *SomeThing = NULL;
	PegThing *SomeThing1 = NULL;
	PegThing *SomeThing2 = NULL;
	PegThing *SomeThing3 = NULL;
	PegThing *SomeThing4 = NULL;
	PegThing *SomeThing5 = NULL;
	PegThing *SomeThing6 = NULL;
	PegThing *SomeThing7 = NULL;
	PegThing *SomeThing8 = NULL;
	PegThing *SomeThing9 = NULL;	
	PegThing *SomeThing10 = NULL;
	PegThing *SomeThing11 = NULL;

	PegMessage NewMessage;
	PegMessage SecondMessage;
	OSAL_Status_t status;
	void *pvMsg = NULL;
	ap_mmi_ChassisMessage_t *pvMsgChassis;
	int tMes[sizeof(ap_mmi_ChassisMessage_t)];
    
	PegScreen *pScreen = PegThing::Screen();

	pvMsg= tMes;

	status = OSAL_MsgWait(pvMsg, OSAL_kWaitForEver); /* wait for message */
    if (status == OSAL_kNoError)
    {
		pvMsgChassis = (ap_mmi_ChassisMessage_t *)pvMsg;

		if (pvMsgChassis->tMessageType == ap_mmi_kRcKeyPressed)
		{
			switch(pvMsgChassis->tMessageParam.tRcParam.iKeyPressed)
			{
				case ap_mmi_kKeyUp:
				case ap_mmi_kKeyDown:
				case ap_mmi_kKeyRight:
				case ap_mmi_kKeyLeft:
				{
					if (pScreen)
					{
						NewMessage.pTarget = PegThing::Presentation()->GetCurrentThing();
						NewMessage.wType = PM_KEY;
						switch (pvMsgChassis->tMessageParam.tRcParam.iKeyPressed)
						{
							case ap_mmi_kKeyUp:
								NewMessage.iData = PK_LNUP;
								break;
							case ap_mmi_kKeyDown:
								NewMessage.iData = PK_LNDN;
								break;
							case ap_mmi_kKeyRight:
								NewMessage.iData = PK_RIGHT;
								break;
							case ap_mmi_kKeyLeft:
								NewMessage.iData = PK_LEFT;
								break;
						}
						NewMessage.lData = 0;
						SomeThing->MessageQueue()->Push(NewMessage);
					}
				}
				break;

				case ap_mmi_kKeySelect:
				{
					if (pScreen)
					{
						NewMessage.pTarget = PegThing::Presentation()->GetCurrentThing();
						NewMessage.wType = PM_KEY;
						NewMessage.iData = PK_CR;
						NewMessage.lData = KF_SHIFT;
						SomeThing->MessageQueue()->Push(NewMessage);

						SecondMessage.pTarget = PegThing::Presentation()->GetCurrentThing();
						SecondMessage.wType = PM_KEY_RELEASE;
						SecondMessage.iData = PK_CR;
						SecondMessage.lData = 0;
						SomeThing->MessageQueue()->Push(SecondMessage);
					}
				}
				break;

				case ap_mmi_kKeyCharValue:
				{
					if (pScreen)
					{
						NewMessage.pTarget = PegThing::Presentation()->GetCurrentThing();
						NewMessage.wType = PM_KEY;
						NewMessage.iData = pvMsgChassis->tMessageParam.tRcParam.ucKeyValueParam;
						NewMessage.lData = 0;
						SomeThing->MessageQueue()->Push(NewMessage);
					}
				}
				break;

				case ap_mmi_kKeyValueInc:
				{
					if (pScreen)
					{
						NewMessage.pTarget = PegThing::Presentation()->GetCurrentThing();
						NewMessage.wType = PM_KEY;
						NewMessage.iData = KEY_VALUE_INCREASE;
						NewMessage.lData = 0;
						SomeThing->MessageQueue()->Push(NewMessage);
					}
				}
				break;

				case ap_mmi_kKeyValueDec:
				{
					if (pScreen)
					{
						NewMessage.pTarget = PegThing::Presentation()->GetCurrentThing();
						NewMessage.wType = PM_KEY;
						NewMessage.iData = KEY_VALUE_DECREASE;
						NewMessage.lData = 0;
						SomeThing->MessageQueue()->Push(NewMessage);
					}
				}
				break;

				case ap_mmi_kMainMenu:
				{
					SomeThing = PegThing::Presentation()->Find(MAIN_MENU_ID);
					SomeThing1 = PegThing::Presentation()->Find(HELP_MENU_ID);
					SomeThing2 = PegThing::Presentation()->Find(PICTURE_MENU_ID);
					SomeThing3 = PegThing::Presentation()->Find(SERVICE_MENU_ID);
					SomeThing4 = PegThing::Presentation()->Find(INSTALL_MENU_ID);
					SomeThing5 = PegThing::Presentation()->Find(PROGRESSWINDOW_ID);
					SomeThing6 = PegThing::Presentation()->Find(TUNING_MENU_ID);
					SomeThing7 = PegThing::Presentation()->Find(MAIN_AUDIO_MENU_ID);


					if (SomeThing1 || SomeThing2 || SomeThing3 || SomeThing4 || 
						SomeThing5 || SomeThing6 || SomeThing7 )
					{ /* a child is opened so send PM_CLOSE message */
						NewMessage.pTarget = SomeThing->Presentation();
						NewMessage.wType = PM_CLOSE;
						NewMessage.iData = 0;
						NewMessage.lData = 0;
						SomeThing->MessageQueue()->Push(NewMessage);
					}
					else
					{ /* no child only main menu or nothing */
						if (SomeThing)
						{ /* kill main menu */
							PegThing::Presentation()->Destroy(SomeThing);
						}
						else
						{ /* display main menu */
							PegThing::Presentation()->Add(new MainMenuClass(200, 100));
							/*PegThing::Presentation()->Add(new PEGMainMenu1(200, 100));*/
						}
					}
				}
				break;
			}
			
		}
		else if(pvMsgChassis->tMessageType == ap_mmi_kSystem)
		{/* this is a system message*/
			switch(pvMsgChassis->tMessageParam.tSystem.SystemMessage)
			{
				case ap_mmi_kExit:
				{
					PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
					
					if (pWin)
					{
						NewMessage.pTarget = SomeThing->Presentation();
						NewMessage.wType = PM_EXIT;
						NewMessage.iData = 0;
						NewMessage.lData = 0;
						SomeThing->MessageQueue()->Push(NewMessage);
					}
				}
				break;
			
				case ap_mmi_kCloseWindow:
				{
					PEGSCREEN *pWin = (PEGSCREEN *) SomeThing->Screen();
					
					if (pWin)
					{
						NewMessage.pTarget = SomeThing->Presentation();
						NewMessage.wType = PM_CLOSE;
						NewMessage.iData = 0;
						NewMessage.lData = 0;
						SomeThing->MessageQueue()->Push(NewMessage);
					}
				}
				break;
				
				case ap_mmi_kTimerTick:
				{
					PegThing *SomeThing = NULL;
					SomeThing->MessageQueue()->TimerTick();
				}
				break;
			} // end switch
		} // end else if
	    else
		{/* not a key pressed nor a system message*/
		}
	}

#endif
}

