/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pquant.cpp - PEG color quantization and reduction class. 
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
//  This class works with PegImageConvert derived classes to create an
//  optimal color palette.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#ifdef PEG_QUANT

#if defined(PEGDOS) || defined(PEGSTANDALONE)
#include "mem.h"
#include "stdlib.h"
#endif

#if defined(POSIXPEG) || defined(PEGX11)
#include <string.h>
#include <stdlib.h>
#endif

#include <float.h>

LONG QuantLongDim = 0;
int ColCompare(const void * a1, const void * a2);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
PegQuant::PegQuant()
{
    memset(this, 0, sizeof(PegQuant));

    mpHist = new WORD[HSIZE];
	memset((char *) mpHist, 0, sizeof(WORD) * HSIZE);

    mpHistPtr = new WORD[HSIZE];
	memset((char *) mpHistPtr, 0, sizeof(WORD) * HSIZE);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
PegQuant::~PegQuant()
{
    delete [] mpHist;
    delete [] mpHistPtr;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void PegQuant::AddColor(PegPixel Pixel)
{
    WORD wColor = RGB15(Pixel);		

    if (mpHist[wColor] != 0xffff)
    {
    	mpHist[wColor]++;
    }
}



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
int ColCompare(const void * a1, const void * a2)
{
   /*
   ** Called by the sort routine in "MedianCut". Compares two
   ** colors based on the external variable "QuantLongDim".
   */

   WORD        color1,color2;
   SIGNED      c1,c2;

   color1 = (WORD)*(WORD *)a1;
   color2 = (WORD)*(WORD *)a2;
   switch (QuantLongDim)
   {
      case 0:  
         c1 = RED(color1),  c2 = RED(color2);
         break;
      case 1:
         c1 = GREEN(color1), c2 = GREEN(color2);
         break;
      case 2:
         c1 = BLUE(color2), c2 = BLUE(color2);
         break;
	}
	return (c1-c2);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
WORD PegQuant::MedianCut()
{
   UCHAR        lr,lg,lb;
   WORD        i,median,color;
   DWORD       count;
   LONG        k,level,ncubes,splitpos;
   void        *base;
   size_t      num,width;
   cube_t      Cube,CubeA,CubeB;

   /*
   ** Create the initial cube, which is the whole RGB-cube.
   */

   ncubes = 0;
   Cube.count = 0;

   for (i=0,color=0;i<=HSIZE-1;i++)
   {
      if (mpHist[i] != 0)
      {
        *(mpHistPtr + color) = i;
        color++;
		Cube.count = Cube.count + mpHist[i];
      }
   }

   Cube.lower = 0;
   Cube.upper = color-1;
   Cube.level = 0;
   Shrink(&Cube);
   mCubes[ncubes] = Cube;
   ncubes++;

   /*
   ** The main loop follows.  Search the list of cubes for the
   ** next cube to split, which is the lowest level cube.  A
   ** special case is when a cube has only one color, so that it
   ** cannot be split.
   */

   while (ncubes < miMaxColors)
   {
      level = 255; splitpos = -1;
      for (k=0;k<=ncubes-1;k++)
      {
         if (mCubes[k].lower == mCubes[k].upper)  
                  ;                            /* single color */
         else
         {
            if (mCubes[k].level < level)
            {
                level = mCubes[k].level;
                splitpos = k;
            }
         }
      }
      if (splitpos == -1)            /* no more cubes to split */
         break;

      /*
      ** Must split the cube "splitpos" in the list of cubes.
      ** Next find the longest dimension of this cube, and update
      ** the external variable "QuantLongDim", which is used by the 
      ** sort routine so that it knows along which axis to sort.
      */

      Cube = mCubes[splitpos];
      lr = Cube.rmax - Cube.rmin;
      lg = Cube.gmax - Cube.gmin;
      lb = Cube.bmax - Cube.bmin;
      if (lr >= lg && lr >= lb) QuantLongDim = 0;
      if (lg >= lr && lg >= lb) QuantLongDim = 1;
      if (lb >= lr && lb >= lg) QuantLongDim = 2;

      /*
      ** Sort along "QuantLongDim". This prepares for the next step,
      ** namely, finding the median. Use standard lib's "qsort".
      */

      base = (void *)(mpHistPtr + Cube.lower);
      num  = (size_t)(Cube.upper - Cube.lower + 1);
      width = (size_t)sizeof(WORD);
      qsort(base,num,width,ColCompare);

      /*
      ** Find median by scanning through the cube, and computing
      ** a running sum. When the running sum equals half the
      ** total for the cube, the median has been found.
      */
      
      count = 0;
      for (i=Cube.lower;i<=Cube.upper-1;i++)
      {
         if (count >= Cube.count/2) break;
         color = *(mpHistPtr + i);
         count = count + mpHist[color];
      }
      median = i;

      /*
      ** Now split "Cube" at the median. Then add the two new
      ** cubes to the list of cubes.
      */

      CubeA = Cube;
	  CubeA.upper = median-1;
      CubeA.count = count;
      CubeA.level = Cube.level + 1;
      Shrink(&CubeA);
      mCubes[splitpos] = CubeA;               /* add in old slot */

      CubeB = Cube;
	  CubeB.lower = median; 
      CubeB.count = Cube.count - count;
      CubeB.level = Cube.level + 1;
      Shrink(&CubeB);
      mCubes[ncubes] = CubeB;               /* add in new slot */
	  ncubes++;
   }

   /*
   ** We have enough cubes, or we have split all we can. Now
   ** compute the color map, the inverse color map, and return
   ** the number of colors in the color map.
   */

   InvMap(ncubes);
   return((WORD)ncubes + 17);
}


/*----------------------------------------------------------------------------*/
void PegQuant::Shrink(cube_t * Cube)
{
   /*
   ** Encloses "Cube" with a tight-fitting cube by updating the
   ** (rmin,gmin,bmin) and (rmax,gmax,bmax) members of "Cube".
   */

   UCHAR        r,g,b;
   WORD        i,color;

   Cube->rmin = 255; Cube->rmax = 0;
   Cube->gmin = 255; Cube->gmax = 0;
   Cube->bmin = 255; Cube->bmax = 0;
   for (i=Cube->lower;i<=Cube->upper;i++)
   {
      color = *(mpHistPtr + i);
      r = RED(color);
      if (r > Cube->rmax) Cube->rmax = r;
      if (r < Cube->rmin) Cube->rmin = r;
      g = GREEN(color);
      if (g > Cube->gmax) Cube->gmax = g;
      if (g < Cube->gmin) Cube->gmin = g;
      b = BLUE(color);
      if (b > Cube->bmax) Cube->bmax = b;
      if (b < Cube->bmin) Cube->bmin = b;
   }
}


/*----------------------------------------------------------------------------*/
void PegQuant::InvMap(LONG ncubes)
{
	/*
	** For each cube in the list of cubes, computes the centroid
	** (average value) of the colors enclosed by that cube, and
	** then loads the centroids in the color map. Next loads the
	** histogram with indices into the color map.
	*/
	
	UCHAR        r,g,b;
	LONG        i,k,color;
	float       rsum,gsum,bsum;
	cube_t      Cube;
	UCHAR       *pPut, *pGet;

    UCHAR SysPalette[16*3] = {
    0, 0, 0,        // black
    191, 0, 0,      // red
    0, 191, 0,      // green
    191, 191, 0,    // brown
    0, 0, 191,      // blue
    191, 0, 191,    // magenta
    0, 191, 191,    // cyan
    192, 192, 192,  // lightgray
    128, 128, 128,  // darkgray
    255, 0, 0,      // lightred
    0, 255, 0,      // lightgreen
    255, 255, 0,    // yellow
    0, 0, 255,      // lightblue
    255, 0, 255,    // lightmagenta
    0, 255, 255,    // lightcyan
    255, 255, 255   // white
    };
	
	for (k=0;k<=ncubes-1;k++)
	{
	    Cube = mCubes[k];
	    rsum = gsum = bsum = (float)0.0;
	
	    for (i=Cube.lower;i<=Cube.upper;i++)
	    {
	      color = *(mpHistPtr + i);
	      r = RED(color);  
	      rsum += (float)r*(float)mpHist[color];
	      g = GREEN(color);
	      gsum += (float)g*(float)mpHist[color];
	      b = BLUE(color);
	      bsum += (float)b*(float)mpHist[color];
        }
	
        /* Update the color map */  
	
        ColMap[k][0] = (UCHAR)(rsum/(float)Cube.count);
	    ColMap[k][1] = (UCHAR)(gsum/(float)Cube.count);
	    ColMap[k][2] = (UCHAR)(bsum/(float)Cube.count);
	    ColMap[k][3] = (UCHAR)1;
	}

    pPut = mPalette;
    pGet = SysPalette;

    for (k = 0; k < 16 * 3; k++)
    {
        *pPut++ = *pGet++;
    }
    for (k = 0; k < ncubes; k++)
    {
        *pPut++ = ColMap[k][0];
        *pPut++ = ColMap[k][1];
        *pPut++ = ColMap[k][2];
    }
}


/*----------------------------------------------------------------------------*/
WORD PegQuant::ReduceColors(SIGNED iLimit)
{

	miStartIndex = 16;
	miEndIndex = iLimit;
	miMaxColors = miEndIndex - miStartIndex;
    mlPalSize = MedianCut();
    return (WORD) mlPalSize;
}

#endif  // PEG_QUANT if







