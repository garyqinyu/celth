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


#include "gbemg_registermacro.h"
#include "gbemg_boxctlmacro.h"

#include "gbemg_boxctrlpriv.h"




static  CELTHDemux_SectionFilterHandle_t hemgboxctrlFilter=NULL;

static  CELTH_EventId_t  emgboxctrlEvent;

static CELTHOSL_TaskId_t emgboxctrlsectmonitorTaskId;


static U8 boxcoverareacode[6];


static GBEMG_Msg_Type CoverCmdToMsg(U8 cmdtag)
{

   GBEMG_Msg_Type msgtype=GBEMG_MSG_eMax;

   switch(cmdtag)
   	{

	case 1:

		msgtype=GBEMG_MSG_eUrgentTrain;
		break;

	case 2:
		msgtype=GBEMG_MSG_eValumeCtrl;
        break;
	case 3:
		msgtype=GBEMG_MSG_ePowerSwitch;
		break;
	case 4:
		msgtype=GBEMG_MSG_eAmpSwitch;
		break;
	case 5:
		msgtype=GBEMG_MSG_eSetTime;
		break;
	case 6:
		msgtype=GBEMG_MSG_eSetReturnCircle;
		break;
	case 7:
		msgtype=GBEMG_MSG_eSetReturnIp;
		break;
	case 8:
		msgtype=GBEMG_MSG_eRequestFile;
		break;
	default:
		break;
		
		
   	}
   return msgtype;
	
}




static CELTH_VOID emgboxctrlsection_detect(CELTH_INT iparam,CELTH_VOID* pparam)
{
	CELTH_EventId_t  event = (CELTH_EventId_t)pparam;
	CelthApi_Debug_Msg("\n++++++detectBOXCTRLSect ++++++++++\n");
	CELTH_SetEvent(event);
		
}

static bool gbemg_emgboxctrlsectmonitor_start(CELTH_EventId_t ev)
{

	CelthApi_Debug_EnterFunction("gbemg_emgboxctrlsectmonitor_start");

	Celth_Demux_SectionFilter_s  emgboxctrlFilterSettings;

	int i;

	
	Celth_DemuxSectionCallback_s emgboxctrlSectionCallbackfun;

	bool bret =false;

	emgboxctrlSectionCallbackfun.fDetectCallback=emgboxctrlsection_detect;

	emgboxctrlSectionCallbackfun.fErrorCallback =NULL;

	CelthApi_Demux_AllocateSectionFilter(NULL,&hemgboxctrlFilter);

	if(hemgindxFilter)
	{

		CelthApi_Demux_SetSectionFilterPid(hemgboxctrlFilter,GBEMG_PID);		


		CelthApi_Demux_SectionSetDefaultFilterSettings(&emgboxctrlFilterSettings);
						
	

		
		emgboxctrlFilterSettings.mask[0]=0x00;
		
		
		emgboxctrlFilterSettings.coefficient[0]=GBEMG_BOXCTRL_TABLEID|;
		
		CelthApi_Demux_SetSectionFilter(hemgboxctrlFilter,&emgboxctrlFilterSettings);

		CelthApi_Demux_SetSectionFilterCallback(hemgboxctrlFilter,&emgboxctrlSectionCallbackfun);

		CelthApi_Demux_SetSectionFilterCallbackParam(hemgboxctrlFilter,(void *)ev);

	
		CelthApi_Demux_EnableSectionFilter(hemgboxctrlFilter);

		bret=true;
	}

	CelthApi_Debug_LeaveFunction("gbemg_emgboxctrlsectmonitor_start");
	return bret;
}





