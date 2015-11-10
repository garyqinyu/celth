/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: tspsimgr.c $
 * $brcm_Revision: 21 $
 * $brcm_Date: 4/26/10 1:20p $
 *
 ****************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_message.h"

#include "celth_stddef.h"
#include "celth_avplay.h"

#include "ts_psi.h"
#include "ts_pat.h"
#include "ts_pmt.h"


#include "tspsisearch.h"

#include "bkni.h"
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memset */

#define PSI_BFR_SIZE    TS_PSI_MAX_PSI_TABLE_SIZE

/* Note that the following timeouts are not 400 milliseconds.
It is 400 BKNI_Sleep(10)'s, which will be about 4000 ms (4 seconds) on linux.
Also, the original default of 200 was found to be too short for some customer streams. The downside
of having too large a timeout value is usually app delays when scanning non-existent streamers
or streams with no PSI information. Not critical. */
static int tsPsi_patTimeout = 400;

/* PR 18488 - temporarily increasing timeout for 740x platforms. */
static int tsPsi_pmtTimeout = 600;

static void tsPsiSearch_procProgDescriptors( const uint8_t *p_bfr, unsigned bfrSize, PROGRAM_INFO_T *progInfo );
static void tsPsiSearch_procStreamDescriptors( const uint8_t *p_bfr, unsigned bfrSize, int streamNum, EPID *ePidData );
static uint8_t tsPsiSearch_getProgramMaps( const void *p_patBfr, unsigned pat_bfrSize,
    CHANNEL_INFO_T *p_chanInfo, Celth_ParserBand parseband ,Celth_FrontendBand inputband);
static void tsPsiSearch_p_dumpBuffer(const uint8_t *p_bfr, unsigned bfrSize);

static NEXUS_ParserBand ConvertCelth2Nexus(Celth_ParserBand parseband);

#if BDBG_DEBUG_BUILD
#define B_ERROR(ERR) (BDBG_ERR(("%s at line %d", #ERR, __LINE__)), (ERR))
#else
#define B_ERROR(ERR) (ERR)
#endif



NEXUS_ParserBand ConvertCelth2Nexus(Celth_ParserBand parseband)
{
	return (NEXUS_ParserBand)parseband;
}





void tsPsiSearch_message_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}




