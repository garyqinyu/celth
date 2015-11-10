
#include "celth_stddef.h"


#include "celth_rtos.h"
#include "celth_mutex.h"


/*#include "celth_list.h"*/

#include "celth_debug.h"

#include "gbemg_stddef.h"
#include "gbemg_gendef.h"

#include "gbemg_type.h"
#include "gbemg_index.h"
#include "gbemg_content.h"

#include "celth_list.h"

#include "gbemg_emglist.h"





static GBEMG_IndexTable gbemg_indextable;

static GB_Node_Head  gbemg_hnode;
static CELTHOSL_SemId_t  gbemg_tablesem;



static GBEMG_Err InsertCaseIndexToIndexTable(GBEMG_CaseIndex_t* pIndx,GBEMG_IndexTable table);

static GBEMG_Err InsertCaseContToIndexTable(GBEMG_CaseContent_t* pCont,GBEMG_IndexTable table);

static bool CompareTime(GBEMG_Time_t atimer, GBEMG_Time_t btimer);
static bool EqualTime(GBEMG_Time_t atimer, GBEMG_Time_t btimer);


/* the function of manage the caseindex*/

static bool CompareTime(GBEMG_Time_t atimer, GBEMG_Time_t btimer)
{
	return true;
} 

static bool EqualTime(GBEMG_Time_t atimer, GBEMG_Time_t btimer)
{
	return true;
}


U8 CompareCaseIndexTime(GBEMG_CaseIndex_t* OneIndx,GBEMG_CaseIndex_t* TwoIndx)
{
	U8 ret=0x00;
	
	if(OneIndx==NULL||TwoIndx==NULL)
	{
		ret=0x00;
		
	}
	else
	{
		if(CompareTime(OneIndx->emg_issue_time,TwoIndx->emg_issue_time))
		{
			ret=0x01;
		}
		else
		{
			if(EqualTime(OneIndx->emg_issue_time,TwoIndx->emg_issue_time))
			{
				if(OneIndx->emg_case_urgent_level>TwoIndx->emg_case_urgent_level)
				{
					ret=0x02;
				}
				else
				{
					ret=0xFE;
				}
			}
			ret=0xFF;
		}
	}
	
	return ret;
	
}




GBEMG_Err InsertCaseIndexToIndexTable(GBEMG_CaseIndex_t* pIndx,GBEMG_IndexTable table)
{
	
	U8 comret;
	GBEMG_CaseIndex_t* pCompareIndx;
	
	GB_Node* pLastNode;
	GB_Node* pCurNode;
	GB_Node* pTempNode;
	GB_Node* pNode;
	GB_Node* pHead;
	CelthApi_Debug_EnterFunction("InsertCaseIndexToIndexTable");
	
	if(pIndx==NULL)
	{
		return GBEMG_Err_InvalidParam;
	}	
	pNode=(GB_Node*)CELTHOSL_MemoryAlloc(sizeof(GB_Node));
	
	if(pNode)
	{
		pNode->emg_case_id=pIndx->emg_case_id;
		pNode->pcaseindex=pIndx;
		pNode->bInfoComplete=false;
		CELTHLST_INIT_NEXT(pNode,link);


		CelthApi_Debug_Msg("\npNode=%x,link=%x\n",pNode,pNode->link);
	}
	else
	{
		return GBEMG_Err_NoMemory;
	}
	
	table.indexnum++;
	
	pHead=CELTHLST_S_FIRST(table.list);



	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\nInsertCaseIndexToIndexTable TABLE.LIST=%x\n",table.list);
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	

	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");

	CelthApi_Debug_Msg("\npHead=%x\n",pHead);
	
	if(pHead==NULL)
	{
		
		CELTHLST_S_INSERT_HEAD(table.list, pNode, link);
		pHead=CELTHLST_S_FIRST(table.list);	
		CelthApi_Debug_Msg("insert the indx to the head %x\n",pHead);
		return GBEMG_No_Err;
	}
	

	
	for(pLastNode=NULL,pCurNode=CELTHLST_S_FIRST(table.list);pCurNode;pCurNode=CELTHLST_S_NEXT(pCurNode,link))
	{
		
		pLastNode=pCurNode;
		
		if(pLastNode)
		{
			pCompareIndx=pLastNode->pcaseindex;
				
			comret=CompareCaseIndexTime(pIndx,pCompareIndx);
		
			if(comret==0xFF||comret==0xFE)
			{
			CELTHLST_S_INSERT_AFTER(table.list,pLastNode,pNode,link);
			break; 
			}
			
		}
		else
		{
			CELTHLST_S_INSERT_AFTER(table.list,pTempNode,pNode,link);
		}
		
		pTempNode=pCurNode;
	
	}
	CelthApi_Debug_LeaveFunction("InsertCaseIndexToIndexTable");
	return GBEMG_No_Err;

}


