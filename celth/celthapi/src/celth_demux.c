
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>


#include "celth_stddef.h"
#include "celth_mutex.h"
#include "celth_demux.h"

#include "celth_tuner.h"

#include "celth_ringbuffer.h"



#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_message.h"


#include "bstd.h"
#include "bkni.h"
 

#define SECTIONLENGTHGETLENGTH 4

CELTH_STATIC	CELTHOSL_Mutex_t			s_SectionFilterReadMutex;




CELTH_STATIC	CELTHOSL_Sem_t			s_stSectionFilterArraySemaphore;
CELTH_STATIC	CELTHOSL_SemId_t			s_SectionFilterArraySemaphoreId;




typedef struct CELTHDemux_Filter_Local_s
{

	bool secgetterminate;
	bool overflowsecgetterminate;
	BKNI_EventHandle hEvent;

	BKNI_EventHandle hOverflowEvent;


	NEXUS_MessageHandle  sectmsg;

	NEXUS_PidChannelHandle sectpidchannel;

	Celth_ringbuf*        psecbuf;


	pthread_t SectionMsgthread;

	
	pthread_t SectionCrcErrorthread;

	
	CELTH_UINT ithreadstacksize;

	CELTH_CHAR ulsectionlenghbuf[SECTIONLENGTHGETLENGTH];


	
	Celth_DemuxSectionCallback_s       Callbackfun;

	CELTH_VOID*  pCallbackDetectParam;
	


	
	

}CELTHDemux_Filter_Local_s;


typedef struct CELTHDemux_Section_s
{
    CELTH_INT									iFilterSectionIndex;
    CELTH_PID							wPid;
    Celth_Demux_SectionFilter_s		stFilterData;
    CELTH_BOOL							bEnabled;
	CELTH_BOOL                          bBigBuf;
	CELTH_INT                           iBigBufSize;/* if bBigBUf==FALSE,this item value is zero*/
	CELTH_VOID*                         pBigBuffer;
	CELTHDemux_Filter_Local_s   filterlocal;
} CELTHDemux_Section_s;

#if (CELTH_DEMUX_MAX_SECTION_FILTERS != 0)
CELTH_STATIC	CELTHDemux_Section_s	s_astSectionFilter[CELTH_DEMUX_MAX_SECTION_FILTERS];
#endif





void * sectionmsgmonitor_thread(void *Local_)
{
	int rc;
	CELTHDemux_Filter_Local_s *local=Local_;
	
	
	
	const unsigned char *buffer;

	while(!local->secgetterminate)
	{
		size_t size;

        rc = NEXUS_Message_GetBuffer(local->sectmsg, (const void **)&buffer, &size);

/*	printf("\ntry to get data length [%x]\n",size);*/		
	if (!size) {
            rc = BKNI_WaitForEvent(local->hEvent,1000); /* wait 1 seconds */
            if (rc) { rc = -1; 
            		continue;
			}
		}
		
		if(size!=0)
		{
		CelthApi_Debug_Msg("\n===W LEN=[%x],D=[%x]",size,buffer[0]);
		CelthApi_Debug_Msg(",[%x]=====\n",buffer[1]);
		CelthApi_Debug_Msg("\n===W LAST Two D[%x],  D[%x]=====\n",buffer[size-2],buffer[size-1]);
  		/*celth_rb_write(local->psecbuf,buffer,size);*/
		rc=NEXUS_Message_ReadComplete(local->sectmsg,size);
		/* write to ringbuf*/
		celth_rb_write(local->psecbuf,buffer,size);
		/* call callback function to notify the high level app*/

			if(local->Callbackfun.fDetectCallback)
			{
				local->Callbackfun.fDetectCallback((CELTH_INT)size, local->pCallbackDetectParam);
			}
		
		BKNI_Sleep(10);		


	/*	rc = NEXUS_Message_ReadComplete(local->sectmsg, size);*/
		}

	}
	
}



