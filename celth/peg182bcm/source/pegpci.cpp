/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pegpci.cpp - PCI functions
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
//
// $Author: $
// $Date: $
// $Revision: $
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#if defined(USE_PCI)

#include "pegpci.hpp"

#if defined(PHARLAP)

#if defined(PEGTNT)
#include "pharlap.h"
#endif

#include "embpci.h"         // include PHARLAP PCI functions

#elif defined(PEGSMX)

#include "pcibios.h"

#endif

/*--------------------------------------------------------------------------*/
// PegPCIGetVideoAddress
//
// Called by screen drivers that use the PCI bus in the ::GetVideoAddress
// method.
/*--------------------------------------------------------------------------*/
UCHAR PEGFAR * PegPCIGetVideoAddress(DWORD dwClassID, WORD wVendorID, 
                                     WORD wDeviceID)
{
    UCHAR* pVidMem = NULL;
    PCI_CONFIG PciConfig;

   #if defined(PHARLAP)
    EK_PCIBIOSINFO BiosInfo;
    EK_PCIDEVICE DevClass;

    int Status = EtsPCIInit(&BiosInfo);
    if(!Status)
    {
        return(NULL);
    }

    int iIndex = 0;

    while(1)
    {
        if (EtsPCIFindDeviceByClass(dwClassID, iIndex, &DevClass))
        {
            EtsPCIReadCfgWord(PCI_CFGREG_VENDORID, &DevClass);
            PciConfig.Vendor = (WORD) DevClass.result;
            EtsPCIReadCfgWord(PCI_CFGREG_DEVICEID, &DevClass);
            PciConfig.Device = (UCHAR) DevClass.result;

            EtsPCIReadCfgDWord(PCI_CFGREG_BASEADDRESS1, &DevClass);
            PciConfig.BaseAddr1 = DevClass.result;
        }
        else
        {
            break;
        }

        if (PciConfig.Vendor == wVendorID)
        {
            pVidMem = (UCHAR *) (PciConfig.BaseAddr1 & 0xFFFF0000L);
            break;
        }

        iIndex++;
    }

   #elif defined(PEGSMX)
    byte Bus;
    byte DevFunc;

    if (FindPCIBios())
    {
        /* Make things simple; assume they have only 1 of these controllers. */
        if (FindPCIDevice(wDeviceID, wVendorID, 0, &Bus, &DevFunc))
        {
            // 0x10 seems to be the offset that specifies the base address
            PciConfig.BaseAddr1 = ReadConfigurationDWord(Bus, DevFunc, 0x10);
            pVidMem = (UCHAR *) (PciConfig.BaseAddr1 & 0xFFFF0000L);
        }
    }

   #endif

    return(pVidMem);
}


/*--------------------------------------------------------------------------*/
// PegPCIReadControllerConfig
//
// Called by some screen drivers that need extended information
// regarding the controller (over and above just the base video memory
// address).
//
// Note: Not all operating systems have the functionality to read all
//       of the configuration data concerning a particular PCI device.
/*--------------------------------------------------------------------------*/
BOOL PegPCIReadControllerConfig(DWORD dwClassID, WORD wVendorID, 
                                WORD wDeviceID, PCI_CONFIG* pConfigInfo)
{
    BOOL bRetVal = FALSE;
    PCI_CONFIG PciConfig;

   #if defined(PHARLAP)
    EK_PCIBIOSINFO BiosInfo;
    EK_PCIDEVICE DevClass;

    int Status = EtsPCIInit(&BiosInfo);

    if(!Status)
    {
        return(bRetVal);
    }

    int iIndex = 0;

    while(1)
    {
        if (EtsPCIFindDeviceByClass(dwClassID, iIndex, &DevClass))
        {
            EtsPCIReadCfgWord(PCI_CFGREG_VENDORID, &DevClass);
            PciConfig.Vendor = (WORD) DevClass.result;

            if(PciConfig.Vendor == wVendorID)
            {
                bRetVal = TRUE;

                EtsPCIReadCfgWord(PCI_CFGREG_DEVICEID, &DevClass);
                PciConfig.Device = (UCHAR) DevClass.result;
                EtsPCIReadCfgWord(PCI_CFGREG_COMMAND, &DevClass);
                PciConfig.Command = (UCHAR) DevClass.result; 
                EtsPCIReadCfgWord(PCI_CFGREG_STATUS, &DevClass);
                PciConfig.Status = (UCHAR) DevClass.result;
                EtsPCIReadCfgByte(PCI_CFGREG_REVISIONID, &DevClass);
                PciConfig.Rev = (UCHAR) DevClass.result; 
                EtsPCIReadCfgByte(PCI_CFGREG_PROGIF, &DevClass);
                PciConfig.ProgIf = (UCHAR) DevClass.result;
                EtsPCIReadCfgByte(PCI_CFGREG_SUBCLASS, &DevClass);
                PciConfig.SubClass = (UCHAR) DevClass.result;
                EtsPCIReadCfgByte(PCI_CFGREG_BASECLASS, &DevClass);
                PciConfig.BaseClass = (UCHAR) DevClass.result;
                EtsPCIReadCfgByte(PCI_CFGREG_CACHELINESIZE, &DevClass);
                PciConfig.CacheLineSize = (UCHAR) DevClass.result;
                EtsPCIReadCfgByte(PCI_CFGREG_LATENCYTIMER, &DevClass);
                PciConfig.Latency = (UCHAR) DevClass.result;
                EtsPCIReadCfgByte(PCI_CFGREG_HEADERTYPE, &DevClass);
                PciConfig.HeaderType = (UCHAR) DevClass.result;
                EtsPCIReadCfgByte(PCI_CFGREG_BIST, &DevClass);
                PciConfig.BIST = (UCHAR) DevClass.result;
                EtsPCIReadCfgDWord(PCI_CFGREG_BASEADDRESS1, &DevClass);
                PciConfig.BaseAddr1 = DevClass.result;
                EtsPCIReadCfgDWord(PCI_CFGREG_BASEADDRESS2, &DevClass);
                PciConfig.BaseAddr2 = DevClass.result;
                EtsPCIReadCfgDWord(PCI_CFGREG_BASEADDRESS3, &DevClass);
                PciConfig.BaseAddr3 = DevClass.result;
                EtsPCIReadCfgDWord(PCI_CFGREG_BASEADDRESS4, &DevClass);
                PciConfig.BaseAddr4 = DevClass.result;
                EtsPCIReadCfgDWord(PCI_CFGREG_BASEADDRESS5, &DevClass);
                PciConfig.BaseAddr5 = DevClass.result;
                EtsPCIReadCfgDWord(PCI_CFGREG_BASEADDRESS6, &DevClass);
                PciConfig.BaseAddr6 = DevClass.result;

                break;
            }
        }
        else
        {
            break;
        }

        iIndex++;
    }

   #elif defined(PEGSMX)
    byte Bus;
    byte DevFunc;

    if (FindPCIBios())
    {
        /* Make things simple; assume they have only 1 of these controllers. */
        if (FindPCIDevice(wDeviceID, wVendorID, 0, &Bus, &DevFunc))
        {
            bRetVal = TRUE;

            PciConfig.BaseAddr1 = ReadConfigurationDWord(Bus, DevFunc, 0x10);
            PciConfig.BaseAddr2 = ReadConfigurationDWord(Bus, DevFunc, 0x14);
        }
    }

   #endif

    return(bRetVal);
}

#endif  // USE_PCI

