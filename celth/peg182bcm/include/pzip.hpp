/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pzip.hpp - runtime compress and decompress utilities.
//
// Author: Jeremy Dalmer
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
// The low-level ZLIB utilities are derived heavily from the work of
// Jean-loup Gailly and Mark Adler.
//
// The original form has been modified heavily to simplify integration
// and usage in the PEG environments.
//
/*
  Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu

  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
  (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
*/
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PZIP_
#define _PZIP_

#if defined(PEG_ZIP) || defined(PEG_UNZIP)

#if defined(PEG_ZIP)

/*
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in PzipDeflateInit.  sourceLen is the
   byte length of the source buffer. Upon entry, destLen is the total size of
   the destination buffer, which must be at least 0.1% larger than sourceLen
   plus 12 bytes. Upon exit, destLen is the actual size of the compressed
   buffer.

     PegZip returns PZIP_OK if success, PZIP_MEM_ERROR if there was not enough
   memory, PZIP_BUF_ERROR if there was not enough room in the output buffer,
   PZIP_STREAM_ERROR if the level parameter is invalid.
*/

SIGNED PegZip( UCHAR **pDest, DWORD *pDestLen, const UCHAR *pSource,
               DWORD sourceLen);

#endif  // PEG_ZIP

#if defined(PEG_UNZIP)

/*
     Decompresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be large enough to hold the
   entire uncompressed data. (The size of the uncompressed data must have
   been saved previously by the compressor and transmitted to the decompressor
   by some mechanism outside the scope of this compression library.)
   Upon exit, destLen is the actual size of the compressed buffer.
     This function can be used to decompress a whole file at once if the
   input file is mmap'ed.

     PegUnzip returns PZIP_OK if success, PZIP_MEM_ERROR if there was not
   enough memory, PZIP_BUF_ERROR if there was not enough room in the output
   buffer, or PZIP_DATA_ERROR if the input data was corrupted.
*/

SIGNED PegUnzip( UCHAR **pDest, DWORD *pDestLen, const UCHAR *pSource,
                 DWORD sourceLen );

#endif  // PEG_UNZIP

//
//****************************************************************************
// zconf.h - configuration of the zlib compression library
//****************************************************************************
//

/* Maximum value for memLevel in PzipDeflateInit */
#define PZIP_MAX_MEM_LEVEL 9

/* Maximum value for windowBits in PzipDeflateInit and PzipInflateInit.
 */
#define PZIP_MAX_WBITS 15 /* 32K LZ77 window */

//
//****************************************************************************
// zlib.h - interface of the 'zlib' general purpose compression library
//****************************************************************************
//

/*
     The 'zlib' compression library provides in-memory compression and
  decompression functions, including integrity checks of the uncompressed
  data.  This version of the library supports only one compression method
  (deflation) but other algorithms will be added later and will have the same
  stream interface.

     Compression can be done in a single step if the buffers are large
  enough (for example if an input file is mmap'ed), or can be done by
  repeated calls of the compression function.  In the latter case, the
  application must provide more input and/or consume the output
  (providing more output space) before each call.

     The library does not install any signal handler. The decoder checks
  the consistency of the compressed data, so the library should never
  crash even in case of corrupted input.
*/

/*
   The application must update mNextIn and mAvailIn when mAvailIn has
   dropped to zero. It must update mpNextOut and mAvailOut when mAvailOut
   has dropped to zero. All other fields are set by the
   compression library and must not be updated by the application.

   The fields mTotalIn and mTotalOut can be used for statistics or
   progress reports. After compression, mTotalIn holds the total size of
   the uncompressed data and may be saved for use in the decompressor
   (particularly if the decompressor wants to decompress everything in
   a single step).
*/

struct PzipInternalState;

typedef struct 
{
    const UCHAR *mpNextIn;      // next input byte 
    DWORD mAvailIn;             // number of bytes available at mNextIn 
    DWORD mTotalIn;             // total nb of input bytes read so far

    UCHAR *mpOutBuf;            // Start of output buffer 
    UCHAR* mpNextOut;           // next output byte should be put there 
    DWORD mAvailOut;            // remaining free space at mpNextOut 
    DWORD mTotalOut;            // total nb of bytes output so far 

    PzipInternalState* mpState; // not visible by applications 

    SIGNED mDataType;           // best guess about the data type: ascii or binary 
    DWORD mAdler;               // PzipAdler32 value of the uncompressed data 

} PzipStream;

/* Allowed flush values; see PzipDeflate() below for details */
#define PZIP_NO_FLUSH      0
#define PZIP_PARTIAL_FLUSH 1
#define PZIP_SYNC_FLUSH    2
#define PZIP_FULL_FLUSH    3
#define PZIP_FINISH        4

