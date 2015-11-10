/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// permedia2.hpp - Linear screen driver for 8-bpp (256 color) operation with
//  3DLabs Permedia video controller.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2000 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PERMEDIA2_
#define _PERMEDIA2_

/* Permedia2 Register Definitions */

/*********************************************************************************
**
** Permedia 2 registers
**
**********************************************************************************
*/

#define	P2_RST_STATUS		0x00	/* Reset Status */
#define	P2_INT_ENABLE		0x08	/* Interrupt Enable */
#define	P2_INT_FLAGS		0x10	/* Interrupt Flags */
#define P2_IN_FIFO_SPACE	0x18	/* space */
#define P2_OUT_FIFO_WORDS	0x20	/* words */
#define P2_DMA_ADDR		0x28	/* Inward DMA Address */
#define P2_DMA_COUNT		0x30	/* Inward DMA Count */
#define P2_ERROR_FLAGS		0x38	/* Error Flags */
#define	P2_VCLK_CTRL		0x40	/* VClk Control */
#define	P2_TEST			0x48	/* Test Register */
#define	P2_APERTURE_ONE		0x50	/* Aperture 1 Register */
#define	P2_APERTURE_TWO		0x58	/* Aperture 2 Register */
#define	P2_DMA_CTRL		0x60	/* DMA Control Register */
#define	P2_FIFO_DISCON		0x68	/* FIFO Disconnect Register */
#define	P2_CHIP_CONFIG		0x70	/* Chip Configuration */
#define	P2_OUT_DMA_ADDR		0x80	/* Outward DMA Address */
#define	P2_OUT_DMA_COUNT	0x88	/* Outward DMA Count */
#define	P2_AGP_TEX_ADDR		0x90	/* AGP Texture Base Addr */
#define	P2_BY_DMA_ADDR		0xA0	/* Bypass DMA Addr */
#define	P2_BY_DMA_STRIDE	0xB8	/* Bypass DMA Stride */
#define	P2_BY_DMA_MEM_ADDR	0xC0	/* Bypass DMA Mem Addr */
#define	P2_BY_DMA_SIZE		0xC8	/* Bypass DMA Size */
#define	P2_BY_DMA_BYTE_MASK	0xD0	/* Bypass DMA Byte Mask */
#define	P2_BY_DMA_CTRL		0xD8	/* Bypass DMA Control */
#define	P2_BY_DMA_COMPLETE	0xE8	/* Bypass DMA Complete */

/* Memory Control Registers */

#define	P2_REBOOT		0x1000	/* Reboot */
#define	P2_MEM_CTRL		0x1040	/* Memory Control */
#define	P2_BOOT_ADDR		0x1080	/* Boot Address */
#define	P2_MEM_CONFIG		0x10C0	/* Memory Config */
#define	P2_WRITE_MASK		0x1100	/* Bypass Write Mask */
#define	P2_FRAME_MASK		0x1140	/* Bypass Frame Buffer Mask */
#define	P2_COUNT		0x1180	/* Count */

/* Video Control Registers */

#define	P2_SCREEN_BASE		0x3000	/* Screen Base */
#define	P2_SCREEN_STRIDE	0x3008	/* Screen Stride */
#define	P2_HTOTAL		0x3010	/* Horizontal Total */
#define	P2_HGEND		0x3018	/* Horizontal Gate End */
#define	P2_HBEND		0x3020	/* Horizontal Blank End */
#define	P2_HSSTART		0x3028	/* Horizontal Sync Start */
#define	P2_HSEND		0x3030	/* Horizontal Sync End */
#define	P2_VTOTAL		0x3038	/* Vertical Total */
#define	P2_VBEND		0x3040	/* Vertical Blank End */
#define	P2_VSSTART		0x3048	/* Vertical Sync Start */
#define	P2_VSEND		0x3050	/* Vertical Sync End */
#define	P2_VIDEO_CTRL		0x3058	/* Video Control */
#define	P2_INT_LINE		0x3060	/* Interrupt Control */
#define	P2_DPY_DATA		0x3068	/* Display Data */
#define	P2_LINE_COUNT		0x3070	/* Line Count */
#define	P2_FIFO_CTRL		0x3078	/* FIFO Control */

/* RAMDAC direct registers */

#define	P2_RD_WRITE_ADDR	0x4000	/* Palette Write Address */
#define	P2_RD_DATA		0x4008	/* Palette Data */
#define	P2_RD_MASK		0x4010	/* Pixel Mask */
#define	P2_RD_READ_ADDR		0x4018	/* Read Address */
#define	P2_RD_CCOL_ADDR		0x4020	/* Cursor Colour Address */
#define	P2_RD_CCOL_DATA		0x4028	/* Cursor Colour Data */
#define	P2_RD_INDEX_DATA	0x4050	/* Indexed Data */
#define	P2_RD_CRAM_DATA		0x4058	/* Cursor RAM Data */
#define	P2_RD_C_X_LOW		0x4060	/* Cursor X Low */
#define	P2_RD_C_X_HI		0x4068	/* Cursor X High */
#define	P2_RD_C_Y_LOW		0x4070	/* Cursor Y Low */
#define	P2_RD_C_Y_HI		0x4078	/* Cursor Y High */

/* RAMDAC Indirect registers */

#define	P2_RD_CURSOR_CTRL	0x6	/* Cursor Control */
#define	P2_RD_COLOUR_MODE	0x18	/* Colour Mode */
#define	P2_RD_MODE_CTRL		0x19	/* Mode Control */
#define	P2_RD_PALETTE_PAGE	0x1C	/* Palette Page */
#define	P2_RD_MISC_CTRL		0x1E	/* Miscellaneous Control */
#define	P2_RD_PCLK_A1		0x20	/* Pixel Clock A1 */
#define	P2_RD_PCLK_A2		0x21	/* Pixel Clock A2 */
#define	P2_RD_PCLK_A3		0x22	/* Pixel Clock A3 */
#define	P2_RD_PCLK_B1		0x23	/* Pixel Clock B1 */
#define	P2_RD_PCLK_B2		0x24	/* Pixel Clock B2 */
#define	P2_RD_PCLK_B3		0x25	/* Pixel Clock B3 */
#define	P2_RD_PCLK_C1		0x26	/* Pixel Clock C1 */
#define	P2_RD_PCLK_C2		0x27	/* Pixel Clock C2 */
#define	P2_RD_PCLK_C3		0x28	/* Pixel Clock C3 */
#define	P2_RD_PCLK_STATUS	0x29	/* Pixel Clock Status */
#define	P2_RD_MCLK_1		0x30	/* Memory Clock Register 1 */
#define	P2_RD_MCLK_2		0x31	/* Memory Clock Register 2 */
#define	P2_RD_MCLK_3		0x32	/* Memory Clock Register 3 */
#define	P2_RD_MCLK_STATUS	0x33	/* Memory Clock Status */
#define	P2_RD_CKEY_CTRL		0x40	/* Colour Key Control */
#define	P2_RD_CKEY_OVERLAY	0x41	/* Colour Key Overlay */
#define	P2_RD_CKEY_RED		0x42	/* Colour Key Red */
#define	P2_RD_CKEY_GREEN	0x43	/* Colour Key Green */
#define	P2_RD_CKEY_BLUE		0x44	/* Colour Key Blue */