void * sectionmsgoverflowmonitor_thread(void *Local_)
{
	int rc;
	 CELTHDemux_Filter_Local_s *local=Local_;
	
	
	 const unsigned char *buffer;

	 size_t size;
	
	
	
	while(!local->overflowsecgetterminate)
	{
	
            rc = BKNI_WaitForEvent(local->hOverflowEvent,1000); /* wait 1 seconds */
            if (rc) { rc = -1; 
            continue;
			}

			NEXUS_Message_GetBuffer(local->sectmsg, (const void **)&buffer, &size);

			CelthApi_Debug_Msg("\n!!!!!CALL OVERFLOW ERROR!!!!!!!!!!\n");	
			celth_rb_write(local->psecbuf,buffer,size);

			NEXUS_Message_ReadComplete(local->sectmsg, size);


			if(local->Callbackfun.fErrorCallback)
			{
				local->Callbackfun.fErrorCallback();
			}

			
		
	}
}



static int filteroverflowmonitor_thread_start(CELTHDemux_Filter_Local_s *local)
{
    int rc;

        pthread_attr_t attr;

        struct sched_param param;

        int iPriority = 16;


        pthread_attr_init(&attr);


        pthread_attr_setschedpolicy(&attr, SCHED_RR);


        param.sched_priority = iPriority;

    pthread_attr_setschedparam(&attr, &param);

        pthread_attr_setstacksize(&attr, local->ithreadstacksize);


	
	
    local->overflowsecgetterminate=false;
    rc = pthread_create(&local->SectionCrcErrorthread, NULL, sectionmsgoverflowmonitor_thread, local);
    
    return rc;
}

static void filteroverflowmonitor_thread_stop(CELTHDemux_Filter_Local_s *local)
{
    local->overflowsecgetterminate = true;
    pthread_join(local->SectionCrcErrorthread, NULL);
}





static void filtermessage_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}


static int filtermonitor_thread_start(CELTHDemux_Filter_Local_s *local)
{
    int rc;
	
	pthread_attr_t attr;   

	struct sched_param param;

	int iPriority = 16; 

	
	pthread_attr_init(&attr);

	
	pthread_attr_setschedpolicy(&attr, SCHED_RR);

	
	param.sched_priority = iPriority;
	
	pthread_attr_setschedparam(&attr, &param);
	
	pthread_attr_setstacksize(&attr, local->ithreadstacksize);
	
    local->secgetterminate=false;
    rc = pthread_create(&local->SectionMsgthread, &attr, sectionmsgmonitor_thread, local);
    
    return rc;
}

static void filtermonitor_thread_stop(CELTHDemux_Filter_Local_s *local)
{
    local->secgetterminate = true;
    pthread_join(local->SectionMsgthread, NULL);
}


static void filtermessage_overflow_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}








CELTH_ErrorCode_t	CelthApi_Demux_SectionInitialize( CELTH_VOID )
{



#if (CELTH_DEMUX_MAX_SECTION_FILTERS != 0)
    CELTH_INT		i, j;


    for( i = 0 ; i < CELTH_DEMUX_MAX_SECTION_FILTERS ; i++ ) 
	{
        s_astSectionFilter[i].iFilterSectionIndex	= i;
		s_astSectionFilter[i].bEnabled	= FALSE;
		s_astSectionFilter[i].bBigBuf   = FALSE;
		s_astSectionFilter[i].iBigBufSize = 0;
		s_astSectionFilter[i].pBigBuffer = NULL;
        for( j = 0 ; j < CELTH_FILTER_SIZE ; j++ ) 
		{
            s_astSectionFilter[i].stFilterData.coefficient[j]=0x00;
			s_astSectionFilter[i].stFilterData.exclusion[j]=0xff;
            s_astSectionFilter[i].stFilterData.mask[j] = 0xff;
			
        }


		s_astSectionFilter[i].wPid =0x1FFF;



		/* initialize the local paramter*/





		s_astSectionFilter[i].filterlocal.secgetterminate=FALSE;

		
		s_astSectionFilter[i].filterlocal.ithreadstacksize=0x8000;
		

		s_astSectionFilter[i].filterlocal.sectmsg=NULL;
		s_astSectionFilter[i].filterlocal.sectpidchannel=NULL;
		s_astSectionFilter[i].filterlocal.hEvent=NULL;
		s_astSectionFilter[i].filterlocal.hOverflowEvent=NULL;

		
		s_astSectionFilter[i].filterlocal.Callbackfun.fDetectCallback=NULL;
		s_astSectionFilter[i].filterlocal.Callbackfun.fErrorCallback=NULL;


/* first set the ringbuffer is NULL, when allocate new filter  ,malloc it */ 
		s_astSectionFilter[i].filterlocal.psecbuf=NULL;


		

		
    }
#endif

	s_SectionFilterArraySemaphoreId	= CELTH_SemInit( "CELTH_SFA", &s_stSectionFilterArraySemaphore, CELTHOSL_FifoNoTimeOut, 1 );
	if( s_SectionFilterArraySemaphoreId == NULL ) 
	{
		return CELTH_ERROR_FAIL ;
	}

	 return CELTH_NO_ERROR ;




	

}




