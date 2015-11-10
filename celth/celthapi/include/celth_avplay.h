#ifndef __CELTH_AVPLAY_H
#define __CELTH_AVPLAY_H

#ifdef __cplusplus

extern "C" {

#endif

#include "celth_stddef.h"

typedef enum
{	/* Track */
	CELTHAV_TRACK_STEREO,

	CELTHAV_TRACK_LEFT,
	CELTHAV_TRACK_RIGHT,
	CELTHAV_TRACK_SWAP,
	CELTHAV_TRACK_MAX
}CELTHAV_Track_t;

typedef struct 
{	/* Video Window Parameters */
	signed short		Left;
	signed short		Top;
	int		Width;
	int		Height;
} CELTHAV_VideoWindowParams_t;  


typedef struct 
{	/* Audio Output Parameters */
    unsigned char	Volume;

	CELTHAV_Track_t  Track;
    
} CELTHAV_AudioOutputParams_t;




typedef enum bvido_codec_type {

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





CELTH_VOID  CelthApi_AV_Init();

CELTH_VOID CelthApi_AV_Play(unsigned char iDecoder,unsigned short VPID, unsigned short APID, unsigned short PCRPID, bvideo_codec_type vcodec,baudio_format_type aformat);

CELTH_VOID CelthApi_Av_Stop();

CELTH_VOID CelthApi_Av_ChangeVideoFormat();

CELTH_VOID CelthApi_Av_Stop_Audio();

CELTH_VOID CelthApi_Av_Stop_Video(unsigned char iDecoder);

CELTH_VOID CelthApi_Av_Start_Audio(unsigned short APID,baudio_format_type aformat);


CELTH_VOID CelthApi_Av_Start_Video(unsigned char iDecoder,unsigned short VPID,unsigned short PCRPID, bvideo_codec_type vcodec);







CELTH_VOID	CelthApi_Av_GetVideoWindowParams( unsigned char iDecoder, CELTHAV_VideoWindowParams_t *WindowParams );
CELTH_VOID	CelthApi_Av_SetVideoWindowParams( unsigned char iDecoder, CELTHAV_VideoWindowParams_t* WindowParams );

CELTH_VOID	CelthApi_Av_SetAudioOutputParams( CELTHAV_AudioOutputParams_t *AudioParams );

CELTH_VOID	CelthApi_Av_GetAudioOutputParams( CELTHAV_AudioOutputParams_t *AudioParams );


CELTH_VOID	CelthApi_Av_SetAudioMute(  );
CELTH_VOID	CelthApi_Av_SetAudioUnmute(  );
CELTH_VOID	CelthApi_Av_SetAudioVolume( unsigned char  Volume );
CELTH_VOID	CelthApi_Av_SetAudioTrack	( CELTHAV_Track_t eTrack );


unsigned char	CelthApi_Av_GetAudioVolume( );
CELTHAV_Track_t	CelthApi_Av_GetAudioTrack	(  );




CELTH_VOID CelthApi_Av_PlayLocalAudioFile(char* cFile,baudio_format_type DataType);

CELTH_VOID CelthApi_Av_PlayLocalVideoFile(char* cFile,bvideo_codec_type DataType);



CELTH_VOID CelthApi_Av_PlayLocalAudioData(unsigned char* ucDataBuf,baudio_format_type DataType,unsigned int iBufLen);

CELTH_VOID CelthApi_Av_PlayLocalVideoData(unsigned char* ucDataBuf,bvideo_codec_type DataType,unsigned int iBufLen);



#ifdef __cplusplus

}

#endif






#endif
