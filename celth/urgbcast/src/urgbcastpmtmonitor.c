#include <stdio.h>

#include "celth_stddef.h"


#include "celth_rtos.h"

#include "celth_event.h"


#include "celth_mutex.h"

#include "celth_symbol.h"

#include "celth_demux.h"

#include "urgbcastdef.h"
#include "urgbcastcontrol.h"

#include "urgbcast.h"




CELTH_STATIC CELTHOSL_TaskId_t pmtmonitorTaskId;

CELTH_STATIC CELTHOSL_TaskId_t pmtchangeactionTaskId;

CELTH_STATIC  CELTH_EventId_t  pmthandleEvent;

CELTH_STATIC  CELTH_EventId_t  pmtEvent;


CELTH_STATIC  CELTHDemux_SectionFilterHandle_t hPmtFilter=NULL;

CELTH_STATIC  CELTH_BOOL bpmtmonitor=TRUE;


typedef struct local_eventTran_t{
	CELTH_EventId_t pmthev;
	CELTH_EventId_t pmtev;
	


}local_eventTran_t;


CELTH_STATIC  local_eventTran_t localEt;


static CELTH_VOID pmtsection_detect(CELTH_INT iparam,CELTH_VOID* pparam)
{
	CELTH_EventId_t  event = (CELTH_EventId_t)pparam;
	printf("\n+++++++detect pmt section+++++++++++++++++\n");
	CELTH_SetEvent(event);
		
}





CELTH_STATIC CELTH_BOOL emerge_Pmtdatamonitor_start(CELTH_EventId_t ev)
{


	Celth_Demux_SectionFilter_s  pmtFilterSettings;

	int i;

	
	
	Celth_DemuxSectionCallback_s pmtSectionCallbackfun;

	CELTH_BOOL bret =FALSE;


	pmtSectionCallbackfun.fDetectCallback=pmtsection_detect;


	CelthApi_Demux_AllocateSectionFilter(NULL,&hPmtFilter);

	if(hPmtFilter)
	{

		CelthApi_Demux_SetSectionFilterPid(hPmtFilter,GetPmtTableInfo()->pmt_pid);		


		CelthApi_Demux_SectionSetDefaultFilterSettings(&pmtFilterSettings);
						
		pmtFilterSettings.mask[0]=0x00;
		pmtFilterSettings.mask[5]=0xC1;

		pmtFilterSettings.exclusion[5]=((GetPmtTableInfo()->version)*2)&0x3E;

		pmtFilterSettings.coefficient[0]=0x02;
		pmtFilterSettings.coefficient[3]=((GetPmtTableInfo()->pmt_program_num)&0xFF00)>>8 ;
		pmtFilterSettings.coefficient[4]=(GetPmtTableInfo()->pmt_program_num)&0xFF ;
		
		CelthApi_Demux_SetSectionFilter(hPmtFilter,&pmtFilterSettings);

		CelthApi_Demux_SetSectionFilterCallback(hPmtFilter,&pmtSectionCallbackfun);

		CelthApi_Demux_SetSectionFilterCallbackParam(hPmtFilter,(CELTH_VOID *)ev);

	
		CelthApi_Demux_EnableSectionFilter(hPmtFilter);

		bret=TRUE;
	}
	return bret;
}







void* psipmtmonitor_fn(void* arg)
{
	local_eventTran_t* pLocalEt=(local_eventTran_t*) arg;

	emerge_pmttable_info pmttable;

	emerge_pmttable_info* globalpmttable;

	CELTH_U8 pmtsecbuf[512];

	CELTH_U8 pmtoldsecbuf[512];

	CELTH_INT pmtseclen;

	CELTH_INT pmtsecleft;

	
	CELTH_Error_Code  ret;

	
	CELTH_BOOL bcompare;

	CELTHOSL_Memset(&pmttable,0x00,sizeof(emerge_pmttable_info));

	CELTHOSL_Memset(pmtoldsecbuf,0x00,sizeof(CELTH_U8)*512);

	while(pmttable.pmt_pid==0x00)/* did not get the real pmttable*/
		{
		bcastPsiSearch_getEGBTChannelInfo(&pmttable);

		globalpmttable= GetPmtTableInfo();
/*		printf("\nglobalpmttable.version=[%x]pmttable.version=[%x]\n",globalpmttable->version,pmttable.version);*/
		
		if(globalpmttable->version!=pmttable.version)
			{
			SetPmtTableInfo(&pmttable);
			CELTH_SetEvent(pLocalEt->pmthev);
			}
		CELTHOSL_Sleep(20);
		}

	if(emerge_Pmtdatamonitor_start(pLocalEt->pmtev))
		{
		


	do{

		
		ret=CELTH_WaitForEvent(pLocalEt->pmtev,500);

	 	if(ret==CELTHERR_ERROR_TIMEOUT)
		{
			continue;
		}

		do{

			pmtsecleft=CelthApi_Demux_GetSection(hPmtFilter,pmtsecbuf,&pmtseclen,512);


		if(pmtseclen!=0)
		{
		
			
			if(CELTHOSL_Memcmp(pmtoldsecbuf,pmtsecbuf,pmtseclen))
			{
			CELTHOSL_Memcpy(pmtoldsecbuf,pmtsecbuf,pmtseclen);
			
			}
			else
			{
			/* data is identified with last gotten one*/
			 continue;	
			}	

			

			if(bcastSearch_CheckPmtData(pmtsecbuf,GetPmtTableInfo()->pmt_pid,GetPmtTableInfo()->pmt_program_num, pmtseclen,&pmttable))
			{
				
				globalpmttable= GetPmtTableInfo();

				
				if(globalpmttable->version!=pmttable.version)
				{
					SetPmtTableInfo(&pmttable);
					CELTH_SetEvent(pLocalEt->pmthev);
				}

				
				CELTHOSL_Sleep(20);

				
			}
			
			
		}

			}while(pmtsecleft);

		

		
		}while(bpmtmonitor);
	}
	else
	{
		printf("\n*******+++++++PMT monitor did not run+++++++++++++******\n");
	}

	
	
}

void* psipmtchangehandle_fn(void* arg)
{
	CELTH_EventId_t event=(CELTH_EventId_t) arg;


	
	 while(1)
	{

		CELTH_WaitForEvent(event,CELTH_INFINITE);

		emerge_Psiegbtmonitor_stop();
		
		emerge_Psiegbtmonitor_start();
		

		
	}

	
}




CELTH_VOID emerge_PsipmtMonitor_start(local_eventTran_t* localEt)
{

	
	pmtmonitorTaskId = CELTHOSL_TaskCreate(NULL,psipmtmonitor_fn,localEt,1024*2,14,0,0);

	
}

CELTH_VOID emerge_PsipmtChangeHandle_start(CELTH_EventId_t event)
{
	


	pmtchangeactionTaskId = CELTHOSL_TaskCreate(NULL,psipmtchangehandle_fn,event,1024,14,0,0); 

	
}

CELTH_VOID emerge_PsipmtControlInit()
{
	CELTH_CreateEvent(&pmthandleEvent);
	CELTH_CreateEvent(&pmtEvent);


	localEt.pmthev=pmthandleEvent;
	
	localEt.pmtev = pmtEvent;
	
	

	emerge_PsipmtMonitor_start(&localEt);

	emerge_PsipmtChangeHandle_start(pmthandleEvent);

	

	
}







