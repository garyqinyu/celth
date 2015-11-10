/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pegpci.hpp - PCI functions
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-2002 Swell Software 
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

#ifndef _PEGPCI_HPP_
#define _PEGPCI_HPP_

/*--------------------------------------------------------------------------*/
// PCI_CONFIG structure - when you absolutely positively have to know 
// everything about a PCI device...
/*--------------------------------------------------------------------------*/
typedef struct {
    WORD Vendor;
    WORD Device;
    WORD Command;
    WORD Status;
    UCHAR Rev;
    UCHAR ProgIf;
    UCHAR SubClass;
    UCHAR BaseClass;
    UCHAR CacheLineSize;
    UCHAR Latency;
    UCHAR HeaderType;
    UCHAR BIST;
    DWORD BaseAddr1;
    DWORD BaseAddr2;
    DWORD BaseAddr3;
    DWORD BaseAddr4;
    DWORD BaseAddr5;
    DWORD BaseAddr6;
} PCI_CONFIG;

/*--------------------------------------------------------------------------*/
// Prototypes and externs
/*--------------------------------------------------------------------------*/
extern UCHAR PEGFAR* PegPCIGetVideoAddress(DWORD dwClass, WORD wVendor, 
                                           WORD wDevice);
extern BOOL PegPCIReadControllerConfig(DWORD dwClass, WORD wVendor, 
                                       WORD wDevice, PCI_CONFIG* pConfig);

/*--------------------------------------------------------------------------*/
// Vendor and chip IDs
//
// The screen driver defines one of the following that describes the video
// hardware. Note that all drivers relating to a particular piece of
// hardware will all define the same identifier (ie the 8bpp and 24bpp
// drivers for the CT69000 both define PEGPCI_CT69000).
//
// Also note that we map everything to the generic defines of
// PEGPCI_VENDORID and PEGPCI_DEVICEID so that all of the screen drivers
// look (more or less) the same in the GetVideoAddress function.
//
// The Epson eval cards have a non-standard way of using ID's, so 
// don't be alarmed by the goofy looking VGACLASS.
/*--------------------------------------------------------------------------*/
#if defined(PEGPCI_CL54XX)

//#define PEGPCI_VENDORID     0x1300
//#define PEGPCI_DEVICEID     0x0000

#define PEGPCI_VENDORID     0x1013
//#define PEGPCI_DEVICEID     0x00a0     // CLGD5430
#define PEGPCI_DEVICEID     0x00a4     // CLGD5434-4
//#define PEGPCI_DEVICEID     0x00a8     // CLGD5434-8
//#define PEGPCI_DEVICEID     0x00ac     // CLGD5436
//#define PEGPCI_DEVICEID     0x00b8     // CLGD5446

#elif defined(PEGPCI_CT65545)

#define PEGPCI_VENDORID     0x102c
#define PEGPCI_DEVICEID     0x00d8

#elif defined(PEGPCI_CT65550)

#define PEGPCI_VENDORID     0x102c
#define PEGPCI_DEVICEID     0x00e0

#elif defined(PEGPCI_CT69000)

#define PEGPCI_VENDORID     0x102c
#define PEGPCI_DEVICEID     0x00c0

#elif defined(PEGPCI_MQ200)

#define PEGPCI_VENDORID     0x4d51
#define PEGPCI_DEVICEID     0x0200
#define PEGPCI_VGACLASS     0x038000

#elif defined(PEGPCI_PERMEDIA)

#define PEGPCI_VENDORID     0x104c
#define PEGPCI_DEVICEID     0x3d07

#elif defined(PEGPCI_SED1356)

#define PEGPCI_VENDORID     0xf4
#define PEGPCI_DEVICEID     0x00
#define PEGPCI_VGACLASS     0xff0000

#elif defined(PEGPCI_SED1376)

#define PEGPCI_VENDORID     0xeb
#define PEGPCI_DEVICEID     0x24
#define PEGPCI_VGACLASS     0xff0000

#elif defined(PEGPCI_SED1386)

//#define PEGPCI_VENDORID     0xf4
#define PEGPCI_VENDORID     0x10f4
#define PEGPCI_DEVICEID     0x00
#define PEGPCI_VGACLASS     0xff0000

#elif (defined(PEGPCI_S1D13A03) || defined(PEGPCI_S1D13A04))

//#define PEGPCI_VENDORID     0xf4
#define PEGPCI_VENDORID     0x14eb
#define PEGPCI_DEVICEID     0x00
#define PEGPCI_VGACLASS     0xff0000

#endif

/*--------------------------------------------------------------------------*/
// This is the generic VGA class definition. Pharlap reads the entire 24bit
// Class Code register and expects our VGA class to be the entire 24 bits.
// You may have to adjust this for your RTOS. The generic version is only
// the Class Code high 8 bits, disregarding the 8 bits of Sub-Class Code
// and 8 bits of ProgI/F.
//
// You may end up with a 24 bit value that looks like this: 0x038000 which
// would be a VGA controller with a sub-class of "Other display controller".
//
// Some controllers, such as the Epson series, simply put 0xff0000 in thier
// class since the eval cards are made to be used on the PCI bus, but the
// chip set itself for the target hardware would not have this support.
/*--------------------------------------------------------------------------*/
#if !defined(PEGPCI_VGACLASS)
#if defined(PHARLAP)
#define PEGPCI_VGACLASS     0x030000
#else
#define PEGPCI_VGACLASS     0x03
#endif
#endif

#endif  // _PEGPCI_HPP_

/*--------------------------------------------------------------------------*/
/*
$Workfile: pegpci.hpp $
$Date: 11/12/:2 2:26p $
$Revision: 4 $
*/
/*--------------------------------------------------------------------------*/

// End of file



