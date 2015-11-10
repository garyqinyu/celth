

#include <stdlib.h>
#include <stdio.h>


#include<pthread.h>

/*#include "pegbrcm.hpp"*/

#include "celth_stddef.h"
#include "celth_debug.h"

#include "celth_platform.h"

#include "celth_rtos.h"

#include "celth_event.h"


#include "celth_tuner.h"

#include "celth_timer.h"

#include "celth_demux.h"
#include "celth_mutex.h"
#include "celth_message.h"

#include "tspsisearch.h"
#include "celth_avplay.h"
#include "celth_graphic.h"

/*#include "pegbrcm.hpp"*/

/*
#include "urgbcastdef.h"
#include "urgbcastcontrol.h"
#include "urgbcast.h"

*/


#include "celth_list.h"
#include "gbemg_stddef.h"

#include "gbemg_gendef.h"

#include "gbemg_type.h"
#include "gbemg_index.h"
#include "gbemg_content.h"


#include "gbemg_control.h"


CELTH_STATIC CELTH_BOOL SendSectMsg2process(CELTH_U32 msg);

CELTH_STATIC CELTH_U32 msg;


CELTH_STATIC CELTH_MessageQueue_t* pMsgQ;



typedef struct sectiontrans_param{

CELTH_EventId_t  ev;

CELTHDemux_SectionFilterHandle_t hs;






}sectiontrans_param;


extern int BrcmPeg_Init(); 
/*extern void* PegMain(void* pparam);*/

void printhere()
{
 while(1)
{
 CelthApi_Debug_Msg("print hrere++++++\n");
}

}

void tuner_status(int iparam, void* pParam)
{
	CELTH_EventId_t event=(CELTH_EventId_t)pParam;

	CELTH_SetEvent(event);
	
	
}



void section_detect(int size, void* pParam)
{
	
	CELTH_EventId_t event=(CELTH_EventId_t)pParam;

	printf("section has decected ,set demux event\n");	
	CELTH_SetEvent(event);

	msg++,

	
	

	

	
	SendSectMsg2process(msg);

	printf("send the msg with %x in the triger callback!!!!\n",msg);
	
}


