/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ts_pat.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 1/10/08 12:35p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/ts_pat.h $
 * 
 * 2   1/10/08 12:35p jtna
 * PR38353: Fix crash when tuning to QPSK channel. TS_PAT_getNumPrograms()
 * should not be returning signed values.
 * 
 * 1   2/7/05 11:30p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   5/4/04 1:52p erickson
 * PR10938: added bounds checking and PMT and PAT validation in order to
 * prevent segfaults on bad data
 * 
 * Irvine_BSEAVSW_Devel/2   9/8/03 10:04a marcusk
 * Added routines to get the number of things.
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:05p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef TS_PAT_H__
#define TS_PAT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint16_t	program_number;
	uint16_t	PID;
} TS_PAT_program;

/**
Returns true if it's a valid PAT.
The other TS_PAT functions assume you have validated the buffer beforehand.
**/
bool TS_PAT_validate(const uint8_t *buf, unsigned bfrSize);

uint8_t TS_PAT_getNumPrograms( const uint8_t *buf);
CELTH_Error_Code TS_PAT_getProgram( const uint8_t *buf, unsigned bfrSize, int programNum, TS_PAT_program *p_program );

#ifdef __cplusplus
}
#endif
#endif
/* End of File */
