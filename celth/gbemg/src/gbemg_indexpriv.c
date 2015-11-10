
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






#define ADD_VIDEO_PID(P_DETAILCHANNEL, PID, TYPE,INDEX) \
    do { \
    if( (P_DETAILCHANNEL)->num_video_pids < MAX_GBEMG_DETAIL_PROGRAM_PIDS ) \
    { \
    (P_DETAILCHANNEL)->video_pids[(P_DETAILCHANNEL)->num_video_pids].stream_pid = (PID); \
    (P_DETAILCHANNEL)->video_pids[(P_DETAILCHANNEL)->num_video_pids].stream_type = (TYPE); \
    (P_DETAILCHANNEL)->video_pids[(P_DETAILCHANNEL)->num_video_pids].av_type = GBEMG_VIDEO;\
    (P_DETAILCHANNEL)->num_video_pids++; \
    } \
    } while (0)
#define ADD_AUDIO_PID(P_DETAILCHANNEL, PID, TYPE,INDEX) \
    do { \
    if( (P_DETAILCHANNEL)->num_audio_pids < MAX_GBEMG_DETAIL_PROGRAM_PIDS ) \
    { \
    (P_DETAILCHANNEL)->audio_pids[(P_DETAILCHANNEL)->num_audio_pids].stream_pid = (PID); \
    (P_DETAILCHANNEL)->audio_pids[(P_DETAILCHANNEL)->num_audio_pids].stream_type = (TYPE); \
    (P_DETAILCHANNEL)->audio_pids[(P_DETAILCHANNEL)->num_video_pids].av_type = GBEMG_AUDIO;\
    (P_DETAILCHANNEL)->num_audio_pids++; \
    } \
    } while (0)
#define ADD_OTHER_PID(P_DETAILCHANNEL, PID, TYPE,INDEX) \
    do { \
    if( (P_DETAILCHANNEL)->num_other_pids < MAX_GBEMG_DETAIL_PROGRAM_PIDS ) \
    { \
    (P_DETAILCHANNEL)->other_pids[(P_DETAILCHANNEL)->num_other_pids].stream_pid = (PID); \
    (P_DETAILCHANNEL)->other_pids[(P_DETAILCHANNEL)->num_other_pids].stream_type = (TYPE); \
    (P_DETAILCHANNEL)->other_pids[(P_DETAILCHANNEL)->num_video_pids].av_type = GBEMG_OTHER;\
    (P_DETAILCHANNEL)->num_other_pids++; \
    } \
    } while (0)





static int GBEMG_P_getStreamByteOffset( U8 *buf, U16 bfrSize, int streamNum )
{
	int byteOffset;
	int i;

	/* After the last descriptor */
	byteOffset = 0;

	for (i=0; i < streamNum; i++)
	{
		if (byteOffset >= (int)bfrSize)
			return -1;
		byteOffset += 5 + (TS_READ_16( &buf[byteOffset+3] ) & 0xFFF);
	}

	return byteOffset;
}








/* mjd time convert to year,month,day*/

void mjd_to_ymd(U32 mjd,U16 * year,U8 * month,U8 * day,U8 * week_day)
{
	
	static U32 pre_mjd = 0xFFFFFFFF;
	static U16 pre_year;
	static U8  pre_month;
	static U8 pre_day;
	static U8 pre_week_day;
	U32 Y,M,D,K;

	if (mjd <= 15078)   /*fix bug:stream's mjd error*/
        mjd = 53371;    /*set to default:2005/1/1*/

	if (mjd == pre_mjd)
	{
		*year = pre_year;
		*month = pre_month;
		*day = pre_day;
		*week_day = pre_week_day;
	}
	else
	{/*according to MJD formula, use integer instead of float point*/
		Y = (20*mjd - 301564) / 7305;
		U32 YY = (Y*365 + Y/4);	/*(UINT32)(Y * 365.25)*/
		M = 1000*(10*mjd - 149561 - 10*YY) / 306001;
		D = mjd - 14956 - YY - M * 306001 / 10000;

		if(M == 14 || M == 15)
			K = 1;
		else 
			K = 0;
			
		*year = (U16)(Y + K + 1900);
		*month = M - 1 - K * 12;
		if (*month > 12)
			*month = 1;
		*day = D;
		if (*day > 31)
			*day = 1;	
		*week_day = ((mjd + 2) % 7) + 1;

		/*record it*/
		pre_mjd = mjd;
		pre_year = *year;
		pre_month = *month;
		pre_day = *day;
		pre_week_day = *week_day;
	}

	
	
}

