

#include "celth_stddef.h"


#include "celth_rtos.h"

#include "celth_mutex.h"
#include "celth_event.h"


#include "celth_message.h"


#include "celth_demux.h"



#include "celth_debug.h"




#include "gbemg_stddef.h"
#include "gbemg_gendef.h"



#include "gbemg_type.h"
#include "gbemg_indexmacro.h"
#include "gbemg_index.h"
#include "gbemg_content.h"





#include "gbemg_emglist.h"

#include "gbemg_control.h"



static  CELTHDemux_SectionFilterHandle_t hemgindxFilter=NULL;

static  CELTH_EventId_t  emgindxEvent;

static CELTHOSL_TaskId_t emgindxsectmonitorTaskId;

static U8 emgindxsecver=0xFF;

static U8 emgindxsecbuf[4096+128];






static CELTH_VOID emgindxsection_detect(CELTH_INT iparam,CELTH_VOID* pparam)
{
	CELTH_EventId_t  event = (CELTH_EventId_t)pparam;
	CelthApi_Debug_Msg("\n++++++detectINDX ++++++++++\n");
	CELTH_SetEvent(event);
		
}



static bool SendIndxCaptureMsg2control(GBEMG_Msg_t  Msg)
{

	

	
	CELTH_MessageQueue_t* pMsgQueue;

	CELTH_INT      iMessageQCapability;

	CELTH_Status_t  eStatus;

	if(CELTH_Symbol_Enquire_Value("gb_capindx_msgbox",&pMsgQueue))
	{
		return true;
	}

	
	eStatus =  CELTH_MsgCapabilityGet( pMsgQueue, &iMessageQCapability);

	
	if (iMessageQCapability > 4)
	{
		/* Fill the user input message and sent it to the task ------------------*/

		eStatus = CELTH_MsgSend( pMsgQueue, &Msg );

		
		CelthApi_Debug_Msg("\nsend msg[%x]***********\n",Msg.msg_type);

		if(eStatus!=CELTH_NO_ERROR)
			{return true;}
			
			
	}
	else
	{
			return true;
	}

	return false;
}




static bool gbemg_emgsectmonitor_start(CELTH_EventId_t ev)
{

	CelthApi_Debug_EnterFunction("gbemg_emgsectmonitor_start");

	Celth_Demux_SectionFilter_s  emgindxFilterSettings;

	int i;

	
	
	Celth_DemuxSectionCallback_s emgindxSectionCallbackfun;

	bool bret =false;


	emgindxSectionCallbackfun.fDetectCallback=emgindxsection_detect;

	emgindxSectionCallbackfun.fErrorCallback =NULL;




	CelthApi_Demux_AllocateSectionFilter(NULL,&hemgindxFilter);

	if(hemgindxFilter)
	{

		CelthApi_Demux_SetSectionFilterPid(hemgindxFilter,GBEMG_PID);		


		CelthApi_Demux_SectionSetDefaultFilterSettings(&emgindxFilterSettings);
						
	

		
		emgindxFilterSettings.mask[0]=0x00;
		
		
		emgindxFilterSettings.coefficient[0]=GBEMG_INDX_TABLEID;
		
		CelthApi_Demux_SetSectionFilter(hemgindxFilter,&emgindxFilterSettings);

		CelthApi_Demux_SetSectionFilterCallback(hemgindxFilter,&emgindxSectionCallbackfun);

		CelthApi_Demux_SetSectionFilterCallbackParam(hemgindxFilter,(void *)ev);

	
		CelthApi_Demux_EnableSectionFilter(hemgindxFilter);

		bret=true;
	}

CelthApi_Debug_LeaveFunction("gbemg_emgsectmonitor_start");
	return bret;
}




void* emgindxsecmonitor_fn(void* arg)
{

	CELTH_EventId_t  ev;
	CELTH_Error_Code  ret;

	int emgindxsecleft,emgindxseclen;

	U8 curver;

	GBEMG_Msg_t  Msg;


	ev = (CELTH_EventId_t) arg;


CelthApi_Debug_EnterFunction("emgindxsecmonitor_fn");


	Msg.msg_type=GBEMG_MSG_eCaptureIndxVersionChanged;
	Msg.msg_packet_length=0;
	Msg.msg_packet=NULL;





#if 1

	
	if(gbemg_emgsectmonitor_start(ev))
	{



		
		/* first loop for first indx section capture, and fill the first indxtable and version num*/	
		while(1)
			{
				ret=CELTH_WaitForEvent(ev,500);

				if(ret==CELTHERR_ERROR_TIMEOUT)
				{
					continue;
				}

				emgindxsecleft=CelthApi_Demux_GetSection(hemgindxFilter,emgindxsecbuf,&emgindxseclen,4096);

				CelthApi_Debug_Msg("Get emgindexlen=%d!\n",emgindxseclen);


				emgindxsecver =  GBEMG_INDX_VERSION_NUM(emgindxsecbuf);

		CelthApi_Debug_Msg("Get indxver=%d!\n",emgindxsecver);


				ResetCaseIndexTable();


				BuildCaseIndexFromIndexTable(emgindxsecbuf,emgindxseclen);

				SendIndxCaptureMsg2control(Msg);

				
				break;

				
			}
		while(1)
			{
			
			ret=CELTH_WaitForEvent(ev,500);

			if(ret==CELTHERR_ERROR_TIMEOUT)
				{
					continue;
				}

				while(1){ 

					emgindxsecleft=CelthApi_Demux_GetSection(hemgindxFilter,emgindxsecbuf,&emgindxseclen,4096);

					

					curver =  GBEMG_INDX_VERSION_NUM(emgindxsecbuf);

					if(curver==emgindxsecver)
					{
						CelthApi_Debug_Msg("same index table,continue!&&&&&\n");
						
					}
					else
					{
						emgindxsecver=curver;

						ResetCaseIndexTable();


					
						BuildCaseIndexFromIndexTable(emgindxsecbuf,emgindxseclen);

					
					
						SendIndxCaptureMsg2control(Msg);

					

					
					}
					if(emgindxsecleft==0)
					 {
						break;
					 }			

					CELTHOSL_Sleep(10);
					}
			}


	}

#endif
	
	
	CelthApi_Debug_LeaveFunction("emgindxsecmonitor_fn");

}



/* this thread will take the following charge*/

/*

1, monitor the emg index table 

2, if there are new emg table section rebuild emglist 

3, after rebuild the emglist , send message to need 

*/




U8 Start_GBEMG_MonitorGBEMGIndxTableHandler(void* content)
{

	CelthApi_Debug_EnterFunction("Start_GBEMG_MonitorGBEMGIndxTableHandler");
	
	

	/*InitializeCaseIndexTable();*/
CELTH_CreateEvent(&emgindxEvent);


	
	emgindxsectmonitorTaskId = CELTHOSL_TaskCreate(NULL,emgindxsecmonitor_fn,(void*)emgindxEvent,1024*8,13,0,0);

  

	CelthApi_Debug_LeaveFunction("Start_GBEMG_MonitorGBEMGIndxTableHandler");

	
}