/* Extended register definitions */

#define	P2_X_SEQUENCER(index)	permedia2_reg->X_SEQ_INDEX = index; VGA_DELAY(); \
					permedia2_reg->X_SEQ_DATA

#define	P2_VGA_CTRL		0x05	/* VGA Control */

#define	P2_X_GRAPHICS(index)	permedia2_reg->X_GRA_INDEX = index; VGA_DELAY(); \
					permedia2_reg->X_GRA_DATA

#define	P2_MODE_640		0x09	/* Mode 640 Register */

#define	P2_RAMDAC(index)	permedia2_reg->RD_WRITE_ADDR = index; VGA_DELAY(); \
					permedia2_reg->RD_INDEX_DATA

/* other definitions for chk config */
#define _PPC1_	0
#define _PPC2_	1

//#define	PCI_READ	0
//#define	PCI_WRITE	1

/*
** Register Bit definitions.
*/

#define	P2_MEM_CTRL_DEFAULT	0x00000000
#define	P2_MEM_CONFIG_DEFAULT	0xFFFF9F65 

/* NICSTAR network operation registers  */
typedef struct _permedia2_registers 
{
/* 0000 - 0FFF : Control Status */

	DWORD	RST_STATUS;	/* R/W		Reset Status */
	DWORD	res1;
	DWORD	INT_ENABLE;	/* R/W		Interrupt Enable */
	DWORD	res1a;
	DWORD	INT_FLAGS;	/* R/W		Interrupt Flags */
	DWORD	res1b;
	DWORD IN_FIFO_SPACE;	/* R		Input FIFO space */
	DWORD res1c;
	DWORD OUT_FIFO_WORDS;	/* R		Output FIFO words */
	DWORD res1d;
	DWORD DMA_ADDR;	/* R/W		In DMA Address */
	DWORD res1e;
	DWORD DMA_COUNT;	/* R/W		In DMA Count */
	DWORD res1f;
	DWORD ERROR_FLAGS;	/* R/W		Error Flags */
	DWORD res1g;
	DWORD VCLK_CTRL;	/* R/W		VClk Control */
	DWORD res1h;
	DWORD TEST;		/* R/W		Test */
	DWORD res1i;
	DWORD APERTURE_ONE;	/* R/W		Aperture 1 */
	DWORD res1j;
	DWORD APERTURE_TWO;	/* R/W		Aperture 2 */
	DWORD res1k;
	DWORD DMA_CTRL;	/* R/W		DMA Control */
	DWORD res1l;
	DWORD FIFO_DISCON;	/* R/W		FIFO Disconnect */
	DWORD res1m;
	DWORD CHIP_CONFIG;	/* R/W		Chip Configuration */
	DWORD res1n[3];
	DWORD OUT_DMA_ADDR;	/* R/W		Out DMA Address */
	DWORD res1o;
	DWORD OUT_DMA_COUNT;	/* R/W		Out DMA Count */
	DWORD res1p;
	DWORD AGP_TEX_ADDR;	/* R/W		AGP Texture Base Addr */
	DWORD res1q[3];
	DWORD BY_DMA_ADDR;	/* R/W		Bypass DMA Addr */
	DWORD res1r[5];
	DWORD BY_DMA_STRIDE;	/* R/W		Bypass DMA Stride */
	DWORD res1s;
	DWORD BY_DMA_MEM_ADDR; /* R/W		Bypass DMA Mem Addr */
	DWORD res1t;
	DWORD BY_DMA_SIZE;	/* R/W		Bypass DMA Size */
	DWORD res1u;
	DWORD BY_DMA_BYTE_MASK; /* R/W	Bypass DMA Byte Mask */
	DWORD res1v;
	DWORD BY_DMA_CTRL;	/* R/W		Bypass DMA Control */
	DWORD res1w[3];
	DWORD BY_DMA_COMPLETE; /* R/W		Bypass DMA Complete */

	DWORD res1z[0x3C5];

/* 1000 - 1FFF : Memory Control */

	DWORD REBOOT;		/* W		Reboot */
	DWORD res2[0xF];
	DWORD MEM_CTRL;	/* R/W		Memory Control */
	DWORD res2a[0xF];
	DWORD BOOT_ADDR;	/* R/W		Boot Address */
	DWORD	res2b[0xF];
	DWORD	MEM_CONFIG;	/* R/W		Memory Config */
	DWORD	res2c[0xF];
	DWORD	WRITE_MASK;	/* R/W		Bypass Write Mask */
	DWORD	res2d[0xF];
	DWORD	FRAME_MASK;	/* R		Bypass Frame Mask */
	DWORD	res2e[0xF];
	DWORD	COUNT;		/* R		Count */

	DWORD res2z[0x79F];

/* 3000 - 3FFF : Video Control */

	DWORD	SCREEN_BASE;	/* R/W		Screen Base */
	DWORD	res3;
	DWORD	SCREEN_STRIDE;	/* R/W		Screen Stride */
	DWORD	res3a;
	DWORD	HTOTAL;		/* R/W		Horizontal Total */
	DWORD	res3b;
	DWORD	HGEND;		/* R/W		Horizontal Gate End */
	DWORD	res3c;
	DWORD	HBEND;		/* R/W		Horizontal Blank End */
	DWORD	res3d;
	DWORD	HSSTART;	/* R/W		Horizontal Sync Start */
	DWORD	res3e;
	DWORD	HSEND;		/* R/W		Horizontal Sync End */
	DWORD	res3f;
	DWORD	VTOTAL;		/* R/W		Vertical Total */
	DWORD	res3g;
	DWORD	VBEND;		/* R/W		Vertical Blank End */
	DWORD	res3h;
	DWORD	VSSTART;	/* R/W		Vertical Sync Start */
	DWORD	res3i;
	DWORD	VSEND;		/* R/W		Vertical Sync End */
	DWORD	res3j;
	DWORD	VIDEO_CTRL;	/* R/W		Video Control */
	DWORD	res3k;
	DWORD	INT_LINE;	/* R/W		Interrupt Control */
	DWORD	res3l;
	DWORD	DPY_DATA;	/* R/W		Display Data */
	DWORD	res3m;
	DWORD	LINE_COUNT;	/* R/W		Line Count */
	DWORD	res3n;
	DWORD	FIFO_CTRL;	/* R/W		FIFO Control */
	DWORD	res3o;
	DWORD	SBASE_RIGHT;	/* R/W		Screen Base Right */

	DWORD	res3z[0x3DF];

/* 4000 - 4FFF : RAMDAC */

	UCHAR	RD_WRITE_ADDR;	/* R/W		Palette Write Address */
	UCHAR	res4[7];
	UCHAR	RD_DATA;	/* R/W		Palette Data */
	UCHAR	res4a[7];
	UCHAR	RD_MASK;	/* R/W		Pixel Mask */
	UCHAR	res4b[7];
	UCHAR	RD_READ_ADDR;	/* R/W		Read Address */
	UCHAR	res4c[7];
	UCHAR	RD_CCOL_ADDR;	/* R/W		Cursor Colour Address */
	UCHAR	res4d[7];
	UCHAR	RD_CCOL_DATA;	/* R/W		Cursor Colour Data */
	UCHAR	res4e[39];
	UCHAR	RD_INDEX_DATA;	/* R/W		Indexed Data */
	UCHAR	res4f[7];
	UCHAR	RD_CRAM_DATA;	/* R/W		Cursor RAM Data */
	UCHAR	res4g[7];
	UCHAR	RD_C_X_LOW;	/* R/W		Cursor X Low */
	UCHAR	res4h[7];
	UCHAR	RD_C_X_HI;	/* R/W		Cursor X High */
	UCHAR	res4i[7];
	UCHAR	RD_C_Y_LOW;	/* R/W		Cursor Y Low */
	UCHAR	res4j[7];
	UCHAR	RD_C_Y_HI;	/* R/W		Cursor Y High */
	UCHAR	res4k[7];

	DWORD	res4z[0x5E0];

/* 5800 - 5FFF : Video Streams */

	DWORD	VS_CONFIG;	/* R/W Video Streams Config */
	DWORD	res5a;
	DWORD	VS_STATUS;	/* R/W Video Streams Status */
	DWORD	res5b;
	DWORD	VS_SBUS_CTRL;	/* R/W V Streams SBus Ctrl */
	DWORD	res5c[0x3B];

	DWORD	VSA_CTRL;	/* R/W Video SA Ctrl */
	DWORD	res5d;
	DWORD	VSA_INT;	/* R/W Video SA Interrupt */
	DWORD	res5e;
	DWORD	VSA_CUR_LINE;	/* R/W Video SA Current Line */
	DWORD	res5f;
	DWORD	VSA_VADDR_HOST;	/* R/W Video Host Address */
	DWORD	res5g;
	DWORD	VSA_VADDR_INDEX;/* R/W Video Index Address */
	DWORD	res5h;
	DWORD	VSA_VADDR_0;	/* R/W Video Address 0 */
	DWORD	res5i;
	DWORD	VSA_VADDR_1;	/* R/W Video Address 1 */
	DWORD	res5j;
	DWORD	VSA_VADDR_2;	/* R/W Video Address 2 */
	DWORD	res5k;
	DWORD	VSA_VSTRIDE;	/* R/W Video Stride */
	DWORD	res5l;
	DWORD	VSA_VSLINE;	/* R/W Video Start Line */
	DWORD	res5m;
	DWORD	VSA_VELINE;	/* R/W Video End Line */
	DWORD	res5n;
	DWORD	VSA_VSDATA;	/* R/W Video Start Data */
	DWORD	res5o;
	DWORD	VSA_VEDATA;	/* R/W Video End Data */
	DWORD	res5p;
	DWORD	VSA_VBIADDR_HOST; /* R/W VBI Address Host */
	DWORD	res5q;
	DWORD	VSA_VBIADDR_INDEX; /* R/W VBI Address Index */
	DWORD	res5r;
	DWORD	VSA_VBIADDR_0;	/* R/W VBI Address 0 */
	DWORD	res5s;
	DWORD	VSA_VBIADDR_1;	/* R/W VBI Address 1 */
	DWORD	res5t;
	DWORD	VSA_VBIADDR_2;	/* R/W VBI Address 2 */
	DWORD	res5u;
	DWORD	VSA_VBISTRIDE;	/* R/W VBI Stride */
	DWORD	res5v;
	DWORD	VSA_VBISLINE;	/* R/W VBI Start Line */
	DWORD	res5w;
	DWORD	VSA_VBIELINE;	/* R/W VBI End Line */
	DWORD	res5x;
	DWORD	VSA_VBISDATA;	/* R/W VBI Start Data */
	DWORD	res5y;
	DWORD	VSA_VBIEDATA;	/* R/W VBI End Data */
	DWORD	res5z;
	DWORD	VSA_FIFO_CTRL;	/* R/W FIFO Control */

	DWORD	res5aa[0x191];

/* 6000 - 6FFF : VGA Control */

	UCHAR	res6[0x3C4];
	UCHAR	X_SEQ_INDEX;	/* W		Sequencer Index */
	UCHAR	X_SEQ_DATA;	/* R/W		Sequencer Data */
	WORD	res6a;
	DWORD res6b;
	WORD res6c;
	UCHAR	X_GRA_INDEX;	/* W		Graphics Index */
	UCHAR	X_GRA_DATA;	/* R/W		Graphics Data */

	DWORD	res6d[0x30C];

/* 7000 - 7FFF : Reserved */

	DWORD res7a[0x400];

/* 8000 - 8FFF : Graphics Processor Registers */

/* 8000 - 807F : GP Regs - Major Group 00 */

	DWORD	GP_START_X_DOM;		/* Start X Dominant */
	DWORD	res8a;
	DWORD	GP_DELTA_X_DOM;		/* Delta X Dominant */
	DWORD	res8b;
	DWORD	GP_START_X_SUB;		/* Start X Subordinate */
	DWORD	res8c;
	DWORD	GP_DELTA_X_SUB;		/* Delta X Subordinate */
	DWORD	res8d;
	DWORD	GP_START_Y;		/* Start Y */
	DWORD	res8e;
	DWORD	GP_DELTA_Y;		/* Delta Y */
	DWORD	res8f;
	DWORD	GP_COUNT;		/* Count */
	DWORD	res8g;
	DWORD	GP_RENDER;		/* Render */
	DWORD	res8h;
	DWORD	GP_CONT_NEW_LINE;	/* Continue New Line */
	DWORD	res8i;
	DWORD	GP_CONT_NEW_DOM;	/* Continue New Dominant */
	DWORD	res8j;
	DWORD	GP_CONT_NEW_SUB;	/* Continue New Subordinate */
	DWORD	res8k;
	DWORD	GP_CONTINUE;		/* Continue */
	DWORD	res8l[3];
	DWORD	GP_BIT_MASK_PATTERN;	/* Bit Mask Pattern */
	DWORD	res8m[5];

/* 8080 - 80FF : GP Regs - Major Group 01 */

	DWORD	res8m1[8];
	DWORD	GP_RASTERIZER_MODE;	/* Rasterizer Mode */
	DWORD	res8n;
	DWORD	GP_Y_LIMITS;		/* Y Limits */
	DWORD	res8o[3];
	DWORD	GP_WAIT_COMPLETE;	/* Wait for Completion */
	DWORD	res8p[3];
	DWORD	GP_X_LIMITS;		/* X Limits */
	DWORD	res8q;
	DWORD	GP_RECT_ORIGIN;		/* Rectangle Origin */
	DWORD	res8r;
	DWORD	GP_RECT_SIZE;		/* Rectangle Size */
	DWORD	res8s[9];

/* 8100 - 817F : GP Regs - Major Group 02 */

	DWORD	res8s1[20];
	DWORD	GP_PACKED_DATA_LIMITS;	/* Packed Data Limits */
	DWORD	res8t[11];

/* 8180 - 81FF : GP Regs - Major Group 03 */

	DWORD	GP_SCISSOR_MODE;	/* Scissor Mode */
	DWORD	res8u;
	DWORD	GP_SCISSOR_MIN_XY;	/* Scissor Minimum XY */
	DWORD	res8v;
	DWORD	GP_SCISSOR_MAX_XY;	/* Scissor Maximum XY */
	DWORD	res8w;
	DWORD	GP_SCREEN_SIZE;		/* Screen Size */
	DWORD	res8x;
	DWORD	GP_AREA_STIPPLE_MODE;	/* Area Stipple Mode */
	DWORD	res8y[9];
	DWORD	GP_WINDOW_ORIGIN;	/* Window Origin */
	DWORD	res8z[13];

/* 8200 - 827F : GP Regs - Major Group 04 */

	DWORD	GP_AREA_STIPPLE_1;	/* Area Stipple Pattern 1 */
	DWORD	res8aa1;
	DWORD	GP_AREA_STIPPLE_2;	/* Area Stipple Pattern 2 */
	DWORD	res8aa2;
	DWORD	GP_AREA_STIPPLE_3;	/* Area Stipple Pattern 3 */
	DWORD	res8aa3;
	DWORD	GP_AREA_STIPPLE_4;	/* Area Stipple Pattern 4 */
	DWORD	res8aa4;
	DWORD	GP_AREA_STIPPLE_5;	/* Area Stipple Pattern 5 */
	DWORD	res8aa5;
	DWORD	GP_AREA_STIPPLE_6;	/* Area Stipple Pattern 6 */
	DWORD	res8aa6;
	DWORD	GP_AREA_STIPPLE_7;	/* Area Stipple Pattern 7 */
	DWORD	res8aa7;
	DWORD	GP_AREA_STIPPLE_8;	/* Area Stipple Pattern 8 */
	DWORD	res8aa8[17];

/* 8280 - 82FF : GP Regs - Major Group 05 */

	DWORD	res8aa81[32];

/* 8300 - 837F : GP Regs - Major Group 06 */

	DWORD	res8aa82[32];

/* 8380 - 83FF : GP Regs - Major Group 07 */

	DWORD	GP_TEXTURE_ADDR_MODE;	/* Texture Address Mode */
	DWORD	res8ab;
	DWORD	GP_S_START;		/* Texture S Start Value */
	DWORD	res8ac;
	DWORD	GP_DELTA_S_DX;		/* Delta S X Derivative */
	DWORD	res8ad;
	DWORD	GP_DELTA_S_DY_DOM;	/* Delta S Y Derivative Dom' */
	DWORD	res8ae;
	DWORD	GP_T_START;		/* Texture T Start Value */
	DWORD	res8af;
	DWORD	GP_DELTA_T_DX;		/* Delta T X Derivative */
	DWORD	res8ag;
	DWORD	GP_DELTA_T_DY_DOM;	/* Delta T Y Derivative Dom' */
	DWORD	res8ah;
	DWORD	GP_Q_START;		/* Texture Q Start Value */
	DWORD	res8ai;
	DWORD	GP_DELTA_Q_DX;		/* Delta Q X Derivative */
	DWORD	res8aj;
	DWORD	GP_DELTA_Q_DY_DOM;	/* Delta Q Y Derivative Dom' */
	DWORD	res8ak[13];

/* 8400 - 847F : GP Regs - Major Group 08 */

	DWORD	res8ak1[32];

/* 8480 - 84FF : GP Regs - Major Group 09 */

	DWORD	res8ak2[16];
	DWORD	GP_TEXEL_LUT_INDEX;	/* Index data for LUT */
	DWORD	res8al;
	DWORD	GP_TEXEL_LUT_DATA;	/* Data for Texture LUT */
	DWORD	res8am;
	DWORD	GP_TEXEL_LUT_ADDR;	/* Address of LUT */
	DWORD	res8an;
	DWORD	GP_TEXEL_LUT_XFER;	/* Transfer Data to LUT */
	DWORD	res8ao[9];

/* 8500 - 857F : GP Regs - Major Group 0A */

	DWORD	res8ao1[32];

/* 8580 - 85FF : GP Regs - Major Group 0B */

	DWORD	GP_TEXTURE_BASE_ADDR;	/* Texture Base Address */
	DWORD	res8ap;
	DWORD	GP_TEXTURE_MAP_FORMAT;	/* Texture Map Format */
	DWORD	res8aq;
	DWORD	GP_TEXTURE_DATA_FORMAT;	/* Texture Data Format */
	DWORD	res8ar[27];

/* 8600 - 867F : GP Regs - Major Group 0C */

	DWORD	GP_TEXEL_0;		/* Texel Value */
	DWORD	res8as[27];
	DWORD	GP_TEXTURE_READ_MODE;	/* Texture Read Mode */
	DWORD	res8at;
	DWORD	GP_TEXEL_LUT_MODE;	/* Texel LUT Mode */
	DWORD	res8au;

/* 8680 - 86FF : GP Regs - Major Group 0D */

	DWORD	GP_TEXTURE_COLOUR_MODE;	/* Texture Colour Mode */
	DWORD	res8av[3];
	DWORD	GP_FOG_MODE;		/* Fog Mode */
	DWORD	res8aw;
	DWORD	GP_FOG_COLOUR;		/* Fog Colour */
	DWORD	res8ax;
	DWORD	GP_F_START;		/* Fog Value */
	DWORD	res8ay;
	DWORD	GP_DELTA_F_DX;		/* Delta F X Derivative */
	DWORD	res8az;
	DWORD	GP_DELTA_F_DY_DOM;	/* Delta F Y Derivative Dom' */
	DWORD	res8ba[5];
	DWORD	GP_KS_START;		/* Ks Value */
	DWORD	res8bb;
	DWORD	GP_DELTA_KS_DX;		/* Delta Ks X Derivative */
	DWORD	res8bc;
	DWORD	GP_DELTA_KS_DY_DOM;	/* Delta Ks Y Derivative Dom' */
	DWORD	res8bd;
	DWORD	GP_KD_START;		/* Kd Value */
	DWORD	res8be;
	DWORD	GP_DELTA_KD_DX;		/* Delta Kd X Derivative */
	DWORD	res8bf;
	DWORD	GP_DELTA_KD_DY_DOM;	/* Delta Kd Y Derivative Dom' */
	DWORD	res8bg[3];

/* 8700 - 877F : GP Regs - Major Group 0E */

	DWORD	res8bg1[32];

/* 8780 - 87FF : GP Regs - Major Group 0F */

	DWORD	GP_R_START;		/* Red Value */
	DWORD	res8bh;
	DWORD	GP_DELTA_R_DX;		/* Delta Red X Derivative */
	DWORD	res8bi;
	DWORD	GP_DELTA_R_DY_DOM;	/* Delta Red Y Derivative Dom'*/
	DWORD	res8bj;
	DWORD	GP_G_START;		/* Green Value */
	DWORD	res8bk;
	DWORD	GP_DELTA_G_DX;		/* Delta Green X Derivative */
	DWORD	res8bl;
	DWORD	GP_DELTA_G_DY_DOM;	/* Delta Green Y Der'tive Dom'*/
	DWORD	res8bm;
	DWORD	GP_B_START;		/* Blue Value */
	DWORD	res8bn;
	DWORD	GP_DELTA_B_DX;		/* Delta Blue X Derivative */
	DWORD	res8bo;
	DWORD	GP_DELTA_B_DY_DOM;	/* Delta Blue Y Der'tive Dom'*/
	DWORD	res8bp;
	DWORD	GP_A_START;		/* Alpha Value */
	DWORD	res8bq[5];
	DWORD	GP_COLOUR_DDA_MODE;	/* Colour DDA Mode */
	DWORD	res8br;
	DWORD	GP_CONSTANT_COLOUR;	/* Constant Colour */
	DWORD	res8bs;
	DWORD	GP_COLOUR;		/* Colour */
	DWORD	res8bt[3];

/* 8800 - 887F : GP Regs - Major Group 10 */

	DWORD	res8bt1[4];
	DWORD	GP_ALPHA_BLEND_MODE;	/* Alpha Blend Mode */
	DWORD	res8bu;
	DWORD	GP_DITHER_MODE;		/* Dither Mode */
	DWORD	res8bv;
	DWORD	GP_FB_SW_WRITE_MASK;	/* FB S/W Write Mask */
	DWORD	res8bw;
	DWORD	GP_LOGICAL_OP_MODE;	/* Logical Operation Mode */
	DWORD	res8bx[21];

/* 8880 - 88FF : GP Regs - Major Group 11 */

	DWORD	GP_LB_READ_MODE;	/* LB Read Mode */
	DWORD	res8by;
	DWORD	GP_LB_READ_FORMAT;	/* LB Read Format */
	DWORD	res8bz;
	DWORD	GP_LB_SRC_OFFSET;	/* LB Source Offset */
	DWORD	res8ca;
	DWORD	GP_LB_DATA;		/* LB Data */
	DWORD	res8cb;
	DWORD	GP_LB_STENCIL;		/* LB Stencil */
	DWORD	res8cc;
	DWORD	GP_LB_DEPTH;		/* LB Depth */
	DWORD	res8cd;
	DWORD	GP_LB_WINDOW_BASE;	/* LB Window Base */
	DWORD	res8ce;
	DWORD	GP_LB_WRITE_MODE;	/* LB Write Mode */
	DWORD	res8cf;
	DWORD	GP_LB_WRITE_FORMAT;	/* LB Write Format */
	DWORD	res8cg[7];
	DWORD	GP_TEXTURE_DATA;	/* Texture Data */
	DWORD	res8ch;
	DWORD	GP_TEXTURE_DLOAD_OFF;	/* Texture Download Offset */
	DWORD	res8ci[5];

/* 8900 - 897F : GP Regs - Major Group 12 */

	DWORD	res8ci1[32];

/* 8980 - 89FF : GP Regs - Major Group 13 */

	DWORD	GP_WINDOW;		/* Window */
	DWORD	res8cj;
	DWORD	GP_STENCIL_MODE;	/* Stencil Mode */
	DWORD	res8ck;
	DWORD	GP_STENCIL_DATA;	/* Stencil Data */
	DWORD	res8cl;
	DWORD	GP_STENCIL;		/* Stencil */
	DWORD	res8cm;
	DWORD	GP_DEPTH_MODE;		/* Depth Mode */
	DWORD	res8cn;
	DWORD	GP_DEPTH;		/* Depth */
	DWORD	res8co;
	DWORD	GP_Z_START_U;		/* Z Start Upper */
	DWORD	res8cp;
	DWORD	GP_Z_START_L;		/* Z Start Lower */
	DWORD	res8cq;
	DWORD	GP_DELTA_Z_DX_U;	/* Delta Z X Derivative Upper */
	DWORD	res8cr;
	DWORD	GP_DELTA_Z_DX_L;	/* Delta Z X Derivative Lower */
	DWORD	res8cs;
	DWORD	GP_DELTA_Z_DY_DOM_U;	/* Delta Z X D'ive Dom' Upper */
	DWORD	res8ct;
	DWORD	GP_DELTA_Z_DY_DOM_L;	/* Delta Z X D'ive Dom' Lower */
	DWORD	res8cu[9];

/* 8A00 - 8A7F : GP Regs - Major Group 14 */

	DWORD	res8cu1[32];

/* 8A80 - 8AFF : GP Regs - Major Group 15 */

	DWORD	GP_FB_READ_MODE;	/* FB Read Mode */
	DWORD	res8cv;
	DWORD	GP_FB_SRC_OFFSET;	/* FB Source Offset */
	DWORD	res8cw;
	DWORD	GP_FB_PIXEL_OFFSET;	/* FB Pixel Offset */
	DWORD	res8cx;
	DWORD	GP_FB_COLOUR;		/* FB Colour */
	DWORD	res8cy;
	DWORD	GP_FB_DATA;		/* FB Data */
	DWORD	res8cz;
	DWORD	GP_FB_SRC_DATA;		/* FB Source Data */
	DWORD	res8da;
	DWORD	GP_FB_WINDOW_BASE;	/* FB Window Base */
	DWORD	res8db;
	DWORD	GP_FB_WRITE_MODE;	/* FB Write Mode */
	DWORD	res8dc;
	DWORD	GP_FB_HW_WRITE_MASK;	/* FB H/W Write Mask */
	DWORD	res8dd;
	DWORD	GP_FB_BLOCK_COLOUR;	/* FB Block Colour */
	DWORD	res8de;
	DWORD	GP_FB_READ_PIXEL;	/* FB Read Pixel */
	DWORD	res8df[11];

/* 8B00 - 8B7F : GP Regs - Major Group 16 */

	DWORD	res8df1[32];

/* 8B80 - 8BFF : GP Regs - Major Group 17 */

	DWORD	res8df2[32];

/* 8C00 - 8C7F : GP Regs - Major Group 18 */

	DWORD	GP_FILTER_MODE;		/* Filter Mode */
	DWORD	res8dg;
	DWORD	GP_STATISTIC_MODE;	/* Statistic Mode */
	DWORD	res8dh;
	DWORD	GP_MIN_REGION;		/* Minimum Region */
	DWORD	res8di;
	DWORD	GP_MAX_REGION;		/* Maximum Region */
	DWORD	res8dj;
	DWORD	GP_RESET_PICK_RESULT;	/* Reset Pick Result */
	DWORD	res8dk;
	DWORD	GP_MIN_HIT_REGION;	/* Minimum Hit Region */
	DWORD	res8dl;
	DWORD	GP_MAX_HIT_REGION;	/* Maximum Hit Region */
	DWORD	res8dm;
	DWORD	GP_PICK_RESULT;		/* Pick Result */
	DWORD	res8dn;
	DWORD	GP_SYNC;		/* Sync */
	DWORD	res8do[9];
	DWORD	GP_FB_BLOCK_COLOUR_U;	/* FB Block Colour Upper */
	DWORD	res8dp;
	DWORD	GP_FB_BLOCK_COLOUR_L;	/* FB Block Colour Lower */
	DWORD	res8dq;
	DWORD	GP_SUSPEND_FBLANK;	/* Suspend Until Frame Blank */
	DWORD	res8dr;

/* 8C80 - 8CFF : GP Regs - Major Group 19 */

	DWORD	res8dr1[32];

/* 8D00 - 8D7F : GP Regs - Major Group 1A */

	DWORD	res8dr2[32];

/* 8D80 - 8DFF : GP Regs - Major Group 1B */

	DWORD	GP_FB_SRC_BASE;		/* FB Source Base */
	DWORD	res8ds;
	DWORD	GP_FB_SRC_DELTA;	/* FB Source Delta */
	DWORD	res8dt;
	DWORD	GP_CONFIG;		/* Configuration */
	DWORD	res8du[27];

/* 8E00 - 8E7F : GP Regs - Major Group 1C */

	DWORD	res8du1[32];

/* 8E80 - 8EFF : GP Regs - Major Group 1D */

	DWORD	GP_TEXEL_LUT_0;		/* Texel LUT 0 */
	DWORD	res8dv1;
	DWORD	GP_TEXEL_LUT_1;		/* Texel LUT 1 */
	DWORD	res8dv2;
	DWORD	GP_TEXEL_LUT_2;		/* Texel LUT 2 */
	DWORD	res8dv3;
	DWORD	GP_TEXEL_LUT_3;		/* Texel LUT 3 */
	DWORD	res8dv4;
	DWORD	GP_TEXEL_LUT_4;		/* Texel LUT 4 */
	DWORD	res8dv5;
	DWORD	GP_TEXEL_LUT_5;		/* Texel LUT 5 */
	DWORD	res8dv6;
	DWORD	GP_TEXEL_LUT_6;		/* Texel LUT 6 */
	DWORD	res8dv7;
	DWORD	GP_TEXEL_LUT_7;		/* Texel LUT 7 */
	DWORD	res8dv8;
	DWORD	GP_TEXEL_LUT_8;		/* Texel LUT 8 */
	DWORD	res8dv9;
	DWORD	GP_TEXEL_LUT_9;		/* Texel LUT 9 */
	DWORD	res8dv10;
	DWORD	GP_TEXEL_LUT_10;	/* Texel LUT 10 */
	DWORD	res8dv11;
	DWORD	GP_TEXEL_LUT_11;	/* Texel LUT 11 */
	DWORD	res8dv12;
	DWORD	GP_TEXEL_LUT_12;	/* Texel LUT 12 */
	DWORD	res8dv13;
	DWORD	GP_TEXEL_LUT_13;	/* Texel LUT 13 */
	DWORD	res8dv14;
	DWORD	GP_TEXEL_LUT_14;	/* Texel LUT 14 */
	DWORD	res8dv15;
	DWORD	GP_TEXEL_LUT_15;	/* Texel LUT 15 */
	DWORD	res8dv16;

/* 800 - 8F7F : GP Regs - Major Group 1E */

	DWORD	GP_YUV_MODE;		/* YUV Mode */
	DWORD	res8dw;
	DWORD	GP_CHROMA_BOUND_U;	/* Chroma Upper Bound */
	DWORD	res8dx;
	DWORD	GP_CHROMA_BOUND_L;	/* Chroma Lower Bound */
	DWORD	res8dy;
	DWORD	GP_ALPHA_MAP_BOUND_U;	/* Alpha Map Upper Bound */
	DWORD	res8dz;
	DWORD	GP_ALPHA_MAP_BOUND_L;	/* Alpha Map Lower Bound */
	DWORD	res8ea[19];
	DWORD	GP_TEXTURE_ID;		/* Texture ID */
	DWORD	res8eb;
	DWORD	GP_TEXEL_LUT_ID;	/* Texel LUT ID */
	DWORD	res8ec;

/* 8F80 - 8FFF : GP Regs - Major Group 1F */

	DWORD	res8ec1[32];

/* 9000 - 907F : GP Regs - Major Group 20 */

	DWORD	GP_V0_FIXED_0;		/* Vertex 0 Data 0 */
	DWORD	res8ed1;
	DWORD	GP_V0_FIXED_1;		/* Vertex 0 Data 1 */
	DWORD	res8ed2;
	DWORD	GP_V0_FIXED_2;		/* Vertex 0 Data 2 */
	DWORD	res8ed3;
	DWORD	GP_V0_FIXED_3;		/* Vertex 0 Data 3 */
	DWORD	res8ed4;
	DWORD	GP_V0_FIXED_4;		/* Vertex 0 Data 4 */
	DWORD	res8ed5;
	DWORD	GP_V0_FIXED_5;		/* Vertex 0 Data 5 */
	DWORD	res8ed6;
	DWORD	GP_V0_FIXED_6;		/* Vertex 0 Data 6 */
	DWORD	res8ed7;
	DWORD	GP_V0_FIXED_7;		/* Vertex 0 Data 7 */
	DWORD	res8ed8;
	DWORD	GP_V0_FIXED_8;		/* Vertex 0 Data 8 */
	DWORD	res8ed9;
	DWORD	GP_V0_FIXED_9;		/* Vertex 0 Data 9 */
	DWORD	res8ed10;
	DWORD	GP_V0_FIXED_10;		/* Vertex 0 Data 10 */
	DWORD	res8ed11;
	DWORD	GP_V0_FIXED_11;		/* Vertex 0 Data 11 */
	DWORD	res8ed12;
	DWORD	GP_V0_FIXED_12;		/* Vertex 0 Data 12 */
	DWORD	res8ed13;
	DWORD	GP_V0_FIXED_13;		/* Vertex 0 Data 13 */
	DWORD	res8ed14[5];

/* 9080 - 90FF : GP Regs - Major Group 21 */

	DWORD	GP_V1_FIXED_0;		/* Vertex 1 Data 0 */
	DWORD	res8ee1;
	DWORD	GP_V1_FIXED_1;		/* Vertex 1 Data 1 */
	DWORD	res8ee2;
	DWORD	GP_V1_FIXED_2;		/* Vertex 1 Data 2 */
	DWORD	res8ee3;
	DWORD	GP_V1_FIXED_3;		/* Vertex 1 Data 3 */
	DWORD	res8ee4;
	DWORD	GP_V1_FIXED_4;		/* Vertex 1 Data 4 */
	DWORD	res8ee5;
	DWORD	GP_V1_FIXED_5;		/* Vertex 1 Data 5 */
	DWORD	res8ee6;
	DWORD	GP_V1_FIXED_6;		/* Vertex 1 Data 6 */
	DWORD	res8ee7;
	DWORD	GP_V1_FIXED_7;		/* Vertex 1 Data 7 */
	DWORD	res8ee8;
	DWORD	GP_V1_FIXED_8;		/* Vertex 1 Data 8 */
	DWORD	res8ee9;
	DWORD	GP_V1_FIXED_9;		/* Vertex 1 Data 9 */
	DWORD	res8ee10;
	DWORD	GP_V1_FIXED_10;		/* Vertex 1 Data 10 */
	DWORD	res8ee11;
	DWORD	GP_V1_FIXED_11;		/* Vertex 1 Data 11 */
	DWORD	res8ee12;
	DWORD	GP_V1_FIXED_12;		/* Vertex 1 Data 12 */
	DWORD	res8ee13;
	DWORD	GP_V1_FIXED_13;		/* Vertex 1 Data 13 */
	DWORD	res8ee14[5];

/* 9100 - 917F : GP Regs - Major Group 22 */

	DWORD	GP_V2_FIXED_0;		/* Vertex 2 Data 0 */
	DWORD	res8ef1;
	DWORD	GP_V2_FIXED_1;		/* Vertex 2 Data 1 */
	DWORD	res8ef2;
	DWORD	GP_V2_FIXED_2;		/* Vertex 2 Data 2 */
	DWORD	res8ef3;
	DWORD	GP_V2_FIXED_3;		/* Vertex 2 Data 3 */
	DWORD	res8ef4;
	DWORD	GP_V2_FIXED_4;		/* Vertex 2 Data 4 */
	DWORD	res8ef5;
	DWORD	GP_V2_FIXED_5;		/* Vertex 2 Data 5 */
	DWORD	res8ef6;
	DWORD	GP_V2_FIXED_6;		/* Vertex 2 Data 6 */
	DWORD	res8ef7;
	DWORD	GP_V2_FIXED_7;		/* Vertex 2 Data 7 */
	DWORD	res8ef8;
	DWORD	GP_V2_FIXED_8;		/* Vertex 2 Data 8 */
	DWORD	res8ef9;
	DWORD	GP_V2_FIXED_9;		/* Vertex 2 Data 9 */
	DWORD	res8ef10;
	DWORD	GP_V2_FIXED_10;		/* Vertex 2 Data 10 */
	DWORD	res8ef11;
	DWORD	GP_V2_FIXED_11;		/* Vertex 2 Data 11 */
	DWORD	res8ef12;
	DWORD	GP_V2_FIXED_12;		/* Vertex 2 Data 12 */
	DWORD	res8ef13;
	DWORD	GP_V2_FIXED_13;		/* Vertex 2 Data 13 */
	DWORD	res8ef14[5];

/* 9180 - 91FF : GP Regs - Major Group 23 */

	DWORD	GP_V0_FLOAT_0;		/* Vertex 0 Float Data 0 */
	DWORD	res8eg1;
	DWORD	GP_V0_FLOAT_1;		/* Vertex 0 Float Data 1 */
	DWORD	res8eg2;
	DWORD	GP_V0_FLOAT_2;		/* Vertex 0 Float Data 2 */
	DWORD	res8eg3;
	DWORD	GP_V0_FLOAT_3;		/* Vertex 0 Float Data 3 */
	DWORD	res8eg4;
	DWORD	GP_V0_FLOAT_4;		/* Vertex 0 Float Data 4 */
	DWORD	res8eg5;
	DWORD	GP_V0_FLOAT_5;		/* Vertex 0 Float Data 5 */
	DWORD	res8eg6;
	DWORD	GP_V0_FLOAT_6;		/* Vertex 0 Float Data 6 */
	DWORD	res8eg7;
	DWORD	GP_V0_FLOAT_7;		/* Vertex 0 Float Data 7 */
	DWORD	res8eg8;
	DWORD	GP_V0_FLOAT_8;		/* Vertex 0 Float Data 8 */
	DWORD	res8eg9;
	DWORD	GP_V0_FLOAT_9;		/* Vertex 0 Float Data 9 */
	DWORD	res8eg10;
	DWORD	GP_V0_FLOAT_10;		/* Vertex 0 Float Data 10 */
	DWORD	res8eg11;
	DWORD	GP_V0_FLOAT_11;		/* Vertex 0 Float Data 11 */
	DWORD	res8eg12;
	DWORD	GP_V0_FLOAT_12;		/* Vertex 0 Float Data 12 */
	DWORD	res8eg13;
	DWORD	GP_V0_FLOAT_13;		/* Vertex 0 Float Data 13 */
	DWORD	res8eg14[5];

/* 9200 - 927F : GP Regs - Major Group 24 */

	DWORD	GP_V1_FLOAT_0;		/* Vertex 1 Float Data 0 */
	DWORD	res8eh1;
	DWORD	GP_V1_FLOAT_1;		/* Vertex 1 Float Data 1 */
	DWORD	res8eh2;
	DWORD	GP_V1_FLOAT_2;		/* Vertex 1 Float Data 2 */
	DWORD	res8eh3;
	DWORD	GP_V1_FLOAT_3;		/* Vertex 1 Float Data 3 */
	DWORD	res8eh4;
	DWORD	GP_V1_FLOAT_4;		/* Vertex 1 Float Data 4 */
	DWORD	res8eh5;
	DWORD	GP_V1_FLOAT_5;		/* Vertex 1 Float Data 5 */
	DWORD	res8eh6;
	DWORD	GP_V1_FLOAT_6;		/* Vertex 1 Float Data 6 */
	DWORD	res8eh7;
	DWORD	GP_V1_FLOAT_7;		/* Vertex 1 Float Data 7 */
	DWORD	res8eh8;
	DWORD	GP_V1_FLOAT_8;		/* Vertex 1 Float Data 8 */
	DWORD	res8eh9;
	DWORD	GP_V1_FLOAT_9;		/* Vertex 1 Float Data 9 */
	DWORD	res8eh10;
	DWORD	GP_V1_FLOAT_10;		/* Vertex 1 Float Data 10 */
	DWORD	res8eh11;
	DWORD	GP_V1_FLOAT_11;		/* Vertex 1 Float Data 11 */
	DWORD	res8eh12;
	DWORD	GP_V1_FLOAT_12;		/* Vertex 1 Float Data 12 */
	DWORD	res8eh13;
	DWORD	GP_V1_FLOAT_13;		/* Vertex 1 Float Data 13 */
	DWORD	res8eh14[5];

/* 9280 - 92FF : GP Regs - Major Group 25 */

	DWORD	GP_V2_FLOAT_0;		/* Vertex 2 Float Data 0 */
	DWORD	res8ei1;
	DWORD	GP_V2_FLOAT_1;		/* Vertex 2 Float Data 1 */
	DWORD	res8ei2;
	DWORD	GP_V2_FLOAT_2;		/* Vertex 2 Float Data 2 */
	DWORD	res8ei3;
	DWORD	GP_V2_FLOAT_3;		/* Vertex 2 Float Data 3 */
	DWORD	res8ei4;
	DWORD	GP_V2_FLOAT_4;		/* Vertex 2 Float Data 4 */
	DWORD	res8ei5;
	DWORD	GP_V2_FLOAT_5;		/* Vertex 2 Float Data 5 */
	DWORD	res8ei6;
	DWORD	GP_V2_FLOAT_6;		/* Vertex 2 Float Data 6 */
	DWORD	res8ei7;
	DWORD	GP_V2_FLOAT_7;		/* Vertex 2 Float Data 7 */
	DWORD	res8ei8;
	DWORD	GP_V2_FLOAT_8;		/* Vertex 2 Float Data 8 */
	DWORD	res8ei9;
	DWORD	GP_V2_FLOAT_9;		/* Vertex 2 Float Data 9 */
	DWORD	res8ei10;
	DWORD	GP_V2_FLOAT_10;		/* Vertex 2 Float Data 10 */
	DWORD	res8ei11;
	DWORD	GP_V2_FLOAT_11;		/* Vertex 2 Float Data 11 */
	DWORD	res8ei12;
	DWORD	GP_V2_FLOAT_12;		/* Vertex 2 Float Data 12 */
	DWORD	res8ei13;
	DWORD	GP_V2_FLOAT_13;		/* Vertex 2 Float Data 13 */
	DWORD	res8ei14[5];

/* 9300 - 937F : GP Regs - Major Group 26 */

	DWORD	GP_DELTA_MODE;		/* Delta Mode */
	DWORD	res8ej;
	DWORD	GP_DRAW_TRIANGLE;	/* Draw Triangle */
	DWORD	res8ek;
	DWORD	GP_REPEAT_TRIANGLE;	/* Repeat Triangle */
	DWORD	res8el;
	DWORD	GP_DRAW_LINE_01;	/* Draw Line 01 */
	DWORD	res8em;
	DWORD	GP_DRAW_LINE_10;	/* Draw Line 10 */
	DWORD	res8en;
	DWORD	GP_REPEAT_LINE;		/* Repeat Line */

} PERMEDIA_REGS;