/* year, month,day convert to mjd*/

U32 ymd_to_mjd(U16 y, U8 m, U8 d)
{
	signed char l;
	short Y=y;
	
	if (m == 1 || m== 2)
		l = 1;
	else
		l = 0;
	
	Y -= 1900;
	y=(Y < 0)?0:Y;
		
/*	return (14956 + d + (UINT32)((y -l) * 365.25) + (UINT32)(((m + 1 + l * 12) * 30.6001)));*/
	U32 yl = (y -l);
	U32 yy = yl * 365 + yl / 4;

	return (14956 + d + yy + (U32)(((m + 1 + l * 12) * 306001)/10000));
	
}


/* Convert BCD code to hour, min, sec */
void bcd_to_hms(U8 * time,	U8 * hour, U8 * min, U8 * sec)
{
	*hour = (time[0] >> 4) * 10 + (time[0] & 0x0f);
	*min = (time[1] >> 4) * 10 + (time[1] & 0x0f);
	*sec = (time[2] >> 4) * 10 + (time[2] & 0x0f);
}
/* Convert BCD code to hour, min, */
 void bcd_to_hm(U8 * time,	U8 * hour, U8 * min)
{
	*hour = (time[0] >> 4) * 10 + (time[0] & 0x0f);
	*min = (time[1] >> 4) * 10 + (time[1] & 0x0f);
}






U8* Get_GBEMGDescriptor(U8* TableSegment,U16 SegmentSize, int descriptorNum)
{

	U8* descriptor;
	int i;
	U16 byteOffset = 0;

	descriptor = NULL;

CelthApi_Debug_Msg("SegmentSize=%x,num=%x\n",SegmentSize,descriptorNum);

	

	for( i = 0; byteOffset < SegmentSize; i++ )
	{
		descriptor = &TableSegment[byteOffset];
		
		
		byteOffset += TableSegment[byteOffset+1] + 2;

		CelthApi_Debug_Msg("descriptor=%x,byteOffset=%x\n",descriptor,byteOffset);

		if( i == descriptorNum ) 
			break;
		else 
			descriptor = NULL;
	}

	return descriptor;
}


U8* GBEMG_getStreamDescriptor( U8* TableSegment, U16 SegmentSize, int streamNum, int descriptorNum )
{
	int byteOffset;

	byteOffset = GBEMG_P_getStreamByteOffset( TableSegment, SegmentSize, streamNum );
	if (byteOffset == -1)
		return NULL;

	return (Get_GBEMGDescriptor(&TableSegment[byteOffset+5], TS_READ_16(&TableSegment[byteOffset+3])&0xFFF, descriptorNum ));
}






int GBEMG_getNumStreams( U8 *TableSegment, U16 SegmentSize,U16 IndexTableLen)
{
	int byteOffset=0;
	int i = 0;

	CelthApi_Debug_EnterFunction("GBEMG_getNumStreams");
	while (byteOffset <(int) IndexTableLen && byteOffset < (int)SegmentSize)
	{
		byteOffset += 5 + (TS_READ_16( &TableSegment[byteOffset+3] ) & 0xFFF);
	/*CelthApi_Debug_Msg("byteOffset=%x\n",byteOffset);*/
		i++;
	}

	

	CelthApi_Debug_LeaveFunction("GBEMG_getNumStreams");

	return i;
}
	
