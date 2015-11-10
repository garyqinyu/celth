#ifndef _GBEMG_STDDEF_H_
#define _GBEMG_STDDEF_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/


#ifdef __cplusplus
extern "C" {
#endif



#define bool unsigned char
#define true 1
#define false 0




typedef  unsigned char  U8;
typedef unsigned short  U16;
typedef unsigned long   U32;

typedef int           I32;


/*typedef unsigned char uint8_t;

typedef unsigned short uint16_t;

typedef unsigned int   uint32_t;

*/

#ifndef uint8_t
#define uint8_t unsigned char 
#endif

#ifndef uint16_t
#define uint16_t unsigned short
#endif

#ifndef uint32_t
#define uint32_t unsigned int 
#endif




typedef enum GBEMG_Err_s{
  GBEMG_No_Err=0,
  GBEMG_Err_InvalidHandle,	
  GBEMG_Err_InvalidParam,
  GBEMG_Err_NoMemory,
  GBEMG_Err_TimeOut,
  GBEMG_Err_Sectlength,
  GBEMG_Err_SectData,
  GBEMG_Err_InvalidInfo,	
  GBEMG_Err_Unknown,
  
	
}GBEMG_Err;













#ifdef __cplusplus
}
#endif




#endif