/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */
#define PZIP_OK            0
#define PZIP_STREAM_END    1
#define PZIP_NEED_DICT     2
#define PZIP_ERRNO        (-1)
#define PZIP_STREAM_ERROR (-2)
#define PZIP_DATA_ERROR   (-3)
#define PZIP_MEM_ERROR    (-4)
#define PZIP_BUF_ERROR    (-5)
#define PZIP_VERSION_ERROR (-6)

/* compression levels */
#define PZIP_NO_COMPRESSION         0
#define PZIP_BEST_SPEED             1
#define PZIP_BEST_COMPRESSION       9
#define PZIP_DEFAULT_COMPRESSION  (-1)

/* compression strategy; see PzipDeflateInit() below for details */
#define PZIP_FILTERED            1
#define PZIP_HUFFMAN_ONLY        2
#define PZIP_DEFAULT_STRATEGY    0

/* Possible values of the mDataType field */
#define PZIP_BINARY   0
#define PZIP_ASCII    1
#define PZIP_UNKNOWN  2

/* The deflate compression method (the only one supported in this version) */
#define PZIP_DEFLATED   8

#if defined(PEG_ZIP)

SIGNED PzipDeflateInit( PzipStream* pStrm, LONG level, LONG method,
                        LONG windowBits, LONG memLevel, SIGNED strategy,
                        const PEGCHAR* pVersion, DWORD streamSize );

/*
    PzipDeflate compresses as much data as possible, and stops when the input
  buffer becomes empty or the output buffer becomes full. It may introduce
  some output latency (reading input without producing any output) except when
  forced to flush.

    The detailed semantics are as follows. PzipDeflate performs one or both of
  the following actions:

  - Compress more input starting at mNextIn and update mNextIn and mAvailIn
    accordingly. If not all input can be processed (because there is not
    enough room in the output buffer), mNextIn and mAvailIn are updated and
    processing will resume at this point for the next call of PzipDeflate().

  - Provide more output starting at mpNextOut and update mpNextOut and
    mAvailOut accordingly. This action is forced if the parameter flush is non
    zero.  Forcing flush frequently degrades the compression ratio, so this
    parameter should be set only when necessary (in interactive applications).
    Some output may be provided even if flush is not set.

  Before the call of PzipDeflate(), the application should ensure that at
  least one of the actions is possible, by providing more input and/or
  consuming more output, and updating mAvailIn or mAvailOut accordingly;
  mAvailOut should never be zero before the call. The application can consume
  the compressed output when it wants, for example when the output buffer is
  full (mAvailOut == 0), or after each call of PzipDeflate(). If PzipDeflate
  returns PZIP_OK and with zero mAvailOut, it must be called again after
  making room in the output buffer because there might be more output pending.

    If the parameter flush is set to PZIP_SYNC_FLUSH, all pending output is
  flushed to the output buffer and the output is aligned on a byte boundary,
  so that the decompressor can get all input data available so far. (In
  particular mAvailIn is zero after the call if enough output space has been
  provided before the call.)  Flushing may degrade compression for some
  compression algorithms and so it should be used only when necessary.

    If flush is set to PZIP_FULL_FLUSH, all output is flushed as with
  PZIP_SYNC_FLUSH, and the compression state is reset so that decompression
  can restart from this point if previous compressed data has been damaged or
  if random access is desired. Using PZIP_FULL_FLUSH too often can seriously
  degrade the compression.

    If PzipDeflate returns with mAvailOut == 0, this function must be called
  again with the same value of the flush parameter and more output space
  (updated mAvailOut), until the flush is complete (PzipDeflate returns with
  non-zero mAvailOut).

    If the parameter flush is set to PZIP_FINISH, pending input is processed,
  pending output is flushed and PzipDeflate returns with PZIP_STREAM_END if
  there was enough output space; if PzipDeflate returns with PZIP_OK, this
  function must be called again with PZIP_FINISH and more output space
  (updated mAvailOut) but no more input data, until it returns with
  PZIP_STREAM_END or an error.  After PzipDeflate has returned
  PZIP_STREAM_END, the only possible operations on the stream are
  PzipDeflateReset or PzipDeflateEnd.

    PZIP_FINISH can be used immediately after PzipDeflateInit if all the
  compression is to be done in a single step. In this case, mAvailOut must be
  at least 0.1% larger than mAvailIn plus 12 bytes.  If PzipDeflate does not
  return PZIP_STREAM_END, then it must be called again as described above.

    PzipDeflate() sets pStrm->mAdler to the adler32 checksum of all input read
  so far (that is, mTotalIn bytes).

    PzipDeflate() may update mDataType if it can make a good guess about
  the input data type (PZIP_ASCII or PZIP_BINARY). In doubt, the data is
  considered binary. This field is only for information purposes and does not
  affect the compression algorithm in any manner.

    PzipDeflate() returns PZIP_OK if some progress has been made (more input
  processed or more output produced), PZIP_STREAM_END if all input has been
  consumed and all output has been produced (only when flush is set to
  PZIP_FINISH), PZIP_STREAM_ERROR if the stream state was inconsistent (for
  example if mNextIn or mpNextOut was NULL), PZIP_BUF_ERROR if no progress is
  possible (for example mAvailIn or mAvailOut was zero).
*/

