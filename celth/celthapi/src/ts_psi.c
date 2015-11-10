/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ts_psi.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:30p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/ts_psi.c $
 * 
 * 1   2/7/05 11:30p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:05p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#include<stdio.h>
#include "bstd.h"
#include "ts_priv.h"
#include "ts_psi.h"

void TS_PSI_getSectionHeader( const uint8_t *buf, TS_PSI_header *p_header )
{
       printf("1 header\n");
	p_header->table_id = buf[TS_PSI_TABLE_ID_OFFSET];
	p_header->section_syntax_indicator = (buf[TS_PSI_SECTION_LENGTH_OFFSET]>>7)&1;
	p_header->private_indicator = (buf[TS_PSI_SECTION_LENGTH_OFFSET]>>6)&1;
	p_header->section_length = TS_PSI_GET_SECTION_LENGTH(buf);
	p_header->table_id_extension = (uint16_t)(TS_READ_16(&(buf)[TS_PSI_TABLE_ID_EXT_OFFSET] ) & 0x0FFF);
printf("in header\n");
	p_header->version_number = (uint8_t)((buf[TS_PSI_CNI_OFFSET]>>1)&0x1F);
	p_header->current_next_indicator = buf[TS_PSI_CNI_OFFSET]&1;
	p_header->section_number = buf[TS_PSI_SECTION_NUMBER_OFFSET];
	p_header->last_section_number = buf[TS_PSI_LAST_SECTION_NUMBER_OFFSET];
	p_header->CRC_32 = TS_READ_32( &(buf[p_header->section_length+TS_PSI_SECTION_LENGTH_OFFSET-4]) );
printf("fff header\n");
}
