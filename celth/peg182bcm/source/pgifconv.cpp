/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pgifconv.cpp - .gif file converter, outputs PegBitmap.
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

#include "peg.hpp"

#ifdef PEG_GIF_CONVERT

#if defined(PEGDOS) || defined(PEGSTANDALONE)
#include "string.h"
#endif

#if defined(POSIXPEG) || defined(PEGX11)
#include <string.h>
#endif

extern PEGCHAR lsBadMagic[];
extern PEGCHAR lsNotAGif[];
extern PEGCHAR lsBadGifVer[];
extern PEGCHAR lsScrnDescFail[];
extern PEGCHAR lsGifRdErr[];
extern PEGCHAR lsBadColormap[];
extern PEGCHAR lsOutOfMem[];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegGifConvert::PegGifConvert(WORD wId) : PegImageConvert(wId)
{
    mbZeroDataBlock = FALSE;
    mGifHead.IsGif89  = FALSE;
    muImageType = PIC_TYPE_GIF;

    mbFresh = FALSE;
    pBuf = new UCHAR[280];
    mpImageInfo = NULL;
    iInfoCount = -1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegGifConvert::~PegGifConvert()
{
    delete [] pBuf;

/*
    if (mpImageInfo)
    {
        delete [] mpImageInfo;
        mpImageInfo = NULL;
    }
*/
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef PIC_FILE_MODE
BOOL PegGifConvert::ReadImage(FILE *fd, SIGNED iCount)
#else
BOOL PegGifConvert::ReadImage(SIGNED iCount)
#endif
{

	UCHAR	buf[16];
	UCHAR	c;

	SIGNED		useGlobalColormap;
	SIGNED		bitPixel;

    #ifdef PIC_FILE_MODE
     mFile = fd;
    #endif


	if (! ReadOK(buf,6))
    {
		pm_error(lsBadMagic);
        return FALSE;
    }

    if (buf[0] != 'G' || buf[1] != 'I' || buf[2] != 'F')
    { 
		pm_error(lsNotAGif);
        return FALSE;
    }

	if (buf[3] == '8' && buf[4] == '7' && buf[5] == 'a')
    {
        mGifHead.IsGif89 = 0;    
    }
    else
    {
        if (buf[3] == '8' && buf[4] == '9' &&
            buf[5] == 'a')
        {
            mGifHead.IsGif89 = 1;
        }
        else
        {
    		pm_error(lsBadGifVer );
            return FALSE;
        }
    }

	if (! ReadOK(buf,7))
    {
		pm_error(lsScrnDescFail);
        return FALSE;
    }

	mGifHead.wWidth          = GIFWORD(buf[0],buf[1]);
	mGifHead.wHeight         = GIFWORD(buf[2],buf[3]);
	mGifHead.wColors         = 2<<(buf[4]&0x07);
	mGifHead.uBackClrIndex   = buf[5];
	mGifHead.uAspectRatio    = buf[6];
    mGifHead.cBackground = 0;

    SendState(PIC_HEADER_KNOWN);
    muInternalBitsPerPix = 8;

	if (BitSet(buf[4], LOCALCOLORMAP))
    {	/* Global Colormap */
		if (ReadColorMap(mGifHead.wColors, mpLocalPalette))
        {
			pm_error(lsGifRdErr);
            return FALSE;
        }
        mwLocalPalSize = 256;
        WORD wIndex = mGifHead.uBackClrIndex * 3;
        PegPixel uBack;
        uBack.Red = mpLocalPalette[wIndex];
        uBack.Green = mpLocalPalette[wIndex + 1];
        uBack.Blue = mpLocalPalette[wIndex + 2];

        if (mwSysPalSize <= 256 && mpSystemPalette)
        {
            mGifHead.cBackground = LookupBestColor(uBack);
        }
        else
        {
            mGifHead.cBackground = 0;
        }
	}

	while(iCount)
    {
		if (! ReadOK(&c,1))
        {
			pm_error(lsGifRdErr);
            return FALSE;
        }

        switch(c)
        {
        case ';':   // terminator
            SendState(PIC_COMPLETE);
			return TRUE;

        case '!':   // extension

			if (! ReadOK(&c,1))
            {
				pm_error(lsGifRdErr);
                return FALSE;
            }

			DoExtension(c);
			break;

        case ',':
            {
	    	if (! ReadOK(buf,9))
            {
		    	pm_error(lsGifRdErr);
                return FALSE;
            }

            GIF_IMAGE_INFO *pInfo = CurrentImageInfo();
            pInfo->xOffset = GIFWORD(buf[0], buf[1]);
            pInfo->yOffset = GIFWORD(buf[2], buf[3]);
            pInfo->wWidth = GIFWORD(buf[4], buf[5]);
            pInfo->wHeight = GIFWORD(buf[6], buf[7]);
    		useGlobalColormap = ! BitSet(buf[8], LOCALCOLORMAP);

	    	bitPixel = 1<<((buf[8]&0x07)+1);

		    if (!useGlobalColormap)
            {
			    if (ReadColorMap(bitPixel, mpLocalPalette))
                {
				    pm_error(lsGifRdErr);
                    return FALSE;
                }

                mwLocalPalSize = 256;

			    if (!ReadImage(pInfo->wWidth, pInfo->wHeight,
                    BitSet(buf[8], INTERLACE)))
                {
                    if (mwImageCount)
                    {
                        return TRUE;
                    }
                    return FALSE;
                }

    		}
            else
            {
			    if (!ReadImage(pInfo->wWidth, pInfo->wHeight,
				    BitSet(buf[8], INTERLACE)))
                {
                    if (mwImageCount)
                    {
                        return TRUE;
                    }
                    return FALSE;
                }
            }
    		mwImageCount++;
            iCount--;
            SendState(PIC_ONE_CONV_DONE);
            }
            break;

        default:
			break;
		}
	}

    if (InlineMode())
    {
        muInternalBitsPerPix = muOutBitsPerPix;
    }
    return TRUE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegGifConvert::ReadColorMap(SIGNED number, UCHAR *pPalette)
{
	SIGNED		i;
	UCHAR	rgb[3];

	for (i = 0; i < number; ++i)
    {
		if (! ReadOK(&rgb[0], 3))
        {
			pm_error(lsBadColormap);
            return TRUE;
        }
        *pPalette++ = rgb[0] ;
		*pPalette++ = rgb[1] ;
		*pPalette++ = rgb[2] ;
	}
    SendState(PIC_PALETTE_KNOWN);
	return FALSE;
}


/*--------------------------------------------------------------------------*/
GIF_IMAGE_INFO *PegGifConvert::CurrentImageInfo(void)
{
    if (iInfoCount < (SIGNED) mwImageCount)
    {
        // need to make another info structure:
        GIF_IMAGE_INFO *pInfo = mpImageInfo;
        mpImageInfo = new GIF_IMAGE_INFO[mwImageCount + 1];

        if (mwImageCount)
        {
            memcpy(mpImageInfo, pInfo, mwImageCount * sizeof(GIF_IMAGE_INFO));
        }
        if (pInfo)
        {
            delete [] pInfo;
        }
        pInfo = mpImageInfo + mwImageCount;
        memset(pInfo, 0, sizeof(GIF_IMAGE_INFO));
        iInfoCount++;
    }
    return mpImageInfo + mwImageCount;
}


/*--------------------------------------------------------------------------*/
void PegGifConvert::DestroyImages(void)
{
    PegImageConvert::DestroyImages();

    if (mpImageInfo)
    {
        delete [] mpImageInfo;
        mpImageInfo = NULL;
    }
    iInfoCount = -1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegGifConvert::DoExtension(SIGNED label)
{
    GIF_IMAGE_INFO *pInfo;

	switch (label) {
	case 0x01:		/* Plain Text Extension */
		break;

	case 0xff:		/* Application Extension */
		break;

	case 0xfe:		/* Comment Extension */
		while (GetDataBlock((UCHAR *) pBuf) > 0) {}
		return FALSE;

	case 0xf9:		/* Graphic Control Extension */
        (void) GetDataBlock((UCHAR *) pBuf);
        pInfo = CurrentImageInfo();

		pInfo->uDisposal    = (pBuf[0] >> 2) & 0x7;
		pInfo->uInputFlag   = (pBuf[0] >> 1) & 0x1;
		pInfo->tDelay       = GIFWORD(pBuf[1],pBuf[2]);
        mGifHead.IsGif89    = 1;

		if ((pBuf[0] & 0x1) != 0)
        {
			pInfo->uHasTrans = 1;
            pInfo->uTransColor = pBuf[3];
            SetTransColor(pBuf[3]);
        }

		while (GetDataBlock((UCHAR *) pBuf) > 0) {}
		return FALSE;

	default:
		break;
	}

	while (GetDataBlock((UCHAR *) pBuf) > 0) {}
	return FALSE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegGifConvert::GetDataBlock(UCHAR *buf)
{
	UCHAR count;

	if (!ReadOK(&count,1))
    {
		pm_error(lsGifRdErr);
		return -1;
	}
	mbZeroDataBlock = count == 0;

	if ((count != 0) && (! ReadOK(buf, count))) {
		pm_error(lsGifRdErr);
		return -1;
	}
	return count;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegGifConvert::GetCode(SIGNED code_size, SIGNED flag)
{
	SIGNED	  i, j, ret;
	WORD count;

	if (flag) {
		mCurbit = 0;
		mLastbit = 0;
		mDone = 0;
        mLastByte = 0;

        for (i = 0; i < 280; i++)
        {
            *(pBuf + i) = 0;
        }
		return 0;
	}

	if ((mCurbit + code_size) >= mLastbit)
    {
		if (mDone)
        {
			if (mCurbit >= mLastbit)
            {
				pm_error(lsGifRdErr);
            }
   			return -1;
		}
		*pBuf = *(pBuf + mLastByte - 2);
		*(pBuf + 1) = *(pBuf + mLastByte - 1);

		if ((count = GetDataBlock(pBuf + 2)) == 0)
        {
			mDone = TRUE;
        }

		mLastByte = 2 + count;
		mCurbit = (mCurbit - mLastbit) + 16;
		mLastbit = (2+count) * 8;
	}

	ret = 0;
	for (i = mCurbit, j = 0; j < code_size; ++i, ++j)
    {
		ret |= ((pBuf[ i / 8 ] & (1 << (i % 8))) != 0) << j;
    }

	mCurbit += code_size;
    return ret;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegGifConvert::LWZReadByte(SIGNED flag, SIGNED input_code_size)
{
	LONG		code, incode, i;

	if (flag)
    {
		mSetCodeSize = input_code_size;
		mCodeSize = mSetCodeSize+1;
		mClearCode = 1 << mSetCodeSize ;
		mEndCode = mClearCode + 1;
		mMaxCodeSize = 2*mClearCode;
		mMaxCode = mClearCode+2;

		GetCode(0, TRUE);
		mbFresh = TRUE;

		for (i = 0; i < mClearCode; ++i)
        {
			mTable[0][i] = 0;
			mTable[1][i] = (SIGNED) i;
		}
		for (; i < (1<<MAX_LWZ_BITS); ++i)
        {
			mTable[0][i] = mTable[1][0] = 0;
        }

		mpStack = mStack;
		return 0;
	}
    else
    {
        if (mbFresh)
        {
		    mbFresh = FALSE;
    		do
            {
	    		mFirstCode = mOldCode = GetCode(mCodeSize, FALSE);
		    } while (mFirstCode == mClearCode);

    		return mFirstCode;
	    }
    }

	if (mpStack > mStack)
    {
        mpStack--;
        return *mpStack;
    }

	while ((code = GetCode(mCodeSize, FALSE)) >= 0)
    {
		if (code == mClearCode)
        {
			for (i = 0; i < mClearCode; ++i)
            {
				mTable[0][i] = 0;
				mTable[1][i] = (SIGNED) i;
			}
			for (; i < (1<<MAX_LWZ_BITS); ++i)
				mTable[0][i] = mTable[1][i] = 0;
			mCodeSize = mSetCodeSize+1;
			mMaxCodeSize = 2 * mClearCode;
			mMaxCode = mClearCode + 2;
			mpStack = mStack;
			mFirstCode = mOldCode = GetCode(mCodeSize, FALSE);
			return mFirstCode;
		}
        else
        {
            if (code == mEndCode)
            {
			    SIGNED		count;
    			UCHAR	buf[280];

	    		if (mbZeroDataBlock)
                {
		    		return -2;
                }

    			while ((count = GetDataBlock(buf)) > 0) {}
    			if (count != 0)
                {
	    			pm_message(lsGifRdErr);
                }
    			return -3;
            }
		}

		incode = code;

		if (code >= mMaxCode)
        {
			*mpStack++ = mFirstCode;
			code = mOldCode;
		}

		while (code >= mClearCode)
        {
			*mpStack++ = mTable[1][code];
			if (code == mTable[0][code])
            {
				pm_error(lsGifRdErr);
                return -2;
            }
			code = mTable[0][code];
		}

		*mpStack++ = mFirstCode = mTable[1][code];

		if ((code = mMaxCode) <(1<<MAX_LWZ_BITS))
        {
			mTable[0][code] = mOldCode;
			mTable[1][code] = mFirstCode;
			++mMaxCode;
			if ((mMaxCode >= mMaxCodeSize) &&
				(mMaxCodeSize < (1<<MAX_LWZ_BITS)))
            {
				mMaxCodeSize *= 2;
				++mCodeSize;
			}
		}

		mOldCode = (SIGNED) incode;

		if (mpStack > mStack)
        {
            mpStack--;
			return *mpStack;
        }
	}
	return (SIGNED) code;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegGifConvert::ReadImage(SIGNED len, SIGNED height, SIGNED interlace)
{
	UCHAR	c;	
	SIGNED		v;
	SIGNED		xpos = 0, ypos = 0, pass = 0;
    WORD wOutPitch;

	if(len < 0 || height < 0)
	{
		return FALSE;
	}

    if (InlineMode())
    {
        SetupInlineBuf(len);
        wOutPitch = GetByteWidthForOutputColorDepth(len);
    }
    else
    {
        wOutPitch = len;
    }

	/*
	**  Initialize the Compression routines
	*/
	if (! ReadOK(&c,1))
    {
		pm_error(lsGifRdErr);
        return FALSE;
    }

	if (LWZReadByte(TRUE, c) < 0)
    {
		pm_error(lsGifRdErr);
        return FALSE;
    }

    if (mwMode & PIC_INLINE_DITHER)
    {
        if (!InitDither(len))
        {
            return FALSE;
        }
    }

    muInternalBitsPerPix = 8;
    mwRowsConverted = 0;
    PegBitmap *pBitmap = AddNewBitmapToList((LONG) wOutPitch * (LONG) height);

    if (!pBitmap)
    {
        pm_error(lsOutOfMem);
        return FALSE;
    }
    pBitmap->wWidth = len;
    pBitmap->wHeight = height;

    UCHAR PEGFAR *pPut;

    if (InlineMode())
    {
        pPut = mpInlineReadBuf;
    }
    else
    {
        pPut = pBitmap->pStart;
    }

	while ((v = LWZReadByte(FALSE,c)) >= 0 )
    {
        *pPut++ = (UCHAR) v;      // save .gif palette index

		++xpos;
		if (xpos == len)
        {
            if (InlineMode())
            {
                pPut = pBitmap->pStart;
                pPut += wOutPitch * ypos;

                if (mwMode & PIC_INLINE_DITHER)
                {
                    DitherBitmapRow(mpInlineReadBuf, pPut, len);
                }
                else
                {
                    RemapBitmapRow(mpInlineReadBuf, pPut, len);
                }
                mwRowsConverted++;
                pPut = mpInlineReadBuf;

            }

			xpos = 0;
			if (interlace)
            {
				switch (pass)
                {
				case 0:
				case 1:
					ypos += 8; break;
				case 2:
					ypos += 4; break;
				case 3:
					ypos += 2; break;
				}

				if (ypos >= height)
                {
					++pass;
					switch (pass)
                    {
					case 1:
						ypos = 4; break;
					case 2:
						ypos = 2; break;
					case 3:
						ypos = 1; break;
					default:
						goto fini;
					}
				}
			}
            else
            {
				++ypos;
			}
		
    		if (ypos >= height)
            {
		    	break;
            }
            if (!InlineMode())
            {
                pPut = pBitmap->pStart + ((LONG) ypos * (LONG) len);
            }
        }
	}

fini:

    if (InlineMode())
    {
        if (mwMode & PIC_INLINE_DITHER)
        {
            EndDither();
        }
        EndRemapping(pBitmap);
    }

    muInternalBitsPerPix = 8;

    GIF_IMAGE_INFO *pInfo = CurrentImageInfo();

    if (v != -3)
    {
        return TRUE;
    }
    return FALSE;
}

#endif // PEG_IMAGE_CONVERT if

