/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ppngconv.hpp - PNG decompressor object.
//
// Author: Kenneth G. Maxwell
//
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

#ifndef _PEGPNGCONVERT_
#define _PEGPNGCONVERT_

#if defined(PEG_PNG_DECODER)

//
//****************************************************************************
// pngconf.h - machine configurable file for libpng
//****************************************************************************
//

/* This is the size of the compression buffer, and thus the size of
 * an IDAT chunk.  Make this whatever size you feel is best for your
 * machine.  One of these will be allocated per PegPngStruct.  When this
 * is full, it writes the data to the disk, and does some other
 * calculations.  Making this an extremely small size will slow
 * the library down, but you may want to experiment to determine
 * where it becomes significant, if you are concerned with memory
 * usage.  Note that zlib allocates at least 32Kb also.  For readers,
 * this describes the size of the buffer available to read the data in.
 * Unless this gets smaller than the size of a row (compressed),
 * it should not make much difference how big this is.
 */

#define PEG_PNG_ZBUF_SIZE 8192

//
//****************************************************************************
// png.h - header file for PNG reference library
//****************************************************************************
//

/* Three color definitions.  The order of the mRed, mGreen, and mBlue (and
 * the exact size) is not important, although the size of the fields need to
 * be UCHAR or WORD (as defined below).
 */

#if 0

typedef struct
{
    UCHAR mRed;
    UCHAR mGreen;
    UCHAR mBlue;
} PegPngColor;

typedef struct
{
    UCHAR mIndex;    /* used for palette files */
    WORD mRed;   /* for use in red green blue files */
    WORD mGreen;
    WORD mBlue;
    WORD mGray;  /* for use in grayscale files */
} PegPngColor16;

#endif

/* color type masks */
#define PEG_PNG_COLOR_MASK_PALETTE 1
#define PEG_PNG_COLOR_MASK_COLOR 2
#define PEG_PNG_COLOR_MASK_ALPHA 4

/* Maximum positive integer used in PNG is (2^31) - 1 */
#define PEG_PNG_MAX_UINT ((DWORD) 0x7FFFFFFFL)

/* color types.  Note that not all combinations are legal */
#define PEG_PNG_COLOR_TYPE_GRAY 0
#define PEG_PNG_COLOR_TYPE_PALETTE \
        (PEG_PNG_COLOR_MASK_COLOR | PEG_PNG_COLOR_MASK_PALETTE)
#define PEG_PNG_COLOR_TYPE_RGB (PEG_PNG_COLOR_MASK_COLOR)
#define PEG_PNG_COLOR_TYPE_RGB_ALPHA \
        (PEG_PNG_COLOR_MASK_COLOR | PEG_PNG_COLOR_MASK_ALPHA)
#define PEG_PNG_COLOR_TYPE_GRAY_ALPHA (PEG_PNG_COLOR_MASK_ALPHA)

/* This is for compression type. PNG 1.0-1.2 only define the single type. */
#define PEG_PNG_COMPRESSION_TYPE_BASE 0 /* Deflate method 8, 32K window */

/* This is for filter type. PNG 1.0-1.2 only define the single type. */
#define PEG_PNG_FILTER_TYPE_BASE 0 /* Single row per-byte filtering */

/* These are for the interlacing type.  These values should NOT be changed. */
#define PEG_PNG_INTERLACE_NONE 0 /* Non-interlaced image */
#define PEG_PNG_INTERLACE_ADAM7 1 /* Adam7 interlacing */
#define PEG_PNG_INTERLACE_LAST 2 /* Not a valid value */

/* These determine if an ancillary chunk's data has been successfully read
 * from the PNG header, or if the application has filled in the corresponding
 * data in the info_struct to be written into the output file.  The values
 * of the PNG_INFO_<chunk> defines should NOT be changed.
 */
#define PEG_PNG_INFO_PLTE 0x0008
#define PEG_PNG_INFO_tRNS 0x0010

