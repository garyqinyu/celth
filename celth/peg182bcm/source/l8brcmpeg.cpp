
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "peg.hpp"
#include "pegbrcm.hpp"



#include "l8brcmscrn.hpp"
#include "customerpalette.hpp"
#include "pal256.hpp"
#include "celth_stddef.h"
#include "celth_graphic.h"

#include "celth_rtos.h"
#include "celth_mutex.h"
#include "celth_message.h"

#include "celth_symbol.h"




static UCHAR gbCurrentLanguage;
static UCHAR gbCurrentLanguageA;
extern PegFont Pronto_12;
extern PegFont chinesefont;
extern PegFont korean;
/*extern UCHAR PegCustomPalette[];*/



static char* PEG_MsgQueueName="peg_process_msgbox";

static CELTH_MessageQueue_t* pPegMsgQueue;



#if 1
extern void PegAppInitialize(PegPresentationManager *);
#endif

PegPresentationManager *pPresentation;



#if 0

void PegAppInitialize(PegPresentationManager* pPresentation)
{  

	PegThing *SomeThing = NULL;

				gbCurrentLanguage = 1;/* set  language to Chinese */
		
				/*gbCurrentLanguage = 0;*//* set default language to English */
				
    	pPresentation->Screen()->SetPalette(0, 255, PegCustomPalette); /* use custom palette */
	pPresentation->SetColor(PCI_NTEXT, 1);
	pPresentation->SetColor(PCI_NORMAL, 0xFF);
	
    	switch(gbCurrentLanguage)
    	{
		case 0:
			    	PegTextThing::SetDefaultFont(PEG_TBUTTON_FONT, &Pronto_12);
			   	PegTextThing::SetDefaultFont(PEG_PROMPT_FONT, &Pronto_12);   
			   	
			break;
		case 1:
				PegTextThing::SetDefaultFont(PEG_TBUTTON_FONT, &chinesefont);
			   	PegTextThing::SetDefaultFont(PEG_PROMPT_FONT, &chinesefont);   			
			break;
    	}
	

}

#endif

void* PegMain(void* pparam)
{	
			PegRect Rect;
			
			
			CELTHPEG_InitParams_t pegCelInitParam;
			printf("peg003\n");
		/*	pparam=NULL;*/
			printf("peg002\n");
			pegCelInitParam.xSize = 720;
			pegCelInitParam.ySize = 576;
			pegCelInitParam.Celth_OsdMemoryPitch=CelthApi_Graphic_GetOsdPitch(0);
			pegCelInitParam.Celth_OsdMemoryAddr = CelthApi_Graphic_GetOsdAddress(0);	
	
		// create the screen interface object:
		printf("peg001 memory addr =0x[%08x]\n",pegCelInitParam.Celth_OsdMemoryAddr);
		Rect.Set(0, 0, pegCelInitParam.xSize - 1, pegCelInitParam.ySize - 1);
		printf("peg000\n");
		PegScreen *pScreen = new L8BrcmScreen(Rect, pegCelInitParam);
		printf("peg111\n");
		PegThing::SetScreenPtr(pScreen);
		printf("peg222\n");
			// create the PEG message Queue:
			PegMessageQueue *pMsgQueue = new PegMessageQueue();
			PegThing::SetMessageQueuePtr(pMsgQueue);
		printf("peg333\n");
			// create the screen manager:
			// Rect has been initialized in SVPEG_Init
			pPresentation = new PegPresentationManager(Rect);
			PegThing::SetPresentationManagerPtr(pPresentation);
		printf("peg444\n");
	   #ifdef PEG_FULL_CLIPPING
		printf("peg555\n");
			  pScreen->GenerateViewportList(pPresentation);
		#endif
	
		#ifdef RECORD_AUTO_SCRIPT
				pOutFile = fopen("pegas.txt", "w");
		#endif  // RECORD_AUTO_SCRIPT
	
			printf("peg666\n");
			PegAppInitialize(pPresentation);
	
		// re-load the screen, PresentationManager, and MessageQueue pointers
		// in case the application changed them (WindowBuilder does):
	
			PegThing *pTest = NULL;
			printf("peg777\n");
			pPresentation = pTest->Presentation();
			pScreen = pTest->Screen();
			printf("peg888\n");
			pPresentation->Invalidate(pPresentation->mReal);
			printf("peg999\n");
		/*	pScreen->SetPointerType(PPT_NORMAL);*/
			printf("peg101010\n");
			while (1)
			{
				printf("loop in execute\n");
				pPresentation->Execute();		 // do message processing
			}
	
			pPresentation = pTest->Presentation();
			pScreen = pTest->Screen();
			pMsgQueue = pTest->MessageQueue();
	
			delete pPresentation;
			delete pMsgQueue;
			delete pScreen;
	
		/*	if (gpPegCmdLine)
			{
		  delete gpPegCmdLine;
			}*/
		
	#ifdef RECORD_AUTO_SCRIPT
			if (pOutFile)
			{
		  fclose(pOutFile);
	  }
#endif

}