U8 GBEMG_getStream( U8 *TableSegment, U16 SegmentSize, int streamNum, GBEMG_STREAM_t *p_stream )
{
	
	int byteOffset;

CelthApi_Debug_EnterFunction("GBEMG_getStream");

	byteOffset = GBEMG_P_getStreamByteOffset( TableSegment, SegmentSize, streamNum );
	if (byteOffset == -1)
	{
		CelthApi_Debug_Err("byteOffset=%x\n",byteOffset);
		return 0x03;
	}

	p_stream->stream_type = TableSegment[byteOffset];
	p_stream->stream_pid = (U16)(TS_READ_16( &TableSegment[byteOffset+1] ) & 0x1FFF);

	
	

	CelthApi_Debug_LeaveFunction("GBEMG_getStream");

	return 0x00;
}	
	


U8 GBEMG_ParseSream(U8 *TableSegment, U16 SegmentSize,U16 SectionLen,GBEMG_Detail_Channel_t* pchannel)	
{
	
    int i;
    U8 ret=0x00;
    GBEMG_STREAM_t gbemg_stream;

CelthApi_Debug_EnterFunction("GBEMG_ParseSream");
    
    if(pchannel==NULL)
    {
    	ret=0x01;
    	return ret;
    }
    
    pchannel->num_video_pids=0;
    pchannel->num_audio_pids=0;
    pchannel->num_other_pids=0;
 
    for( i = 0; i < GBEMG_getNumStreams(TableSegment, SegmentSize, SectionLen); i++ )
    {
        int descIdx = 0;
        if (GBEMG_getStream(TableSegment, SegmentSize, i, &gbemg_stream )) {
            CelthApi_Debug_Err("Invalid GB EMG stream data detected");
            continue;
        }
	CelthApi_Debug_Msg("LOOP: stream type=%x\n",gbemg_stream.stream_type);

        switch( gbemg_stream.stream_type )
        {
        /* video formats */
	CelthApi_Debug_Msg(" Switch:stream_type=%x\n",gbemg_stream.stream_type);
        	
        case GBEMG_11172_2_Video:  /* MPEG-1 */
            ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_mpeg1,i);
            break;
        case GBEMG_ATSC_Video:   /* ATSC MPEG-2 */
            ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_mpeg2,i);
            break;
        case GBEMG_13818_2_Video: /* MPEG-2 */
            ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_mpeg2,i);
            break;
        case GBEMG_14496_2_Video: /* MPEG-4 Part 2 */
            ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_mpeg4_part2, i);
            break;
        case GBEMG_14496_10_Video: /* H.264/AVC */
            ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_h264, i);
            break;
        case GBEMG_AVS_Video: /* AVS */
            ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_avs, i);
            break;
        case GBEMG_SMPTE_VC1:      /* VC-1 */
            /* need to parse descriptor and then subdescriptor to determine profile */
            for (;;) {
                U8* desc = GBEMG_getStreamDescriptor(TableSegment, SegmentSize, i, descIdx);
                if (desc == NULL) break;
                descIdx++;

                switch(desc[0]) {
                case GBEMG_DT_Registration:
                    /* calculate and check format_identifier */
                    {
                    U32 format_identifier = (desc[2] << 24) + (desc[3] << 16) + (desc[4] << 8) + desc[5];
                    if (format_identifier == 0x56432D31) {
                        /* check that proper sub-descriptor exists */
                        int subdescriptor_tag = desc[6];
                        if (subdescriptor_tag == 0x01) {
                            int profile_level = desc[7];
                            if (profile_level >= 0x90)  /* Advanced Profile ES */
                                ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_vc1,i);
                            else /* Simple/Main Profile ES */
                                ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_vc1_sm, i);
                        }
                    }
                    }
                    break;
                default:
                    break;
                }
            }
            break;

        /* audio formats */
        case GBEMG_11172_3_Audio: /* MPEG-1 */
            ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_mpeg, i);  /* Same baudio_format for MPEG-1 or MPEG-2 audio */
            break;
        case GBEMG_13818_3_Audio: /* MPEG-2 */
            ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_mpeg, i);  /* Same baudio_format for MPEG-1 or MPEG-2 audio */
            break;
        case GBEMG_13818_7_AAC:  /* MPEG-2 AAC */
            ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_aac, i);    /* Note baudio_format_aac = MPEG-2 AAC */
            break;
        case GBEMG_14496_3_Audio: /* MPEG-4 AAC */
            ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_aac_plus,i);   /* Note baudio_format_aac_plus = MPEG-4 AAC */
            break;
        case GBEMG_ATSC_AC3:      /* ATSC AC-3 */
            ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_ac3, i);
            break;
        case GBEMG_ATSC_EAC3:     /* ATSC Enhanced AC-3 */
            ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_ac3_plus, i);
            break;
        case GBEMG_ATSC_DTS:     /* ASTC ??? DTS audio */
            ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_dts, i);
            break;
        case GBEMG_AVS_Audio:     /* AVS */
            ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_avs, i);
            break;
        case GBEMG_DRA_Audio:     /* DRA */
            ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_dra, i);
            break;


        /* video or audio */
        case GBEMG_13818_1_PrivatePES:  /* examine descriptors to handle private data */
            for (;;) {
                U8* desc = GBEMG_getStreamDescriptor(TableSegment, SegmentSize, i, descIdx);
                if (desc == NULL) break;
                descIdx++;

                switch(desc[0]) {
                /* video formats */
                case GBEMG_DT_VideoStream:
                    /* MPEG_1_only_flag is bit 2 of desc[2], this determines MPEG-1/2 */
                    if ((desc[2] & 0x04) == 1)
                        ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_mpeg1,i);
                    else
                        ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_mpeg2,i);
                    break;
                case GBEMG_DT_MPEG4_Video:
                    ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_mpeg4_part2,i);
                    break;
                case GBEMG_DT_AVC:
                    ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_h264,i);
                    break;
                case GBEMG_DT_AVS_Video:
                    ADD_VIDEO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Vcodec_avs,i);
                    break;

                /* audio formats */
                case GBEMG_DT_AudioStream:
                    ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_mpeg,i);   /* Same baudio_format for MPEG-1 or MPEG-2 audio */
                    break;
                case GBEMG_DT_MPEG2_AAC:
                    ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_aac,i);   /* Note baudio_format_aac = MPEG-2 AAC */
                    break;
                case GBEMG_DT_MPEG4_Audio:
                    ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_aac_plus,i); /* Note baudio_format_aac_plus = MPEG-4 AAC */
                    break;
                case GBEMG_DT_DVB_AAC:
                    ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_aac_plus,i); /* Note baudio_format_aac_plus = MPEG-4 AAC */
                    break;
                case GBEMG_DT_DVB_AC3:
                    ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_ac3,i);
                    break;
                case GBEMG_DT_DVB_EnhancedAC3:
                    ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_ac3_plus,i);
                    break;
                case GBEMG_DT_DVB_DTS:
                    ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_dts,i);
                    break;
                case GBEMG_DT_DVB_DRA:
                    ADD_AUDIO_PID(pchannel, gbemg_stream.stream_pid, GBEMG_Acodec_dra,i);
                    break;
                default:
                    CelthApi_Debug_Err("Unsupported private descriptor 0x%x",desc[0]);
                    break;
                }
            }
            break;
        case GBEMG_13818_1_PrivateSection:
            if( pchannel->num_other_pids < MAX_GBEMG_DETAIL_PROGRAM_PIDS )
            {
                ADD_OTHER_PID(pchannel, gbemg_stream.stream_pid, gbemg_stream.stream_type, i);
            }
            break;
        default:
            {
            	CelthApi_Debug_Err("Unsupported private descriptor 0x%x",gbemg_stream.stream_type);
            }
            break;		
            
        }
        /* If we get any data our status is complete! */
    } /* EFOR Program map loop */