SIGNED PzipDeflate( PzipStream* pStrm, SIGNED flush );

/*
     All dynamically allocated data structures for this stream are freed.
   This function discards any unprocessed input and does not flush any
   pending output.

     PzipDeflateEnd returns PZIP_OK if success, PZIP_STREAM_ERROR if the
   stream state was inconsistent, PZIP_DATA_ERROR if the stream was freed
   prematurely (some input or output was discarded).
*/

SIGNED PzipDeflateEnd( PzipStream* pStrm );

/*
     This function is equivalent to PzipDeflateEnd followed by
   PzipDeflateInit, but does not free and reallocate all the internal
   compression state.  The stream will keep the same compression level and any
   other attributes that may have been set by PzipDeflateInit.

      PzipDeflateReset returns PZIP_OK if success, or PZIP_STREAM_ERROR if the
   source stream state was inconsistent (such as mpState being NULL).
*/

SIGNED PzipDeflateReset( PzipStream* pStrm );

#endif  // PEG_ZIP

#if defined(PEG_UNZIP)

SIGNED PzipInflateInit( PzipStream* pZ, LONG windowBits,
                      const PEGCHAR* pVersion, DWORD streamSize );

/*
    PzipInflate decompresses as much data as possible, and stops when the
  input buffer becomes empty or the output buffer becomes full. It may some
  introduce some output latency (reading input without producing any output)
  except when forced to flush.

  The detailed semantics are as follows. PzipInflate performs one or both of
  the following actions:

  - Decompress more input starting at mNextIn and update mNextIn and mAvailIn
    accordingly. If not all input can be processed (because there is not
    enough room in the output buffer), mNextIn is updated and processing
    will resume at this point for the next call of PzipInflate().

  - Provide more output starting at mpNextOut and update mpNextOut and
    mAvailOut accordingly.  PzipInflate() provides as much output as possible,
    until there is no more input data or no more space in the output buffer
    (see below about the flush parameter).

  Before the call of PzipInflate(), the application should ensure that at
  least one of the actions is possible, by providing more input and/or
  consuming more output, and updating the next_* and avail_* values
  accordingly.  The application can consume the uncompressed output when it
  wants, for example when the output buffer is full (mAvailOut == 0), or after
  each call of PzipInflate(). If PzipInflate returns PZIP_OK and with zero
  mAvailOut, it must be called again after making room in the output buffer
  because there might be more output pending.

    If the parameter flush is set to PZIP_SYNC_FLUSH, PzipInflate flushes as
  much output as possible to the output buffer. The flushing behavior of
  PzipInflate is not specified for values of the flush parameter other than
  PZIP_SYNC_FLUSH and PZIP_FINISH, but the current implementation actually
  flushes as much output as possible anyway.

    PzipInflate() should normally be called until it returns PZIP_STREAM_END
  or an error. However if all decompression is to be performed in a single
  step (a single call of PzipInflate), the parameter flush should be set to
  PZIP_FINISH. In this case all pending input is processed and all pending
  output is flushed; mAvailOut must be large enough to hold all the
  uncompressed data. (The size of the uncompressed data may have been saved
  by the compressor for this purpose.) The next operation on this stream must
  be PzipInflateEnd to deallocate the decompression state. The use of
  PZIP_FINISH is never required, but can be used to inform PzipInflate that a
  faster routine may be used for the single PzipInflate() call.

     If a preset dictionary is needed at this point (see inflateSetDictionary
  below), PzipInflate sets pZ->mAdler to the adler32 checksum of the
  dictionary chosen by the compressor and returns PZIP_NEED_DICT; otherwise
  it sets pZ->mAdler to the adler32 checksum of all output produced
  so far (that is, mTotalOut bytes) and returns PZIP_OK, PZIP_STREAM_END or
  an error code as described below. At the end of the stream, PzipInflate()
  checks that its computed adler32 checksum is equal to that saved by the
  compressor and returns PZIP_STREAM_END only if the checksum is correct.

    PzipInflate() returns PZIP_OK if some progress has been made (more input
  processed or more output produced), PZIP_STREAM_END if the end of the
  compressed data has been reached and all uncompressed output has been
  produced, PZIP_NEED_DICT if a preset dictionary is needed at this point,
  PZIP_DATA_ERROR if the input data was corrupted (input stream not conforming
  to the zlib format or incorrect adler32 checksum), PZIP_STREAM_ERROR if the
  stream structure was inconsistent (for example if mNextIn or mpNextOut was
  NULL), PZIP_MEM_ERROR if there was not enough memory, PZIP_BUF_ERROR if no
  progress is possible or if there was not enough room in the output buffer
  when PZIP_FINISH is used.
*/