/* This is used for the transformation routines, as some of them
 * change these values for the row.  It also should enable using
 * the routines for other purposes.
 */
typedef struct
{
    WORD mWidth; /* width of row */
    WORD mRowBytes; /* number of bytes in row */
    UCHAR mColorType; /* color type of row */
    UCHAR mBitDepth; /* bit depth of row */
    UCHAR mChannels; /* number of channels (1, 2, 3, or 4) */
    UCHAR mPixelDepth; /* bits per pixel (mBitDepth * mChannels) */
} PegPngRowInfo;

/* The structure that holds the information to read and write PNG files.
 * The only people who need to care about what is inside of this are the
 * people who will be modifying the library for their own special needs.
 * It should NOT be accessed directly by an application, except to store
 * the jmp_buf.
 */
typedef struct
{
    DWORD mMode;          /* tells us where we are in the PNG file */
    DWORD mFlags;         /* flags indicating various things to libpng */
    WORD mValid;

    PzipStream mZstream;      /* pointer to decompression structure (below) */
    UCHAR* mpZbuf;            /* buffer for zlib */
    DWORD mZbufSize;      /* size of mpZbuf */
    SIGNED mZlibLevel;            /* holds zlib compression level */
    SIGNED mZlibMethod;           /* holds zlib compression method */
    SIGNED mZlibWindowBits;      /* holds zlib compression window bits */
    SIGNED mZlibMemLevel;        /* holds zlib compression memory level */
    SIGNED mZlibStrategy;         /* holds zlib compression strategy */

    WORD mWidth;         /* width of image in pixels */
    WORD mHeight;        /* height of image in pixels */
    DWORD mNumRows;      /* number of rows in current pass */
    DWORD mUsrWidth;     /* width of row at start of write */
    WORD mRowBytes;      /* size of row in bytes */
    DWORD mIrowBytes;     /* size of current interlaced row in bytes */
    DWORD mIwidth;        /* width of current interlaced row in pixels */
    WORD mRowNumber;    /* current row in interlace pass */
    UCHAR* mpPrevRow;        /* buffer to save previous (unfiltered) row */
    UCHAR* mpRowBuf;         /* buffer to save current (unfiltered) row */
    UCHAR* mpSubRow;         /* buffer to save "sub" row when filtering */
    UCHAR* mpUpRow;          /* buffer to save "up" row when filtering */
    UCHAR* mpAvgRow;         /* buffer to save "avg" row when filtering */
    UCHAR* mpPaethRow;       /* buffer to save "Paeth" row when filtering */
    PegPngRowInfo mRowInfo;     /* used for transformation routines */

    DWORD mIdatSize;     /* current IDAT size for read */
    DWORD mCrc;           /* current chunk CRC value */
//    PegPngColor* mpPalette;        /* palette from the input file */
//    WORD mNumPalette;   /* number of color entries in palette */
    WORD mNumTrans;     /* number of transparency values */
    UCHAR mChunkName[5];    /* null-terminated name of current chunk */
    UCHAR mInterlaced;       /* PEG_PNG_INTERLACE_NONE, PEG_PNG_INTERLACE_ADAM7 */
    UCHAR mPass;             /* current interlace pass (0 - 6) */
    UCHAR mDoFilter;        /* row filter flags (see PNG_FILTER_ below ) */
    UCHAR mColorType;       /* color type of file */
    UCHAR mBitDepth;        /* bit depth of file */
    UCHAR mUsrBitDepth;    /* bit depth of users row */
    UCHAR mPixelDepth;      /* number of bits per pixel */
    UCHAR mChannels;         /* number of channels in file */
    UCHAR mUsrChannels;     /* channels at start of write */

    DWORD mFlushDist;    /* how many rows apart to flush, 0 - no flush */
    DWORD mFlushRows;    /* number of rows written since last flush */

    UCHAR* mpTrans;           /* transparency values for paletted files */
    PegPixel mTransVal; // transparency values for non-paletted files
} PegPngStruct;

