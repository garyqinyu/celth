// $Id: pbmpconv.cpp,v 1.3 2002/11/27 20:18:30 jim Exp $

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pbmpconv.cpp - .bmp file converter, outputs PegBitmap.
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
/*----------------------------------------------------------------------------*/
#include "peg.hpp"

#ifdef PEG_BMP_CONVERT

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegBmpConvert::PegBmpConvert(WORD wId) : PegImageConvert(wId)
{
    muImageType = PIC_TYPE_BMP;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegBmpConvert::~PegBmpConvert()
{
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef PIC_FILE_MODE
BOOL PegBmpConvert::ReadImage(FILE *fd, SIGNED)
#else
BOOL PegBmpConvert::ReadImage(SIGNED)
#endif
{
   #ifdef PIC_FILE_MODE
    mFile = fd;
   #endif

    mdBytesRead = 0;
    ReadBitmapHeader();
    SendState(PIC_HEADER_KNOWN);
    ReadBitmapPalette();
    SendState(PIC_PALETTE_KNOWN);
    if (ReadBitmapData())
    {
        mwImageCount++;
        SendState(PIC_ONE_CONV_DONE|PIC_COMPLETE);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#ifndef PIC_FILE_MODE
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void PegBmpConvert::SkipToBitmapOffset(DWORD dOffset)
{
    LONG lSkipCount = dOffset - mdBytesRead;
    UCHAR uTempBuf[100];

    if (mdBytesRead >= dOffset)
    {
        return;
    }

    // if we haven't read far enough into the input stream, skip some
    // bytes to get to where we need to be:

    
    if (lSkipCount > 5000)  // should never be a real big number 
    {
        return;
    }

    while (lSkipCount > 0)
    {
        if (lSkipCount <= 100)
        {
            ReadBytes(uTempBuf, (WORD) lSkipCount);
            mdBytesRead = dOffset;
            return;
        }
        ReadBytes(uTempBuf, 100);
        lSkipCount -= 100;
    }
}

#endif


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void PegBmpConvert::ReadBitmapHeader()
{
	ReadBytes((UCHAR *)&mBmpHeader.id, 2);
	ReadBytes((UCHAR *)&mBmpHeader.file_size, 4);
	ReadBytes((UCHAR *)&mBmpHeader.reserved[0], 2);
	ReadBytes((UCHAR *)&mBmpHeader.reserved[1], 2);
	ReadBytes((UCHAR *)&mBmpHeader.image_offset, 4);
	ReadBytes((UCHAR *)&mBmpHeader.header_size, 2);
	ReadBytes((UCHAR *)&mBmpHeader.not_used, 2);
	ReadBytes((UCHAR *)&mBmpHeader.xres, 4);
	ReadBytes((UCHAR *)&mBmpHeader.yres, 4);
	ReadBytes((UCHAR *)&mBmpHeader.numplanes, 2);
	ReadBytes((UCHAR *)&mBmpHeader.bits_per_pix, 2);
	ReadBytes((UCHAR *)&mBmpHeader.compression, 4);
	ReadBytes((UCHAR *)&mBmpHeader.bit_map_size, 4);
	ReadBytes((UCHAR *)&mBmpHeader.hor_res, 4);
	ReadBytes((UCHAR *)&mBmpHeader.vert_res, 4);
	ReadBytes((UCHAR *)&mBmpHeader.number_of_colors, 4);
	ReadBytes((UCHAR *)&mBmpHeader.num_sig_colors, 4);

    mdBytesRead = 54;
    muInternalBitsPerPix = (UCHAR) mBmpHeader.bits_per_pix;
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void PegBmpConvert::ReadBitmapPalette(void)
{
    if (!mBmpHeader.number_of_colors &&
        mBmpHeader.bits_per_pix != 24)
	{
	    switch(mBmpHeader.bits_per_pix)
	    {
	    case 8:
	        mBmpHeader.number_of_colors = 256;
	        break;

	    case 4:
            mBmpHeader.number_of_colors = 16;
	        break;
	
	    case 2:
	        mBmpHeader.number_of_colors = 4;
	        break;
	
	    case 1:
	        mBmpHeader.number_of_colors = 2;
	        break;
	    }
    }

    mwLocalPalSize = (WORD) mBmpHeader.number_of_colors;

	if (mwLocalPalSize)
	{
	    BmpPalEntry *pPal = new BmpPalEntry[mwLocalPalSize];

       #ifdef PIC_FILE_MODE

		fseek(mFile, mBmpHeader.header_size + 14, SEEK_SET);

       #else
        
        SkipToBitmapOffset(mBmpHeader.header_size + 14);

       #endif

        ReadBytes((UCHAR *) pPal, mwLocalPalSize * 4);
        mdBytesRead += mwLocalPalSize * 4;

        // copy the 4 byte entries into my local 8-8-8 palette

        UCHAR *pPut = mpLocalPalette;
        BmpPalEntry *pGet = pPal;

        for (WORD wLoop = 0; wLoop < mwLocalPalSize; wLoop++)
        {
            *pPut++ = pGet->Red;
            *pPut++ = pGet->Green;
            *pPut++ = pGet->Blue;
            pGet++;
        }
        delete [] pPal;
	}
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
BOOL PegBmpConvert::ReadBitmapData(void)
{
    LONG lSize;
    WORD wOutputRowSize;
    WORD wInputRowSize;
    UCHAR cWaste[8];

    wOutputRowSize = GetByteWidthForOutputColorDepth((WORD) mBmpHeader.xres);
    wInputRowSize = GetByteWidthForInternalColorDepth((WORD) mBmpHeader.xres);
    mwRowsConverted = 0;

    if (mwMode & PIC_INLINE_DITHER)
    {
        if (!InitDither((WORD) mBmpHeader.xres))
        {
            return FALSE;
        }
    }

    if (InlineMode())
    {
        SetupInlineBuf((WORD) mBmpHeader.xres);
        lSize = wOutputRowSize * mBmpHeader.yres;
    }
    else
    {
        lSize = wInputRowSize * mBmpHeader.yres;
    }

    PegBitmap *pBitmap = AddNewBitmapToList(lSize);

    if (!pBitmap)
    {
        return FALSE;
    }

    pBitmap->wWidth = (WORD) mBmpHeader.xres;
    pBitmap->wHeight = (WORD) mBmpHeader.yres;

    UCHAR PEGFAR *pPut = pBitmap->pStart;
    pPut += lSize;

    if (InlineMode())
    {
        pPut -= wOutputRowSize;
    }
    else
    {
        pPut -= wInputRowSize;
    }

	DWORD dOffset = mBmpHeader.image_offset;

   #ifdef PIC_FILE_MODE

	fseek(mFile, dOffset, SEEK_SET);

   #else

    SkipToBitmapOffset(dOffset);

   #endif

    UCHAR bPadding = GetBitmapPaddingBytes();

    for (SIGNED iLoop = (WORD) mBmpHeader.yres -1; iLoop >= 0; iLoop--)
    {
        if (InlineMode())
        {
            ReadBitmapScanLine(mpInlineReadBuf, wInputRowSize);
        }
        else
        {
            ReadBitmapScanLine(pPut, wInputRowSize);
        }

        if (mwMode & PIC_INLINE_DITHER)
        {
            DitherBitmapRow(mpInlineReadBuf, pPut, pBitmap->wWidth);
            mwRowsConverted++;
        }
        else
        {
            if (mwMode & PIC_INLINE_REMAP)
            {
                RemapBitmapRow(mpInlineReadBuf, pPut, pBitmap->wWidth);
                mwRowsConverted++;
            }
        }

        if (bPadding)
        {
            ReadBytes(cWaste, bPadding);
        }

        if (InlineMode())
        {
            pPut -= wOutputRowSize;
        }
        else
        {
            pPut -= wInputRowSize;
        }
    }

    if (InlineMode())
    {
        if (mwMode & PIC_INLINE_DITHER)
        {
            EndDither();
        }
        EndRemapping(pBitmap);
    }

    return TRUE;
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void PegBmpConvert::ReadBitmapScanLine(UCHAR *pPut, WORD wActual)
{
    UCHAR *pRed, *pBlue;
    UCHAR uTemp;

    ReadBytes(pPut, wActual);

    if (muInternalBitsPerPix == 24)
    {
        // need to swap the R & B values:

        pRed = pPut;
        pBlue = pRed + 2;

        while(wActual >= 3)
        {
            uTemp = *pRed;
            *pRed = *pBlue;
            *pBlue = uTemp;

            wActual -= 3;
            pRed += 3;
            pBlue += 3;
        }
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
UCHAR PegBmpConvert::GetBitmapPaddingBytes()
{
    WORD wBytesPerLine = GetByteWidthForInternalColorDepth((WORD) mBmpHeader.xres);
    WORD wActualBytes = wBytesPerLine;
	wActualBytes += 3;
	wActualBytes &= ~3;
	return (wActualBytes - wBytesPerLine);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
PegPixel PegBmpConvert::GetULCColor(void)
{
    return PegImageConvert::GetULCColor();

/*
    PegBitmap *pMap = mpConversionData[0].pBitmap;
    UCHAR *pGet = pMap->pStart;
    Pixel.Red = *pGet++;
    Pixel.Green = *pGet++;
    Pixel.Blue = *pGet;
    return Pixel;
*/
}


#ifdef PEG_QUANT
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void PegBmpConvert::CountColors(PegQuant *pQuant)
{
    PegImageConvert::CountColors(pQuant);
/*
    PegBitmap *pMap = mpConversionData[0].pBitmap; 
	UCHAR *pGet = pMap->pStart;
	WORD wLoop, wLoop1;

    for (wLoop1 = 0; wLoop1 < pMap->wHeight; wLoop1++)
    {
        for (wLoop = 0; wLoop < pMap->wWidth; wLoop++)
    	{   
	        Pixel.Red = *(pGet + 2);
	        Pixel.Green = *(pGet + 1);
	        Pixel.Blue = *pGet;
	
	        if (mwMode & PIC_RGB_TRANS)
	        {
	            if (Pixel != mTransColor)
	            {
	                pQuant->AddColor(Pixel);
	            }
	            else
	            {
	                pQuant->AddColor(Pixel);
	            }
			}
	        else
	        {
	            pQuant->AddColor(Pixel);
	        }
	    	pGet += 3;
	    }
    }
*/
}

#endif  // PEG_QUANT if



#endif // PEG_IMAGE_CONVERT if