SIGNED PzipInflate( PzipStream* pStrm, LONG flush );

/*
     All dynamically allocated data structures for this stream are freed.
   This function discards any unprocessed input and does not flush any
   pending output.

     PzipInflateEnd returns PZIP_OK if success, PZIP_STREAM_ERROR if the
   stream state was inconsistent.
*/

SIGNED PzipInflateEnd( PzipStream* pZ );

/*
     This function is equivalent to PzipInflateEnd followed by
   PzipInflateInit, but does not free and reallocate all the internal
   decompression state.  The stream will keep attributes that may have been
   set by PzipInflateInit.

      PzipInflateReset returns PZIP_OK if success, or PZIP_STREAM_ERROR if the
   source stream state was inconsistent (such as mpState being NULL).
*/

SIGNED PzipInflateReset( PzipStream* pZ );

#endif  // PEG_UNZIP

/*
     These functions are not related to compression but are exported
   anyway because they might be useful in applications using the
   compression library.
*/

/*
     Update a running Adler-32 checksum with the bytes pBuf[0..len-1] and
   return the updated checksum. If pBuf is NULL, this function returns
   the required initial value for the checksum.
   An Adler-32 checksum is almost as reliable as a CRC32 but can be computed
   much faster. Usage example:

     DWORD adler = PzipAdler32(0L, NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       adler = PzipAdler32(adler, buffer, length);
     }
     if (adler != original_adler) error();
*/

DWORD PzipAdler32( DWORD adler, const UCHAR* pBuf, DWORD len );

/*
     Update a running crc with the bytes pBuf[0..len-1] and return the updated
   crc. If pBuf is NULL, this function returns the required initial value
   for the crc. Pre- and post-conditioning (one's complement) is performed
   within this function so it shouldn't be done by the application.
   Usage example:

     DWORD crc = PzipCrc32(0L, NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       crc = PzipCrc32(crc, buffer, length);
     }
     if (crc != original_crc) error();
*/

DWORD PzipCrc32( DWORD crc, const UCHAR* pBuf, WORD len );

//
//****************************************************************************
// zutil.h - internal interface and configuration of the compression library
//****************************************************************************
//

/* To be used only when the state is known to be valid */

/* common constants */

/* default windowBits for decompression. PZIP_MAX_WBITS is for compression
   only */
#define PZIP_DEF_WBITS PZIP_MAX_WBITS

/* default memLevel */
#if PZIP_MAX_MEM_LEVEL >= 8
#  define PZIP_DEF_MEM_LEVEL 8
#else
#  define PZIP_DEF_MEM_LEVEL PZIP_MAX_MEM_LEVEL
#endif

/* The three kinds of block type */
#define PZIP_STORED_BLOCK 0
#define PZIP_STATIC_TREES 1
#define PZIP_DYN_TREES 2

/* The minimum and maximum match lengths */
#define PZIP_MIN_MATCH 3
#define PZIP_MAX_MATCH 258

#define PZIP_PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

/* functions */

typedef DWORD (*PzipCheckFunc)( DWORD, const UCHAR*, DWORD );

//
//****************************************************************************
// infblock.h - header to use infblock.c
//****************************************************************************
//

#if defined(PEG_UNZIP)

struct PzipInflateBlocksState;

// c: check function
// w: window size
PzipInflateBlocksState* PzipInflateBlocksNew(
   PzipStream* pZ, PzipCheckFunc c, DWORD w );

// (third parameter): initial return code
SIGNED PzipInflateBlocks( PzipInflateBlocksState*, PzipStream*, SIGNED );

// (third parameter): check value on output
void PzipInflateBlocksReset( PzipInflateBlocksState*, PzipStream*, DWORD* );

LONG PzipInflateBlocksFree( PzipInflateBlocksState*, PzipStream* );

#endif  // PEG_UNZIP

//
//****************************************************************************
// inftrees.h - header to use inftrees.c
//****************************************************************************
//

#if defined(PEG_UNZIP)

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model). */

typedef struct
{
    union
    {
        struct
        {
            UCHAR mExop;        /* number of extra bits or operation */
            UCHAR mBits;        /* number of bits in this code or subcode */
        } mWhat;
    } mWord;

    DWORD mBase;            /* literal, length base, distance base,
                              or table offset */
} PzipInflateHuft;

