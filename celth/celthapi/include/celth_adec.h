
#ifndef _CELTH_AUDIODECODER_H_
#define _CELTH_AUDIODECODER_H_


#ifdef __cplusplus

extern "C" {

#endif



#include "celth_stddef.h"

#define CELTH_ADEC_PRIV_DATA_LEN    512


typedef enum  CELTH_ADEC_AUDIO_FORMAT_e
{
    CELTH_AUDIO_CODEC_DEFAULT,                 /* don¡¯t assigned by iPanel Middleware*/
    CELTH_AUDIO_CODEC_MPEG           = 0x3,    /* MPEG1/2, layer 1/2. */
    CELTH_AUDIO_CODEC_MP3            = 0x4,    /* MPEG1/2, layer 3. */
    CELTH_AUDIO_CODEC_AAC            = 0xF,    /* Advanced audio coding. Part of MPEG-4 */
    CELTH_AUDIO_CODEC_AAC_PLUS       = 0x11,   /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE)*/
    CELTH_AUDIO_CODEC_AC3            = 0x81,   /* Dolby Digital AC3 audio */
    CELTH_AUDIO_CODEC_AC3_PLUS       = 0x6,    /* Dolby Digital Plus (AC3+ or DDP) audio */
    CELTH_AUDIO_CODEC_WMAPRO         = 0x7,
    CELTH_AUDIO_CODEC_WMASTD         = 0x8,
    CELTH_AUDIO_CODEC_DTS            = 0x9,
    CELTH_AUDIO_CODEC_DTSHD          = 0xa,
    CELTH_AUDIO_CODEC_LPCMDVD        = 0xb,
    CELTH_AUDIO_CODEC_LPCMHDDVD      = 0xc,
    CELTH_AUDIO_CODEC_LPCMBLURAY     = 0xd,
    CELTH_AUDIO_CODEC_PCM            = 0xe,
    CELTH_AUDIO_CODEC_PCMWAV         = 0x20,
    CELTH_AUDIO_CODEC_RA144          = 0x30,
    CELTH_AUDIO_CODEC_RA288          = 0x31,
    CELTH_AUDIO_CODEC_COOK           = 0x32,
    CELTH_AUDIO_CODEC_SIPR           = 0x33,
    CELTH_AUDIO_CODEC_ATRAC3         = 0x34,
    CELTH_AUDIO_CODEC_VORBIS         = 0x40,
    CELTH_AUDIO_CODEC_AVS,
    CELTH_AUDIO_CODEC_DOBLY_TRUEHD	  = 0x83,
    CELTH_AUDIO_CODEC_UNDEFINED
} CELTH_ADEC_AUDIO_FORMAT;

typedef enum CELTH_ADEC_CHANNEL_OUT_MODE_e
{
    CELTH_AUDIO_MODE_STEREO            = 0,   
    CELTH_AUDIO_MODE_LEFT	       = 1,   
    CELTH_AUDIO_MODE_RIGHT             = 2,   
    CELTH_AUDIO_MODE_MIX_MONO          = 3,   /* mix left and right mono*/
    CELTH_AUDIO_MODE_STEREO_REVERSE    = 4    /* stereo covert*/
}CELTH_ADEC_CHANNEL_OUT_MODE; 

