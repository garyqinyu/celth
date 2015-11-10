
#include "celth_stddef.h"


#include "celth_rtos.h"

#include "celth_event.h"


#include "celth_mutex.h"

#include "celth_symbol.h"

#include "celth_demux.h"
#include "celth_message.h"
#include "urgbcastdef.h"
#include "urgbcastcontrol.h"

#include "urgbcast.h"

#include  "ts_priv.h"
#include "ts_egbt.h"



typedef struct localmonitordata{

CELTH_EventId_t ev;

CELTHDemux_SectionFilterHandle_t hf;



}localmonitordata;





CELTH_STATIC CELTHOSL_TaskId_t egbtmonitorTaskId=NULL;


CELTH_STATIC CELTHOSL_TaskId_t egbtchangeactTaskId;


CELTH_STATIC CELTHDemux_SectionFilterHandle_t*  hEgbtFilterArray=NULL;

CELTH_STATIC CELTH_EventId_t*    hEgbtEventArray=NULL;



CELTH_STATIC CELTH_EventGroup_t  hEgbtEventGroup;

/***************************************************/
/* egbt change handle data*/
/****************************************************/

CELTH_STATIC CELTH_EventId_t  hegbtchangeEvent;


CELTH_STATIC CELTHOSL_TaskId_t egbtchangehandleTaskId=NULL;


/****************************************************/
/* local data*/
/****************************************************/

static localmonitordata lmdata;  

static CELTH_BOOL egbtmonitorterminate=0;





static CELTH_VOID egbtsection_detect(CELTH_INT iparam,CELTH_VOID* pparam)
{
	CELTH_EventId_t  event = (CELTH_EventId_t)pparam;
	printf("\n+++++++detect egbt section+++++++++++++++++\n");
	CELTH_SetEvent(event);
		
}


void*  psiegbtmonitor_fn(void* context)
{
	/*CELTH_EventId_t  event = (CELTH_EventId_t)context;*/


	CELTH_U8 sectbuf[4096];
	CELTH_U8 oldsectbuf[4096+128];
	CELTH_UINT seclength;
	CELTH_INT secleft;
	CELTH_UINT lastsectlength;
	emerge_channel_info  channelinfo;

	CELTH_Error_Code  ret;

	
	CELTH_BOOL bcompare;
/*	localmonitordata*  glmdata = (localmonitordata*)context;*/

	secleft=0;
		
	CELTHOSL_Memset(oldsectbuf,0x00,sizeof(CELTH_U8)*(4096+128));	



	while(egbtmonitorterminate)
		{
		
	/*	ret=CELTH_WaitForEvent(lmdata->ev,1000);*/
		ret=CELTH_WaitForEvent(lmdata.ev,1000);

/*		printf("\n++++ Loop ++++\n"); */
	 	if(ret==CELTHERR_ERROR_TIMEOUT)
			{
				continue;
			}


	/*	printf("\nget the egbt section\n");*/


	do{
		/*	secleft=CelthApi_Demux_GetSection(lmdata->hf,sectbuf,&seclength,4096);*/
		secleft=CelthApi_Demux_GetSection(lmdata.hf,sectbuf,&seclength,4096);
/*	printf("\n++++ Get ebgt section length=[%x]\n",seclength);*/

		if(seclength!=0)
		{
		
			
			if(!CELTHOSL_Memcmp(oldsectbuf,sectbuf,seclength))
			
	{
			if(seclength>4096)
			{
				seclength=4096;
			}
			CELTHOSL_Memcpy(oldsectbuf,sectbuf,seclength);
			
			}
			else
			{
			/* data is identified with last gotten one*/
			 continue;	
			}	
		
		

	        if(Ts_parseEGBT(sectbuf,seclength,&channelinfo))
		{

		SetNewEgbtTableInfo(&channelinfo);

		
		bcompare=ComapareNewEgbtTable2OldEgbtTable();

		if(!bcompare)
			{

			
		CELTH_SetEvent(hegbtchangeEvent);
			}
		}
		}


		CELTHOSL_Sleep(10);

		}while(secleft!=0);

		
		}
	


}