/* Flags to say which filters to use.  The flags are chosen so that they don't
 * conflict with real filter types below, in case they are supplied instead of
 * the #defined constants.  These values should NOT be changed.
 */
#define PEG_PNG_NO_FILTERS 0x00
#define PEG_PNG_FILTER_NONE 0x08
#define PEG_PNG_FILTER_SUB 0x10
#define PEG_PNG_FILTER_UP 0x20
#define PEG_PNG_FILTER_AVG 0x40
#define PEG_PNG_FILTER_PAETH 0x80
#define PEG_PNG_ALL_FILTERS \
        (PEG_PNG_FILTER_NONE | PEG_PNG_FILTER_SUB | PEG_PNG_FILTER_UP | \
         PEG_PNG_FILTER_AVG | PEG_PNG_FILTER_PAETH)

/* Filter values (not flags) - used in pngwrite.c, pngwutil.c for now.
 * These defines should NOT be changed.
 */
#define PEG_PNG_FILTER_VALUE_NONE 0
#define PEG_PNG_FILTER_VALUE_SUB 1
#define PEG_PNG_FILTER_VALUE_UP 2
#define PEG_PNG_FILTER_VALUE_AVG 3
#define PEG_PNG_FILTER_VALUE_PAETH 4
#define PEG_PNG_FILTER_VALUE_LAST 5

/* Various modes of operation.  Note that after an init, mMode is set to
 * zero automatically when the structure is created.
 */
#define PEG_PNG_HAVE_IHDR 0x01
#define PEG_PNG_HAVE_PLTE 0x02
#define PEG_PNG_HAVE_IDAT 0x04
#define PEG_PNG_AFTER_IDAT 0x08
#define PEG_PNG_HAVE_IEND 0x10

/* flags for the png_ptr->mFlags rather than declaring a byte for each one */
#define PEG_PNG_FLAG_ZLIB_CUSTOM_STRATEGY 0x0001
#define PEG_PNG_FLAG_ZLIB_CUSTOM_LEVEL 0x0002
#define PEG_PNG_FLAG_ZLIB_CUSTOM_MEM_LEVEL 0x0004
#define PEG_PNG_FLAG_ZLIB_CUSTOM_WINDOW_BITS 0x0008
#define PEG_PNG_FLAG_ZLIB_CUSTOM_METHOD 0x0010
#define PEG_PNG_FLAG_ZLIB_FINISHED 0x0020
#define PEG_PNG_FLAG_ROW_INIT 0x0040
#define PEG_PNG_FLAG_CRC_ANCILLARY_USE 0x0100
#define PEG_PNG_FLAG_CRC_ANCILLARY_NOWARN 0x0200
#define PEG_PNG_FLAG_CRC_CRITICAL_USE 0x0400
#define PEG_PNG_FLAG_CRC_CRITICAL_IGNORE 0x0800
#define PEG_PNG_FLAG_FREE_PLTE 0x1000
#define PEG_PNG_FLAG_FREE_TRNS 0x2000

#define PEG_PNG_FLAG_CRC_ANCILLARY_MASK \
        (PEG_PNG_FLAG_CRC_ANCILLARY_USE | PEG_PNG_FLAG_CRC_ANCILLARY_NOWARN)

/* Constant strings for known chunk types.  If you need to add a chunk,
 * define the name here, and add an invocation of the macro in png.c and
 * wherever it's needed.
 */
#define PEG_PNG_IHDR const UCHAR png_IHDR[5] = { 73,  72,  68,  82, '\0' }
#define PEG_PNG_IDAT const UCHAR png_IDAT[5] = { 73,  68,  65,  84, '\0' }
#define PEG_PNG_IEND const UCHAR png_IEND[5] = { 73,  69,  78,  68, '\0' }
#define PEG_PNG_PLTE const UCHAR png_PLTE[5] = { 80,  76,  84,  69, '\0' }
#define PEG_PNG_tRNS const UCHAR png_tRNS[5] = { 116,  82,  78,  83, '\0' }