#if 0
typedef enum
{
    IPANEL_ADEC_SET_SOURCE            = 1,
    IPANEL_ADEC_START                 = 2,
    IPANEL_ADEC_STOP                  = 3,
    IPANEL_ADEC_PAUSE                 = 4,
    IPANEL_ADEC_RESUME                = 5,
    IPANEL_ADEC_CLEAR                 = 6,
    IPANEL_ADEC_SYNCHRONIZE           = 7,
    IPANEL_ADEC_SET_CHANNEL_MODE      = 8,
    IPANEL_ADEC_SET_MUTE              = 9,
    IPANEL_ADEC_SET_PASS_THROUGH      = 10,
    IPANEL_ADEC_SET_VOLUME            = 11,
    IPANEL_ADEC_GET_BUFFER_RATE       = 12,
    IPANEL_ADEC_SET_SYNC_OFFSET       = 13,
    IPANEL_ADEC_GET_BUFFER            = 14,
    IPANEL_ADEC_GET_BUFFER_CAP        = 15,
    IPANEL_ADEC_SET_CONFIG_PARAMS     = 16,
    IPANEL_ADEC_GET_CURRENT_PTS       = 17,
    IPANEL_ADEC_SET_DEVICE_OUTPUT     = 18,
    IPANEL_ADEC_SET_CODEC_TYPE        = 19,
    IPANEL_ADEC_GET_RECV_FRAME_NUM    = 20,        
    IPANEL_ADEC_GET_BIT_RATE          = 21,        
    IPANEL_ADEC_GET_CODEC_SUPPORT     = 22,
    IPANEL_ADEC_UNDEFINED
} IPANEL_ADEC_IOCTL_e;

#endif
typedef struct CELTH_ADEC_PARAM_t
{
    CELTH_UINT valid;              
    CELTH_UINT audio_pid;          
    CELTH_UINT codec_id;           
    CELTH_U16 channelcount;       
    CELTH_U16 samplesize;         
    CELTH_UINT samplerate;         
    CELTH_UINT profile;            
    CELTH_UINT private_len;        
    CELTH_U8   private_data[CELTH_ADEC_PRIV_DATA_LEN];
    CELTH_UINT stream_type;        
}CELTH_ADEC_PARAM;

typedef struct{
    Celth_MemData_Type type;
    CELTH_UINT                   timestamp;  /*45K PTS*/
}IPANEL_ADEC_STREAM_DESC;

typedef enum CELTH_ADEC_EVENT_e{
    CELTH_ADEC_NONE                        = 0x00,
    CELTH_ADEC_NORMAL                      = 0x00, /* audio decoder work in normal state*/
    CELTH_ADEC_HUNGER                      = 0x01  /* no data input the decoder, it hangged*/
}CELTH_ADEC_EVENT;

typedef CELTH_INT(*CELTH_ADEC_NOTIFY)(CELTH_HANDLE adec, CELTH_VOID *pparam,CELTH_UINT uiparam);

CELTH_VOID  CelthApi_AudDec_Init();
CELTH_VOID  CelthApi_AudDec_Exit();

CELTH_INT  CelthApi_AudDec_GetNum();
CELTH_Error_Code  CelthApi_AudDec_RegisterCallback(CELTH_ADEC_NOTIFY func, CELTH_UINT uparam);

CELTH_HANDLE CelthApi_AudDec_Open (CELTH_INT index);
CELTH_Error_Code  CelthApi_AudDec_Close(CELTH_HANDLE adec);
CELTH_Error_Code  CelthApi_AudDec_Push (CELTH_HANDLE adec, Celth_MemData_Type type,CELTH_INT datalen,CELTH_U8* databuf);

CELTH_Error_Code  CelthApi_AudDec_SetDecoderParam(CELTH_HANDLE adec,CELTH_ADEC_PARAM param );

CELTH_Error_Code  CelthApi_AudDec_SetDataSource(CELTH_HANDLE adec,CELTH_STREAM_SOURCE source );


CELTH_Error_Code CelthApi_AudDec_Start(CELTH_HANDLE adec);

CELTH_Error_Code CelthApi_AudDec_Stop(CELTH_HANDLE adec);




CELTH_Error_Code CelthApi_AudDec_SetMute(CELTH_HANDLE adec);

CELTH_Error_Code CelthApi_AudDec_SetSync(CELTH_HANDLE adec, CELTH_BOOL bSync);


#ifdef __cplusplus

}

#endif

#endif /*_CELTH_AUDIODECODER_H_*/

