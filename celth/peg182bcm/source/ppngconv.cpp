/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ppngconv.cpp - .png file converter, outputs PegBitmap.
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
// The low-level PNG utilities are derived heavily from the work of
// Simon-Pierre Cadieux, Eric S. Raymond, Gilles Vollant, Tom Lane, Glenn
// Randers-Pehrson, Willem van Schaik, John Bowler, Kevin Bracey, Sam Bushell,
// Magnus Holmgren, Greg Roelofs, Tom Tanner, Andreas Dilger, Dave Martindale,
// Guy Eric Schalnat, Paul Schmidt, and Tim Wegner.
//
// The original form has been modified heavily to simplify integration
// and usage in the PEG environments.
//
/*
 * COPYRIGHT NOTICE, DISCLAIMER, and LICENSE:
 *
 * If you modify libpng you may insert additional notices immediately following
 * this sentence.
 *
 * libpng versions 1.0.7, July 1, 2000, through  1.0.8, July 24, 2000, are
 * Copyright (c) 2000 Glenn Randers-Pehrson, and are
 * distributed according to the same disclaimer and license as libpng-1.0.6
 * with the following individuals added to the list of Contributing Authors
 *
 *    Simon-Pierre Cadieux
 *    Eric S. Raymond
 *    Gilles Vollant
 *
 * and with the following additions to the disclaimer:
 *
 *    There is no warranty against interference with your enjoyment of the
 *    library or against infringement.  There is no warranty that our
 *    efforts or the library will fulfill any of your particular purposes
 *    or needs.  This library is provided with all faults, and the entire
 *    risk of satisfactory quality, performance, accuracy, and effort is with
 *    the user.
 *
 * libpng versions 0.97, January 1998, through 1.0.6, March 20, 2000, are
 * Copyright (c) 1998, 1999, 2000 Glenn Randers-Pehrson
 * Distributed according to the same disclaimer and license as libpng-0.96,
 * with the following individuals added to the list of Contributing Authors:
 *
 *    Tom Lane
 *    Glenn Randers-Pehrson
 *    Willem van Schaik
 *
 * libpng versions 0.89, June 1996, through 0.96, May 1997, are
 * Copyright (c) 1996, 1997 Andreas Dilger
 * Distributed according to the same disclaimer and license as libpng-0.88,
 * with the following individuals added to the list of Contributing Authors:
 *
 *    John Bowler
 *    Kevin Bracey
 *    Sam Bushell
 *    Magnus Holmgren
 *    Greg Roelofs
 *    Tom Tanner
 *
 * libpng versions 0.5, May 1995, through 0.88, January 1996, are
 * Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.
 *
 * For the purposes of this copyright and license, "Contributing Authors"
 * is defined as the following set of individuals:
 *
 *    Andreas Dilger
 *    Dave Martindale
 *    Guy Eric Schalnat
 *    Paul Schmidt
 *    Tim Wegner
 *
 * The PNG Reference Library is supplied "AS IS".  The Contributing Authors
 * and Group 42, Inc. disclaim all warranties, expressed or implied,
 * including, without limitation, the warranties of merchantability and of
 * fitness for any purpose.  The Contributing Authors and Group 42, Inc.
 * assume no liability for direct, indirect, incidental, special, exemplary,
 * or consequential damages, which may result from the use of the PNG
 * Reference Library, even if advised of the possibility of such damage.
 *
 * Permission is hereby granted to use, copy, modify, and distribute this
 * source code, or portions hereof, for any purpose, without fee, subject
 * to the following restrictions:
 *
 * 1. The origin of this source code must not be misrepresented.
 *
 * 2. Altered versions must be plainly marked as such and must not
 *    be misrepresented as being the original source.
 *
 * 3. This Copyright notice may not be removed or altered from any
 *    source or altered source distribution.
 *
 * The Contributing Authors and Group 42, Inc. specifically permit, without
 * fee, and encourage the use of this source code as a component to
 * supporting the PNG file format in commercial products.  If you use this
 * source code in a product, acknowledgment is not required but would be
 * appreciated.
 */
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#if defined(PEG_PNG_DECODER)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegPngConvert::PegPngConvert( WORD wId ) : PegImageConvert(wId),
    mpPng( NULL )
{
    muImageType = PIC_TYPE_PNG;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegPngConvert::~PegPngConvert()
{
    DestroyReadStruct();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#if defined(PIC_FILE_MODE)
BOOL PegPngConvert::ReadImage( FILE* fd, SIGNED )
#else
BOOL PegPngConvert::ReadImage( SIGNED )
#endif
{
    // previous invocation left structure intact for subsequent invocations of
    //   accessor functions
    DestroyReadStruct();

#if defined(PIC_FILE_MODE)
    mFile = fd;
#endif

    if (!IsValid())
    {
        return FALSE;
    }

    if (!InitializeStructures())
    {
        return FALSE;
    }

    if (!ReadPngHeader())
    {
        return FALSE;
    }
    SendState( PIC_HEADER_KNOWN );

    if (ReadPngData())
    {
        mwImageCount++;
        SendState( PIC_ONE_CONV_DONE | PIC_COMPLETE );
        return TRUE;
    }

    return FALSE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegPngConvert::IsValid()
{
    UCHAR buf[PEG_PNG_SIGNATURE_BYTES];

    if (ReadBytes( buf, PEG_PNG_SIGNATURE_BYTES ) != PEG_PNG_SIGNATURE_BYTES)
    {
        return FALSE;
    }

    static const UCHAR png_signature[PEG_PNG_SIGNATURE_BYTES] =
       { 137, 80, 78, 71, 13, 10, 26, 10 };

    return !memcmp( buf, png_signature, PEG_PNG_SIGNATURE_BYTES );
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegPngConvert::InitializeStructures()
{
    CreateReadStruct();

    if (!mpPng)
    {
        return FALSE;
    }

    return TRUE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegPngConvert::ReadPngHeader()
{
    for( ; ; )
    {
        PEG_PNG_IHDR;
        PEG_PNG_IDAT;
        PEG_PNG_IEND;
        PEG_PNG_PLTE;
        PEG_PNG_tRNS;
        UCHAR chunk_length[4];
        DWORD length;

        if (!ReadOK( chunk_length, 4 ))
        {
            return FALSE;
        }

        length = PegPngGetUint32( chunk_length );

        mpPng->mCrc = PzipCrc32( 0, NULL, 0 );
        CrcRead( mpPng->mChunkName, 4 );

        /* This should be a binary subdivision search or a hash for
         * matching the chunk name rather than a linear search.
         */
        if (!memcmp( mpPng->mChunkName, png_IHDR, 4 ))
        {
            if (!HandleIHDR( length ))
            {
                return FALSE;
            }
        }
        else if (!memcmp( mpPng->mChunkName, png_IEND, 4 ))
        {
            if (!HandleIEND( length ))
            {
                return FALSE;
            }
        }
        else if (!memcmp( mpPng->mChunkName, png_PLTE, 4 ))
        {
            if (!HandlePLTE( length ))
            {
                return FALSE;
            }
        }
        else if (!memcmp( mpPng->mChunkName, png_IDAT, 4 ))
        {
            if (!(mpPng->mMode & PEG_PNG_HAVE_IHDR))
            {
                return FALSE;
            }
            else if (mpPng->mColorType == PEG_PNG_COLOR_TYPE_PALETTE &&
                     !(mpPng->mMode & PEG_PNG_HAVE_PLTE))
            {
                return FALSE;
            }

            mpPng->mIdatSize = length;
            mpPng->mMode |= PEG_PNG_HAVE_IDAT;
            break;
        }
        else if (!memcmp( mpPng->mChunkName, png_tRNS, 4 ))
        {
            if (!HandleTRNS( length ))
            {
                return FALSE;
            }
        }
        else
        {
            HandleUnknown( length );
        }
    }
    return TRUE;
}

int idummy;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegPngConvert::ReadPngData()
{
    PegBitmap *p_bmp;
    UCHAR *pRow;
    WORD wInternalBytesPerRow;
    WORD wOutputBytesPerRow;
    SIGNED row, col;
    DWORD lPixVal;

    static const SIGNED adam7dy[7] = { 8, 8, 8, 4, 4, 2, 2 };
    static const SIGNED adam7iy[7] = { 0, 0, 4, 0, 2, 0, 1 };
    static const SIGNED adam7dx[7] = { 8, 8, 4, 4, 2, 2, 1 };
    static const SIGNED adam7ix[7] = { 0, 4, 0, 2, 0, 1, 0 };

    wInternalBytesPerRow = GetByteWidthForInternalColorDepth(mpPng->mWidth);
    wOutputBytesPerRow = GetByteWidthForOutputColorDepth(mpPng->mWidth);

    // For PNG, we always read into the internal buffer first, because
    // the format might not be a supported internal format. So we read
    // into the internal buffer, then copy the data in one of the 
    // supported formats to the final or inline buffer.

    SetupInlineBuf(wInternalBytesPerRow);

    pRow = new UCHAR[mpPng->mRowBytes];

    if (InlineMode())
    {
        p_bmp = AddNewBitmapToList(wOutputBytesPerRow * mpPng->mHeight);
    }
    else
    {
        p_bmp = AddNewBitmapToList(wInternalBytesPerRow * mpPng->mHeight);
    }

    if (!p_bmp)
    {
        return FALSE;
    }

    if (mwMode & PIC_INLINE_DITHER)
    {
        if (!InitDither(mpPng->mWidth))
        {
            return FALSE;
        }
    }

    p_bmp->uFlags = 0;
    p_bmp->wWidth = (SIGNED)mpPng->mWidth;
    p_bmp->wHeight = (SIGNED)mpPng->mHeight;

    if (mpPng->mColorType == PEG_PNG_COLOR_TYPE_GRAY ||
        mpPng->mColorType == PEG_PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        p_bmp->uFlags = BMF_GRAYSCALE;
    }

    UCHAR* pPut;

    if (mpPng->mInterlaced == PEG_PNG_INTERLACE_NONE)
    {
        pPut = p_bmp->pStart;

        for (DWORD y = 0; y < mpPng->mHeight; y++)
        {
            ReadRow(pRow);
            if (InlineMode())
            {
                ConvertRawToPeg(mpInlineReadBuf, pRow);
                if (mwMode & PIC_INLINE_DITHER)
                {
                    DitherBitmapRow(mpInlineReadBuf, pPut, mpPng->mWidth);
                }
                else
                {
                    RemapBitmapRow(mpInlineReadBuf, pPut, mpPng->mWidth);
                }
                pPut += wOutputBytesPerRow;
            }
            else
            {
                ConvertRawToPeg(pPut, pRow);
                pPut += wInternalBytesPerRow;
            }
            mwRowsConverted++;
        }
    }
    else    // mpInfo->mInterlaceType == PEG_PNG_INTERLACE_ADAM7
    {
        if (InlineMode())
        {
            // Cannot do row-by-row remapping of an interlaced image:

            return FALSE;
        }
        for (SIGNED pass = 0; pass < 7; pass++)
        {
            for (row = adam7iy[pass]; row < mpPng->mHeight;
                 row += adam7dy[pass])
            {
                ReadRow(pRow);
                ConvertRawToPeg(mpInlineReadBuf, pRow);

                pPut = p_bmp->pStart;
                pPut += wInternalBytesPerRow * row;

                WORD wCount = 0;
        
                for (col = adam7ix[pass]; col < mpPng->mWidth;
                     col += adam7dx[pass])
                {
                    if (row == 0 && col == 1)
                    {
                        idummy++;
                    }
                    lPixVal = GetRawPixValue(mpInlineReadBuf, wCount);

                    if (lPixVal)
                    {
                        WriteRowPixel(pPut, col, lPixVal);
                    }
                    wCount++;
                }
            }
        }
    }

    delete [] pRow;

    if (InlineMode())
    {
        EndRemapping(p_bmp);
    }
    return TRUE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegPngConvert::WriteRowPixel(UCHAR *pPut, SIGNED col, DWORD dVal)
{
    UCHAR uVal;
    UCHAR uShift;
    UCHAR uData;
    WORD  *pWord;

    switch(muInternalBitsPerPix)
    {
    case 1:
        pPut += col >> 3;
        uVal = (UCHAR) dVal << (7 - (col & 7));
        *pPut |= uVal;
        break;

    case 2:
        pPut +=  col >> 2;
        uShift = (col % 4) * 2;
        uVal = (UCHAR) dVal << (6 - uShift);
        *pPut |= uVal;
        break;
        
    case 4:
        pPut += col >> 1;
        uData = *pPut;
        if (!(col & 1))
        {
            dVal <<= 4;
        }
        uData |= (UCHAR) dVal;
        *pPut = uData;        
        break;

    case 8:
        pPut += col;
        *pPut = (UCHAR) dVal;
        break;

    case 16:
        pWord = (WORD *) pPut;
        pWord += col;
        *pWord = (WORD) dVal;
        break;
        
    case 24:
        pPut += col * 3;
        *pPut++ = (UCHAR) (dVal >> 16);
        *pPut++ = (UCHAR) (dVal >> 8);
        *pPut = (UCHAR) dVal;
        break;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegPngConvert::ConvertRawToPeg(UCHAR *pPut, UCHAR *pGet)
{
    PegPixel pixel;
    WORD wByteWidth;
    WORD wLoop;

    switch(mpPng->mColorType)
    {
    case PEG_PNG_COLOR_TYPE_RGB:

        // this is a supported color type, just copy the RGB values
        // into the output buffer:

        if (mpPng->mBitDepth == 8)
        {
            // this is a supported color type, just copy the RGB values
            // into the output buffer:
            memcpy(pPut, pGet, mpPng->mWidth * 3);
        }
        // bit depth 16
        else // mpPng->mBitDepth == 16
        {
            for (wLoop = 0; wLoop < mpPng->mWidth; wLoop++)
            {
                *pPut++ = *pGet;
                pGet += 2;
                *pPut++ = *pGet;
                pGet += 2;
                *pPut++ = *pGet;
                pGet += 2;
            }
        }
        break;

    case PEG_PNG_COLOR_TYPE_RGB_ALPHA:
        // bit depth 8
        if (mpPng->mBitDepth == 8)
        {
            for (wLoop = 0; wLoop < mpPng->mWidth; wLoop++)
            {
                *pPut++ = *pGet++;
                *pPut++ = *pGet++;
                *pPut++ = *pGet;
                pGet += 2;
            }
        }
        // bit depth 16
        else // mpPng->mBitDepth == 16
        {
            for (wLoop = 0; wLoop < mpPng->mWidth; wLoop++)
            {
                *pPut++ = *pGet;
                pGet += 2;
                *pPut++ = *pGet;
                pGet += 2;
                *pPut++ = *pGet;
                pGet += 4;
            }
        }
        break;

    case PEG_PNG_COLOR_TYPE_PALETTE:
    case PEG_PNG_COLOR_TYPE_GRAY:
        switch(mpPng->mBitDepth)
        {
        case 1:
            wByteWidth = (mpPng->mWidth + 7) / 8;
            memcpy(pPut, pGet, wByteWidth);
            break;

        case 2:
            wByteWidth = (mpPng->mWidth + 3) / 4;
            memcpy(pPut, pGet, wByteWidth);
            break;

        case 4:
            wByteWidth = (mpPng->mWidth + 1) / 2;
            memcpy(pPut, pGet, wByteWidth);
            break;

        case 8:
            memcpy(pPut, pGet, mpPng->mWidth);
            break;

        case 16:
            // convert 16bpp gray to 8bpp gray, we don't support 65535 grays!
            for (wLoop = 0; wLoop < mpPng->mWidth; wLoop++)
            {
                *pPut++ = *pGet;
                pGet += 2;
            }
            break;
        }
        break;

    case  PEG_PNG_COLOR_TYPE_GRAY_ALPHA:
        if (mpPng->mBitDepth == 8)
	    {
            for (wLoop = 0; wLoop < mpPng->mWidth; wLoop++)
            {
                *pPut++ = *pGet;
                pGet += 2;
            }
        }
        else // mpPng->mBitDepth == 16
        {
            for (wLoop = 0; wLoop < mpPng->mWidth; wLoop++)
            {
                *pPut++ = *pGet;
                pGet += 4;
            }
	    }
        break;
    }
}



//****************************************************************************
// png.c - location for general purpose libpng functions
//****************************************************************************
//

/* Calculate the CRC over a section of data.  We can only pass as
 * much data to this routine as the largest single buffer size.  We
 * also check that this data will actually be used before going to the
 * trouble of calculating it.
 */
void PegPngConvert::CalculateCrc( UCHAR* ptr, DWORD length )
{
    SIGNED need_crc = 1;

    if (mpPng->mChunkName[0] & 0x20)                     /* ancillary */
    {
        if ((mpPng->mFlags & PEG_PNG_FLAG_CRC_ANCILLARY_MASK) ==
            (PEG_PNG_FLAG_CRC_ANCILLARY_USE |
             PEG_PNG_FLAG_CRC_ANCILLARY_NOWARN))
        {
            need_crc = 0;
        }
    }
    else                                                    /* critical */
    {
        if (mpPng->mFlags & PEG_PNG_FLAG_CRC_CRITICAL_IGNORE)
        {
            need_crc = 0;
        }
    }

    if (need_crc)
    {
        mpPng->mCrc = PzipCrc32( mpPng->mCrc, ptr, (WORD) length );
    }
}

//
//****************************************************************************
// pngread.c - read a PNG file
//****************************************************************************
//

/* Create a PNG structure for reading, and allocate any memory needed. */
BOOL PegPngConvert::CreateReadStruct()
{
    mpPng = new PegPngStruct;

    if (!mpPng)
    {
        return FALSE;
    }
    else
    {
        memset( mpPng, 0, sizeof( PegPngStruct ) );
    }

    /* initialize mpZbuf - compression buffer */
    mpPng->mZbufSize = PEG_PNG_ZBUF_SIZE;
    mpPng->mpZbuf = new UCHAR[mpPng->mZbufSize];

    if (PzipInflateInit( &mpPng->mZstream, PZIP_DEF_WBITS, NULL,
           sizeof( PzipStream ) ) != PZIP_OK)
    {
        return FALSE;
    }

    mpPng->mZstream.mpNextOut = mpPng->mpZbuf;
    mpPng->mZstream.mAvailOut = (WORD) mpPng->mZbufSize;
    return TRUE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegPngConvert::ReadRow( UCHAR* row )
{
    PEG_PNG_IDAT;
    SIGNED ret;

    /* save jump buffer and error functions */
    if (!(mpPng->mFlags & PEG_PNG_FLAG_ROW_INIT))
    {
        ReadStartRow();
    }

    if (!(mpPng->mMode & PEG_PNG_HAVE_IDAT))
    {
        return FALSE;
    }

    mpPng->mZstream.mpNextOut = mpPng->mpRowBuf;
    mpPng->mZstream.mAvailOut = (WORD) mpPng->mIrowBytes;
    do
    {
        if (!(mpPng->mZstream.mAvailIn))
        {
            while (!mpPng->mIdatSize)
            {
                UCHAR chunk_length[4];

                CrcFinish( 0 );

                if (!ReadOK( chunk_length, 4 ))
                {
                    return FALSE;
                }

                mpPng->mIdatSize = PegPngGetUint32( chunk_length );

                mpPng->mCrc = PzipCrc32( 0, NULL, 0 );;
                CrcRead( mpPng->mChunkName, 4 );

                if (memcmp( mpPng->mChunkName, png_IDAT, 4 ))
                {
                    return FALSE;
                }
            }

            mpPng->mZstream.mAvailIn = (WORD) mpPng->mZbufSize;
            mpPng->mZstream.mpNextIn = mpPng->mpZbuf;

            if (mpPng->mZbufSize > mpPng->mIdatSize)
            {
                mpPng->mZstream.mAvailIn = (WORD) mpPng->mIdatSize;
            }

            CrcRead( mpPng->mpZbuf,
                          (WORD) mpPng->mZstream.mAvailIn );
            mpPng->mIdatSize -= mpPng->mZstream.mAvailIn;
        }

        ret = (SIGNED)PzipInflate( &mpPng->mZstream, PZIP_PARTIAL_FLUSH );

        if (ret == PZIP_STREAM_END)
        {
            if (mpPng->mZstream.mAvailOut || mpPng->mZstream.mAvailIn ||
                mpPng->mIdatSize)
            {
                return FALSE;
            }

            mpPng->mMode |= PEG_PNG_AFTER_IDAT;
            mpPng->mFlags |= PEG_PNG_FLAG_ZLIB_FINISHED;
            break;
        }

        if (ret != PZIP_OK)
        {
            return FALSE;
        }
    } while (mpPng->mZstream.mAvailOut);

    mpPng->mRowInfo.mColorType = mpPng->mColorType;
    mpPng->mRowInfo.mWidth = (WORD) mpPng->mIwidth;
    mpPng->mRowInfo.mChannels = mpPng->mChannels;
    mpPng->mRowInfo.mBitDepth = mpPng->mBitDepth;
    mpPng->mRowInfo.mPixelDepth = mpPng->mPixelDepth;
    mpPng->mRowInfo.mRowBytes = (WORD) (mpPng->mRowInfo.mWidth *
       (DWORD) mpPng->mRowInfo.mPixelDepth + 7) >> 3;

    if (mpPng->mpRowBuf[0])
    {
        ReadFilterRow(
           &(mpPng->mRowInfo), mpPng->mpRowBuf + 1, mpPng->mpPrevRow + 1,
           (SIGNED) (mpPng->mpRowBuf[0]) );
    }

    memcpy( mpPng->mpPrevRow, mpPng->mpRowBuf, mpPng->mRowBytes + 1 );

    if (row)
    {
        CombineRow( row, 0xff );
    }

    ReadFinishRow();
    return TRUE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegPngConvert::DestroyReadStruct()
{
    if (mpPng)
    {
        if (mpPng->mpZbuf) delete mpPng->mpZbuf;
        if (mpPng->mpRowBuf) delete mpPng->mpRowBuf;
        if (mpPng->mpPrevRow) delete mpPng->mpPrevRow;

        if (mpPng->mFlags & PEG_PNG_FLAG_FREE_TRNS)
        {
            if (mpPng->mpTrans) delete mpPng->mpTrans;
        }

        PzipInflateEnd( &mpPng->mZstream );

        delete mpPng;
        mpPng = NULL;
    }
}

//
//****************************************************************************
// pngrutil.c - utilites to read a PNG file
//****************************************************************************
//

/* Grab an unsigned 32-bit integer from a buffer in big-endian format. */
DWORD PegPngConvert::PegPngGetUint32( UCHAR* pBuf )
{
    return ((DWORD) (*pBuf) << 24) + ((DWORD) (*(pBuf + 1)) << 16) +
           ((DWORD) (*(pBuf + 2)) << 8) + (DWORD) (*(pBuf + 3));
}

/* Grab an unsigned 16-bit integer from a buffer in big-endian format. */
WORD PegPngConvert::PegPngGetUint16( UCHAR* pBuf )
{
    return static_cast<WORD>( ((WORD) (*pBuf) << 8) + (WORD) *(pBuf + 1) );
}

/* Read data, and (optionally) run it through the CRC. */
BOOL PegPngConvert::CrcRead( UCHAR* buf, WORD length )
{
    if (!ReadOK( buf, length ))
    {
        return FALSE;
    }

    CalculateCrc( buf, length );
    return TRUE;
}

/* Optionally skip data and then check the CRC.  Depending on whether we
   are reading a ancillary or critical chunk, and how the program has set
   things up, we may calculate the CRC on the data and print a message.
   Returns '1' if there was a CRC error, '0' otherwise. */
SIGNED PegPngConvert::CrcFinish( DWORD skip )
{
    DWORD i;
    DWORD istop = mpPng->mZbufSize;

    for (i = skip; i > istop; i -= istop)
    {
        CrcRead( mpPng->mpZbuf, (WORD)mpPng->mZbufSize );
    }

    if (i)
    {
        CrcRead( mpPng->mpZbuf, (WORD)i );
    }

    if (CrcError())
    {
        if ((!(mpPng->mChunkName[0] & 0x20) ||
             mpPng->mFlags & PEG_PNG_FLAG_CRC_ANCILLARY_NOWARN) &&
            (mpPng->mChunkName[0] & 0x20 ||
             !(mpPng->mFlags & PEG_PNG_FLAG_CRC_CRITICAL_USE)))
        {
            return FALSE;
        }

        return 1;
    }

    return 0;
}

/* Compare the CRC stored in the PNG file with that calculated by libpng from
   the data it has read thus far. */
BOOL PegPngConvert::CrcError()
{
    UCHAR crc_bytes[4];
    DWORD crc;
    SIGNED need_crc = 1;

    if (mpPng->mChunkName[0] & 0x20)                     /* ancillary */
    {
        if ((mpPng->mFlags & PEG_PNG_FLAG_CRC_ANCILLARY_MASK) ==
            (PEG_PNG_FLAG_CRC_ANCILLARY_USE |
             PEG_PNG_FLAG_CRC_ANCILLARY_NOWARN))
        {
            need_crc = 0;
        }
    }
    else if (mpPng->mFlags & PEG_PNG_FLAG_CRC_CRITICAL_IGNORE)   /* critical */
    {
        need_crc = 0;
    }

    if (!ReadOK( crc_bytes, 4 ))
    {
        return TRUE;
    }

    if (need_crc)
    {
        crc = PegPngGetUint32( crc_bytes );
        if (crc != mpPng->mCrc)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/* read and check the IDHR chunk */
BOOL PegPngConvert::HandleIHDR( DWORD length )
{
    UCHAR buf[13];
    DWORD width, height;
    SIGNED bit_depth, color_type, compression_type, filter_type;
    SIGNED interlace_type;
    UCHAR *pPut;

    const static UCHAR PNG_MONO_PALETTE[6] = {0,0,0,0xff,0xff,0xff};
    const static UCHAR PNG_4GRAY_PALETTE[12] = {0,0,0,85,85,85,170,170,170,0xff,0xff,0xff};
    const static UCHAR PNG_16GRAY_PALETTE[48] = {0,0,0,17,17,17,34,34,34,51,51,51,68,68,68,
        85,85,85,102,102,102,119,119,119,136,136,136,153,153,153,170,170,170,
        187,187,187,204,204,204,221,221,221,238,238,238,255,255,255};


    if (mpPng->mMode & PEG_PNG_HAVE_IHDR)
    {
        return FALSE;
    }

    /* check the length */
    if (length != 13)
    {
        return FALSE;
    }

    mpPng->mMode |= PEG_PNG_HAVE_IHDR;

    CrcRead( buf, 13 );
    CrcFinish( 0 );

    width = PegPngGetUint32( buf );
    height = PegPngGetUint32( buf + 4 );
    bit_depth = buf[8];
    color_type = buf[9];
    compression_type = buf[10];
    filter_type = buf[11];
    interlace_type = buf[12];

    /* check for width and height valid values */
    if (width == 0 || width > PEG_PNG_MAX_UINT || height == 0 ||
        height > PEG_PNG_MAX_UINT)
    {
        return FALSE;
    }

    /* check other values */
    if (bit_depth != 1 && bit_depth != 2 && bit_depth != 4 &&
        bit_depth != 8 && bit_depth != 16)
    {
        return FALSE;
    }

    if (color_type < 0 || color_type == 1 || color_type == 5 ||
        color_type > 6)
    {
        return FALSE;
    }

    if ((color_type == PEG_PNG_COLOR_TYPE_PALETTE && bit_depth > 8) ||
        ((color_type == PEG_PNG_COLOR_TYPE_RGB ||
          color_type == PEG_PNG_COLOR_TYPE_GRAY_ALPHA ||
          color_type == PEG_PNG_COLOR_TYPE_RGB_ALPHA) && bit_depth < 8))
    {
        return FALSE;
    }

    if (interlace_type >= PEG_PNG_INTERLACE_LAST)
    {
        return FALSE;
    }

    if (compression_type != PEG_PNG_COMPRESSION_TYPE_BASE)
    {
        return FALSE;
    }

    if (filter_type != PEG_PNG_FILTER_TYPE_BASE)
    {
        return FALSE;
    }

    /* set internal variables */

    mpPng->mWidth = (WORD) width;
    mpPng->mHeight = (WORD) height;
    mpPng->mBitDepth = (UCHAR) bit_depth;
    mpPng->mInterlaced = (UCHAR) interlace_type;
    mpPng->mColorType = (UCHAR) color_type;

    /* find number of channels */
    switch (mpPng->mColorType)
    {
    case PEG_PNG_COLOR_TYPE_GRAY:
        muInternalBitsPerPix = mpPng->mBitDepth;
        mpPng->mChannels = 1;

        switch(muInternalBitsPerPix)
        {
        case 1:
            memcpy(mpLocalPalette, PNG_MONO_PALETTE, 6);
            break;

        case 2:
            memcpy(mpLocalPalette, PNG_4GRAY_PALETTE, 12);
            break;

        case 4:
            memcpy(mpLocalPalette, PNG_16GRAY_PALETTE, 48);
            break;

        case 8:
        case 16:    // reduce 16 bit gray to 8 bit gray
            pPut = mpLocalPalette;            
            for (width = 0; width < 256; width++)
            {
                *pPut++ = (UCHAR) width;
                *pPut++ = (UCHAR) width;
                *pPut++ = (UCHAR) width;
            }
            muInternalBitsPerPix = 8;
        }
        break;

    case PEG_PNG_COLOR_TYPE_PALETTE:
        muInternalBitsPerPix = mpPng->mBitDepth;
        mpPng->mChannels = 1;
        break;

    case PEG_PNG_COLOR_TYPE_RGB:
        mpPng->mChannels = 3;
        muInternalBitsPerPix = 24;
        break;

    case PEG_PNG_COLOR_TYPE_GRAY_ALPHA:
        mpPng->mChannels = 2;
        muInternalBitsPerPix = mpPng->mBitDepth;

        switch(muInternalBitsPerPix)
        {
        case 1:
            memcpy(mpLocalPalette, PNG_MONO_PALETTE, 6);
            break;

        case 2:
            memcpy(mpLocalPalette, PNG_4GRAY_PALETTE, 12);
            break;

        case 4:
            memcpy(mpLocalPalette, PNG_16GRAY_PALETTE, 48);
            break;

        case 8:
        case 16:    // reduce 16 bit gray to 8 bit gray
            pPut = mpLocalPalette;            
            for (width = 0; width < 256; width++)
            {
                *pPut++ = (UCHAR) width;
                *pPut++ = (UCHAR) width;
                *pPut++ = (UCHAR) width;
            }
            muInternalBitsPerPix = 8;
        }
        break;

    case PEG_PNG_COLOR_TYPE_RGB_ALPHA:
        mpPng->mChannels = 4;
        muInternalBitsPerPix = 24;
        break;
    }

    /* set up other useful info */
    mpPng->mPixelDepth = (UCHAR) (mpPng->mBitDepth * mpPng->mChannels);
    mpPng->mRowBytes = (WORD) (mpPng->mWidth * (DWORD) mpPng->mPixelDepth + 7) >> 3;
    return TRUE;
}

/* read and check the palette */
BOOL PegPngConvert::HandlePLTE( DWORD length )
{
    SIGNED index;
    UCHAR *Put;

    if (!(mpPng->mMode & PEG_PNG_HAVE_IHDR))
    {
        return FALSE;
    }
    else if (mpPng->mMode & PEG_PNG_HAVE_IDAT)
    {
        CrcFinish( length );
        return TRUE;
    }
    else if (mpPng->mMode & PEG_PNG_HAVE_PLTE)
    {
        return FALSE;
    }

    mpPng->mMode |= PEG_PNG_HAVE_PLTE;

    if (mpPng->mColorType != PEG_PNG_COLOR_TYPE_PALETTE)
    {
        CrcFinish( length );
        return TRUE;
    }

    if (length > 768 || length % 3)
    {
        if (mpPng->mColorType != PEG_PNG_COLOR_TYPE_PALETTE)
        {
            CrcFinish( length );
            return TRUE;
        }

        return FALSE;
    }

    mwLocalPalSize = (WORD) (length / 3);

    if (mwLocalPalSize > 256)      // check for more than 256 entries, which is the default
                        // local palette size created by the base class
    {
        delete [] mpLocalPalette;
        mpLocalPalette = new UCHAR[length];
    }

    Put = mpLocalPalette;

    for (index = 0; index < mwLocalPalSize; index++)
    {
        UCHAR buf[3];

        CrcRead( buf, 3 );
        *Put++ = buf[0];
        *Put++ = buf[1];
        *Put++ = buf[2];
    }

    /* If we actually NEED the PLTE chunk (ie for a paletted image), we do
       whatever the normal CRC configuration tells us.  However, if we
       have an RGB image, the PLTE can be considered ancillary, so
       we will act as though it is. */
    if (mpPng->mColorType == PEG_PNG_COLOR_TYPE_PALETTE)
    {
        CrcFinish( 0 );
    }
    else if (CrcError())  /* Only if we have a CRC error */
    {
        /* If we don't want to use the data from an ancillary chunk,
           we have two options: an error abort, or a warning and we
           ignore the data in this chunk (which should be OK, since
           it's considered ancillary for a RGB or RGBA image). */

        if (!(mpPng->mFlags & PEG_PNG_FLAG_CRC_ANCILLARY_USE))
        {
            if (mpPng->mFlags & PEG_PNG_FLAG_CRC_ANCILLARY_NOWARN)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }

    mpPng->mFlags |= PEG_PNG_FLAG_FREE_PLTE;
    mpPng->mValid |= PEG_PNG_INFO_PLTE;

    if (mpPng->mColorType == PEG_PNG_COLOR_TYPE_PALETTE)
    {
        if (mpPng && (mpPng->mValid & PEG_PNG_INFO_tRNS))
        {
            if (mpPng->mNumTrans >  mwLocalPalSize)
            {
                mpPng->mNumTrans = mwLocalPalSize;
            }
        }
    }
    return TRUE;
}

BOOL PegPngConvert::HandleIEND( DWORD length )
{
    if (!(mpPng->mMode & PEG_PNG_HAVE_IHDR) ||
        !(mpPng->mMode & PEG_PNG_HAVE_IDAT))
    {
        return FALSE;
    }

    mpPng->mMode |= (PEG_PNG_AFTER_IDAT | PEG_PNG_HAVE_IEND);

    CrcFinish( length );
    return TRUE;
}

BOOL PegPngConvert::HandleTRNS( DWORD length )
{
    if (!(mpPng->mMode & PEG_PNG_HAVE_IHDR))
    {
        return FALSE;
    }
    else if (mpPng->mMode & PEG_PNG_HAVE_IDAT)
    {
        CrcFinish( length );
        return TRUE;
    }
    else if (mpPng && (mpPng->mValid & PEG_PNG_INFO_tRNS))
    {
        CrcFinish( length );
        return TRUE;
    }

    if (mpPng->mColorType == PEG_PNG_COLOR_TYPE_PALETTE)
    {
        if (mpPng->mMode & PEG_PNG_HAVE_PLTE &&
            length > mwLocalPalSize)
        {
            CrcFinish( length );
            return TRUE;
        }

        if (length == 0)
        {
            CrcFinish( length );
            return TRUE;
        }

        mpPng->mpTrans = new UCHAR[length];
        CrcRead( mpPng->mpTrans, (WORD) length );
        mpPng->mNumTrans = (WORD) length;
    }
    else if (mpPng->mColorType == PEG_PNG_COLOR_TYPE_RGB)
    {
        UCHAR buf[6];

        if (length != 6)
        {
            CrcFinish( length );
            return TRUE;
        }

        CrcRead( buf, (WORD) length );
        mpPng->mNumTrans = 1;
        mpPng->mTransVal.Red = buf[0];
        mpPng->mTransVal.Green = buf[2];
        mpPng->mTransVal.Blue = buf[4];
    }
    else if (mpPng->mColorType == PEG_PNG_COLOR_TYPE_GRAY)
    {
        UCHAR buf[6];

        if (length != 2)
        {
            CrcFinish( length );
            return TRUE;
        }

        CrcRead( buf, 2 );
        mpPng->mNumTrans = 1;
        mpPng->mTransVal.Red = mpPng->mTransVal.Green = mpPng->mTransVal.Blue = buf[0];
    }
    else
    {
        CrcFinish( length );
        return TRUE;
    }

    if ( CrcFinish( 0 ) )
    {
        return TRUE;
    }

    if (mpPng->mColorType == PEG_PNG_COLOR_TYPE_PALETTE)
    {
        mpPng->mFlags |= PEG_PNG_FLAG_FREE_TRNS;
    }

    mpPng->mValid |= PEG_PNG_INFO_tRNS;
    return TRUE;
}

/* This function is called when we haven't found a handler for a
   chunk.  If there isn't a problem with the chunk itself (ie bad
   chunk name, CRC, or a critical chunk), the chunk is silently ignored.
  */
BOOL PegPngConvert::HandleUnknown( DWORD length )
{
    DWORD skip;

    if (mpPng->mMode & PEG_PNG_HAVE_IDAT)
    {
        PEG_PNG_IDAT;

        if (memcmp( mpPng->mChunkName, png_IDAT, 4 ))  /* not an IDAT */
        {
            mpPng->mMode |= PEG_PNG_AFTER_IDAT;
        }
    }

    CheckChunkName( mpPng->mChunkName );

    if (!(mpPng->mChunkName[0] & 0x20))
    {
        return FALSE;
    }

    skip = length;

    CrcFinish( skip );
    return TRUE;
}

/* This function is called to verify that a chunk name is valid.
   This function can't have the "critical chunk check" incorporated
   into it, since in the future we will need to be able to call user
   functions to handle unknown critical chunks after we check that
   the chunk name itself is valid. */
#define isnonalpha(c) ((c) < 41 || (c) > 122 || ((c) > 90 && (c) < 97))
BOOL PegPngConvert::CheckChunkName( UCHAR* chunk_name )
{
    if (isnonalpha(chunk_name[0]) || isnonalpha(chunk_name[1]) ||
        isnonalpha(chunk_name[2]) || isnonalpha(chunk_name[3]))
    {
        return FALSE;
    }
    return TRUE;
}


/* Combines the row recently read in with the existing pixels in the
   row.  This routine takes care of alpha and transparency if requested.
   This routine also handles the two methods of progressive display
   of interlaced images, depending on the mask value.
   The mask value describes which pixels are to be combined with
   the row.  The pattern always repeats every 8 pixels, so just 8
   bits are needed.  A one indicates the pixel is to be combined,
   a zero indicates the pixel is to be skipped.  This is in addition
   to any alpha or transparency value associated with the pixel.  If
   you want all pixels to be combined, pass 0xff (255) in mask.  */
void PegPngConvert::CombineRow( UCHAR* row, SIGNED mask )
{
    if (mask == 0xff)
    {
        memcpy( row, mpPng->mpRowBuf + 1,
                (DWORD) ((mpPng->mWidth * mpPng->mRowInfo.mPixelDepth +
                   7) >> 3));
    }
    else
    {
        switch (mpPng->mRowInfo.mPixelDepth)
        {
            case 1:
            {
                UCHAR* sp = mpPng->mpRowBuf + 1;
                UCHAR* dp = row;
                SIGNED s_inc, s_start, s_end;
                SIGNED m = 0x80;
                SIGNED shift;
                DWORD i;
                DWORD row_width = mpPng->mWidth;

                s_start = 7;
                s_end = 0;
                s_inc = -1;

                shift = s_start;

                for (i = 0; i < row_width; i++)
                {
                    if (m & mask)
                    {
                        SIGNED value;

                        value = static_cast<SIGNED>( (*sp >> shift) & 0x01 );
                        *dp &= (UCHAR) ((0x7f7f >> (7 - shift)) & 0xff);
                        *dp |= (UCHAR) (value << shift);
                    }

                    if (shift == s_end)
                    {
                        shift = s_start;
                        sp++;
                        dp++;
                    }
                    else
                    {
                        shift = static_cast<SIGNED>( shift + s_inc );
                    }

                    if (m == 1)
                    {
                        m = 0x80;
                    }
                    else
                    {
                        m >>= 1;
                    }
                }
                break;
            }

            case 2:
            {
                UCHAR* sp = mpPng->mpRowBuf + 1;
                UCHAR* dp = row;
                SIGNED s_start, s_end, s_inc;
                SIGNED m = 0x80;
                SIGNED shift;
                DWORD i;
                DWORD row_width = mpPng->mWidth;
                SIGNED value;

                s_start = 6;
                s_end = 0;
                s_inc = -2;

                shift = s_start;

                for (i = 0; i < row_width; i++)
                {
                    if (m & mask)
                    {
                        value = static_cast<SIGNED>( (*sp >> shift) & 0x03 );
                        *dp &= (UCHAR) ((0x3f3f >> (6 - shift)) & 0xff);
                        *dp |= (UCHAR) (value << shift);
                    }

                    if (shift == s_end)
                    {
                        shift = s_start;
                        sp++;
                        dp++;
                    }
                    else
                    {
                        shift = static_cast<SIGNED>( shift + s_inc );
                    }

                    if (m == 1)
                    {
                        m = 0x80;
                    }
                    else
                    {
                        m >>= 1;
                    }
                }
                break;
            }

            case 4:
            {
                UCHAR* sp = mpPng->mpRowBuf + 1;
                UCHAR* dp = row;
                SIGNED s_start, s_end, s_inc;
                SIGNED m = 0x80;
                SIGNED shift;
                DWORD i;
                DWORD row_width = mpPng->mWidth;
                SIGNED value;

                s_start = 4;
                s_end = 0;
                s_inc = -4;
                shift = s_start;

                for (i = 0; i < row_width; i++)
                {
                    if (m & mask)
                    {
                        value = static_cast<SIGNED>( (*sp >> shift) & 0xf );
                        *dp &= (UCHAR) ((0xf0f >> (4 - shift)) & 0xff);
                        *dp |= (UCHAR) (value << shift);
                    }

                    if (shift == s_end)
                    {
                        shift = s_start;
                        sp++;
                        dp++;
                    }
                    else
                    {
                        shift = static_cast<SIGNED>( shift + s_inc );
                    }

                    if (m == 1)
                    {
                        m = 0x80;
                    }
                    else
                    {
                        m >>= 1;
                    }
                }
                break;
            }

            default:
            {
                UCHAR* sp = mpPng->mpRowBuf + 1;
                UCHAR* dp = row;
                DWORD pixel_bytes = (mpPng->mRowInfo.mPixelDepth >> 3);
                DWORD i;
                DWORD row_width = mpPng->mWidth;
                UCHAR m = 0x80;


                for (i = 0; i < row_width; i++)
                {
                    if (m & mask)
                    {
                        memcpy( dp, sp, pixel_bytes );
                    }

                    sp += pixel_bytes;
                    dp += pixel_bytes;

                    if (m == 1)
                    {
                        m = 0x80;
                    }
                    else
                    {
                        m >>= 1;
                    }
                }
                break;
            }
        }
    }
}

void PegPngConvert::ReadFilterRow(
   PegPngRowInfo* row_info, UCHAR* row, UCHAR* prev_row, SIGNED filter )
{
    switch (filter)
    {
        case PEG_PNG_FILTER_VALUE_NONE:
            break;

        case PEG_PNG_FILTER_VALUE_SUB:
        {
            DWORD i;
            DWORD istop = row_info->mRowBytes;
            DWORD bpp = (row_info->mPixelDepth + 7) >> 3;
            UCHAR* rp = row + bpp;
            UCHAR* lp = row;

            for (i = bpp; i < istop; i++)
            {
                *rp = (UCHAR) (((SIGNED) (*rp) + (SIGNED) (*lp++)) & 0xff);
                rp++;
            }
            break;
        }

        case PEG_PNG_FILTER_VALUE_UP:
        {
            DWORD i;
            DWORD istop = row_info->mRowBytes;
            UCHAR* rp = row;
            UCHAR* pp = prev_row;

            for (i = 0; i < istop; i++)
            {
                *rp = (UCHAR) (((SIGNED) (*rp) + (SIGNED) (*pp++)) & 0xff);
                rp++;
            }
            break;
        }

        case PEG_PNG_FILTER_VALUE_AVG:
        {
            DWORD i;
            UCHAR* rp = row;
            UCHAR* pp = prev_row;
            UCHAR* lp = row;
            DWORD bpp = (row_info->mPixelDepth + 7) >> 3;
            DWORD istop = row_info->mRowBytes - bpp;

            for (i = 0; i < bpp; i++)
            {
                *rp = (UCHAR) (((SIGNED) (*rp) +
                   ((SIGNED) (*pp++) / 2 )) & 0xff);
                rp++;
            }

            for (i = 0; i < istop; i++)
            {
                *rp = (UCHAR) (((SIGNED) (*rp) +
                   (SIGNED) (*pp++ + *lp++) / 2 ) & 0xff);
                rp++;
            }
            break;
        }

        case PEG_PNG_FILTER_VALUE_PAETH:
        {
            DWORD i;
            UCHAR* rp = row;
            UCHAR* pp = prev_row;
            UCHAR* lp = row;
            UCHAR* cp = prev_row;
            DWORD bpp = (row_info->mPixelDepth + 7) >> 3;
            DWORD istop=row_info->mRowBytes - bpp;

            for (i = 0; i < bpp; i++)
            {
                *rp = (UCHAR) (((SIGNED) (*rp) + (SIGNED) (*pp++)) & 0xff);
                rp++;
            }

            for (i = 0; i < istop; i++)   /* use leftover rp,pp */
            {
                SIGNED a, b, c, pa, pb, pc, p;

                a = *lp++;
                b = *pp++;
                c = *cp++;

                p = static_cast<SIGNED>( b - c );
                pc = static_cast<SIGNED>( a - c );

                pa = static_cast<SIGNED>( p < 0 ? -p : p );
                pb = static_cast<SIGNED>( pc < 0 ? -pc : pc );
                pc = static_cast<SIGNED>( (p + pc) < 0 ? -(p + pc) : p + pc );

                p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;

                *rp = (UCHAR) (((SIGNED) (*rp) + p) & 0xff);
                rp++;
            }
            break;
        }

        default:
            *row=0;
            break;
    }
}

BOOL PegPngConvert::ReadFinishRow()
{
    /* arrays to facilitate easy interlacing - use pass (0 - 6) as index */

    /* start of interlace block */
    const SIGNED png_pass_start[7] = { 0, 4, 0, 2, 0, 1, 0 };

    /* offset to next interlace block */
    const SIGNED png_pass_inc[7] = { 8, 8, 4, 4, 2, 2, 1 };

    /* start of interlace block in the y direction */
    const SIGNED png_pass_ystart[7] = { 0, 0, 4, 0, 2, 0, 1 };

    /* offset to next interlace block in the y direction */
    const SIGNED png_pass_yinc[7] = { 8, 8, 8, 4, 4, 2, 2 };

    mpPng->mRowNumber++;

    if (mpPng->mRowNumber < mpPng->mNumRows)
    {
        return TRUE;
    }

    if (mpPng->mInterlaced)
    {
        mpPng->mRowNumber = 0;
        memset( mpPng->mpPrevRow, 0, mpPng->mRowBytes + 1 );

        do
        {
            mpPng->mPass++;

            if (mpPng->mPass >= 7)
            {
                break;
            }

            mpPng->mIwidth = (mpPng->mWidth + png_pass_inc[mpPng->mPass] -
               1 - png_pass_start[mpPng->mPass]) / png_pass_inc[mpPng->mPass];

            mpPng->mIrowBytes = ((mpPng->mIwidth *
               (DWORD) mpPng->mPixelDepth + 7) >> 3) + 1;

            mpPng->mNumRows = (mpPng->mHeight + png_pass_yinc[mpPng->mPass] -
               1 - png_pass_ystart[mpPng->mPass]) /
               png_pass_yinc[mpPng->mPass];

            if (!(mpPng->mNumRows))
            {
                continue;
            }
        } while (mpPng->mIwidth == 0);

        if (mpPng->mPass < 7)
        {
            return TRUE;
        }
    }

    if (!(mpPng->mFlags & PEG_PNG_FLAG_ZLIB_FINISHED))
    {
        PEG_PNG_IDAT;
        PEGCHAR extra;
        SIGNED ret;

        mpPng->mZstream.mpNextOut = (UCHAR*) &extra;
        mpPng->mZstream.mAvailOut = (WORD) 1;

        for( ; ; )
        {
            if (!(mpPng->mZstream.mAvailIn))
            {
                while (!mpPng->mIdatSize)
                {
                    UCHAR chunk_length[4];

                    CrcFinish( 0 );

                    if (!ReadOK( chunk_length, 4 ))
                    {
                        return FALSE;
                    }

                    mpPng->mIdatSize = PegPngGetUint32( chunk_length );

                    mpPng->mCrc = PzipCrc32( 0, NULL, 0 );;
                    CrcRead( mpPng->mChunkName, 4 );

                    if (memcmp( mpPng->mChunkName, (UCHAR*) png_IDAT, 4 ))
                    {
                        return FALSE;
                    }
                }

                mpPng->mZstream.mAvailIn = (WORD) mpPng->mZbufSize;
                mpPng->mZstream.mpNextIn = mpPng->mpZbuf;

                if (mpPng->mZbufSize > mpPng->mIdatSize)
                {
                    mpPng->mZstream.mAvailIn = (WORD) mpPng->mIdatSize;
                }

                CrcRead( mpPng->mpZbuf, (WORD)mpPng->mZstream.mAvailIn );
                mpPng->mIdatSize -= mpPng->mZstream.mAvailIn;
            }

            ret = (SIGNED)PzipInflate( &mpPng->mZstream, PZIP_PARTIAL_FLUSH );

            if (ret == PZIP_STREAM_END)
            {
                if (!(mpPng->mZstream.mAvailOut) ||
                    mpPng->mZstream.mAvailIn || mpPng->mIdatSize)
                {
                    return FALSE;
                }

                mpPng->mMode |= PEG_PNG_AFTER_IDAT;
                mpPng->mFlags |= PEG_PNG_FLAG_ZLIB_FINISHED;
                break;
            }

            if (ret != PZIP_OK)
            {
                return FALSE;
            }

            if (!(mpPng->mZstream.mAvailOut))
            {
                return FALSE;
            }
        }

        mpPng->mZstream.mAvailOut = 0;
    }

    if (mpPng->mIdatSize || mpPng->mZstream.mAvailIn)
    {
        return FALSE;
    }

    PzipInflateReset( &mpPng->mZstream );

    mpPng->mMode |= PEG_PNG_AFTER_IDAT;
    return TRUE;
}

BOOL PegPngConvert::ReadStartRow()
{
    /* arrays to facilitate easy interlacing - use mPass (0 - 6) as index */

    /* start of interlace block */
    const SIGNED png_pass_start[7] = { 0, 4, 0, 2, 0, 1, 0 };

    /* offset to next interlace block */
    const SIGNED png_pass_inc[7] = { 8, 8, 4, 4, 2, 2, 1 };

    /* start of interlace block in the y direction */
    const SIGNED png_pass_ystart[7] = { 0, 0, 4, 0, 2, 0, 1 };

    /* offset to next interlace block in the y direction */
    const SIGNED png_pass_yinc[7] = { 8, 8, 8, 4, 4, 2, 2 };

    SIGNED max_pixel_depth;
    DWORD row_bytes;

    mpPng->mZstream.mAvailIn = 0;

    if (mpPng->mInterlaced)
    {
        mpPng->mNumRows = (mpPng->mHeight + png_pass_yinc[0] - 1 -
           png_pass_ystart[0]) / png_pass_yinc[0];

        mpPng->mIwidth = (mpPng->mWidth + png_pass_inc[mpPng->mPass] - 1 -
           png_pass_start[mpPng->mPass]) / png_pass_inc[mpPng->mPass];

        row_bytes = ((mpPng->mIwidth * (DWORD) mpPng->mPixelDepth + 7) >> 3) +
           1;

        mpPng->mIrowBytes = (DWORD) row_bytes;

        if ((DWORD) mpPng->mIrowBytes != row_bytes)
        {
            return FALSE;
        }
    }
    else
    {
        mpPng->mNumRows = mpPng->mHeight;
        mpPng->mIwidth = mpPng->mWidth;
        mpPng->mIrowBytes = mpPng->mRowBytes + 1;
    }

    max_pixel_depth = mpPng->mPixelDepth;

    /* align the width on the next larger 8 pixels.  Mainly used for
     * interlacing */
    row_bytes = ((mpPng->mWidth + 7) & ~((DWORD) 7));

    /* calculate the maximum bytes needed, adding a byte and a pixel for
     * safety's sake */
    row_bytes = ((row_bytes * (DWORD) max_pixel_depth + 7) >> 3) + 1 +
       ((max_pixel_depth + 7) >> 3);

    mpPng->mpRowBuf = new UCHAR[row_bytes];

    mpPng->mpPrevRow = new UCHAR[mpPng->mRowBytes + 1];

    memset( mpPng->mpPrevRow, 0, mpPng->mRowBytes + 1 );

    mpPng->mFlags |= PEG_PNG_FLAG_ROW_INIT;
    return TRUE;
}

#endif  // PEG_PNG_DECODER