static void start_psimonitor_egbt_thread(void* context)
{

	egbtmonitorterminate = TRUE;
/*	egbtmonitorTaskId=CELTHOSL_TaskCreate(NULL,psiegbtmonitor_fn,context,1024*10,13,0,0);*/
	egbtmonitorTaskId=CELTHOSL_TaskCreate(NULL,psiegbtmonitor_fn,NULL,1024*32,13,0,0);	

}

static void stop_psimonitor_egbt_thread()
{
	if(egbtmonitorTaskId)
		{

		
		egbtmonitorterminate = FALSE;
	CELTHOSL_TaskDelete(egbtmonitorTaskId);
		}
}




CELTH_VOID emerge_Psiegbtmonitor_start()
{


	Celth_Demux_SectionFilter_s  egbtFilterSettings;

	int i;
	
	CELTH_Error_Code err;

/*	CELTH_EventId_t*    hEgbtEventArray;*/

/*	CELTH_EventGroup_t  hEgbtEventGroup;*/


	
	Celth_DemuxSectionCallback_s SectionCallbackfun;

/*printf("\n@@@@@@@@@@@ START  EGBT MONITOR@@@@@@@@@@@\n");*/

	hEgbtFilterArray = (CELTHDemux_SectionFilterHandle_t *)CELTHOSL_MemoryAlloc( sizeof(CELTHDemux_SectionFilterHandle_t) * GetPmtTableInfo()->egtable_num);

	if(hEgbtFilterArray==NULL)
	{
		return ;
	}

	CELTHOSL_Memset(hEgbtFilterArray, 0, sizeof(CELTHDemux_SectionFilterHandle_t) * GetPmtTableInfo()->egtable_num);


	hEgbtEventArray = (CELTH_EventId_t*) CELTHOSL_MemoryAlloc(sizeof(CELTH_EventId_t)*GetPmtTableInfo()->egtable_num);

	CELTH_CreateEventGroup(&hEgbtEventGroup);

	SectionCallbackfun.fDetectCallback=egbtsection_detect;


for(i=0;i<GetPmtTableInfo()->egtable_num;i++)
{

	CelthApi_Demux_AllocateSectionFilter(NULL,&hEgbtFilterArray[i]);

	err=CELTH_CreateEvent(&hEgbtEventArray[i]);
	if(err!=CELTHERR_SUCCESS)
	{
		printf("\ncreate the event error\n");
		return;
	}

	/*CELTH_AddEventGroup(hEgbtEventGroup,hEgbtEventArray[i]);*/

/*	printf("\n@@@@@@@@@@@@@@@@@Set the EGBT Filter pid [%x]\n",GetPmtTableInfo()->egtable[i].eg_pid); */

	CelthApi_Demux_SetSectionFilterPid(hEgbtFilterArray[i],GetPmtTableInfo()->egtable[i].eg_pid);		


	CelthApi_Demux_SectionSetDefaultFilterSettings(&egbtFilterSettings);
						
	egbtFilterSettings.mask[0]=0x00;

	egbtFilterSettings.coefficient[0]=0xA0;
	CelthApi_Demux_SetSectionFilter(hEgbtFilterArray[i],&egbtFilterSettings);

	CelthApi_Demux_SetSectionFilterCallback(hEgbtFilterArray[i],&SectionCallbackfun);

	CelthApi_Demux_SetSectionFilterCallbackParam(hEgbtFilterArray[i],(CELTH_VOID *)hEgbtEventArray[i]);

	/*CELTH_AddEventGroup(hEgbtEventGroup,hEgbtEventArray[i]);*/

	CelthApi_Demux_EnableSectionFilter(hEgbtFilterArray[i]);
}



/*start_psimonitor_egbt_thread(hEgbtEventGroup);*/

/* discuss with the tongfang,  because one pmt  carried only one emerge bcast descriptor, need not use the eventgroup . just use the first event */ 


lmdata.ev=hEgbtEventArray[0];
lmdata.hf = hEgbtFilterArray[0];


start_psimonitor_egbt_thread(NULL);
printf("\n###++++Finish start EGBT MONITOR Thread@@@\n");

}