GBEMG_Err InsertCaseContToIndexTable(GBEMG_CaseContent_t* pCont,GBEMG_IndexTable table)
{
		
	GB_Node* pCurNode;
	GB_Node* pNode;
	
	GBEMG_Err ret=GBEMG_Err_Unknown;

	CelthApi_Debug_EnterFunction("InsertCaseContToIndexTable");
	
	if(pCont==NULL)
	{
		return GBEMG_Err_InvalidParam;
	}

	
	pNode=CELTHLST_S_FIRST(table.list);
	
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\nInsertCaseContToIndexTable TABLE.LIST=%x\n",table.list);
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");

	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\npHead=%x\n",pNode);
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	
	
	if(pNode==NULL)
	{
		CelthApi_Debug_Msg("No pNode, exit from InsertCaseContToIndexTable!\n ");
		return GBEMG_Err_InvalidHandle;
	}
	
	for(pCurNode=CELTHLST_S_FIRST(table.list);pCurNode;pCurNode=CELTHLST_S_NEXT(pCurNode,link))
	{
		
		if(pCurNode->emg_case_id==pCont->emg_case_id)
		{
			pCurNode->pcasecontent=pCont;
			pCurNode->bInfoComplete=true;
			ret=GBEMG_No_Err;
			break;
		}

	}
	CelthApi_Debug_LeaveFunction("InsertCaseContToIndexTable");
	return ret;
		
}

static GBEMG_Err GetHeadCaseFromIndexTable(GBEMG_ProcParam_t* pCaseParam,GBEMG_IndexTable table )
{
	GBEMG_Err ret=GBEMG_No_Err;
	
	GB_Node* pRemoveNode;
	
	/* just remove the head node, it is the emergence regulation request*/

	CelthApi_Debug_EnterFunction("GetHeadCaseFromIndexTable");
	pRemoveNode=CELTHLST_S_FIRST(table.list);


	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\nGetHeadCaseFromIndexTable TABLE.LIST=%x\n",table.list);
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	

	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\npRemoveNode=%x\n",pRemoveNode);
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");

	if(pCaseParam==NULL)
	{
		return GBEMG_Err_InvalidParam;
	}
	
	if(pRemoveNode==NULL)
	{
		return GBEMG_Err_InvalidHandle;
	}
	else
	{
	     if(pRemoveNode->bInfoComplete==true)
		{		
	  		if(pCaseParam->pcaseindex!=NULL&&pRemoveNode->pcaseindex!=NULL)
			{
	  		CELTHOSL_Memcpy(pCaseParam->pcaseindex,pRemoveNode->pcaseindex,sizeof(GBEMG_CaseIndex_t));
			}
	  		if(pCaseParam->pcasecontent!=NULL&&pRemoveNode->pcasecontent!=NULL)
			{
	  		CELTHOSL_Memcpy(pCaseParam->pcasecontent,pRemoveNode->pcasecontent,sizeof(GBEMG_CaseContent_t));
			}
			CELTHLST_S_REMOVE_HEAD(table.list,link);

			CELTHOSL_MemoryFree(pRemoveNode);

			table.indexnum--;
		}
		else
		{
			ret=GBEMG_Err_InvalidInfo;
		}
	}
	

	CelthApi_Debug_LeaveFunction("GetHeadCaseFromIndexTable");
	
	return ret;
	
}




/* index table function */



void InitializeCaseIndexTable()
{
	CelthApi_Debug_EnterFunction("InitializeCaseIndexTable");
	gbemg_indextable.indexnum=0;
	
	CelthApi_Debug_Msg("init the list!\n");

	
	gbemg_indextable.list = &gbemg_hnode;

	CelthApi_Debug_Msg("\nINIT TABLE.LIST=%x\n",gbemg_indextable.list);
	CELTHLST_S_INIT(gbemg_indextable.list);

	CelthApi_Debug_Msg("Create the emgtablesem\n");
	gbemg_tablesem=CELTH_SemCreate( "emgtablesem", CELTHOSL_FifoNoTimeOut, 1 );

	CelthApi_Debug_LeaveFunction("InitializeCaseIndexTable");
}




