#ifndef _GBEMG_GENDEF_H_
#define _GBEMG_GENDEF_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/
 

#include "gbemg_stddef.h"



#ifdef __cplusplus
extern "C" {
#endif


#define GBEMG_PID 0x21

#define GBEMG_INDX_TABLEID 0xFD

#define GBEMG_CONT_TABELID 0xFE

#define GBEMG_REGISTER_TABLEID 0xFC
#define GBEMG_BOXCTRL_TABLEID     0xFB


#define GBEMG_CONT_TRANS_BYDETAILCHANNEL_FLAG 0x00

#define GBEMG_CONT_TRANS_BYSELFCHANNEL_FLAG 0x01


#define PMT_GBEMG_STREAM_TYPE 0x05 /*just for detail channel*/


#define GBEMG_CONT_TERRESTRIAL_TRANS_SYSTEM_DESCRIPTOR_TAG  0x5A

#define GBEMG_CONT_VIDEO_STREAM_DESCRIPTOR_TAG  0x02
#define GBEMG_CONT_AUDIO_STREAM_DESCRIPTOR_TAG  0x03

#define GBEMG_CONT_AVS_VIDEO_STREAM_DESCRIPTOR_TAG 0x3F

#define GBEMG_CONT_COPYRIGHT_DESCRIPTOR_TAG 0x0D



#define TS_READ_32( buf ) ((U32)((buf)[0]<<24|(buf)[1]<<16|(buf)[2]<<8|(buf)[3]))

#define TS_READ_24( buf ) ((U32)(((buf)[0]<<16|(buf)[1]<<8|(buf)[2])&0x00ffffff))

#define TS_READ_16( buf ) ((U16)((buf)[0]<<8|(buf)[1]))



typedef void(*IndxSetContentParamCallback)(U16 ContentPid,U16 EBMid );


/***************************************************************/
/* followiing is the emerg index define *************************/
/****************************************************************/

typedef struct GBEMG_Time_Duration_s
{
	U32 durationsec;
	
}GBEMG_Time_Duration_t;

typedef struct GBEMG_Time_s
{
	U16 year;
	U8  month;
	U8  day;
	U8  weekday;
	
	
	U8  hour;
	U8  minute;
	U8  second;
	
}GBEMG_Time_t;


typedef enum GBEMG_URGENT_LEVEL_e
{
  Level_One=15,
  Level_Two=11,
  Level_Three=7,
  Level_Four=3,
  Level_Test=0
	
}GBEMG_URGENT_LEVEL;



typedef enum GBEMG_AV_TYPE_e{
GBEMG_AUDIO =0,
GBEMG_VIDEO,
GBEMG_OTHER

}GBEMG_AV_TYPE;


typedef struct GBEMG_Area_s
{
}GBEMG_Area_t;


typedef struct GBEMG_Cover_Area_s
{
   U16  cover_area_number;
   GBEMG_Area_t* pcover_area;	
}GBEMG_Cover_Area_t;







typedef enum  GBEMG_FEC_e
{
	GBEMG_T_DVBT,
	GBEMG_T_DTMB
}GBEMG_T_FEC;

typedef enum  GBEMG_MOD_e
{
	GBEMG_T_MOD_unknewn,
}GBEMG_T_MOD;

typedef enum  GBEMG_FRAME_MODE_e
{
	GBEMG_T_FRAME_unknewn,
}GBEMG_T_FRAME_MODE;

typedef struct GBEMG_Terrestrial_Param_s
{
	U32 			centre_freq;
	GBEMG_T_FEC  		fec;
	GBEMG_T_MOD  		modulation;
	GBEMG_T_FRAME_MODE  	frame_mode;
}GBEMG_Terrestrial_Param_t;


typedef struct _GBEMG_Terl_Param_Node_t GBEMG_Terl_Param_Node;

struct _GBEMG_Terl_Param_Node_t
{
	GBEMG_Terl_Param_Node* pnext;
	GBEMG_Terrestrial_Param_t pterparam;
	
};








typedef enum GBEMG_Vcodec_type {

GBEMG_Vcodec_none =0,
GBEMG_Vcodec_unknown=0,
GBEMG_Vcodec_mpeg1 = 1,
GBEMG_Vcodec_mpeg2 = 2,
GBEMG_Vcodec_mpeg4_part2 = 0x10,
GBEMG_Vcodec_h263 = 0x1A,
GBEMG_Vcodec_h264 = 0x1B,
GBEMG_Vcodec_vc1 = 0xEA,
GBEMG_Vcodec_vc1_sm = 0xEB,
GBEMG_Vcodec_divx_311= 0x311,
GBEMG_Vcodec_avs = 0x42,
GBEMG_Vcodec_sorenson_h263 = 0xF0,
GBEMG_Vcodec_vp6 = 0xF1,
GBEMG_Vcodec_rv40 = 0xF2
}GBEMG_Vcodec_type;


typedef enum GBEMG_Acodec_type {
   GBEMG_Acodec_unknown = 0,           /* unknown/not supported audio format */
   GBEMG_Acodec_mpeg = 0x3,        /* MPEG1/2, layer 1/2. This does not support layer 3 (mp3). */
   GBEMG_Acodec_mp3 = 0x1,         /* MPEG1/2, layer 3. */
   GBEMG_Acodec_aac = 0xF,         /* Advanced audio coding. Part of MPEG-4 */
   GBEMG_Acodec_aac_plus = 0x11,   /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE) */
   GBEMG_Acodec_aac_plus_adts = 0x12,  /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with ADTS (Audio Data Transport Format) */
   GBEMG_Acodec_aac_plus_loas = 0x11,  /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with LOAS (Low Overhead Audio Stream) */
   GBEMG_Acodec_ac3 = 0x81,        /* Dolby Digital AC3 audio */
   GBEMG_Acodec_ac3_plus = 0x6,    /* Dolby Digital Plus (AC3+ or DDP) audio */
   GBEMG_Acodec_dts = 0x82,        /* Digital Digital Surround sound. */
   GBEMG_Acodec_lpcm_hddvd = 0x83, /* LPCM, HD-DVD mode */
   GBEMG_Acodec_lpcm_bluray = 0x84, /* LPCM, Blu-Ray mode */
   GBEMG_Acodec_dts_hd = 0x85,     /* Digital Digital Surround sound, HD */
   GBEMG_Acodec_wma_std = 0x86,    /* WMA Standard */
   GBEMG_Acodec_wma_pro = 0x87,    /* WMA Professional */
   GBEMG_Acodec_lpcm_dvd = 0x88,    /* LPCM, DVD mode */
   GBEMG_Acodec_avs = 0x43,         /* AVS Audio */
   GBEMG_Acodec_pcm,
   GBEMG_Acodec_amr,
   GBEMG_Acodec_dra = 0xda,
   GBEMG_Acodec_cook = 0xf0
} GBEMG_Acodec_type;





