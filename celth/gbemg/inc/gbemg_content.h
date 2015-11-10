#ifndef _GBEMG_CONTENT_H_
#define _GBEMG_CONTENT_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/
 


#ifdef __cplusplus
extern "C" {
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++ABOUT THE URGENT CONTENT TABLE+++++++++++++++++++++++++++++++++++*/

/* The GB urgent data content table is  transfered through two methods:*/

/* 1: through the PID 0x21 as if GB urgent index table .*/

/* In this situation, the index table's item:EBC_current_frequency_indicator*/
/* segment will put as 1. (See the regularation of GB emergence 5.2 section)*/

/* 2: through the detail channel's PID.*/

/* It this situation , the index table's item:EBC_current_frequency_indicator*/
/* segment will put as 0.(see the regularation of GB emergence 5.2 section)*/
/* Mainwhile , the detail channel PID will been told by PMT table.*/
/* In the PMT table if there are  stream_type set as 5, it mean the stream PID is the GB*/
/* urgent content table's transfer PID, named as detail channel PID. */
/* (See the regularation of GB emergence 6.2 section) */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++END++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* described by the GB emgence 5.3 section */

/*
 everytime ,the GB system just sent less then two type the GB emgence auxiliary data.

one audio aux data with a graphics data (that is two type data)

or one av data. (that is one type data)

or one audio data .(thata is one type data)
*/



typedef struct GBEMG_Content_Description_s
{
	GBEMG_MULTILINGUAL_TYPE language_type;
	
	U8   agency_name_length;
	
	U8   agency_name[256];
	
	U16  content_msg_length;
	
	U8*  pcontent_msg_buffer;
	
	U8   auxiliary_data_num;
	
	GBEMG_AuxData_t*             pemg_auxiliary_data;
	
	
}GBEMG_Content_Description_t;


typedef struct GBEMG_CaseContent_s
{
	U16 emg_case_id;
	
	U8 emg_multilingual_description_num;
	
	GBEMG_Content_Description_t* pemg_content_description;
	
	/*GBEMG_AuxData_t*             pemg_auxiliary_data;*/
	
}GBEMG_CaseContent_t;

typedef GBEMG_CaseContent_t* pGBEMG_CaseContent_t;

typedef pGBEMG_CaseContent_t (*IndxGetContFromParamCallback)(U16 ContentPid,U16 EBMid );





GBEMG_CaseContent_t* GetContFromParam(U16 contPid,U16 Ebm_id);




#ifdef __cplusplus
}
#endif



#endif

