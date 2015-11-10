/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pimgconv.cpp - Image conversion base class, used by GIF, BMP, and
//                JPG conversion classes.
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
// The base PegImageConvert class performs the following:
//
// 1) Gets data, either from a file or from an input buffer depending on
//    whether or not PIC_FILE_MODE is defined.
//
// 2) Performs RLE encoding.
//
// 3) Performs Dithering and best match re-mapping.
//
// 4) Keeps local palette.
//
// 5) Keeps global palette.
//
// 6) Keeps a list of PegBitmaps. These are the output of the ImageConvert
//    conversion process, regardless of the input file type or type of 
//    output remapping and/or encoding.
//  
// 7) Keeps error and state information during read and conversion.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include <string.h>     // For Unicode builds on Unix systems, this has to
                        // be included before peg.hpp

#include "peg.hpp"

#if defined(PEGDOS) || defined(PEGSTANDALONE)
#include "string.h"
#endif

#if defined(PEG_IMAGE_CONVERT)

#define GRAYSCALE_RED_WEIGHT    299     // 0.299
#define GRAYSCALE_GREEN_WEIGHT  587     // 0.587
#define GRAYSCALE_BLUE_WEIGHT   144     // 0.144


/*--------------------------------------------------------------------------*/
PegImageConvert::PegImageConvert(WORD wId)
{
    mwRowsConverted = 0;
    mwSysPalSize = 256;
    mpSystemPalette = NULL;
    mpLocalPalette = NULL;
    mwImageCount = 0;
    mpConversionData = NULL;
    mpInlineReadBuf = NULL;
    mwState = 0;
    mwId = wId;
    mwMode = 0;
    mbUseFastLookup = FALSE;
    mbGrayscale = FALSE;

    muOutBitsPerPix = 8;               // default to generating 8 bits/pix
    muInternalBitsPerPix = 8;          // default to keeping 8 bits/pix internally
    muAlphaVal = 0xff;

    mpLocalPalette = new UCHAR[256*3];  // worst case palette size

   #ifndef  PIC_FILE_MODE

    mpCallBack = NULL;
    mpInBuf = new UCHAR[PIC_INPUT_BUF_SIZE];
    mpPutInData = mpInBuf;
    mpGetInData = mpInBuf;
    mpInBufStop = mpInBuf + PIC_INPUT_BUF_SIZE;

   #endif
}


/*--------------------------------------------------------------------------*/
PegImageConvert::~PegImageConvert()
{
    delete mpLocalPalette;

    if (mpInlineReadBuf)
    {
        delete mpInlineReadBuf;
    }
   
   #ifndef PIC_FILE_MODE
    delete mpInBuf;
   #endif
}