CELTH_VOID  emerge_Psiegbtmonitor_stop()
{

	int i;
	
	stop_psimonitor_egbt_thread();
	if(hEgbtFilterArray)
		{
		
			for(i=0;i<GetPmtTableInfo()->egtable_num;i++)
				{
		
				CelthApi_Demux_DisableSectionFilter(hEgbtFilterArray[i]);
				CelthApi_Demux_FreeSectionFilter(hEgbtFilterArray[i]);
		
				}
		}
	if(hEgbtEventArray)
		{
			
			for(i=0;i<GetPmtTableInfo()->egtable_num;i++)
				{
				CELTH_DestroyEvent(hEgbtEventArray[i]);
				}
			
		}

	if(hEgbtEventGroup)
		{
		CELTH_DestroyEventGroup(hEgbtEventGroup);
		}

		
}



/********************************************************************************************/

/* egbt channel change handle thread*/

/********************************************************************************************/

void psiegbtchangehandle_fn(void* context)
{

	CELTH_Error_Code  ret;

	CELTH_BOOL bcompare;

	Emerge_Cmd_t  cmd;
	
	CELTH_EventId_t ev=(CELTH_EventId_t)context;


	while(1)
		{
		ret=CELTH_WaitForEvent(ev,0xFFFFFFFF);

		printf("\nIIIIIIIIIII*********Get the EGBT Event****IIIIIII\n");

	/*	if(ret==CELTHERR_ERROR_TIMEOUT)
			{
				continue;
			}*/

		
		bcompare=ComapareNewEgbtTable2OldEgbtTable();
		if(bcompare)/*equal*/
			{
			continue;
			}
		else/* not equal*/
			{

			/* send command to audio thread*/

			SetNewEgbtTableInfo2OldEgbtTableInfo();

			
			do{
			cmd =EmergeBcast_CommandAnalysis(GetOldEgbtTableInfo());

			SendBcastMsg2control(cmd);

			printf("\n****************send message msg[%x]**************\n",cmd.cmd_type);

			
				}while(cmd.cmd_type!=EMERGE_CMD_MAX);
			
			}
	
		}
}


CELTH_VOID emerge_egbtChangHandle_start()
{

		

		CELTH_CreateEvent(&hegbtchangeEvent);
		
		egbtchangehandleTaskId = CELTHOSL_TaskCreate(NULL,psiegbtchangehandle_fn,hegbtchangeEvent,1024*8,15,0,0);
}



CELTH_BOOL SendBcastMsg2control(Emerge_Cmd_t cmd)
{
	
	CELTH_MessageQueue_t* pMsgQueue;

	CELTH_INT      iMessageQCapability;

	CELTH_Status_t  eStatus;

	if(CELTH_Symbol_Enquire_Value("eb_process_msgbox",&pMsgQueue))
	{
		return TRUE;
	}

	
	eStatus =  CELTH_MsgCapabilityGet( pMsgQueue, &iMessageQCapability);

	
	if (iMessageQCapability > 4)
	{
		/* Fill the user input message and sent it to the task ------------------*/

		eStatus = CELTH_MsgSend( pMsgQueue, &cmd );

		
		printf("\n****************send message msg[%x] in msg send function**************\n",cmd.cmd_type);

		if(eStatus!=CELTH_NO_ERROR)
			{return TRUE;}
			
			
	}
	else
	{
			return TRUE;
	}

	return FALSE;
}






