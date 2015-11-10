

#include "celth_stddef.h"

#include "celth_rtos.h"

#include "celth_mutex.h"
#include "celth_message.h"

#include "celth_symbol.h"


#include "celth_debug.h"

#include "gbemg_stddef.h"

#include "gbemg_gendef.h"

#include "gbemg_type.h"
#include "gbemg_index.h"
#include "gbemg_content.h"


#include "gbemg_control.h"







static CELTH_MessageQueue_t* pCaptureIndxMsgQueue;
static CELTH_MessageQueue_t* pPlayMsgQueue;



static CELTHOSL_TaskId_t controlhandlerTaskId;


static GBEMG_BoxBasic_Info GlobalBoxInfo;





void gbemg_contrlhandler_fn(void* content)
{
	
	int iMessageQCapability;
	GBEMG_Msg_t  msg;


	CELTH_MessageQueue_t* pMqueue;

CelthApi_Debug_EnterFunction("gbemg_contrlhandler_fn");


	
	if(CELTH_Symbol_Enquire_Value("gb_capindx_msgbox",&pMqueue))
	{

CelthApi_Debug_LeaveFunction("gbemg_contrlhandler_fn");
		return ;
	}

	while(1)
	{
		
		CELTH_MsgWait(pMqueue,(void*)&msg,0xFFFFFFFF);

		switch(msg.msg_type)
		{
			case GBEMG_MSG_eCaptureIndxVersionChanged:
			{
				ResetPlayIndxHandler();
				ResetPlayRecList();
				break;
			}
			default:
				break;
		}
	}



}



U8 Start_GBEMG_ControlHandler(void* content)
{
CelthApi_Debug_EnterFunction("Start_GBEMG_ControlHandler");	



    controlhandlerTaskId = CELTHOSL_TaskCreate(NULL,gbemg_contrlhandler_fn,NULL,1024,13,0,0);

   

	CelthApi_Debug_LeaveFunction("Start_GBEMG_ControlHandler");	

}






U8 GBEMG_Init(void* content)
{

	CelthApi_Debug_EnterFunction("GBEMG_Init");
	

	pCaptureIndxMsgQueue  = CELTH_MsgCreate(sizeof(GBEMG_Msg_t),CONTRLMSGNUM);
	
	CelthApi_Debug_Msg("Create the pCaptureIndxMsgQueue=0x%08x\n",pCaptureIndxMsgQueue);

	
	pPlayMsgQueue  = CELTH_MsgCreate(sizeof(GBEMG_Msg_t),CONTRLMSGNUM);

	CelthApi_Debug_Msg("Create the pPlayMsgQueue=0x%08x\n",pPlayMsgQueue);



	
	CelthApi_Debug_Msg("register the playMsgQueue\n");
	
	CELTH_Symbol_Register("gb_play_msgbox",pPlayMsgQueue);

	CelthApi_Debug_Msg("register the captureMsgQueue\n");	
	
	

	CELTH_Symbol_Register("gb_capindx_msgbox",pCaptureIndxMsgQueue);
/*	CELTH_Symbol_Register("gb_setupindxhandler_msgbox",pSetupIndxHandlerMsgQueue);
	CELTH_Symbol_Register("gb_actionhandler_msgbox",pActionHandlerMsgQueue);
	CELTH_Symbol_Register("gb_buildconthandler_msgbox",pBuildContHandlerMsgQueue);

*/	

	CelthApi_Debug_Msg("Initialize the CaseIndx Table List!\n");
	InitializeCaseIndexTable();



	CELTHOSL_Memset(&GlobalBoxInfo,0x00,sizeof(GBEMG_BoxBasic_Info));


	ReadTheBoxBasicInfo(&GlobalBoxInfo);

	

	


	CelthApi_Debug_LeaveFunction("GBEMG_Init");

	
}


void* Start_GBEMG_MonitorIndxTable(void)
{
	return (void*)gbemg_begin_monitorindextable();
}


U8 GBEMG_Start(void* content)
{

	GBEMG_Time_t tm;

	

	CelthApi_Debug_EnterFunction("GBEMG_Start");


tm.year=2015;
tm.month=10;
tm.day=10;
tm.weekday=4;

tm.hour=12;
tm.minute=58;
tm.second=0;
/*
CelthApi_Debug_Msg("Start Gb Time!\n");

CelthApi_Debug_Msg("Start GB time with y:%d!\n",tm.year);
CelthApi_Debug_Msg("Start GB time with mm:%d!\n",tm.month);
CelthApi_Debug_Msg("Start GB time with d:%d!\n",tm.day);
CelthApi_Debug_Msg("Start GB time with h:%d!\n",tm.hour);

CelthApi_Debug_Msg("Start GB time with m:%d!\n",tm.minute);

CelthApi_Debug_Msg("Start GB time with s:%d!\n",tm.second);

CelthApi_Debug_Msg("Start GB time with m:%d,s:%d!\n",tm.minute,tm.second);

CelthApi_Debug_Msg("StartGbTime with time addr=0x%x\n",&tm);
*/
StartGbTime(&tm);


/*CelthApi_Debug_Msg("Start the Monitor GBEMGIndxTableHandle!\n");*/


Start_GBEMG_MonitorGBEMGIndxTableHandler(content);


Start_GBEMG_MonitorBoxCtrlInfoSectionHandler(content);

/*CelthApi_Debug_Msg("Start the Control Handle!\n");*/

Start_GBEMG_ControlHandler(NULL);


/*CelthApi_Debug_Msg("Start the Timer Process!\n");*/

Start_GBEMG_EmrgTimerProcess(content);

/*	CelthApi_Debug_Msg("Reset the PlayRecList!\n");*/
	ResetPlayRecList();

/*Start_GBEMG_PlayControlHandler(NULL);
Start_GBEMG_StopPlayControlHandler(NULL);  
*/

Start_GBEMG_AllPlayProcess(NULL);


CelthApi_Debug_LeaveFunction("GBEMG_Start");


}


GBEMG_Err ResetPlayIndxHandler()
{

	 Stop_GBEMG_PlayControlHandler(NULL);	   
	 Stop_GBEMG_StopPlayControlHandler(NULL); 

	
	 Start_GBEMG_PlayControlHandler(NULL);	   
	 Start_GBEMG_StopPlayControlHandler(NULL);  
															
															


}




GBEMG_Err ReadTheBoxBasicInfo(GBEMG_BoxBasic_Info* BoxInfo)
{
	if(BoxInfo)
		{
		
	return  GBEMG_No_Err;
		}
	return GBEMG_Err_InvalidHandle;
}


GBEMG_Err  GetTheBoxCoverAreaCode(U8* areacode)
{

	
	if(areacode)
		{
		
	return  GBEMG_No_Err;
		}
	return GBEMG_Err_InvalidHandle;
}