void* thr_fn(void* arg)
{
GBEMG_Time_t* ptime;

 ptime=(GBEMG_Time_t*) arg;

 
 Celth_TunerStatus tstatus;


 
while(1)
{
 /* CELTH_WaitForEvent(event,0xFFFFFFFF);
  printf("wait event ok in the thread\n");

  
  CelthApi_TunerGetTunerStatus(&tstatus);*/

CelthApi_Debug_Msg("CelthApi_Debug_Msg:y:%04d,m:%02d,d:%02d,h:%02d,mm:%02d,s:%02d\n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second);

printf("printf:y:%d,m:%d,d:%d,h:%d,mm:%d,s:%d\n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second);
    
CELTHOSL_Sleep(1000);
}
  

 return NULL;
}


void* sectiondemux_fn(void* arg)
{

 int sectlength;
	
 int i;

 unsigned char sectionbuf[1024];
 CELTH_EventId_t event;

 CELTHDemux_SectionFilterHandle_t hHandle;

 sectiontrans_param* psectiontransparam;

 psectiontransparam = (sectiontrans_param*)arg;

 event=psectiontransparam->ev;

 hHandle=psectiontransparam->hs;

 
 CELTH_U32 qinlen;


 
 CELTH_MessageQueue_t* pMessage;
 
 
 
 if(CELTH_Symbol_Enquire_Value("demux_section_box",&pMessage))
 {
 	printf("could not get the message queue!!!\n");
	 return ;
 }
else
{
	printf("get the messageQ [%x]in the section thread\n",pMessage);
}
 
 

 
 while(1)
{
 /* CELTH_WaitForEvent(event,0xFFFFFFFF);*/
 	CELTH_MsgWait(pMessage,&qinlen,0xFFFFFFFF);

 	printf("get the message with %x in the handle thread+++++++++++++++++++\n ",qinlen);

 

  
  CelthApi_Demux_GetSection(hHandle,sectionbuf,&sectlength,1024);

	printf("GET Section DATA with length [%x] :\n",sectlength);
  	for(i=0;i<sectlength;i++)
  	{
  		printf("%x\t",sectionbuf[i]);
  		
  	}
	printf("\n");
}
  

 return NULL;
}



void testtime_fn(void* arg)
{
	printf("timer has trigger\n");
}


void printinfo(CHANNEL_INFO_T *info) {
	int i,j;

	printf(
		"\n"
		"version %d\n"
		"transport_stream_id %d\n"
		"sectionBitmap %d\n"
		"num_programs %d\n\n",
		info->version,
		info->transport_stream_id,
		info->sectionBitmap,
		info->num_programs);
	for (i=0;i<info->num_programs;i++) {
		printf(
			"program_number %d\n"
			"  version %d\n"
			"  pcr pid 0x%x\n"
			"  video_pids:",
			info->program_info[i].program_number,
			info->program_info[i].version,
			info->program_info[i].pcr_pid);
		for (j=0;j<info->program_info[i].num_video_pids;j++)
			printf(" 0x%x/%d",
				info->program_info[i].video_pids[j].pid,
				info->program_info[i].video_pids[j].streamType);
		printf("\n");

		printf("  audio_pids:");
		for (j=0;j<info->program_info[i].num_audio_pids;j++)
			printf(" 0x%x/%d",
				info->program_info[i].audio_pids[j].pid,
				info->program_info[i].audio_pids[j].streamType);
		printf("\n");

		printf("  other_pids:");
		for (j=0;j<info->program_info[i].num_other_pids;j++)
			printf(" 0x%x/%d",
				info->program_info[i].other_pids[j].pid,
				info->program_info[i].other_pids[j].streamType);
		printf("\n");
	}
}




void main()
{


Celth_FrontendBand  frontband;

Celth_TunerStatus tstatus;

Celth_ParserBand parserband;

/*NEXUS_ParserBand parserband;*/


CELTHOSL_TaskId_t pTaskId;


CELTHOSL_TaskId_t pDemuxTaskId;


CELTH_EventId_t  Event;

CELTH_EventId_t  demuxEvent;

CELTHDemux_SectionFilterHandle_t  hSectionHandle;

Celth_DemuxSectionCallback_s SectionCallbackfun;

Celth_Demux_SectionFilter_s  testfilter;


sectiontrans_param sp;


GBEMG_Time_t tm;

Celth_DemodType demodtype;

	



tm.year=2015;
tm.month=10;
tm.day=8;
tm.weekday=4;

tm.hour=12;
tm.minute=58;
tm.second=0;




CELTH_TimerId_t   llTimer;

CHANNEL_INFO_T *info=NULL;

Celth_Rect  viewrect={0,0,120,240};

Celth_Rect  viewrect1={120,0,120,240};

Celth_Rect  viewrect2 = {0,240,120,240};
Celth_Rect  viewrect3 ={120,240,120,240};
Celth_Rect  viewrect4 ={240,0,120,240};
Celth_Rect  viewrect5 ={240,240,120,240};
Celth_Rect  viewrect6 ={360,0,120,240};
Celth_Rect  viewrect7 ={360,240,120,240};


CelthApi_Platform_Init();
CelthApi_Debug_Init();

frontband=0xff;

if((info=(CHANNEL_INFO_T*)malloc(sizeof(CHANNEL_INFO_T)))==NULL)
{
  printf("malloc failed\n");
  return ;
}


printf("5\n");
CELTH_CreateEvent(&Event);
CELTH_CreateEvent(&demuxEvent);


#if 1

CelthApi_Debug_SetDebugLevel(CELTHDBG_eMsg);


CelthApi_Debug_Msg("USE CelthApi_Debug_Msg Here is Debug Level %d\n", CelthApi_Debug_GetDebugLevel());
CelthApi_Debug_Wrn("USE CelthApi_Debug_Wrn Here is Debug Level %d\n", CelthApi_Debug_GetDebugLevel());
CelthApi_Debug_Err("USE CelthApi_Debug_Err Here is Debug Level %d\n", CelthApi_Debug_GetDebugLevel());

CelthApi_Debug_SetDebugLevel(CELTHDBG_eWrn);


CelthApi_Debug_Msg("USE CelthApi_Debug_Msg Here is Debug Level %d\n", CelthApi_Debug_GetDebugLevel());
CelthApi_Debug_Wrn("USE CelthApi_Debug_Wrn Here is Debug Level %d\n", CelthApi_Debug_GetDebugLevel());
CelthApi_Debug_Err("USE CelthApi_Debug_Err Here is Debug Level %d\n", CelthApi_Debug_GetDebugLevel());

CelthApi_Debug_SetDebugLevel(CELTHDBG_eErr);
CelthApi_Debug_Msg("USE CelthApi_Debug_Msg Here is Debug Level %d\n", CelthApi_Debug_GetDebugLevel());
CelthApi_Debug_Wrn("USE CelthApi_Debug_Wrn Here is Debug Level %d\n", CelthApi_Debug_GetDebugLevel());
CelthApi_Debug_Err("USE CelthApi_Debug_Err Here is Debug Level %d\n", CelthApi_Debug_GetDebugLevel());

CelthApi_Debug_SetDebugLevel(CELTHDBG_eMsg);
#endif
printf("1\n");

demodtype=CELTH_D_OFDM;
CelthApi_TunerInit(demodtype);
printf("2\n");

/*
CELTH_TimerCreate(&llTimer);

CELTH_TimerConfigure(llTimer,testtime_fn);

CELTH_TimerStart(llTimer,NULL,1000);
*/

CelthApi_AV_Init();
printf("3\n");


CelthApi_TunerSetFreq(586);




/*CelthApi_TunerSetCallback(tuner_status);
CelthApi_TunerSetCallbackParams(0,(CELTH_VOID *)Event);
*/
frontband = CelthApi_TunerTune();
if(frontband!=0xff){

/*CelthApi_TunerGetTunerStatus(&tstatus);*/


/*pTaskId=CELTHOSL_TaskCreate(NULL,thr_fn,(void*)&tm,1024,15,1024,8);*/





CelthApi_Debug_Msg("frontband=%d\n",frontband);
/*(printf("band=%d,%d\n",frontband,tstatus.lockstate);*/


#if 0
parserband = Celth_ParserBand_0;
/*parserband = NEXUS_ParserBand_e0;*/
tsPsiSearch_getChannelInfo(info,parserband,/*(NEXUS_InputBand)*/frontband);
printf("++++++++++++++++\n");
printinfo(info);

CelthApi_AV_Play(frontband,info->program_info[0].video_pids[0].pid,info->program_info[0].audio_pids[0].pid,info->program_info[0].pcr_pid,info->program_info[0].video_pids[0].streamType,info->program_info[0].audio_pids[0].streamType);


#endif

#if 0

/*CelApi_Av_ChangeVideoFormat();*/

printf("333333\n");
CelthApi_Graphic_Init();

printf("4\n");

BrcmPeg_Init();


/*

CelthApi_Graphic_SetDrawRectwithPixel(0,viewrect,0xff00ff00);

CelthApi_Graphic_SetDrawRectwithPixel(1,viewrect,0xff00ff00);
*/
/*
printf("fill rect with palette\n");
CelthApi_Graphic_FillRectWithPalette(0,viewrect, 22);
CelthApi_Graphic_FillRectWithPalette(0,viewrect1,44);
CelthApi_Graphic_FillRectWithPalette(0,viewrect2,33);
CelthApi_Graphic_FillRectWithPalette(0,viewrect3,66);
*/
/*
CelthApi_Graphic_FillRectWithPalette(0,viewrect4, 12);
CelthApi_Graphic_FillRectWithPalette(0,viewrect5,24);
CelthApi_Graphic_FillRectWithPalette(0,viewrect6,31);
CelthApi_Graphic_FillRectWithPalette(0,viewrect7,55);

*/

/*CelthApi_Graphic_Flush(0);*/






printf("5\n");
CELTH_CreateEvent(&Event);


#endif

	CelthApi_Debug_Msg("init demux section\n");
	CelthApi_Demux_SectionInitialize();
/*	printf("get a section handle\n");*/

#if 0

	CelthApi_Demux_AllocateSectionFilter(NULL,&hSectionHandle);

	


	SectionCallbackfun.fDetectCallback=section_detect;
	SectionCallbackfun.fErrorCallback =NULL;

	CelthApi_Demux_SetSectionFilterCallback(hSectionHandle,&SectionCallbackfun );
	printf("set section handle callback param as event\n");
	CelthApi_Demux_SetSectionFilterCallbackParam(hSectionHandle,(CELTH_VOID *)demuxEvent );

	CelthApi_Demux_SectionSetDefaultFilterSettings(&testfilter);

	printf("set the pid 0x0\n"); 
        CelthApi_Demux_SetSectionFilterPid(hSectionHandle,0x21);
	printf("set section handle filter\n");
	testfilter.mask[0]=0x00;
	testfilter.coefficient[0]=0xFD;
	CelthApi_Demux_SetSectionFilter(hSectionHandle,&testfilter);
	
	
/*	CelthApi_Demux_SetSectionFilterPid(hSectionHandle,0x00);*/

	printf("start the thread to monitor the section,then enable the filter section!\n");
	/*CelthApi_Demux_EnableSectionFilter(hSectionHandle);*/


	sp.ev =demuxEvent;
	sp.hs =hSectionHandle;


	pMsgQ=CELTH_MsgCreate(sizeof(int),16);

	printf("Create the message Q[%x]\n",pMsgQ);

	
	CELTH_Symbol_Register("demux_section_box",pMsgQ);


	
	pDemuxTaskId=CELTHOSL_TaskCreate(NULL,sectiondemux_fn,(CELTH_VOID*)&sp,1024*10,15,0,0);


	
	printf("enable the section\n");
	
	CelthApi_Demux_EnableSectionFilter(hSectionHandle);


#endif

/*
	emerge_Platform_Init(NULL);
    emerge_PsipmtControlInit();
*/
	

	GBEMG_Init(NULL);

	

      GBEMG_Start(NULL);

	


	while(1)
	{
 /*	printf("in main thread sleep\n");*/

	sleep(10);

	/*CELTH_SetEvent(Event);*/

	}
}
else
{
printf("no Locked\n");

}



}


CELTH_BOOL SendSectMsg2process(CELTH_U32 msg)
{
	
	CELTH_MessageQueue_t* pMessage;

	CELTH_INT      iMessageQCapability;

	CELTH_Status_t  eStatus;


	if(CELTH_Symbol_Enquire_Value("demux_section_box",&pMessage))
	{
		return TRUE;
	}
	else
	{
		printf("Get the Message Q[%x]in the send function\n",pMessage);
	}


	
	eStatus =  CELTH_MsgCapabilityGet( pMessage, &iMessageQCapability);

	
	if (iMessageQCapability > 4)
	{
		/* Fill the user input message and sent it to the task ------------------*/

		printf(" the send the message \n");
		eStatus = CELTH_MsgSend( pMessage, &msg);

		if(eStatus!=CELTH_NO_ERROR)
			{return TRUE;}
			
			
	}
	else
	{
			return TRUE;
	}

	return FALSE;
}