CelthApi_Debug_LeaveFunction("GBEMG_ParseSream");

	
}
	




GBEMG_Terrestrial_Param_t* GetTerrestrialParamFromDescriptor(U8* descripor)
{
	GBEMG_Terrestrial_Param_t* pTerrestrial_param;
	
	pTerrestrial_param=(GBEMG_Terrestrial_Param_t*) CELTHOSL_MemoryAlloc(sizeof(GBEMG_Terrestrial_Param_t));
	
	if(pTerrestrial_param==NULL)
	{
		return NULL;
	}
	
	if(GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_DESCRIPTOR_TAG(descripor)!=GBEMG_CONT_TERRESTRIAL_TRANS_SYSTEM_DESCRIPTOR_TAG)
	{
		return NULL;
	}
	
	if(GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_DESCRIPTOR_LENGTH(descripor)!=GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_DESCRIP_LENGTH-2)
	{
		return NULL;
	}
	
	pTerrestrial_param->centre_freq = GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_CENTRE_FREQ(descripor);
	pTerrestrial_param->fec = GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_FEC(descripor);
	pTerrestrial_param->modulation = GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_MOD(descripor);
	pTerrestrial_param->frame_mode = GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_FRAME_HEADER(descripor);
	
	return pTerrestrial_param;
	
}