/* Maximum size of dynamic tree.  The maximum found in a long but non-
   exhaustive search was 1004 huft structures (850 for length/literals
   and 154 for distances, the latter actually the result of an
   exhaustive search).  The actual maximum is not known, but the
   value below is more than safe. */

#define MANY 1440

// (first parameter): 19 code lengths
// (second parameter): bits tree desired/actual depth
// (third parameter): bits tree result
// (fourth parameter): space for trees
// (fifth parameter): for messages
SIGNED PzipInflateTreesBits( DWORD*, DWORD*, PzipInflateHuft**,
                           PzipInflateHuft*, PzipStream* );

// (first parameter): number of literal/length codes
// (second parameter): number of distance codes
// (third parameter): that many (total) code lengths
// (fourth parameter): literal desired/actual bit depth
// (fifth parameter): distance desired/actual bit depth
// (sixth parameter): literal/length tree result
// (seventh parameter): distance tree result
// (eighth parameter): space for trees
// (ninth parameter): for messages
SIGNED PzipInflateTreesDynamic( DWORD, DWORD, DWORD*, DWORD*, DWORD*,
                              PzipInflateHuft**, PzipInflateHuft**,
                              PzipInflateHuft*, PzipStream* );

// (first parameter): literal desired/actual bit depth
// (second parameter): distance desired/actual bit depth
// (third parameter): literal/length tree result
// (fourth parameter): distance tree result
// (fifth parameter): for memory allocation
SIGNED PzipInflateTreesFixed( DWORD*, DWORD*, PzipInflateHuft**,
                            PzipInflateHuft**, PzipStream* );

#endif  // PEG_UNZIP

//
//****************************************************************************
// infcodes.h - header to use infcodes.c
//****************************************************************************
//

#if defined(PEG_UNZIP)

struct InflateCodesState;

InflateCodesState* PzipInflateCodesNew(
   DWORD, DWORD, PzipInflateHuft*, PzipInflateHuft*, PzipStream* );

SIGNED PzipInflateCodes( PzipInflateBlocksState*, PzipStream*, SIGNED );

void PzipInflateCodesFree( InflateCodesState*, PzipStream* );

#endif  // PEG_UNZIP

//
//****************************************************************************
// infutil.h - types and macros common to blocks and codes
//****************************************************************************
//

#if defined(PEG_UNZIP)

typedef enum
{
    PZIP_TYPE,     /* get type bits (3, including end bit) */
    PZIP_LENS,     /* get lengths for stored */
    PZIP_STORED,   /* processing stored block */
    PZIP_TABLE,    /* get table lengths */
    PZIP_BTREE,    /* get bit lengths tree for a dynamic block */
    PZIP_DTREE,    /* get length, distance trees for a dynamic block */
    PZIP_CODES,    /* processing fixed or dynamic block */
    PZIP_DRY,      /* output remaining window bytes */
    PZIP_DONE,     /* finished last block, done */
    PZIP_BAD       /* got a data error--stuck here */
} PzipInflateBlockMode;

/* inflate blocks semi-private state */
struct PzipInflateBlocksState
{
    /* mode */
    PzipInflateBlockMode mMode;     /* current inflate_block mode */

    /* mode dependent information */
    union
    {
        DWORD mLeft;          /* if PZIP_STORED, bytes left to copy */

        struct
        {
            WORD mTable;               /* table lengths (14 bits) */
            WORD mIndex;               /* index into mpBlens (or border) */
            DWORD* mpBlens;             /* bit lengths of codes */
            DWORD mBb;                  /* bit length tree depth */
            PzipInflateHuft* mpTb;         /* bit length decoding tree */
        } mTrees;            /* if PZIP_DTREE, decoding info for trees */

        struct
        {
            InflateCodesState* mpCodes;
        } mDecode;           /* if PZIP_CODES, current state */
    } mSub;                /* submode */

    WORD mLast;            /* true if this block is the last block */

    /* mode independent information */
    DWORD mBitk;            /* bits in bit buffer */
    DWORD mBitb;           /* bit buffer */
    PzipInflateHuft* mpHufts;  /* single malloc for tree space */
    UCHAR* mpWindow;        /* sliding window */
    UCHAR* mpEnd;           /* one byte after sliding window */
    UCHAR* mpRead;          /* window read pointer */
    UCHAR* mpWrite;         /* window write pointer */
    PzipCheckFunc mCheckFn;   /* check function */
    DWORD mCheck;          /* check on output */
};

/* defines for PzipInflate input/output */
/*   update pointers and return */

#define PZIP_UPDBITS { pS->mBitb = b; pS->mBitk = k; }

