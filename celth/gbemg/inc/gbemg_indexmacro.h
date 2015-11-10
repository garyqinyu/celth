#ifndef _GBEMG_INDXMACRODEF_H_
#define _GBEMG_INDXMACRODEF_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/
 


#ifdef __cplusplus
extern "C" {
#endif







#define GBEMG_INDX_SECTION_TABLE_ID_OFFSET (0)
#define GBEMG_INDX_SECTION_LENGTH_OFFSET (1)
#define GBEMG_INDX_TABLEID_EX_OFFSET (3)
#define GBEMG_INDX_VERSION_NUM_OFFSET (5)
#define GBEMG_INDX_SECTION_NUM_OFFSET (6)
#define GBEMG_INDX_LAST_SECTION_NUM_OFFSET (7)
#define GBEMG_INDX_CURRENT_FREQ_INDICATOR_OFFSET (8)
#define GBEMG_INDX_EBM_NUM_OFFSET     (13)


#define GBEMG_INDX_SECTION_TABLE(buf) (U8)((buf[GBEMG_INDX_SECTION_TABLE_ID_OFFSET]&0xFF))
#define GBEMG_INDX_SECTION_LENGTH(buf) (TS_READ_16(&buf[GBEMG_INDX_SECTION_LENGTH_OFFSET])&0xFFF)

#define GBEMG_INDX_TABLEID_EX(buf)   (TS_READ_16(&buf[GBEMG_INDX_TABLEID_EX_OFFSET])&0xFFFF)

#define GBEMG_INDX_VERSION_NUM(buf)   (U8)((buf[GBEMG_INDX_VERSION_NUM_OFFSET]>>1)&0x1F)

#define GBEMG_INDX_SECTION_NUM(buf)   (U8)(buf[GBEMG_INDX_SECTION_NUM_OFFSET]&0xFF)


#define GBEMG_INDX_LAST_SECTION_NUM(buf) (U8)(buf[GBEMG_INDX_LAST_SECTION_NUM_OFFSET]&0xFF)

#define GBEMG_INDX_CURRENT_FREQ_INDICATOR(buf) (U8)(buf[GBEMG_INDX_CURRENT_FREQ_INDICATOR_OFFSET]&0x01)

#define GBEMG_INDX_EBM_NUM(buf)    (U8)(buf[GBEMG_INDX_EBM_NUM_OFFSET]&0xFF)


/***************************************************************************/
/****EB index content segment***********************************************/


#define GBEMG_INDX_EBM_CONT_BASE                       (14)
#define GBEMG_INDX_EBM_GENERATE_LENGTH                       (14)
#define GBEMG_INDX_CONT_EBM_LENGTH_OFFSET_FROM_BASE               (0)
#define GBEMG_INDX_CONT_EBM_ID_OFFSET_FROM_BASE                   (2)
#define GBEMG_INDX_CONT_EBM_ORIGINAL_NETWORKID_OFFSET_FROM_BASE   (4)
#define GBEMG_INDX_CONT_EBM_START_TIME_OFFSET_FROM_BASE           (6)
#define GBEMG_INDX_CONT_EBM_DURATION_TIME_OFFSET_FROM_BASE        (11)
#define GBEMG_INDX_CONT_EBM_TYPE_OFFSET_FROM_BASE                 (14)
#define GBEMG_INDX_CONT_EBM_LEVEL_OFFSET_FROM_BASE                (19)

#define GBEMG_INDX_CONT_EBM_COVER_AREA_NUM_OFFSET_FROM_BASE       (21)

#define GBEMG_INDX_CONT_EBM_TIME_COVER_AREA_LENGTH                (23)


/*the following items should relocation the offset of the emergence index table segment to the EBM_length*/

#define GBEMG_INDX_CONT_EBM_LENGTH(buf) (TS_READ_16(&buf[GBEMG_INDX_CONT_EBM_LENGTH_OFFSET_FROM_BASE])&0xFFFF)
#define GBEMG_INDX_CONT_EBM_ID(buf) (TS_READ_16(&buf[GBEMG_INDX_CONT_EBM_ID_OFFSET_FROM_BASE])&0xFFFF)
#define GBEMG_INDX_CONT_EBM_ORIGINAL_NETWORKID(buf) (TS_READ_16(&buf[GBEMG_INDX_CONT_EBM_ORIGINAL_NETWORKID_OFFSET_FROM_BASE])&0xFFFF)
#define GBEMG_INDX_CONT_EBM_LEVEL(buf)  (U8) (buf[GBEMG_INDX_CONT_EBM_LEVEL_OFFSET_FROM_BASE]&0x0F)





#define GBEMG_INDX_CONT_EBM_COVER_AREA_NUM(buf)    (TS_READ_16(&buf[GBEMG_INDX_CONT_EBM_COVER_AREA_NUM_OFFSET_FROM_BASE])&0xFFFF)


#define GBEMG_INDX_CONT_EBM_COVER_AREA_SEGMENT_LENGTH             (5)
#define GBEMG_INDX_CONT_EBM_COVER_AREA_OFFSET_FROM_AREA_LOOP_BASE             (0)



#define GBEMG_INDX_CONT_EBM_TRANS_STREAM_ID_OFFSET_FROM_AREA_END_BASE             (0)
#define GBEMG_INDX_CONT_EBM_PROGRAM_NUM_OFFSET_FROM_AREA_END_BASE                 (2)
#define GBEMG_INDX_CONT_EBM_PCR_PID_OFFSET_FROM_AREA_END_BASE                     (4)
#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_LENGTH_OFFSET_FROM_AREA_END_BASE         (6)


/* the following items should relocation the offest of the emergence index table segment to the EB_transport_stream_id*/
#define GBEMG_INDX_CONT_EBM_TRANS_STREAM_ID(buf) (TS_READ_16(&buf[GBEMG_INDX_CONT_EBM_TRANS_STREAM_ID_OFFSET_FROM_AREA_END_BASE])&0xFFFF)
#define GBEMG_INDX_CONT_EBM_PROGRAM_NUM(buf) (TS_READ_16(&buf[GBEMG_INDX_CONT_EBM_PROGRAM_NUM_OFFSET_FROM_AREA_END_BASE])&0xFFFF)
#define GBEMG_INDX_CONT_EBM_PCR_PID(buf)      (TS_READ_16(&buf[GBEMG_INDX_CONT_EBM_PCR_PID_OFFSET_FROM_AREA_END_BASE])&0x1FFF)
#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_LENGTH(buf)      (TS_READ_16(&buf[GBEMG_INDX_CONT_EBM_PROGRAM_INFO_LENGTH_OFFSET_FROM_AREA_END_BASE])&0x0FFF)

#define GBEMG_INDX_CONT_EBM_TRANS_STREAM_DESCRIPTOR_OFFSET   (8)


#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_DESCRIP_LENGTH         (13)

#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_DESCRIPTOR_TAG_OFFSET (0)
#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_DESCRIPTOR_LENGTH_OFFSET (1)
#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_CENTRE_FREQ_OFFSET (2)  

#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_FEC_MOD_OFFSET (6)

#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_FRAME_HEADER_OFFSET (7)


/* the following items should relocation the offset of the emergence index table segment to the trans_system descriptor*/

#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_DESCRIPTOR_TAG(buf) (U8)(buf[GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_DESCRIPTOR_TAG_OFFSET]&0xFF)
#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_DESCRIPTOR_LENGTH(buf) (U8)(buf[GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_DESCRIPTOR_LENGTH_OFFSET]&0xFF)
#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_CENTRE_FREQ(buf) (TS_READ_32(&buf[GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_CENTRE_FREQ_OFFSET])&0xFFFFFFFF)
#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_FEC(buf) (U8)((buf[GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_FEC_MOD_OFFSET]>>4)&0xF)
#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_MOD(buf) (U8)((buf[GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_FEC_MOD_OFFSET])&0xF)
#define GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_FRAME_HEADER(buf) (U8)((buf[GBEMG_INDX_CONT_EBM_PROGRAM_INFO_TRANS_T_SYSTEM_FRAME_HEADER_OFFSET])&0xF)



#define GBEMG_INDX_CONT_EBM_STREAM_TYPE_OFFSET_FROM_TRANS_DECSCRIPTOR_END (0)

#define GBEMG_INDX_CONT_EBM_STREAM_PID_OFFSET_FROM_TRANS_DECSCRIPTOR_END (1)

#define GBEMG_INDX_CONT_EBM_STREAM_ES_INFO_LENGTH_OFFSET_FROM_TRANS_DECSCRIPTOR_END (3)



#define GBEMG_11172_2_Video         0x01
#define GBEMG_13818_2_Video         0x02
#define GBEMG_11172_3_Audio         0x03
#define GBEMG_13818_3_Audio         0x04
#define GBEMG_13818_1_PrivateSection 0x05
#define GBEMG_13818_1_PrivatePES    0x06
#define GBEMG_13522_MHEG            0x07
#define GBEMG_13818_1_DSMCC         0x08
#define GBEMG_ITU_H222_1            0x09
#define GBEMG_13818_6_TypeA         0x0A
#define GBEMG_13818_6_TypeB         0x0B
#define GBEMG_13818_6_TypeC         0x0C
#define GBEMG_13818_6_TypeD         0x0D
#define GBEMG_13818_1_Aux           0x0E
#define GBEMG_13818_7_AAC           0x0F
#define GBEMG_14496_2_Video         0x10   
#define GBEMG_14496_3_Audio         0x11
#define GBEMG_14496_1_FlexMuxPES    0x12
#define GBEMG_14496_1_FlexMuxSection 0x13
#define GBEMG_13818_6_SyncDownload  0x14
#define GBEMG_MetadataPES           0x15
#define GBEMG_MetadataSection       0x16
#define GBEMG_MetadataDataCarousel  0x17
#define GBEMG_MetadataObjectCarousel 0x18
#define GBEMG_MetadataSyncDownload  0x19
#define GBEMG_13818_11_IPMP         0x1A
#define GBEMG_14496_10_Video        0x1B


#define GBEMG_AVS_Video             0x42
#define GBEMG_AVS_Audio             0x43
#define GBEMG_DRA_Audio             0xDA

#define GBEMG_ATSC_Video            0x80
#define GBEMG_ATSC_AC3              0x81
#define GBEMG_ATSC_DTS              0x86
#define GBEMG_ATSC_EAC3             0x87

#define GBEMG_SMPTE_VC1             0xEA




#define GBEMG_DT_VideoStream           0x02
#define GBEMG_DT_AudioStream           0x03
#define GBEMG_DT_Hierarchy             0x04
#define GBEMG_DT_Registration          0x05
#define GBEMG_DT_DataStreamAlign       0x06
#define GBEMG_DT_TargetBgGrid          0x07
#define GBEMG_DT_VideoWindow           0x08
#define GBEMG_DT_CA                    0x09
#define GBEMG_DT_ISO_639_Language      0x0A
#define GBEMG_DT_SystemClock           0x0B
#define GBEMG_DT_MultiplexBufferUtilization 0x0C
#define GBEMG_DT_Copyright             0x0D
#define GBEMG_DT_MaximumBitrate        0x0E
#define GBEMG_DT_PrivateDataIndicator  0x0F
#define GBEMG_DT_SmoothingBuffer       0x10
#define GBEMG_DT_STD                   0x11
#define GBEMG_DT_IBP                   0x12
#define GBEMG_DT_13818_6_DSMCC_1       0x13
#define GBEMG_DT_13818_6_DSMCC_2       0x14
#define GBEMG_DT_13818_6_DSMCC_3       0x15
#define GBEMG_DT_13818_6_DSMCC_4       0x16
#define GBEMG_DT_13818_6_DSMCC_5       0x17
#define GBEMG_DT_13818_6_DSMCC_6       0x18
#define GBEMG_DT_13818_6_DSMCC_7       0x19
#define GBEMG_DT_13818_6_DSMCC_8       0x1A
#define GBEMG_DT_MPEG4_Video           0x1B
#define GBEMG_DT_MPEG4_Audio           0x1C
#define GBEMG_DT_IOD                   0x1D
#define GBEMG_DT_SL                    0x1E
#define GBEMG_DT_FMC                   0x1F
#define GBEMG_DT_External_ES_ID        0x20
#define GBEMG_DT_MuxCode               0x21
#define GBEMG_DT_FmxBufferSize         0x22
#define GBEMG_DT_MultiplexBuffer       0x23
#define GBEMG_DT_ContentLabeling       0x24
#define GBEMG_DT_MetadataPointer       0x25
#define GBEMG_DT_Metadata              0x26
#define GBEMG_DT_MetadataSTD           0x27
#define GBEMG_DT_AVC                   0x28
#define GBEMG_DT_13818_11_IPMP         0x29
#define GBEMG_DT_AVCTimingHRD          0x2A
#define GBEMG_DT_MPEG2_AAC             0x2B
#define GBEMG_DT_FlexMuxTiming         0x2C

/* AVS video,can be assigned stream_type of 0x06(private  pes)
   in that case AVS video is identfied by unique descriptor tag.
   AVS Audio is not supported with private pes */
#define GBEMG_DT_AVS_Video             0x3F


#define GBEMG_DT_DVB_AC3               0x6A


#define GBEMG_DT_DVB_EnhancedAC3       0x7A
#define GBEMG_DT_DVB_DTS               0x7B
#define GBEMG_DT_DVB_AAC               0x7C
#define GBEMG_DT_DVB_DRA               0xA0








/*the following items should relocation the offset of the emergence index table segment to the end of trans_system descriptor */









#define GBEMG_INDX_CONT_EBM_STREAM_TYPE(buf) (U8)(buf[GBEMG_INDX_CONT_EBM_STREAM_TYPE_OFFSET_FROM_TRANS_DECSCRIPTOR_END]&0xFF)
#define GBEMG_INDX_CONT_EBM_STREAM_PID(buf)  (TS_READ_16(&buf[GBEMG_INDX_CONT_EBM_STREAM_PID_OFFSET_FROM_TRANS_DECSCRIPTOR_END])&0x1FFF)













#ifdef __cplusplus
}
#endif

#else
#error "Multiple inclusions of gbemg_indexmacro.h"

#endif
