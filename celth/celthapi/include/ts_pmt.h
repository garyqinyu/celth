/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ts_pmt.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:30p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/ts_pmt.h $
 * 
 * 1   2/7/05 11:30p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   5/4/04 1:52p erickson
 * PR10938: added bounds checking and PMT and PAT validation in order to
 * prevent segfaults on bad data
 *
 * Irvine_BSEAVSW_Devel/2   9/8/03 10:05a marcusk
 * Added routines to get the number of things.
 *
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:05p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef TS_PMT_H__
#define TS_PMT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint8_t		stream_type;
	uint16_t	elementary_PID;
} TS_PMT_stream;

/**
Returns true if it's a valid PMT
The other TS_PMT functions assume you have validated the buffer beforehand.
**/
bool TS_PMT_validate(const uint8_t *buf, unsigned bfrSize);

uint16_t TS_PMT_getPcrPid( const uint8_t *buf, unsigned bfrSize);
TS_PSI_descriptor TS_PMT_getDescriptor( const uint8_t *buf, unsigned bfrSize, int descriptorNum );

int TS_PMT_getNumStreams( const uint8_t *buf, unsigned bfrSize);
CELTH_Error_Code TS_PMT_getStream( const uint8_t *buf, unsigned bfrSize, int streamNum, TS_PMT_stream *p_stream );
TS_PSI_descriptor TS_PMT_getStreamDescriptor( const uint8_t *buf, unsigned bfrSize, int streamNum, int descriptorNum );

#ifdef __cplusplus
}
#endif
#endif
/* End of File */
