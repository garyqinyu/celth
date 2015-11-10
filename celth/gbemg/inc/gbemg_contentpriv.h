#ifndef _GBEMG_CONTPRIV_H_
#define _GBEMG_CONTPRIV_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/
 


#ifdef __cplusplus
extern "C" {
#endif



bool IsValidContentTable(U8* SegTable);

GBEMG_CaseContent_t* GBEMG_BuildContFromTable(U8* ContTable,U32 TableLength);

GBEMG_CaseContent_t* GBEMG_BuildContFromMultiTable(U8* MultiContTable,U32 MultiTableLength);




#ifdef __cplusplus
}
#endif

#else
#error "Multiple inclusions of gbemg_contentpriv.h"

#endif

