/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: tspsimgr.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 1/18/07 12:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/tspsi/tspsimgr.h $
 * 
 * 4   1/18/07 12:10p ahulse
 * PR27186: Differentiate between parser and input bands. AKA, fixing
 * bmessage from playback source
 * 
 * 3   12/14/06 12:00p jjordan
 * PR26473: add get timeout capability
 * 
 * 2   10/11/05 6:23p mphillip
 * PR17300: Increased from 32 to 64 (after all, it's only RAM)
 * 
 * 1   2/7/05 11:34p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   11/4/04 1:02p erickson
 * PR13141: added pmt pid to PMT callback
 * 
 * Irvine_BSEAVSW_Devel/2   7/21/04 12:07p erickson
 * PR11682: refactored so that finer-grain control is available, without
 * breaking existing api
 * 
 * Irvine_BSEAVSW_Devel/2   9/16/03 2:40p erickson
 * support c++, added #ifndef wrapper
 * include bsettop.h because this header file is not a part of the settop
 * api, therefore
 * it shouldn't assume it
 *
 * Irvine_BSEAVSW_Devel/1   9/10/03 4:33p marcusk
 * initial version
 *
 ****************************************************************/
#ifndef TSPSISEARCH_H__
#define TSPSISEARCH_H__


#ifdef __cplusplus
extern "C" {
#endif


#ifndef uint8_t
#define uint8_t unsigned char
#endif

#ifndef uint16_t
#define uint16_t unsigned short
#endif

#ifndef uint32_t
#define uint32_t unsigned int
#endif

#if 0
typedef enum bvideo_codec_type {

bvideo_codec_none =0,
bvideo_codec_unknown=0,
bvideo_codec_mpeg1 = 1,
bvideo_codec_mpeg2 = 2,
bvideo_codec_mpeg4_part2 = 0x10,
bvideo_codec_h263 = 0x1A,
bvideo_codec_h264 = 0x1B,
bvideo_codec_vc1 = 0xEA,
bvideo_codec_vc1_sm = 0xEB,
bvideo_codec_divx_311= 0x311,
bvideo_codec_avs = 0x42,
bvideo_codec_sorenson_h263 = 0xF0,
bvideo_codec_vp6 = 0xF1,
bvideo_codec_rv40 = 0xF2
}bvideo_codec_type;



typedef enum baudio_format_type {
   baudio_format_unknown = 0,           /* unknown/not supported audio format */
   baudio_format_mpeg = 0x3,        /* MPEG1/2, layer 1/2. This does not support layer 3 (mp3). */
   baudio_format_mp3 = 0x1,         /* MPEG1/2, layer 3. */
   baudio_format_aac = 0xF,         /* Advanced audio coding. Part of MPEG-4 */
   baudio_format_aac_plus = 0x11,   /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE) */
   baudio_format_aac_plus_adts = 0x12,  /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with ADTS (Audio Data Transport Format) */
   baudio_format_aac_plus_loas = 0x11,  /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with LOAS (Low Overhead Audio Stream) */
   baudio_format_ac3 = 0x81,        /* Dolby Digital AC3 audio */
   baudio_format_ac3_plus = 0x6,    /* Dolby Digital Plus (AC3+ or DDP) audio */
   baudio_format_dts = 0x82,        /* Digital Digital Surround sound. */
   baudio_format_lpcm_hddvd = 0x83, /* LPCM, HD-DVD mode */
   baudio_format_lpcm_bluray = 0x84, /* LPCM, Blu-Ray mode */
   baudio_format_dts_hd = 0x85,     /* Digital Digital Surround sound, HD */
   baudio_format_wma_std = 0x86,    /* WMA Standard */
   baudio_format_wma_pro = 0x87,    /* WMA Professional */
   baudio_format_lpcm_dvd = 0x88,    /* LPCM, DVD mode */
   baudio_format_avs = 0x43,         /* AVS Audio */
   baudio_format_pcm,
   baudio_format_amr,
   baudio_format_dra = 0xda,
   baudio_format_cook = 0xf0
} baudio_format_type;

#endif


typedef struct
{
	uint16_t	pid;
	uint8_t		streamType;
	uint16_t  	ca_pid;
} EPID;

#define MAX_PROGRAM_MAP_PIDS	12
typedef struct
{
	uint16_t	program_number;
	uint16_t	map_pid;
	uint8_t		version;
	uint16_t	pcr_pid;
	uint16_t  	ca_pid;
	uint8_t		num_video_pids;
	EPID		video_pids[MAX_PROGRAM_MAP_PIDS];
	uint8_t		num_audio_pids;
	EPID		audio_pids[MAX_PROGRAM_MAP_PIDS];
	uint8_t		num_other_pids;
	EPID		other_pids[MAX_PROGRAM_MAP_PIDS];
} PROGRAM_INFO_T;

#define MAX_PROGRAMS_PER_CHANNEL 64
typedef struct
{
	uint8_t		version;
	uint16_t	transport_stream_id;
	uint32_t	sectionBitmap;
	uint16_t	num_programs;
	PROGRAM_INFO_T program_info[MAX_PROGRAMS_PER_CHANNEL];
} CHANNEL_INFO_T;

/**
Summary:
Populate a CHANNEL_INFO_T structure by scanning PSI information on
a band.

Description:
This call waits for a PAT, then waits for the PMT's for each program,
then builds the structure.

If you want finer-grain control, use the other tspsimgr functions below.
**/
uint8_t tsPsiSearch_getChannelInfo( CHANNEL_INFO_T *p_chanInfo, Celth_ParserBand parseband ,Celth_FrontendBand inputband);

/**
Set the timeout values for various blocking operations in tspsimgr.
**/
void tsPsiSearch_setTimeout( int patTimeout, int pmtTimeout );

/**
Get the timeout values for various blocking operations in tspsimgr.
**/
void tsPsiSearch_getTimeout( int *patTimeout, int *pmtTimeout );

/**
Summary:
Synchronous call to read the PAT, using the patTimeout.

Description:
bufferSize should be >= TS_PSI_MAX_PSI_TABLE_SIZE in order to read the PAT.

Return Values:
Returns the number of bytes read.
-1 for an error.
0 for no PAT read.
>0 for successful PAT read.
**/
int tsPsiSearch_getPAT( void *buffer, unsigned  bufferSize, Celth_ParserBand parseband,Celth_FrontendBand inputband );

/**
Callback used by tsPsi_getPMTs
**/
typedef void (*tsPsiSearch_PMT_callback)(void *context, uint16_t pmt_pid, const void *pmt, unsigned  pmtSize);

/**
Summary:
Read PMT's for each program specified in the PAT.

Description:
This will launch multiple bmessage_stream's and call the callback as each
PMT is read.
**/
uint8_t	tsPsiSearch_getPMTs(
	const void *pat, unsigned  patSize,
	tsPsiSearch_PMT_callback callback, void *context, Celth_ParserBand parseband, Celth_FrontendBand inputband );

/**
Summary:
Parse a PMT structure into a PROGRAM_INFO_T structure.
**/
void tsPsiSearch_parsePMT(const void *pmt, unsigned  pmtSize, PROGRAM_INFO_T *p_programInfo);

#ifdef __cplusplus
}
#endif

#endif /* TSPSISEARCH_H__ */