void* emgboxctrlsecmonitor_fn(void* arg)
{

	CELTH_EventId_t  ev;
	CELTH_Error_Code  ret;

	U8 curver=0xFF;
	
	U8 getsectver;

	GBEMG_Msg_t  Msg;
	
	U32 boxctrlsectlen;
	
	U8 boxctrlsectbuf[512];

	U8 boxbuf[512];

	U8 infocoverareacode[6];

	U8 infolen;

	U8 i;
	U8 j;

	U8 EBM_cover_area_num;

	ev = (CELTH_EventId_t) arg;


	


	CelthApi_Debug_EnterFunction("emgboxctrlsecmonitor_fn");


	Msg.msg_type=GBEMG_MSG_eBoxType;
	Msg.msg_packet_length=0;
	Msg.msg_packet=NULL;


    CELTHOSL_Memset(boxctrlsectbuf,0xff,sizeof(U8)*512);
	
	CELTHOSL_Memset(boxbuf,0xff,sizeof(U8)*512);


	GetTheBoxCoverAreaCode(&boxcoverareacode);


#if 1

	
	if(gbemg_emgboxctrlsectmonitor_start(ev))
	{



		
		/* first loop for first indx section capture, and fill the first indxtable and version num*/	
		while(1)
			{
				ret=CELTH_WaitForEvent(ev,500);

				if(ret==CELTHERR_ERROR_TIMEOUT)
				{
					continue;
				}

				CelthApi_Demux_GetSection(hemgindxFilter,boxctrlsectbuf,&boxctrlsectlen,512);

				CelthApi_Debug_Msg("Get boxctrlsectlen=%d!\n",boxctrlsectlen);


				getsectver =  GBEMG_BOXCTL_VERSION_NUM(boxctrlsectbuf);

				CelthApi_Debug_Msg("Get boxver=%d!\n",getsectver);
				
				if(curver!=getsectver)
				{
					curver=getsectver;

					

					CELTHOSL_Memcpy(boxbuf,boxctrlsectbuf,512);

					EBM_cover_area_num=GBEMG_BOXCTL_EBM_AREACODE_NUM(boxbuf);

					boxbuf += GBEMG_BOXCTL_TABLE_HEADER_LENGTH;

					for(i=0;i<EBM_cover_area_num;i++)
					{
						CELTHOSL_Memcpy(infocoverareacode,boxbuf,sizeof(U8)*6);

						infolen=GBEMG_BOXCTL_EBM_CONTROL_INFO_TOTALLEN(boxbuf);

						

						if(CELTHOSL_Memcmp(boxcoverareacode,infocoverareacode,sizeof(U8)*6))
							{
								U8 cmdnum;
                                U8 cmdtag;
								
								cmdnum=GBEMG_BOXCTL_EBM_CONTROL_INFO_TOTALNUM(boxbuf);
								for(j=0;j<cmdnum;j++)
								{
									cmdtag=GBEMG_BOXCTL_EBM_CONTROL_INFO_TAG(boxbuf);

                                    Msg.msg_type=CoverCmdToMsg(cmdtag);  

									Msg.msg_packet_length=GBEMG_BOXCTL_EBM_CONTROL_INFO_LEN(boxbuf);


									if(Msg.msg_packet_length!=0)
										{

											Msg.msg_packet = CELTHOSL_MemoryAlloc(Msg.msg_packet_length*sizeof(U8));

											if(Msg.msg_packet)
											{
												CELTHOSL_Memcpy(Msg.msg_packet,GBEMG_BOXCTL_EBM_CONTROL_INFO_DATA(boxbuf),Msg.msg_packet);
											}
										}

									SendPlayMsg2PlayHander(Msg);

									boxbuf += 2; /* through the tag and len*/
									boxbuf+=GBEMG_BOXCTL_EBM_CONTROL_INFO_LEN(boxbuf);
									


									
								}
								
							}
						boxbuf+=infolen;
					}
					
				}
				
				
				
			}
		}
	
	
	
	CelthApi_Debug_LeaveFunction("emgboxctrlsecmonitor_fn");

}



U8 Start_GBEMG_MonitorBoxCtrlInfoSectionHandler(void* content)
{

	CelthApi_Debug_EnterFunction("Start_GBEMG_MonitorBoxCtrlInfoSectionHandler");
	
	


	CELTH_CreateEvent(&emgboxctrlEvent);


	
	emgboxctrlsectmonitorTaskId = CELTHOSL_TaskCreate(NULL,emgboxctrlsecmonitor_fn,(void*)emgboxctrlEvent,1024*2,13,0,0);

  

	CelthApi_Debug_LeaveFunction("Start_GBEMG_MonitorBoxCtrlInfoSectionHandler");

	
}



