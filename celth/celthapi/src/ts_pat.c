/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ts_pat.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 1/10/08 12:36p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/ts_pat.c $
 * 
 * 2   1/10/08 12:36p jtna
 * PR38353: Fix crash when tuning to QPSK channel. TS_PAT_getNumPrograms()
 * should not be returning signed values.
 * 
 * 1   2/7/05 11:29p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/5   7/6/04 3:53p erickson
 * PR11771: settop api dataflow redesign
 * 
 * Irvine_BSEAVSW_Devel/4   5/4/04 1:52p erickson
 * PR10938: added bounds checking and PMT and PAT validation in order to
 * prevent segfaults on bad data
 *
 * Irvine_BSEAVSW_Devel/3   2/4/04 9:56a erickson
 * PR9217: converted BDBG_ASSERT calls to CHECK calls. Don't assert on bad
 * data.
 *
 * Irvine_BSEAVSW_Devel/2   9/8/03 10:04a marcusk
 * Added routines to get the number of things.
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:05p marcusk
 * Initial Version.
 *
 ***************************************************************************/

#include "stdio.h"
#include "celth_stddef.h"
 
#include "bstd.h"
#include "ts_priv.h"
#include "ts_pat.h"

bool TS_PAT_validate(const uint8_t *buf, unsigned bfrSize)
{
	return (buf[0] == 0x00);
}

CELTH_Error_Code TS_PAT_getProgram( const uint8_t *buf, unsigned bfrSize, int programNum, TS_PAT_program *p_program )
{
	int byteOffset = TS_PSI_LAST_SECTION_NUMBER_OFFSET+1;

	byteOffset += programNum*4;

	if( byteOffset >= TS_PSI_MAX_BYTE_OFFSET(buf) || byteOffset >= (int)bfrSize)
	{
		return CELTHERR_ERROR_BAD_PARAMETER;
	}

	p_program->program_number = TS_READ_16( &buf[byteOffset] );
	p_program->PID = (uint16_t)(TS_READ_16( &buf[byteOffset+2] ) & 0x1FFF);

	return CELTHERR_SUCCESS;
}

uint8_t TS_PAT_getNumPrograms( const uint8_t *buf)
{
	int i;
	i=TS_PSI_MAX_BYTE_OFFSET(buf);
	printf("\nget the TS_MAX_BYTE_OFFSET[%x]\n",i);
	return (TS_PSI_MAX_BYTE_OFFSET(buf)-(TS_PSI_LAST_SECTION_NUMBER_OFFSET+1))/4;
}
