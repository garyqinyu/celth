
#include "celth_stddef.h"


#include "celth_rtos.h"


#include "celth_list.h"

#include "celth_debug.h"

#include "gbemg_stddef.h"
#include "gbemg_gendef.h"

#include "gbemg_type.h"

#include "gbemg_indexmacro.h"
#include "gbemg_indexpriv.h"




#include "gbemg_index.h"
#include "gbemg_content.h"
#include "gbemg_emglist.h"







/*static U8 local_BuildCaseIndexFromIndexTable(U8* IndexTable,U32 TableLength,IndxSetContentParamCallback Contentcallback);*/


static U8 local_BuildCaseIndxandContFromIndexTable(U8* IndexTable,U32 TableLength,IndxGetContFromParamCallback SetupContcallback);


void SetContentParam(U16 contPid,U16 Ebm_id)
{
	U16 i=0;
	i= contPid + Ebm_id;
	return ;
}


U8 IsValidIndexTable(U8* indextable)
{
	
	GBEMG_Err err=GBEMG_Err_InvalidHandle;

	CelthApi_Debug_EnterFunction("IsValidIndexTable");
	if(indextable!=NULL)
	{
	
		if(GBEMG_INDX_SECTION_TABLE(indextable)==GBEMG_INDX_TABLEID)
		{
			err= GBEMG_No_Err;
		
		}
		else
		{
			err=GBEMG_Err_SectData;
		}
	}

CelthApi_Debug_Msg("IsValidIndexTable ,err=%d\n",err);
CelthApi_Debug_LeaveFunction("IsValidIndexTable");
	
	return err;
}

U8 GetIndexTableVer(U8* indextable)
{
	return GBEMG_INDX_VERSION_NUM(indextable);
}

U8 GetCurrentTableSectionNum(U8* indextable)
{
	return GBEMG_INDX_SECTION_NUM(indextable);
}
U8 GetLastTableSectionNum(U8* indextable)
{
	return GBEMG_INDX_LAST_SECTION_NUM(indextable);
}
U8 GetEBCFreqIndicator(U8* indextable)
{

	return 	GBEMG_INDX_CURRENT_FREQ_INDICATOR(indextable);
}
U8 GetEBMNumber(U8* indextable)
{
	return GBEMG_INDX_EBM_NUM(indextable);
}




GBEMG_CaseIndex_t* CreateNewCaseIndex()
{
	GBEMG_CaseIndex_t* pNewIndex=NULL;
	pNewIndex=CELTHOSL_MemoryAlloc(sizeof(GBEMG_CaseIndex_t)*1);
	return pNewIndex;
}

U8 FillCaseIndex(GBEMG_CaseIndex_t* pIndx,
			U16 emg_case_id,
			GBEMG_URGENT_LEVEL emg_case_urgent_level,
			GBEMG_TYPE   emg_case_type,
			GBEMG_Time_t* emg_issue_time,
			GBEMG_Time_Duration_t* emg_duration)

{
	GBEMG_Err ret=GBEMG_No_Err ;
	if(pIndx==NULL||emg_issue_time==NULL||emg_duration==NULL)
	{
		ret=GBEMG_Err_InvalidParam; 
	}
	else
	{
		pIndx->emg_case_id=emg_case_id;
		pIndx->emg_case_urgent_level = emg_case_urgent_level;
		pIndx->emg_case_type = emg_case_type;
		
		CELTHOSL_Memcpy(&pIndx->emg_issue_time,emg_issue_time,sizeof(GBEMG_Time_t));
		CELTHOSL_Memcpy(&pIndx->emg_duration,emg_duration,sizeof(GBEMG_Time_Duration_t));
		
	}
	
	return ret;
}

U8 SetCaseIndexArea(GBEMG_CaseIndex_t* pIndx,GBEMG_Cover_Area_t* p_area)
{
	GBEMG_Err ret=GBEMG_No_Err ;
	if(pIndx==NULL||p_area==NULL)
	{
		ret=GBEMG_Err_InvalidParam;
	}
	else
	{
		pIndx->pemg_case_cover_area=p_area;
		
	}
	return ret;
}