int tsPsiSearch_getPAT( void *buffer, unsigned bufferSize, Celth_ParserBand parseband ,Celth_FrontendBand inputband)
{
    NEXUS_PidChannelHandle pidChannel;
    NEXUS_MessageHandle msg;
    NEXUS_MessageSettings settings;
    NEXUS_MessageStartSettings startSettings;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidChannelSettings;
    BKNI_EventHandle event;
    NEXUS_Error rc;
    int timeout;
	
	
	
    unsigned patSize = 0;

  /*  NEXUS_ParserBand_GetSettings(parseband, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    NEXUS_ParserBand_SetSettings(parseband, &parserBandSettings);*/
	
	
	    /* Map a parser band to the demod's input band. */
    parseband = Celth_ParserBand_0;

  	
    NEXUS_ParserBand_GetSettings(ConvertCelth2Nexus(parseband), &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand =(NEXUS_InputBand) inputband;  /* Platform initializes this to input band */
    
    NEXUS_ParserBand_SetSettings(ConvertCelth2Nexus(parseband), &parserBandSettings);
	

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannel = NEXUS_PidChannel_Open(ConvertCelth2Nexus(parseband), 0x0, &pidChannelSettings);

    BKNI_CreateEvent(&event);

    NEXUS_Message_GetDefaultSettings(&settings);
    settings.dataReady.callback = tsPsiSearch_message_callback;
    settings.dataReady.context = event;
    settings.maxContiguousMessageSize = 4096;
    msg = NEXUS_Message_Open(&settings);
	
	
	NEXUS_Message_GetDefaultStartSettings(msg, &startSettings);
    startSettings.pidChannel = pidChannel;
    /* use the default filter for any data */

    rc = NEXUS_Message_Start(msg, &startSettings);

    printf(("Looking for PAT"));
    for( timeout = tsPsi_patTimeout; timeout > 0 && patSize == 0; timeout-- )
    {
        const void *locbuffer;
		
       if (NEXUS_Message_GetBuffer(msg, &locbuffer, &patSize))
            return -1;
        if (patSize) {
         printf(("PAT message size=%d",patSize));
            if (patSize > bufferSize)
                patSize = bufferSize;

            BKNI_Memcpy(buffer, locbuffer, patSize);
           printf(("PAT message copy finished"));
            /* don't call bmessage_read_complete because we're stopping anyway */
        }
        else {
            BKNI_Sleep(10);
        }
    }
    NEXUS_Message_Stop(msg);

	
	printf("\nclose the msg[%x] at line[%x]on %s\n",msg,__LINE__,__FILE__);
    NEXUS_Message_Close(msg);
    NEXUS_PidChannel_Close(pidChannel);



    /* validate before returning it */
    if (patSize && !TS_PAT_validate(buffer, patSize)) {
        tsPsiSearch_p_dumpBuffer(buffer, patSize);
        printf(("Invalid PAT data detected"));
        return -1;
    }

printf(("return patSize=%d",patSize));

    return patSize;
}

uint8_t tsPsiSearch_getChannelInfo( CHANNEL_INFO_T *p_chanInfo, Celth_ParserBand parseband ,Celth_FrontendBand inputband)
{
    uint8_t     pat[PSI_BFR_SIZE];
    size_t      patSize;
    TS_PSI_header header;



printf("get channel info with parserband [%x], frontendband[%x]\n",parseband,inputband);

    /* Blocking call to get PAT */
    patSize = tsPsiSearch_getPAT( pat, PSI_BFR_SIZE, parseband,inputband);
    /* If there's no PAT, return but don't print an error because this can happen
    normally. */
    if (patSize <= 0)
        return -1;
    printf(("Parsing PAT header"));
    TS_PSI_getSectionHeader( pat, &header );
    printf(("Finished Parsing PAT header"));
    p_chanInfo->version                 = header.version_number;
    p_chanInfo->transport_stream_id     = header.table_id_extension;
    p_chanInfo->num_programs            = 0;

    printf(("Parsing PAT"));
    return tsPsiSearch_getProgramMaps( pat, patSize, p_chanInfo, parseband ,inputband);
}

#define ADD_VIDEO_PID(P_INFO, PID, TYPE, PMT, PMTSIZE, INDEX) \
    do { \
    if( (P_INFO)->num_video_pids < MAX_PROGRAM_MAP_PIDS ) \
    { \
    printf(("  vpid[%d] 0x%x, type 0x%x", (P_INFO)->num_video_pids, (PID), (TYPE))); \
    (P_INFO)->video_pids[(P_INFO)->num_video_pids].pid = (PID); \
    (P_INFO)->video_pids[(P_INFO)->num_video_pids].streamType = (TYPE); \
    tsPsiSearch_procStreamDescriptors((PMT), (PMTSIZE), (INDEX), &(P_INFO)->video_pids[(P_INFO)->num_video_pids] ); \
    (P_INFO)->num_video_pids++; \
    } \
    } while (0)
#define ADD_AUDIO_PID(P_INFO, PID, TYPE, PMT, PMTSIZE, INDEX) \
    do { \
    if( (P_INFO)->num_audio_pids < MAX_PROGRAM_MAP_PIDS ) \
    { \
    printf(("  apid[%d] 0x%x, type 0x%x", (P_INFO)->num_audio_pids, (PID), (TYPE))); \
    (P_INFO)->audio_pids[(P_INFO)->num_audio_pids].pid = (PID); \
    (P_INFO)->audio_pids[(P_INFO)->num_audio_pids].streamType = (TYPE); \
    tsPsiSearch_procStreamDescriptors((PMT), (PMTSIZE), (INDEX), &(P_INFO)->audio_pids[(P_INFO)->num_audio_pids] ); \
    (P_INFO)->num_audio_pids++; \
    } \
    } while (0)
#define ADD_OTHER_PID(P_INFO, PID, TYPE, PMT, PMTSIZE, INDEX) \
    do { \
    if( (P_INFO)->num_other_pids < MAX_PROGRAM_MAP_PIDS ) \
    { \
    printf(("  opid[%d] 0x%x, type 0x%x", (P_INFO)->num_audio_pids, (PID), (TYPE))); \
    (P_INFO)->other_pids[(P_INFO)->num_other_pids].pid = (PID); \
    (P_INFO)->other_pids[(P_INFO)->num_other_pids].streamType = (TYPE); \
    tsPsiSearch_procStreamDescriptors((PMT), (PMTSIZE), (INDEX), &(P_INFO)->other_pids[(P_INFO)->num_other_pids] ); \
    (P_INFO)->num_other_pids++; \
    } \
    } while (0)

void tsPsiSearch_parsePMT( const void *pmt, unsigned pmtSize, PROGRAM_INFO_T *p_programInfo)
{
    int i;
    TS_PMT_stream pmt_stream;
    TS_PSI_header header;

    TS_PSI_getSectionHeader(pmt, &header );

    /* Store the main information about the program */
    p_programInfo->program_number   = header.table_id_extension;
    p_programInfo->version          = header.version_number;
    p_programInfo->pcr_pid          = TS_PMT_getPcrPid(pmt, pmtSize);

    /* find and process Program descriptors */
    tsPsiSearch_procProgDescriptors(pmt, pmtSize, p_programInfo );

    /* Find the video and audio pids... */
    p_programInfo->num_video_pids   = 0;
    p_programInfo->num_audio_pids   = 0;
    p_programInfo->num_other_pids   = 0;

    for( i = 0; i < TS_PMT_getNumStreams(pmt, pmtSize); i++ )
    {
        int descIdx = 0;
        if (TS_PMT_getStream(pmt, pmtSize, i, &pmt_stream )) {
            printf(("Invalid PMT data detected"));
            continue;
        }

        switch( pmt_stream.stream_type )
        {
        /* video formats */
        case TS_PSI_ST_11172_2_Video:  /* MPEG-1 */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg1, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_ATSC_Video:   /* ATSC MPEG-2 */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg2, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_13818_2_Video: /* MPEG-2 */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg2, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_14496_2_Video: /* MPEG-4 Part 2 */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg4_part2, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_14496_10_Video: /* H.264/AVC */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_h264, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_AVS_Video: /* AVS */
            ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_avs, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_SMPTE_VC1:      /* VC-1 */
            /* need to parse descriptor and then subdescriptor to determine profile */
            for (;;) {
                TS_PSI_descriptor desc = TS_PMT_getStreamDescriptor(pmt, pmtSize, i, descIdx);
                if (desc == NULL) break;
                descIdx++;

                switch(desc[0]) {
                case TS_PSI_DT_Registration:
                    /* calculate and check format_identifier */
                    {
                    uint32_t format_identifier = (desc[2] << 24) + (desc[3] << 16) + (desc[4] << 8) + desc[5];
                    if (format_identifier == 0x56432D31) {
                        /* check that proper sub-descriptor exists */
                        int subdescriptor_tag = desc[6];
                        if (subdescriptor_tag == 0x01) {
                            int profile_level = desc[7];
                            if (profile_level >= 0x90)  /* Advanced Profile ES */
                                ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_vc1, pmt, pmtSize, i);
                            else /* Simple/Main Profile ES */
                                ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_vc1_sm, pmt, pmtSize, i);
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
        case TS_PSI_ST_11172_3_Audio: /* MPEG-1 */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_mpeg, pmt, pmtSize, i);  /* Same baudio_format for MPEG-1 or MPEG-2 audio */
            break;
        case TS_PSI_ST_13818_3_Audio: /* MPEG-2 */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_mpeg, pmt, pmtSize, i);  /* Same baudio_format for MPEG-1 or MPEG-2 audio */
            break;
        case TS_PSI_ST_13818_7_AAC:  /* MPEG-2 AAC */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_aac, pmt, pmtSize, i);    /* Note baudio_format_aac = MPEG-2 AAC */
            break;
        case TS_PSI_ST_14496_3_Audio: /* MPEG-4 AAC */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_aac_plus, pmt, pmtSize, i);   /* Note baudio_format_aac_plus = MPEG-4 AAC */
            break;
        case TS_PSI_ST_ATSC_AC3:      /* ATSC AC-3 */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_ac3, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_ATSC_EAC3:     /* ATSC Enhanced AC-3 */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_ac3_plus, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_ATSC_DTS:     /* ASTC ??? DTS audio */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_dts, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_AVS_Audio:     /* AVS */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_avs, pmt, pmtSize, i);
            break;
        case TS_PSI_ST_DRA_Audio:     /* DRA */
            ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_dra, pmt, pmtSize, i);
            break;


        /* video or audio */
        case TS_PSI_ST_13818_1_PrivatePES:  /* examine descriptors to handle private data */
            for (;;) {
                TS_PSI_descriptor desc = TS_PMT_getStreamDescriptor(pmt, pmtSize, i, descIdx);
                if (desc == NULL) break;
                descIdx++;

                switch(desc[0]) {
                /* video formats */
                case TS_PSI_DT_VideoStream:
                    /* MPEG_1_only_flag is bit 2 of desc[2], this determines MPEG-1/2 */
                    if ((desc[2] & 0x04) == 1)
                        ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg1, pmt, pmtSize, i);
                    else
                        ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg2, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_MPEG4_Video:
                    ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_mpeg4_part2, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_AVC:
                    ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_h264, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_AVS_Video:
                    ADD_VIDEO_PID(p_programInfo, pmt_stream.elementary_PID, bvideo_codec_avs, pmt, pmtSize, i);
                    break;

                /* audio formats */
                case TS_PSI_DT_AudioStream:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_mpeg, pmt, pmtSize, i);   /* Same baudio_format for MPEG-1 or MPEG-2 audio */
                    break;
                case TS_PSI_DT_MPEG2_AAC:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_aac, pmt, pmtSize, i);   /* Note baudio_format_aac = MPEG-2 AAC */
                    break;
                case TS_PSI_DT_MPEG4_Audio:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_aac_plus, pmt, pmtSize, i); /* Note baudio_format_aac_plus = MPEG-4 AAC */
                    break;
                case TS_PSI_DT_DVB_AAC:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_aac_plus, pmt, pmtSize, i); /* Note baudio_format_aac_plus = MPEG-4 AAC */
                    break;
                case TS_PSI_DT_DVB_AC3:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_ac3, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_DVB_EnhancedAC3:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_ac3_plus, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_DVB_DTS:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_dts, pmt, pmtSize, i);
                    break;
                case TS_PSI_DT_DVB_DRA:
                    ADD_AUDIO_PID(p_programInfo, pmt_stream.elementary_PID, baudio_format_dra, pmt, pmtSize, i);
                    break;
                default:
                    printf(("Unsupported private descriptor 0x%x",desc[0]));
                    break;
                }
            }
            break;
        default:
            if( p_programInfo->num_other_pids < MAX_PROGRAM_MAP_PIDS )
            {
                ADD_OTHER_PID(p_programInfo, pmt_stream.elementary_PID, pmt_stream.stream_type, pmt, pmtSize, i);
            }
            break;
        }
        /* If we get any data our status is complete! */
    } /* EFOR Program map loop */
}

