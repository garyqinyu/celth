/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ts_priv.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:30p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/ts_priv.c $
 * 
 * 1   2/7/05 11:30p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:02p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "ts_priv.h"

TS_PSI_descriptor TS_P_getDescriptor( const uint8_t *p_descBfr, uint32_t descriptorsLength, int descriptorNum )
{
	TS_PSI_descriptor descriptor;
	int i;
	uint32_t byteOffset = 0;

	descriptor = NULL;

	for( i = 0; byteOffset < descriptorsLength; i++ )
	{
		descriptor = &p_descBfr[byteOffset];
		byteOffset += p_descBfr[byteOffset+1] + 2;

		if( i == descriptorNum ) 
			break;
		else 
			descriptor = NULL;
	}

	return descriptor;
}