U8 SetCaseIndexDetailChannel(GBEMG_CaseIndex_t* pIndx,GBEMG_Detail_Channel_t*  pdetailchannel)
{
	GBEMG_Err ret=GBEMG_No_Err ;
	if(pIndx==NULL||pdetailchannel==NULL)
	{
		ret=GBEMG_Err_InvalidParam;
	}
	else
	{
		pIndx->pdetailchannel=pdetailchannel;
		
	}
	return ret;
}


U8 InsertCaseIndexTerParam(GBEMG_CaseIndex_t* pIndx,GBEMG_Terrestrial_Param_t* p_param)
{
	GBEMG_Err ret=GBEMG_No_Err;
	
	GBEMG_Terl_Param_Node* phead;
        GBEMG_Terl_Param_Node* pcur;
	
		
	if(pIndx==NULL||p_param==NULL)
	{
		ret=GBEMG_Err_InvalidParam;
		goto err;
	}
	else
	{
		phead=(GBEMG_Terl_Param_Node*)CELTHOSL_MemoryAlloc(sizeof(GBEMG_Terl_Param_Node));
		
		if(phead)
		{
		
			CELTHOSL_Memcpy(&(phead->pterparam),p_param,sizeof(GBEMG_Terrestrial_Param_t));
			phead->pnext=NULL;
		}
		else
		{
			ret=GBEMG_Err_NoMemory;
			goto err;
		}
		/* insert to the head of terl parameter list*/
		pcur=pIndx->pemg_terl_head;
		
		pIndx->pemg_terl_head=phead;
		
		phead->pnext=pcur;
	}
	
	err:
	return ret;
}


U8 BuildCaseIndexFromIndexTable(U8* IndexTable,U32 TableLength)
{
	/*return local_BuildCaseIndexFromIndexTable(IndexTable,TableLength,SetContentParam);*/

	return local_BuildCaseIndxandContFromIndexTable(IndexTable,TableLength,GetContFromParam);
}


#if 0

