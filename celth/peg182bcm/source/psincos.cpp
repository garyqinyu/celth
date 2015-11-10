/*--------------------------------------------------------------------------*/
// psincos.cpp
//
// Author: Jim DeLisle
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
// Sin/Cos lookup table with acess functions.
//
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#if defined(PEG_HMI_GADGETS)

/*--------------------------------------------------------------------------*/
// Sin/Cos lookup table. The values in the table are actually the 
// sin of a given angle multiplied by 1024, thus making for accurate and
// easy, shifting, math...with no radian conversions necessary.
// This is 90 degrees, including boundaries, for 1 quadrant. From that 
// we can get the sin and cos of any angle.
/*--------------------------------------------------------------------------*/

WORD PegSinCosTable[] = 
    /* 00 - 09 */ { 0, 18, 36, 54, 71, 89, 107, 125, 143, 160,
    /* 10 - 19 */   178, 195, 213, 230, 248, 265, 282, 299, 316, 333,
    /* 20 - 29 */   350, 367, 384, 400, 416, 433, 449, 465, 481, 496,
    /* 30 - 39 */   512, 527, 543, 558, 573, 587, 602, 616, 630, 644,
    /* 40 - 49 */   658, 672, 685, 698, 711, 724, 736, 748, 761, 772,
    /* 50 - 59 */   784, 796, 807, 818, 828, 838, 849, 859, 868, 878,
    /* 60 - 69 */   887, 896, 904, 912, 920, 928, 935, 942, 949, 956,
    /* 70 - 79 */   962, 968, 974, 979, 984, 989, 994, 998, 1002, 1005,
    /* 80 - 89 */   1008, 1011, 1014, 1016, 1018, 1020, 1021, 1022, 1023, 1024,
    /* 90      */   1024 };


void PegLookupSinCos(SIGNED iAngle, SIGNED* iSin, SIGNED* iCos)
{
    SIGNED iIndex = iAngle;

    // Normal the index
    while(iIndex > 90)
    {
        iIndex -= 90;
    }

    while(iIndex < 0)
    {
        iIndex += 90;
    }

    // Normal the angle
    while(iAngle < 0)
    {
        iAngle += 360;
    }

    while(iAngle >= 360)
    {
        iAngle -= 360;
    }

    // Quadrant I
    if(iAngle > 0 && iAngle < 90)
    {
        if(iSin)
		{
			*iSin = PegSinCosTable[iIndex];
		}

        if(iCos)
		{
			*iCos = PegSinCosTable[90 - iIndex];
		}
    }
    else
    {
        // Quadrant II
        if(iAngle > 90 && iAngle < 180)
        {
            if(iSin)
			{
				*iSin = PegSinCosTable[90 - iIndex];
			}

            if(iCos)
			{
				*iCos = -PegSinCosTable[iIndex];
			}
        }
        else
        {
            // Quadrant III
            if(iAngle > 180 && iAngle < 270)
            {
                if(iSin)
				{
					*iSin = -PegSinCosTable[iIndex];
				}

                if(iCos)
				{
					*iCos = -PegSinCosTable[90 - iIndex];
				}
            }
            // Quadrant IV
            else
            {
                if(iAngle > 270 && iAngle < 360)
                {
                    if(iSin)
                    {
                        *iSin = -PegSinCosTable[90 - iIndex];
                    }

                    if(iCos)
                    {
                        *iCos = PegSinCosTable[iIndex];
                    }
                }
                else
                {
                    // Special case: Quadrant boundary
                    if(iAngle == 0)
                    {
                        if(iSin)
                        {
                            *iSin = PegSinCosTable[0];
                        }

                        if(iCos)
                        {
                            *iCos = PegSinCosTable[90];
                        }
                    }
                    else if(iAngle == 90)
                    {
                        if(iSin)
                        {
                            *iSin = PegSinCosTable[90];
                        }

                        if(iCos)
                        {
                            *iCos = PegSinCosTable[0];
                        }
                    }
                    else if(iAngle == 180)
                    {
                        if(iSin)
                        {
                            *iSin = PegSinCosTable[0];
                        }

                        if(iCos)
                        {
                            *iCos = -PegSinCosTable[90];
                        }
                    }
                    else if(iAngle == 270)
                    {
                        if(iSin)
                        {
                            *iSin = -PegSinCosTable[90];
                        }

                        if(iCos)
                        {
                            *iCos = PegSinCosTable[0];
                        }
                    }
                }
            }
        }
    }
}

#endif	// PEG_HMI_GADGETS


