/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pquant.hpp - PEG color quantizer.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1993-2000 Swell Software 
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

#ifdef PEG_QUANT

#ifndef _PEGQUANT_
#define _PEGQUANT_

/* Macros for converting between (r,g,b)-colors and 15-bit     */
/* colors follow.                                              */

#define RGB15(a) (WORD) (((WORD) a.Blue & 0xf8) << 7)|((WORD) (a.Green & 0xf8)<<2)|((WORD) a.Red>>3)
#define RED(x)     (UCHAR)(((x)&31)<<3)
#define GREEN(x)   (UCHAR)((((x)>>5)&255)<< 3)
#define BLUE(x)    (UCHAR)((((x)>>10)&255)<< 3)

#define HSIZE      32768         /* size of image histogram    */
#define	MAX_HIST			0xFFFF
#define	MAXCOLORS			256

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
typedef  struct {       /* structure for a cube in color space */
   WORD  lower;         /* one corner's index in histogram     */
   WORD  upper;         /* another corner's index in histogram */
   DWORD count;         /* cube's histogram count              */
   LONG  level;         /* cube's level                        */
   UCHAR  rmin,rmax;   
   UCHAR  gmin,gmax;   
   UCHAR  bmin,bmax;   
} cube_t;


typedef struct	{
	UCHAR	Blue;  
	UCHAR	Green;
	UCHAR	Red;
	UCHAR	Count;
} PalEntry;




/*----------------------------------------------------------------------------*/
// Pegquant- PEG Color quantizer.
/*----------------------------------------------------------------------------*/
class PegQuant
{
    public:
        PegQuant();
        ~PegQuant();
        void AddColor(PegPixel Pixel);
        WORD ReduceColors(SIGNED iLimit = 254);
        UCHAR *GetPalette(void)
        {
            return mPalette;
        }
        WORD PalSize(void) {return (WORD) mlPalSize;}

    protected:
        WORD MedianCut(void);
        void Shrink(cube_t * Cube);
        void InvMap(LONG ncubes);
        UCHAR GetNewIndex( LONG index );
        LONG LookupPalIndex(UCHAR Red, UCHAR Green, UCHAR Blue);
        cube_t mCubes[MAXCOLORS];  
        LONG longdim;            
        WORD *mpHistPtr;	     
        WORD *mpHist;            
        UCHAR ColMap[256][4];
        SIGNED miMaxColors;
        WORD miStartIndex;
        WORD miEndIndex;
        UCHAR mPalette[256*3];
        LONG mlPalSize;
};

#endif
#endif


