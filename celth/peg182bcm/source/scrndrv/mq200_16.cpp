/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// mq200_16.cpp - Linear 16 bit-per-pixel screen driver. This driver can be
//   configured to work with any screen resolution.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2000 Swell Software, Inc.
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// There are conditional sections of code delineated by #ifdef PEGWIN32. This
// is used ONLY for testing the driver. These sections of code can be removed
// to improve readability without affecting your target system.
//
// This driver is intended for systems which have direct, linear (i.e. not
// paged) access to the video frame buffer. This driver does not implement
// double buffering, although that could easily be added.
//
// All available configuration flags are found in the mq200_16.hpp
// header file.
//
// Known Limitations:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "stdlib.h"
#include "string.h"
#include "peg.hpp"
#include "mqregdef.hpp"

#if defined(USE_PCI)
#include "pegpci.hpp"
#endif

extern UCHAR DefPalette256[];

/*--------------------------------------------------------------------------*/
// PLL1 frequency and corresponding P/M/N parameters for DC00R register
// - make sure it's NOT above 95MHz
/*--------------------------------------------------------------------------*/
MQ_PLL_PARAM MQPLLParam[] = 
{
	{  18,	0x00ab0640 },   // 18.881 MHz
	{  25,	0x00a30930 },   // 25.175
	{  31,	0x00f50b30 },   // 31.5
	{  36,	0x00d20830 },   // 36.0
	{  40,	0x00e90830 },   // 40.0
	{  49,	0x00900820 },   // 49.5
	{  50,	0x00b20a20 },   // 50
	{  56,	0x00b60920 },   // 56.250
	{  65,	0x00fd0b20 },   // 65.0
	{  75,	0x00f30920 },   // 75.0
	{  78,	0x00cc0720 },   // 78.75
	{  83,	0x00f20820 },   // 83.0
	{  94,	0x007a0710 }    // 94.5
};       
#define MAX_MQPLL	(sizeof(MQPLLParam) / sizeof(MQPLLParam[0]))

/*--------------------------------------------------------------------------*/
// Init Parameters ....
/*--------------------------------------------------------------------------*/
MQ_INIT_PARAM MQInitParam =
{
	0x0ef2082a,
	0x0043E086,
	0x6D6AABFF,
	0x0000010d,
	0x00000001,
	0x05000271,
	0x00000071,
	0x00000000,
	0x00000000,
	0x00000000
};