#define PZIP_UPDIN \
        { pZ->mAvailIn = n; pZ->mTotalIn += p_p - pZ->mpNextIn; \
          pZ->mpNextIn = p_p; }

#define PZIP_UPDOUT { pS->mpWrite = p_q; }

#define PZIP_UPDATE { PZIP_UPDBITS PZIP_UPDIN PZIP_UPDOUT }

#define PZIP_LEAVE { PZIP_UPDATE return PzipInflateFlush( pS, pZ, r ); }

/*   get bytes and bits */
#define PZIP_LOADIN \
        { p_p = pZ->mpNextIn; n = pZ->mAvailIn; b = pS->mBitb; \
          k = pS->mBitk; }

#define PZIP_NEEDBYTE { if (n) { r = PZIP_OK; } else { PZIP_LEAVE } }

#define PZIP_NEXTBYTE (n--, *p_p++)

#define PZIP_NEEDBITS(j) \
        { while (k < (j) ) \
          { PZIP_NEEDBYTE; b |= ((DWORD) PZIP_NEXTBYTE) << k; k += 8; } }

#define PZIP_DUMPBITS(j) { b >>= (j); k -= (j); }

/*   output bytes */
#define PZIP_WAVAIL \
        (WORD) (p_q < pS->mpRead ? pS->mpRead - p_q - 1 : pS->mpEnd - p_q)

#define PZIP_LOADOUT { p_q = pS->mpWrite; m = (WORD) PZIP_WAVAIL; }
#define PZIP_LOADOUT_2 { p_q = pS->mpWrite; }

#define PZIP_WRAP \
        { if (p_q == pS->mpEnd && pS->mpRead != pS->mpWindow) \
          { p_q = pS->mpWindow; m = (WORD) PZIP_WAVAIL; } }

#define PZIP_FLUSH \
        { PZIP_UPDOUT r = PzipInflateFlush( pS, pZ, r); PZIP_LOADOUT }
#define PZIP_FLUSH_2 \
        { PZIP_UPDOUT r = PzipInflateFlush( pS, pZ, r); PZIP_LOADOUT_2 }

#define PZIP_NEEDOUT \
        { \
            if (m == 0) \
            { \
                PZIP_WRAP \
                if (m == 0) \
                { \
                    PZIP_FLUSH \
                    PZIP_WRAP \
                    if (m == 0) PZIP_LEAVE \
                } \
            } \
            r = PZIP_OK; \
        }

#define PZIP_OUTBYTE(a) { *p_q++ = (UCHAR) (a); m--; }

/*   load local pointers */
#define PZIP_LOAD { PZIP_LOADIN PZIP_LOADOUT }

/* masks for lower bits (size given to avoid silly warnings with Visual C++)
 */
extern WORD PzipInflateMask[17];

/* copy as much as possible from the sliding window to the output area */
SIGNED PzipInflateFlush( PzipInflateBlocksState*, PzipStream*, SIGNED);

#endif  // PEG_UNZIP

//
//****************************************************************************
// inffast.h - header to use inffast.c
//****************************************************************************
//

#if defined(PEG_UNZIP)

SIGNED PzipInflateFast( WORD, WORD, PzipInflateHuft*, PzipInflateHuft*,
                      PzipInflateBlocksState*, PzipStream* );

#endif  // PEG_UNZIP

//
//****************************************************************************
// deflate.h - internal compression state
//****************************************************************************
//

#if defined(PEG_ZIP)

/* number of length codes, not counting the special END_BLOCK code */
#define PZIP_LENGTH_CODES 29

/* number of literal bytes 0..255 */
#define PZIP_LITERALS 256

/* number of Literal or Length codes, including the END_BLOCK code */
#define PZIP_L_CODES (PZIP_LITERALS + 1 + PZIP_LENGTH_CODES)

/* number of distance codes */
#define PZIP_D_CODES 30

/* number of codes used to transfer the bit lengths */
#define PZIP_BL_CODES 19

/* maximum heap size */
#define PZIP_HEAP_SIZE (2 * PZIP_L_CODES + 1)

/* All codes must not exceed PZIP_MAX_BITS bits */
#define PZIP_MAX_BITS 15

/* Stream status */
#define PZIP_INIT_STATE 42
#define PZIP_BUSY_STATE 113
#define PZIP_FINISH_STATE 666

/* Data structure describing a single value and its code string. */
typedef struct
{
    union
    {
        WORD mFreq;       /* frequency count */
        WORD mCode;       /* bit string */
    } mFc;

    union
    {
        WORD mDad;        /* father node in Huffman tree */
        WORD mLen;        /* length of bit string */
    } mDl;
} PzipCtData;