#ifndef PIC_FILE_MODE

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::SetIdleCallback(BOOL (*pFunc)(WORD, WORD, 
    PegImageConvert *))
{
    mpCallBack = pFunc;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
WORD PegImageConvert::SendData(UCHAR *Get, WORD wSize)
{
    WORD wAvailable = InputFreeSpace();

    if (wSize > wAvailable)
    {
        wSize = wAvailable;
    }
    else
    {
        wAvailable = wSize;     // for return value only
    }

    while(wSize--)
    {
        *mpPutInData++ = *Get++;
        if (mpPutInData == mpInBufStop)
        {
            mpPutInData = mpInBuf;
        }
    }
    return(wAvailable);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
WORD PegImageConvert::InputFreeSpace(void)
{
    WORD wReturn;

    if (mpPutInData < mpGetInData)
    {
        return (mpGetInData - mpPutInData - 1);
    }
    else
    {
        wReturn = mpInBufStop - mpGetInData - 1;
        wReturn += mpGetInData - mpInBuf;
    }
    return wReturn;

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
WORD PegImageConvert::ConsumeImageInput(UCHAR *Buffer, WORD wLimit)
{
    UCHAR PEGFAR *pPut = Buffer;
    WORD wBytesRead = 0;

    while(wLimit > 0)
    {
        while(mpPutInData == mpGetInData)
        {
            mwState |= PIC_IDLE;

            if (mpCallBack)
            {
                if (!mpCallBack(mwId, mwState, this))
                {
                    mwState &= ~PIC_IDLE;
                    return wBytesRead;
                }
            }
            else
            {
                PIC_SLEEP
            }

            mwState &= ~PIC_IDLE;
        }
        *pPut++ = *mpGetInData++;
        wBytesRead++;
        wLimit--;
            
        if (mpGetInData == mpInBufStop)
        {
            mpGetInData = mpInBuf;
        }
    }
    return wBytesRead;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::SendState(WORD wState)
{
    mwState |= wState;
    if (mpCallBack)
    {
        mpCallBack(mwId, mwState, this);
    }
}

#else

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
WORD PegImageConvert::ConsumeImageInput(UCHAR *Buffer, WORD wLimit)
{
    WORD wBytes = (WORD) fread(Buffer, 1, wLimit, mFile);
    return wBytes;
}

void PegImageConvert::SendState(WORD wState)
{
    mwState |= wState;
}

#endif   // PIC_FILE_MODE if

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
WORD PegImageConvert::GetByteWidthForOutputColorDepth(WORD wImgWidth)
{
    WORD wOutWidth = wImgWidth;

    if (TransMode() && muOutBitsPerPix <= 8)
    {
        // transparent images with less that 8bpp have to be written out as
        // 8-bpp images

        return wImgWidth;
    }
    
	switch(muOutBitsPerPix)
	{
	case 1:
	    wOutWidth += 7;
	    wOutWidth /= 8;
	    break;
	
	case 2:
	    wOutWidth += 3;
	    wOutWidth /= 4;
	    break;
	
	case 4:
	    wOutWidth += 1;
	    wOutWidth /= 2;
	    break;

	case 8:
	    break;
	
	case 16:
	    wOutWidth *= 2;
	    break;
	
	case 24:
	    wOutWidth *= 3;
	    break;

    case 32:
        wOutWidth *= 4;
        break;
	}
    return wOutWidth;
}	

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
WORD PegImageConvert::GetByteWidthForInternalColorDepth(WORD wImgWidth)
{
    WORD wOutWidth = wImgWidth;

	switch(muInternalBitsPerPix)
	{
	case 1:
	    wOutWidth += 7;
	    wOutWidth /= 8;
	    break;
	
	case 2:
	    wOutWidth += 3;
	    wOutWidth /= 4;
	    break;
	
	case 4:
	    wOutWidth += 1;
	    wOutWidth /= 2;
	    break;
	
	case 8:
	    break;
	
	case 16:
	    wOutWidth *= 2;
	    break;
	
	case 24:
	    wOutWidth *= 3;
	    break;

	case 32:
	    wOutWidth *= 4;
	    break;
	}
    return wOutWidth;
}	

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::SetSystemPalette(UCHAR *pPal, WORD wPalSize, BOOL bFast)
{
    mpSystemPalette = pPal;
    mwSysPalSize = wPalSize;
    mbUseFastLookup = bFast;

    switch(mwSysPalSize)
    {
    case 2:
        muOutBitsPerPix = 1;
        break;

    case 4:
        muOutBitsPerPix = 2;
        break;

    case 16:
        muOutBitsPerPix = 4;
        break;

    case HI_COLORS:
        muOutBitsPerPix = 16;
        break;

    case TRUE_COLORS:
        muOutBitsPerPix = 24;
        break;

    case YUVA_COLORS:
        muOutBitsPerPix = 32;
        break;

    default:
        muOutBitsPerPix = 8;
        break;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::EndRemapping(PegBitmap *pMap)
{
    if (TransMode())
    {
        pMap->uFlags |= BMF_HAS_TRANS;

        if (muOutBitsPerPix > 8)
        {
            muInternalBitsPerPix = muOutBitsPerPix;
        }
        else
        {
            muInternalBitsPerPix = 8;
        }

        if (muOutBitsPerPix < 8)
        {
            pMap->dTransColor = 0xff;
        }
        else
        {
            if (muOutBitsPerPix == 8)
            {
                if (mpSystemPalette)
                {
                    pMap->dTransColor = 0xff;
                }
                else
                {
                    pMap->dTransColor =  mTransColor.Red & 0xe0;
                    pMap->dTransColor |= (mTransColor.Green & 0xe0) >> 3;
                    pMap->dTransColor |= (mTransColor.Blue & 0xe0) >> 6;
                }
            }
            else
            {
                pMap->dTransColor = 1;
            }
        }
    }
    else
    {
        pMap->dTransColor = 0;
        muInternalBitsPerPix = muOutBitsPerPix;
    }
    if (mbGrayscale)
    {
        pMap->uFlags |= BMF_GRAYSCALE;
    }
    if (mwMode & PIC_RGB_ORDER)
    {
        pMap->uFlags |= BMF_RGB;

    }
    pMap->uBitsPix = muOutBitsPerPix;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Output: PegBitmap data pointer is modified to point to new encoded
// data, and format is set to RLE encoded. If the encoded size is larger
// than the original size, RleEncode returns without doing anything.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegImageConvert::RleEncode(BOOL bForce)
{
    BOOL bStatus = FALSE;
    WORD wRows;
    LONG lOrigSize;

    for (WORD wImage = 0; wImage < mwImageCount; wImage++)
    {
        PegBitmap *pMap = mpConversionData[wImage].pBitmap;

        LONG lSize = 0;
        UCHAR PEGFAR *pGet = pMap->pStart;
        UCHAR PEGFAR *pPut = NULL;


        wRows = pMap->wHeight;
        while(wRows--)
        {
            lSize += RleEncode(pGet, pPut, pMap->wWidth);
            pGet += GetByteWidthForInternalColorDepth(pMap->wWidth);
        }

        lOrigSize = pMap->wWidth;

        lOrigSize = GetByteWidthForOutputColorDepth((WORD) lOrigSize);

   	    lOrigSize *= pMap->wHeight;

        if (lSize < lOrigSize || bForce)
        {
            UCHAR PEGFAR *pNewData = new UCHAR[lSize];
            mpConversionData[wImage].lDataSize = lSize;
            
            pPut = pNewData;
            pGet = pMap->pStart;

            wRows = pMap->wHeight;

            while(wRows--)
            {   
                pPut += RleEncode(pGet, pPut, pMap->wWidth);
                pGet += GetByteWidthForInternalColorDepth(pMap->wWidth);
            }
            pPut = pMap->pStart;
            pMap->pStart = pNewData;        // reset data pointer
            pMap->uFlags |= BMF_RLE;         // reset PegBitmap type
            delete pPut;                    // delete old data
            bStatus =  TRUE;
        }
    }
    return bStatus;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegImageConvert::SetupInlineBuf(WORD wWidth)
{
    if (mpInlineReadBuf)
    {
        delete mpInlineReadBuf;
        mpInlineReadBuf = NULL;
    }
    LONG lSize = wWidth * 4;
    mpInlineReadBuf = new UCHAR[lSize];

    if (!mpInlineReadBuf)
    {
        return FALSE;
    }
    return TRUE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LONG PegImageConvert::GetRawPixValue(UCHAR *pData, WORD wIndex)
{
    WORD *pGetHiColor;
    LONG lVal = 0;
    UCHAR uMask;
    
    switch(muInternalBitsPerPix)
    {
    case 1:
        pData += wIndex / 8;
        uMask = 0x80 >> (wIndex % 8);
        lVal = *pData;
        lVal &= uMask;
        lVal >>= 7 - (wIndex % 8);
        break;

    case 2:
        pData += wIndex / 4;
        uMask = 0xc0 >> ((wIndex % 4) * 2);
        lVal = *pData;
        lVal &= uMask;
        lVal >>= 6 - ((wIndex % 4) * 2);
        break;

    case 4:
        pData += wIndex / 2;
        lVal = *pData;
        if (wIndex & 1)
        {
            lVal &= 0x0f;
        }
        else
        {
            lVal >>= 4;
        }
        break;

    case 8:
        pData += wIndex;
        lVal = *pData;
        break;

    case 16:
        pGetHiColor = (WORD *) pData;
        pGetHiColor += wIndex;
        lVal = *pGetHiColor;
        break;

    case 24:
        pData += wIndex * 3;
        lVal = *pData++;
        lVal <<= 8;
        lVal |= *pData++;
        lVal <<= 8;
        lVal |= *pData;
        break;
    }
    return lVal;
}


/*--------------------------------------------------------------------------*/
// RleEncode
/*--------------------------------------------------------------------------*/
WORD PegImageConvert::RleEncode(UCHAR *pData, UCHAR *pPut, WORD wWidth)
{
    // If pPut is NULL, we are just trying to find out how much room
    // the encoded version will require, so we don't actually put any
    // data.

	UCHAR bRawCount = 0;
	UCHAR PEGFAR *pPutRaw = pPut;
    UCHAR PEGFAR *pPutStart = pPut;
    WORD wPixIndex = 0;
    LONG lPixVal;

   	while(wPixIndex < wWidth)
	{
		WORD wCount = CountDuplicates(pData, wPixIndex, wWidth);

		if (wCount >= 3)
		{
			if (bRawCount)
			{
                if (pPutStart)
                {
    				*pPutRaw = (bRawCount - 1) | 0x80;
                }
				bRawCount = 0;
			}

			if (wCount > 128)
			{
				wCount = 128;
			}

            if (pPutStart)
            {
    			*pPut = wCount - 1;
            }
            pPut++;
                
            lPixVal = GetRawPixValue(pData, wPixIndex);

            switch(muOutBitsPerPix)
            {
            case 1:
            case 2:
            case 4:
            case 8:
                if (pPutStart)
                {
                    *pPut = (UCHAR) lPixVal;
                }
                pPut++;
                break;

            case 16:
                if (pPutStart)
                {
              		*pPut++ = (UCHAR) (lPixVal);
	           		*pPut++ = (UCHAR) (lPixVal >> 8);
                }
                else
                {
                    pPut += 2;
                }
                break;

            case 24:
                if (pPutStart)
                {
          		    *pPut++ = (UCHAR) (lPixVal >> 16);
	       		    *pPut++ = (UCHAR) (lPixVal >> 8);
	       		    *pPut++ = (UCHAR) (lPixVal);
                }
                else
                {
                    pPut += 3;
                }
                break;

            case 32:
                if (pPutStart)
                {
          		    *pPut++ = (UCHAR) (lPixVal >> 16);
	       		    *pPut++ = (UCHAR) (lPixVal >> 8);
	       		    *pPut++ = (UCHAR) (lPixVal);
                    *pPut++ = muAlphaVal;
                }
                else
                {
                    pPut += 4;
                }
                break;
            }
            wPixIndex += wCount;
			pPutRaw = pPut;
			bRawCount = 0;
		}
		else
		{
			if (!bRawCount)
			{
				pPutRaw = pPut;
				pPut++;
			}
			bRawCount++;

            lPixVal = GetRawPixValue(pData, wPixIndex);

            switch(muOutBitsPerPix)
            {
            case 1:
            case 2:
            case 4:
            case 8:
                if (pPutStart)
                {
                    *pPut = (UCHAR) lPixVal;
                }
                pPut++;
                break;

            case 16:
                if (pPutStart)
                {
              		*pPut++ = (UCHAR) (lPixVal);
	           		*pPut++ = (UCHAR) (lPixVal >> 8);
                }
                else
                {
                    pPut += 2;
                }
                break;

            case 24:
                if (pPutStart)
                {
  	      		    *pPut++ = (UCHAR) (lPixVal >> 16);
           		    *pPut++ = (UCHAR) (lPixVal >> 8);
           		    *pPut++ = (UCHAR) (lPixVal);
                 }
                else
                {
                    pPut += 3;
                }
                break;

            case 32:
                if (pPutStart)
                {
  	      		    *pPut++ = (UCHAR) (lPixVal >> 16);
           		    *pPut++ = (UCHAR) (lPixVal >> 8);
           		    *pPut++ = (UCHAR) (lPixVal);
                    *pPut++ = muAlphaVal;
                 }
                else
                {
                    pPut += 4;
                }
                break;
            }

            wPixIndex++;

			if (bRawCount == 128 || wPixIndex == wWidth)
			{
                if (pPutStart)
                {
    				*pPutRaw = (bRawCount - 1) | 0x80;
                }
				pPutRaw = pPut;
				bRawCount = 0;
			}
		}
	}
	return (pPut - pPutStart);
}


/*--------------------------------------------------------------------------*/
WORD PegImageConvert::CountDuplicates(UCHAR *Start, WORD wIndex, WORD wWidth)
{
	WORD wDups = 1;
    LONG lPixVal = GetRawPixValue(Start, wIndex);
    wIndex++;

	while(wIndex < wWidth)
	{
		if(GetRawPixValue(Start, wIndex) == lPixVal)
		{
			wDups++;
		}
		else
		{
			break;
		}
		wIndex++;
	}
	return(wDups);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegBitmap *PegImageConvert::AddNewBitmapToList(LONG lSize)
{
    PegBitmap *pNew;

    if (mwImageCount)
    {
        ConversionData *pList = new ConversionData [mwImageCount + 1];

        for (WORD wLoop = 0; wLoop < mwImageCount; wLoop++)
        {
            *(pList + wLoop) = *(mpConversionData + wLoop);
        }
        delete mpConversionData;
        mpConversionData = pList;
        pNew = new PegBitmap;
        mpConversionData[mwImageCount].pBitmap = pNew;
        mpConversionData[mwImageCount].lDataSize = lSize;
    }
    else
    {
        mpConversionData = new ConversionData[2];
        pNew = new PegBitmap;
        mpConversionData[0].pBitmap = pNew;
        mpConversionData[0].lDataSize = lSize;
    }

    pNew->pStart = new UCHAR[lSize];
    pNew->uBitsPix = muOutBitsPerPix;

    if (mbGrayscale)
    {
        pNew->uFlags = BMF_GRAYSCALE;
    }
    else
    {
        pNew->uFlags = 0;
    }

    if (!(pNew->pStart))
    {
        return NULL;
    }
    memset(pNew->pStart, 0, lSize);
    mlDataSize = lSize;
    return pNew;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LONG PegImageConvert::GetDataSize(int iIndex)
{
    if (iIndex < mwImageCount)
    {
        return mpConversionData[iIndex].lDataSize;
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegBitmap *PegImageConvert::GetBitmapPointer(int iIndex)
{
    if (iIndex < mwImageCount)
    {
        return mpConversionData[iIndex].pBitmap;
    }
    return NULL;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::DestroyImages(void)
{
    if (mpConversionData)
    {
        for (WORD wLoop  = 0; wLoop < mwImageCount; wLoop++)
        {
            PegBitmap *pMap = mpConversionData[wLoop].pBitmap;

            delete pMap->pStart;
            delete pMap;
        }
        delete mpConversionData;
        mpConversionData = NULL;
    }
    mwImageCount = 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::RotateImages(SIGNED iRotation)
{
    // The rotation is fairly
    // simple, and works thus:
    //
    // 1: allocate memory for a copy of bitmap data, make data copy
    // 2: switch height/width of source bitmap
    // 3: write the data back in to the source, rotated from the
    //    copy area.

    WORD wInputStride;
    WORD wOutputStride;
    UCHAR *pGet;
    UCHAR *pPut;
    WORD  *pPutHiColor;
    DWORD *pPutYUV;
    UCHAR uShift;
    UCHAR uMask;
    UCHAR uData;

    LONG lPixVal;

    if (iRotation != 90 && iRotation != 270)
    {
        return;
    }

    for (WORD wImage = 0; wImage < mwImageCount; wImage++)
    {
        PegBitmap *pMap = mpConversionData[wImage].pBitmap;
        wInputStride = GetByteWidthForInternalColorDepth(pMap->wWidth);
        wOutputStride = GetByteWidthForInternalColorDepth(pMap->wHeight);
        DWORD dSize = (DWORD) wOutputStride * (DWORD) pMap->wWidth;

        UCHAR *pCopy = new UCHAR[dSize];

        if (!pCopy)
        {
            return;
        }

        memset(pCopy, 0, dSize);

        UCHAR *pGetStart = pMap->pStart;
        pGet = pGetStart;

        for (WORD wLoop1 = 0; wLoop1 < pMap->wHeight; wLoop1++)
        {
            pGet = pGetStart;
            pGet += wLoop1 * wInputStride;

            UCHAR *pPutStart = pCopy;

            if (iRotation == 90)
            {
                pPutStart += (pMap->wWidth - 1) * wOutputStride;

                switch(muInternalBitsPerPix)
                {
                case 1:
                    pPutStart += wLoop1 / 8;
                    break;

                case 2:
                    pPutStart += wLoop1 / 4;
                    break;

                case 4:
                    pPutStart += wLoop1 / 2;
                    break;

                case 8:
                    pPutStart += wLoop1;
                    break;

                case 16:
                    pPutStart += wLoop1 * 2;
                    break;
                    
                case 24:
                    pPutStart += wLoop1 * 3;
                    break;

                case 32:
                    pPutStart += wLoop1 * 4;
                    break;
                }
            }
            else
            {
                pPutStart += wOutputStride;

                switch(muInternalBitsPerPix)
                {
                case 1:
                    pPutStart -= 1 + (wLoop1 / 8);
                    break;

                case 2:
                    pPutStart -= 1 + (wLoop1 / 4);
                    break;

                case 4:
                    pPutStart -= 1 + (wLoop1 / 2);
                    break;

                case 8:
                    pPutStart -= 1 + wLoop1;
                    break;

                case 16:
                    pPutStart -= 2 + (wLoop1 * 2);
                    break;

                case 24:
                    pPutStart -= 3 + (wLoop1 * 3);
                    break;

                case 32:
                    pPutStart -= 4 + (wLoop1 * 4);
                    break;
                }
            }
            pPut = pPutStart;

            for (WORD wLoop = 0; wLoop < pMap->wWidth; wLoop++)
            {
                lPixVal = GetRawPixValue(pGet, wLoop);

                switch(muInternalBitsPerPix)
                {
                case 1:
                    if (iRotation == 90)
                    {
                        uShift = wLoop1 % 8;
                    }
                    else
                    {
                        uShift = 7 - (wLoop1 % 8);
                    }

                    lPixVal <<= 7 - uShift;
                    uMask = 0x80 >> uShift;
                    uData = *pPut;
                    uData &= ~uMask;
                    uData |= lPixVal;
                    *pPut = uData;
                    break;

                case 2:
                    uShift = (wLoop1 % 4) * 2;

                    if (iRotation != 90)
                    {
                        uShift = 6 - uShift;
                    }

                    lPixVal <<= 6 - uShift;
                    uMask = 0xc0 >> uShift;
                    uData = *pPut;
                    uData &= ~uMask;
                    uData |= lPixVal;
                    *pPut = uData;
                    break;

                case 4:
                    uData = *pPut;
                    if (iRotation == 90)
                    {
                        if (wLoop1 & 1)
                        {
                            uData &= 0xf0;
                            uData |= lPixVal;
                        }
                        else
                        {
                            uData &= 0x0f;
                            uData |= lPixVal << 4;
                        }
                    }
                    else
                    {
                        if (wLoop1 & 1)
                        {
                            uData &= 0x0f;
                            uData |= lPixVal << 4;
                        }
                        else
                        {
                            uData &= 0xf0;
                            uData |= lPixVal;
                        }
                    }
                    *pPut = uData;        
                    break;

                case 8:
		            *pPut = (UCHAR) lPixVal;
                    break;

                case 16:
                    pPutHiColor = (WORD *) pPut;
                    *pPutHiColor = (WORD) lPixVal;
                    break;

                case 24:
                    *(pPut + 2) = (UCHAR) lPixVal;
                    *(pPut + 1) = (UCHAR) (lPixVal >> 8);
                    *pPut = (UCHAR) (lPixVal >> 16);
                    break;

                case 32:
                    pPutYUV = (DWORD *) pPut;
                    *pPutYUV = lPixVal;
                    break;
                }

                if (iRotation == 90)
                {
                    pPut -= wOutputStride;
                }
                else
                {
                    pPut += wOutputStride;
                }
            }
        }
        pPut = pMap->pStart;
        pMap->pStart = pCopy;
        wOutputStride = pMap->wHeight;
        pMap->wHeight = pMap->wWidth;
        pMap->wWidth = wOutputStride;
        delete pPut;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// This function is used for internal testing only and is not included in
// run-time target builds.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0
#ifdef PEG_NUM_COLORS
#if (PEG_NUM_COLORS == 16)
void PegImageConvert::Write6BitMapFile(void)
{
    FILE *pOut = fopen("map6bit.hpp", "w");

    if (!pOut)
    {
        return;
    }

    UCHAR Red, Green, Blue;
    PegPixel Pix;

    for (Red = 0x0; Red <= 7; Red++)
    {
        for (Green = 0x0; Green <= 7; Green++)
        {
            for (Blue = 0; Blue < 4; Blue++)
            {
                Pix.Red = (Red << 5) + 15;
                Pix.Green = (Green << 5) + 15;
                Pix.Blue = (Blue << 6) + 31;
                UCHAR uMatch = LookupBestColor(Pix);
                fprintf(pOut, "%d,", uMatch);
            }
        }
        fprintf(pOut, "\n");
    }
    fclose(pOut);
}
#endif
#endif
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegImageConvert::InitDither(WORD cols)
{
//    Write6BitMapFile();

	mpThisRerr = new LONG[cols + 2];
    if (!mpThisRerr)
    {
        return(FALSE);
    }
    mpNextRerr = new LONG[cols + 2];
    if (!mpNextRerr)
    {
        return(FALSE);
    }
	mpThisGerr = new LONG[cols + 2];
    if (!mpThisGerr)
    {
        return(FALSE);
    }
    mpNextGerr = new LONG[cols + 2];
    if (!mpNextGerr)
    {
        return(FALSE);
    }
	mpThisBerr = new LONG[cols + 2];
    if (!mpThisBerr)
    {
        return(FALSE);
    }
    mpNextBerr = new LONG[cols + 2];
    if (!mpNextBerr)
    {
        return(FALSE);
    }

    memset(mpThisRerr, 0, sizeof(LONG) * (cols + 2));
    memset(mpThisGerr, 0, sizeof(LONG) * (cols + 2));
    memset(mpThisBerr, 0, sizeof(LONG) * (cols + 2));
    mbFSDir = 1;
    return TRUE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::EndDither(void)
{
    delete mpThisRerr;
    delete mpNextRerr;
    delete mpThisGerr;
    delete mpNextGerr;
    delete mpThisBerr;
    delete mpNextBerr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegImageConvert::DitherBitmap()
{
    if (!mpSystemPalette)
    {
        return FALSE;
    }

    UCHAR *pGet;
    UCHAR *pPut;
    UCHAR *pNewStart;
    WORD wGetPitch;
    WORD wPutPitch;
    LONG lOutputSize;

    for (WORD wImage = 0; wImage < mwImageCount; wImage++)
    {
        PegBitmap *pMap = mpConversionData[wImage].pBitmap;
        InitDither(pMap->wWidth);
        pGet = pMap->pStart;
        wGetPitch = GetByteWidthForInternalColorDepth(pMap->wWidth);
        wPutPitch = GetByteWidthForOutputColorDepth(pMap->wWidth);
        lOutputSize = wPutPitch * pMap->wHeight;

        pNewStart = new UCHAR[lOutputSize];
        if (!pNewStart)
        {
            return FALSE;
        }
        pPut = pNewStart;
        
        for (WORD wLoop = 0; wLoop < pMap->wHeight; wLoop++)
        {
            DitherBitmapRow(pGet, pPut, pMap->wWidth);
            pGet += wGetPitch;
            pPut += wPutPitch;
        }
        EndDither();
        EndRemapping(pMap);
        delete pMap->pStart;
        pMap->pStart = pNewStart;
    }
    return TRUE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::DitherBitmapRow(UCHAR PEGFAR *pGet, UCHAR PEGFAR *pPut,
     WORD wWidth)
{
    if (!mpSystemPalette)
    {
        return;
    }

    LONG    lRawVal;
    PegPixel   Pixel;
    PegPixel   NewPixel;
    PegPixel   WantPixel;
    PegPixel   SourcePixel;
    SIGNED  iCol, iLimitCol;
    BOOL    bUseColor;
    WORD wPixIndex = 0;

    memset(mpNextRerr, 0, sizeof(LONG) * (wWidth + 2));
    memset(mpNextGerr, 0, sizeof(LONG) * (wWidth + 2));
    memset(mpNextBerr, 0, sizeof(LONG) * (wWidth + 2));

	if (mbFSDir)
    {
        iCol = 0;
	    iLimitCol = (SIGNED) wWidth;
	}
	else
    {
	    iCol = (SIGNED) wWidth - 1;
	    iLimitCol = -1;
        wPixIndex = wWidth - 1;
    }

    do 
    {
        bUseColor = TRUE;

        if (mwMode & PIC_INDEX_TRANS)
        {
            lRawVal = GetRawPixValue(pGet, wPixIndex);
            if ((UCHAR) lRawVal == mTransColor.Red)
            {
                WriteTransValue(pPut, wPixIndex);
                bUseColor = FALSE;
            }
        }

        if (bUseColor)
        {            
            Pixel = GetPixelColor(pGet, wPixIndex);

            if (mwMode & PIC_RGB_TRANS)
            {
                if (Pixel == mTransColor)
                {
                    WriteTransValue(pPut, wPixIndex);
                    bUseColor = FALSE;
                }
            }
        }

        if (bUseColor)
        {
            SourcePixel = Pixel;
            Pixel.Red +=  mpThisRerr[iCol + 1] / FS_SCALE ;
            Pixel.Green += mpThisGerr[iCol + 1] / FS_SCALE;
            Pixel.Blue  += mpThisBerr[iCol + 1] / FS_SCALE;
            WantPixel = Pixel;

	        if (Pixel.Red < 0)
	        {
	            Pixel.Red = 0;
	        }
	        else
	        {
	            if (Pixel.Red > 255)
	            {
	                Pixel.Red = 255;
	            }
	        }
	        if (Pixel.Green < 0)
	        {
	            Pixel.Green = 0;
	        }
	        else
	        {
	            if (Pixel.Green > 255)
	            {
	                Pixel.Green = 255;
	            }
	        }
	        if (Pixel.Blue < 0)
	        {
	            Pixel.Blue = 0;
	        }
	        else
	        {
	            if (Pixel.Blue > 255)
	            {
	                Pixel.Blue = 255;
	            }
	        }

            NewPixel = WriteBestRowPixelValue(pPut, wPixIndex, Pixel);
	
	        LONG err;
	
			if (mbFSDir)
		    {
			    err = (WantPixel.Red - NewPixel.Red) * FS_SCALE;
			    mpThisRerr[iCol + 2] += ( err * 7 ) / 16;
			    mpNextRerr[iCol    ] += ( err * 3 ) / 16;
			    mpNextRerr[iCol + 1] += ( err * 5 ) / 16;
			    mpNextRerr[iCol + 2] += ( err     ) / 16;
			    err = (WantPixel.Green - NewPixel.Green) * FS_SCALE;
			    mpThisGerr[iCol + 2] += ( err * 7 ) / 16;
			    mpNextGerr[iCol    ] += ( err * 3 ) / 16;
			    mpNextGerr[iCol + 1] += ( err * 5 ) / 16;
			    mpNextGerr[iCol + 2] += ( err     ) / 16;
			    err = (WantPixel.Blue - NewPixel.Blue) * FS_SCALE;
			    mpThisBerr[iCol + 2] += ( err * 7 ) / 16;
			    mpNextBerr[iCol    ] += ( err * 3 ) / 16;
			    mpNextBerr[iCol + 1] += ( err * 5 ) / 16;
			    mpNextBerr[iCol + 2] += ( err     ) / 16;
		    }
	    	else
		    {
			    err = (WantPixel.Red - NewPixel.Red) * FS_SCALE;
			    mpThisRerr[iCol    ] += ( err * 7 ) / 16;
			    mpNextRerr[iCol + 2] += ( err * 3 ) / 16;
			    mpNextRerr[iCol + 1] += ( err * 5 ) / 16;
			    mpNextRerr[iCol    ] += ( err     ) / 16;
			    err = (WantPixel.Green - NewPixel.Green) * FS_SCALE;
			    mpThisGerr[iCol    ] += ( err * 7 ) / 16;
			    mpNextGerr[iCol + 2] += ( err * 3 ) / 16;
			    mpNextGerr[iCol + 1] += ( err * 5 ) / 16;
			    mpNextGerr[iCol    ] += ( err     ) / 16;
			    err = (WantPixel.Blue - NewPixel.Blue) * FS_SCALE;
			    mpThisBerr[iCol    ] += ( err * 7 ) / 16;
			    mpNextBerr[iCol + 2] += ( err * 3 ) / 16;
			    mpNextBerr[iCol + 1] += ( err * 5 ) / 16;
			    mpNextBerr[iCol    ] += ( err     ) / 16;
		    }
        }

	    if (mbFSDir)
		{
    		iCol++;
            wPixIndex++;
		}
	    else
		{
    		iCol--;
            wPixIndex--;
		}
    } while ( iCol != iLimitCol);

	LONG *temperr = mpThisRerr;
	mpThisRerr = mpNextRerr;
	mpNextRerr = temperr;
	temperr = mpThisGerr;
	mpThisGerr = mpNextGerr;
	mpNextGerr = temperr;
	temperr = mpThisBerr;
	mpThisBerr = mpNextBerr;
	mpNextBerr = temperr;
	mbFSDir = ! mbFSDir;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegPixel PegImageConvert::GetPixelColor(UCHAR *pGet, WORD wYPos, WORD wIndex,
    WORD wImgWidth)
{
    WORD wOffset = GetByteWidthForInternalColorDepth(wImgWidth);
    wOffset *= wYPos;
    return GetPixelColor(pGet + wOffset, wIndex);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegPixel PegImageConvert::GetPixelColor(UCHAR *pGet, WORD wIndex)
{
    PegPixel Pixel;
    LONG lRawVal = GetRawPixValue(pGet, wIndex);

    switch(muInternalBitsPerPix)
    {
    case 1:
    case 2:
    case 4:
    case 8:
        lRawVal *= 3;
        Pixel.Red =  mpLocalPalette[lRawVal];
        Pixel.Green = mpLocalPalette[lRawVal + 1];
        Pixel.Blue  = mpLocalPalette[lRawVal + 2];
        break;

    case 16:
        if (mwMode & PIC_555_MODE)
        {
            Pixel.Blue = (lRawVal >> 7) & 0xf8;
            Pixel.Green = (lRawVal >> 2) & 0xf8;
            Pixel.Red = (lRawVal << 3) & 0xf8;
        }
        else
        {
            Pixel.Blue = (lRawVal >> 8) & 0xf8;
            Pixel.Green = (lRawVal >> 3) & 0xf8;
            Pixel.Red = (lRawVal << 3) & 0xf8;
        }
        break;

    case 24:

        // the internal order for raw data is always R:G:B

        Pixel.Red = (UCHAR) (lRawVal >> 16);
        Pixel.Green = (lRawVal >> 8) & 0xff;
        Pixel.Blue = lRawVal & 0xff;
        break;
    }
    return Pixel;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegImageConvert::RemapBitmap()
{
    if (mwMode & PIC_YUV_MODE)
    {
//        return (RemapImagesTo420YUVA());
    }

    for (WORD wImage = 0; wImage < mwImageCount; wImage++)
    {
        PegBitmap *pMap = mpConversionData[wImage].pBitmap;

        if ((muInternalBitsPerPix == muOutBitsPerPix) &&
            (muOutBitsPerPix == 1 || muOutBitsPerPix == 16 || muOutBitsPerPix == 24))
        {
            
            if (muOutBitsPerPix != 24 || (mwMode & PIC_RGB_ORDER))
            {
                if (!TransMode())
                {
                    EndRemapping(pMap);
                    if (wImage == mwImageCount - 1)
                    {
                        return TRUE;
                    }
                    continue;
                }
            }
        }

        UCHAR *pGet = pMap->pStart;
        UCHAR *pGetStart = pGet;
        UCHAR *pPut;

        WORD wGetPitch = GetByteWidthForInternalColorDepth(pMap->wWidth);
        WORD wPutPitch = GetByteWidthForOutputColorDepth(pMap->wWidth);

        LONG lSize = (LONG) wPutPitch * (LONG) pMap->wHeight;
        pMap->pStart = new UCHAR[lSize];
        pPut = pMap->pStart;

        if (!pMap->pStart)
        {
            return FALSE;
        }

        for (WORD wRow = 0; wRow < pMap->wHeight; wRow++)
        {
            RemapBitmapRow(pGet, pPut, pMap->wWidth);
            pGet += wGetPitch;
            pPut += wPutPitch;
        }
        EndRemapping(pMap);
        mpConversionData[wImage].lDataSize = lSize;
        delete(pGetStart);                  // free the old buffer
    }
    return TRUE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::WriteTransValue(UCHAR *pPut, WORD wPixIndex)
{
    UCHAR uVal;
    WORD *pWrite = (WORD *) pPut;

    switch(muOutBitsPerPix)
    {
    case 1:
    case 2:
    case 4:
        pPut += wPixIndex;
        *pPut = 0xff;
        break;

    case 8:
        pPut += wPixIndex;

        if (!mpSystemPalette)   // mapping to 3:3:2 mode?
        {
            uVal =  mTransColor.Red & 0xe0;
            uVal |= (mTransColor.Green & 0xe0) >> 3;
            uVal |= (mTransColor.Blue & 0xe0) >> 6;
            *pPut = uVal;
        }
        else
        {
            *pPut = 0xff;
        }
        break;

    case 16:
        pWrite += wPixIndex;
        *pWrite = 1;
        break;

    case 24:
        pPut += wPixIndex * 3;
        *pPut++ = 0;
        *pPut++ = 0;
        *pPut++ = 1;
        break;
    }
}

#define YUVA_BIG_ENDIAN

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegPixel PegImageConvert::WriteBestRowPixelValue(UCHAR *pLine, WORD wPixIndex,
    PegPixel Pixel)
{
    UCHAR *pPut;
    UCHAR uShift;
    UCHAR uMask;
    UCHAR uVal;
    UCHAR uData;
    WORD HiColor;
    WORD *pPutHiColor;
    PegPixel NewPixel;

    if (TransMode() && muOutBitsPerPix < 8)
    {
        // have to write 8bpp for transparent bitmaps with less than 8bpp

        pPut = pLine + wPixIndex;
        uVal = LookupBestColor(Pixel, &NewPixel);
        *pPut = uVal;
        return NewPixel;
    }

    switch(muOutBitsPerPix)
    {
    case 1:
        pPut = pLine + wPixIndex / 8;
        uShift = wPixIndex % 8;
        uVal = LookupBestColor(Pixel, &NewPixel);
        uVal <<= 7 - uShift;
        uMask = 0x80 >> uShift;
        uData = *pPut;
        uData &= ~uMask;
        uData |= uVal;
        *pPut = uData;
        break;
                
    case 2:
        pPut = pLine + wPixIndex / 4;
        uShift = (wPixIndex % 4) * 2;
        uVal = LookupBestColor(Pixel, &NewPixel);
        uVal <<= 6 - uShift;
        uMask = 0xc0 >> uShift;
        uData = *pPut;
        uData &= ~uMask;
        uData |= uVal;
        *pPut = uData;
        break;

    case 4:
        pPut = pLine + wPixIndex / 2;
        uVal = LookupBestColor(Pixel, &NewPixel);
        uData = *pPut;
        if (wPixIndex & 1)
        {
            uData &= 0xf0;
            uData |= uVal;
        }
        else
        {
            uData &= 0x0f;
            uData |= uVal << 4;
        }
        *pPut = uData;        
        break;

    case 8:
        pPut = pLine + wPixIndex;

        // If the system palette is NULL, this means that we are actually
        // compressing to a 3:3:2 packed 8-bit format, rather than remapping
        // to an actual system palette
        
        if (!mpSystemPalette)
        {
            uVal = Pixel.Red & 0xe0;
            uVal |= (Pixel.Green & 0xe0) >> 3;
            uVal |= (Pixel.Blue & 0xe0) >> 6;
            *pPut = uVal;
            NewPixel = Pixel;
        }
        else
        {
            uVal = LookupBestColor(Pixel, &NewPixel);
            *pPut = uVal;
        }
        break;

    case 16:
        pPut = pLine + (wPixIndex * 2);
        pPutHiColor = (WORD *) pPut;

        if (mwMode & PIC_555_MODE)
        {
            HiColor = ((WORD) Pixel.Red << 7) & 0x7c00;
            HiColor |= ((WORD) Pixel.Green << 2) & 0x03e0;
            HiColor |= Pixel.Blue >> 3;
        }
        else
        {
            HiColor =  (WORD) Pixel.Red;
            HiColor <<= 8;
            HiColor &= 0xf800;
            HiColor |= ((WORD) Pixel.Green << 3) & 0x07e0;
            HiColor |= Pixel.Blue >> 3;
        }
        *pPutHiColor = HiColor;
        NewPixel = Pixel;
        break;
        
    case 24:
        pPut = pLine + (wPixIndex * 3);

        if (mwMode & PIC_RGB_ORDER)
        {
            *pPut++ = (UCHAR) Pixel.Red;
            *pPut++ = (UCHAR) Pixel.Green;
            *pPut++ = (UCHAR) Pixel.Blue;
        }
        else
        {
            *pPut++ = (UCHAR) Pixel.Blue;
            *pPut++ = (UCHAR) Pixel.Green;
            *pPut++ = (UCHAR) Pixel.Red;
        }
        NewPixel = Pixel;
        break;

    case 32:
        // Here for YUVA 32 bpp format:
        pPut = pLine + (wPixIndex * 4);

        #ifdef YUVA_BIG_ENDIAN
        
        UCHAR  Y;  
        Y = (UCHAR) (0.299*Pixel.Red+0.587*Pixel.Green+0.114*Pixel.Blue);
		*pPut++ = Y;

    	if (Pixel.Blue - Y+128 > 255)
        {
            *pPut++ = 255;
        }
    	else
        {
            if(Pixel.Blue - Y+128 < 0)
            {
                *pPut++ = 0;
            }
            else
            {
                *pPut++ = Pixel.Blue - Y+128; 
            }
        }

    	if (Pixel.Red - Y+128 > 255)
        {
            *pPut++ = 255;
        }
        else
        {
            if (Pixel.Red - Y+128 < 0)
            {
                *pPut++ = 0;
            }
            else
            {
                *pPut++ = Pixel.Red - Y+128; 
            }
        }
            
  		*pPut++ = muAlphaVal;

        #else

        // write Alpha
        *pPut++ = muAlphaVal;

        // write V
        dVal = ((112 * Pixel.Red - 94 * Pixel.Green - 18 * Pixel.Blue + 128) >> 8) + 128;
        *pPut++ = (UCHAR) dVal;

        // write U
        dVal = ((-38 * Pixel.Red - 74*Pixel.Green + 112 * Pixel.Blue + 128) >> 8) + 128;
        *pPut++ = (UCHAR) dVal;

        // write Y
        dVal = ((LONG) (66*Pixel.Red + 129*Pixel.Blue + 128) >> 8) + 16;
        *pPut++ = (UCHAR) dVal;

        #endif
    }
    return NewPixel;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// RemapBitmapRow
// This base class implementation will remap any paletted color value
// from the source data to the local palette value, and write the
// local palette value back into the same image data location.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void PegImageConvert::RemapBitmapRow(UCHAR *pGet, UCHAR *pPut, WORD wWidth)
{
    LONG lRawVal;
    PegPixel Pixel;
    WORD wPixIndex = 0;
    
    // here to remap to system palette:

    for (wPixIndex = 0; wPixIndex < wWidth; wPixIndex++)
    {
        if (mwMode & PIC_INDEX_TRANS)
        {
            lRawVal = GetRawPixValue(pGet, wPixIndex);
            if ((UCHAR) lRawVal == mTransColor.Red)
            {
                WriteTransValue(pPut, wPixIndex);
                continue;
            }
        }
            
        Pixel = GetPixelColor(pGet, wPixIndex);

        if (mwMode & PIC_RGB_TRANS)
        {
            if (Pixel == mTransColor)
            {
                WriteTransValue(pPut, wPixIndex);
            }
            else
            {
                WriteBestRowPixelValue(pPut, wPixIndex, Pixel);
            }
        }
        else
        {
            WriteBestRowPixelValue(pPut, wPixIndex, Pixel);
        }
    }
}


#ifdef PEG_QUANT
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void PegImageConvert::CountColors(PegQuant *pQuant)
{
    PegBitmap *pMap;
    PegPixel Pixel;

	WORD wLoop, wPixIndex, wGetPitch;


    for (WORD wImage = 0; wImage < mwImageCount; wImage++)
    {
        pMap = mpConversionData[wImage].pBitmap;
	    UCHAR *pGet = pMap->pStart;
        wGetPitch = GetByteWidthForInternalColorDepth(pMap->wWidth);

        for (wLoop = 0; wLoop < pMap->wHeight; wLoop++)
        {
            for (wPixIndex = 0; wPixIndex < pMap->wWidth; wPixIndex++)
    	    {
                Pixel = GetPixelColor(pGet, wPixIndex);

	            if (mwMode & PIC_RGB_TRANS)
	            {
	                if (Pixel == mTransColor)
	                {
                        continue;
                    }
                }
                pQuant->AddColor(Pixel);
	        }
            pGet += wGetPitch;
        }
    }
}

#endif


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegPixel PegImageConvert::GetULCColor()
{
    if (mpConversionData)
    {
        PegBitmap *pMap = mpConversionData[0].pBitmap;
        return GetPixelColor(pMap->pStart, 0, 0, 0);
    }
    PegPixel Pix;
    return Pix;
}


// Fast 16-color translation table. Converts 3:3:2 RGB into a 
// 16 color palette index.

UCHAR MapTo16[256] = {
0,0,4,4,0,0,4,4,0,0,4,4,2,8,6,6,2,2,6,6,2,2,6,6,2,2,6,6,10,10,6,14,
0,0,4,4,0,0,4,4,0,8,4,4,2,8,8,6,2,8,6,6,2,8,6,6,2,2,6,6,10,10,6,14,
0,0,4,4,0,8,4,4,0,8,8,4,8,8,8,8,2,8,8,6,2,8,8,6,2,8,6,6,10,10,6,14,
1,8,5,5,1,8,8,5,8,8,8,8,8,8,8,8,3,8,8,8,3,8,8,7,3,8,8,7,3,8,7,7,
1,1,5,5,1,8,5,5,1,8,8,5,3,8,8,8,3,8,8,7,3,8,8,7,3,8,7,7,3,3,7,7,
1,1,5,5,1,8,5,5,1,8,8,5,3,8,8,7,3,8,8,7,3,8,7,7,3,3,7,7,3,3,7,7,
1,1,5,5,1,1,5,5,1,8,5,5,3,8,8,7,3,8,7,7,3,3,7,7,3,3,7,7,3,3,7,7,
9,9,5,13,9,9,5,13,9,9,5,13,3,8,7,7,3,3,7,7,3,3,7,7,3,3,7,7,11,11,7,15};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
UCHAR PegImageConvert::LookupBestColor(PegPixel Pixel, PegPixel *pNewPixel)
{
    int iVal;
    int iIndex;

    if (!mpSystemPalette)
    {
        return 0;
    }

    if (mbUseFastLookup)
    {
        iVal = ((Pixel.Blue + 25) / 51) * 36;
        iVal += ((Pixel.Green + 25) / 51) * 6;
        iVal += (Pixel.Red + 25) / 51;
        iVal += 16;

        if (pNewPixel)
        {
            iIndex = iVal * 3;
            pNewPixel->Red = mpSystemPalette[iIndex];
            pNewPixel->Green = mpSystemPalette[iIndex + 1];
            pNewPixel->Blue = mpSystemPalette[iIndex + 2];
        }
        return iVal;
    }

    if (mwSysPalSize == 16 && mbUseFastLookup)
    {
        if (!mbGrayscale)
        {
            // Use the fast 3:3:2 bit translation table to convert to 16 colors:
    
            iVal = MapTo16[(Pixel.Red & 0xe0)|((Pixel.Green & 0xe0) >> 3)|(Pixel.Blue >> 6)];

            if (pNewPixel)
            {
                iIndex = iVal * 3;
                pNewPixel->Red = mpSystemPalette[iIndex];
                pNewPixel->Green = mpSystemPalette[iIndex + 1];
                pNewPixel->Blue = mpSystemPalette[iIndex + 2];
            }
            return iVal;
        }
    }


	LONG i, r1, g1, b1, r2, g2, b2;
	LONG dist, newdist;
    UCHAR ind = 0;

	r1 = Pixel.Red;
	g1 = Pixel.Green;
	b1 = Pixel.Blue;

	dist = 2000000000;

	for ( i = 0; i < mwSysPalSize; ++i )
	{
	    newdist = i * 3;

	    r2 = *(mpSystemPalette + newdist);
	    g2 = *(mpSystemPalette + newdist + 1);
	    b2 = *(mpSystemPalette + newdist + 2);

        if (mbGrayscale)
        {
            #if 1
	        newdist = (( r1 - r2 ) * ( r1 - r2 ) * GRAYSCALE_RED_WEIGHT)  +
	        (( g1 - g2 ) * ( g1 - g2 ) * GRAYSCALE_GREEN_WEIGHT) +
	        (( b1 - b2 ) * ( b1 - b2 ) * GRAYSCALE_BLUE_WEIGHT);
            #else
	        newdist = ( r1 - r2 ) * ( r1 - r2 ) +
	        ( g1 - g2 ) * ( g1 - g2 ) +
	        ( b1 - b2 ) * ( b1 - b2 );
            #endif
        }
        else
        {
	        newdist = ( r1 - r2 ) * ( r1 - r2 ) +
	        ( g1 - g2 ) * ( g1 - g2 ) +
	        ( b1 - b2 ) * ( b1 - b2 );
        }
	    if ( newdist < dist )
	    {
	        ind = (UCHAR) i;
	        dist = newdist;
            if (!dist)
            {
                break;
            }
	    }
	}

    if (pNewPixel)
    {
        newdist = ind * 3;
        pNewPixel->Red = *(mpSystemPalette + newdist);
        pNewPixel->Green = *(mpSystemPalette + newdist + 1);
        pNewPixel->Blue = *(mpSystemPalette + newdist + 2);
    }

    return ind;
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::pm_error(PEGCHAR *pText)
{
    pm_message(pText);
    SendState(PIC_ERROR);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegImageConvert::pm_message(PEGCHAR *pText)
{
    strcpy(msError, pText);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PegPixel::PegPixel(void)
{
    Red = Green = Blue = 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegPixel::PegPixel(LONG r, LONG g, LONG b)
{
    Red = r;
    Green = g;
    Blue = b;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegPixel::operator != (const PegPixel &Pixel) const
{
    if (Red == Pixel.Red && Green == Pixel.Green && Blue == Pixel.Blue)
    {
        return FALSE;
    }
    return TRUE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegPixel::operator == (const PegPixel &Pixel) const
{
    if (Red == Pixel.Red && Green == Pixel.Green && Blue == Pixel.Blue)
    {
        return TRUE;
    }
    return FALSE;
}

#endif  // PEG_IMAGE_CONVERT if


