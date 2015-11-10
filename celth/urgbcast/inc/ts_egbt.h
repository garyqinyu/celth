#ifndef TS_EGBT_H__
#define TS_EGBT_H__

#ifdef __cplusplus
extern "C" {
#endif


CELTH_BOOL Ts_Validate_EGBT(CELTH_CONST CELTH_U8 *egbt,CELTH_U32 egbtSize);

TS_PSI_descriptor Ts_EGBT_getPrivateDescriptor( CELTH_CONST CELTH_U8 *buf, CELTH_U32 egbtSize, CELTH_U8 descriptorNum );


CELTH_BOOL Ts_parseEGBT(CELTH_CONST CELTH_U8 *egbt, CELTH_U32 egbtSize, emerge_channel_info *p_emergechanInfo);

#ifdef __cplusplus
}
#endif

#endif