//
//****************************************************************************
// ppngconv.hpp
//****************************************************************************
//

// the total number of signature bytes (all are checked)
#define PEG_PNG_SIGNATURE_BYTES 8

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegPngConvert : public PegImageConvert
{
public:

    PegPngConvert( WORD wId = 0 );
    virtual ~PegPngConvert();

#if defined(PIC_FILE_MODE)
    // fd read pointer must point to the beginning of fd; all PNG signature
    //    bytes are checked.  There is a quick return of FALSE if fd does not
    //    have a valid PNG signature.
    BOOL ReadImage( FILE* fd, SIGNED iCount = 1 );
#else
    BOOL ReadImage( SIGNED iCount = 1 );
#endif

    // Accessor functions should not be invoked unless
    //    ReadImage has been invoked and
    //    the most recent invocation of ReadImage returned TRUE.

    DWORD Width() const { return mpPng->mWidth; };
    DWORD Height() const { return mpPng->mHeight; };
    DWORD BytesPerRow() const { return mpPng->mRowBytes; };
    UCHAR BitsPerChannel() const { return mpPng->mBitDepth; };
    UCHAR ColorType() const { return mpPng->mColorType; };
    UCHAR InterlaceType() const { return mpPng->mInterlaced; };
    UCHAR NumChannels() const { return mpPng->mChannels; };

protected:

private:

    BOOL IsValid();

    BOOL InitializeStructures();

    BOOL ReadPngHeader();

    BOOL ReadPngData();
    void ConvertRawToPeg(UCHAR *pPut, UCHAR *pGet);
    void WriteRowPixel(UCHAR *pPut, SIGNED col, DWORD dVal);

    PegPngStruct* mpPng;

    /* Calculate the CRC over a section of data.  Note that we are only
     * passing a maximum of 64K on systems that have this as a memory limit,
     * since this is the maximum buffer size we can specify.
     */
    void CalculateCrc( UCHAR* ptr, DWORD length );

    /* Allocate and initialize mpPng for reading, and any other memory. */
    BOOL CreateReadStruct();

    /* read a row of data.*/
    BOOL ReadRow( UCHAR* row );

    /* free any memory associated with mpPng and mpInfo */
    void DestroyReadStruct();

    DWORD PegPngGetUint32( UCHAR* pBuf );

    WORD PegPngGetUint16( UCHAR* pBuf );

    /* Read bytes into buf, and update mpPng->mCrc */
    BOOL CrcRead( UCHAR* buf, WORD length );

    /* Read "skip" bytes, read the file crc, and (optionally) verify
     * mpPng->mCrc */
    SIGNED CrcFinish( DWORD skip );

    /* Read the CRC from the file and compare it to the libpng calculated
     * CRC */
    BOOL CrcError();

    /* decode the IHDR chunk */
    BOOL HandleIHDR( DWORD length );

    BOOL HandlePLTE( DWORD length );

    BOOL HandleIEND( DWORD length );

    BOOL HandleTRNS( DWORD length );

    BOOL HandleUnknown( DWORD length );

    BOOL CheckChunkName( UCHAR* chunk_name );

    /* combine a row of data, dealing with alpha, etc. if requested */

    void CombineRow( UCHAR* row, SIGNED mask );

    /* unfilter a row */

    void ReadFilterRow( PegPngRowInfo* row_info, UCHAR* row, UCHAR* prev_row,
                        SIGNED filter );

    /* finish a row while reading, dealing with interlacing passes, etc. */

    BOOL ReadFinishRow();

    /* initialize the row buffers, etc. */

    BOOL ReadStartRow();
};

#endif  // PEG_PNG_DECODER

#endif  // _PEGPNGCONVERT_