/*--------------------------------------------------------------------------*/
// CRT timing parameters for each resolution
/*--------------------------------------------------------------------------*/
MQ_TIMING_PARAM MQTimingParam[] = 
{
	{  // 640 x 480 60Hz (25.175 MHz)
		640,480,60,							// X/Y/Freq
		(800-2) | (640L << 16),      	// HD Total + HD End
		(525-1) | ((480L-1) << 16),	// VD Total + VD End
		656 | (752L << 16),				// HS Start + HS End
		490 | (492L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		25,						// 25.175 PLLx frequency
	},

	{  // 640 x 480 72Hz (31.5 MHz)
		640,480,72,							// X/Y/Freq
		(832-2) | (640L << 16),      	// HD Total + HD End
		(520-1) | ((480L-1) << 16),	// VD Total + VD End
		//664 | (704L << 16),				// HS Start + HS End
		688 | (728L << 16),				// HS Start + HS End
		489 | (492L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		31,							// 31.5 PLLx frequency
	},

	{  // 640 x 480 75Hz (31.5 MHz)
		640,480,75,							// X/Y/Freq
		(840-2) | (640L << 16),      	// HD Total + HD End
		(500-1) | ((480L-1) << 16),	// VD Total + VD End
		//656 | (720L << 16),				// HS Start + HS End
		680 | (744L << 16),				// HS Start + HS End
		481 | (484L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		31,							// PLLx frequency
	},

	{  // 640 x 480 85Hz (36 MHz)
		640,480,85,							// X/Y/Freq
		(832-2) | (640L << 16),      	// HD Total + HD End
		(509-1) | ((480L-1) << 16),	// VD Total + VD End
		696 | (752L << 16),				// HS Start + HS End
		481 | (484L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		36,							// 36 PLLx frequency
	},

	{  // 800 x 600 60Hz (40 MHz)
		800,600,60,							// X/Y/Freq
		(1054-2) | (800L << 16),		// HD Total + HD End
		(628-1) | ((600L-1) << 16),	// VD Total + VD End
		839 | (967L << 16),				// HS Start + HS End
		601 | (605L << 16),				// VS Start + VS End
		BLANK_PED_ENABLE,
		40,							// PLLx frequency
	},
   
	{  // 800 x 600 72Hz 50 MHz)
		800,600,72,							// X/Y/Freq
		(1040-2) | (800L << 16),		// HD Total + HD End
		(666-1) | ((600L-1) << 16),	// VD Total + VD End
		856 | (976L << 16),				// HS Start + HS End
		637 | (643L << 16),				// VS Start + VS End
		BLANK_PED_ENABLE,
		50,							// PLLx frequency
	},
   
	{  // 800 x 600 75Hz (49.5 MHz)
		800,600,75,							// X/Y/Freq
		(1056-2) | (800L << 16),		// HD Total + HD End
		(625-1) | ((600L-1) << 16),	// VD Total + VD End
		816 | (896L << 16),				// HS Start + HS End
		601 | (604L << 16),				// VS Start + VS End
		BLANK_PED_ENABLE,
		49,							// PLLx frequency
	},
   
	{  // 800 x 600 85Hz (56.25 MHz)
		800,600,85,							// X/Y/Freq
		(1047-2) | (800L << 16),		// HD Total + HD End
		(631-1) | ((600L-1) << 16),	// VD Total + VD End
		832 | (896L << 16),				// HS Start + HS End
		601 | (604L << 16),				// VS Start + VS End
		BLANK_PED_ENABLE,
		56,							// PLLx frequency
	},
   
	{  // 1024 x 768 60Hz (65 MHz)
		1024,768,60,						// X/Y/Freq
		(1344-2) | (1024L << 16),		// HD Total + HD End
		(806-1) | ((768L-1) << 16),	// VD Total + VD End
		1048 | (1184L << 16),			// HS Start + HS End
		771 | (777L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		65,							// PLLx frequency
	},			

	{  // 1024 x 768 70Hz (75 MHz)
		1024,768,70,						// X/Y/Freq
		(1327-2) | (1024L << 16),		// HD Total + HD End
		(806-1) | ((768L-1) << 16),	// VD Total + VD End
		1047 | (1183L << 16),			// HS Start + HS End
		771 | (777L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		75,							// PLLx frequency
	},

	{  // 1024 x 768 75Hz (78.750 MHz)
		1024,768,75,						// X/Y/Freq
		(1312-2) | (1024L << 16),		// HD Total + HD End
		(806-1) | ((768L-1) << 16),	// VD Total + VD End
		1040 | (1136L << 16),			// HS Start + HS End
		769 | (772L << 16),				// VS Start + VS End
		BLANK_PED_ENABLE,
		78,							// PLLx frequency
	},

	{  // 1024 x 768 85Hz (94.5 MHz)
		1024,768,85,						// X/Y/Freq
		(1375-2) | (1024L << 16),		// HD Total + HD End
		(808-1) | ((768L-1) << 16),	// VD Total + VD End
		1072 | (1168L << 16),			// HS Start + HS End
		769 | (772L << 16),				// VS Start + VS End
		BLANK_PED_ENABLE,
		94,							// PLLx frequency
	}
};       

#define MAX_MQTIMING	(sizeof(MQTimingParam) / sizeof(MQTimingParam[0]))

//-----------------------------------------------------------------------
// Panel Data
//-----------------------------------------------------------------------

/*--------------------------------------------------------------------------*/
// Flat panel control register init values for various LCD panels
/*--------------------------------------------------------------------------*/

#ifdef LCD_ACTIVE


FP_CONTROL_STRUC FPControlData[] =
{
	// Type 1 : SSTN VGA 8Bit Color - 72Hz
	// - Sanyo SSTN 640x480 8-bit color interface
	//
	{	// Flat panel control
		640,
		480,

		// Flat panel timing
		(832-2) | (640L << 16),			// HD Total + HD End
		(520-1) | ((480L-1) << 16),	// VD Total + VD End
		688 | (728L << 16),				// HS Start + HS End
		489 | (492L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		31,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_SSTN
		| FP_COLOR
		| SSTN_8BITS_MONOCLR
		| DITHER_PATTERN_3
		| DITHER_BASE_4BITS
		| FRC_16LEVEL
		| 0x00400000
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		//| FP_SCLK_16mA
		//| FP_FD0_16mA
		//| FP_DATA_16mA
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0000
		,
		0x5ca1
	},

	// Type 2 : DSTN 16 Bit VGA Color - 72Hz
	// - Hitachi 8.2" SX21V001
	// - Sanyo 10.4" LM-CJ53-22NTK
	// - Sharp 10.4" LM64C35P
	//
	{	// Flat panel control
		640,
		480,

		// Flat panel timing
		(832-2) | (640L << 16),			// HD Total + HD End
		(520-1) | ((480L-1) << 16),			// VD Total + VD End
		688 | (728L << 16),				// HS Start + HS End
		489 | (492L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		31,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_DSTN
		| FP_COLOR
		| DSTN_16BITS_MONOCLR
		| DITHER_PATTERN_3
		| DITHER_BASE_4BITS
		| FRC_16LEVEL
		| 0x0c840000
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		//| FP_SCLK_16mA
		//| FP_FD0_16mA
		//| FP_DATA_16mA
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},

	// Type 3 : TFT 18 Bit VGA - 60Hz
	// - NEC 10.4" NL6448AC33-24
	//
	{	// Flat panel control
		640,
		480,

		// Flat panel timing
		(800-2) | (640L << 16),       	// HD Total + HD End
		(525-1) | ((480L-1) << 16),			// VD Total + VD End
		656 | (752L << 16),				// HS Start + HS End
		490 | (492L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		25,						// PLLx frequency

		// Flat panel Control
		FP_TYPE_TFT
		| FP_COLOR
		| TFT_18BITS_COLOR
		| DITHER_PATTERN_3
		| DITHER_BASE_6BITS 
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},

	// Type 4 : TFT 18 Bit SVGA - 60Hz
	// - Hitachi 12.1" 800x600 TX31D24VC1CAA
	//
	{	// Flat panel control
		800,
		600,

		// Flat panel timing
		(1054-2) | (800L << 16),		// HD Total + HD End
		(628-1) | ((600L-1) << 16),			// VD Total + VD End
		839 | (967L << 16),				// HS Start + HS End
		601 | (605L << 16),				// VS Start + VS End
		BLANK_PED_ENABLE,
		40,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_TFT
		| FP_COLOR
		| TFT_18BITS_COLOR
		| DITHER_PATTERN_3
		| DITHER_BASE_6BITS 
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		//| FHSYNC_ACTIVE_L
		//| FVSYNC_ACTIVE_L
		//| FP_FSCLK_MAX
		//| FP_FD2_MAX
		//| FP_DATA_MAX
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},

	// Type 5 : DSTN 16Bit SVGA Color Panel - 72Hz
	// - Hitachi 10.0" SX25S001
	// - Hitachi 12.1" SX25S003
	//
	{	// Flat panel control
		800,
		600,

		// Flat panel timing
		(1040-2) | (800L << 16),		// HD Total + HD End
		(666-1) | ((600L-1) << 16),			// VD Total + VD End
		856 | (976L << 16),				// HS Start + HS End
		637 | (643L << 16),				// VS Start + VS End
		BLANK_PED_ENABLE,
		50,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_DSTN
		| FP_COLOR
		| DSTN_16BITS_MONOCLR
		| DITHER_PATTERN_3
		| DITHER_BASE_4BITS
		| FRC_16LEVEL
		| 0x0c840000
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		//| FP_SCLK_16mA
		//| FP_FD0_16mA
		//| FP_DATA_16mA
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},

	// Type 6 : DSTN 8 Bit VGA Color - 72Hz
	//
	{	// Flat panel control
		640,
		480,

		// Flat panel timing
		(832-2) | (640L << 16),       	// HD Total + HD End
		(520-1) | ((480L-1) << 16),			// VD Total + VD End
		688 | (728L << 16),				// HS Start + HS End
		489 | (492L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		31,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_DSTN
		| FP_COLOR
		| DSTN_8BITS_MONOCLR
		| DITHER_PATTERN_3
		| DITHER_BASE_4BITS
		| FRC_16LEVEL
		| 0x0c840000
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		//| FP_SCLK_16mA
		//| FP_FD0_16mA
		//| FP_DATA_16mA
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},

	// Type 7 : SSTN VGA 16Bit Color - 72Hz
	//
	{	// Flat panel control
		640,
		480,

		// Flat panel timing
		(832-2) | (640L << 16),       	// HD Total + HD End
		(520-1) | ((480L-1) << 16),			// VD Total + VD End
		688 | (728L << 16),				// HS Start + HS End
		489 | (492L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		31,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_SSTN
		| FP_COLOR
		| SSTN_16BITS_MONOCLR
		| DITHER_PATTERN_3
		| DITHER_BASE_4BITS
		| FRC_16LEVEL
		| 0x00400000
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		//| FP_SCLK_16mA
		//| FP_FD0_16mA
		//| FP_DATA_16mA
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0000
		,
		0x5ca1
	},

	// Type 8 : SSTN VGA 8Bit Color - 60Hz
	// - Sanyo SSTN 640x480 8-bit color interface
	//
	{	// Flat panel control
		640,
		480,

		// Flat panel timing
		(800-2) | (640L << 16),       	// HD Total + HD End
		(525-1) | ((480L-1) << 16),			// VD Total + VD End
		656 | (752L << 16),				// HS Start + HS End
		490 | (492L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		25,						// PLLx frequency

		// Flat panel Control
		FP_TYPE_SSTN
		| FP_COLOR
		| SSTN_8BITS_MONOCLR
		| DITHER_PATTERN_3
		| DITHER_BASE_4BITS
		| FRC_16LEVEL
		| 0x00400000
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0000
		,
		0x5ca1
	},

	// Type 9 : DSTN 16 Bit VGA Color - 60Hz
	// - Hitachi 8.2" SX21V001
	// - Sanyo 10.4" LM-CJ53-22NTK
	// - Sharp 10.4" LM64C35P
	//
	{	// Flat panel control
		640,
		480,

		// Flat panel timing
		(800-2) | (640L << 16),       	// HD Total + HD End
		(525-1) | ((480L-1) << 16),			// VD Total + VD End
		656 | (752L << 16),				// HS Start + HS End
		490 | (492L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		25,						// PLLx frequency

		// Flat panel Control
		FP_TYPE_DSTN
		| FP_COLOR
		| DSTN_16BITS_MONOCLR
		| DITHER_PATTERN_3
		| DITHER_BASE_4BITS
		| FRC_16LEVEL
		| 0x0c840000
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},

	// Type 10 : DSTN 16Bit SVGA Color Panel - 60Hz
	// - Hitachi 10.0" SX25S001
	// - Hitachi 12.1" SX25S003
	// - Sanyo LM-FC53-22NTK
	//
	{	// Flat panel control
		800,
		600,

		// Flat panel timing
		(1054-2) | (800L << 16),		// HD Total + HD End
		(628-1) | ((600L-1) << 16),			// VD Total + VD End
		839 | (967L << 16),				// HS Start + HS End
		601 | (605L << 16),				// VS Start + VS End
		BLANK_PED_ENABLE,
		40,							// PLLx frequency

		0x0C1B4129
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0001
		,
		//0x5ca1
		0x80100000
	},

	// Type 11 : DSTN 24Bit XGA Color Panel - 60Hz
	// - Hitachi 12.1" SX25S003
	//
	{	// Flat panel control
		1024,
		768,

		// Flat panel timing
		(1344-2) | (1024L << 16),		// HD Total + HD End
		(806-1) | ((768L-1) << 16),			// VD Total + VD End
		1048 | (1184L << 16),			// HS Start + HS End
		771 | (777L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		65,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_DSTN
		| FP_COLOR
		| DSTN_24BITS_COLOR
		| DITHER_PATTERN_3
		| DITHER_BASE_4BITS
		| FRC_16LEVEL
		| 0x0c840000
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},

	// Type 12 : DSTN 16Bit XGA Color Panel - 60Hz
	// - Hitachi 12.1" SX25S003
	//
	{	// Flat panel control
		1024,
		768,

		// Flat panel timing
		(1344-2) | (1024L << 16),		// HD Total + HD End
		(806-1) | ((768L-1) << 16),			// VD Total + VD End
		1048 | (1184L << 16),			// HS Start + HS End
		771 | (777L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		65,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_DSTN
		| FP_COLOR
		| DSTN_16BITS_MONOCLR
		| DITHER_PATTERN_3
		| DITHER_BASE_4BITS
		| FRC_16LEVEL
		| 0x0c840000
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},

	// Type 13 : TFT 18Bit XGA - 60Hz
	// - Hitachi 12.1" 800x600 TX31D24VC1CAA
	//
	{	// Flat panel control
		1024,
		768,

		// Flat panel timing
		(1344-2) | (1024L << 16),		// HD Total + HD End
		(806-1) | ((768L-1) << 16),			// VD Total + VD End
		1048 | (1184L << 16),			// HS Start + HS End
		771 | (777L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		65,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_TFT
		| FP_COLOR
		| TFT_18BITS_COLOR
		| DITHER_PATTERN_3
		| DITHER_BASE_6BITS 
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| FHSYNC_ACTIVE_L
		| FVSYNC_ACTIVE_L
		//| FP_SCLK_16mA
		//| FP_DATA_16mA 
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},

	// Type 14 : TFT 24Bit XGA - 60Hz
	// - Hitachi 12.1" 800x600 TX31D24VC1CAA
	//
	{	// Flat panel control
		1024,
		768,

		// Flat panel timing
		(1344-2) | (1024L << 16),		// HD Total + HD End
		(806-1) | ((768L-1) << 16),			// VD Total + VD End
		1048 | (1184L << 16),			// HS Start + HS End
		771 | (777L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		65,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_TFT
		| FP_COLOR
		| TFT_24BITS_COLOR
		| DITHER_PATTERN_3
		| DITHER_BASE_6BITS 
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| FHSYNC_ACTIVE_L
		| FVSYNC_ACTIVE_L
		//| FP_SCLK_16mA
		//| FP_DATA_16mA 
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},
	
	// Type 15 : TFT 18 Bit SVGA - 60Hz (Similar to type 4)
	// - NEC 12.1" 800x600 TX31D24VC1CAA
	//
	{	// Flat panel control
		800,
		600,

		// Flat panel timing
		(1054-2) | (800L << 16),		// HD Total + HD End
		(628-1) | ((600L-1) << 16),			// VD Total + VD End
		839 | (967L << 16),				// HS Start + HS End
		601 | (605L << 16),				// VS Start + VS End
		BLANK_PED_ENABLE,
		40,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_TFT
		| FP_COLOR
		| TFT_18BITS_COLOR
		| DITHER_PATTERN_3
		| DITHER_BASE_6BITS 
		,

		// Flat panel pin control
		0x03000020
		//FSCLK_OUTPUT_ENABLE
		//| FHSYNC_ACTIVE_L
		//| FVSYNC_ACTIVE_L
		//| FP_FSCLK_MAX
		//| FP_FD2_MAX
		//| FP_DATA_MAX
		,

		// STN panel control
		0x00bd0001
		,
		0x5ca1
	},
	
	// Type 16 : SSTN VGA 8Bit Color - 90Hz
	// - Sharp LM8M64 SSTN 640x240 8-bit color interface
	//
	{	// Flat panel control
		640,
		240,

		// Flat panel timing
		(793-2) | (640L << 16),			// HD Total + HD End
		(262-1) | ((240L-1) << 16),		// VD Total + VD End
		647 | (704L << 16),				// HS Start + HS End
		245 | (246L << 16),				// VS Start + VS End
		HSYNC_POLARITY_LOW|VSYNC_POLARITY_LOW|BLANK_PED_ENABLE,
		25,							// PLLx frequency

		// Flat panel Control
		FP_TYPE_SSTN
		| FP_COLOR
		| SSTN_8BITS_MONOCLR
		| DITHER_PATTERN_1
		| DITHER_BASE_4BITS
		| FRC_16LEVEL
		| 0x00400000
		,

		// Flat panel pin control
		FSCLK_OUTPUT_ENABLE
		| SCLK_MASK
		//| FP_SCLK_16mA
		//| FP_FD0_16mA
		//| FP_DATA_16mA
		| FDE_ACTIVE_L	
		,

		// STN panel control
		0x00bd0000
		,
		0x5ca1
	}
};

/*--------------------------------------------------------------------------*/
// Flat panel FRC weight/pattern registers
/*--------------------------------------------------------------------------*/
FRC_CONTROL_STRUC FRCControlData[] =
{
	// FRC Pattern Data
		0x97A4C5F8,
		0x61E3DB02,
		0x3D0E6F52,
		0xCB4971A8,
		0x794A2B16,
		0x8F0D35EC,
		0xD3E081BC,
		0x25A79F46,
		0x1F2C4D70,
		0xE96B538A,
		0xB586E7DA,
		0x43C1F920,
		0xF1C2A39E,
		0x0785BD64,
		0x5B680934,
		0xAD2F17CE,
		0x794A2B16,
		0x8F0D35EC,
		0xD3E081BC,
		0x25A79F46,
		0x97A4C5F8,
		0x61E3DB02,
		0x3D0E6F52,
		0xCB4971A8,
		0xF1C2A39E,
		0x0785BD64,
		0x5B680934,
		0xAD2F17CE,
		0x1F2C4D70,
		0xE96B538A,
		0xB586E7DA,
		0x43C1F920,
	// FRC weight data
		0x80800000,
		0x88888420,
		0x94a49248,
		0xaaaaaa54,
		0x6b5b55ab,
		0x77776db7,
		0x7f7f7bdf,
		0xffff7fff
};

#endif          // LCD_ACTIVE if

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// CreatePegScreen- Called by startup code to instantiate the PegScreen
// class we are going to run with.
/*--------------------------------------------------------------------------*/

PegScreen *CreatePegScreen(void)
{
    PegRect Rect;
    Rect.Set(0, 0, PEG_VIRTUAL_XSIZE - 1, PEG_VIRTUAL_YSIZE - 1);
    PegScreen *pScreen = new MQ200Screen(Rect);
    return pScreen;
}

/*--------------------------------------------------------------------------*/
// Constructor- initialize video memory addresses
/*--------------------------------------------------------------------------*/

MQ200Screen::MQ200Screen(PegRect &Rect) : PegScreen(Rect)
{
    mdNumColors = 65535;

    mwHRes = Rect.Width();
    mwVRes = Rect.Height();

    mpScanPointers = new COLORVAL PEGFAR *[Rect.Height()];

    COLORVAL PEGFAR *CurrentPtr = (COLORVAL PEGFAR *) GetVideoAddress();

    for (SIGNED iLoop = 0; iLoop < Rect.Height(); iLoop++)
    {
        mpScanPointers[iLoop] = CurrentPtr;
        CurrentPtr += mwHRes;
    }

    mLastPointerPos.x = Rect.Width() / 2;
    mLastPointerPos.y = Rect.Height() / 2;
    mbPointerHidden = FALSE;
    mwDrawNesting = 0;

    ConfigureController();        // set up controller registers

    //SetPalette(0, 232, DefPalette256);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// GetVideoAddress- Returns the base address of the video frame buffer.
//
// There are different forms of the GetVideoAddress function. These
// different forms are for different operating systems and system architechtures.
//
// When running the MQ200 eval card in a PCI environment, we must
// find the card using PCI BIOS calls. If running in any other environment
// the base address is simply determined by a #define in the header file.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR *MQ200Screen::GetVideoAddress(void)
{
    UCHAR* pMem = NULL;

   #if defined(USE_PCI)
    PCI_CONFIG tPciConfig;
    if(PegPCIReadControllerConfig(PEGPCI_VGACLASS, PEGPCI_VENDORID, 
                                  PEGPCI_DEVICEID, &tPciConfig))
    {
        pMem = (UCHAR *) (tPciConfig.BaseAddr2 & 0xFFFF0000L);
        mpVidMemBase = pMem;

        pMem = (UCHAR *) (tPciConfig.BaseAddr1 & 0xFFFF0000L);
        mpVidRegBase = pMem;
        pMem = mpVidMemBase;
    }
   
   #else

    // Here for a direct connection. Use pre-defined addresses:

    pMem = (UCHAR *) VID_MEM_BASE;
    mpVidMemBase = pMem;
    mpVidRegBase = (UCHAR *) VID_REG_BASE;

   #endif

    mpVidMemBase = pMem;

   #ifdef DOUBLE_BUFFER
    pMem += mwHRes * mwVRes * 2;
   #endif

    return (pMem);
}


/*--------------------------------------------------------------------------*/
// Destructor
/*--------------------------------------------------------------------------*/
MQ200Screen::~MQ200Screen()
{
    delete mpScanPointers;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::BeginDraw(PegThing *)
{
    LOCK_PEG
    if (!mwDrawNesting)
    {
       #if defined(DOUBLE_BUFFER)
        // make sure bit-blit engine is done:

        BLIT_CHECK
        
       #endif

       #if defined(PEG_MOUSE_SUPPORT) && !defined(HARDWARE_CURSOR)
        if (miInvalidCount)
        {
            if (mInvalid.Overlap(mCapture.Pos()))
            {
                HidePointer();
                mbPointerHidden = TRUE;
            }
        }
       #endif
    }

    mwDrawNesting++;
}

/*--------------------------------------------------------------------------*/
// CreateBitmap- Override default version to create 16-bpp bitmap.
/*--------------------------------------------------------------------------*/
PegBitmap *MQ200Screen::CreateBitmap(SIGNED wWidth, SIGNED wHeight, BOOL bHasTrans)
{
   PegBitmap *pMap = NULL;

    if (wWidth && wHeight)
    {
        pMap = new PegBitmap;
        pMap->wWidth = wWidth;
        pMap->wHeight = wHeight;
        pMap->uFlags = BMF_HAS_TRANS;
        pMap->uBitsPix = 16;

        DWORD dSize = (DWORD) wWidth * (DWORD) wHeight;

       #ifdef USE_VID_MEM_MANAGER
        pMap->pStart = AllocBitmap(dSize * sizeof(COLORVAL));

        if (!pMap->pStart)  // out of video memory?
        {
            pMap->pStart = (UCHAR PEGFAR *) new COLORVAL[dSize];    // try local memory
        }
        else
        {
            pMap->uFlags = BMF_HAS_TRANS|BMF_SPRITE;
        }
       #else
        pMap->pStart = (UCHAR PEGFAR *) new COLORVAL[dSize];
       #endif

        if (!pMap->pStart)
        {
            delete pMap;
            return NULL;
        }
		if(bHasTrans)
		{
			memset(pMap->pStart, TRANSPARENCY, dSize * sizeof(COLORVAL));
		}
		else
		{
			memset(pMap->pStart, 0, dSize * sizeof(COLORVAL));
		}
    }
    return pMap;
}

/*--------------------------------------------------------------------------*/
void MQ200Screen::BeginDraw(PegThing *Caller, PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        return;
    }
    LOCK_PEG

    #if defined(DOUBLE_BUFFER)
    // make sure bit-blit engine is done:

    BLIT_CHECK

    #endif

    mpSaveScanPointers = mpScanPointers;

    if (pMap->wHeight && pMap->wWidth && pMap->pStart)
    {
        mpScanPointers = new COLORVAL PEGFAR *[pMap->wHeight];
        COLORVAL PEGFAR *CurrentPtr = (COLORVAL PEGFAR *) pMap->pStart;
        for (SIGNED iLoop = 0; iLoop < pMap->wHeight; iLoop++)
        {
            mpScanPointers[iLoop] = CurrentPtr;
            CurrentPtr += pMap->wWidth;
        }
        mVirtualRect.Set(0, 0, pMap->wWidth - 1, pMap->wHeight - 1);
        mbVirtualDraw = TRUE;
    }
}


/*--------------------------------------------------------------------------*/
void MQ200Screen::EndDraw()
{
    mwDrawNesting--;

    if (!mwDrawNesting)
    {
       #if defined(PEG_MOUSE_SUPPORT) && !defined(HARDWARE_CURSOR)
        if (mbPointerHidden)
        {
            SetPointer(mLastPointerPos);
            mbPointerHidden = FALSE;
        }
       #endif

       #ifdef DOUBLE_BUFFER
        MemoryToScreen();
       #endif

        while(miInvalidCount > 0)
        {
            miInvalidCount--;
            UNLOCK_PEG
        }
    }
    UNLOCK_PEG
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::EndDraw(PegBitmap *pMap)
{
    if (mbVirtualDraw)
    {
        mbVirtualDraw = FALSE;
        delete [] mpScanPointers;
        mpScanPointers = mpSaveScanPointers;
        UNLOCK_PEG
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::LineView(SIGNED wXStart, SIGNED wYStart, SIGNED wXEnd, 
    SIGNED wYEnd, PegRect &Rect, PegColor Color, SIGNED wWidth)
{
    if (wYStart == wYEnd)
    {
        HorizontalLine(Rect.wLeft, Rect.wRight, Rect.wTop, Color.uForeground, wWidth);
        return;
    }
    if (wXStart == wXEnd)
    {
        VerticalLine(Rect.wTop, Rect.wBottom, Rect.wLeft, Color.uForeground, wWidth);
        return;
    }

    SIGNED dx = abs(wXEnd - wXStart);
    SIGNED dy = abs(wYEnd - wYStart);

    if (((dx >= dy && (wXStart > wXEnd)) ||
        ((dy > dx) && wYStart > wYEnd)))
    {
        PEGSWAP(wXEnd, wXStart);
        PEGSWAP(wYEnd, wYStart);
    }

    SIGNED y_sign = ((int) wYEnd - (int) wYStart) / dy;
    SIGNED x_sign = ((int) wXEnd - (int) wXStart) / dx;
    SIGNED decision;

    SIGNED wCurx, wCury, wNextx, wNexty, wpy, wpx;

    if (Rect.Contains(wXStart, wYStart) &&
        Rect.Contains(wXEnd, wYEnd))
    {
	    if (dx >= dy)
	    {
	        for (wCurx = wXStart, wCury = wYStart, wNextx = wXEnd,
	             wNexty = wYEnd, decision = (dx >> 1);
	             wCurx <= wNextx; wCurx++, wNextx--, decision += dy)
	        {
	            if (decision >= dx)
	            {
	                decision -= dx;
	                wCury += y_sign;
	                wNexty -= y_sign;
	            }
	            for (wpy = wCury - wWidth / 2;
	                 wpy <= wCury + wWidth / 2; wpy++)
	            {
                    PlotPointView(wCurx, wpy, Color.uForeground);
	            }
	
	            for (wpy = wNexty - wWidth / 2;
	                 wpy <= wNexty + wWidth / 2; wpy++)
	            {
                    PlotPointView(wNextx, wpy, Color.uForeground);
	            }
	        }
	    }
	    else
	    {
	        for (wCurx = wXStart, wCury = wYStart, wNextx = wXEnd,
	                wNexty = wYEnd, decision = (dy >> 1);
	            wCury <= wNexty; wCury++, wNexty--, decision += dx)
	        {
	            if (decision >= dy)
	            {
	                decision -= dy;
	                wCurx += x_sign;
	                wNextx -= x_sign;
	            }
	            for (wpx = wCurx - wWidth / 2;
	                 wpx <= wCurx + wWidth / 2; wpx++)
	            {
                    PlotPointView(wpx, wCury, Color.uForeground);
	            }
	
	            for (wpx = wNextx - wWidth / 2;
	                 wpx <= wNextx + wWidth / 2; wpx++)
	            {
                    PlotPointView(wpx, wNexty, Color.uForeground);
	            }
	        }
	    }
    }
    else
    {
	    if (dx >= dy)
	    {
	        for (wCurx = wXStart, wCury = wYStart, wNextx = wXEnd,
	             wNexty = wYEnd, decision = (dx >> 1);
	             wCurx <= wNextx; wCurx++, wNextx--, decision += dy)
	        {
	            if (decision >= dx)
	            {
	                decision -= dx;
	                wCury += y_sign;
	                wNexty -= y_sign;
	            }
	            for (wpy = wCury - wWidth / 2;
	                 wpy <= wCury + wWidth / 2; wpy++)
	            {
	                if (wCurx >= Rect.wLeft &&
	                    wCurx <= Rect.wRight &&
	                    wpy >= Rect.wTop &&
	                    wpy <= Rect.wBottom)
	                {
	                    PlotPointView(wCurx, wpy, Color.uForeground);
	                }
	            }
	
	            for (wpy = wNexty - wWidth / 2;
	                 wpy <= wNexty + wWidth / 2; wpy++)
	            {
	                if (wNextx >= Rect.wLeft &&
	                    wNextx <= Rect.wRight &&
	                    wpy >= Rect.wTop &&
	                    wpy <= Rect.wBottom)
	                {
	                    PlotPointView(wNextx, wpy, Color.uForeground);
	                }
	            }
	        }
	    }
	    else
	    {
	        for (wCurx = wXStart, wCury = wYStart, wNextx = wXEnd,
	                wNexty = wYEnd, decision = (dy >> 1);
	            wCury <= wNexty; wCury++, wNexty--, decision += dx)
	        {
	            if (decision >= dy)
	            {
	                decision -= dy;
	                wCurx += x_sign;
	                wNextx -= x_sign;
	            }
	            for (wpx = wCurx - wWidth / 2;
	                 wpx <= wCurx + wWidth / 2; wpx++)
	            {
	                if (wpx >= Rect.wLeft &&
	                    wpx <= Rect.wRight &&
	                    wCury >= Rect.wTop &&
	                    wCury <= Rect.wBottom)
	                {
	                    PlotPointView(wpx, wCury, Color.uForeground);
	                }
	            }
	
	            for (wpx = wNextx - wWidth / 2;
	                 wpx <= wNextx + wWidth / 2; wpx++)
	            {
	                if (wpx >= Rect.wLeft &&
	                    wpx <= Rect.wRight &&
	                    wNexty >= Rect.wTop &&
	                    wNexty <= Rect.wBottom)
	                {
	                    PlotPointView(wpx, wNexty, Color.uForeground);
	                }
	            }
	        }
	    }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::HorizontalLine(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos,
    COLORVAL Color, SIGNED wWidth)
{
    SIGNED iLen = wXEnd - wXStart + 1;

    if (iLen <= 0)
    {
        return;
    }

    COLORVAL *Put;

#if 1               // this version uses bit-blit engine for speed

    DWORD *pReg;

    if (wWidth > 1)
    {
        // for wide horizontal lines, use the bitblit engine, this could be
        // a big rectangle fill:
       #ifdef DOUBLE_BUFFER
        wYPos += mwVRes;
       #endif

        pReg = (DWORD *) (mpVidRegBase + GE_BASE);
        WaitEngineFIFO(4);  // need 4 command FIFO entries

	    *(pReg + WIDTH_HEIGHT) = ((DWORD) wWidth << 16) | (wXEnd - wXStart + 1);
	    *(pReg + DEST_XY) = ((DWORD) wYPos << 16) | wXStart;
	    *(pReg + PAT_FG_COLOR) = Color;
	    *(pReg + DRAW_CMD) =  MONO_PATTERN|MONO_SOLID|DO_BITBLT|0xf0;
        return;
    }

    // here for a horizontal, 1-pixel hi line, just draw it directly:

    Put = mpScanPointers[wYPos] + wXStart;

    while(iLen--)
    {
        *Put++ = Color;
    }

#else
     
    while(wWidth-- > 0)
    {
        Put = mpScanPointers[wYPos] + wXStart;
        SIGNED iLen1 = iLen;

        while(iLen1--)
        {
            *Put++ = Color;
        }
        wYPos++;
    }
#endif
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::VerticalLine(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos,
    COLORVAL Color, SIGNED wWidth)
{
    while(wYStart <= wYEnd)
    {
        SIGNED lWidth = wWidth;
        COLORVAL *Put = mpScanPointers[wYStart] + wXPos;

        while (lWidth-- > 0)
        {
            *Put++ = Color;
        }
        wYStart++;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::HorizontalLineXOR(SIGNED wXStart, SIGNED wXEnd, SIGNED wYPos)
{
    COLORVAL *Put = mpScanPointers[wYPos] + wXStart;

    while (wXStart <= wXEnd)
    {
        *Put ^= 0xffff;
        Put += 2;
        wXStart += 2;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::VerticalLineXOR(SIGNED wYStart, SIGNED wYEnd, SIGNED wXPos)
{
    COLORVAL uVal;

    while (wYStart <= wYEnd)
    {
        COLORVAL *Put = mpScanPointers[wYStart] + wXPos;
        uVal = *Put;
        uVal ^= 0xffff;
        *Put = uVal;
        wYStart += 2;
    }
}

/*--------------------------------------------------------------------------*/
void MQ200Screen::Capture(PegCapture *Info, PegRect &CaptureRect)
{
    PegBitmap *pMap = Info->Bitmap();

    if (CaptureRect.wLeft < 0)
    {
        CaptureRect.wLeft = 0;
    }

    if (CaptureRect.wTop < 0)
    {
        CaptureRect.wTop = 0;
    }

    Info->SetPos(CaptureRect);
    LONG Size = pMap->wWidth * pMap->wHeight * 2;
    Info->Realloc(Size);

    COLORVAL *GetStart = mpScanPointers[CaptureRect.wTop] + CaptureRect.wLeft;

    // make room for the memory bitmap:

    pMap->uFlags = 0;         // raw format
    pMap->uBitsPix = 16;      // 16 bits per pixel

    // fill in the image with our captured info:

    COLORVAL *Put = (COLORVAL *) pMap->pStart;

    for (WORD wLine = 0; wLine < pMap->wHeight; wLine++)
    {
        COLORVAL *Get = GetStart;
        for (WORD wLoop = 0; wLoop < pMap->wWidth; wLoop++)
        {
            *Put++ = *Get++;
        }
        GetStart += mwHRes;
    }
    Info->SetValid(TRUE);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::BitmapView(const PegPoint Where, const PegBitmap *Getmap,
    const PegRect &View)
{
    if (IS_RLE(Getmap))
    {
        DrawRleBitmap(Where, View, Getmap);
    }
    else
    {
        if (Getmap->uBitsPix == 8)
        {
            Draw8BitBitmap(Where, View, Getmap);
        }
        else
        {
            COLORVAL *Get = (COLORVAL *) Getmap->pStart;
            Get += (View.wTop - Where.y) * Getmap->wWidth;
            Get += View.wLeft - Where.x;

            if (HAS_TRANS(Getmap))
            {
                for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
                {
                    COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
                    for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
                    {
                        COLORVAL uVal = *Get++;

                        if (uVal != Getmap->dTransColor)
                        {
                            *Put = uVal;
                        }
                        Put++;
                    }
                }
                Get += Getmap->wWidth - View.Width();
            }
            else
            {
                for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
                {
                    COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
                    for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
                    {
                        *Put++ = *Get++;
                    }
                }
                Get += Getmap->wWidth - View.Width();
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::Draw8BitBitmap(const PegPoint Where, const PegRect View,
    const PegBitmap *Getmap)
{
    
    UCHAR *Get = Getmap->pStart;
    Get += (View.wTop - Where.y) * Getmap->wWidth;
    Get += View.wLeft - Where.x;

    if (HAS_TRANS(Getmap))
    {
        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
            {
                UCHAR uVal = *Get++;
                if (uVal != Getmap->dTransColor)
                {
                    *Put = mcHiPalette[uVal];
                }
                Put++;
            }
            Get += Getmap->wWidth - View.Width();
        }
    }
    else
    {
        for (SIGNED wLine = View.wTop; wLine <= View.wBottom; wLine++)
        {
            COLORVAL *Put = mpScanPointers[wLine] + View.wLeft;
            for (SIGNED wLoop1 = View.wLeft; wLoop1 <= View.wRight; wLoop1++)
            {
                UCHAR uVal = *Get++;
                *Put++ = mcHiPalette[uVal];
            }
            Get += Getmap->wWidth - View.Width();
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::DrawRleBitmap(const PegPoint Where, const PegRect View,
    const PegBitmap *Getmap)
{
    UCHAR *Get = Getmap->pStart;
    UCHAR uVal;
    SIGNED uCount;

    SIGNED wLine = Where.y;

    uCount = 0;

    while (wLine < View.wTop)
    {
        uCount = 0;

        while(uCount < Getmap->wWidth)
        {
            uVal = *Get++;
            if (uVal & 0x80)
            {
                uVal = (uVal & 0x7f) + 1;
                uCount += uVal;
                Get += uVal;
            }
            else
            {
                Get++;
                uCount += uVal + 1;
            }
        }
        wLine++;
    }

    while (wLine <= View.wBottom)
    {
        COLORVAL *Put = mpScanPointers[wLine] + Where.x;
        SIGNED wLoop1 = Where.x;

        while (wLoop1 < Where.x + Getmap->wWidth)
        {
            uVal = *Get++;

            if (uVal & 0x80)        // raw packet?
            {
                uCount = (uVal & 0x7f) + 1;
                
                while (uCount--)
                {
                    uVal = *Get++;
                    if (wLoop1 >= View.wLeft &&
                        wLoop1 <= View.wRight &&
                        uVal != Getmap->dTransColor)
                    {
                        *Put = mcHiPalette[uVal];
                    }
                    Put++;
                    wLoop1++;
                }
            }
            else
            {
                uCount = uVal + 1;
                uVal = *Get++;

                if (uVal == 0xff)
                {
                    wLoop1 += uCount;
                    Put += uCount;
                }
                else
                {
                    while(uCount--)
                    {
                        if (wLoop1 >= View.wLeft &&
                            wLoop1 <= View.wRight)
                        {
                            *Put++ = mcHiPalette[uVal];
                        }
                        else
                        {
                            Put++;
                        }
                        wLoop1++;
                    }
                }
            }
        }
        wLine++;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::RectMoveView(PegThing *Caller, const PegRect &View,
     const SIGNED xMove, const SIGNED yMove)
{
    // use the bit-blit engine to do a fast rectmove:

    WORD wXDir, wYDir;

    if (!xMove && !yMove)
    {
        return;
    }

    PegRect Clip = View;

    if (!xMove)
    {
        if (yMove < 0)  // moving up?
        {
            wXDir = wYDir = 0;
            Clip.wTop += yMove;
        }
        else            // moving down
        {
            wXDir = 0;
            wYDir = 0x1000;
            Clip.wBottom += yMove;
        }
    }
    else
    {
        if (xMove > 0)  // moving to the right?
        {
            wXDir = 0x800;
            wYDir = 0;
            Clip.wRight += xMove;
        }
        else            // moving to the left
        {
            Clip.wLeft += xMove;
            wXDir = wYDir = 0;
        }
    }

    DWORD *pReg = (DWORD *) (mpVidRegBase + GE_BASE);
    WaitEngineFIFO(4);  // need 4 command FIFO entries

   #ifdef DOUBLE_BUFFER
    DWORD dVal = View.wTop + mwVRes;
   #else
    DWORD dVal = View.wTop;
   #endif
    if (wYDir)
    {
        dVal += View.Height() - 1;
    }

    dVal <<= 16;
    dVal |= View.wLeft;

    if (wXDir)
    {
        dVal += View.Width() - 1;
    }        
    *(pReg + SRC_XY) = dVal;        // x,y source position

    dVal = View.Height();
    dVal <<= 16;
    dVal |= View.Width();
    *(pReg + WIDTH_HEIGHT) = dVal;  // width and height

   #ifdef DOUBLE_BUFFER
    dVal = Clip.wTop + mwVRes;
   #else
    dVal = Clip.wTop;
   #endif
    dVal <<= 16;
    dVal |= Clip.wLeft;
    *(pReg + CLIP_LeftT) = dVal;    // clip left-top

   #ifdef DOUBLE_BUFFER
    dVal = Clip.wBottom + mwVRes;
   #else
    dVal = Clip.wBottom;
   #endif

    dVal <<= 16;
    dVal |= Clip.wRight;
    *(pReg + CLIP_RightB) = dVal;   // clip right-bottom

   #ifdef DOUBLE_BUFFER
    dVal = View.wTop + yMove + mwVRes;
   #else
    dVal = View.wTop + yMove;
   #endif

    if (wYDir)
    {
        dVal += View.Height() - 1;
    }
    dVal <<= 16;
    dVal |= View.wLeft + xMove;

    if (wXDir)
    {
        dVal += View.Width() - 1;
    }
    *(pReg + DEST_XY) = dVal;       // x,y destination position

    dVal = 0xCC;                    // ROP = SRCCOPY
    dVal |= wXDir|wYDir|CLIP_ENABLE|DO_BITBLT;

    *(pReg + DRAW_CMD) = dVal;      // do the BLIT!!
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::DrawTextView(PegPoint Where, const PEGCHAR *Text, PegColor &Color,
    PegFont *Font, SIGNED iCount, PegRect &Rect)
{
   #ifdef PEG_UNICODE
    PEGCHAR PEGFAR *pCurrentChar = (PEGCHAR PEGFAR *) Text;
    PegFont *pFontStart = Font;
   #else
    UCHAR PEGFAR *pCurrentChar = (UCHAR PEGFAR *) Text;
   #endif

    UCHAR PEGFAR *pGetData;
    UCHAR PEGFAR *pGetDataBase;
    WORD  wIndex;
    WORD  wBitOffset;
    SIGNED  wXpos = Where.x;
    WORD cVal = *pCurrentChar++;
    SIGNED iCharWidth;

   #ifdef DO_OUTLINE_TEXT
    if (IS_OUTLINE(Font))
    {
        DrawOutlineText(Where, Text, Color, Font, iCount, Rect);
        return;
    }
   #endif

    while(cVal && wXpos <= Rect.wRight)
    {
        if (iCount == 0)
        {
            return;
        }
        iCount--;

       #ifdef PEG_UNICODE
        Font = pFontStart;

        while(Font)
        {
            if (cVal >= Font->wFirstChar &&
                cVal <= Font->wLastChar)
            {
                break;
            }
            Font = Font->pNext;
        }
        if (!Font)                 // this font doesn't contain this glyph?
        {
            cVal = *pCurrentChar++; // just skip to next char
            continue;               
        }

        wIndex = cVal - (WORD) Font->wFirstChar;

        if (IS_VARWIDTH(Font))
        {
            wBitOffset = Font->pOffsets[wIndex];
            iCharWidth = Font->pOffsets[wIndex+1] - wBitOffset;
        }
        else
        {
            iCharWidth = (SIGNED) Font->pOffsets;
            wBitOffset = iCharWidth * wIndex;
        }

       #else

        wIndex = cVal - (WORD) Font->wFirstChar;
        wBitOffset = Font->pOffsets[wIndex];
        iCharWidth = Font->pOffsets[wIndex+1] - wBitOffset;

       #endif

        if (wXpos + iCharWidth > Rect.wRight)
        {
            iCharWidth = Rect.wRight - wXpos + 1;
        }

        WORD ByteOffset = wBitOffset / 8;
        pGetDataBase = Font->pData + ByteOffset;
        pGetDataBase += (Rect.wTop - Where.y) * Font->wBytesPerLine;

        for (SIGNED ScanRow = Rect.wTop; ScanRow <= Rect.wBottom; ScanRow++)
        {
            pGetData = pGetDataBase;
            UCHAR InMask = 0x80 >> (wBitOffset & 7);
            WORD wBitsOutput = 0;
            UCHAR cData;

           #ifdef PEG_UNICODE
            if (ScanRow - Where.y < Font->uHeight)
            {
                cData = *pGetData++;
            }
            else
            {
                cData = 0;
            }
           #else
            cData = *pGetData++;
           #endif

            COLORVAL *Put = mpScanPointers[ScanRow] + wXpos;

            while(wBitsOutput < iCharWidth)
            {
                if (!InMask)
                {
                    InMask = 0x80;
                    // read a byte:

                   #ifdef PEG_UNICODE
                    if (ScanRow - Where.y < Font->uHeight)
                    {
                        cData = *pGetData++;
                    }
                    else
                    {
                        cData = 0;
                    }
                   #else
                    cData = *pGetData++;
                   #endif
                }

                if (wXpos >= Rect.wLeft)
                {
                    if (cData & InMask)        // is this bit a 1?
                    {
                        *Put = Color.uForeground;
                    }
                    else
                    {
                        if (Color.uFlags & CF_FILL)
                        {
                            *Put = Color.uBackground;
                        }
                    }
                }
                InMask >>= 1;
                wXpos++;
                Put++;
                wBitsOutput++;
                if (wXpos > Rect.wRight)
                {
                    break;
                }
            }
            pGetDataBase += Font->wBytesPerLine;
            wXpos -= iCharWidth;
        }
        wXpos += iCharWidth;
        cVal = *pCurrentChar++;
    }
}

/*--------------------------------------------------------------------------*/
void MQ200Screen::ShowPointer(BOOL bShow)
{
    #ifdef HARDWARE_CURSOR
    if (bShow)
    {
        *(pReg + GC1_CONTROL) = ulTemp|GC_ENABLE|HC_ENABLE;
    }
    else
    {
        *(pReg + GC1_CONTROL) = ulTemp|GC_ENABLE;
    }
    #endif
    PegScreen::ShowPointer(bShow);
}

/*--------------------------------------------------------------------------*/
void MQ200Screen::HidePointer(void)
{
   #if defined(PEG_MOUSE_SUPPORT) && !defined(HARDWARE_CURSOR)
    if (mbShowPointer)
    {
        PegThing *pt = NULL;
        PegPresentationManager *pp = pt->Presentation();
        Restore(pp, &mCapture, TRUE);
        mCapture.SetValid(FALSE);
    }
   #endif
}

/*--------------------------------------------------------------------------*/
void MQ200Screen::SetPointer(PegPoint Where)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (mbShowPointer)
    {
	   #ifdef HARDWARE_CURSOR
	    mLastPointerPos = Where;
	    Where.x -= miCurXOffset;
	    Where.y -= miCurYOffset;
	
	    DWORD *pReg = (DWORD *) (mpVidRegBase + GC_BASE);
	    DWORD dTemp = Where.y;
	    dTemp <<= 16;
	    dTemp |= Where.x;
	    *(pReg + HW_CURSOR1_POS) = dTemp;
	   
	   #else
	
	    LOCK_PEG
	    HidePointer();
	    mLastPointerPos = Where;
	
	    PegThing *pt = NULL;
	    PegPresentationManager *pp = pt->Presentation();
	    Where.x -= miCurXOffset;
	    Where.y -= miCurYOffset;
	
	    PegRect MouseRect;
	    MouseRect.wLeft = Where.x;
	    MouseRect.wTop =  Where.y;
	    MouseRect.wBottom = MouseRect.wTop + mpCurPointer->wHeight - 1;
	    MouseRect.wRight = MouseRect.wLeft + mpCurPointer->wWidth - 1;
	    Capture(&mCapture, MouseRect);
	        
	    Bitmap(pp, Where, mpCurPointer, TRUE);
	    UNLOCK_PEG
	
	   #endif   // HARDWARE_CURSOR ?
    }
   #endif   // MOUSE_SUPPORT ?
}


/*--------------------------------------------------------------------------*/
void MQ200Screen::SetPointerType(UCHAR bType)
{
   #ifdef PEG_MOUSE_SUPPORT
    if (bType < NUM_POINTER_TYPES)
    {
       #ifndef HARDWARE_CURSOR
        LOCK_PEG
        HidePointer();
       #endif

        mpCurPointer = mpPointers[bType].Bitmap;

       #if defined(HARDWARE_CURSOR)

        // set the bitmap source address register:

        DWORD dIndex = VID_MEM_SIZE >> 10;
        dIndex -= NUM_POINTER_TYPES - bType;
        //wIndex++;

        DWORD *pReg = (DWORD *) (mpVidRegBase + GC_BASE);
        *(pReg + HW_CURSOR1_ADDR) = dIndex;

       #endif

        miCurXOffset = mpPointers[bType].xOffset;
        miCurYOffset = mpPointers[bType].yOffset;
        SetPointer(mLastPointerPos);

       #ifndef HARDWARE_CURSOR
        UNLOCK_PEG
       #endif
    }
   #endif
}


/*--------------------------------------------------------------------------*/
void MQ200Screen::ResetPalette(void)
{
    SetPalette(0, 232, DefPalette256);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR *MQ200Screen::GetPalette(DWORD *pPutSize)
{
    *pPutSize = mdNumColors;
    return NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MQ200Screen::SetPalette(SIGNED iFirst, SIGNED iCount, const UCHAR *pGet)
{
    // no palette when running in 16bpp mode

    // The default operation of the SetPalette function in this 16-bit driver
    // is to convert the standard 256 color palette to HiColor 16-bit values.
    // This allows 8-bit bitmaps to be displayed in native format by simply
    // looking up the correct 5-6-5 color value corresponding to each 8-bit
    // palette index. In other words, the 16-bit Hi-Color driver can easily
    // emulate 8-bit operation when required, while still having 65535 colors
    // available.

    COLORVAL *pPut = mcHiPalette;

    pPut += iFirst;

   #if 1
    // Use this version for 5-6-5 operation:

    while(iCount--)
    {
        COLORVAL cVal = *pGet++ >> 3;   // 5 bits RED
        cVal <<= 6;                     // make room for GREEN
        cVal |= *pGet++ >> 2;           // 6 bits GREEN
        cVal <<= 5;                     // make room for BLUE
        cVal |= *pGet++ >> 3;           // keep 5 bits BLUE
       *pPut++ = cVal;                  // save the Hi-Color equivalent
    }
   #else

    // Use this version for 5-5-5 operation

    while(iCount--)
    {
        COLORVAL cVal = *pGet++ >> 3;   // 5 bits RED
        cVal <<= 5;                     // make room for GREEN
        cVal |= *pGet++ >> 3;           // 5 bits GREEN
        cVal <<= 5;                     // make room for BLUE
        cVal |= *pGet++ >> 3;           // keep 5 bits BLUE
       *pPut++ = cVal;                  // save the Hi-Color equivalent
    }
   #endif  
}

/*--------------------------------------------------------------------------*/
// Return PLL register setting value
/*--------------------------------------------------------------------------*/
void GetMQPLL(SIGNED Freq, DWORD *ulPLLData)
{
	unsigned int i;

	*ulPLLData = 0UL;
	for ( i = 0; i < MAX_MQPLL; i++ )
    {
		if (Freq == MQPLLParam[i].Freq )
		{
			*ulPLLData = MQPLLParam[i].ulPLLData;
			break;
		}
    }
}

/*--------------------------------------------------------------------------*/
// Return correct CRT timing parameters pointer
//
// Looks for a match of x resolution, y resolution, and refresh rate.
// Make sure you pass values that are included in the list of supported
// modes above!!
/*--------------------------------------------------------------------------*/
MQ_TIMING_PARAM *GetMQTiming(SIGNED x, SIGNED y, WORD freq)
{
	SIGNED i;
	for (i=0; i < MAX_MQTIMING; i++)
	{
		if ( MQTimingParam[i].usResoX == x
			&& MQTimingParam[i].usResoY == y
			&& MQTimingParam[i].usFreq == freq )
        {
			return ( (MQ_TIMING_PARAM *) &MQTimingParam[i] );
        }
	}
	return (NULL);		//not found
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//
// This routine set up GC2 and flat panel registers. LCD is driven by
// graphics controller 2 and uses PLL3 as clock source.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#ifdef LCD_ACTIVE
void MQ200Screen::SetupLCD(FP_CONTROL_STRUC *fp)
{
	DWORD ulTemp;
    DWORD ulPLL3;

    DWORD *pReg = (DWORD *) (mpVidRegBase + GC_BASE);

	// Set up start address and stride

    *(pReg + IW2_START_ADDR) = 0;
    *(pReg + IW2_STRIDE) = fp->usHoriSize;

	// Set up LCD Window-related registers

    *(pReg + HW2_CONTROL) = (fp->usHoriSize - 1) << 16;
    *(pReg + VW2_CONTROL) = (fp->usVertSize - 1) << 16;

	// Set up LCD timing registers ...

    *(pReg + HD2_CONTROL) = fp->ulHD;
    *(pReg + VD2_CONTROL) = fp->ulVD;
    *(pReg + HS2_CONTROL) = fp->ulHS;
    *(pReg + VS2_CONTROL) = fp->ulVS;

	// Set up DSTN half frame buffer address

    pReg = (DWORD *) (mpVidRegBase + FP_BASE);

	if (fp->ulFPControl & FP_TYPE_DSTN)
	{
		// Color D-STN memory requirement - no need to *3 for mono dstn panel
     	unsigned long ulDSTNFBSize = (((fp->usHoriSize * 3 + 127) >> 7) *
					 	fp->usVertSize) << 3;
	
  		unsigned long	ulScreenSize = mwHRes * mwVRes;

  		unsigned long	ulStartAddr = (ulScreenSize + 127) >> 7;
  		unsigned long	ulEndAddr = (ulScreenSize + ulDSTNFBSize + 15) >> 4;

        *(pReg + DSTN_FB_CONTROL) = ulStartAddr | ((ulEndAddr - 1) << 16);
	}
	
	//Set up the rest of flat panel registers

    *(pReg + FP_CONTROL)     = fp->ulFPControl;
    *(pReg + FP_PIN_CONTROL) = fp->ulFPPinControl;
    *(pReg + STN_CONTROL)    = fp->ulSTNControl;
    *(pReg + PWM_CONTROL)    = fp->ulPWMControl;

	//FRC is NOT needed for TFT panel

	if ( (fp->ulFPControl & FP_TYPE_MASK) != FP_TYPE_TFT )
	{
		int		i;
		for ( i = 0; i < FRC_PATTERN_CNT; i++ )
        {
            *(pReg + FRC_PATTERN + i) = FRCControlData[0].ulFRCPattern[i];
        }
		for ( i = 0; i < FRC_WEIGHT_CNT; i++ )
        {
            *(pReg + FRC_WEIGHT + i) = FRCControlData[0].ulFRCWeight[i];
        }
	}

	// Use PLL3 to drive LCD

	GetMQPLL(fp->PLLFreq, &ulPLL3);

    pReg = (DWORD *) mpVidRegBase;

    *(pReg + PLL3_CONTROL) = ulPLL3;
    ulTemp = *(pReg + PM_MISC);
    *(pReg + PM_MISC) = ulTemp | PLL3_ENABLE;

	ulTemp = FDx_1 | (1UL << 24)|IM_ENABLE|GxRCLK_PLL3;
    ulTemp |= GC_8BPP;

    pReg = (DWORD *) (mpVidRegBase + GC_BASE);
    *(pReg + GC2_CONTROL) = ulTemp;

	// Enable LCD driven by GC2

    pReg = (DWORD *) (mpVidRegBase + FP_BASE);
    ulTemp = *(pReg + FP_CONTROL);
    ulTemp |= FPI_BY_GC2;
    *(pReg + FP_CONTROL) = ulTemp;

    pReg = (DWORD *) (mpVidRegBase + GC_BASE);

	ulTemp = *(pReg + GC2_CONTROL);
    ulTemp |= GC_ENABLE;
    *(pReg + GC2_CONTROL) = ulTemp;
}
#endif

/*--------------------------------------------------------------------------*/
// This routine sets up GC1 registers to driver a standard CRT.
// Graphics Controller 1 and uses PLL2 as clock source.
/*--------------------------------------------------------------------------*/
#ifdef CRT_ACTIVE
void MQ200Screen::SetupCRT()
{
	DWORD ulTemp;
    DWORD ulPLL2;

	MQ_TIMING_PARAM *pTiming = GetMQTiming(mwHRes, mwVRes, CRT_FREQUENCY);

    if (!pTiming)
    {
        mwHRes = mwVRes = 0;
        return;
    }

	// Set up start address and stride
    DWORD *pReg = (DWORD *) (mpVidRegBase + GC_BASE);
    
    *(pReg + IW1_START_ADDR) = 0;
    *(pReg + IW1_STRIDE)     = mwHRes * 2;

	// Set up CRT Window-related registers

    *(pReg + HW1_CONTROL) = (mwHRes - 1) << 16;
    *(pReg + VW1_CONTROL) = (mwVRes -1) << 16;

    *(pReg + HD1_CONTROL) = pTiming->ulHD;
    *(pReg + VD1_CONTROL) = pTiming->ulVD;
    *(pReg + HS1_CONTROL) = pTiming->ulHS;
    *(pReg + VS1_CONTROL) = pTiming->ulVS;
    *(pReg + GC1_CRT_CONTROL) = pTiming->ulCRTC;


	// Use PLL2 to drive CRT
	GetMQPLL(pTiming->PLLFreq, &ulPLL2);

    pReg = (DWORD *) mpVidRegBase;
    *(pReg + PLL2_CONTROL) = ulPLL2;
    ulTemp = *(pReg + PM_MISC);
    ulTemp |= PLL2_ENABLE;
    *(pReg + PM_MISC) = ulTemp;

	ulTemp = FDx_1 | (1UL << 24) | IM_ENABLE | GxRCLK_PLL2;
    ulTemp |= GC_16BPP_BP;

    pReg = (DWORD *) (mpVidRegBase + GC_BASE);
    *(pReg + GC1_CONTROL) = ulTemp;

	// Enable CRT driven by GC1
	//
	ulTemp = *(pReg + GC1_CRT_CONTROL);
    *(pReg + GC1_CRT_CONTROL) = ulTemp|CRT_BY_GC1;

	ulTemp = *(pReg + GC1_CONTROL);

    #ifdef HARDWARE_CURSOR
    *(pReg + GC1_CONTROL) = ulTemp|GC_ENABLE|HC_ENABLE;
    #else
    *(pReg + GC1_CONTROL) = ulTemp|GC_ENABLE;
    #endif

}
#endif



/*--------------------------------------------------------------------------*/
//  ConfigureController- Main register initialization routine.
//
//  Initialization sequence ...
//
//  1. Setup DCR00R -  for bus clock/MIU/GE etc...
//  2. Enter D0 state from reset (D3 state)
//  3. Setup PMU.         
//  4. Setup MIU.        
//  5. Misc registers
//  6. Program hardware cursor bitmaps (if enabled with HARDWARE_CURSOR)
//  7. Set up LCD-related registers and PLL - enable it afterwards
//  8. Set up CRT-related registers and PLL - enable it afterwards
/*--------------------------------------------------------------------------*/
void MQ200Screen::ConfigureController(void)
{
	DWORD *pReg;
	
	// Set up DC00R (PLL1) for BIU, GE and MIU clock - 83MHz
    pReg = (DWORD *) (mpVidRegBase + DC_BASE);
    *(pReg + DC_0) = MQInitParam.ulDC0;
	Delay(5);

    pReg = (DWORD *) (mpVidRegBase + PC_BASE);
    *(pReg + PCI_PM_CNTL_STATUS / 4) = ENTER_D0;
	Delay(30);

	CheckIfState(0); 		//Make sure in a stable state


	//Set up PMU misc registers 
	// - also PMCLK_2048CYCLE and FP_PMCLK_128K if SA1110

    pReg = (DWORD *) (mpVidRegBase + PM_BASE);
    *(pReg + PM_MISC) = GE_ENABLE|GE_BY_PLL1;
    *(pReg + D1_STATE) = MQInitParam.ulD1;
    *(pReg + D2_STATE) = MQInitParam.ulD2;

	//To initialize MIU bloc:
	//

    pReg = (DWORD *) (mpVidRegBase + MM_BASE);
    *(pReg + MIU_CONTROL1) = DRAM_RESET_DISABLE;
	Delay(5);

    *(pReg + MIU_CONTROL1) = 0;
	Delay(5);

	// Enable auto-refresh for 1B and above ...
//	if (ulClassRevID != MQ200_REV_1A)
    MQInitParam.ulMIU2 |= AUTO_REF_ENABLE;

	*(pReg + MIU_CONTROL2) = MQInitParam.ulMIU2;
    *(pReg + MIU_CONTROL3) = MQInitParam.ulMIU3;

	// MIU REG 5 MUST BE PROGRAMMED BEFORE MIU REG 4

	*(pReg + MIU_CONTROL5) = MQInitParam.ulMIU5;
	*(pReg + MIU_CONTROL4) = MQInitParam.ulMIU4;
	Delay(5);

    *(pReg + MIU_CONTROL1) = MIU_ENABLE|MIU_RESET_DISABLE;
	Delay(5);
	
	//Here, MIU is supposed to ready to serve ...
    
    pReg = (DWORD *) (mpVidRegBase + FP_BASE);    
    *(pReg + FP_GPO_CONTROL) =  MQInitParam.ulGPO;
	*(pReg + FP_GPIO_CONTROL) = MQInitParam.ulGPIO;

    // If we are using hardware cursor, put the cursor bitmaps into video
    // memory in the correct, i.e. strange, format required:

   #ifdef HARDWARE_CURSOR
    UCHAR *pPut = mpVidMemBase + VID_MEM_SIZE;
    pPut -= NUM_POINTER_TYPES * 1024;   // allow 1K video ram per cursor

    //UCHAR *pPut = mpVidMemBase + (mwHRes * mwVRes);

    for (WORD wLoop = 0; wLoop < NUM_POINTER_TYPES; wLoop++)
    {
        ProgramCursorBitmap(mpPointers[wLoop].Bitmap, pPut);
        pPut += 1024;
    }

    pReg = (DWORD *) (mpVidRegBase + GC_BASE);
    *(pReg + HW_CURSOR1_FGCLR) = 0x000000L;
    *(pReg + HW_CURSOR1_BGCLR) = 0xffffffL;

    SetPointerType(PPT_NORMAL);
   #endif

    // Set up the graphics engine stride and color depth

    pReg = (DWORD *) (mpVidRegBase + GE_BASE);
    *(pReg + DEST_STRIDE) = (mwHRes*2)|GE_16BPP;     // 16BPP, no palette

    *(pReg + BASE_ADDRESS) = 0;         // 0 offset

    // Set up LCD panel : GC1 clocked by PLL2

   #ifdef LCD_ACTIVE
	SetupLCD(&FPControlData[LCD_PANEL_TYPE - 1]);
   #endif
	
	// Set up CRT: GC2 clocked by PLL3

   #ifdef CRT_ACTIVE
	SetupCRT();
   #endif

	// Set up Palette
	ResetPalette();
}


/*--------------------------------------------------------------------------*/
// MemoryToScreen-
//
// This function is used if we are double-buffering. If we are, then
// the invalid portion of the working buffer must be transferred to the
// visible buffer at the end of a drawing operation.
//
/*--------------------------------------------------------------------------*/
#ifdef DOUBLE_BUFFER

void MQ200Screen::MemoryToScreen(void)
{
    // perform a system-to-screen or screen-screen bitblit:

    if (!miInvalidCount)
    {
        return;
    }

    PegRect Copy;
    Copy.Set(0, 0, mwHRes - 1, mwVRes - 1);
    Copy &= mInvalid;

    Copy.wLeft &= 0xfffc;   // for DWORD alignment on the left edge

    SIGNED iTop = Copy.wTop;
    SIGNED iLeft = Copy.wLeft;
    SIGNED iWidth = Copy.Width();

    if (iWidth <= 0)
    {
        return;
    }
    iWidth += 3;
    iWidth &= 0xfffc;       // for DWORD alignment on the right edge

    SIGNED iHeight = Copy.Height();
    if (iHeight <= 0)
    {
        return;
    }

    // use the bit-blit engine to do a fast rectmove:

    DWORD *pReg = (DWORD *) (mpVidRegBase + GE_BASE);
    WaitEngineFIFO(4);  // need 4 command FIFO entries

    DWORD dVal = iTop + mwVRes;

    dVal <<= 16;
    dVal |= iLeft;

    *(pReg + SRC_XY) = dVal;        // x,y source position

    dVal = iHeight;
    dVal <<= 16;
    dVal |= iWidth;
    *(pReg + WIDTH_HEIGHT) = dVal;  // width and height

    dVal = iTop;          
    dVal <<= 16;
    dVal |= iLeft;

    *(pReg + DEST_XY) = dVal;       // x,y destination position

    dVal = 0xCC|DO_BITBLT;          // ROP = SRCCOPY
    *(pReg + DRAW_CMD) = dVal;      // do the BLIT!!
}

#endif



/*--------------------------------------------------------------------------*/
// Begin Utility Functions for bit testing and other little useful items.
/*--------------------------------------------------------------------------*/


DWORD iDummy;

/*--------------------------------------------------------------------------*/
// Delay-
//
// A terrible software delay loop. This is used because this driver is
// intended to be target OS independant, and so we can't expect a timer
// interrupt based delay to be functional. If you DO have a timer-driven
// time-base available, you should change this function ASAP to use the
// accurate time base.
//
// This routine is used only during configuration of the MQ200, and does not
// affect post-power-up performance, so we just want to insure that all
// delays are long enough to work.
//
// iCount is intended to be milliseconds.
/*--------------------------------------------------------------------------*/
void MQ200Screen::Delay(SIGNED iCount)
{
    iCount *= 10;

    while(iCount-- >= 0)
    {
        for (WORD wLoop = 0; wLoop < 8000; wLoop++)
        {
            if (iDummy)
            {
                iDummy++;
            }
            else
            {
                iDummy += 2;
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
// CheckIfState-
//
// Checks for the current state of the controller equal to the expected
// state, doesn't return until they are a match. This is used during
// powerup and powerdown to wait for the controller to finish going
// through the power sequence.
/*--------------------------------------------------------------------------*/
void MQ200Screen::CheckIfState(SIGNED iState)
{
    volatile DWORD *pReg = (DWORD *) (mpVidRegBase + PC_BASE);

    while (1)
    {
        iDummy = *(pReg + PCI_PM_CNTL_STATUS / 4);
		if ((SIGNED) (iDummy & 0x03) == iState)
        {
            return;
        }
    }
}

/*--------------------------------------------------------------------------*/
// WaitEngineFifo-
//
// Wait until the requested number of command words are available in the 
// graphics engine.
/*--------------------------------------------------------------------------*/
void MQ200Screen::WaitEngineFIFO(SIGNED iNum)
{
    volatile DWORD *pReg;

    pReg = (DWORD *) (mpVidRegBase + CC_BASE);
	DWORD dStatus;
	
	while( 1 )
	{
		dStatus = *(pReg + DRAW_STATUS) & CMD_FIFO_MASK;
		if ((SIGNED) dStatus >= iNum)
        {
			return;
        }
	}
}

/*--------------------------------------------------------------------------*/
// ProgramCursorBitmap-
//
// If HARDWARE_CURSOR is turned on, the pointer bitmaps are placed in the
// last N KBytes of video memory, where N is the number of supported pointer
// types. This is fast because at run-time we can just change one register
// on the MQ200 to change the pointer bitmap, and we don't have to do any
// actual drawing of the bitmap, we just tell the MQ200 where to put it!!
// 
// The MQ200 cursor bitmaps are always 64x64, and they are stored in
// a strange 2-bpp format. The PEG pointers are much smaller than
// 64x64, so this routine does the translation, etc.. to make the PEG
// cursor bitmaps work with the MQ200.
/*--------------------------------------------------------------------------*/
#ifdef HARDWARE_CURSOR

void MQ200Screen::ProgramCursorBitmap(PegBitmap *pMap, UCHAR *pPut)
{
    UCHAR *pGet = pMap->pStart;
    DWORD *pPutVal = (DWORD *) pPut;
    WORD wMask;
    WORD XVal;
    WORD AVal;
    UCHAR uInVal;

    for (WORD wRow = 0; wRow < 64; wRow++)
    {
        XVal = AVal = 0;
        wMask = 1;

        for (WORD wCol = 0; wCol < 64; wCol++)
        {
            if (wRow < pMap->wHeight && wCol < pMap->wWidth)
            {
                uInVal = *pGet++;
                switch(uInVal)
                {
                case 0:
                    XVal |= wMask;
                    break;

                case 0xff:
                    AVal |= wMask;
                    break;

                default:
                    break;
                }
            }
            else
            {
                AVal |= wMask;
            }

            if (wMask == 0x8000)
            {
                DWORD dVal = XVal;
                dVal <<= 16;
                dVal |= AVal;
                *pPutVal++ = dVal;
                XVal = AVal = 0;
                wMask = 1;
            }
            else
            {
                wMask <<= 1;
            }
        }
    }
}

#endif



