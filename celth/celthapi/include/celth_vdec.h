#ifndef	_CELTH_VIDEODEC_H_
#define	_CELTH_VIDEODEC_H_

/* Includes -------------------------------------------------------------------*/


#ifdef __cplusplus /* celth video decoder  */

extern "C" {

#endif



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




typedef enum {
    CELTH_VDEC_FRAMERATE_UNKNOWN = 0, 
    CELTH_VDEC_FRAMERATE_E23_976 = 1, 
    CELTH_VDEC_FRAMERATE_E24     = 2, 
    CELTH_VDEC_FRAMERATE_E25     = 3, 
    CELTH_VDEC_FRAMERATE_E29_97  = 4, 
    CELTH_VDEC_FRAMERATE_E30     = 5, 
    CELTH_VDEC_FRAMERATE_E50     = 6, 
    CELTH_VDEC_FRAMERATE_E59_94  = 7, 
    CELTH_VDEC_FRAMERATE_E60     = 8, 
    CELTH_VDEC_FRAMERATE_EMAX    = 9  
}CELTH_VDEC_FRAMERATE_e;

typedef enum
{
    CELTH_VDEC_LAST_FRAME,
    CELTH_VDEC_BLANK
}CELTH_VDEC_STOP_MODE_e;

typedef struct
{
    CELTH_U32 valid;               /*当前视频是否有效*/
    CELTH_U16 video_pid;           /*如果存在pid下发视频pid*/
    bvideo_codec_type codec_id;            /*下发视频对应codec种类，与CELTH_VDEC_START中附带参数相同*/
    CELTH_VDEC_FRAMERATE_e frame_rate;          /*帧率，对应CELTH_VDEC_FRAMERATE_e*/
    CELTH_U32 width;               /*视频宽度*/
    CELTH_U32 height;              /*视频高度*/
    CELTH_U32 private_len;         
    CELTH_U8   private_data[512];  
    CELTH_U32 stream_type;         /*当前数据的种类，对应CELTH_XMEM_PAYLOAD_TYPE，用于部分平台要详细区分注入数据的种类*/
}CELTH_VDEC_PARAM;

typedef struct{
    Celth_MemData_Type type;
    CELTH_U32                   timestamp; /*45K PTS，高32位*/
}CELTH_VDEC_STREAM_DESC;









typedef CELTH_INT(*CELTH_VDEC_NOTIFY)(CELTH_HANDLE vdec, CELTH_U32 type, CELTH_VOID *pParam, CELTH_U32 uParam);

CELTH_Error_Code  CelthApi_VDec_Init();
CELTH_VOID     		CelthApi_VDec_Exit();

CELTH_INT  CelthApi_VDec_GetNum();
CELTH_Error_Code  CelthApi_VDec_Register(CELTH_VDEC_NOTIFY func, CELTH_U32 type, VOID *pParam, CELTH_U32 uParam);

CELTH_HANDLE    CelthApi_VDec_Open (CELTH_INT index);
CELTH_Error_Code  CelthApi_VDec_Close(CELTH_HANDLE vdec);

CELTH_Error_Code  CelthApi_VDec_Push (CELTH_HANDLE vdec, CELTH_U8 *databuf,Celth_MemData_Type type);


CELTH_Error_Code  CelthApi_VDec_Config_Param(CELTH_HANDLE vdec,CELTH_VDEC_PARAM* pParam);


CELTH_Error_Code  CelthApi_VDec_SetPlaySource(CELTH_HANDLE vdec,CELTH_STREAM_SOURCE source);

CELTH_Error_Code  CelthApi_VDec_ClearPlaySource(CELTH_HANDLE vdec);

CELTH_Error_Code  CelthApi_VDec_Start(CELTH_HANDLE vdec);

CELTH_Error_Code  CelthApi_VDec_Stop(CELTH_HANDLE vdec);

CELTH_Error_Code  CelthApi_VDec_SetSyncMode(CELTH_HANDLE vdec,CELTH_BOOL bSyncWithAdec);







#ifdef __cplusplus /* celth video decoder */

}

#endif

#endif