BRCMPEG_ErrorCode_t BrcmPeg_TimerTick(HDL_Handle_t CnxHandle, const int iTimerRef) /* function to be called for timer update. to be called each 200ms */
{

	BRCMPEG_ChassisMessage_t		Msg;


	
	CELTH_MessageQueue_t* pMsgQueue;






	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}



		Msg.tMessageType = BRCMPEG_kSystem;
		Msg.tMessageParam.tSystem.SystemMessage = BRCMPEG_kTimerTick;
		Msg.tMessageParam.tSystem.pChildObject = NULL;
		CELTH_MsgSend (pMsgQueue, &Msg);



	return BRCMPEG_kNoError;
}
BRCMPEG_ErrorCode_t BrcmPeg_Open(HDL_Handle_t* pCnxHandle) /* open a connection to peg */
{
	return BRCMPEG_kNoError;
}
BRCMPEG_ErrorCode_t BrcmPeg_Close( HDL_Handle_t CnxHandle ) /* close connection to peg */
{
	return BRCMPEG_kNoError;
}
BRCMPEG_ErrorCode_t BrcmPeg_Terminate(void) /* terminate peg module */
{
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kSystem;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kExit;
	
	Msg.tMessageParam.tSystem.pChildObject = NULL;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;
}
BRCMPEG_ErrorCode_t BrcmPeg_KeyUp(HDL_Handle_t CnxHandle) /* Key up pressed on user interface */
{



	
	BRCMPEG_ChassisMessage_t		Msg;


	
	CELTH_MessageQueue_t* pMsgQueue;






	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kRcKeyPressed;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kKeyUp;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;
}
BRCMPEG_ErrorCode_t BrcmPeg_KeyDown(HDL_Handle_t CnxHandle)/* Key down pressed on user interface */
{

	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kRcKeyPressed;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kKeyDown;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;



}
BRCMPEG_ErrorCode_t BrcmPeg_KeyLeft(HDL_Handle_t CnxHandle)/* Key left pressed on user interface */
{
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kRcKeyPressed;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kKeyLeft;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;

}
BRCMPEG_ErrorCode_t BrcmPeg_KeyRight(HDL_Handle_t CnxHandle)/* Key right pressed on user interface */
{
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kRcKeyPressed;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kKeyRight;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;
}
BRCMPEG_ErrorCode_t BrcmPeg_KeySelect(HDL_Handle_t CnxHandle)/* OK Key pressed on user interface */
{
	
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kRcKeyPressed;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kKeySelect;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;
	
}
BRCMPEG_ErrorCode_t BrcmPeg_KeyExit(HDL_Handle_t CnxHandle)/* go to the menu object */
{
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kRcKeyPressed;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kMenu;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;

}
BRCMPEG_ErrorCode_t BrcmPeg_CloseWindow(HDL_Handle_t CnxHandle)/* close current window */
{
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kSystem;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kCloseWindow;
	Msg.tMessageParam.tSystem.pChildObject = NULL;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;

}
BRCMPEG_ErrorCode_t BrcmPeg_AddWindow(HDL_Handle_t CnxHandle, void *pChild) /* add a new window on the screen */
{
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kSystem;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kAddWindow;
	Msg.tMessageParam.tSystem.pChildObject = pChild;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;
}
BRCMPEG_ErrorCode_t BrcmPeg_KeyCharValue(HDL_Handle_t CnxHandle, unsigned char ucKeyValue) /* transfer this key ascii code to the current object */
{
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kRcKeyPressed;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kKeyCharValue;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;
}
BRCMPEG_ErrorCode_t BrcmPeg_KeyValueInc(HDL_Handle_t CnxHandle) /* increase value contained in the current object */
{
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kRcKeyPressed;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kKeyValueInc;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;
}
BRCMPEG_ErrorCode_t BrcmPeg_KeyValueDec(HDL_Handle_t CnxHandle) /* decrease value contained in the current object */
{
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kRcKeyPressed;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kKeyValueDec;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;
}
BRCMPEG_ErrorCode_t BrcmPeg_KeyMenu(HDL_Handle_t CnxHandle) /* goto key menu */
{
	BRCMPEG_ChassisMessage_t		Msg;

	CELTH_MessageQueue_t* pMsgQueue;

	if(CELTH_Symbol_Enquire_Value("peg_process_msgbox",(void**)&pMsgQueue))
	{

		return BRCMPEG_kInvalidHandle ;
	}

		/* test if component is initiated */
	Msg.tMessageType = BRCMPEG_kRcKeyPressed;
	Msg.tMessageParam.tRcParam.iKeyPressed = BRCMPEG_kMenu;
	CELTH_MsgSend ( pMsgQueue, &Msg);
	return BRCMPEG_kNoError;
}












int BrcmPeg_Init()
{
/*	CELTHOSL_TaskId_t pPegTaskId;
	
	pPegTaskId=CELTHOSL_TaskCreate(NULL,PegMain,NULL,1024,15,1024,8);
*/
pthread_t pid;

pthread_attr_t attr;

pthread_attr_init(&attr);




 pthread_attr_setstacksize(&attr, 1024*16);

 pPegMsgQueue = CELTH_MsgCreate(sizeof(PegMessage),20);

 
 CELTH_Symbol_Register(PEG_MsgQueueName,pPegMsgQueue);


printf("call peg main\n");
pthread_create(&pid,NULL,PegMain,NULL);
printf("call peg main finish\n");

return 0;

}



