/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// psincos.hpp
//
// Author: Jim DeLisle
//
// Copyright (c) 1998-2001 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// Sin/Cos lookup table with acess functions.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PSINCOS_
#define _PSINCOS_

#if defined(PEG_HMI_GADGETS) || defined(PEG_FULL_GRAPHICS)

void PegLookupSinCos(SIGNED iAngle, SIGNED* pSin, SIGNED* pCos);

#endif  // PEG_HMI_GADGETS || PEG_FULL_GRAPHICS

#endif  // _PSINCOS_

