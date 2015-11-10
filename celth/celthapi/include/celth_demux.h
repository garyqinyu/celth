#ifndef __CELTH_DEMUX_H
#define __CELTH_DEMUX_H

#ifdef __cplusplus

extern "C" {

#endif

#include "celth_stddef.h"


#define  CELTH_DEMUX_MAX_SECTION_FILTERS		32


#define CELTH_FILTER_SIZE  16

typedef unsigned short CELTH_PID ;


typedef CELTH_VOID*	CELTHDemux_SectionFilterHandle_t;




typedef enum CELTH_DemuxMessageFormat
{
    CELTH_DemuxMessageFormat_ePsi,       /* Filter Program Specific Information from an MPEG2 Transport stream */
    CELTH_DemuxMessageFormat_ePes,       /* PES data capture. Captures PES packets with respect to PES packet length. */
    CELTH_DemuxMessageFormat_ePesSaveAll,/* PES data capture. Captures entire payload without respect to PES packet length. */
    CELTH_DemuxMessageFormat_eTs,        /* MPEG2 Transport data capture */
    CELTH_DemuxMessageFormat_eMax
} CELTH_DemuxMessageFormat;



typedef struct Celth_Demux_SectionFilter_s{

    CELTH_BYTE mask[CELTH_FILTER_SIZE];        
    CELTH_BYTE coefficient[CELTH_FILTER_SIZE]; 
    CELTH_BYTE exclusion[CELTH_FILTER_SIZE];   


}Celth_Demux_SectionFilter_s; 

typedef	CELTH_VOID (* CELTHDemux_Callback_t)( CELTHDemux_SectionFilterHandle_t hHandle,CELTH_UINT iParam, CELTH_VOID* pParam);



typedef struct Celth_DemuxSectionCallback_s
{
    CELTH_VOID  ( *fDetectCallback )( CELTH_UINT iParam, CELTH_VOID* pParam );
    CELTH_VOID  ( *fErrorCallback )(  CELTH_VOID );
} Celth_DemuxSectionCallback_s;



CELTH_ErrorCode_t	CelthApi_Demux_SectionInitialize( CELTH_VOID );




CELTH_ErrorCode_t	CelthApi_Demux_AllocateSectionFilter( Celth_DemuxSectionCallback_s* pFilterCallback, CELTHDemux_SectionFilterHandle_t  *phFilterHandle );

CELTH_ErrorCode_t	CelthApi_Demux_FreeSectionFilter(  CELTHDemux_SectionFilterHandle_t hFilterHandle );

CELTH_ErrorCode_t	CelthApi_Demux_SetSectionFilterCallback(  CELTHDemux_SectionFilterHandle_t hFilterHandle,Celth_DemuxSectionCallback_s* pFilterCallback );

CELTH_ErrorCode_t	CelthApi_Demux_SetSectionFilterCallbackParam(  CELTHDemux_SectionFilterHandle_t hFilterHandle,CELTH_VOID* pparam );
CELTH_ErrorCode_t   CelthApi_Demux_SetSectionFilterBigBufParam(CELTHDemux_SectionFilterHandle_t hFilterHandle,CELTH_INT iBufSize,CELTH_BOOL bBig);/* this function must call before CelthApi_Demux_SetSectionFilter,but after CelthApi_Demux_SetSectionFilterPid*/


CELTH_ErrorCode_t	CelthApi_Demux_SetSectionFilterPid( CELTHDemux_SectionFilterHandle_t hFilterHandle, CELTH_PID wPid );
CELTH_ErrorCode_t	CelthApi_Demux_SetSectionFilter( CELTHDemux_SectionFilterHandle_t hFilterHandle, Celth_Demux_SectionFilter_s* pstFilterData );

/* the following two functions must use together or use the CelthApi_Demux_GetSection to get section */

CELTH_BOOL	CelthApi_Demux_ReadSectionLength( CELTHDemux_SectionFilterHandle_t hFilterHandle, CELTH_UINT* puSectionLength);
CELTH_ErrorCode_t	CelthApi_Demux_ReadSectionData( CELTHDemux_SectionFilterHandle_t hFilterHandle, CELTH_BYTE* pucSectionData,CELTH_INT ireadLength );

/* max read length is the databuffer's length, if the data bufer's length is smaller then the section length, need read the data again with funciton  CelthApi_Demux_ReadSectionData*/

/* the return value of the function is the left data of the demux section buffer*/

CELTH_INT  CelthApi_Demux_GetSection(CELTHDemux_SectionFilterHandle_t hFilterHandle,CELTH_BYTE* pucSectionData,CELTH_UINT* puSectionLength,CELTH_UINT maxReadLength );


CELTH_ErrorCode_t	CelthApi_Demux_DisableSectionFilter( CELTHDemux_SectionFilterHandle_t hFilterHandle );
CELTH_ErrorCode_t	CelthApi_Demux_EnableSectionFilter( CELTHDemux_SectionFilterHandle_t hFilterHandle );






CELTH_VOID	CelthApi_Demux_SectionError( CELTHDemux_SectionFilterHandle_t hFilterHandle );
CELTH_VOID	CelthApi_Demux_SectionDetect( CELTHDemux_SectionFilterHandle_t hFilterHandle,CELTH_UINT* pwSectionLength );


CELTH_VOID  CelthApi_Demux_SectionSetDefaultFilterSettings(Celth_Demux_SectionFilter_s* pstFilterData );





#ifdef __cplusplus

}

#endif






#endif