U8 local_BuildCaseIndexFromIndexTable(U8* IndexTable,U32 TableLength,IndxSetContentParamCallback Contentcallback)
{
	

	GBEMG_Err ret=GBEMG_No_Err;
	
	U16 tablelen,caselen,emg_case_id,cover_area_num,j,programinfolength;
	
	U8 i, ebm_num,descriptornum,EBCFreqIndicator;
	
	U8* descriptor;
	
	U16 gbemg_stream_pcr_pid,ContPid;
	
	GBEMG_TYPE emg_case_type;
	GBEMG_URGENT_LEVEL emg_case_urgent_level;
	GBEMG_Time_Start_t emg_issue_time;
	GBEMG_Time_Duration_t emg_duration;
	
	GBEMG_Area_t* parea;
	
	GBEMG_Terrestrial_Param_t* pemg_terl_param;
	
	GBEMG_Cover_Area_t cover_area;
	
	GBEMG_Detail_Channel_t* pdetailchannel;
	GBEMG_CaseIndex_t* pIndx;

	ret=IsValidIndexTable(IndexTable);
	if(ret!=GBEMG_No_Err)
	{
		return ret;
	}
	
	
	
	tablelen = GBEMG_INDX_SECTION_LENGTH(IndexTable);
	
	if(tablelen>TableLength)
	{
		return GBEMG_Err_Sectlength;
	}

	ContPid = GBEMG_PID;/* first set it as 0x21*/

	EBCFreqIndicator=GetEBCFreqIndicator(IndexTable);
	
	ebm_num= GetEBMNumber(IndexTable);
	
	IndexTable+=GBEMG_INDX_EBM_GENERATE_LENGTH;
	
	for(i=0;i<ebm_num;i++)
	{
	
		pIndx=CreateNewCaseIndex();
		
		if(pIndx==NULL)
		{
			ret=GBEMG_Err_NoMemory;
			goto err;
			
		}
		caselen = GBEMG_INDX_CONT_EBM_LENGTH(IndexTable);
		emg_case_id = GBEMG_INDX_CONT_EBM_ID(IndexTable);
		emg_case_urgent_level = GBEMG_INDX_CONT_EBM_LEVEL(IndexTable);
		
		GetGBEMGIssueTimeFromIndexTable(IndexTable+GBEMG_INDX_CONT_EBM_START_TIME_OFFSET_FROM_BASE,&emg_issue_time);
		
		GetGBEMGDurationTimeFromIndexTable(IndexTable+GBEMG_INDX_CONT_EBM_DURATION_TIME_OFFSET_FROM_BASE,&emg_duration);
		
		emg_case_type  = GetGBEMGType(IndexTable+GBEMG_INDX_CONT_EBM_TYPE_OFFSET_FROM_BASE);
		
		ret=FillCaseIndex(pIndx,emg_case_id,emg_case_urgent_level,emg_case_type,&emg_issue_time,&emg_duration);
		
		
		/* now parse the cover area */ 
		cover_area_num = GBEMG_INDX_CONT_EBM_COVER_AREA_NUM(IndexTable);
		
		cover_area.cover_area_number = cover_area_num;
		
		parea=(GBEMG_Area_t*)CELTHOSL_MemoryAlloc(sizeof(GBEMG_Area_t)*cover_area_num);
		
		if(parea==NULL)
		{
			CELTHOSL_MemoryFree(pIndx);
			ret=GBEMG_Err_NoMemory;
			goto err;
		}	
		
		for(j=0;j<cover_area_num;j++)
		{
			GetEBMEGCoverArea(IndexTable,&parea[j]);
			IndexTable+=GBEMG_INDX_CONT_EBM_COVER_AREA_SEGMENT_LENGTH;
		}
		
		cover_area.pemg_case_cover_area = parea;
		
		SetCaseIndexArea(pIndx,&cover_area);
		
		/*now fill the terrestrial parameter*/
		
		gbemg_stream_pcr_pid=GBEMG_INDX_CONT_EBM_PCR_PID(IndexTable);
		
		programinfolength=GBEMG_INDX_CONT_EBM_PROGRAM_INFO_LENGTH(IndexTable);
		
			
		for( i = 0, descriptor = Get_GBEMGDescriptor( IndexTable, programinfolength, i );
        		descriptor != NULL;
        	i++, descriptor = Get_GBEMGDescriptor( IndexTable, programinfolength, i ) )
        	{
        		if(descriptor[0]==GBEMG_CONT_TERRESTRIAL_TRANS_SYSTEM_DESCRIPTOR_TAG)
        		{
        			pemg_terl_param=GetTerrestrialParamFromDescriptor(descriptor);
        		
        			ret=InsertCaseIndexTerParam(pIndx,pemg_terl_param);
        		}
        	}
        
        /* now parse the datail channel info*/
        	IndexTable+=programinfolength;
        
        	pdetailchannel = (GBEMG_Detail_Channel_t*)CELTHOSL_MemoryAlloc(sizeof(GBEMG_Detail_Channel_t));
        
        	if(pdetailchannel==NULL)
        	{
        		ret=GBEMG_Err_NoMemory;
        		goto err;
        	}
        	
        	pdetailchannel->detail_channel_pcr_pid=gbemg_stream_pcr_pid;
        
        	ret=GBEMG_ParseSream(IndexTable,caselen,pdetailchannel);
        
        	SetCaseIndexDetailChannel(pIndx,pdetailchannel);
       	
        	InsertCaseIndex(pIndx);

			/* start search the content table of this*/

			if(Contentcallback)
			{
				if(EBCFreqIndicator==0x00)
				{
					ContPid=pIndx->pdetailchannel->other_pids[0];
				}

					Contentcallback(ContPid,emg_case_id);
				
			}
        
	}
		
	err:
	return ret;
	
}

#endif