/* End Permedia2 Register Definitions */

/*--------------------------------------------------------------------------*/
// Select the resolution you want to run in-
/*--------------------------------------------------------------------------*/

//#define MODE800         // turn this on for 800x600 mode
#define MODE1024        // turn this on for 1024x768

#if defined(MODE800)
#define PEG_VIRTUAL_XSIZE 800
#define PEG_VIRTUAL_YSIZE 600
#elif defined(MODE1024)
#define PEG_VIRTUAL_XSIZE 1024
#define PEG_VIRTUAL_YSIZE 768
#endif

/*--------------------------------------------------------------------------*/
// FRAME_BUFFER_SIZE- calculated size of one complete frame buffer
/*--------------------------------------------------------------------------*/
#define FRAME_BUFFER_SIZE (((PEG_VIRTUAL_XSIZE * PEG_VIRTUAL_YSIZE) + 4095) & 0xfffff000)

/*--------------------------------------------------------------------------*/
// VID_MEM_SIZE- size of video memory available in bytes
/*--------------------------------------------------------------------------*/
#define VID_MEM_SIZE 0x200000

/*--------------------------------------------------------------------------*/
// USE_VID_MEM_MANAGER- turn this on to provide run-time video memory
// management. This makes it possible to create dynamic PegBitmaps that
// are in video memory, and display them using hardware BitBlt capability.
// This is useful for Sprite or animation display.
/*--------------------------------------------------------------------------*/
#define USE_VID_MEM_MANAGER