GBEMG_TYPE  GetGBEMGType(U8* tableSegment)
{
		
	if(tableSegment)
	{
		return 	tableSegment[0]*16*16*16*16+tableSegment[1]*16*16*16+tableSegment[2]*16*16+tableSegment[3]*16+tableSegment[4];
	}
	else
	{
		return 0xFFFFF;
	}
}

void GetGBEMGIssueTimeFromIndexTable(U8* TableSegment,GBEMG_Time_t* pissueTime)
{
	U16 MJD;

	U8 time[3];

	/*U8 weekday;*/

	CelthApi_Debug_EnterFunction("GetGBEMGIssueTimeFromIndexTable");

	if(TableSegment==NULL&&pissueTime==NULL)
	{
		return;
	}

	MJD=(U16)((TableSegment[0]<<8)|TableSegment[1]);

	

	time[0]=TableSegment[2];

	time[1]=TableSegment[3];

	time[2]=TableSegment[4];

CelthApi_Debug_Msg("MJD=%x,Time[0]=%x,",MJD,time[0]);
CelthApi_Debug_Msg("Time[1]=%x,Time[2]=%x\n",time[1],time[2]);

/*	if(time[0]==0xFF&&time[1]==0xFF&&time[2]==0xFF)
	{
		return ;
	}
*/

	mjd_to_ymd((U32)MJD,&(pissueTime->year),&(pissueTime->month),&(pissueTime->day),&(pissueTime->weekday));


	CelthApi_Debug_Msg("\nset year=%d,month=%d,",pissueTime->year,pissueTime->month);
	CelthApi_Debug_Msg("day=%d,weekday=%d,",pissueTime->day,pissueTime->weekday);


	bcd_to_hms(time,&pissueTime->hour,&pissueTime->minute,&pissueTime->second);

	CelthApi_Debug_Msg("hour=%d,minute=%d,",pissueTime->hour,pissueTime->minute);
	CelthApi_Debug_Msg("second=%d\n",pissueTime->second);



	CelthApi_Debug_LeaveFunction("GetGBEMGIssueTimeFromIndexTable");

	
	
}

void GetGBEMGDurationTimeFromIndexTable(U8* TableSegment,GBEMG_Time_Duration_t* pdurationtime)
{
	CelthApi_Debug_EnterFunction("GetGBEMGDurationTimeFromIndexTable");

	if(TableSegment==NULL&&pdurationtime==NULL)
	{
		return;
	}

	pdurationtime->durationsec =(U32)(0x00FFFFFF|( (TableSegment[0]<<16)|(TableSegment[1]<<8)|TableSegment[2])); 

	CelthApi_Debug_Msg("duration=%x\n",pdurationtime->durationsec);
	CelthApi_Debug_LeaveFunction("GetGBEMGDurationTimeFromIndexTable");
	
}

void GetEBMEGCoverArea(U8* TableSegment,GBEMG_Area_t* parea)
{
	
	if(TableSegment==NULL&&parea==NULL)
	{
		return;
	}

	CelthApi_Debug_Msg("Cover Area:\n");
	CelthApi_Debug_Msg("code[0]=%x,code[1]=%x,",*(TableSegment+1),*(TableSegment+2));
	CelthApi_Debug_Msg("code[2]=%x,code[3]=%x,\n",*(TableSegment+3),*(TableSegment+4));

return ;
}