static void tsPsiSearch_p_addChanInfo(void *context, uint16_t pmt_pid, const void *pmt, unsigned size)
{
    CHANNEL_INFO_T *p_chanInfo = (CHANNEL_INFO_T*)context;
    PROGRAM_INFO_T programInfo;
    int i;

    BKNI_Memset(&programInfo, 0, sizeof(programInfo));

    /* If this isn't true, then the logic at the top of tsPsi_getPMTs has failed,
    or we haven't stopped each msgstream after reading one and only one item. */
    BDBG_ASSERT(p_chanInfo->num_programs < MAX_PROGRAMS_PER_CHANNEL);

    /* The "if" comparision below is present to silence the Coverity from choosing the false path in the above "BDBG_ASSERT" line of code. */
    if(p_chanInfo->num_programs < MAX_PROGRAMS_PER_CHANNEL){
        tsPsiSearch_parsePMT(pmt, size, &programInfo);
        programInfo.map_pid = pmt_pid;

        /* now that we know the program_number, insert it into the array */
        for (i=0;i<p_chanInfo->num_programs;i++) {
            if (programInfo.program_number < p_chanInfo->program_info[i].program_number)
                break;
        }
        /* make room for an insertion */
        if (i < p_chanInfo->num_programs) {
            unsigned char *ptr = (unsigned char *)&p_chanInfo->program_info[i];
            BKNI_Memmove(ptr + sizeof(PROGRAM_INFO_T), ptr, sizeof(PROGRAM_INFO_T) * (p_chanInfo->num_programs - i));
        }
        /* now copy into place */
        BKNI_Memcpy(&p_chanInfo->program_info[i], &programInfo, sizeof(PROGRAM_INFO_T));
        p_chanInfo->num_programs++;
    }
}

