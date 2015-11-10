/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// winpeg.h - Definition file for running PEG on Win32. 
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

#if !defined(__winpeg_h)              // Sentry, use file only if it's not already included.
#define __winpeg_h

/*  Project winpeg
    Swell Software, Inc.
    Copyright © 1995. All Rights Reserved.

    SUBSYSTEM:    winpeg.exe Application
    FILE:         winpeg.h
    AUTHOR:       Kenneth G. Maxwell


    OVERVIEW
    ========
    Class definition for winpegApp (TApplication).      
*/

#include "winpeg.rh"            // Definition of all resources.

typedef struct
{
	SIGNED iScan;
	SIGNED iKey;
} KEYXLATE;

// scan codes for extended keys:

#define SCAN_DEL 	0x0053
#define SCAN_INS 	0x0052
#define SCAN_HOME	0x0047
#define SCAN_END	0x004f
#define SCAN_PGUP	0x0049
#define SCAN_PGDN	0x0051
#define SCAN_LNUP	0x0048
#define SCAN_LNDN	0x0050
#define SCAN_LEFT	0x004b
#define SCAN_RIGHT	0x004d

#endif                                      // __winpeg_h sentry.


