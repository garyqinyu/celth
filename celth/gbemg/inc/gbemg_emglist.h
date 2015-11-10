
#ifndef _GBEMG_EMGLIST_H_
#define _GBEMG_EMGLIST_H_




/* 
copyright reserved by 
celtech company Co Ltd,
*/


#ifdef __cplusplus
extern "C" {
#endif

#include "celth_list.h"

/*#include "gbemg_index.h"

#include "gbemg_content.h"*/



typedef struct GB_Node{

/*CELTHLST_S_ENTRY(GB_Node)link;*/

struct {struct GB_Node* sl_next;} link;




U16                emg_case_id;/* set it out of the pcaseindex and pcasecontent, make the search easy*/ 

GBEMG_CaseIndex_t* pcaseindex;

GBEMG_CaseContent_t* pcasecontent;

bool               bInfoComplete;

}GB_Node;


/*

typedef struct GB_Node_Head GB_Node_Head;


CELTHLST_S_HEAD(GB_Node_Head, GB_Node);
*/

typedef struct GB_Head{
GB_Node* sl_first;
}GB_Node_Head;


typedef struct GBEMG_IndexTable{

U8 indexnum;

GB_Node_Head*  list;

}GBEMG_IndexTable;




typedef struct GBEMG_ProcParam_s{
	GBEMG_CaseIndex_t* pcaseindex;
	
	GBEMG_CaseContent_t* pcasecontent;

}GBEMG_ProcParam_t;




void InitializeCaseIndexTable();



GBEMG_Err GetHeadCaseRunTime(GBEMG_Time_t* timer);

U8 GetTotalIndxNum();


U8 CompareCaseIndexTime(GBEMG_CaseIndex_t* OneIndx,GBEMG_CaseIndex_t* TwoIndx);



/* the following function is general used by outside */

void ResetCaseIndexTable();





GBEMG_Err InsertCaseIndex(GBEMG_CaseIndex_t* pIndx);	

GBEMG_Err InsertCaseContent(GBEMG_CaseContent_t* pCont);

GBEMG_Err GetHeadCaseInfo(GBEMG_ProcParam_t* pCaseParam );








#ifdef __cplusplus
}
#endif



#endif