U8 local_BuildCaseIndxandContFromIndexTable(U8* IndexTable,U32 TableLength,IndxGetContFromParamCallback SetupContcallback)
{
		GBEMG_Err ret=GBEMG_No_Err;
		
		U16 tablelen,caselen,emg_case_id,cover_area_num,j,programinfolength,EB_Program_Num,EB_Stream_id;
		
		U8 i, ebm_num,descriptornum,EBCFreqIndicator;
		
		U8* descriptor;
		
		U16 gbemg_stream_pcr_pid,ContPid;
		
		GBEMG_TYPE emg_case_type;
		GBEMG_URGENT_LEVEL emg_case_urgent_level;
		GBEMG_Time_t emg_issue_time;
		GBEMG_Time_Duration_t emg_duration;
		
		GBEMG_Area_t* parea;
		
		GBEMG_Terrestrial_Param_t* pemg_terl_param;
		
		GBEMG_Cover_Area_t cover_area;
		
		GBEMG_Detail_Channel_t* pdetailchannel;
		GBEMG_CaseIndex_t* pIndx;

		GBEMG_CaseContent_t* pCont;

		CelthApi_Debug_EnterFunction("local_BuildCaseIndxandContFromIndexTable");
	
		ret=IsValidIndexTable(IndexTable);
		if(ret!=GBEMG_No_Err)
		{
			CelthApi_Debug_LeaveFunction("local_BuildCaseIndxandContFromIndexTable");
			CelthApi_Debug_Err("\n++++invalid indextable get!\n");
			return ret;
		}
		
		
		
		tablelen = GBEMG_INDX_SECTION_LENGTH(IndexTable);
		
		if(tablelen>TableLength)
		{
			CelthApi_Debug_LeaveFunction("local_BuildCaseIndxandContFromIndexTable");
			CelthApi_Debug_Err("\n++++invalid indextable with length error!\n");
			return GBEMG_Err_Sectlength;
		}
	
		ContPid = GBEMG_PID;/* first set it as 0x21*/
	
		EBCFreqIndicator=GetEBCFreqIndicator(IndexTable);
		CelthApi_Debug_Msg("FreqIndicator=%x",EBCFreqIndicator);
		
		ebm_num= GetEBMNumber(IndexTable);
		CelthApi_Debug_Msg("ebm_num=%x\n",ebm_num);
		
		IndexTable+=GBEMG_INDX_EBM_GENERATE_LENGTH;
		
		for(i=0;i<ebm_num;i++)
		{
		
			pIndx=CreateNewCaseIndex();
			
			if(pIndx==NULL)
			{
				ret=GBEMG_Err_NoMemory;
				goto err;
				
			}
			caselen = GBEMG_INDX_CONT_EBM_LENGTH(IndexTable);
			CelthApi_Debug_Msg("caselen=%x\n",caselen);
			emg_case_id = GBEMG_INDX_CONT_EBM_ID(IndexTable);
			CelthApi_Debug_Msg("emg_case_id=%x\n",emg_case_id);
			emg_case_urgent_level = GBEMG_INDX_CONT_EBM_LEVEL(IndexTable);
			CelthApi_Debug_Msg("emg_case_urgent_level=%x\n",emg_case_urgent_level);
			
			GetGBEMGIssueTimeFromIndexTable(IndexTable+GBEMG_INDX_CONT_EBM_START_TIME_OFFSET_FROM_BASE,&emg_issue_time);
			
			GetGBEMGDurationTimeFromIndexTable(IndexTable+GBEMG_INDX_CONT_EBM_DURATION_TIME_OFFSET_FROM_BASE,&emg_duration);
			
			emg_case_type  = GetGBEMGType(IndexTable+GBEMG_INDX_CONT_EBM_TYPE_OFFSET_FROM_BASE);
			CelthApi_Debug_Msg("emg_case_type=%x\n",emg_case_type);
			
			ret=FillCaseIndex(pIndx,emg_case_id,emg_case_urgent_level,emg_case_type,&emg_issue_time,&emg_duration);
			
			
			/* now parse the cover area */ 
			cover_area_num = GBEMG_INDX_CONT_EBM_COVER_AREA_NUM(IndexTable);

			CelthApi_Debug_Msg("cover_area_num=%x\n",cover_area_num);
			
			cover_area.cover_area_number = cover_area_num;
			
			parea=(GBEMG_Area_t*)CELTHOSL_MemoryAlloc(sizeof(GBEMG_Area_t)*cover_area_num);
			
			if(parea==NULL)
			{
				CELTHOSL_MemoryFree(pIndx);
				ret=GBEMG_Err_NoMemory;
				goto err;
			}
			IndexTable+=GBEMG_INDX_CONT_EBM_TIME_COVER_AREA_LENGTH;	
			
			for(j=0;j<cover_area_num;j++)
			{
				GetEBMEGCoverArea(IndexTable,&parea[j]);
				IndexTable+=GBEMG_INDX_CONT_EBM_COVER_AREA_SEGMENT_LENGTH;
			}
			
			cover_area.pcover_area = parea;
			
			SetCaseIndexArea(pIndx,&cover_area);
			
			/*now fill the terrestrial parameter*/
			EB_Stream_id   = GBEMG_INDX_CONT_EBM_TRANS_STREAM_ID(IndexTable);
			EB_Program_Num = GBEMG_INDX_CONT_EBM_PROGRAM_NUM(IndexTable);
			
			gbemg_stream_pcr_pid=GBEMG_INDX_CONT_EBM_PCR_PID(IndexTable);
			
			programinfolength=GBEMG_INDX_CONT_EBM_PROGRAM_INFO_LENGTH(IndexTable);
			CelthApi_Debug_Msg("EB_Stream_id=%x,EB_Program_Num=%x\n",EB_Stream_id,EB_Program_Num);

			CelthApi_Debug_Msg("gbemg_pcr_pid=%x,programinfolength=%x\n",gbemg_stream_pcr_pid,programinfolength);

			IndexTable += GBEMG_INDX_CONT_EBM_TRANS_STREAM_DESCRIPTOR_OFFSET;
			
				
			for( i = 0, descriptor = Get_GBEMGDescriptor( IndexTable, programinfolength, i );
					descriptor != NULL;
				i++, descriptor = Get_GBEMGDescriptor( IndexTable, programinfolength, i ) )
				{
					if(descriptor[0]==GBEMG_CONT_TERRESTRIAL_TRANS_SYSTEM_DESCRIPTOR_TAG)
					{
						pemg_terl_param=GetTerrestrialParamFromDescriptor(descriptor);
					
						ret=InsertCaseIndexTerParam(pIndx,pemg_terl_param);
					}
				CelthApi_Debug_Msg("i=%x,descriptor=%x\n",i,descriptor[0]);

				}
			
			/* now parse the datail channel info*/
				IndexTable+=programinfolength;
			
				pdetailchannel = (GBEMG_Detail_Channel_t*)CELTHOSL_MemoryAlloc(sizeof(GBEMG_Detail_Channel_t));
			
				if(pdetailchannel==NULL)
				{
					ret=GBEMG_Err_NoMemory;
					goto err;
				}
				
				pdetailchannel->detail_channel_pcr_pid=gbemg_stream_pcr_pid;
			
				ret=GBEMG_ParseSream(IndexTable,caselen,tablelen,pdetailchannel);
			
				SetCaseIndexDetailChannel(pIndx,pdetailchannel);
			
				InsertCaseIndex(pIndx);
	
				/* start search the content table of this,and setup it.*/
	
				if(SetupContcallback)
				{
					
					if(EBCFreqIndicator==0x00)
					{
						ContPid=pIndx->pdetailchannel->other_pids[0].stream_pid;
					}

					CelthApi_Debug_Msg("Call SetupContcallback with Pid=%x\n",ContPid);	
	
					pCont=SetupContcallback(ContPid,emg_case_id);
					if(pCont)
					{
					CelthApi_Debug_Msg("Insert the content+++\n");	

					InsertCaseContent(pCont);
					}
					
				}
			
		}
			
		err:
		CelthApi_Debug_LeaveFunction("local_BuildCaseIndxandContFromIndexTable");
		return ret;
		
	}










  