typedef struct GBEMG_STREAM_s
{
	GBEMG_AV_TYPE av_type;  /* av_type=0, mean audio, av_type=1, mean video av_type=? mean other*/
	U8 stream_type;
	U16 stream_pid;
}GBEMG_STREAM_t;

#define MAX_GBEMG_DETAIL_PROGRAM_PIDS	12


typedef struct 
{
	U16     eb_transport_stream_id;
	U16	eb_program_number;
	U16	detail_channel_pcr_pid;
	U8		num_video_pids;
	GBEMG_STREAM_t	video_pids[MAX_GBEMG_DETAIL_PROGRAM_PIDS];
	U8		num_audio_pids;
	GBEMG_STREAM_t	audio_pids[MAX_GBEMG_DETAIL_PROGRAM_PIDS];
	U8		num_other_pids;
	GBEMG_STREAM_t	other_pids[MAX_GBEMG_DETAIL_PROGRAM_PIDS];
} GBEMG_Detail_Channel_t;


/***********************************************************************************/
/*************the following is content define***************************************/
/***********************************************************************************/



typedef enum GBEMG_CaseAux_Data_Type_e
{
	/* the following is the Audio data*/
	GBEMG_DATA_MPEG1_LEVEL1_AUDIO_MPG    =1,
	GBEMG_DATA_MPEG1_LEVEL23_AUDIO_MP3   =2,
	
	/* the following is A/V data, option items*/
	GBEMG_DATA_MPEG2_AUDIO_VIDEO_MPG           =21,
	GBEMG_DATA_H264_AUDIO_VIDEO_264            =22,
	GBEMG_DATA_AVSP_AUDIO_VIDEO_AVS            =23,
	
	/* the following is the Graphics data*/
	
	GBEMG_DATA_GRAPHICS_PNG    = 41,
	GBEMG_DATA_GRAPHICS_JPEG   = 42,
	/* option item*/
	GBEMG_DATA_GRAPHICS_GIF    = 43,
	
	GBEMG_DATA_UNKNOWN 

}GBEMG_AUXDATA_TYPE;


typedef struct GBEMG_AuxData_s{

GBEMG_AUXDATA_TYPE aux_data_type;

U32                aux_data_length;

U8*                paux_data_buffer;
}GBEMG_AuxData_t;


typedef enum GBEMG_Multilingual_Type_e
{
	GBEMG_LANG_ENG, /* english*/
	GBEMG_LANG_CHI,/* chinese*/
	GBEMG_LANG_WIW,
	GBEMG_LANG_KEK,
	GBEMG_LANG_UNKONWN
	
}GBEMG_MULTILINGUAL_TYPE;







typedef enum GBEMG_Msg_Type_e{

GBEMG_MSG_eHandlerInit,
GBEMG_MSG_eCaptureDataType =0x0,
GBEMG_MSG_eCaptureIndxVersionChanged=0x1,

GBEMG_MSG_eActType = 0x80,
GBEMG_MSG_eActReset = 0x80,

GBEMG_MSG_eActArrial=0x81,
GBEMG_MSG_eActTerminal=0x82,


GBEMG_MSG_eBoxType = 0x8000,

GBEMG_MSG_eBoxRegister = 0x8000,




GBEMG_MSG_ePowerSwitch   = 0x8001,
/*GBEMG_MSG_eBoxTurnOff  = 0x8002,*/

GBEMG_MSG_eAmpSwitch   = 0x8004,
/*GBEMG_MSG_eAmpTurnOff  = 0x8008,*/

GBEMG_MSG_eUrgentTrain     = 0x8010,

GBEMG_MSG_eValumeCtrl      = 0x8020,
GBEMG_MSG_eSetTime         = 0x8040,

GBEMG_MSG_eSetReturnCircle = 0x8080,

GBEMG_MSG_eSetReturnIp     = 0x8100,
GBEMG_MSG_eRequestFile     = 0x8200,


GBEMG_MSG_eMax




}GBEMG_Msg_Type;



/* if the msg is capturedatatype, the msg_packet_length is zero*/

typedef struct GBEMG_Msg_s{

GBEMG_Msg_Type msg_type;
U32             msg_packet_length;
void*           msg_packet;
}GBEMG_Msg_t;




typedef struct GBEMG_PlayCaseRec_s{


	bool invalid;
	U16 emgid;

	GBEMG_URGENT_LEVEL emglevel;

	GBEMG_Time_t starttime;

	U32 durationsec;
	
}GBEMG_PlayCaseRec_t;




#ifdef __cplusplus
}
#endif

#else
#error "Multiple inclusions of gbemg_gendef.h"

#endif


