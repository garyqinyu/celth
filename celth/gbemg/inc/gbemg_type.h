#ifndef _GBEMG_TYPE_H_
#define _GBEMG_TYPE_H_


#ifdef __cplusplus
extern "C" {
#endif



typedef enum GBEMG_TYPE_e
{
	Test=(U32)0x00000,

	
}GBEMG_TYPE;



typedef struct GBEMG_Type_Description_s
{
   GBEMG_TYPE emg_type;
   
   char   eng_description[64];
   char   cha_description[128];	
	
}GBEMG_Type_Description_t;


void Build_EmgDescription_Table();

char* Get_EmgEnglishDescription(GBEMG_TYPE emg_type);

char* Get_EmgChineseDescription(GBEMG_TYPE emg_type);




#ifdef __cplusplus
}
#endif



#endif