CELTH_ErrorCode_t	CelthApi_Demux_AllocateSectionFilter( Celth_DemuxSectionCallback_s* pFilterCallback, CELTHDemux_SectionFilterHandle_t  *phFilterHandle )
{


#if (CELTH_DEMUX_MAX_SECTION_FILTERS != 0)
    CELTH_INT		i;

	i=0;
    while( i < CELTH_DEMUX_MAX_SECTION_FILTERS  ) 
	{
		if(s_astSectionFilter[i].bEnabled==FALSE)
			{
			CelthApi_Debug_Msg("\nallocate the filter num is [%x]\n",i);
			break;
			}
		else
			{
			i++;
			}
   	}

	if(i==CELTH_DEMUX_MAX_SECTION_FILTERS)
	{
		*phFilterHandle=NULL;
		return CELTH_ERROR_NO_FREE_HANDLES;
	}
	else
	{
		

	
		if(celth_rb_init(&s_astSectionFilter[i].filterlocal.psecbuf,0x2000)!=0)
		{
	
			return CELTH_ERROR_NO_MEMORY ;
		}



                BKNI_CreateEvent(&s_astSectionFilter[i].filterlocal.hEvent);

				

		if(s_astSectionFilter[i].filterlocal.hEvent==NULL)
		{
			return CELTH_ERROR_NO_MEMORY;
		}


                BKNI_CreateEvent(&s_astSectionFilter[i].filterlocal.hOverflowEvent);

				
		if(s_astSectionFilter[i].filterlocal.hOverflowEvent==NULL)
		{
			return CELTH_ERROR_NO_MEMORY;
		}



if(pFilterCallback!=NULL)
{	
		s_astSectionFilter[i].filterlocal.Callbackfun.fDetectCallback=pFilterCallback->fDetectCallback;
		s_astSectionFilter[i].filterlocal.Callbackfun.fErrorCallback=pFilterCallback->fErrorCallback;
}
		s_astSectionFilter[i].bEnabled=TRUE;

		

		
		*phFilterHandle=(CELTHDemux_SectionFilterHandle_t)&s_astSectionFilter[i];


		return CELTH_NO_ERROR;
	}


	#endif



	
		


}
CELTH_ErrorCode_t	CelthApi_Demux_FreeSectionFilter(  CELTHDemux_SectionFilterHandle_t hFilterHandle )
{
    CELTHDemux_Section_s*	pstFilterBody;


	CELTH_INT i;
	

	if(hFilterHandle==NULL)
		{
		return CELTH_ERROR_BAD_PARAMETER;
		}
	
	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;

	if(pstFilterBody->bBigBuf)
		{
			if(pstFilterBody->pBigBuffer)
				{
			NEXUS_Memory_Free(pstFilterBody->pBigBuffer);
			pstFilterBody->iBigBufSize=0;
				}
			
		}

	pstFilterBody->bBigBuf=FALSE;
	
    if( pstFilterBody->bEnabled ) 
	{

		if(pstFilterBody->filterlocal.sectmsg!=NULL)
			{
				
				NEXUS_Message_Stop(pstFilterBody->filterlocal.sectmsg);

				
				NEXUS_Message_Close(pstFilterBody->filterlocal.sectmsg);
				pstFilterBody->filterlocal.sectmsg=NULL;

			
			}
		if(pstFilterBody->filterlocal.sectpidchannel!=NULL)
			{
				NEXUS_PidChannel_Close(pstFilterBody->filterlocal.sectpidchannel);
				pstFilterBody->filterlocal.sectpidchannel=NULL;
			}

    }


	
/*	CELTH_SemWait( s_SectionFilterArraySemaphoreId, CELTHOSL_WaitForEver );*/

	pstFilterBody->bEnabled=FALSE;

	for( i = 0 ; i < CELTH_FILTER_SIZE ; i++ ) 
	{
		pstFilterBody->stFilterData.coefficient[i]=0x00;
		pstFilterBody->stFilterData.exclusion[i]=0xFF;
		pstFilterBody->stFilterData.mask[i] = 0xFF;
		
	}
		if(pstFilterBody->filterlocal.hEvent)
		{

		
		BKNI_ResetEvent(pstFilterBody->filterlocal.hEvent);	
                BKNI_DestroyEvent(pstFilterBody->filterlocal.hEvent);
		pstFilterBody->filterlocal.hEvent=NULL;
		}
		if(pstFilterBody->filterlocal.hOverflowEvent)
		{

		BKNI_ResetEvent(pstFilterBody->filterlocal.hOverflowEvent);
                BKNI_DestroyEvent(pstFilterBody->filterlocal.hOverflowEvent);
		pstFilterBody->filterlocal.hOverflowEvent=NULL;

		}




	pstFilterBody->filterlocal.Callbackfun.fDetectCallback=NULL;
	pstFilterBody->filterlocal.Callbackfun.fErrorCallback=NULL;


	celth_rb_clear(pstFilterBody->filterlocal.psecbuf);
	celth_rb_destroy(pstFilterBody->filterlocal.psecbuf);

	
   /* CELTH_SemPost( s_SectionFilterArraySemaphoreId );*/
	

	return	CELTH_NO_ERROR;

	



}
CELTH_ErrorCode_t	CelthApi_Demux_SetSectionFilterPid( CELTHDemux_SectionFilterHandle_t hFilterHandle, CELTH_PID wPid )
{

	CELTHDemux_Section_s*	pstFilterBody;

	CELTH_INT i;



	

    NEXUS_PidChannelHandle pidChannel;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidChannelSettings;

    Celth_ParserBand parserBand = Celth_ParserBand_0;


	if(hFilterHandle==NULL)
	{
		return CELTH_ERROR_BAD_PARAMETER;
	}

	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;



	
	if(pstFilterBody->bBigBuf)
		{
		  if(pstFilterBody->pBigBuffer)
		  	{
		  		pstFilterBody->iBigBufSize=0;
		  		NEXUS_Memory_Free(pstFilterBody->pBigBuffer);
		  	}
		}


	if( pstFilterBody->bEnabled ) 
	{

		if(pstFilterBody->filterlocal.sectmsg!=NULL)
		{
			
			NEXUS_Message_Stop(pstFilterBody->filterlocal.sectmsg);

			
			NEXUS_Message_Close(pstFilterBody->filterlocal.sectmsg);

		
		}
		if(pstFilterBody->filterlocal.sectpidchannel!=NULL)
		{
			NEXUS_PidChannel_Close(pstFilterBody->filterlocal.sectpidchannel);
		}

	}


	pstFilterBody->bEnabled=TRUE;


/* clear the section filter and the data buffer*/
	
/*	CELTH_SemWait( s_SectionFilterArraySemaphoreId, CELTHOSL_WaitForEver );*/



	for( i = 0 ; i < CELTH_FILTER_SIZE ; i++ ) 
	{
		pstFilterBody->stFilterData.coefficient[i]=0x00;
		pstFilterBody->stFilterData.exclusion[i]=0xFF;
		pstFilterBody->stFilterData.mask[i] = 0xFF;
		
	}



	celth_rb_clear(pstFilterBody->filterlocal.psecbuf);

	
  /*  CELTH_SemPost( s_SectionFilterArraySemaphoreId );*/


	/* set the new pid to the section*/

	pstFilterBody->wPid=wPid;
	



	NEXUS_ParserBand_GetSettings((NEXUS_ParserBand)parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
	
    parserBandSettings.sourceTypeSettings.inputBand =(NEXUS_InputBand) CelthApi_GetFrontendBandFromTune();  /* Platform initializes this to input band */
    NEXUS_ParserBand_SetSettings((NEXUS_ParserBand)parserBand, &parserBandSettings);

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
   pstFilterBody->filterlocal.sectpidchannel = NEXUS_PidChannel_Open((NEXUS_ParserBand)parserBand, wPid, &pidChannelSettings);

	/*pstFilterBody->filterlocal.sectpidchannel=pidChannel;*/

	return CELTH_NO_ERROR;

	

	
}


CELTH_ErrorCode_t	CelthApi_Demux_SetSectionFilterCallback(  CELTHDemux_SectionFilterHandle_t hFilterHandle,Celth_DemuxSectionCallback_s* pFilterCallback )
{
	
	CELTHDemux_Section_s*	pstFilterBody;

	
	if(hFilterHandle==NULL)
	{
		return CELTH_ERROR_BAD_PARAMETER;
	}

	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;
	


	
/*	CELTH_SemWait( s_SectionFilterArraySemaphoreId, CELTHOSL_WaitForEver );*/

if(pFilterCallback!=NULL)
{
	pstFilterBody->filterlocal.Callbackfun.fDetectCallback=pFilterCallback->fDetectCallback;
	pstFilterBody->filterlocal.Callbackfun.fErrorCallback =pFilterCallback->fErrorCallback;
}
	
 /*   CELTH_SemPost( s_SectionFilterArraySemaphoreId );*/

	
	return CELTH_NO_ERROR;
}

CELTH_ErrorCode_t	CelthApi_Demux_SetSectionFilterCallbackParam(  CELTHDemux_SectionFilterHandle_t hFilterHandle,CELTH_VOID* pparam )
{
	CELTHDemux_Section_s*	pstFilterBody;

	
	if(hFilterHandle==NULL)
	{
		return CELTH_ERROR_BAD_PARAMETER;
	}

	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;
	


	
/*	CELTH_SemWait( s_SectionFilterArraySemaphoreId, CELTHOSL_WaitForEver );*/

	pstFilterBody->filterlocal.pCallbackDetectParam=pparam;

	
/*    CELTH_SemPost( s_SectionFilterArraySemaphoreId );*/
	return CELTH_NO_ERROR;
}



CELTH_ErrorCode_t   CelthApi_Demux_SetSectionFilterBigBufParam(CELTHDemux_SectionFilterHandle_t hFilterHandle,CELTH_INT iBufSize,CELTH_BOOL bBig)
{

	
	CELTHDemux_Section_s*	pstFilterBody;

	
	if(hFilterHandle==NULL)
	{
		return CELTH_ERROR_BAD_PARAMETER;
	}

	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;


	pstFilterBody->bBigBuf=bBig;

	pstFilterBody->iBigBufSize = iBufSize;

	
	
	return CELTH_NO_ERROR;
	
	
}



CELTH_ErrorCode_t	CelthApi_Demux_SetSectionFilter( CELTHDemux_SectionFilterHandle_t hFilterHandle, Celth_Demux_SectionFilter_s* pstFilterData )
{


	CELTHDemux_Section_s*	pstFilterBody;

	CELTH_INT i;



	NEXUS_MessageSettings settings;
	NEXUS_MessageStartSettings startSettings;
	NEXUS_ParserBandSettings parserBandSettings;

	


	NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;


	
	if(hFilterHandle==NULL)
	{
		return CELTH_ERROR_BAD_PARAMETER;
	}

	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;


	if(pstFilterBody->bBigBuf)
		{
		CelthApi_Debug_Msg("\n pstFilterBody->bBigBuf=%x",pstFilterBody->bBigBuf);
		  if(pstFilterBody->pBigBuffer)
		  	{
		  		/*pstFilterBody->iBigBufSize=0;*/ /* just free the buf*/

		CelthApi_Debug_Msg("\nFree the bigbuffer\n");
		  		NEXUS_Memory_Free(pstFilterBody->pBigBuffer);
		  	}
		}


	if( pstFilterBody->bEnabled ) 
	{

		if(pstFilterBody->filterlocal.sectmsg!=NULL)
		{
		
			NEXUS_Message_Stop(pstFilterBody->filterlocal.sectmsg);

			
			NEXUS_Message_Close(pstFilterBody->filterlocal.sectmsg);

	
		}

	}

	


	pstFilterBody->bEnabled=TRUE;


/* clear the buffer data*/

	
	/*CELTH_SemWait( s_SectionFilterArraySemaphoreId, CELTHOSL_WaitForEver );*/



	celth_rb_clear(pstFilterBody->filterlocal.psecbuf);

	
  /*  CELTH_SemPost( s_SectionFilterArraySemaphoreId );*/


	/* start the nexus message */


	
    NEXUS_Message_GetDefaultSettings(&settings);
    settings.dataReady.callback = filtermessage_callback;
    settings.dataReady.context = pstFilterBody->filterlocal.hEvent;

    settings.overflow.callback = filtermessage_overflow_callback;
    settings.overflow.context = pstFilterBody->filterlocal.hOverflowEvent;

	if(pstFilterBody->bBigBuf)
	{

	
		settings.bufferSize = 0;

CelthApi_Debug_Msg("\nfilterMsg.settings buffersize as zero %x ---\n",settings.bufferSize);
	}

	
   	 settings.maxContiguousMessageSize = /*4096;*/0;

    pstFilterBody->filterlocal.sectmsg = NEXUS_Message_Open(&settings);

/*	pstFilterBody->filterlocal.sectmsg=msg;*/

	
	memcpy(pstFilterBody->stFilterData.mask,pstFilterData->mask, CELTH_FILTER_SIZE);
    memcpy(pstFilterBody->stFilterData.coefficient,pstFilterData->coefficient,CELTH_FILTER_SIZE);
	memcpy(pstFilterBody->stFilterData.exclusion,pstFilterData->exclusion,CELTH_FILTER_SIZE);

#if 0
	
    NEXUS_Message_GetDefaultStartSettings(pstFilterBody->filterlocal.sectmsg, &startSettings);
    startSettings.pidChannel = pstFilterBody->filterlocal.sectpidchannel;

	memcpy(startSettings.filter.mask,pstFilterBody->stFilterData.mask, CELTH_FILTER_SIZE);
    memcpy(startSettings.filter.coefficient,pstFilterBody->stFilterData.coefficient,CELTH_FILTER_SIZE);
	memcpy(startSettings.filter.exclusion,pstFilterBody->stFilterData.exclusion,CELTH_FILTER_SIZE);

	printf("in set filter function start message pid channel is 0x[%08x]\n",startSettings.pidChannel);
	printf("in set filter function start message  message is 0x[%08x]\n",pstFilterBody->filterlocal.sectmsg);
    NEXUS_Message_Start(pstFilterBody->filterlocal.sectmsg, &startSettings);
#endif

	return CELTH_NO_ERROR;

	
}




#define TS_READ_16( BUF ) ((uint16_t)((BUF)[0]<<8|(BUF)[1]))
#define TS_PSI_GET_SECTION_LENGTH( BUF )    (uint16_t)(TS_READ_16( &(BUF)[1] ) & 0x0FFF)




CELTH_BOOL	CelthApi_Demux_ReadSectionLength( CELTHDemux_SectionFilterHandle_t hFilterHandle, CELTH_UINT* puSectionLength)
{

	CELTHDemux_Section_s*	pstFilterBody;


	CELTH_CHAR  i;

	CELTH_INT left,sectionlength;

	CELTH_BOOL bMoreDataAvialable = FALSE;


	if(hFilterHandle==NULL) 
	{
		return FALSE;
	}

	if(puSectionLength==NULL)
	{
	
		return FALSE;
	}

	

	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;

	for(i=0;i<SECTIONLENGTHGETLENGTH;i++)
	{
		pstFilterBody->filterlocal.ulsectionlenghbuf[i]=0x00;
	}

	left=celth_rb_data_size(pstFilterBody->filterlocal.psecbuf);


	celth_rb_read(pstFilterBody->filterlocal.psecbuf,pstFilterBody->filterlocal.ulsectionlenghbuf,SECTIONLENGTHGETLENGTH);


	sectionlength=TS_PSI_GET_SECTION_LENGTH((pstFilterBody->filterlocal.ulsectionlenghbuf))+3;

	*puSectionLength = sectionlength;
	
	if(left>sectionlength)
	{
	bMoreDataAvialable = TRUE;
	}

	return bMoreDataAvialable;


}
CELTH_ErrorCode_t	CelthApi_Demux_ReadSectionData( CELTHDemux_SectionFilterHandle_t hFilterHandle, CELTH_BYTE* pucSectionData ,CELTH_INT ireadLength)
{
	CELTHDemux_Section_s*	pstFilterBody;


	CELTH_CHAR  i;

	CELTH_INT left;


	if(hFilterHandle==NULL) 
	{
		return CELTH_ERROR_BAD_PARAMETER;
	}

	if(pucSectionData==NULL)
	{
	
		return CELTH_ERROR_BAD_PARAMETER;
	}

	
	
	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;

	celth_rb_read(pstFilterBody->filterlocal.psecbuf,pucSectionData,ireadLength);

	return CELTH_NO_ERROR;


	
}



CELTH_INT  CelthApi_Demux_GetSection(CELTHDemux_SectionFilterHandle_t hFilterHandle,CELTH_BYTE* pucSectionData,CELTH_UINT* puSectionLength,CELTH_UINT maxReadLength )
{

	
	CELTHDemux_Section_s*	pstFilterBody;


	CELTH_CHAR  i;

	CELTH_INT left,sectionlength;

	CELTH_BOOL bMoreDataAvialable = FALSE;


	if(hFilterHandle==NULL) 
	{
		return 0;
	}

	if(puSectionLength==NULL)
	{
	
		return 0;;
	}

	if(pucSectionData==NULL)
	{
		return 0;
	}

	

	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;

/*	for(i=0;i<SECTIONLENGTHGETLENGTH;i++)
	{
		pstFilterBody->filterlocal.ulsectionlenghbuf[i]=0x00;
	}*/
	
	CELTHOSL_Memset(pstFilterBody->filterlocal.ulsectionlenghbuf,0x00,SECTIONLENGTHGETLENGTH);


	
	/*CELTH_SemWait( s_SectionFilterArraySemaphoreId, CELTHOSL_WaitForEver );*/
	left=celth_rb_data_size(pstFilterBody->filterlocal.psecbuf);
	
	if(left==0)
	{	
		*puSectionLength=0;
		return left;
	}

	celth_rb_read(pstFilterBody->filterlocal.psecbuf,pstFilterBody->filterlocal.ulsectionlenghbuf,SECTIONLENGTHGETLENGTH);

	sectionlength=TS_PSI_GET_SECTION_LENGTH((pstFilterBody->filterlocal.ulsectionlenghbuf))+3;

/*	*puSectionLength = sectionlength;*/

	/* 4 bytes aligned*/
	if(sectionlength%4){
		sectionlength=sectionlength-(sectionlength%4)+4;
	}
	


	memcpy(pucSectionData,pstFilterBody->filterlocal.ulsectionlenghbuf,SECTIONLENGTHGETLENGTH);

	if(maxReadLength<=sectionlength)
	{
		sectionlength= maxReadLength;
	}
	*puSectionLength=sectionlength;

	
	left=celth_rb_read(pstFilterBody->filterlocal.psecbuf,pucSectionData+SECTIONLENGTHGETLENGTH,sectionlength-SECTIONLENGTHGETLENGTH);

	
 /*   CELTH_SemPost( s_SectionFilterArraySemaphoreId );*/

	CelthApi_Debug_Msg("\n==R LEN=[%x],D=[%x],",sectionlength,pucSectionData[0]);
	CelthApi_Debug_Msg("[%x],LEFT DATA Len=[%x]==\n",pucSectionData[1],left);
	CelthApi_Debug_Msg("\n==R LAST D[%x],[%x]==\n",pucSectionData[sectionlength-2],pucSectionData[sectionlength-1]);


	return left;
	
}


CELTH_ErrorCode_t	CelthApi_Demux_DisableSectionFilter( CELTHDemux_SectionFilterHandle_t hFilterHandle )
{

	CELTHDemux_Section_s*	pstFilterBody;

	NEXUS_MessageStartSettings startSettings;


	if(hFilterHandle==NULL)	
	{
		return CELTH_ERROR_BAD_PARAMETER;
	}

	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;

	/* stop the two thread first, let them release the resource */

       filtermonitor_thread_stop(&(pstFilterBody->filterlocal));

        filteroverflowmonitor_thread_stop(&(pstFilterBody->filterlocal));


	if(pstFilterBody->filterlocal.sectmsg)
		{
	
    NEXUS_Message_Stop(pstFilterBody->filterlocal.sectmsg);
	/*pstFilterBody->filterlocal.sectmsg=NULL;*/
		}


	return CELTH_NO_ERROR;


}
CELTH_ErrorCode_t	CelthApi_Demux_EnableSectionFilter( CELTHDemux_SectionFilterHandle_t hFilterHandle )
{

	
	CELTHDemux_Section_s*	pstFilterBody;

	NEXUS_MessageStartSettings startSettings;

	
	if(hFilterHandle==NULL)
	{
		return CELTH_ERROR_BAD_PARAMETER;
	}

	pstFilterBody =(CELTHDemux_Section_s*) hFilterHandle;


	if(pstFilterBody->bEnabled)
	{

	/*CELTH_SemWait( s_SectionFilterArraySemaphoreId, CELTHOSL_WaitForEver );*/



	celth_rb_clear(pstFilterBody->filterlocal.psecbuf);

	
    /*CELTH_SemPost( s_SectionFilterArraySemaphoreId );*/

	
	if(pstFilterBody->filterlocal.sectmsg)
		{
    NEXUS_Message_GetDefaultStartSettings(pstFilterBody->filterlocal.sectmsg, &startSettings);
		}
	else
		{
			return CELTH_ERROR_INVALID_HANDLE;
		}

	if(pstFilterBody->filterlocal.sectpidchannel)
		{
    startSettings.pidChannel = pstFilterBody->filterlocal.sectpidchannel;
		}
	else
		{
			return CELTH_ERROR_INVALID_HANDLE;
		}

	memcpy(startSettings.filter.mask,pstFilterBody->stFilterData.mask, CELTH_FILTER_SIZE);
    memcpy(startSettings.filter.coefficient,pstFilterBody->stFilterData.coefficient,CELTH_FILTER_SIZE);
	memcpy(startSettings.filter.exclusion,pstFilterBody->stFilterData.exclusion,CELTH_FILTER_SIZE);


	if(pstFilterBody->bBigBuf)
		{

			CelthApi_Debug_Msg("\n before Allocate the buf length=[%x], addr=[%x]\n",pstFilterBody->iBigBufSize,pstFilterBody->pBigBuffer);
			NEXUS_Memory_Allocate(pstFilterBody->iBigBufSize,NULL,&(pstFilterBody->pBigBuffer));

			if(pstFilterBody->pBigBuffer==NULL)
				{
					return CELTH_ERROR_NO_MEMORY;
				}

			CelthApi_Debug_Msg("\n Allocate the buf leng=[%x], addr=[%x]\n",pstFilterBody->iBigBufSize,pstFilterBody->pBigBuffer);

			startSettings.buffer=pstFilterBody->pBigBuffer;
			startSettings.bufferSize = pstFilterBody->iBigBufSize;
		}
	
	

	if(pstFilterBody->filterlocal.sectmsg)
		{
    NEXUS_Message_Start(pstFilterBody->filterlocal.sectmsg, &startSettings);
		}
	else
		{
			return CELTH_ERROR_INVALID_HANDLE;
		}


	filtermonitor_thread_start(&(pstFilterBody->filterlocal));

	filteroverflowmonitor_thread_start(&(pstFilterBody->filterlocal));

	
	}


	
	return CELTH_NO_ERROR;
	

}


CELTH_VOID  CelthApi_Demux_SectionSetDefaultFilterSettings(Celth_Demux_SectionFilter_s* pstFilterData )
{
	int i;
	if(pstFilterData!=NULL)
	{
		
		for( i = 0 ; i < CELTH_FILTER_SIZE ; i++ ) 
		{
			pstFilterData->coefficient[i]=0xFF;
			pstFilterData->exclusion[i]=0xFF;
			pstFilterData->mask[i] = 0xFF;
			
		}
		
	}
	return;
}



CELTH_VOID	CelthApi_Demux_SectionError( CELTHDemux_SectionFilterHandle_t hFilterHandle )
{
	return ;
}
CELTH_VOID	CelthApi_Demux_SectionDetect( CELTHDemux_SectionFilterHandle_t hFilterHandle,CELTH_UINT* pwSectionLength )
{
	return ;
}












