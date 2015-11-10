#ifndef _GBEMG_INDXPRIV_H_
#define _GBEMG_INDXPRIV_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/
 


#ifdef __cplusplus
extern "C" {
#endif








/* mjd time convert to year,month,day*/

void mjd_to_ymd(U32 mjd,U16 * year,U8 * month,U8 * day,U8 * week_day);

/* year, month,day convert to mjd*/

U32 ymd_to_mjd(U16 y, U8 m, U8 d);



void bcd_to_hms(U8 * time,	U8 * hour, U8 * min, U8 * sec);
void bcd_to_hm(U8 * time,	U8 * hour, U8 * min);


U8* Get_GBEMGDescriptor(U8* TableSegment,U16 SegmentSize, int descriptorNum);

U8* GBEMG_getStreamDescriptor( U8* TableSegment, U16 SegmentSize, int streamNum, int descriptorNum );

int GBEMG_getNumStreams( U8 *TableSegment, U16 SegmentSize,U16 IndexTableLen);

U8 GBEMG_getStream( U8 *TableSegment, U16 SegmentSize, int streamNum, GBEMG_STREAM_t *p_stream );



U8 GBEMG_ParseSream(U8 *TableSegment, U16 SegmentSize,U16 SectionLen,GBEMG_Detail_Channel_t* pchannel);

GBEMG_Terrestrial_Param_t* GetTerrestrialParamFromDescriptor(U8* descripor);



GBEMG_TYPE  GetGBEMGType(U8* tableSegment);

void GetGBEMGIssueTimeFromIndexTable(U8* TableSegment,GBEMG_Time_t* pissueTime);

void GetGBEMGDurationTimeFromIndexTable(U8* TableSegment,GBEMG_Time_Duration_t* pdurationtime);

void GetEBMEGCoverArea(U8* TableSegment,GBEMG_Area_t* parea);

//GBEMG_Detail_Channel_t*    GetGBEMGDetailChannelFromGBEMGIndexTable(U8* TableSegment); 




#ifdef __cplusplus
}
#endif

#else
#error "Multiple inclusions of gbemg_indexpriv.h"

#endif