static uint8_t  tsPsiSearch_getProgramMaps( const void *p_patBfr, unsigned pat_bfrSize,
    CHANNEL_INFO_T *p_chanInfo, Celth_ParserBand parseband,Celth_FrontendBand inputband )
{
    return tsPsiSearch_getPMTs(p_patBfr, pat_bfrSize, tsPsiSearch_p_addChanInfo, p_chanInfo,parseband ,inputband);
}

uint8_t tsPsiSearch_getPMTs( const void *p_patBfr, unsigned pat_bfrSize,
    tsPsiSearch_PMT_callback callback, void *context,Celth_ParserBand parseband,Celth_FrontendBand inputband )
{
   
    int             i;
	uint8_t     retValue;
    size_t          bfrSize;
    int             num_programs;
    int             num_programs_to_get;
    int             num_messages_received;
    bool            message_received;
    int             timeout;
    
    uint16_t        *pmt_pidArray;
    TS_PAT_program  program;
    int             curProgramNum = 0;
	
	
	NEXUS_PidChannelHandle* pmt_pidChannelArray;
    NEXUS_MessageHandle *pmt_msgArray;
    NEXUS_MessageSettings settings;
    NEXUS_MessageStartSettings startSettings;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidChannelSettings;
	
	
	

    num_programs = TS_PAT_getNumPrograms(p_patBfr);
    printf(("num_programs %d", num_programs));
    if( num_programs > MAX_PROGRAMS_PER_CHANNEL )
    {
        printf(("Maximum number of programs exceeded in tspsimgr: %d > %d",
            num_programs, MAX_PROGRAMS_PER_CHANNEL));
        num_programs = MAX_PROGRAMS_PER_CHANNEL;
    }

    pmt_msgArray = (NEXUS_MessageHandle *)malloc( sizeof(NEXUS_MessageHandle) * num_programs );
    memset(pmt_msgArray, 0, sizeof(NEXUS_MessageHandle) * num_programs);
	
	pmt_pidChannelArray = (NEXUS_PidChannelHandle *)malloc( sizeof(NEXUS_PidChannelHandle) * num_programs );
    memset(pmt_pidChannelArray, 0, sizeof(NEXUS_PidChannelHandle) * num_programs);

    pmt_pidArray = (uint16_t *)malloc( sizeof(uint16_t) * num_programs );
    memset(pmt_pidArray, 0, sizeof(uint16_t) * num_programs);

    retValue = TS_PAT_getProgram( p_patBfr, pat_bfrSize, curProgramNum, &program );
    curProgramNum++;

    if( retValue == 0 )
    {
        while( num_programs )
        {
            /* Always try to read the max number of pids at the same time */
            num_programs_to_get = num_programs;
            num_messages_received = 0;

            for( i = 0; i < num_programs_to_get; i++ )
            {
                if( program.program_number == 0 )
                {
                    /* This is the network PID, so ignore it */
                    num_messages_received++;
                }
                else
                {
                    /*   NEXUS_ParserBand_GetSettings(parseband, &parserBandSettings);
                       parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
                       NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
                       NEXUS_ParserBand_SetSettings(parseband, &parserBandSettings);*/
					   
					
						
						    /* Map a parser band to the demod's input band. */
    parseband = Celth_ParserBand_0;
    NEXUS_ParserBand_GetSettings(ConvertCelth2Nexus(parseband), &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = (NEXUS_InputBand)inputband;  /* Platform initializes this to input band */
     NEXUS_ParserBand_SetSettings(ConvertCelth2Nexus(parseband), &parserBandSettings);
	 
	     NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
						
						
						pmt_pidChannelArray[i] = NEXUS_PidChannel_Open(ConvertCelth2Nexus(parseband), program.PID, &pidChannelSettings);
						
						if(pmt_pidChannelArray[i]==NULL)
						{
						
							num_programs_to_get = i-1;
						
							if( num_programs_to_get == 0 )
							{
                            /* Abort due to not being able to enable messages */
/*                          BRCM_DBG_MSG(("Unable to enable any messages!")); */
								num_programs = 0;
							}
							break;
						}
						
						
						 NEXUS_Message_GetDefaultSettings(&settings);
						settings.dataReady.callback = NULL;
						settings.dataReady.context = NULL;
						settings.maxContiguousMessageSize = 4096;
						pmt_msgArray[i] = NEXUS_Message_Open(&settings);

                   
                    if( pmt_msgArray[i] == NULL )
                    {
                        /* Decrease the number of programs to get in this loop due to transport resources */
                        num_programs_to_get = i-1;
						
						NEXUS_PidChannel_Close(pmt_pidChannelArray[i]);
						pmt_pidChannelArray[i]=NULL;

                        if( num_programs_to_get == 0 )
                        {
                            /* Abort due to not being able to enable messages */
/*                          BRCM_DBG_MSG(("Unable to enable any messages!")); */
                            num_programs = 0;
                        }
                        break;
                    }
					
					
					NEXUS_Message_GetDefaultStartSettings(pmt_msgArray[i], &startSettings);
					
					startSettings.pidChannel = pmt_pidChannelArray[i];
			        startSettings.filter.mask[0]=0x00;
				/*	startSettings.filter.mask[3]=0x00;
					startSettings.filter.mask[4]=0x00;*/
					
					startSettings.filter.coefficient[0]=0x02;
                                    
				    startSettings.filter.coefficient[3]=(program.program_number & 0xFF00) >> 8;
					startSettings.filter.coefficient[4]=program.program_number & 0xFF;

					pmt_pidArray[i]=program.PID;

					printf(("filter pid %#x, program %#x", program.PID, program.program_number));
                                        printf(("fileter setting filter[0]%02x, filter[3]%02x, filter[4]%02x",startSettings.filter.coefficient[0],startSettings.filter.coefficient[3],startSettings.filter.coefficient[4]));
                   

                    if (NEXUS_Message_Start(pmt_msgArray[i], &startSettings)) {
						
						printf("\nclose the msg[%x] at line[%x]on %s\n",pmt_msgArray[i],__LINE__,__FILE__);
                        NEXUS_Message_Close(pmt_msgArray[i]);
						NEXUS_PidChannel_Close(pmt_pidChannelArray[i]);
                        pmt_msgArray[i] = NULL;
						pmt_pidChannelArray[i]=NULL;
                        pmt_pidArray[i] = 0;
                      printf(("error here,exit\n")); 
                    }
                }

                /* We are finished with this program association so go to the next */

                /* TODO: Check for error */
                TS_PAT_getProgram( p_patBfr, pat_bfrSize, curProgramNum, &program );
                curProgramNum++;
            }
            printf(("parpare get data of num_received=%d, num_to get=%d",num_messages_received, num_programs_to_get ));   

            /* Now we have enabled our pid channels, so wait for each one to get some data */
            timeout = tsPsi_pmtTimeout;
            while( num_messages_received != num_programs_to_get && timeout != 0 )
            {
                message_received = 0;
                /* Check each of the pid channels we are waiting for */
                for( i = 0; i < num_programs_to_get; i++ )
                {
                    const void *buffer;
                /*    printf(("start get data of %d",i));*/
                    if (pmt_msgArray[i] &&
                        !NEXUS_Message_GetBuffer(pmt_msgArray[i], &buffer, &bfrSize ) &&
                        bfrSize)
                    {
                        /* don't call bmessage_read_complete because we're stopping anyway */

                        message_received = true;
                        num_messages_received++;

                        printf(("PMT: %d %d (%02x %02x %02x %02x %02x)", i, bfrSize,
                            ((unsigned char *)buffer)[0],((unsigned char *)buffer)[1],((unsigned char *)buffer)[2],((unsigned char*)buffer)[3],((unsigned char*)buffer)[4]));

                        if (!TS_PMT_validate(buffer, bfrSize)) {
                            printf(("Invalid PMT data detected: ch %d, bfrSize 0x%x", i, bfrSize));
                            tsPsiSearch_p_dumpBuffer(buffer, bfrSize);
                        }
                        else {
                            /* Give the PMT to the callback */
                            (*callback)(context, pmt_pidArray[i], buffer, bfrSize);
                        }

                        /* cannot stop until after the data has been parsed because
                        we're not copying the data into a local buffer */
                        NEXUS_Message_Stop(pmt_msgArray[i]);
                    }
                }
                if( !message_received )
                {
                    BKNI_Sleep(10);
                    timeout--;
                }
            }

            /* Now disable our pid channels */
            for( i = 0; i < num_programs_to_get; i++ )
            {
                if (pmt_msgArray[i])
				{
					printf("\nclose the msg[%x] at line[%x]on %s\n",pmt_msgArray[i],__LINE__,__FILE__);
                    NEXUS_Message_Close(pmt_msgArray[i]);
					pmt_msgArray[i]=NULL;
				}
					
				if(pmt_pidChannelArray[i])
				{
                    NEXUS_PidChannel_Close(pmt_pidChannelArray[i]);
					pmt_pidChannelArray[i]=NULL;
                }					
            }
            num_programs -= num_programs_to_get;
        }
    }

    free( pmt_msgArray );
    free(pmt_pidArray);
	free(pmt_pidChannelArray);

    return retValue;
}

void tsPsiSearch_procProgDescriptors( const uint8_t *p_bfr, unsigned bfrSize, PROGRAM_INFO_T *progInfo )
{
    int i;
    TS_PSI_descriptor descriptor;

    for( i = 0, descriptor = TS_PMT_getDescriptor( p_bfr, bfrSize, i );
        descriptor != NULL;
        i++, descriptor = TS_PMT_getDescriptor( p_bfr, bfrSize, i ) )
    {
        switch (descriptor[0])
        {
        case TS_PSI_DT_CA:
            progInfo->ca_pid = ((descriptor[4] & 0x1F) << 8) + descriptor[5];
            break;

        default:
            break;
        }
    }
}

void tsPsiSearch_procStreamDescriptors( const uint8_t *p_bfr, unsigned bfrSize, int streamNum, EPID *ePidData )
{
    int i;
    TS_PSI_descriptor descriptor;

    for( i = 0, descriptor = TS_PMT_getStreamDescriptor( p_bfr, bfrSize, streamNum, i );
        descriptor != NULL;
        i++, descriptor = TS_PMT_getStreamDescriptor( p_bfr, bfrSize, streamNum, i ) )
    {
        switch (descriptor[0])
        {
        case TS_PSI_DT_CA:
            ePidData->ca_pid = ((descriptor[4] & 0x1F) << 8) + descriptor[5];
            break;

        default:
            break;
        }
    }
}

void tsPsiSearch_setTimeout( int patTimeout, int pmtTimeout )
{
    tsPsi_patTimeout = patTimeout;
    tsPsi_pmtTimeout = pmtTimeout;
}

void tsPsiSearch_getTimeout( int *patTimeout, int *pmtTimeout )
{
    *patTimeout = tsPsi_patTimeout;
    *pmtTimeout = tsPsi_pmtTimeout;
}

static void tsPsiSearch_p_dumpBuffer(const uint8_t *p_bfr, unsigned bfrSize)
{
    unsigned i;
    for (i=0;i<bfrSize;i++)
        printf("%02X", p_bfr[i]);
    printf("\n");
}
