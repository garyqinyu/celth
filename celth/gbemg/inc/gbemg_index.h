#ifndef _GBEMG_INDEX_H_
#define _GBEMG_INDEX_H_



/* 
copyright reserved by 
celtech company Co Ltd,
*/


#ifdef __cplusplus
extern "C" {
#endif


typedef struct GBEMG_CaseIndex_s{

U16 emg_case_id;

GBEMG_URGENT_LEVEL emg_case_urgent_level;

GBEMG_TYPE   emg_case_type;

GBEMG_Time_t emg_issue_time;

GBEMG_Time_Duration_t emg_duration;

GBEMG_Terl_Param_Node* pemg_terl_head;

U8                      detailchannel_num;

GBEMG_Detail_Channel_t*       pdetailchannel;

GBEMG_Cover_Area_t*     pemg_case_cover_area;


} GBEMG_CaseIndex_t;



/* the following function is create and build caseindex function*/


GBEMG_CaseIndex_t* CreateNewCaseIndex();


U8 FillCaseIndex(GBEMG_CaseIndex_t* pIndx,
			U16 emg_case_id,
			GBEMG_URGENT_LEVEL emg_case_urgent_level,
			GBEMG_TYPE   emg_case_type,
			GBEMG_Time_t* emg_issue_time,
			GBEMG_Time_Duration_t* emg_duration);


U8 SetCaseIndexArea(GBEMG_CaseIndex_t* pIndx,GBEMG_Cover_Area_t* p_area);

U8 InsertCaseIndexTerParam(GBEMG_CaseIndex_t* pIndx,GBEMG_Terrestrial_Param_t* p_param);

U8 SetCaseIndexDetailChannel(GBEMG_CaseIndex_t* pIndx,GBEMG_Detail_Channel_t*  pdetailchannel);




/* the following is analyze the indextable function*/ 

U8 IsValidIndexTable(U8* indextable);

U8 GetIndexTableVer(U8* indextable);

U8 GetCurrentTableSectionNum(U8* indextable);
U8 GetLastTableSectionNum(U8* indextable);
U8 GetEBCFreqIndicator(U8* indextable);
U8 GetEBMNumber(U8* indextable);


U8 BuildCaseIndexFromIndexTable(U8* IndexTable,U32 TableLength);




/* the following function is the manage caseindex list function */




		

/*void    DeleteCaseIndex(U16 emg_case_id);*/



#ifdef __cplusplus
}
#endif



#endif