GBEMG_Err GetHeadCaseRunTime(GBEMG_Time_t* timer)
{

	
	CelthApi_Debug_EnterFunction("GetHeadCaseRunTime");
	GB_Node* pHeadNode;

	if(timer==NULL)
		{
		return GBEMG_Err_InvalidParam;
		}

	
	pHeadNode=CELTHLST_S_FIRST(gbemg_indextable.list);


	
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\nGetHeadCaseRunTime TABLE.LIST=%x\n",gbemg_indextable.list);
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\pHeadNode=%x\n",pHeadNode);
	CelthApi_Debug_Msg("\n");
	CelthApi_Debug_Msg("\n");

	if(pHeadNode==NULL)
		{
		CelthApi_Debug_Msg("GetHeadCaseRunTime return with no headnode\n");
		return GBEMG_Err_InvalidHandle;
		}


		CELTHOSL_Memcpy(timer,&pHeadNode->pcaseindex->emg_issue_time,sizeof(GBEMG_Time_t));
		
	CelthApi_Debug_LeaveFunction("GetHeadCaseRunTime");
	return GBEMG_No_Err;
	
}





GBEMG_Err InsertCaseContent(GBEMG_CaseContent_t* pCont)
{
	GBEMG_Err ret;

	
	CELTH_SemWait( gbemg_tablesem, CELTHOSL_WaitForEver );
	
	ret=InsertCaseContToIndexTable(pCont,gbemg_indextable);

	CELTH_SemPost(gbemg_tablesem);

	return ret;

	

}


GBEMG_Err InsertCaseIndex(GBEMG_CaseIndex_t* pIndx)
{
	GBEMG_Err ret;
	CELTH_SemWait( gbemg_tablesem, CELTHOSL_WaitForEver );
	
	ret=InsertCaseIndexToIndexTable(pIndx,gbemg_indextable);

	
	CELTH_SemPost(gbemg_tablesem);

	return ret;

	
}

GBEMG_Err GetHeadCaseInfo(GBEMG_ProcParam_t* pCaseParam )
{
	GBEMG_Err ret;
	
	CELTH_SemWait( gbemg_tablesem, CELTHOSL_WaitForEver );
    ret=GetHeadCaseFromIndexTable(pCaseParam,gbemg_indextable);
	
	CELTH_SemPost(gbemg_tablesem);

	return ret;
}

void ResetCaseIndexTable()
{
	CelthApi_Debug_EnterFunction("ResetCaseIndexTable");

	CELTH_SemWait( gbemg_tablesem, CELTHOSL_WaitForEver );
	
	
	CelthApi_Debug_Msg("reset the gbemg_indextable.list\n");
	gbemg_indextable.indexnum=0;
	CELTHLST_S_INIT(gbemg_indextable.list);

	CELTH_SemPost(gbemg_tablesem);
	CelthApi_Debug_LeaveFunction("ResetCaseIndexTable");
}



/*GBEMG_CaseIndex_t* RemoveCaseIndex()
{
	return RemoveHeadCaseIndexFromIndexTable(gbemg_indextable);
}
*/

#if 0
U8 GetGBEMGProcParamFromTable(GBEMG_ProcParam_t* pParam,GBEMG_IndexTable table)
{

	
		
	GB_Node* pRemoveNode;

	if(pParam==NULL)
	{
		return GBEMG_Err_InvalidParam;
	}
	
	/* just remove the head node, it is the emergence regulation request*/
	pRemoveNode=CELTHLST_S_FIRST(table.list);
	
	if(pRemoveNode==NULL)
	{
		return GBEMG_Err_InvalidHandle;
	}
	else
	{
	   pParam->pcaseindex=pRemoveNode->pcaseindex;	
	   pParam->pcasecontent=pRemoveNode->pcasecontent;
	}
	
	CELTHLST_S_REMOVE_HEAD(table.list,link);

	table.indexnum--;
	
	return GBEMG_No_Err ;
}



U8 GetGBEMGProcParam(GBEMG_ProcParam_t* pParam)
{
	return GetGBEMGProcParamFromTable(pParam,gbemg_indextable);
}
#endif

static U8 GetIndxNumFromIndexTable(GBEMG_IndexTable table)
{
    return table.indexnum;
}


U8 GetTotalIndxNum()
{
	return GetIndxNumFromIndexTable(gbemg_indextable);
}




