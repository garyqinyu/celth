#include "celth_stddef.h"

#include "celth_rtos.h"

#include "celth_mutex.h"
#include "celth_message.h"

#include "celth_symbol.h"



#include "celth_demux.h"

#include "celth_event.h"



#include "celth_debug.h"

#include "gbemg_stddef.h"
#include "gbemg_gendef.h"

#include "gbemg_type.h"
#include "gbemg_index.h"
#include "gbemg_content.h"
#include "gbemg_control.h"





static void gbemg_contsection_detect(int iparam,void* pparam)
{
	CELTH_EventId_t  event = (CELTH_EventId_t)pparam;
	printf("\n$$$$$$$$$ detect CONT section---$$$$$$$$$$$$$$$$$$$$$$--------------$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	CELTH_SetEvent(event);
		
}




static bool IsValidContTable(U8* Table)
{
	bool ret=false;

	if(Table!=NULL)
	{

		if(Table[0]==GBEMG_CONT_TABELID)
		{
			ret=true;
		}
	}
	return ret;
}



GBEMG_CaseContent_t* GetContFromParam(U16 contPid,U16 Ebm_id)
{


	CELTH_Error_Code  ret;
	
	CELTH_EventId_t ev;
	
	CELTHDemux_SectionFilterHandle_t hf;
	CELTHDemux_SectionFilterHandle_t lhf;

	
	Celth_Demux_SectionFilter_s  contFilterSettings;


	Celth_DemuxSectionCallback_s contSectionCallbackfun;

	U8 ii,sectbuf[4096];

	U8* pmultisecbuf;

	
	
	

	U32 i,secleft, seclength;

	GBEMG_CaseContent_t* pCont=NULL;

	contSectionCallbackfun.fDetectCallback=gbemg_contsection_detect;
	contSectionCallbackfun.fErrorCallback =NULL;
	
	CelthApi_Demux_AllocateSectionFilter(NULL,&hf);
	
	CELTH_CreateEvent(&ev);

	CelthApi_Debug_Msg("Ebm_id=%x\n",Ebm_id);
	
	
	if(hf)
	{
	
		CelthApi_Demux_SetSectionFilterPid(hf,contPid);	
	
		CelthApi_Demux_SetSectionFilterBigBufParam(hf,1024*8,TRUE);
	
		CelthApi_Demux_SectionSetDefaultFilterSettings(&contFilterSettings);

		/* first ,just get the first section of the content table*/
						
		/* table id mask*/
		contFilterSettings.mask[0]=0x00;

		/* ebm id mask*/
		contFilterSettings.mask[2]=0x00;
		contFilterSettings.mask[3]=0x00;
	
		/* section num mask*/
		contFilterSettings.mask[5]=0x00;
	
	
		contFilterSettings.coefficient[0]=GBEMG_CONT_TABELID;

		contFilterSettings.coefficient[2]=(U8)((Ebm_id>>8)&0xFF);
		contFilterSettings.coefficient[3]=(U8)((Ebm_id)&0xFF);

		contFilterSettings.coefficient[5]=0x00;
		
		CelthApi_Demux_SetSectionFilter(hf,&contFilterSettings);
	
		CelthApi_Demux_SetSectionFilterCallback(hf,&contSectionCallbackfun);
	
		CelthApi_Demux_SetSectionFilterCallbackParam(hf,(void *)ev);
	
	
		CelthApi_Demux_EnableSectionFilter(hf);

		CelthApi_Debug_Msg("\n-------Enable cont filter-------\n");
	}



	ret=CELTH_WaitForEvent(ev,5000);


	if(ret==CELTHERR_ERROR_TIMEOUT)
	{
		CelthApi_Demux_DisableSectionFilter(hf);
		CelthApi_Demux_FreeSectionFilter(hf);
	
		CELTH_DestroyEvent(ev);
		CelthApi_Debug_Err("-------Time out return NULL-------\n");
		return NULL;
	}
	
		
	CelthApi_Demux_GetSection(hf,sectbuf,&seclength,4096);

	CelthApi_Debug_Msg("\n-------Disable cont filter-------\n");

	CelthApi_Demux_DisableSectionFilter(hf);

	CelthApi_Demux_FreeSectionFilter(hf);

	


	if(IsValidContTable(sectbuf))
	{
		U16 ebmid;

		U8 secnum,lastsecnum;

		ebmid =((U16)((sectbuf[3]<<8|sectbuf[4])&0xFFFF));

		CelthApi_Debug_Msg("\nIs valid Cont table, with ebmid=%x\n",ebmid);

		secnum=sectbuf[6];
		lastsecnum=sectbuf[7];
		CelthApi_Debug_Msg("\n the conttable secnum=%x, lastsecnum=%x\n",secnum,lastsecnum);

		if(ebmid==Ebm_id)
		{
			
			CelthApi_Debug_Msg("\nget the right cont table with ebmid=%x, needed ebmid=%x\n",ebmid,Ebm_id);


			if(lastsecnum==0)/* only one section*/
			{
				CelthApi_Debug_Msg("Only one content section, build content!\n");

				pCont=GBEMG_BuildContFromTable(sectbuf,seclength);
			
			}
			else
			{

				CelthApi_Debug_Err("multi content section [%d]\n",lastsecnum);
				

				pmultisecbuf=CELTHOSL_MemoryAlloc((lastsecnum+1)*sizeof(U8)*4096);

				if(secnum<=lastsecnum)
				{
					CelthApi_Debug_Msg("First Copy content section [%d]\n",secnum);

					CELTHOSL_Memcpy((pmultisecbuf+secnum*4096),sectbuf,4096);
					CelthApi_Debug_Msg("\nsectbuf[6]=%x\n",sectbuf[6]);

					CelthApi_Debug_Msg("\nCopy data sectbuf[30]=%x,sectbuf[31]=%x\n",sectbuf[30],sectbuf[31]);
					CelthApi_Debug_Msg("\nCopyed data sectbuf[30]=%x,sectbuf[31]=%x\n",*(pmultisecbuf+sectbuf[6]*4096+30),*(pmultisecbuf+sectbuf[6]*4096+31));	
CelthApi_Debug_Msg("\nCopyed data sectbuf[30]=%x,sectbuf[31]=%x\n",*(pmultisecbuf+30),*(pmultisecbuf+31));	
	
				
				}

/* begin search other section of content table*/

			
			
				for(ii=1;ii<=lastsecnum;ii++)
				{

					CelthApi_Demux_AllocateSectionFilter(NULL,&lhf);
					CELTH_ResetEvent(ev);
					

					if(hf)
					{


					CelthApi_Demux_SetSectionFilterPid(lhf,contPid); 
	
					CelthApi_Demux_SetSectionFilterBigBufParam(lhf,1024*16,TRUE);
			
					CelthApi_Demux_SectionSetDefaultFilterSettings(&contFilterSettings);

					CELTHOSL_Memset(sectbuf,0x00,sizeof(U8)*4096);
							
					contFilterSettings.mask[0]=0x00;
					contFilterSettings.mask[2]=0x00;
					contFilterSettings.mask[3]=0x00;
					contFilterSettings.mask[5]=0x00;
			
			
					contFilterSettings.coefficient[0]=GBEMG_CONT_TABELID;
			
					contFilterSettings.coefficient[2]=(U8)((Ebm_id>>8)&0xFF);
					contFilterSettings.coefficient[3]=(U8)((Ebm_id)&0xFF);
					contFilterSettings.coefficient[5]= ii;
			
					CelthApi_Demux_SetSectionFilter(lhf,&contFilterSettings);
			
					CelthApi_Demux_SetSectionFilterCallback(lhf,&contSectionCallbackfun);
			
					CelthApi_Demux_SetSectionFilterCallbackParam(lhf,(void *)ev);
			
			
					CelthApi_Debug_Msg("\nEnable cont filter again with secnum=%x\n",ii);
					CelthApi_Demux_EnableSectionFilter(lhf);
					
					
					



					ret=CELTH_WaitForEvent(ev,5000);


				/* we dont check if there are  section, just get it*/ 

					CelthApi_Demux_GetSection(lhf,sectbuf,&seclength,4096);

					CelthApi_Debug_Msg("\nGet the %x num section of cont, in fact the num is[%x],",ii,sectbuf[6]);
					CelthApi_Debug_Msg("Last sectnum=[%x]-------\n",sectbuf[7]);
		
					

					CelthApi_Debug_Msg("\nCopy to buf sectbuf[6]=%d\n",sectbuf[6]);


					CELTHOSL_Memcpy((pmultisecbuf+sectbuf[6]*4096),sectbuf,4096);

					CelthApi_Debug_Msg("\nCopy data sectbuf[30]=%x,sectbuf[31]=%x\n",sectbuf[30],sectbuf[31]);
					CelthApi_Debug_Msg("\nCopyed data sectbuf[30]=%x,sectbuf[31]=%x\n",*(pmultisecbuf+sectbuf[6]*4096+30),*(pmultisecbuf+sectbuf[6]*4096+31));	

CelthApi_Debug_Msg("\nCopyed data sectbuf[30]=%x,sectbuf[31]=%x\n",*(pmultisecbuf+ii*4096+30),*(pmultisecbuf+ii*4096+31));	


					CelthApi_Debug_Msg("\nDisable cont filter with secnum=%x\n",ii);

					CelthApi_Demux_DisableSectionFilter(lhf);
					CelthApi_Demux_FreeSectionFilter(lhf);
					

					}





			
				}

/* finished search other content table*/

CelthApi_Debug_Msg("\n0Copyed data sectbuf[30]=%x,sectbuf[31]=%x\n",*(pmultisecbuf+30),*(pmultisecbuf+31));	

CelthApi_Debug_Msg("\n1Copyed data sectbuf[30]=%x,sectbuf[31]=%x\n",*(pmultisecbuf+4096+30),*(pmultisecbuf+4096+31));

CelthApi_Debug_Msg("\n2Copyed data sectbuf[30]=%x,sectbuf[31]=%x\n",*(pmultisecbuf+2*4096+30),*(pmultisecbuf+2*4096+31));

CelthApi_Debug_Msg("\n3Copyed data sectbuf[30]=%x,sectbuf[31]=%x\n",*(pmultisecbuf+3*4096+30),*(pmultisecbuf+3*4096+31));			


				pCont=GBEMG_BuildContFromMultiTable(pmultisecbuf,(lastsecnum+1)*4096);

				CELTHOSL_MemoryFree(pmultisecbuf);
				
			}

		}
		
		
	}

	
	/*CelthApi_Demux_FreeSectionFilter(hf);*/
	
	CELTH_DestroyEvent(ev);

	
return pCont;
	
}