/*--------------------------------------------------------------------------*/
// DOUBLE_BUFFER- Define this only if you have enough memory available for
//    two complete frame buffers
/*--------------------------------------------------------------------------*/
//#define DOUBLE_BUFFER

/*--------------------------------------------------------------------------*/
// USE_PCI: Define this on if your controller is plugged into a PCI adapter
//    and you want to "find" the controller at power up. Otherwise, you can
//    simply pre- define the controller address below.
/*--------------------------------------------------------------------------*/
#define USE_PCI 

#if defined(USE_PCI)

#define PEGPCI_PERMEDIA

// If running on the RadStone PPC, define the following. This determines
// how we find the video controller on the PCI bus
#define USE_RADSTONEPPC

#else

// If not running with PCI, we need to #define the controller address:

#define VID_MEM_BASE 0x3c00000L     /*** SET FOR YOUR HARDWARE!! ***/
#endif

/*--------------------------------------------------------------------------*/
#define PlotPointView(x, y, c) \
        *(mpScanPointers[y] + x) = (UCHAR) c;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Permedia2Screen : public PegScreen
{
    public:
        Permedia2Screen(PegRect &);
        virtual ~Permedia2Screen();

        //---------------------------------------------------------
        // Public pure virtual functions in PegScreen:
        //---------------------------------------------------------

        void BeginDraw(PegThing *);
        void BeginDraw(PegThing *, PegBitmap *);
        void EndDraw(void);
        void EndDraw(PegBitmap *);
        void SetPointerType(UCHAR bType);
        void SetPointer(PegPoint);
        void Capture(PegCapture *Info, PegRect &Rect);
        COLORVAL GetPixelView(SIGNED x, SIGNED y)
        {
            return((COLORVAL) *(mpScanPointers[y] + x));
        }

        #ifdef PEG_IMAGE_SCALING
        COLORVAL GetBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap)
        {
            COLORVAL *pGet = pMap->pStart;
            pGet += pMap->wWidth * y;
            pGet += x;
            return *pGet;
        }
        void PutBitmapPixel(SIGNED x, SIGNED y, PegBitmap *pMap, COLORVAL cVal)
        {
            COLORVAL *pPut = pMap->pStart;
            pPut += pMap->wWidth * y;
            pPut += x;
            *pPut = cVal;
        }
        #endif

        void SetPalette(SIGNED iFirst, SIGNED iNum, const UCHAR *Get);
        UCHAR *GetPalette(DWORD *pPutSize);
        void ResetPalette(void);

        //---------------------------------------------------------
        // End public pure virtual functions.
        //---------------------------------------------------------

    protected:
    
        void HidePointer(void);
        UCHAR *GetVideoAddress(void);
        void ConfigureController(void);

    private:

        //---------------------------------------------------------
        // Protected pure virtual functions in PegScreen:
        //---------------------------------------------------------

        void DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
            PegFont *Font, SIGNED iCount, PegRect &Rect);
        void LineView(SIGNED xStart, SIGNED yStart, SIGNED xEnd, SIGNED yEnd,
            PegRect &Rect, PegColor Color, SIGNED wWidth);
        void BitmapView(const PegPoint Where, const PegBitmap *Getmap,
            const PegRect &View);
        void RectMoveView(PegThing *Caller, const PegRect &View,
            const SIGNED xMove, const SIGNED yMove);

        void DrawRleBitmap(const PegPoint Where, const PegRect View,
            const PegBitmap *Getmap);
        void HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
            COLORVAL Color, SIGNED wWidth);
        void VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
            COLORVAL Color, SIGNED wWidth);
        void HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos);
        void VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos);

        //---------------------------------------------------------
        // End protected pure virtual functions.
        //---------------------------------------------------------

       #ifdef DO_OUTLINE_TEXT
        void DrawOutlineText(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
            PegFont *Font, SIGNED iCount, PegRect &Rect);
       #endif

        UCHAR  muPalette[256 * 3];
        UCHAR  *mpVidMemBase;
        UCHAR  *mpVidRegBase;

        void WriteExtReg(int index, int val);
        UCHAR ReadExtReg(int index);
        void WaitNotBusy(void);
        void WriteFPReg(int index, int val);
        UCHAR ReadFPReg(int index);
        void WriteCRTCReg(int index, int val);
        int ReadCRTCReg(int index);
        UCHAR ReadSeqReg(int index);
        void WriteSeqReg(int index, int val);
        void WriteAttribReg(int rg, int val);
        void WriteGraphicsReg(int index, int val);

       #ifdef DOUBLE_BUFFER
        void MemoryToScreen(void);
       #endif
};

#endif