#define PzipFreq mFc.mFreq
#define PzipCode mFc.mCode
#define PzipDad mDl.mDad
#define PzipLen mDl.mLen

typedef struct PzipStaticTreeDescStruct PzipStaticTreeDesc;

typedef struct
{
    PzipCtData* mpDynTree;           /* the dynamic tree */
    SIGNED mMaxCode;            /* largest code with non zero frequency */
    PzipStaticTreeDesc* mpStatDesc; /* the corresponding static tree */
} PzipTreeDesc;

typedef struct
{
    PzipStream* mpStrm;      /* pointer back to this zlib stream */
    SIGNED mStatus;        /* as the name implies */
    UCHAR* mpPendingBuf;  /* output still pending */
    DWORD mPendingBufSize; /* size of mpPendingBuf */
    UCHAR* mpPendingOut;  /* next pending byte to output to the stream */
    DWORD mPending;       /* nb of bytes in the pending buffer */
    SIGNED mNoHeader;      /* suppress zlib header and adler32 */
    UCHAR mDataType;     /* UNKNOWN, BINARY or ASCII */
    UCHAR mMethod;        /* PZIP_STORED (for zip only) or DEFLATED */
    SIGNED mLastFlush;/* value of flush param for previous PzipDeflate call */

    /* used by deflate.c: */

    DWORD mWsize;        /* LZ77 window size (32K by default) */
    LONG mWbits;        /* log2(mWsize)  (8..16) */
    DWORD mWmask;        /* mWsize - 1 */

    /* Sliding window. Input bytes are read into the second half of the
     * window, and move to the first half later to keep a dictionary of at
     * least wSize bytes. With this organization, matches are limited to a
     * distance of wSize - PZIP_MAX_MATCH bytes, but this ensures that IO is
     * always performed with a length multiple of the block size. Also, it
     * limits the window size to 64K, which is quite useful on PEGDOS.
     * To do: use the user input buffer as sliding window.
     */
    UCHAR* mpWindow;

    /* Actual size of window: 2*wSize, except when the user input buffer
     * is directly used as sliding window.
     */
    DWORD mWindowSize;

    /* Link to older string with same hash index. To limit the size of this
     * array to 64K, this link is maintained only for the last 32K strings.
     * An index in this array is thus a window index modulo 32K.
     */
    WORD* mpPrev;

    WORD* mpHead; /* Heads of the hash chains or NIL. */

    WORD mInsh;          /* hash index of string to be inserted */
    WORD mHashSize;      /* number of elements in hash table */
    WORD mHashBits;      /* log2(mHashSize) */
    WORD mHashMask;      /* mHashSize - 1 */

    /* Number of bits by which mInsh must be shifted at each input
     * step. It must be such that after PZIP_MIN_MATCH steps, the oldest
     * byte no longer takes part in the hash key, that is:
     *   mHashShift * PZIP_MIN_MATCH >= mHashBits
     */
    WORD mHashShift;

    /* Window position at the beginning of the current output block. Gets
     * negative when the window is moved backwards.
     */
    LONG mBlockStart;

    DWORD mMatchLength;           /* length of best match */
    WORD mPrevMatch;             /* previous match */
    SIGNED mMatchAvailable;         /* set if previous match exists */
    DWORD mStrStart;               /* start of string to insert */
    WORD mMatchStart;            /* start of matching string */
    DWORD mLookahead;              /* number of valid bytes ahead in window */

    /* Length of the best match at previous step. Matches not greater than
     * this are discarded. This is used in the lazy match evaluation.
     */
    DWORD mPrevLength;

    /* To speed up deflation, hash chains are never searched beyond this
     * length.  A higher limit improves compression ratio but degrades the
     * speed.
     */
    WORD mMaxChainLength;

    /* Attempt to find a better match only when the current match is strictly
     * smaller than this value. This mechanism is used only for compression
     * levels >= 4.
     */
    DWORD mMaxLazyMatch;

    /* Insert new strings in the hash table only if the match length is not
     * greater than this length. This saves time but degrades compression.
     * PzipMaxInsertLength is used only for compression levels <= 3.
     */
#define PzipMaxInsertLength mMaxLazyMatch

    SIGNED mLevel;    /* compression level (1..9) */
    SIGNED mStrategy; /* favor or force Huffman coding*/

    /* Use a faster search when the previous match is longer than this */
    DWORD mGoodMatch;

    SIGNED mNiceMatch; /* Stop searching when current match exceeds this */

    /* used by trees.c: */

    PzipCtData mDynLtree[PZIP_HEAP_SIZE];   /* literal and length tree */
    PzipCtData mDynDtree[2 * PZIP_D_CODES + 1]; /* distance tree */
    PzipCtData mBlTree[2 * PZIP_BL_CODES + 1];// Huffman tree for bit lengths

    PzipTreeDesc mLdesc;               /* desc. for literal tree */
    PzipTreeDesc mDdesc;               /* desc. for distance tree */
    PzipTreeDesc mBlDesc;              /* desc. for bit length tree */

    /* number of codes at each bit length for an optimal tree */
    WORD mBlCount[PZIP_MAX_BITS + 1];

    /* The sons of mHeap[n] are mHeap[2 * n] and mHeap[2 * n + 1].  mHeap[0]
     * is not used.  The same heap array is used to build all trees.
     */
    SIGNED mHeap[2 * PZIP_L_CODES + 1];// heap used to build the Huffman trees
    SIGNED mHeapLen;               /* number of elements in the heap */
    SIGNED mHeapMax;               /* element of largest frequency */

    /* Depth of each subtree used as tie breaker for trees of equal frequency
     */
    UCHAR mDepth[2 * PZIP_L_CODES + 1];

    UCHAR* mpLbuf;          /* buffer for literals or lengths */

    /* Size of match buffer for literals/lengths.  There are 4 reasons for
     * limiting mLitBufsize to 64K:
     *   - frequencies can be kept in 16 bit counters
     *   - if compression is not successful for the first block, all input
     *     data is still in the window so we can still emit a stored block
     *     even when input comes from standard input.  (This can also be done
     *     for all blocks if mLitBufsize is not greater than 32K.)
     *   - if compression is not successful for a file smaller than 64K, we
     *     can even emit a stored file instead of a stored block (saving 5
     *     bytes).  This is applicable only for zip (not gzip or zlib).
     *   - creating new Huffman trees less frequently may not provide fast
     *     adaptation to changes in the input data statistics. (Take for
     *     example a binary file with poorly compressible code followed by
     *     a highly compressible string table.) Smaller buffer sizes give
     *     fast adaptation but have of course the overhead of transmitting
     *     trees more frequently.
     *   - I can't count above 4
     */
    WORD mLitBufsize;

    WORD mLastLit;      /* running index in mpLbuf */

    /* Buffer for distances. To simplify the code, mpDbuf and mpLbuf have
     * the same number of elements. To use different lengths, an extra flag
     * array would be necessary.
     */
    WORD* mpDbuf;

    DWORD mOptLen;        /* bit length of current block with optimal trees */
    DWORD mStaticLen;     /* bit length of current block with static trees */
    WORD mMatches;       /* number of string matches in current block */
    SIGNED mLastEobLen;   /* bit length of EOB code for last block */

    /* Output buffer. bits are inserted starting at the bottom (least
     * significant bits).
     */
    WORD mBiBuf;

    /* Number of valid bits in mBiBuf.  All bits above the last valid bit
     * are always zero.
     */
    LONG mBiValid;
} PzipDeflateInternalState;

