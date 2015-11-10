#ifndef BCASTPSISEARCH_H__
#define BCASTPSISEARCH_H__


#ifdef __cplusplus
extern "C" {
#endif



typedef CELTH_BOOL (*bcastPsiSearch_PMT_callback)(CELTH_VOID *context, CELTH_U16 pmt_pid,CELTH_U16 pmt_program_num, const CELTH_U8 *pmt, CELTH_U32  pmtSize);


CELTH_INT bcastPsiSearch_getPAT(CELTH_VOID *buffer, CELTH_INT  bufferSize);


CELTH_CHAR	bcastPsiSearch_getPMTs(
	const CELTH_VOID *pat, CELTH_INT  patSize,
	bcastPsiSearch_PMT_callback callback, CELTH_VOID *context);


CELTH_VOID bcastPsiSearch_setTimeout( CELTH_INT patTimeout, CELTH_INT pmtTimeout );

CELTH_VOID bcastPsiSearch_getTimeout( CELTH_INT *patTimeout, CELTH_INT *pmtTimeout );


CELTH_U8 bcastPsiSearch_getEGBTChannelInfo( emerge_pmttable_info *p_emergetblechanInfo);

CELTH_BOOL bcastSearch_CheckPmtData(CELTH_U8* pmt,CELTH_U16 pmt_pid,CELTH_U16 pmt_program_num, CELTH_INT pmtSize,emerge_pmttable_info* pmttable);



/*CELTH_VOID bcastPsiSearch_parseEGBT(CELTH_CONST CELTH_VOID *egbt, CELTH_U32 egbtSize, emerge_channel_info *p_emergechanInfo);*/





#ifdef __cplusplus
}
#endif

#endif /* BCASTPSISEARCH_H__ */