/* Output a byte on the stream.
 * IN assertion: there is enough room in mpPendingBuf.
 */
#define PzipPutByte(s, c) { s->mpPendingBuf[s->mPending++] = (c); }

/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the PZIP_MIN_MATCH + 1.
 */
#define PZIP_MIN_LOOKAHEAD (PZIP_MAX_MATCH + PZIP_MIN_MATCH + 1)

/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to PZIP_MAX_DIST instead of WSIZE.
 */
#define PZIP_MAX_DIST(s) ((s)->mWsize - PZIP_MIN_LOOKAHEAD)

/* in trees.c */

void PzipTrInit( PzipDeflateInternalState* pS );

LONG PzipTrTally( PzipDeflateInternalState* pS, DWORD dist, DWORD lc );

void PzipTrFlushBlock( PzipDeflateInternalState* pS, PEGCHAR* pBuf,
                       DWORD storedLen, LONG eof );

void PzipTrAlign( PzipDeflateInternalState* pS );

void PzipTrStoredBlock( PzipDeflateInternalState* pS, PEGCHAR* pBuf,
                        DWORD storedLen, LONG eof );

/* Mapping from a distance to a distance code. dist is the distance - 1 and
 * must not have side effects. gPzipDistCode[256] and gPzipDistCode[257] are
 * never used.
 */
#define PzipDcode(dist) \
    ((dist) < 256 ? gPzipDistCode[dist] : gPzipDistCode[256 + ((dist) >> 7)])

#define PzipTrTallyLit(s, c, flush) flush = PzipTrTally( s, 0, c )
#define PzipTrTallyLit_2(s, c, flush) PzipTrTally( s, 0, c )

#define PzipTrTallyDist(s, distance, length, flush) \
        flush = PzipTrTally( s, distance, length )

#endif  // PEG_ZIP

#endif  // PEG_ZIP || PEG_UNZIP

#endif  // _PZIP_

