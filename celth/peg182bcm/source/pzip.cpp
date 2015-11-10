/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pzip.cpp - runtime compress and decompress utilities.
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

#include "peg.hpp"

#if defined(PEG_ZIP) || defined(PEG_UNZIP)


#if defined(PEG_ZIP)
SIGNED PegZip( UCHAR** pDest, DWORD* pDestLen, const UCHAR *pSource,
               DWORD sourceLen)
{
    PzipStream stream;
    SIGNED err;

    stream.mpNextIn = pSource;
    stream.mAvailIn = sourceLen;
    stream.mpOutBuf = NULL;
    stream.mpNextOut = NULL;
    stream.mAvailOut = 0;

    err = PzipDeflateInit(
       &stream, PZIP_DEFAULT_COMPRESSION, PZIP_DEFLATED, PZIP_MAX_WBITS,
       PZIP_DEF_MEM_LEVEL, PZIP_DEFAULT_STRATEGY, NULL,
       sizeof( PzipStream ) );

    if (err != PZIP_OK)
    {
        return err;
    }

    err = PzipDeflate( &stream, PZIP_FINISH );
    if (err != PZIP_STREAM_END)
    {
        if (err == PZIP_OK)
        {
            err = PZIP_BUF_ERROR;
        }

        PzipDeflateEnd( &stream );

        return err;
    }

    *pDestLen = stream.mTotalOut;
    *pDest = stream.mpOutBuf;

    return (SIGNED) PzipDeflateEnd( &stream );
}
#endif

#if defined(PEG_UNZIP)
SIGNED PegUnzip( UCHAR** pDest, DWORD* pDestLen, const UCHAR* pSource,
                 DWORD sourceLen )
{
    PzipStream stream;
    SIGNED err;

    stream.mpOutBuf = NULL;
    stream.mpNextIn = pSource;
    stream.mAvailIn = sourceLen;
    stream.mpNextOut = NULL;
    stream.mAvailOut = 0;

    err = PzipInflateInit( &stream, PZIP_DEF_WBITS, NULL,
                           sizeof( PzipStream ) );
    if (err != PZIP_OK)
    {
        return err;
    }

    err = PzipInflate( &stream, PZIP_FINISH );
    if (err != PZIP_STREAM_END)
    {
        if (err == PZIP_OK)
        {
            err = PZIP_BUF_ERROR;
        }

        PzipInflateEnd( &stream );

        return err;
    }

    *pDestLen = stream.mTotalOut;
    *pDest = stream.mpOutBuf;

    return PzipInflateEnd( &stream );
}
#endif

//
//****************************************************************************
// adler32.c - compute the Adler-32 checksum of a data stream
//****************************************************************************
//

#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {s_1 += buf[i]; s_2 += s_1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

DWORD PzipAdler32( DWORD adler, const UCHAR* pBuf, DWORD len )
{
    DWORD s_1 = adler & 0xFFFF;
    DWORD s_2 = (adler >> 16) & 0xFFFF;
    LONG k;

    if (!pBuf)
    {
        return 1L;
    }

    while (len > 0)
    {
        if (len < NMAX)
        {
            k = len;
        }
        else
        {
            k = NMAX;
        }
        len -= k;
        while (k >= 16)
        {
            DO16(pBuf);
	        pBuf += 16;
            k -= 16;
        }
        if (k != 0)
        {
            do
            {
                s_1 += *pBuf++;
	            s_2 += s_1;
            } while (--k);
        }
        s_1 %= BASE;
        s_2 %= BASE;
    }

    return (s_2 << 16) | s_1;
}

//
//****************************************************************************
// crc32.c - compute the CRC-32 of a data stream
//****************************************************************************
//

/*
 * Table of CRC-32's of all single-byte values
 */
const DWORD gPzipCrcTable[256] =
{
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

#undef DO1
#define DO1(buf) \
        crc = gPzipCrcTable[((LONG)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#undef DO2
#define DO2(buf)  DO1(buf); DO1(buf);
#undef DO4
#define DO4(buf)  DO2(buf); DO2(buf);
#undef DO8
#define DO8(buf)  DO4(buf); DO4(buf);

DWORD PzipCrc32( DWORD crc, const UCHAR* pBuf, WORD len )
{
    if (!pBuf)
    {
        return 0L;
    }
    crc = crc ^ 0xFFFFFFFFL;
    while (len >= 8)
    {
        DO8(pBuf);
        len -= 8;
    }
    if (len)
    {
        do
        {
            DO1(pBuf);
        } while (--len);
    }
    return crc ^ 0xFFFFFFFFL;
}

//
//****************************************************************************
// deflate.c - compress data using the deflation algorithm
//****************************************************************************
//

#if defined(PEG_ZIP)

/*
 *  ALGORITHM
 *
 *      The "deflation" process depends on being able to identify portions
 *      of the input text which are identical to earlier input (within a
 *      sliding window trailing behind the input currently being processed).
 *
 *      The most straightforward technique turns out to be the fastest for
 *      most input files: try all possible matches and select the longest.
 *      The key feature of this algorithm is that insertions into the string
 *      dictionary are very simple and thus fast, and deletions are avoided
 *      completely. Insertions are performed at each input character, whereas
 *      string matches are performed only when the previous match ends. So it
 *      is preferable to spend more time in matches to allow very fast string
 *      insertions and avoid deletions. The matching algorithm for small
 *      strings is inspired from that of Rabin & Karp. A brute force approach
 *      is used to find longer strings when a small match has been found.
 *      A similar algorithm is used in comic (by Jan-Mark Wams) and freeze
 *      (by Leonid Broukhis).
 *         A previous version of this file used a more sophisticated algorithm
 *      (by Fiala and Greene) which is guaranteed to run in linear amortized
 *      time, but has a larger average cost, uses more memory and is patented.
 *      However the F&G algorithm may be faster for some highly redundant
 *      files if the parameter mMaxChainLength (described below) is too
 *      large.
 */

/*
 *  Function prototypes.
 */

typedef enum
{
    NEED_MORE,      /* block not completed, need more input or more output */
    BLOCK_DONE,     /* block flush performed */
    FINISH_STARTED,/* finish started, need only more output at next deflate */
    FINISH_DONE     /* finish done, accept no more input or output */
} BlockState;

/* Compression function. Returns the block state after the call. */
typedef BlockState (*CompressFunc)( PzipDeflateInternalState*, LONG );

void fill_window( PzipDeflateInternalState* pS );
BlockState deflate_stored( PzipDeflateInternalState* pS, LONG flush );
BlockState deflate_fast( PzipDeflateInternalState* pS, LONG flush );
BlockState deflate_slow( PzipDeflateInternalState* pS, LONG flush );
void lm_init( PzipDeflateInternalState* pS );
void put_short_msb( PzipDeflateInternalState* pS, WORD b );
void flush_pending( PzipStream* pStrm );
DWORD read_buf( PzipStream* pStrm, UCHAR* pBuf, DWORD size );
DWORD longest_match( PzipDeflateInternalState* pS, WORD cur_match );
//LONG read_buf( PzipStream* pStrm, UCHAR* pBuf, WORD size );
//WORD longest_match( PzipDeflateInternalState* pS, WORD cur_match );

/*
 * Local data
 */

/* Tail of hash chains */
#define NIL 0

/* Matches of length 3 are discarded if their distance exceeds TOO_FAR */
#ifndef TOO_FAR
#  define TOO_FAR 4096
#endif

/* Values for mMaxLazyMatch, mGoodMatch and mMaxChainLength, depending on
 * the desired pack level (0..9). The values given below have been tuned to
 * exclude worst case performance for pathological files. Better values may be
 * found for specific files.
 */
typedef struct
{
   WORD mGoodLength; /* reduce lazy search above this match length */
   WORD mMaxLazy;    /* do not perform lazy search above this match length */
   WORD mNiceLength; /* quit search above this match length */
   WORD mMaxChain;
   CompressFunc mFunc;
} Config;

const Config gPzipConfigurationTable[10] =
{
/*      good lazy nice chain */
/* 0 */ {0,    0,  0,    0, deflate_stored},  /* store only */
/* 1 */ {4,    4,  8,    4, deflate_fast}, // maximum speed, no lazy matches
/* 2 */ {4,    5, 16,    8, deflate_fast},
/* 3 */ {4,    6, 32,   32, deflate_fast},

/* 4 */ {4,    4, 16,   16, deflate_slow},  /* lazy matches */
/* 5 */ {8,   16, 32,   32, deflate_slow},
/* 6 */ {8,   16, 128, 128, deflate_slow},
/* 7 */ {8,   32, 128, 256, deflate_slow},
/* 8 */ {32, 128, 258, 1024, deflate_slow},
/* 9 */ {32, 258, 258, 4096, deflate_slow} /* maximum compression */
};

/* Note: the PzipDeflate() code requires mMaxLazy >= PZIP_MIN_MATCH and
 * mMaxChain >= 4.  For deflate_fast() (levels <= 3) good is ignored and lazy
 * has a different meaning.
 */

/*
 * Update a hash value with the given input byte
 * IN  assertion: all calls to to UPDATE_HASH are made with consecutive
 *    input characters, so that a running hash key can be computed from the
 *    previous key instead of complete recalculation each time.
 */
#define UPDATE_HASH(s,h,c) (h = (WORD) (((h)<<s->mHashShift) ^ (c)) & s->mHashMask)

/*
 * Insert string str in the dictionary and set match_head to the previous head
 * of the hash chain (the most recent string with same hash key). Return
 * the previous length of the hash chain.
 * IN  assertion: all calls to to INSERT_STRING are made with consecutive
 *    input characters and the first PZIP_MIN_MATCH bytes of str are valid
 *    (except for the last PZIP_MIN_MATCH - 1 bytes of the input file).
 */
#define INSERT_STRING(s, str, match_head) \
   (UPDATE_HASH( s, s->mInsh, s->mpWindow[(str) + (PZIP_MIN_MATCH - 1)]), \
    s->mpPrev[(str) & s->mWmask] = match_head = s->mpHead[s->mInsh], \
    s->mpHead[s->mInsh] = (WORD)(str))

/*
 * Initialize the hash table (avoiding 64K overflow for 16 bit systems).
 * mpPrev[] will be initialized on the fly.
 */
#define CLEAR_HASH(s) \
    s->mpHead[s->mHashSize-1] = NIL; \
    memset( (UCHAR*) s->mpHead, 0, \
            (WORD) (s->mHashSize - 1) * sizeof( *s->mpHead ) );

SIGNED PzipDeflateInit( PzipStream* pStrm, LONG level, LONG method,
                        LONG windowBits, LONG memLevel, SIGNED strategy,
                        const PEGCHAR* pVersion, DWORD streamSize )
{
    PzipDeflateInternalState* p_s;
    SIGNED no_header = 0;

    /* We overlay mpPendingBuf and mpDbuf + mpLbuf. This works since the
     * average output size for (length,distance) codes is <= 24 bits.
     */
    WORD* p_overlay;

    if (streamSize != sizeof( PzipStream ))
    {
	    return PZIP_VERSION_ERROR;
    }

    if (!pStrm)
    {
        return PZIP_STREAM_ERROR;
    }

    if (level == PZIP_DEFAULT_COMPRESSION)
    {
        level = 6;
    }

    if (windowBits < 0) /* undocumented feature: suppress zlib header */
    {
        no_header = 1;
        windowBits = -windowBits;
    }

    if (memLevel < 1 || memLevel > PZIP_MAX_MEM_LEVEL ||
        method != PZIP_DEFLATED || windowBits < 8 || windowBits > 15 ||
        level < 0 || level > 9 || strategy < 0 ||
        strategy > PZIP_HUFFMAN_ONLY)
    {
        return PZIP_STREAM_ERROR;
    }

    p_s = new PzipDeflateInternalState;

    if (!p_s)
    {
        return PZIP_MEM_ERROR;
    }

    pStrm->mpState = (PzipInternalState*) p_s;
    p_s->mpStrm = pStrm;

    p_s->mNoHeader = no_header;
    p_s->mWbits = windowBits;
    p_s->mWsize = (WORD) (1 << p_s->mWbits);
    p_s->mWmask = (WORD) (p_s->mWsize - 1);

    p_s->mHashBits = (WORD) (memLevel + 7);
    p_s->mHashSize = (WORD) (1 << p_s->mHashBits);
    p_s->mHashMask = (WORD) (p_s->mHashSize - 1);
    p_s->mHashShift = (WORD) ((p_s->mHashBits + PZIP_MIN_MATCH - 1) / PZIP_MIN_MATCH);

    p_s->mpWindow = new UCHAR[2 * p_s->mWsize];
    p_s->mpPrev = new WORD[p_s->mWsize];
    p_s->mpHead = new WORD[p_s->mHashSize];

    p_s->mLitBufsize = (WORD) (1 << (memLevel + 6)); /* 16K elements by default */

    // sizeof( DWORD ) == sizeof( WORD ) + 2
    p_overlay = (WORD*) new DWORD[p_s->mLitBufsize];

    p_s->mpPendingBuf = (UCHAR*) p_overlay;
    p_s->mPendingBufSize = (DWORD) p_s->mLitBufsize * (sizeof( WORD ) + 2L);

    if (!p_s->mpWindow || !p_s->mpPrev || !p_s->mpHead || !p_s->mpPendingBuf)
    {
        PzipDeflateEnd( pStrm );
        return PZIP_MEM_ERROR;
    }

    p_s->mpDbuf = p_overlay + p_s->mLitBufsize / sizeof( WORD );
    p_s->mpLbuf = p_s->mpPendingBuf + (1 + sizeof( WORD )) * p_s->mLitBufsize;

    p_s->mLevel = (SIGNED) level;
    p_s->mStrategy = strategy;
    p_s->mMethod = (UCHAR) method;

    return PzipDeflateReset( pStrm );
}

SIGNED PzipDeflateReset( PzipStream* pStrm )
{
    PzipDeflateInternalState* p_s;

    if (!pStrm || !pStrm->mpState)
    {
        return PZIP_STREAM_ERROR;
    }

    pStrm->mTotalIn = pStrm->mTotalOut = 0;
    pStrm->mDataType = PZIP_UNKNOWN;

    p_s = (PzipDeflateInternalState*) pStrm->mpState;
    p_s->mPending = 0;
    p_s->mpPendingOut = p_s->mpPendingBuf;

    if (p_s->mNoHeader < 0)
    {
        /* was set to -1 by PzipDeflate(..., PZIP_FINISH); */
        p_s->mNoHeader = 0;
    }

    if (p_s->mNoHeader)
    {
        p_s->mStatus = PZIP_BUSY_STATE;
    }
    else
    {
        p_s->mStatus = PZIP_INIT_STATE;
    }

    pStrm->mAdler = 1;
    p_s->mLastFlush = PZIP_NO_FLUSH;

    PzipTrInit( p_s );
    lm_init( p_s );

    return PZIP_OK;
}

/*
 * Put a short in the pending buffer. The 16-bit value is put in MSB order.
 * IN assertion: the stream state is correct and there is enough room in
 * mpPendingBuf.
 */
void put_short_msb( PzipDeflateInternalState* pS, WORD b )
{
    PzipPutByte( pS, (UCHAR)(b >> 8) );
    PzipPutByte( pS, (UCHAR)(b & 0xFF) );
}

/*
 * Flush as much pending output as possible. All PzipDeflate() output goes
 * through this function so some applications may wish to modify it
 * to avoid allocating a large pStrm->mpNextOut buffer and copying into it.
 * (See also read_buf()).
 */
void flush_pending( PzipStream* pStrm )
{
#define PSTRM_STATE ((PzipDeflateInternalState*) pStrm->mpState)

    DWORD len = PSTRM_STATE->mPending;
    DWORD dNewSize;
    DWORD dTest;
    UCHAR *pOldBuf;

    if (len > pStrm->mAvailOut)
    {
        // We need more output space. Allocate it, copy output
        // up to this point into new space, and delete the old space.
        
        pOldBuf = pStrm->mpOutBuf;

        dNewSize = 1024;
        dTest = pStrm->mTotalOut >> 9;
        while(dTest)
        {
            dNewSize <<= 1;
            dTest >>= 1;
        }
        pStrm->mpOutBuf = new UCHAR[dNewSize];

        if (pStrm->mpOutBuf)
        {
            memcpy(pStrm->mpOutBuf, pOldBuf, pStrm->mTotalOut);
            pStrm->mpNextOut = pStrm->mpOutBuf + pStrm->mTotalOut;
            pStrm->mAvailOut = dNewSize - pStrm->mTotalOut;
            delete pOldBuf;
        }
        else
        {
            len = pStrm->mAvailOut;
        }
    }

    if (len == 0)
    {
        return;
    }

    memcpy( pStrm->mpNextOut, PSTRM_STATE->mpPendingOut, len );
    pStrm->mpNextOut += len;
    PSTRM_STATE->mpPendingOut += len;
    pStrm->mTotalOut += len;
    pStrm->mAvailOut -= len;
    PSTRM_STATE->mPending -= len;

    if (PSTRM_STATE->mPending == 0)
    {
        PSTRM_STATE->mpPendingOut = PSTRM_STATE->mpPendingBuf;
    }

#undef PSTRM_STATE
}

SIGNED PzipDeflate( PzipStream* pStrm, SIGNED flush )
{
#define PSTRM_STATE ((PzipDeflateInternalState*) pStrm->mpState)

    LONG old_flush; /* value of flush param for previous PzipDeflate call */
    PzipDeflateInternalState* p_s;

    if (!pStrm || !pStrm->mpState || flush > PZIP_FINISH || flush < 0)
    {
        return PZIP_STREAM_ERROR;
    }

    p_s = PSTRM_STATE;

    if ((!pStrm->mpNextIn && pStrm->mAvailIn != 0) ||
	    (p_s->mStatus == PZIP_FINISH_STATE && flush != PZIP_FINISH))
    {
        return PZIP_STREAM_ERROR;
    }

    p_s->mpStrm = pStrm; /* just in case */
    old_flush = p_s->mLastFlush;
    p_s->mLastFlush = flush;

    /* Write the zlib header */
    if (p_s->mStatus == PZIP_INIT_STATE)
    {
        WORD header = (WORD) ((PZIP_DEFLATED + ((p_s->mWbits - 8) << 4)) << 8);
        WORD level_flags = (WORD) ((p_s->mLevel - 1) >> 1);

        if (level_flags > 3)
        {
            level_flags = 3;
        }

        header |= (level_flags << 6);

	    if (p_s->mStrStart != 0)
        {
            header |= PZIP_PRESET_DICT;
        }

        header += (WORD) (31 - (header % 31));

        p_s->mStatus = PZIP_BUSY_STATE;
        put_short_msb( p_s, header );

	    /* Save the adler32 of the preset dictionary: */
	    if (p_s->mStrStart != 0)
        {
	        put_short_msb( p_s, (WORD) (pStrm->mAdler >> 16) );
	        put_short_msb( p_s, (WORD) (pStrm->mAdler & 0xFFFF) );
	    }

	    pStrm->mAdler = 1L;
    }

    /* Flush as much pending output as possible */
    if (p_s->mPending != 0)
    {
        flush_pending( pStrm );

//        if (pStrm->mAvailOut == 0)
//        {
	        /* Since mAvailOut is 0, PzipDeflate will be called again with
	         * more output space, but possibly with both pending and
	         * mAvailIn equal to zero. There won't be anything to do,
	         * but this is not an error situation so make sure we
	         * return OK instead of BUF_ERROR at next call of PzipDeflate:
             */
//	        p_s->mLastFlush = -1;
//	        return PZIP_OK;
//	    }

        /* Make sure there is something to do and avoid duplicate consecutive
         * flushes. For repeated and useless calls with PZIP_FINISH, we keep
         * returning PZIP_STREAM_END instead of Z_BUFF_ERROR.
         */
    }
    else if (pStrm->mAvailIn == 0 && flush <= old_flush &&
	         flush != PZIP_FINISH)
    {
        return PZIP_BUF_ERROR;
    }

    /* User must not provide more input after the first FINISH: */
    if (p_s->mStatus == PZIP_FINISH_STATE && pStrm->mAvailIn != 0)
    {
        return PZIP_BUF_ERROR;
    }

    /* Start a new block or continue the current one.
     */
    if (pStrm->mAvailIn != 0 || p_s->mLookahead != 0 ||
        (flush != PZIP_NO_FLUSH && p_s->mStatus != PZIP_FINISH_STATE))
    {
        BlockState b_state;

	    b_state = (*(gPzipConfigurationTable[p_s->mLevel].mFunc))(
           p_s, flush );

        if (b_state == FINISH_STARTED || b_state == FINISH_DONE)
        {
            p_s->mStatus = PZIP_FINISH_STATE;
        }

        if (b_state == NEED_MORE || b_state == FINISH_STARTED)
        {
	        if (pStrm->mAvailOut == 0)
            {
	            p_s->mLastFlush = -1; // avoid BUF_ERROR next call, see above
	        }

	        return PZIP_OK;

	        /* If flush != PZIP_NO_FLUSH && mAvailOut == 0, the next call
	         * of PzipDeflate should use the same flush parameter to make sure
	         * that the flush is complete. So we don't have to output an
	         * empty block here, this will be done at next call. This also
	         * ensures that for a very small output buffer, we emit at most
	         * one empty block.
	         */
	    }

        if (b_state == BLOCK_DONE)
        {
            if (flush == PZIP_PARTIAL_FLUSH)
            {
                PzipTrAlign( p_s );
            }
            else /* FULL_FLUSH or SYNC_FLUSH */
            {
                PzipTrStoredBlock( p_s, NULL, 0L, 0 );

                /* For a full flush, this empty block will be recognized
                 * as a special marker by inflate_sync().
                 */

                if (flush == PZIP_FULL_FLUSH)
                {
                    CLEAR_HASH(p_s);             /* forget history */
                }
            }

            flush_pending( pStrm );

	        if (pStrm->mAvailOut == 0)
            {
	            p_s->mLastFlush = -1; /* avoid BUF_ERROR at next call,
                                        see above */

	            return PZIP_OK;
	        }
        }
    }

    if (flush != PZIP_FINISH)
    {
        return PZIP_OK;
    }

    if (p_s->mNoHeader)
    {
        return PZIP_STREAM_END;
    }

    /* Write the zlib trailer (adler32) */
    put_short_msb( p_s, (WORD) (pStrm->mAdler >> 16) );
    put_short_msb( p_s, (WORD) (pStrm->mAdler & 0xFFFF) );
    flush_pending( pStrm );

    /* If mAvailOut is zero, the application will call PzipDeflate again
     * to flush the rest.
     */
    p_s->mNoHeader = -1; /* write the trailer only once! */

    if (p_s->mPending != 0)
    {
        return PZIP_OK;
    }

    return PZIP_STREAM_END;

#undef PSTRM_STATE
}

SIGNED PzipDeflateEnd( PzipStream* pStrm )
{
#define PSTRM_STATE ((PzipDeflateInternalState*) pStrm->mpState)

    LONG status;

    if (!pStrm || !pStrm->mpState)
    {
        return PZIP_STREAM_ERROR;
    }

    status = PSTRM_STATE->mStatus;

    if (status != PZIP_INIT_STATE && status != PZIP_BUSY_STATE &&
	    status != PZIP_FINISH_STATE)
    {
        return PZIP_STREAM_ERROR;
    }

    /* Deallocate in reverse order of allocations: */
    if (PSTRM_STATE->mpPendingBuf)
    {
        delete PSTRM_STATE->mpPendingBuf;
    }

    if (PSTRM_STATE->mpHead)
    {
        delete PSTRM_STATE->mpHead;
    }

    if (PSTRM_STATE->mpPrev)
    {
        delete PSTRM_STATE->mpPrev;
    }

    if (PSTRM_STATE->mpWindow)
    {
        delete PSTRM_STATE->mpWindow;
    }

    delete PSTRM_STATE;
    pStrm->mpState = NULL;

    if (status == PZIP_BUSY_STATE)
    {
        return PZIP_DATA_ERROR;
    }

    return PZIP_OK;

#undef PSTRM_STATE
}

/*
 * Read a new buffer from the current input stream, update the adler32
 * and total number of bytes read.  All PzipDeflate() input goes through
 * this function so some applications may wish to modify it to avoid
 * allocating a large pStrm->mNextIn buffer and copying from it.
 * (See also flush_pending()).
 */
DWORD read_buf( PzipStream* pStrm, UCHAR* pBuf, DWORD size )
{
#define PSTRM_STATE ((PzipDeflateInternalState*) pStrm->mpState)

    DWORD len = pStrm->mAvailIn;

    if (len > size)
    {
        len = size;
    }

    if (len == 0)
    {
        return 0;
    }

    pStrm->mAvailIn  -= len;

    if (!PSTRM_STATE->mNoHeader)
    {
        pStrm->mAdler = PzipAdler32( pStrm->mAdler, pStrm->mpNextIn, len );
    }

    memcpy( pBuf, pStrm->mpNextIn, len );
    pStrm->mpNextIn  += len;
    pStrm->mTotalIn += len;

    return len;

#undef PSTRM_STATE
}

/*
 * Initialize the "longest match" routines for a new zlib stream
 */
void lm_init( PzipDeflateInternalState* pS )
{
    pS->mWindowSize = (DWORD) 2L * pS->mWsize;

    CLEAR_HASH(pS);

    /* Set the default configuration parameters:
     */

    pS->mMaxLazyMatch = gPzipConfigurationTable[pS->mLevel].mMaxLazy;
    pS->mGoodMatch = gPzipConfigurationTable[pS->mLevel].mGoodLength;
    pS->mNiceMatch = gPzipConfigurationTable[pS->mLevel].mNiceLength;
    pS->mMaxChainLength = gPzipConfigurationTable[pS->mLevel].mMaxChain;

    pS->mStrStart = 0;
    pS->mBlockStart = 0L;
    pS->mLookahead = 0;
    pS->mMatchLength = pS->mPrevLength = PZIP_MIN_MATCH - 1;
    pS->mMatchAvailable = 0;
    pS->mInsh = 0;
}

/*
 * Set mMatchStart to the longest match starting at the given string and
 * return its length. Matches shorter or equal to mPrevLength are discarded,
 * in which case the result is equal to mPrevLength and mMatchStart is
 * garbage.
 * IN assertions: cur_match is the head of the hash chain for the current
 *   string (mStrStart) and its distance is <= PZIP_MAX_DIST, and
 *   mPrevLength >= 1
 * OUT assertion: the match length is not greater than s->mLookahead.
 */
DWORD longest_match( PzipDeflateInternalState* pS, WORD cur_match )
    /* cur_match: current match */
{
    WORD chain_length = pS->mMaxChainLength; /* max hash chain length */
    register UCHAR* p_scan = pS->mpWindow + pS->mStrStart;/* current string */
    register UCHAR* p_match;                       /* matched string */
    register LONG len;                         /* length of current match */
    LONG best_len = pS->mPrevLength;           /* best match length so far */
    LONG nice_match = pS->mNiceMatch;          /* stop if match long enough */

    WORD limit;
    if (pS->mStrStart > (DWORD) PZIP_MAX_DIST(pS))
    {
        limit = (WORD) (pS->mStrStart - PZIP_MAX_DIST(pS));
    }
    else
    {
        limit = NIL;
    }

    /* Stop when cur_match becomes <= limit. To simplify the code,
     * we prevent matches with the string of window index 0.
     */

    WORD* p_prev = pS->mpPrev;
    WORD w_mask = pS->mWmask;

    register UCHAR* p_str_end = pS->mpWindow + pS->mStrStart + PZIP_MAX_MATCH;
    register UCHAR scan_end_1  = p_scan[best_len-1];
    register UCHAR scan_end   = p_scan[best_len];

    /* The code is optimized for HASH_BITS >= 8 and PZIP_MAX_MATCH - 2
     * multiple of 16.  It is easy to get rid of this optimization if
     * necessary.
     */

    /* Do not waste too much time if we already have a good match: */
    if (pS->mPrevLength >= pS->mGoodMatch)
    {
        chain_length >>= 2;
    }

    /* Do not look for matches beyond the end of the input. This is necessary
     * to make PzipDeflate deterministic.
     */
    if ((DWORD) nice_match > pS->mLookahead)
    {
        nice_match = pS->mLookahead;
    }

    do
    {
        p_match = pS->mpWindow + cur_match;

        /* Skip to next match if the match length cannot increase
         * or if the match length is less than 2:
         */

        if (p_match[best_len]!= scan_end  ||
            p_match[best_len - 1] != scan_end_1 || *p_match != *p_scan ||
            *++p_match != p_scan[1])
        {
            continue;
        }

        /* The check at best_len - 1 can be removed because it will be made
         * again later. (This heuristic is not always a win.)
         * It is not necessary to compare p_scan[2] and p_match[2] since they
         * are always equal when the other bytes match, given that
         * the hash keys are equal and that HASH_BITS >= 8.
         */

        p_scan += 2;
        p_match++;

        /* We check for insufficient lookahead only every 8th comparison;
         * the 256th check will be made at mStrStart + 258.
         */
        do
        {
        }
        while (*++p_scan == *++p_match && *++p_scan == *++p_match &&
               *++p_scan == *++p_match && *++p_scan == *++p_match &&
               *++p_scan == *++p_match && *++p_scan == *++p_match &&
               *++p_scan == *++p_match && *++p_scan == *++p_match &&
               p_scan < p_str_end);

        len = PZIP_MAX_MATCH - (LONG) (p_str_end - p_scan);
        p_scan = p_str_end - PZIP_MAX_MATCH;

        if (len > best_len)
        {
            pS->mMatchStart = cur_match;
            best_len = len;
            if (len >= nice_match)
            {
                break;
            }
            scan_end_1 = p_scan[best_len - 1];
            scan_end = p_scan[best_len];
        }
    }
    while ((cur_match = p_prev[cur_match & w_mask]) > limit &&
           --chain_length != 0);

    if ((DWORD) best_len <= pS->mLookahead)
    {
        return (WORD) best_len;
    }

    return pS->mLookahead;
}

#define check_match(s, start, match, length)

/*
 * Fill the window when the lookahead becomes insufficient.
 * Updates mStrStart and mLookahead.
 *
 * IN assertion: mLookahead < PZIP_MIN_LOOKAHEAD
 * OUT assertions: mStrStart <= mWindowSize - PZIP_MIN_LOOKAHEAD
 *    At least one byte has been read, or mAvailIn == 0; reads are
 *    performed for at least two bytes (required for the zip translate_eol
 *    option -- not supported here).
 */
void fill_window( PzipDeflateInternalState* pS )
{
    register DWORD n, m;
    register WORD* p_p;
    DWORD more;    /* Amount of free space at the end of the window. */
    DWORD wsize = pS->mWsize;

    do
    {
        more = (pS->mWindowSize - (DWORD) pS->mLookahead -
                       (DWORD) pS->mStrStart);

 
        if (more == (WORD) (-1))
        {
            /* Very unlikely, but possible on 16 bit machine if mStrStart == 0
             * and mLookahead == 1 (input done one byte at time)
             */
            more--;

            /* If the window is almost full and there is insufficient
             * lookahead, move the upper half to the lower one to make room in
             * the upper half.
             */
        }
        else if (pS->mStrStart >= (DWORD) (wsize + PZIP_MAX_DIST(pS)))
        {
            memcpy( pS->mpWindow, pS->mpWindow + wsize, (WORD) wsize );
            pS->mMatchStart -= (WORD) wsize;
            pS->mStrStart -= wsize; // we now have mStrStart >= PZIP_MAX_DIST
            pS->mBlockStart -= (LONG) wsize;

            /* Slide the hash table (could be avoided with 32 bit values
               at the expense of memory usage). We slide even when level == 0
               to keep the hash table consistent if we switch back to
               level > 0 later. (Using level 0 permanently is not an optimal
               usage of zlib, so we don't care about this pathological case.)
             */

	        n = pS->mHashSize;
	        p_p = &pS->mpHead[n];
	        do
            {
		        m = *--p_p;

                if (m >= wsize)
                {
                    *p_p = (WORD) (m - wsize);
                }
                else
                {
                    *p_p = (WORD) NIL;
                }
	        } while (--n);

	        n = wsize;
	        p_p = &pS->mpPrev[n];
	        do
            {
		        m = *--p_p;

                if (m >= wsize)
                {
                    *p_p = (WORD) (m - wsize);
                }
                else
                {
                    *p_p = (WORD) NIL;
                }

    		    /* If n is not on any hash chain, mpPrev[n] is garbage but
		         * its value will never be used.
		         */
	        } while (--n);

            more += wsize;
        }

        if (pS->mpStrm->mAvailIn == 0)
        {
            return;
        }

        /* If there was no sliding:
         *    mStrStart <= WSIZE + PZIP_MAX_DIST - 1 &&
         *    mLookahead <= PZIP_MIN_LOOKAHEAD - 1 &&
         *    more == mWindowSize - mLookahead - mStrStart
         * => more >= mWindowSize -
         *               (PZIP_MIN_LOOKAHEAD - 1 + WSIZE + PZIP_MAX_DIST - 1)
         * => more >= mWindowSize - 2 * WSIZE + 2
         * In the BIG_MEM or MMAP case (not yet supported),
         *   mWindowSize == input_size + PZIP_MIN_LOOKAHEAD  &&
         *   mStrStart + s->mLookahead <= input_size =>
         *      more >= PZIP_MIN_LOOKAHEAD.
         * Otherwise, mWindowSize == 2 * WSIZE so more >= 2.
         * If there was sliding, more >= WSIZE. So in all cases, more >= 2.
         */

        n = read_buf( pS->mpStrm,
                      pS->mpWindow + pS->mStrStart + pS->mLookahead, more );

        pS->mLookahead += n;

        /* Initialize the hash value now that we have some input: */

        if (pS->mLookahead >= PZIP_MIN_MATCH)
        {
            pS->mInsh = pS->mpWindow[pS->mStrStart];
            UPDATE_HASH(pS, pS->mInsh, pS->mpWindow[pS->mStrStart + 1]);
#if PZIP_MIN_MATCH != 3
            Call UPDATE_HASH() PZIP_MIN_MATCH - 3 more times
#endif
        }

        /* If the whole input has less than PZIP_MIN_MATCH bytes, mInsh is
         * garbage, but this is not important since only literal bytes will be
         * emitted.
         */
    }
    while (pS->mLookahead < PZIP_MIN_LOOKAHEAD && pS->mpStrm->mAvailIn != 0);
}

/*
 * Flush the current block, with given end-of-file flag.
 * IN assertion: mStrStart is set to the end of the current match.
 */
#define FLUSH_BLOCK_ONLY(s, eof) \
    { \
        if (s->mBlockStart >= 0L) \
        { \
            PzipTrFlushBlock( \
               s, (PEGCHAR*) &s->mpWindow[(WORD) s->mBlockStart], \
               (DWORD) ((LONG) s->mStrStart - s->mBlockStart), (eof) ); \
        } \
        else \
        { \
            PzipTrFlushBlock( s, (PEGCHAR*) NULL, \
		                     (DWORD) ((LONG) s->mStrStart - s->mBlockStart), \
                             (eof) ); \
        } \
        s->mBlockStart = s->mStrStart; \
        flush_pending( s->mpStrm ); \
    }

/* Same but force premature exit if necessary. */
#define FLUSH_BLOCK(s, eof) \
    { \
        FLUSH_BLOCK_ONLY(s, eof); \
        if (s->mpStrm->mAvailOut == 0) \
        { \
            return ((eof) ? FINISH_STARTED : NEED_MORE); \
        } \
    }

/*
 * Copy without compression as much as possible from the input stream, return
 * the current block state.
 * This function does not insert new strings in the dictionary since
 * uncompressible data is probably not useful. This function is used
 * only for the level=0 compression option.
 * NOTE: this function should be optimized to avoid extra copying from
 * window to mpPendingBuf.
 */
BlockState deflate_stored( PzipDeflateInternalState* pS, LONG flush )
{
    /* Stored blocks are limited to 0xFFFF bytes, mpPendingBuf is limited
     * to mPendingBufSize, and each stored block has a 5 byte header:
     */
    DWORD max_block_size = 0xFFFF;
    DWORD max_start;

    if (max_block_size > pS->mPendingBufSize - 5)
    {
        max_block_size = pS->mPendingBufSize - 5;
    }

    /* Copy as much as possible from input to output: */
    for (;;)
    {
        /* Fill the window as much as possible: */
        if (pS->mLookahead <= 1)
        {
            fill_window( pS );
            if (pS->mLookahead == 0 && flush == PZIP_NO_FLUSH)
            {
                return NEED_MORE;
            }

            if (pS->mLookahead == 0) /* flush the current block */
            {
                break;
            }
        }

	    pS->mStrStart += pS->mLookahead;
	    pS->mLookahead = 0;

	    /* Emit a stored block if mpPendingBuf will be full: */

 	    max_start = pS->mBlockStart + max_block_size;

        if (pS->mStrStart == 0 || (DWORD) pS->mStrStart >= max_start)
        {
	        /* mStrStart == 0 is possible when wraparound on 16-bit machine */
	        pS->mLookahead = (WORD) (pS->mStrStart - max_start);
	        pS->mStrStart = (WORD) max_start;
            FLUSH_BLOCK(pS, 0);
	    }

	    /* Flush if we may have to slide, otherwise mBlockStart may become
         * negative and the data will be gone:
         */

        if (pS->mStrStart - (WORD) pS->mBlockStart >= PZIP_MAX_DIST(pS))
        {
            FLUSH_BLOCK(pS, 0);
	    }
    }

    FLUSH_BLOCK(pS, flush == PZIP_FINISH);

    if (flush == PZIP_FINISH)
    {
        return FINISH_DONE;
    }
    else
    {
        return BLOCK_DONE;
    }
}

/*
 * Compress as much as possible from the input stream, return the current
 * block state.
 * This function does not perform lazy evaluation of matches and inserts
 * new strings in the dictionary only for unmatched strings or for short
 * matches. It is used only for the fast compression options.
 */
BlockState deflate_fast( PzipDeflateInternalState* pS, LONG flush )
{
    WORD hash_head = NIL; /* head of the hash chain */
    LONG b_flush;           /* set if current block must be flushed */

    for (;;)
    {
        /* Make sure that we always have enough lookahead, except
         * at the end of the input file. We need PZIP_MAX_MATCH bytes
         * for the next match, plus PZIP_MIN_MATCH bytes to insert the
         * string following the next match.
         */

        if (pS->mLookahead < PZIP_MIN_LOOKAHEAD)
        {
            fill_window( pS );

            if (pS->mLookahead < PZIP_MIN_LOOKAHEAD && flush == PZIP_NO_FLUSH)
            {
	            return NEED_MORE;
	        }

            if (pS->mLookahead == 0) /* flush the current block */
            {
                break;
            }
        }

        /* Insert the string window[mStrStart .. mStrStart + 2] in the
         * dictionary, and set hash_head to the head of the hash chain:
         */

        if (pS->mLookahead >= PZIP_MIN_MATCH)
        {
            INSERT_STRING(pS, pS->mStrStart, hash_head);
        }

        /* Find the longest match, discarding those <= mPrevLength.
         * At this point we have always mMatchLength < PZIP_MIN_MATCH
         */

        if (hash_head != NIL &&
            pS->mStrStart - hash_head <= PZIP_MAX_DIST(pS))
        {
            /* To simplify the code, we prevent matches with the string
             * of window index 0 (in particular we have to avoid a match
             * of the string with itself at the start of the input file).
             */

            if (pS->mStrategy != PZIP_HUFFMAN_ONLY)
            {
                /* longest_match() sets mMatchStart */
                pS->mMatchLength = longest_match( pS, hash_head );
            }
        }

        if (pS->mMatchLength >= PZIP_MIN_MATCH)
        {
            check_match( pS, pS->mStrStart, pS->mMatchStart,
                         pS->mMatchLength );

            PzipTrTallyDist( pS, pS->mStrStart - pS->mMatchStart,
                             pS->mMatchLength - PZIP_MIN_MATCH, b_flush );

            pS->mLookahead -= pS->mMatchLength;

            /* Insert new strings in the hash table only if the match length
             * is not too large. This saves time but degrades compression.
             */

            if (pS->mMatchLength <= pS->PzipMaxInsertLength &&
                pS->mLookahead >= PZIP_MIN_MATCH)
            {
                /* string at mStrStart already in hash table */
                pS->mMatchLength--;

                do
                {
                    pS->mStrStart++;
                    INSERT_STRING(pS, pS->mStrStart, hash_head);

                    /* mStrStart never exceeds WSIZE - PZIP_MAX_MATCH, so
                     * there are always PZIP_MIN_MATCH bytes ahead.
                     */
                } while (--pS->mMatchLength != 0);

                pS->mStrStart++;
            }
            else
	        {
                pS->mStrStart += pS->mMatchLength;
                pS->mMatchLength = 0;
                pS->mInsh = pS->mpWindow[pS->mStrStart];
                UPDATE_HASH(pS, pS->mInsh, pS->mpWindow[pS->mStrStart + 1]);
#if PZIP_MIN_MATCH != 3
                Call UPDATE_HASH() PZIP_MIN_MATCH - 3 more times
#endif
                /* If mLookahead < PZIP_MIN_MATCH, mInsh is garbage, but it
                 * does not matter since it will be recomputed at next
                 * PzipDeflate call.
                 */
            }
        }
        else
        {
            /* No match, output a literal byte */
            PzipTrTallyLit( pS, pS->mpWindow[pS->mStrStart], b_flush );
            pS->mLookahead--;
            pS->mStrStart++;
        }

        if (b_flush)
        {
            FLUSH_BLOCK(pS, 0);
        }
    }

    FLUSH_BLOCK(pS, flush == PZIP_FINISH);

    if (flush == PZIP_FINISH)
    {
        return FINISH_DONE;
    }

    return BLOCK_DONE;
}

/*
 * Same as above, but achieves better compression. We use a lazy
 * evaluation for matches: a match is finally adopted only if there is
 * no better match at the next window position.
 */
BlockState deflate_slow( PzipDeflateInternalState* pS, LONG flush )
{
    WORD hash_head = NIL;    /* head of hash chain */
    LONG b_flush;              /* set if current block must be flushed */

    /* Process the input block. */
    for (;;)
    {
        /* Make sure that we always have enough lookahead, except
         * at the end of the input file. We need PZIP_MAX_MATCH bytes
         * for the next match, plus PZIP_MIN_MATCH bytes to insert the
         * string following the next match.
         */
        if (pS->mLookahead < PZIP_MIN_LOOKAHEAD)
        {
            fill_window( pS );

            if (pS->mLookahead < PZIP_MIN_LOOKAHEAD && flush == PZIP_NO_FLUSH)
            {
	            return NEED_MORE;
	        }

            if (pS->mLookahead == 0) /* flush the current block */
            {
                break;
            }
        }

        /* Insert the string window[mStrStart .. mStrStart + 2] in the
         * dictionary, and set hash_head to the head of the hash chain:
         */
        if (pS->mLookahead >= PZIP_MIN_MATCH)
        {
            INSERT_STRING(pS, pS->mStrStart, hash_head);
        }

        /* Find the longest match, discarding those <= mPrevLength.
         */

        pS->mPrevLength = pS->mMatchLength, pS->mPrevMatch = pS->mMatchStart;
        pS->mMatchLength = PZIP_MIN_MATCH - 1;

        if (hash_head != NIL && pS->mPrevLength < pS->mMaxLazyMatch &&
            pS->mStrStart - hash_head <= PZIP_MAX_DIST(pS))
        {
            /* To simplify the code, we prevent matches with the string
             * of window index 0 (in particular we have to avoid a match
             * of the string with itself at the start of the input file).
             */
            if (pS->mStrategy != PZIP_HUFFMAN_ONLY)
            {
                /* longest_match() sets mMatchStart */
                pS->mMatchLength = longest_match( pS, hash_head );
            }

            if (pS->mMatchLength <= 5 &&
                (pS->mStrategy == PZIP_FILTERED ||
                 (pS->mMatchLength == PZIP_MIN_MATCH &&
                  pS->mStrStart - pS->mMatchStart > TOO_FAR)))
            {
                /* If mPrevMatch is also PZIP_MIN_MATCH, mMatchStart is
                 * garbage but we will ignore the current match anyway.
                 */
                pS->mMatchLength = PZIP_MIN_MATCH - 1;
            }
        }

        /* If there was a match at the previous step and the current
         * match is not better, output the previous match:
         */
        if (pS->mPrevLength >= PZIP_MIN_MATCH &&
            pS->mMatchLength <= pS->mPrevLength)
        {
            DWORD max_insert = pS->mStrStart + pS->mLookahead - PZIP_MIN_MATCH;

            /* Do not insert strings in hash table beyond this. */

            check_match( pS, pS->mStrStart - 1, pS->mPrevMatch,
                         pS->mPrevLength);

            PzipTrTallyDist( pS, pS->mStrStart - 1 - pS->mPrevMatch,
			                 pS->mPrevLength - PZIP_MIN_MATCH, b_flush );

            /* Insert in hash table all strings up to the end of the match.
             * mStrStart - 1 and mStrStart are already inserted. If there is
             * not enough lookahead, the last two strings are not inserted in
             * the hash table.
             */

            pS->mLookahead -= pS->mPrevLength - 1;
            pS->mPrevLength -= 2;
            do
            {
                if (++pS->mStrStart <= max_insert)
                {
                    INSERT_STRING(pS, pS->mStrStart, hash_head);
                }
            } while (--pS->mPrevLength != 0);

            pS->mMatchAvailable = 0;
            pS->mMatchLength = PZIP_MIN_MATCH - 1;
            pS->mStrStart++;

            if (b_flush)
            {
                FLUSH_BLOCK(pS, 0);
            }
        }
        else if (pS->mMatchAvailable)
        {
            /* If there was no match at the previous position, output a
             * single literal. If there was a match but the current match
             * is longer, truncate the previous match to a single literal.
             */
	        PzipTrTallyLit( pS, pS->mpWindow[pS->mStrStart - 1], b_flush );

	        if (b_flush)
            {
                FLUSH_BLOCK_ONLY(pS, 0);
            }

            pS->mStrStart++;
            pS->mLookahead--;
            if (pS->mpStrm->mAvailOut == 0)
            {
                return NEED_MORE;
            }
        }
        else
        {
            /* There is no previous match to compare with, wait for
             * the next step to decide.
             */
            pS->mMatchAvailable = 1;
            pS->mStrStart++;
            pS->mLookahead--;
        }
    }

    if (pS->mMatchAvailable)
    {
        PzipTrTallyLit_2( pS, pS->mpWindow[pS->mStrStart - 1], b_flush );
        pS->mMatchAvailable = 0;
    }

    FLUSH_BLOCK(pS, flush == PZIP_FINISH);

    if (flush == PZIP_FINISH)
    {
        return FINISH_DONE;
    }

    return BLOCK_DONE;
}

#endif  // PEG_ZIP

//
//****************************************************************************
// infblock.c - interpret and process block types to last block
//****************************************************************************
//

#if defined(PEG_UNZIP)

/* simplify the use of the PzipInflateHuft type with some defines */
#define exop mWord.mWhat.mExop
#define bits mWord.mWhat.mBits

/* Table for deflate from PKZIP's appnote.txt. */
const WORD border[] = /* Order of the bit length code lengths */
{ 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

/*
   Notes beyond the 1.93a appnote.txt:

   1. Distance pointers never point before the beginning of the output
      stream.
   2. Distance pointers can point back across blocks, up to 32k away.
   3. There is an implied maximum of 7 bits for the bit length table and
      15 bits for the actual data.
   4. If only one code exists, then it is encoded using one bit.  (Zero
      would be more efficient, but perhaps a little confusing.)  If two
      codes exist, they are coded using one bit each (0 and 1).
   5. There is no way of sending zero distance codes--a dummy must be
      sent if there are none.  (History: a pre 2.0 version of PKZIP would
      store blocks with no distance codes, but this was discovered to be
      too harsh a criterion.)  Valid only for 1.93a.  2.04c does allow
      zero distance codes, which is sent as one code of zero bits in
      length.
   6. There are up to 286 literal/length codes.  Code 256 represents the
      end-of-block.  Note however that the static length tree defines
      288 codes just to fill out the Huffman codes.  Codes 286 and 287
      cannot be used though, since there is no length base or extra bits
      defined for them.  Similarily, there are up to 30 distance codes.
      However, static trees define 32 codes (all 5 bits) to fill out the
      Huffman codes, but the last two had better not show up in the data.
   7. Unzip can check dynamic Huffman blocks for complete code sets.
      The exception is that a single code would not be complete (see #4).
   8. The five bits following the block type is really the number of
      literal codes sent minus 257.
   9. Length codes 8,16,16 are interpreted as 13 length codes of 8 bits
      (1 + 6 + 6).  Therefore, to output three times the length, you output
      three codes (1 + 1 + 1), whereas to output four times the same length,
      you only need two codes (1 + 3).  Hmm.
  10. In the tree reconstruction algorithm, Code = Code + Increment
      only if BitLength( i ) is not zero.  (Pretty obvious.)
  11. Correction: 4 Bits: # of Bit Length codes - 4     (4 - 19)
  12. Note: length code 284 can represent 227 - 258, but length code 285
      really is 258.  The last length deserves its own, short code
      since it gets used a lot in very redundant files.  The length
      258 is special since 258 - 3 (the min match length) is 255.
  13. The literal/length and distance code bit lengths are read as a
      single stream of lengths.  It is possible (and advantageous) for
      a repeat code (16, 17, or 18) to go across the boundary between
      the two sets of lengths.
*/


void PzipInflateBlocksReset( PzipInflateBlocksState* pS, PzipStream* pZ,
                             DWORD* pC )
{
    if (pC)
    {
        *pC = pS->mCheck;
    }

    if (pS->mMode == PZIP_BTREE || pS->mMode == PZIP_DTREE)
    {
        delete pS->mSub.mTrees.mpBlens;
    }

    if (pS->mMode == PZIP_CODES)
    {
        PzipInflateCodesFree( pS->mSub.mDecode.mpCodes, pZ );
    }

    pS->mMode = PZIP_TYPE;
    pS->mBitk = 0;
    pS->mBitb = 0;
    pS->mpRead = pS->mpWrite = pS->mpWindow;
    if (pS->mCheckFn)
    {
        pZ->mAdler = pS->mCheck =
           (*pS->mCheckFn)( 0L, (const UCHAR*) NULL, 0);
    }
}

PzipInflateBlocksState* PzipInflateBlocksNew(
   PzipStream* pZ, PzipCheckFunc c, DWORD w )
{
    PzipInflateBlocksState* p_s;

    p_s = new PzipInflateBlocksState;
    if (!p_s)
    {
        return p_s;
    }

    p_s->mpHufts = new PzipInflateHuft[MANY];
    if (!p_s->mpHufts)
    {
        delete p_s;
        return NULL;
    }

    p_s->mpWindow = new UCHAR[w];
    if (!p_s->mpWindow)
    {
        delete p_s->mpHufts;
        delete p_s;
        return NULL;
    }

    p_s->mpEnd = p_s->mpWindow + w;
    p_s->mCheckFn = c;
    p_s->mMode = PZIP_TYPE;
    PzipInflateBlocksReset( p_s, pZ, NULL );
    return p_s;
}

SIGNED PzipInflateBlocks( PzipInflateBlocksState* pS, PzipStream* pZ, SIGNED r )
{
    DWORD t;               /* temporary storage */
    DWORD b;              /* bit buffer */
    DWORD k;               /* bits in bit buffer */
    const UCHAR* p_p;             /* input data pointer */
    DWORD n;               /* bytes available there */
    UCHAR* p_q;             /* output window write pointer */
    DWORD m;               /* bytes to end of window or read pointer */

    /* copy input/output information to locals (PZIP_UPDATE macro restores) */
    PZIP_LOAD

    /* process input based on current state */
	BOOL loop = TRUE;
    while (loop)
    {
        switch (pS->mMode)
        {
            case PZIP_TYPE:
                PZIP_NEEDBITS(3)
                t = (WORD) b & 7;
                pS->mLast = (WORD) (t & 1);

                switch (t >> 1)
                {
                    case 0:                         /* stored */
                        PZIP_DUMPBITS(3)
                        t = k & 7;         /* go to byte boundary */
                        PZIP_DUMPBITS(t)
                        pS->mMode = PZIP_LENS;/* get length of stored block */
                        break;

                    case 1:                         /* fixed */
                        {
                            DWORD bl, bd;
                            PzipInflateHuft* p_t_l, * p_t_d;

                            PzipInflateTreesFixed( &bl, &bd, &p_t_l, &p_t_d,
                                                   pZ );

                            pS->mSub.mDecode.mpCodes = PzipInflateCodesNew(
                               bl, bd, p_t_l, p_t_d, pZ );

                            if (!pS->mSub.mDecode.mpCodes)
                            {
                                r = PZIP_MEM_ERROR;
                                PZIP_LEAVE
                            }
                        }
                        PZIP_DUMPBITS(3)
                        pS->mMode = PZIP_CODES;
                        break;

                    case 2:                         /* dynamic */
                        PZIP_DUMPBITS(3)
                        pS->mMode = PZIP_TABLE;
                        break;

                    case 3:                         /* illegal */
                        PZIP_DUMPBITS(3)
                        pS->mMode = PZIP_BAD;
                        r = PZIP_DATA_ERROR;
                        PZIP_LEAVE
                }
                break;

            case PZIP_LENS:
                PZIP_NEEDBITS(32)

                if ((((~b) >> 16) & 0xFFFF) != (b & 0xFFFF))
                {
                    pS->mMode = PZIP_BAD;
                    r = PZIP_DATA_ERROR;
                    PZIP_LEAVE
                }

                pS->mSub.mLeft = (WORD) b & 0xFFFF;
                b = k = 0;                      /* dump bits */

                if (pS->mSub.mLeft)
                {
                    pS->mMode = PZIP_STORED;
                }
                else
                {
                    if (pS->mLast)
                    {
                        pS->mMode = PZIP_DRY;
                    }
                    else
                    {
                        pS->mMode = PZIP_TYPE;
                    }
                }
                break;

            case PZIP_STORED:
                if (n == 0)
                {
                    PZIP_LEAVE
                }

                PZIP_NEEDOUT
                t = pS->mSub.mLeft;
                if (t > n)
                {
                    t = n;
                }

                if (t > m)
                {
                    t = m;
                }

                memcpy( p_q, p_p, t );
                p_p += t;
                n -= t;
                p_q += t;
                m -= t;

                if ((pS->mSub.mLeft -= t) != 0)
                {
                    break;
                }

                if (pS->mLast)
                {
                    pS->mMode = PZIP_DRY;
                }
                else
                {
                    pS->mMode = PZIP_TYPE;
                }
                break;

            case PZIP_TABLE:
                PZIP_NEEDBITS(14)
                t =  b & 0x3FFF;
                pS->mSub.mTrees.mTable =  (WORD) t;
                if ((t & 0x1F) > 29 || ((t >> 5) & 0x1F) > 29)
                {
                    pS->mMode = PZIP_BAD;
                    r = PZIP_DATA_ERROR;
                    PZIP_LEAVE
                }
                t = 258 + (t & 0x1F) + ((t >> 5) & 0x1F);

                pS->mSub.mTrees.mpBlens = new DWORD[t];
                if (!pS->mSub.mTrees.mpBlens)
                {
                    r = PZIP_MEM_ERROR;
                    PZIP_LEAVE
                }

                PZIP_DUMPBITS(14)
                pS->mSub.mTrees.mIndex = 0;
                pS->mMode = PZIP_BTREE;

            case PZIP_BTREE:
                while (pS->mSub.mTrees.mIndex <
                       4 + (pS->mSub.mTrees.mTable >> 10))
                {
                    PZIP_NEEDBITS(3)
                    pS->mSub.mTrees.mpBlens[border[pS->mSub.mTrees.mIndex++]]
                       = (WORD) b & 7;
                    PZIP_DUMPBITS(3)
                }

                while (pS->mSub.mTrees.mIndex < 19)
                {
                    pS->mSub.mTrees.mpBlens[border[pS->mSub.mTrees.mIndex++]]
                       = 0;
                }

                pS->mSub.mTrees.mBb = 7;

                t = PzipInflateTreesBits(
                   pS->mSub.mTrees.mpBlens, &pS->mSub.mTrees.mBb,
                   &pS->mSub.mTrees.mpTb, pS->mpHufts, pZ );

                if (t != PZIP_OK)
                {
                    delete pS->mSub.mTrees.mpBlens;
                    r = (SIGNED) t;

                    if (r == PZIP_DATA_ERROR)
                    {
                        pS->mMode = PZIP_BAD;
                    }

                    PZIP_LEAVE
                }

                pS->mSub.mTrees.mIndex = 0;
                pS->mMode = PZIP_DTREE;

            case PZIP_DTREE:
                while (t = pS->mSub.mTrees.mTable,
                       pS->mSub.mTrees.mIndex <
                          258 + (t & 0x1F) + ((t >> 5) & 0x1F))
                {
                    PzipInflateHuft* p_h;
                    DWORD i, j, c;

                    t = pS->mSub.mTrees.mBb;
                    PZIP_NEEDBITS(t)
                    p_h = pS->mSub.mTrees.mpTb +
                             ((WORD) b & PzipInflateMask[t]);
                    t = p_h->bits;
                    c = p_h->mBase;

                    if (c < 16)
                    {
                        PZIP_DUMPBITS(t)
                        pS->mSub.mTrees.mpBlens[pS->mSub.mTrees.mIndex++] = c;
                    }
                    else /* c == 16..18 */
                    {
                        if (c == 18)
                        {
                            i = 7;
                        }
                        else
                        {
                            i = c - 14;
                        }

                        if (c == 18)
                        {
                            j = 11;
                        }
                        else
                        {
                            j = 3;
                        }

                        PZIP_NEEDBITS(t + i)
                        PZIP_DUMPBITS(t)
                        j += (WORD) b & PzipInflateMask[i];
                        PZIP_DUMPBITS(i)
                        i = pS->mSub.mTrees.mIndex;
                        t = pS->mSub.mTrees.mTable;

                        if (i + j > 258 + (t & 0x1F) + ((t >> 5) & 0x1F) ||
                            (c == 16 && i < 1))
                        {
                            delete pS->mSub.mTrees.mpBlens;
                            pS->mMode = PZIP_BAD;
                            r = PZIP_DATA_ERROR;
                            PZIP_LEAVE
                        }

                        if (c == 16)
                        {
                            c = pS->mSub.mTrees.mpBlens[i - 1];
                        }
                        else
                        {
                            c = 0;
                        }

                        do
                        {
                            pS->mSub.mTrees.mpBlens[i++] = c;
                        } while (--j);

                        pS->mSub.mTrees.mIndex = (WORD) i;
                    }
                }

                pS->mSub.mTrees.mpTb = NULL;

                {
                    DWORD bl, bd;
                    PzipInflateHuft* p_t_l, * p_t_d;
                    InflateCodesState* p_c;

                    bl = 9;     /* must be <= 9 for lookahead assumptions */
                    bd = 6;      /* must be <= 9 for lookahead assumptions */
                    t = pS->mSub.mTrees.mTable;

                    t = PzipInflateTreesDynamic(
                       257 + (t & 0x1F), 1 + ((t >> 5) & 0x1F),
                       pS->mSub.mTrees.mpBlens, &bl, &bd, &p_t_l, &p_t_d,
                       pS->mpHufts, pZ );

                    delete pS->mSub.mTrees.mpBlens;
                    
                    if (t != PZIP_OK)
                    {
                        if (t == (WORD) PZIP_DATA_ERROR)
                        {
                            pS->mMode = PZIP_BAD;
                        }

                        r = (SIGNED) t;
                        PZIP_LEAVE
                    }

                    if ((p_c = PzipInflateCodesNew(
                            bl, bd, p_t_l, p_t_d, pZ )) == NULL)
                    {
                        r = PZIP_MEM_ERROR;
                        PZIP_LEAVE
                    }

                    pS->mSub.mDecode.mpCodes = p_c;
                }

                pS->mMode = PZIP_CODES;

            case PZIP_CODES:
                PZIP_UPDATE

                if ((r = PzipInflateCodes( pS, pZ, r )) != PZIP_STREAM_END)
                {
                    return PzipInflateFlush( pS, pZ, r );
                }

                r = PZIP_OK;
                PzipInflateCodesFree( pS->mSub.mDecode.mpCodes, pZ );
                PZIP_LOAD

                if (!pS->mLast)
                {
                    pS->mMode = PZIP_TYPE;
                    break;
                }

                pS->mMode = PZIP_DRY;

            case PZIP_DRY:
                PZIP_FLUSH

                if (pS->mpRead != pS->mpWrite)
                {
                    PZIP_LEAVE
                }

                pS->mMode = PZIP_DONE;

            case PZIP_DONE:
                r = PZIP_STREAM_END;
                PZIP_LEAVE

            case PZIP_BAD:
                r = PZIP_DATA_ERROR;
                PZIP_LEAVE

            default:
                r = PZIP_STREAM_ERROR;
                PZIP_LEAVE
        }
    }

	r = PZIP_STREAM_ERROR;
	PZIP_LEAVE
}

LONG PzipInflateBlocksFree( PzipInflateBlocksState* pS, PzipStream* pZ )
{
    PzipInflateBlocksReset( pS, pZ, NULL );
    delete pS->mpWindow;
    delete pS->mpHufts;
    delete pS;
    return PZIP_OK;
}

#endif  // PEG_UNZIP

//
//****************************************************************************
// infcodes.c - process literals and length/distance pairs
//****************************************************************************
//

#if defined(PEG_UNZIP)

/* simplify the use of the PzipInflateHuft type with some defines */
#define exop mWord.mWhat.mExop
#undef bits
#define bits mWord.mWhat.mBits

typedef enum        /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
{
    START,    /* x: set up for LEN */
    LEN,      /* i: get length/literal/eob next */
    LENEXT,   /* i: getting length extra (have base) */
    DIST,     /* i: get distance next */
    DISTEXT,  /* i: getting distance extra */
    COPY,     /* o: copying bytes in window, waiting for space */
    LIT,      /* o: got literal, waiting for output space */
    WASH,     /* o: got eob, possibly still output waiting */
    END,      /* x: got eob and all data flushed */
    BADCODE  /* x: got error */
} InflateCodesMode;

/* PzipInflate codes private state */
struct InflateCodesState
{
    /* mode */
    InflateCodesMode mMode;      /* current PzipInflateCodes mode */

    /* mode dependent information */
    DWORD mLen;

    union
    {
        struct
        {
            PzipInflateHuft* mpTree;       /* pointer into tree */
            WORD mNeed;                /* bits needed */
        } mCode;             /* if LEN or DIST, where in tree */

        DWORD mLit;           /* if LIT, literal */

        struct
        {
            WORD mGet;                 /* bits to get for extra */
            DWORD mDist;                /* distance back to copy from */
        } mCopy;             /* if EXT or COPY, where and how much */
    } mSub;                /* submode */

    /* mode independent information */
    UCHAR mLbits;           /* mpLtree bits decoded per branch */
    UCHAR mDbits;           /* mpDtree bits decoder per branch */
    PzipInflateHuft* mpLtree;          /* literal/length/eob tree */
    PzipInflateHuft* mpDtree;          /* distance tree */
};

InflateCodesState* PzipInflateCodesNew(
   DWORD bL, DWORD bD, PzipInflateHuft* pTl, PzipInflateHuft* pTd,
   PzipStream* )
/* td: need separate declaration for Borland C++ */
{
    InflateCodesState* p_c;

    p_c = new InflateCodesState;
    if (p_c)
    {
        p_c->mMode = START;
        p_c->mLbits = (UCHAR) bL;
        p_c->mDbits = (UCHAR) bD;
        p_c->mpLtree = pTl;
        p_c->mpDtree = pTd;
    }

    return p_c;
}

SIGNED PzipInflateCodes( PzipInflateBlocksState* pS, PzipStream* pZ, SIGNED r )
{
    WORD j;               /* temporary storage */
    PzipInflateHuft* p_t;      /* temporary pointer */
    WORD e;               /* extra bits or operation */
    DWORD b;              /* bit buffer */
    DWORD k;               /* bits in bit buffer */
    const UCHAR* p_p;             /* input data pointer */
    DWORD n;               /* bytes available there */
    UCHAR* p_q;             /* output window write pointer */
    WORD m;               /* bytes to end of window or read pointer */
    UCHAR* p_f;             /* pointer to copy strings from */
    InflateCodesState* p_c = pS->mSub.mDecode.mpCodes;  /* codes state */

    /* copy input/output information to locals (PZIP_UPDATE macro restores) */
    PZIP_LOAD

    /* process input and output based on current state */
	BOOL loop = TRUE;
    while (loop)
    {
        switch (p_c->mMode)
        {             /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
            case START:         /* x: set up for LEN */
                if (m >= 258 && n >= 10)
                {
                    PZIP_UPDATE
                    r = PzipInflateFast(
                       p_c->mLbits, p_c->mDbits, p_c->mpLtree, p_c->mpDtree,
                       pS, pZ );
                    PZIP_LOAD

                    if (r != PZIP_OK)
                    {
                        if (r == PZIP_STREAM_END)
                        {
                            p_c->mMode = WASH;
                        }
                        else
                        {
                            p_c->mMode = BADCODE;
                        }

                        break;
                    }
                }

                p_c->mSub.mCode.mNeed = p_c->mLbits;
                p_c->mSub.mCode.mpTree = p_c->mpLtree;
                p_c->mMode = LEN;

            case LEN:           /* i: get length/literal/eob next */
                j = p_c->mSub.mCode.mNeed;
                PZIP_NEEDBITS(j)
                p_t = p_c->mSub.mCode.mpTree +
                         ((WORD) b & PzipInflateMask[j]);
                PZIP_DUMPBITS(p_t->bits)
                e = (WORD) (p_t->exop);

                if (e == 0)               /* literal */
                {
                    p_c->mSub.mLit = p_t->mBase;
                    p_c->mMode = LIT;
                    break;
                }

                if (e & 16)               /* length */
                {
                    p_c->mSub.mCopy.mGet = e & 15;
                    p_c->mLen = p_t->mBase;
                    p_c->mMode = LENEXT;
                    break;
                }

                if ((e & 64) == 0)        /* next table */
                {
                    p_c->mSub.mCode.mNeed = e;
                    p_c->mSub.mCode.mpTree = p_t + p_t->mBase;
                    break;
                }

                if (e & 32)               /* end of block */
                {
                    p_c->mMode = WASH;
                    break;
                }

                p_c->mMode = BADCODE;        /* invalid code */
                r = PZIP_DATA_ERROR;
                PZIP_LEAVE

            case LENEXT:        /* i: getting length extra (have base) */
                j = p_c->mSub.mCopy.mGet;
                PZIP_NEEDBITS(j)
                p_c->mLen += (WORD) b & PzipInflateMask[j];
                PZIP_DUMPBITS(j)
                p_c->mSub.mCode.mNeed = p_c->mDbits;
                p_c->mSub.mCode.mpTree = p_c->mpDtree;
                p_c->mMode = DIST;

            case DIST:          /* i: get distance next */
                j = p_c->mSub.mCode.mNeed;
                PZIP_NEEDBITS(j)
                p_t = p_c->mSub.mCode.mpTree +
                         ((WORD) b & PzipInflateMask[j]);
                PZIP_DUMPBITS(p_t->bits)
                e = (WORD) (p_t->exop);

                if (e & 16)               /* distance */
                {
                    p_c->mSub.mCopy.mGet = e & 15;
                    p_c->mSub.mCopy.mDist = p_t->mBase;
                    p_c->mMode = DISTEXT;
                    break;
                }

                if ((e & 64) == 0)        /* next table */
                {
                    p_c->mSub.mCode.mNeed = e;
                    p_c->mSub.mCode.mpTree = p_t + p_t->mBase;
                    break;
                }

                p_c->mMode = BADCODE;        /* invalid code */
                r = PZIP_DATA_ERROR;
                PZIP_LEAVE

            case DISTEXT:       /* i: getting distance extra */
                j = p_c->mSub.mCopy.mGet;
                PZIP_NEEDBITS(j)
                p_c->mSub.mCopy.mDist += (WORD) b & PzipInflateMask[j];
                PZIP_DUMPBITS(j)
                p_c->mMode = COPY;

            case COPY:     /* o: copying bytes in window, waiting for space */
                if ((DWORD) (p_q - pS->mpWindow) < p_c->mSub.mCopy.mDist)
                {
                    p_f = pS->mpEnd - (p_c->mSub.mCopy.mDist -
                       (DWORD) (p_q - pS->mpWindow));
                }
                else
                {
                    p_f = p_q - p_c->mSub.mCopy.mDist;
                }

                while (p_c->mLen)
                {
                    PZIP_NEEDOUT
                    PZIP_OUTBYTE(*p_f++)

                    if (p_f == pS->mpEnd)
                    {
                        p_f = pS->mpWindow;
                    }

                    p_c->mLen--;
                }

                p_c->mMode = START;
                break;

            case LIT:           /* o: got literal, waiting for output space */
                PZIP_NEEDOUT
                PZIP_OUTBYTE(p_c->mSub.mLit)
                p_c->mMode = START;
                break;

            case WASH:          /* o: got eob, possibly more output */
                if (k > 7)        /* return unused byte, if any */
                {
                    k -= 8;
                    n++;
                    p_p--;            /* can always return one */
                }

                PZIP_FLUSH_2

                if (pS->mpRead != pS->mpWrite)
                {
                    PZIP_LEAVE
                }

                p_c->mMode = END;

            case END:
                r = PZIP_STREAM_END;
                PZIP_LEAVE

            case BADCODE:       /* x: got error */
                r = PZIP_DATA_ERROR;
                PZIP_LEAVE

            default:
                r = PZIP_STREAM_ERROR;
                PZIP_LEAVE
        }
    }

	r = PZIP_STREAM_ERROR;
	PZIP_LEAVE
}

void PzipInflateCodesFree( InflateCodesState* pC, PzipStream*)
{
    delete pC;
}

#endif  // PEG_UNZIP

//
//****************************************************************************
// inffast.c - process literals and length/distance pairs fast
//****************************************************************************
//

#if defined(PEG_UNZIP)

/* simplify the use of the PzipInflateHuft type with some defines */
#define exop mWord.mWhat.mExop
#undef bits
#define bits mWord.mWhat.mBits

/* macros for bit input with no checking and for returning unused bytes */
#define GRABBITS(j) \
        { \
            while (k < (j)) \
            { \
                b |= ((DWORD) PZIP_NEXTBYTE) << k; \
                k += 8; \
            } \
        }

#define UNGRAB \
        { \
            c = pZ->mAvailIn - n; \
            if ((k >> 3) < c) \
            { \
                c = k >> 3; \
            } \
            n += c; \
            p_p -= c; \
            k -= c << 3; \
        }

/* Called with number of bytes left to write in window at least 258
   (the maximum string length) and number of input bytes available
   at least ten.  The ten bytes are six bytes for the longest length/
   distance pair plus four bytes for overloading the bit buffer. */

SIGNED PzipInflateFast( WORD bL, WORD bD, PzipInflateHuft* pTl,
                      PzipInflateHuft* pTd, PzipInflateBlocksState* pS,
                      PzipStream* pZ )
/* td: need separate declaration for Borland C++ */
{
    PzipInflateHuft* p_t;      /* temporary pointer */
    DWORD e;               /* extra bits or operation */
    DWORD b;              /* bit buffer */
    DWORD k;               /* bits in bit buffer */
    const UCHAR* p_p;             /* input data pointer */
    DWORD n;               /* bytes available there */
    UCHAR* p_q;             /* output window write pointer */
    DWORD m;               /* bytes to end of window or read pointer */
    WORD m_l;              /* mask for literal/length tree */
    WORD m_d;              /* mask for distance tree */
    DWORD c;               /* bytes to copy */
    DWORD d;               /* distance back to copy from */
    UCHAR* p_r;             /* copy source pointer */

    /* load input, output, bit values */
    PZIP_LOAD

    /* initialize masks */
    m_l = PzipInflateMask[bL];
    m_d = PzipInflateMask[bD];

    /* do until not enough input or output space for fast loop */
    do                          /* assume called with m >= 258 && n >= 10 */
    {
        /* get literal/length code */
        GRABBITS(20)                /* max bits for literal/length code */

        if ((e = (p_t = pTl + ((WORD) b & m_l))->exop) == 0)
        {
            PZIP_DUMPBITS(p_t->bits)
            *p_q++ = (UCHAR) p_t->mBase;
            m--;
            continue;
        }

        BOOL loop = true;
         do
        {
            PZIP_DUMPBITS(p_t->bits)

            if (e & 16)
            {
                /* get extra bits for length */
                e &= 15;
                c = p_t->mBase + ((WORD) b & PzipInflateMask[e]);
                PZIP_DUMPBITS(e)

                /* decode distance base of block to copy */
                GRABBITS(15);           /* max bits for distance code */
                e = (p_t = pTd + ((WORD) b & m_d))->exop;

				BOOL loop = true;
                do
                {
                    PZIP_DUMPBITS(p_t->bits)

                    if (e & 16)
                    {
                        /* get extra bits to add to distance base */
                        e &= 15;
                        GRABBITS(e)         /* get extra bits (up to 13) */
                        d = p_t->mBase + ((WORD) b & PzipInflateMask[e]);
                        PZIP_DUMPBITS(e)

                        /* do the copy */
                        m -= c;

                        /* offset before dest */
                        if ((DWORD) (p_q - pS->mpWindow) >= d)
                        {    /*  just copy */
                            p_r = p_q - d;

                            /* minimum count is three, */
                            *p_q++ = *p_r++;
                            c--;

                            /*  so unroll loop a little */
                            *p_q++ = *p_r++;
                            c--;
                        }
                        else            /* else offset after destination */
                        {
                            /* bytes from offset to end */
                            e = d - (WORD) (p_q - pS->mpWindow);

                            p_r = pS->mpEnd - e;      /* pointer to offset */

                            if (c > e)            /* if source crosses, */
                            {
                                c -= e;          /* copy to end of window */

                                do
                                {
                                    *p_q++ = *p_r++;
                                } while (--e);

                                /* copy rest from start of window */
                                p_r = pS->mpWindow;
                            }
                        }

                        do                      /* copy all or what's left */
                        {
                            *p_q++ = *p_r++;
                        } while (--c);

                        break;
                    }
                    else if ((e & 64) == 0)
                    {
                        p_t += p_t->mBase;
                        e = (p_t += ((WORD) b & PzipInflateMask[e]))->exop;
                    }
                    else
                    {
                        UNGRAB
                        PZIP_UPDATE
                        return PZIP_DATA_ERROR;
                    }
                } while (loop);

                break;
            }

            if ((e & 64) == 0)
            {
                p_t += p_t->mBase;

                if ((e = (p_t += ((WORD) b & PzipInflateMask[e]))->exop) == 0)
                {
                    PZIP_DUMPBITS(p_t->bits)
                    *p_q++ = (UCHAR) p_t->mBase;
                    m--;
                    break;
                }
            }
            else if (e & 32)
            {
                UNGRAB
                PZIP_UPDATE
                return PZIP_STREAM_END;
            }
            else
            {
                UNGRAB
                PZIP_UPDATE
                return PZIP_DATA_ERROR;
            }
        } while (loop);

    } while (m >= 258 && n >= 10);

    /* not enough input or output--restore pointers and return */
    UNGRAB
    PZIP_UPDATE
    return PZIP_OK;
}

#endif  // PEG_UNZIP

//
//****************************************************************************
// inflate.c - zlib interface to PzipInflate modules
//****************************************************************************
//

#if defined(PEG_UNZIP)

typedef enum
{
    METHOD,   /* waiting for method byte */
    FLAG,     /* waiting for flag byte */
    DICT_4,    /* four dictionary check bytes to go */
    DICT_3,    /* three dictionary check bytes to go */
    DICT_2,    /* two dictionary check bytes to go */
    DICT_1,    /* one dictionary check byte to go */
    DICT_0,    /* waiting for inflateSetDictionary */
    BLOCKS,   /* decompressing blocks */
    CHECK_4,   /* four check bytes to go */
    CHECK_3,   /* three check bytes to go */
    CHECK_2,   /* two check bytes to go */
    CHECK_1,   /* one check byte to go */
    INFLATE_DONE,     /* finished check, done */
    INFLATE_BAD      /* got an error--stay here */
} InflateMode;

/* PzipInflate private state */
struct InflateInternalState
{
    /* mode */
    InflateMode  mMode;   /* current PzipInflate mode */

    /* mode dependent information */
    union
    {
        WORD mMethod;        /* if FLAGS, method byte */

        struct
        {
            DWORD mWas;                /* computed check value */
            DWORD mNeed;               /* stream check value */
        } mCheck;            /* if CHECK, check values to compare */
    } mSub;        /* submode */

    /* mode independent information */
    LONG mNoWrap;          /* flag for no wrapper */
    WORD mWbits;           /* log2(window size)  (8..15, defaults to 15) */
    PzipInflateBlocksState* mpBlocks;   /* current PzipInflateBlocks state */
};


SIGNED PzipInflateReset( PzipStream* pZ )
{
#define PZ_STATE ((InflateInternalState*) pZ->mpState)

    if (!pZ || !pZ->mpState)
    {
        return PZIP_STREAM_ERROR;
    }

    pZ->mTotalIn = pZ->mTotalOut = 0;

    if (PZ_STATE->mNoWrap)
    {
        PZ_STATE->mMode = BLOCKS;
    }
    else
    {
        PZ_STATE->mMode = METHOD;
    }

    PzipInflateBlocksReset(PZ_STATE->mpBlocks, pZ, NULL );
                          
    return PZIP_OK;

/* #undef PZ_STATE */
}

SIGNED PzipInflateEnd( PzipStream* pZ )
{
#define PZ_STATE ((InflateInternalState*) pZ->mpState)

    if (!pZ || !pZ->mpState)
    {
        return PZIP_STREAM_ERROR;
    }

    if (PZ_STATE->mpBlocks)
    {
        PzipInflateBlocksFree( PZ_STATE->mpBlocks, pZ );
    }

    delete PZ_STATE;
    pZ->mpState = NULL;

    return PZIP_OK;

#undef PZ_STATE
}


#define PZ_STATE ((InflateInternalState*) pZ->mpState)

SIGNED PzipInflateInit( PzipStream* pZ, LONG w, const PEGCHAR*,
                      DWORD streamSize )
{
    if (streamSize != sizeof( PzipStream ))
    {
        return PZIP_VERSION_ERROR;
    }

    /* initialize state */
    if (!pZ)
    {
        return PZIP_STREAM_ERROR;
    }

    InflateInternalState *ps = new InflateInternalState;
    pZ->mpState = (PzipInternalState *) ps;
    //PZ_STATE = new InflateInternalState;

    if (!pZ)
    {
        return PZIP_MEM_ERROR;
    }

    PZ_STATE->mpBlocks = NULL;

    /* handle undocumented nowrap option (no zlib header or check) */
    PZ_STATE->mNoWrap = 0;

    if (w < 0)
    {
        w = - w;
        PZ_STATE->mNoWrap = 1;
    }

    /* set window size */
    if (w < 8 || w > 15)
    {
        PzipInflateEnd( pZ );
        return PZIP_STREAM_ERROR;
    }

    PZ_STATE->mWbits = (WORD) w;

    /* create PzipInflateBlocks state */
    if ((PZ_STATE->mpBlocks = PzipInflateBlocksNew(
            pZ, (PZ_STATE->mNoWrap ? NULL : PzipAdler32), (DWORD) 1 << w )) ==
        NULL)
    {
        PzipInflateEnd( pZ );
        return PZIP_MEM_ERROR;
    }

    /* reset state */
    PzipInflateReset( pZ );
    return PZIP_OK;

#undef PZ_STATE
}

#define NEEDBYTE \
        { \
            if (pZ->mAvailIn == 0) \
            { \
                return r; \
            } \
            r = (SIGNED) pF; \
        }

#define NEEDBYTE_2 \
        { \
            if (pZ->mAvailIn == 0) \
            { \
                return r; \
            } \
        }

#define NEXTBYTE (pZ->mAvailIn--, pZ->mTotalIn++, *pZ->mpNextIn++)

SIGNED PzipInflate( PzipStream* pZ, LONG pF )
{
//#define PZ_STATE ((InflateInternalState*) pZ->mpState)

    SIGNED r;
    WORD b;
    InflateInternalState *pState = (InflateInternalState *) pZ->mpState;

    if (!pZ || !pZ->mpState || !pZ->mpNextIn)
    {
        return PZIP_STREAM_ERROR;
    }

    if (pF == PZIP_FINISH)
    {
        pF = PZIP_BUF_ERROR;
    }
    else
    {
        pF = PZIP_OK;
    }

    r = PZIP_BUF_ERROR;

	BOOL loop = true;
    while (loop)
    {
        switch (pState->mMode)
        {
            case METHOD:
                NEEDBYTE

                if (((pState->mSub.mMethod = NEXTBYTE) & 0xF) !=
                    PZIP_DEFLATED)
                {
                    pState->mMode = INFLATE_BAD;
                    break;
                }

                if ((pState->mSub.mMethod >> 4) + 8 > pState->mWbits)
                {
                    pState->mMode = INFLATE_BAD;
                    break;
                }

                pState->mMode = FLAG;

            case FLAG:
                NEEDBYTE
                b = NEXTBYTE;

                if (((pState->mSub.mMethod << 8) + b) % 31)
                {
                    pState->mMode = INFLATE_BAD;
                    break;
                }

                if (!(b & PZIP_PRESET_DICT))
                {
                    pState->mMode = BLOCKS;
                    break;
                }

                pState->mMode = DICT_4;

            case DICT_4:
                NEEDBYTE
                pState->mSub.mCheck.mNeed = (DWORD) NEXTBYTE << 24;
                pState->mMode = DICT_3;

            case DICT_3:
                NEEDBYTE
                pState->mSub.mCheck.mNeed += (DWORD) NEXTBYTE << 16;
                pState->mMode = DICT_2;

            case DICT_2:
                NEEDBYTE
                pState->mSub.mCheck.mNeed += (DWORD) NEXTBYTE << 8;
                pState->mMode = DICT_1;

            case DICT_1:
                NEEDBYTE_2
                pState->mSub.mCheck.mNeed += (DWORD) NEXTBYTE;
                pZ->mAdler = pState->mSub.mCheck.mNeed;
                pState->mMode = DICT_0;
                return PZIP_NEED_DICT;

            case DICT_0:
                pState->mMode = INFLATE_BAD;
                return PZIP_STREAM_ERROR;

            case BLOCKS:
                r = PzipInflateBlocks( pState->mpBlocks, pZ, r );

                if (r == PZIP_DATA_ERROR)
                {
                    pState->mMode = INFLATE_BAD;
                    break;
                }

                if (r == PZIP_OK)
                {
                    r = (SIGNED) pF;
                }

                if (r != PZIP_STREAM_END)
                {
                    return r;
                }

                r = (SIGNED) pF;
                PzipInflateBlocksReset(
                   pState->mpBlocks, pZ, &pState->mSub.mCheck.mWas );

                if (pState->mNoWrap)
                {
                    pState->mMode = INFLATE_DONE;
                    break;
                }

                pState->mMode = CHECK_4;

            case CHECK_4:
                NEEDBYTE
                pState->mSub.mCheck.mNeed = (DWORD) NEXTBYTE << 24;
                pState->mMode = CHECK_3;

            case CHECK_3:
                NEEDBYTE
                pState->mSub.mCheck.mNeed += (DWORD) NEXTBYTE << 16;
                pState->mMode = CHECK_2;

            case CHECK_2:
                NEEDBYTE
                pState->mSub.mCheck.mNeed += (DWORD) NEXTBYTE << 8;
                pState->mMode = CHECK_1;

            case CHECK_1:
                NEEDBYTE
                pState->mSub.mCheck.mNeed += (DWORD) NEXTBYTE;

                if (pState->mSub.mCheck.mWas != pState->mSub.mCheck.mNeed)
                {
                    pState->mMode = INFLATE_BAD;
                    break;
                }

                pState->mMode = INFLATE_DONE;

            case INFLATE_DONE:
                return PZIP_STREAM_END;

            case INFLATE_BAD:
                return PZIP_DATA_ERROR;

            default:
                return PZIP_STREAM_ERROR;
        }
    }

	return PZIP_STREAM_ERROR;

//#undef PZ_STATE
}

#endif  // PEG_UNZIP

//
//****************************************************************************
// inftrees.c - generate Huffman trees for efficient decoding
//****************************************************************************
//

#if defined(PEG_UNZIP)

/* simplify the use of the PzipInflateHuft type with some defines */
#define exop mWord.mWhat.mExop
#undef bits
#define bits mWord.mWhat.mBits

SIGNED huft_build(
    DWORD*,            /* code lengths in bits */
    DWORD,               /* number of codes */
    DWORD,               /* number of "simple" codes */
    const DWORD*,      /* list of base values for non-simple codes */
    const DWORD*,      /* list of extra bits for non-simple codes */
    PzipInflateHuft**, /* result: starting table */
    DWORD*,            /* maximum lookup bits (returns actual) */
    PzipInflateHuft*,     /* space for trees */
    DWORD*,             /* hufts used in space */
    DWORD* );         /* space for values */

/* Tables for deflate from PKZIP's appnote.txt. */

        /* see note #13 above about 258 */
const DWORD gPzipCpLens[31] = /* Copy lengths for literal codes 257..285 */
{ 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
  35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0 };

const DWORD gPzipCpLext[31] = /* Extra bits for literal codes 257..285 */
{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
  3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 112, 112 }; /* 112==invalid */

const DWORD gPzipCpDist[30] = /* Copy offsets for distance codes 0..29 */
{ 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
  257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
  8193, 12289, 16385, 24577 };

const DWORD gPzipCpDext[30] = /* Extra bits for distance codes */
{ 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
  7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
  12, 12, 13, 13 };

/*
   Huffman code decoding is performed using a multi-level table lookup.
   The fastest way to decode is to simply build a lookup table whose
   size is determined by the longest code.  However, the time it takes
   to build this table can also be a factor if the data being decoded
   is not very long.  The most common codes are necessarily the
   shortest codes, so those codes dominate the decoding time, and hence
   the speed.  The idea is you can have a shorter table that decodes the
   shorter, more probable codes, and then point to subsidiary tables for
   the longer codes.  The time it costs to decode the longer codes is
   then traded against the time it takes to make longer tables.

   This results of this trade are in the variables mLbits and mDbits
   below.  mLbits is the number of bits the first level table for literal/
   length codes can decode in one step, and mDbits is the same thing for
   the distance codes.  Subsequent tables are also less than or equal to
   those sizes.  These values may be adjusted either when all of the
   codes are shorter than that, in which case the longest code length in
   bits is used, or when the shortest code is *longer* than the requested
   table size, in which case the length of the shortest code in bits is
   used.

   There are two different values for the two tables, since they code a
   different number of possibilities each.  The literal/length table
   codes 286 possible values, or in a flat code, a little over eight
   bits.  The distance table codes 30 possible values, or a little less
   than five bits, flat.  The optimum values for speed end up being
   about one bit more than those, so mLbits is 8 + 1 and mDbits is 5 + 1.
   The optimum values may differ though from machine to machine, and
   possibly even between compilers.  Your mileage may vary.
 */

/* If BMAX needs to be larger than 16, then h and x[] should be DWORD. */
#define BMAX 15         /* maximum bit length of any code */

SIGNED huft_build( DWORD* pB, DWORD n, DWORD s, const DWORD* pD,
                 const DWORD* pE, PzipInflateHuft** pT, DWORD* pM,
                 PzipInflateHuft* pHp, DWORD* pHn, DWORD* pV)
/* pB: code lengths in bits (all assumed <= BMAX) */
/* n: number of codes (assumed <= 288) */
/* s: number of simple-valued codes (0 .. s - 1) */
/* pD: list of base values for non-simple codes */
/* pE: list of extra bits for non-simple codes */
/* pT: result: starting table */
/* pM: maximum lookup bits, returns actual */
/* pHp: space for trees */
/* pHn: hufts used in space */
/* pV: working area: values in order of bit length */
/* Given a list of code lengths and a maximum table size, make a set of
   tables to decode that set of codes.  Return PZIP_OK on success,
   Z_BUF_ERROR if the given code set is incomplete (the tables are still built
   in this case), PZIP_DATA_ERROR if the input is invalid (an over-subscribed
   set of lengths), or PZIP_MEM_ERROR if not enough memory. */
{
    DWORD a;                       /* counter for codes of length k */
    DWORD c[BMAX + 1];               /* bit length count table */
    DWORD f;                       /* i repeats in table every f entries */
    LONG g;                        /* maximum code length */
    LONG h;                        /* table level */
    register DWORD i;              /* counter, current code */
    register DWORD j;              /* counter */
    register LONG k;               /* number of bits in current code */
    LONG l;                        /* bits per table (returned in pM) */
    WORD mask;                    /* (1 << w) - 1, to avoid cc -O bug on HP */
    register DWORD* p_p;            /* pointer into c[], pB[], or pV[] */
    PzipInflateHuft* p_q;              /* points to current table */
    PzipInflateHuft r;      /* table entry for structure assignment */
    PzipInflateHuft* u[BMAX];        /* table stack */
    register LONG w;               /* bits before this table == (l * h) */
    DWORD x[BMAX + 1];               /* bit offsets, then code stack */
    DWORD* p_x_p;                    /* pointer into x */
    LONG y;                        /* number of dummy codes added */
    DWORD z;                       /* number of entries in current table */

    /* Generate counts for each bit length */
    p_p = c;

#define C0 *p_p++ = 0;
#define C0_2 *p_p = 0;
#define C2 C0 C0 C0 C0
#define C2_2 C0 C0 C0 C0_2
#define C4 C2 C2 C2 C2
#define C4_2 C2 C2 C2 C2_2

    C4_2                            /* clear c[]--assume BMAX+1 is 16 */
    p_p = pB;
    i = n;

    do
    {
        c[*p_p++]++;                  /* assume all entries <= BMAX */
    } while (--i);

    if (c[0] == n)                /* null input--all zero length codes */
    {
        *pT = (PzipInflateHuft*) NULL;
        *pM = 0;
        return PZIP_OK;
    }

    /* Find minimum and maximum length, bound *pM by those */
    l = *pM;

    for (j = 1; j <= BMAX; j++)
    {
        if (c[j])
        {
            break;
        }
    }

    k = j;                        /* minimum code length */
    if ((WORD) l < (WORD) j)
    {
        l = j;
    }

    for (i = BMAX; i; i--)
    {
        if (c[i])
        {
            break;
        }
    }

    g = i;                        /* maximum code length */

    if ((DWORD) l > i)
    {
        l = i;
    }

    *pM = l;

    /* Adjust last length count to fill out codes, if needed */
    for (y = 1 << j; j < i; j++, y <<= 1)
    {
        if ((y -= c[j]) < 0)
        {
            return PZIP_DATA_ERROR;
        }
    }

    if ((y -= c[i]) < 0)
    {
        return PZIP_DATA_ERROR;
    }

    c[i] += y;

    /* Generate starting offsets into the value table for each length */
    x[1] = j = 0;
    p_p = c + 1;
    p_x_p = x + 2;

    while (--i)                 /* note that i == g from above */
    {
        *p_x_p++ = (j += *p_p++);
    }

    /* Make a table of values in order of bit lengths */
    p_p = pB;
    i = 0;

    do
    {
        if ((j = *p_p++) != 0)
        {
            pV[x[j]++] = i;
        }
    } while (++i < n);

    n = x[g];                     /* set n to length of pV */

    /* Generate the Huffman codes and for each, make the table entries */
    x[0] = i = 0;                 /* first Huffman code is zero */
    p_p = pV;                        /* grab values in bit order */
    h = -1;                       /* no tables yet--level -1 */
    w = -l;                       /* bits decoded == (l * h) */
    u[0] = (PzipInflateHuft*) NULL;        /* just to keep compilers happy */
    p_q = (PzipInflateHuft*) NULL;   /* ditto */
    z = 0;                        /* ditto */

    /* go through the bit lengths (k already is bits in shortest code) */
    for (; k <= g; k++)
    {
        a = c[k];

        while (a--)
        {
            /* here i is the Huffman code of length k bits for value *p_p */
            /* make tables up to required level */
            while (k > w + l)
            {
                h++;
                w += l;                 /* previous table always l bits */

                /* compute minimum size table less than or equal to l bits */
                z = g - w;

                if ((WORD) z > (WORD) l)
                {
                    z = l;  // table size upper limit
                }

                if ((f = 1 << (j = k - w)) > a + 1)  /* try a k-w bit table */
                {                     /* too few codes for k - w bit table */
                    f -= a + 1;         /* deduct codes from patterns left */
                    p_x_p = c + k;

                    if (j < z)
                    {
                        while (++j < z)  /* try smaller tables up to z bits */
                        {
                            if ((f <<= 1) <= *++p_x_p)
                            {
                                break;     /* enough codes to use up j bits */
                            }
                        }

                        f -= *p_x_p;    /* else deduct codes from patterns */
                    }
                }

                z = 1 << j;             /* table entries for j-bit table */

                /* allocate new table */
                if (*pHn + z > MANY)   /* (note: doesn't matter for fixed) */
                {
                    return PZIP_MEM_ERROR;   /* not enough memory */
                }

                u[h] = p_q = pHp + *pHn;
                *pHn += z;

                /* connect to last table, if there is one */
                if (h)
                {
                    x[h] = i;             /* save pattern for backing up */
                    r.bits = (UCHAR) l;  /* bits to dump before this table */
                    r.exop = (UCHAR) j;     /* bits in this table */
                    j = i >> (w - l);

                    // offset to this table
                    r.mBase = (WORD) (p_q - u[h - 1] - j);

                    u[h - 1][j] = r;        /* connect to last table */
                }
                else
                {
                    *pT = p_q;         /* first table is returned result */
                }
            }

            /* set up table entry in r */
            r.bits = (UCHAR) (k - w);

            if (p_p >= pV + n)
            {
                r.exop = 128 + 64;      /* out of values--invalid code */
            }
            else if (*p_p < s)
            {
                if (*p_p < 256) // 256 is end-of-block
                {
                    r.exop = (UCHAR) 0;
                }
                else
                {
                    r.exop = (UCHAR) (32 + 64);
                }

                r.mBase = *p_p++;          /* simple code is just the value */
            }
            else
            {
                /* non-simple--look up in lists */
                r.exop = (UCHAR) (pE[*p_p - s] + 16 + 64);

                r.mBase = pD[*p_p++ - s];
            }

            /* fill code-like entries with r */
            f = 1 << (k - w);

            for (j = i >> w; j < z; j += f)
            {
                p_q[j] = r;
            }

            /* backwards increment the k-bit code i */
            for (j = 1 << (k - 1); i & j; j >>= 1)
            {
                i ^= j;
            }

            i ^= j;

            /* backup over finished tables */
            mask = (WORD) ((1 << w) - 1);      /* needed on HP, cc -O bug */

            while ((i & mask) != x[h])
            {
                h--;                    /* don't need to update p_q */
                w -= l;
                mask = (WORD) ((1 << w) - 1);
            }
        }
    }

    /* Return PZIP_BUF_ERROR if we were given an incomplete table */
    if (y != 0 && g != 1)
    {
        return PZIP_BUF_ERROR;
    }

    return PZIP_OK;
}

SIGNED PzipInflateTreesBits( DWORD* pC, DWORD* pBb, PzipInflateHuft** pTb,
                           PzipInflateHuft* pHp, PzipStream*)
/* pC: 19 code lengths */
/* pBb: bits tree desired/actual depth */
/* pTb: bits tree result */
/* pHp: space for trees */
/* z: for messages */
{
    SIGNED r;
    DWORD h_n = 0;          /* hufts used in space */
    DWORD* p_v;             /* work area for huft_build */

    p_v = new DWORD[19];
    if (!p_v)
    {
        return PZIP_MEM_ERROR;
    }

    r = huft_build( pC, 19, 19, (const DWORD*) NULL, (const DWORD*) NULL,
                    pTb, pBb, pHp, &h_n, p_v );

    if (r != PZIP_DATA_ERROR && (r == PZIP_BUF_ERROR || *pBb == 0))
    {
        r = PZIP_DATA_ERROR;
    }

    delete p_v;
    return r;
}

SIGNED PzipInflateTreesDynamic( DWORD nL, DWORD nD, DWORD* pC, DWORD* pBl,
                              DWORD* pBd, PzipInflateHuft** pTl,
                              PzipInflateHuft** pTd, PzipInflateHuft* pHp,
                              PzipStream*)
/* nL: number of literal/length codes */
/* nD: number of distance codes */
/* pC: that many (total) code lengths */
/* pBl: literal desired/actual bit depth */
/* pBd: distance desired/actual bit depth */
/* pTl: literal/length tree result */
/* pTd: distance tree result */
/* pHp: space for trees */
/* z: for messages */
{
    SIGNED r;
    DWORD h_n = 0;          /* hufts used in space */
    DWORD* p_v;             /* work area for huft_build */

    /* allocate work area */
    p_v = new DWORD[288];
    if (!p_v)
    {
        return PZIP_MEM_ERROR;
    }

    /* build literal/length tree */
    r = huft_build( pC, nL, 257, gPzipCpLens, gPzipCpLext, pTl, pBl, pHp,
                    &h_n, p_v );

    if (r != PZIP_OK || *pBl == 0)
    {
        if (r != PZIP_DATA_ERROR && r != PZIP_MEM_ERROR)
        {
            r = PZIP_DATA_ERROR;
        }

        delete p_v;
        return r;
    }

    /* build distance tree */
    r = huft_build( pC + nL, nD, 0, gPzipCpDist, gPzipCpDext, pTd, pBd, pHp,
                    &h_n, p_v );

    if (r != PZIP_OK || (*pBd == 0 && nL > 257))
    {
        if (r != PZIP_DATA_ERROR && r == PZIP_BUF_ERROR)
        {
            r = PZIP_DATA_ERROR;
        }
        else if (r != PZIP_MEM_ERROR)
        {
            r = PZIP_DATA_ERROR;
        }

        delete p_v;
        return r;
    }

    /* done */
    delete p_v;

    return PZIP_OK;
}

#undef bits

//
//********************************************
// inffixed.h - table for decoding fixed codes
//********************************************
//

WORD gPzipFixedBl = 9;

WORD pPzipFixedBd = 5;

PzipInflateHuft gPzipFixedTl[] =
{
    {{{96,7}},256}, {{{0,8}},80}, {{{0,8}},16}, {{{84,8}},115},
    {{{82,7}},31}, {{{0,8}},112}, {{{0,8}},48}, {{{0,9}},192},
    {{{80,7}},10}, {{{0,8}},96}, {{{0,8}},32}, {{{0,9}},160},
    {{{0,8}},0}, {{{0,8}},128}, {{{0,8}},64}, {{{0,9}},224},
    {{{80,7}},6}, {{{0,8}},88}, {{{0,8}},24}, {{{0,9}},144},
    {{{83,7}},59}, {{{0,8}},120}, {{{0,8}},56}, {{{0,9}},208},
    {{{81,7}},17}, {{{0,8}},104}, {{{0,8}},40}, {{{0,9}},176},
    {{{0,8}},8}, {{{0,8}},136}, {{{0,8}},72}, {{{0,9}},240},
    {{{80,7}},4}, {{{0,8}},84}, {{{0,8}},20}, {{{85,8}},227},
    {{{83,7}},43}, {{{0,8}},116}, {{{0,8}},52}, {{{0,9}},200},
    {{{81,7}},13}, {{{0,8}},100}, {{{0,8}},36}, {{{0,9}},168},
    {{{0,8}},4}, {{{0,8}},132}, {{{0,8}},68}, {{{0,9}},232},
    {{{80,7}},8}, {{{0,8}},92}, {{{0,8}},28}, {{{0,9}},152},
    {{{84,7}},83}, {{{0,8}},124}, {{{0,8}},60}, {{{0,9}},216},
    {{{82,7}},23}, {{{0,8}},108}, {{{0,8}},44}, {{{0,9}},184},
    {{{0,8}},12}, {{{0,8}},140}, {{{0,8}},76}, {{{0,9}},248},
    {{{80,7}},3}, {{{0,8}},82}, {{{0,8}},18}, {{{85,8}},163},
    {{{83,7}},35}, {{{0,8}},114}, {{{0,8}},50}, {{{0,9}},196},
    {{{81,7}},11}, {{{0,8}},98}, {{{0,8}},34}, {{{0,9}},164},
    {{{0,8}},2}, {{{0,8}},130}, {{{0,8}},66}, {{{0,9}},228},
    {{{80,7}},7}, {{{0,8}},90}, {{{0,8}},26}, {{{0,9}},148},
    {{{84,7}},67}, {{{0,8}},122}, {{{0,8}},58}, {{{0,9}},212},
    {{{82,7}},19}, {{{0,8}},106}, {{{0,8}},42}, {{{0,9}},180},
    {{{0,8}},10}, {{{0,8}},138}, {{{0,8}},74}, {{{0,9}},244},
    {{{80,7}},5}, {{{0,8}},86}, {{{0,8}},22}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},118}, {{{0,8}},54}, {{{0,9}},204},
    {{{81,7}},15}, {{{0,8}},102}, {{{0,8}},38}, {{{0,9}},172},
    {{{0,8}},6}, {{{0,8}},134}, {{{0,8}},70}, {{{0,9}},236},
    {{{80,7}},9}, {{{0,8}},94}, {{{0,8}},30}, {{{0,9}},156},
    {{{84,7}},99}, {{{0,8}},126}, {{{0,8}},62}, {{{0,9}},220},
    {{{82,7}},27}, {{{0,8}},110}, {{{0,8}},46}, {{{0,9}},188},
    {{{0,8}},14}, {{{0,8}},142}, {{{0,8}},78}, {{{0,9}},252},
    {{{96,7}},256}, {{{0,8}},81}, {{{0,8}},17}, {{{85,8}},131},
    {{{82,7}},31}, {{{0,8}},113}, {{{0,8}},49}, {{{0,9}},194},
    {{{80,7}},10}, {{{0,8}},97}, {{{0,8}},33}, {{{0,9}},162},
    {{{0,8}},1}, {{{0,8}},129}, {{{0,8}},65}, {{{0,9}},226},
    {{{80,7}},6}, {{{0,8}},89}, {{{0,8}},25}, {{{0,9}},146},
    {{{83,7}},59}, {{{0,8}},121}, {{{0,8}},57}, {{{0,9}},210},
    {{{81,7}},17}, {{{0,8}},105}, {{{0,8}},41}, {{{0,9}},178},
    {{{0,8}},9}, {{{0,8}},137}, {{{0,8}},73}, {{{0,9}},242},
    {{{80,7}},4}, {{{0,8}},85}, {{{0,8}},21}, {{{80,8}},258},
    {{{83,7}},43}, {{{0,8}},117}, {{{0,8}},53}, {{{0,9}},202},
    {{{81,7}},13}, {{{0,8}},101}, {{{0,8}},37}, {{{0,9}},170},
    {{{0,8}},5}, {{{0,8}},133}, {{{0,8}},69}, {{{0,9}},234},
    {{{80,7}},8}, {{{0,8}},93}, {{{0,8}},29}, {{{0,9}},154},
    {{{84,7}},83}, {{{0,8}},125}, {{{0,8}},61}, {{{0,9}},218},
    {{{82,7}},23}, {{{0,8}},109}, {{{0,8}},45}, {{{0,9}},186},
    {{{0,8}},13}, {{{0,8}},141}, {{{0,8}},77}, {{{0,9}},250},
    {{{80,7}},3}, {{{0,8}},83}, {{{0,8}},19}, {{{85,8}},195},
    {{{83,7}},35}, {{{0,8}},115}, {{{0,8}},51}, {{{0,9}},198},
    {{{81,7}},11}, {{{0,8}},99}, {{{0,8}},35}, {{{0,9}},166},
    {{{0,8}},3}, {{{0,8}},131}, {{{0,8}},67}, {{{0,9}},230},
    {{{80,7}},7}, {{{0,8}},91}, {{{0,8}},27}, {{{0,9}},150},
    {{{84,7}},67}, {{{0,8}},123}, {{{0,8}},59}, {{{0,9}},214},
    {{{82,7}},19}, {{{0,8}},107}, {{{0,8}},43}, {{{0,9}},182},
    {{{0,8}},11}, {{{0,8}},139}, {{{0,8}},75}, {{{0,9}},246},
    {{{80,7}},5}, {{{0,8}},87}, {{{0,8}},23}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},119}, {{{0,8}},55}, {{{0,9}},206},
    {{{81,7}},15}, {{{0,8}},103}, {{{0,8}},39}, {{{0,9}},174},
    {{{0,8}},7}, {{{0,8}},135}, {{{0,8}},71}, {{{0,9}},238},
    {{{80,7}},9}, {{{0,8}},95}, {{{0,8}},31}, {{{0,9}},158},
    {{{84,7}},99}, {{{0,8}},127}, {{{0,8}},63}, {{{0,9}},222},
    {{{82,7}},27}, {{{0,8}},111}, {{{0,8}},47}, {{{0,9}},190},
    {{{0,8}},15}, {{{0,8}},143}, {{{0,8}},79}, {{{0,9}},254},
    {{{96,7}},256}, {{{0,8}},80}, {{{0,8}},16}, {{{84,8}},115},
    {{{82,7}},31}, {{{0,8}},112}, {{{0,8}},48}, {{{0,9}},193},
    {{{80,7}},10}, {{{0,8}},96}, {{{0,8}},32}, {{{0,9}},161},
    {{{0,8}},0}, {{{0,8}},128}, {{{0,8}},64}, {{{0,9}},225},
    {{{80,7}},6}, {{{0,8}},88}, {{{0,8}},24}, {{{0,9}},145},
    {{{83,7}},59}, {{{0,8}},120}, {{{0,8}},56}, {{{0,9}},209},
    {{{81,7}},17}, {{{0,8}},104}, {{{0,8}},40}, {{{0,9}},177},
    {{{0,8}},8}, {{{0,8}},136}, {{{0,8}},72}, {{{0,9}},241},
    {{{80,7}},4}, {{{0,8}},84}, {{{0,8}},20}, {{{85,8}},227},
    {{{83,7}},43}, {{{0,8}},116}, {{{0,8}},52}, {{{0,9}},201},
    {{{81,7}},13}, {{{0,8}},100}, {{{0,8}},36}, {{{0,9}},169},
    {{{0,8}},4}, {{{0,8}},132}, {{{0,8}},68}, {{{0,9}},233},
    {{{80,7}},8}, {{{0,8}},92}, {{{0,8}},28}, {{{0,9}},153},
    {{{84,7}},83}, {{{0,8}},124}, {{{0,8}},60}, {{{0,9}},217},
    {{{82,7}},23}, {{{0,8}},108}, {{{0,8}},44}, {{{0,9}},185},
    {{{0,8}},12}, {{{0,8}},140}, {{{0,8}},76}, {{{0,9}},249},
    {{{80,7}},3}, {{{0,8}},82}, {{{0,8}},18}, {{{85,8}},163},
    {{{83,7}},35}, {{{0,8}},114}, {{{0,8}},50}, {{{0,9}},197},
    {{{81,7}},11}, {{{0,8}},98}, {{{0,8}},34}, {{{0,9}},165},
    {{{0,8}},2}, {{{0,8}},130}, {{{0,8}},66}, {{{0,9}},229},
    {{{80,7}},7}, {{{0,8}},90}, {{{0,8}},26}, {{{0,9}},149},
    {{{84,7}},67}, {{{0,8}},122}, {{{0,8}},58}, {{{0,9}},213},
    {{{82,7}},19}, {{{0,8}},106}, {{{0,8}},42}, {{{0,9}},181},
    {{{0,8}},10}, {{{0,8}},138}, {{{0,8}},74}, {{{0,9}},245},
    {{{80,7}},5}, {{{0,8}},86}, {{{0,8}},22}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},118}, {{{0,8}},54}, {{{0,9}},205},
    {{{81,7}},15}, {{{0,8}},102}, {{{0,8}},38}, {{{0,9}},173},
    {{{0,8}},6}, {{{0,8}},134}, {{{0,8}},70}, {{{0,9}},237},
    {{{80,7}},9}, {{{0,8}},94}, {{{0,8}},30}, {{{0,9}},157},
    {{{84,7}},99}, {{{0,8}},126}, {{{0,8}},62}, {{{0,9}},221},
    {{{82,7}},27}, {{{0,8}},110}, {{{0,8}},46}, {{{0,9}},189},
    {{{0,8}},14}, {{{0,8}},142}, {{{0,8}},78}, {{{0,9}},253},
    {{{96,7}},256}, {{{0,8}},81}, {{{0,8}},17}, {{{85,8}},131},
    {{{82,7}},31}, {{{0,8}},113}, {{{0,8}},49}, {{{0,9}},195},
    {{{80,7}},10}, {{{0,8}},97}, {{{0,8}},33}, {{{0,9}},163},
    {{{0,8}},1}, {{{0,8}},129}, {{{0,8}},65}, {{{0,9}},227},
    {{{80,7}},6}, {{{0,8}},89}, {{{0,8}},25}, {{{0,9}},147},
    {{{83,7}},59}, {{{0,8}},121}, {{{0,8}},57}, {{{0,9}},211},
    {{{81,7}},17}, {{{0,8}},105}, {{{0,8}},41}, {{{0,9}},179},
    {{{0,8}},9}, {{{0,8}},137}, {{{0,8}},73}, {{{0,9}},243},
    {{{80,7}},4}, {{{0,8}},85}, {{{0,8}},21}, {{{80,8}},258},
    {{{83,7}},43}, {{{0,8}},117}, {{{0,8}},53}, {{{0,9}},203},
    {{{81,7}},13}, {{{0,8}},101}, {{{0,8}},37}, {{{0,9}},171},
    {{{0,8}},5}, {{{0,8}},133}, {{{0,8}},69}, {{{0,9}},235},
    {{{80,7}},8}, {{{0,8}},93}, {{{0,8}},29}, {{{0,9}},155},
    {{{84,7}},83}, {{{0,8}},125}, {{{0,8}},61}, {{{0,9}},219},
    {{{82,7}},23}, {{{0,8}},109}, {{{0,8}},45}, {{{0,9}},187},
    {{{0,8}},13}, {{{0,8}},141}, {{{0,8}},77}, {{{0,9}},251},
    {{{80,7}},3}, {{{0,8}},83}, {{{0,8}},19}, {{{85,8}},195},
    {{{83,7}},35}, {{{0,8}},115}, {{{0,8}},51}, {{{0,9}},199},
    {{{81,7}},11}, {{{0,8}},99}, {{{0,8}},35}, {{{0,9}},167},
    {{{0,8}},3}, {{{0,8}},131}, {{{0,8}},67}, {{{0,9}},231},
    {{{80,7}},7}, {{{0,8}},91}, {{{0,8}},27}, {{{0,9}},151},
    {{{84,7}},67}, {{{0,8}},123}, {{{0,8}},59}, {{{0,9}},215},
    {{{82,7}},19}, {{{0,8}},107}, {{{0,8}},43}, {{{0,9}},183},
    {{{0,8}},11}, {{{0,8}},139}, {{{0,8}},75}, {{{0,9}},247},
    {{{80,7}},5}, {{{0,8}},87}, {{{0,8}},23}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},119}, {{{0,8}},55}, {{{0,9}},207},
    {{{81,7}},15}, {{{0,8}},103}, {{{0,8}},39}, {{{0,9}},175},
    {{{0,8}},7}, {{{0,8}},135}, {{{0,8}},71}, {{{0,9}},239},
    {{{80,7}},9}, {{{0,8}},95}, {{{0,8}},31}, {{{0,9}},159},
    {{{84,7}},99}, {{{0,8}},127}, {{{0,8}},63}, {{{0,9}},223},
    {{{82,7}},27}, {{{0,8}},111}, {{{0,8}},47}, {{{0,9}},191},
    {{{0,8}},15}, {{{0,8}},143}, {{{0,8}},79}, {{{0,9}},255}
};

PzipInflateHuft gPzipFixedTd[] =
{
    {{{80,5}},1}, {{{87,5}},257}, {{{83,5}},17}, {{{91,5}},4097},
    {{{81,5}},5}, {{{89,5}},1025}, {{{85,5}},65}, {{{93,5}},16385},
    {{{80,5}},3}, {{{88,5}},513}, {{{84,5}},33}, {{{92,5}},8193},
    {{{82,5}},9}, {{{90,5}},2049}, {{{86,5}},129}, {{{192,5}},24577},
    {{{80,5}},2}, {{{87,5}},385}, {{{83,5}},25}, {{{91,5}},6145},
    {{{81,5}},7}, {{{89,5}},1537}, {{{85,5}},97}, {{{93,5}},24577},
    {{{80,5}},4}, {{{88,5}},769}, {{{84,5}},49}, {{{92,5}},12289},
    {{{82,5}},13}, {{{90,5}},3073}, {{{86,5}},193}, {{{192,5}},24577}
};

SIGNED PzipInflateTreesFixed( DWORD* pBl, DWORD* pBd, PzipInflateHuft** pTl,
                            PzipInflateHuft** pTd, PzipStream*)
/* pBl: literal desired/actual bit depth */
/* pBd: distance desired/actual bit depth */
/* pTl: literal/length tree result */
/* pTd: distance tree result */
/* z: for memory allocation */
{
    *pBl = gPzipFixedBl;
    *pBd = pPzipFixedBd;
    *pTl = gPzipFixedTl;
    *pTd = gPzipFixedTd;

    return PZIP_OK;
}

#endif  // PEG_UNZIP

//
//****************************************************************************
// infutil.c - data and routines common to blocks and codes
//****************************************************************************
//

#if defined(PEG_UNZIP)

/* And'ing with PzipInflateMask[n] masks the lower n bits */
WORD PzipInflateMask[17] =
{
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

/* copy as much as possible from the sliding window to the output area */
SIGNED PzipInflateFlush( PzipInflateBlocksState* pS, PzipStream* pZ, SIGNED r )
{
    DWORD n;
    UCHAR* p_p;
    UCHAR* p_q;

    /* local copies of source and destination pointers */
    p_p = pZ->mpNextOut;
    p_q = pS->mpRead;

    /* compute number of bytes to copy as far as end of window */
    if (p_q <= pS->mpWrite)
    {
        n = (WORD) (pS->mpWrite - p_q);
    }
    else
    {
        n = (WORD) (pS->mpEnd - p_q);
    }

    if (n > pZ->mAvailOut)
    {
        n = pZ->mAvailOut;
    }

    if (n && r == PZIP_BUF_ERROR)
    {
        r = PZIP_OK;
    }

    /* update counters */
    pZ->mAvailOut -= n;
    pZ->mTotalOut += n;

    /* update check information */
    if (pS->mCheckFn)
    {
        pZ->mAdler = pS->mCheck = (*pS->mCheckFn)( pS->mCheck, p_q, n );
    }

    /* copy as far as end of window */
    memcpy( p_p, p_q, n );
    p_p += n;
    p_q += n;

    /* see if more to copy at beginning of window */
    if (p_q == pS->mpEnd)
    {
        /* wrap pointers */
        p_q = pS->mpWindow;

        if (pS->mpWrite == pS->mpEnd)
        {
            pS->mpWrite = pS->mpWindow;
        }

        /* compute bytes to copy */
        n = (WORD) (pS->mpWrite - p_q);

        if (n > pZ->mAvailOut)
        {
            n = pZ->mAvailOut;
        }

        if (n && r == PZIP_BUF_ERROR)
        {
            r = PZIP_OK;
        }

        /* update counters */
        pZ->mAvailOut -= n;
        pZ->mTotalOut += n;

        /* update check information */
        if (pS->mCheckFn)
        {
            pZ->mAdler = pS->mCheck = (*pS->mCheckFn)( pS->mCheck, p_q, n );
        }

        /* copy */
        memcpy( p_p, p_q, n );
        p_p += n;
        p_q += n;
    }

    /* update pointers */
    pZ->mpNextOut = p_p;
    pS->mpRead = p_q;

    /* done */
    return r;
}

#endif  // PEG_UNZIP

//
//****************************************************************************
// trees.c - output deflate data using Huffman coding
//****************************************************************************
//

#if defined(PEG_ZIP)

/*
 *  ALGORITHM
 *
 *      The "deflation" process uses several Huffman trees. The more
 *      common source values are represented by shorter bit sequences.
 *
 *      Each code tree is stored in a compressed form which is itself
 * a Huffman encoding of the lengths of all the code strings (in
 * ascending order by source values).  The actual code strings are
 * reconstructed from the lengths in the inflate process, as described
 * in the deflate specification.
 */

/*
 * Constants
 */

/* Bit length codes must not exceed MAX_BL_BITS bits */
#define MAX_BL_BITS 7

/* end of block literal code */
#define END_BLOCK 256

/* repeat previous bit length 3-6 times (2 bits of repeat count) */
#define REP_3_6 16

/* repeat a zero length 3-10 times  (3 bits of repeat count) */
#define REPZ_3_10 17

/* repeat a zero length 11-138 times  (7 bits of repeat count) */
#define REPZ_11_138 18

/* extra bits for each length code */
const LONG gPzipExtraLbits[PZIP_LENGTH_CODES] =
    { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0 };

/* extra bits for each distance code */
const LONG pPzipExtraDbits[PZIP_D_CODES] =
    { 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13 };

/* extra bits for each bit length code */
const LONG gPzipExtraBlBits[PZIP_BL_CODES] =
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7 };

/* The lengths of the bit length codes are sent in order of decreasing
 * probability, to avoid transmitting the lengths for unused bit length codes.
 */
const UCHAR gPzipBlOrder[PZIP_BL_CODES] =
   { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };

/* Number of bits used within mBiBuf. (mBiBuf might be implemented on
 * more than 16 bits on some systems.)
 */
#define Buf_size (8 * 2 * sizeof( UCHAR ))

/*
 * Local data. These are initialized only once.
 */

#define DIST_CODE_LEN 512 /* see definition of array dist_code below */

//
//********
// trees.h
//********
//

const PzipCtData gPzipStaticLtree[PZIP_L_CODES + 2] =
{
{{ 12},{  8}}, {{140},{  8}}, {{ 76},{  8}}, {{204},{  8}}, {{ 44},{  8}},
{{172},{  8}}, {{108},{  8}}, {{236},{  8}}, {{ 28},{  8}}, {{156},{  8}},
{{ 92},{  8}}, {{220},{  8}}, {{ 60},{  8}}, {{188},{  8}}, {{124},{  8}},
{{252},{  8}}, {{  2},{  8}}, {{130},{  8}}, {{ 66},{  8}}, {{194},{  8}},
{{ 34},{  8}}, {{162},{  8}}, {{ 98},{  8}}, {{226},{  8}}, {{ 18},{  8}},
{{146},{  8}}, {{ 82},{  8}}, {{210},{  8}}, {{ 50},{  8}}, {{178},{  8}},
{{114},{  8}}, {{242},{  8}}, {{ 10},{  8}}, {{138},{  8}}, {{ 74},{  8}},
{{202},{  8}}, {{ 42},{  8}}, {{170},{  8}}, {{106},{  8}}, {{234},{  8}},
{{ 26},{  8}}, {{154},{  8}}, {{ 90},{  8}}, {{218},{  8}}, {{ 58},{  8}},
{{186},{  8}}, {{122},{  8}}, {{250},{  8}}, {{  6},{  8}}, {{134},{  8}},
{{ 70},{  8}}, {{198},{  8}}, {{ 38},{  8}}, {{166},{  8}}, {{102},{  8}},
{{230},{  8}}, {{ 22},{  8}}, {{150},{  8}}, {{ 86},{  8}}, {{214},{  8}},
{{ 54},{  8}}, {{182},{  8}}, {{118},{  8}}, {{246},{  8}}, {{ 14},{  8}},
{{142},{  8}}, {{ 78},{  8}}, {{206},{  8}}, {{ 46},{  8}}, {{174},{  8}},
{{110},{  8}}, {{238},{  8}}, {{ 30},{  8}}, {{158},{  8}}, {{ 94},{  8}},
{{222},{  8}}, {{ 62},{  8}}, {{190},{  8}}, {{126},{  8}}, {{254},{  8}},
{{  1},{  8}}, {{129},{  8}}, {{ 65},{  8}}, {{193},{  8}}, {{ 33},{  8}},
{{161},{  8}}, {{ 97},{  8}}, {{225},{  8}}, {{ 17},{  8}}, {{145},{  8}},
{{ 81},{  8}}, {{209},{  8}}, {{ 49},{  8}}, {{177},{  8}}, {{113},{  8}},
{{241},{  8}}, {{  9},{  8}}, {{137},{  8}}, {{ 73},{  8}}, {{201},{  8}},
{{ 41},{  8}}, {{169},{  8}}, {{105},{  8}}, {{233},{  8}}, {{ 25},{  8}},
{{153},{  8}}, {{ 89},{  8}}, {{217},{  8}}, {{ 57},{  8}}, {{185},{  8}},
{{121},{  8}}, {{249},{  8}}, {{  5},{  8}}, {{133},{  8}}, {{ 69},{  8}},
{{197},{  8}}, {{ 37},{  8}}, {{165},{  8}}, {{101},{  8}}, {{229},{  8}},
{{ 21},{  8}}, {{149},{  8}}, {{ 85},{  8}}, {{213},{  8}}, {{ 53},{  8}},
{{181},{  8}}, {{117},{  8}}, {{245},{  8}}, {{ 13},{  8}}, {{141},{  8}},
{{ 77},{  8}}, {{205},{  8}}, {{ 45},{  8}}, {{173},{  8}}, {{109},{  8}},
{{237},{  8}}, {{ 29},{  8}}, {{157},{  8}}, {{ 93},{  8}}, {{221},{  8}},
{{ 61},{  8}}, {{189},{  8}}, {{125},{  8}}, {{253},{  8}}, {{ 19},{  9}},
{{275},{  9}}, {{147},{  9}}, {{403},{  9}}, {{ 83},{  9}}, {{339},{  9}},
{{211},{  9}}, {{467},{  9}}, {{ 51},{  9}}, {{307},{  9}}, {{179},{  9}},
{{435},{  9}}, {{115},{  9}}, {{371},{  9}}, {{243},{  9}}, {{499},{  9}},
{{ 11},{  9}}, {{267},{  9}}, {{139},{  9}}, {{395},{  9}}, {{ 75},{  9}},
{{331},{  9}}, {{203},{  9}}, {{459},{  9}}, {{ 43},{  9}}, {{299},{  9}},
{{171},{  9}}, {{427},{  9}}, {{107},{  9}}, {{363},{  9}}, {{235},{  9}},
{{491},{  9}}, {{ 27},{  9}}, {{283},{  9}}, {{155},{  9}}, {{411},{  9}},
{{ 91},{  9}}, {{347},{  9}}, {{219},{  9}}, {{475},{  9}}, {{ 59},{  9}},
{{315},{  9}}, {{187},{  9}}, {{443},{  9}}, {{123},{  9}}, {{379},{  9}},
{{251},{  9}}, {{507},{  9}}, {{  7},{  9}}, {{263},{  9}}, {{135},{  9}},
{{391},{  9}}, {{ 71},{  9}}, {{327},{  9}}, {{199},{  9}}, {{455},{  9}},
{{ 39},{  9}}, {{295},{  9}}, {{167},{  9}}, {{423},{  9}}, {{103},{  9}},
{{359},{  9}}, {{231},{  9}}, {{487},{  9}}, {{ 23},{  9}}, {{279},{  9}},
{{151},{  9}}, {{407},{  9}}, {{ 87},{  9}}, {{343},{  9}}, {{215},{  9}},
{{471},{  9}}, {{ 55},{  9}}, {{311},{  9}}, {{183},{  9}}, {{439},{  9}},
{{119},{  9}}, {{375},{  9}}, {{247},{  9}}, {{503},{  9}}, {{ 15},{  9}},
{{271},{  9}}, {{143},{  9}}, {{399},{  9}}, {{ 79},{  9}}, {{335},{  9}},
{{207},{  9}}, {{463},{  9}}, {{ 47},{  9}}, {{303},{  9}}, {{175},{  9}},
{{431},{  9}}, {{111},{  9}}, {{367},{  9}}, {{239},{  9}}, {{495},{  9}},
{{ 31},{  9}}, {{287},{  9}}, {{159},{  9}}, {{415},{  9}}, {{ 95},{  9}},
{{351},{  9}}, {{223},{  9}}, {{479},{  9}}, {{ 63},{  9}}, {{319},{  9}},
{{191},{  9}}, {{447},{  9}}, {{127},{  9}}, {{383},{  9}}, {{255},{  9}},
{{511},{  9}}, {{  0},{  7}}, {{ 64},{  7}}, {{ 32},{  7}}, {{ 96},{  7}},
{{ 16},{  7}}, {{ 80},{  7}}, {{ 48},{  7}}, {{112},{  7}}, {{  8},{  7}},
{{ 72},{  7}}, {{ 40},{  7}}, {{104},{  7}}, {{ 24},{  7}}, {{ 88},{  7}},
{{ 56},{  7}}, {{120},{  7}}, {{  4},{  7}}, {{ 68},{  7}}, {{ 36},{  7}},
{{100},{  7}}, {{ 20},{  7}}, {{ 84},{  7}}, {{ 52},{  7}}, {{116},{  7}},
{{  3},{  8}}, {{131},{  8}}, {{ 67},{  8}}, {{195},{  8}}, {{ 35},{  8}},
{{163},{  8}}, {{ 99},{  8}}, {{227},{  8}}
};

const PzipCtData gPzipStaticDtree[PZIP_D_CODES] =
{
{{ 0},{ 5}}, {{16},{ 5}}, {{ 8},{ 5}}, {{24},{ 5}}, {{ 4},{ 5}},
{{20},{ 5}}, {{12},{ 5}}, {{28},{ 5}}, {{ 2},{ 5}}, {{18},{ 5}},
{{10},{ 5}}, {{26},{ 5}}, {{ 6},{ 5}}, {{22},{ 5}}, {{14},{ 5}},
{{30},{ 5}}, {{ 1},{ 5}}, {{17},{ 5}}, {{ 9},{ 5}}, {{25},{ 5}},
{{ 5},{ 5}}, {{21},{ 5}}, {{13},{ 5}}, {{29},{ 5}}, {{ 3},{ 5}},
{{19},{ 5}}, {{11},{ 5}}, {{27},{ 5}}, {{ 7},{ 5}}, {{23},{ 5}}
};

const UCHAR gPzipDistCode[DIST_CODE_LEN] =
{
 0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,
 8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10,
10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0, 16, 17,
18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22,
23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
};

const UCHAR gPzipLengthCode[PZIP_MAX_MATCH - PZIP_MIN_MATCH + 1]=
{
 0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 12,
13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,
19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
};

const LONG gPzipBaseLength[PZIP_LENGTH_CODES] =
{
0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56,
64, 80, 96, 112, 128, 160, 192, 224, 0
};

const LONG gPzipBaseDist[PZIP_D_CODES] =
{
    0,     1,     2,     3,     4,     6,     8,    12,    16,    24,
   32,    48,    64,    96,   128,   192,   256,   384,   512,   768,
 1024,  1536,  2048,  3072,  4096,  6144,  8192, 12288, 16384, 24576
};

struct PzipStaticTreeDescStruct
{
    const PzipCtData* mpStaticTree;  /* static tree or NULL */
    const LONG* mpExtraBits;      /* extra bits for each code or NULL */
    LONG mExtraBase;          /* base index for mpExtraBits */
    SIGNED mElems;               /* max number of elements in the tree */
    LONG mMaxLength;          /* max bit length for the codes */
};

PzipStaticTreeDesc gPzipStaticLdesc =
{ gPzipStaticLtree, gPzipExtraLbits, PZIP_LITERALS + 1, PZIP_L_CODES,
  PZIP_MAX_BITS };

PzipStaticTreeDesc gPzipStaticDdesc =
{ gPzipStaticDtree, pPzipExtraDbits, 0, PZIP_D_CODES, PZIP_MAX_BITS };

PzipStaticTreeDesc gPzipStaticBlDesc =
{ (const PzipCtData*) 0, gPzipExtraBlBits, 0, PZIP_BL_CODES, MAX_BL_BITS };

/*
 * Local (static) routines in this file.
 */

void init_block( PzipDeflateInternalState* pS );

void pqdownheap( PzipDeflateInternalState* pS, PzipCtData* pTree, LONG k );

void gen_bitlen( PzipDeflateInternalState* pS, PzipTreeDesc* pDesc );

void gen_codes( PzipCtData* pTree, LONG maxCode, WORD* pBlCount );

void build_tree( PzipDeflateInternalState* pS, PzipTreeDesc* pDesc );

void scan_tree(
   PzipDeflateInternalState* pS, PzipCtData* pTree, LONG maxCode );

void send_tree(
   PzipDeflateInternalState* pS, PzipCtData* pTree, LONG maxCode );

LONG build_bl_tree( PzipDeflateInternalState* pS );

void send_all_trees(
   PzipDeflateInternalState* pS, LONG lcodes, LONG dcodes, LONG blcodes);

void compress_block(
   PzipDeflateInternalState* pS, PzipCtData* pLtree, PzipCtData* pDtree );

void set_data_type( PzipDeflateInternalState* pS );

WORD bi_reverse( WORD value, LONG length );

void bi_windup( PzipDeflateInternalState* pS );

void bi_flush( PzipDeflateInternalState* pS );

void copy_block(
   PzipDeflateInternalState* pS, PEGCHAR* pBuf, WORD len, LONG header);

/* Send a code of the given tree. c and tree must not have side effects */
#define send_code(s, c, tree) \
        send_bits( s, tree[c].PzipCode, tree[c].PzipLen )

/*
 * Output a short LSB first on the stream.
 * IN assertion: there is enough room in pendingBuf.
 */
#define put_short(s, w) \
        { \
            PzipPutByte( s, (UCHAR) ((w) & 0xFF)); \
            PzipPutByte( s, (UCHAR) ((WORD) (w) >> 8)); \
        }

/*
 * Send a value on a given number of bits.
 * IN assertion: length <= 16 and value fits in length bits.
 */
#define send_bits(s, value, length) \
        { \
            LONG len = length; \
            if (s->mBiValid > (LONG) Buf_size - len) \
            { \
                LONG val = value; \
                s->mBiBuf |= (val << s->mBiValid); \
                put_short( s, s->mBiBuf ); \
                s->mBiBuf = (WORD) (val >> (Buf_size - s->mBiValid)); \
                s->mBiValid += len - Buf_size; \
            } \
            else \
            { \
                s->mBiBuf |= (value) << s->mBiValid; \
                s->mBiValid += len; \
            } \
        }

/* the arguments must not have side effects */
#undef MAX
#define MAX(a,b) (a >= b ? a : b)

/*
 * Initialize the tree data structures for a new zlib stream.
 */
void PzipTrInit( PzipDeflateInternalState* pS )
{
    pS->mLdesc.mpDynTree = pS->mDynLtree;
    pS->mLdesc.mpStatDesc = &gPzipStaticLdesc;

    pS->mDdesc.mpDynTree = pS->mDynDtree;
    pS->mDdesc.mpStatDesc = &gPzipStaticDdesc;

    pS->mBlDesc.mpDynTree = pS->mBlTree;
    pS->mBlDesc.mpStatDesc = &gPzipStaticBlDesc;

    pS->mBiBuf = 0;
    pS->mBiValid = 0;
    pS->mLastEobLen = 8; /* enough lookahead for PzipInflate */

    /* Initialize the first block of the first file: */
    init_block( pS );
}

/*
 * Initialize a new block.
 */
void init_block( PzipDeflateInternalState* pS )
{
    LONG n; /* iterates over tree elements */

    /* Initialize the trees. */

    for (n = 0; n < PZIP_L_CODES; n++)
    {
        pS->mDynLtree[n].PzipFreq = 0;
    }

    for (n = 0; n < PZIP_D_CODES; n++)
    {
        pS->mDynDtree[n].PzipFreq = 0;
    }

    for (n = 0; n < PZIP_BL_CODES; n++)
    {
        pS->mBlTree[n].PzipFreq = 0;
    }

    pS->mDynLtree[END_BLOCK].PzipFreq = 1;
    pS->mOptLen = pS->mStaticLen = 0L;
    pS->mLastLit = pS->mMatches = 0;
}

/* Index within the heap array of least frequent node in the Huffman tree */
#define SMALLEST 1

/*
 * Remove the smallest element from the heap and recreate the heap with
 * one less element. Updates mHeap and mHeapLen.
 */
#define pqremove(s, tree, top) \
        { \
            top = s->mHeap[SMALLEST]; \
            s->mHeap[SMALLEST] = s->mHeap[s->mHeapLen--]; \
            pqdownheap( s, tree, SMALLEST ); \
        }

/*
 * Compares to subtrees, using the tree depth as tie breaker when
 * the subtrees have equal frequency. This minimizes the worst case length.
 */
#define smaller(tree, n, m, depth) \
        (tree[n].PzipFreq < tree[m].PzipFreq || \
         (tree[n].PzipFreq == tree[m].PzipFreq && depth[n] <= depth[m]))

/*
 * Restore the heap property by moving down the tree starting at node k,
 * exchanging a node with the smallest of its two sons if necessary, stopping
 * when the heap property is re-established (each father smaller than its
 * two sons).
 */
void pqdownheap( PzipDeflateInternalState* pS, PzipCtData* pTree, LONG k )
    /* pTree: the tree to restore */
    /* k: node to move down */
{
    SIGNED v = pS->mHeap[k];
    LONG j = k << 1;  /* left son of k */

    while (j <= pS->mHeapLen)
    {
        /* Set j to the smallest of the two sons: */
        if (j < pS->mHeapLen &&
            smaller( pTree, pS->mHeap[j + 1], pS->mHeap[j], pS->mDepth ))
        {
            j++;
        }

        /* Exit if v is smaller than both sons */
        if (smaller( pTree, v, pS->mHeap[j], pS->mDepth ))
        {
            break;
        }

        /* Exchange v with the smallest son */
        pS->mHeap[k] = pS->mHeap[j];
        k = j;

        /* And continue down the tree, setting j to the left son of k */
        j <<= 1;
    }

    pS->mHeap[k] = v;
}

/*
 * Compute the optimal bit lengths for a tree and update the total bit length
 * for the current block.
 * IN assertion: the fields mFreq and mDad are set, mHeap[mHeapMax] and
 *    above are the tree nodes sorted by increasing frequency.
 * OUT assertions: the field mLen is set to the optimal bit length, the
 *     array bl_count contains the frequencies for each bit length.
 *     The length mOptLen is updated; mStaticLen is also updated if stree is
 *     not null.
 */
void gen_bitlen( PzipDeflateInternalState* pS, PzipTreeDesc* pDesc )
    /* pDesc: the tree descriptor */
{
    PzipCtData* p_tree = pDesc->mpDynTree;
    LONG max_code = pDesc->mMaxCode;
    const PzipCtData* p_stree = pDesc->mpStatDesc->mpStaticTree;
    const LONG* p_extra = pDesc->mpStatDesc->mpExtraBits;
    LONG base = pDesc->mpStatDesc->mExtraBase;
    LONG max_length = pDesc->mpStatDesc->mMaxLength;
    LONG h;              /* heap index */
    LONG n, m;           /* iterate over the tree elements */
    LONG bits;           /* bit length */
    LONG xbits;          /* extra bits */
    WORD f;              /* frequency */
    LONG overflow = 0;   /* number of elements with bit length too large */

    for (bits = 0; bits <= PZIP_MAX_BITS; bits++)
    {
        pS->mBlCount[bits] = 0;
    }

    /* In a first pass, compute the optimal bit lengths (which may
     * overflow in the case of the bit length tree).
     */
    p_tree[pS->mHeap[pS->mHeapMax]].PzipLen = 0; /* root of the heap */

    for (h = pS->mHeapMax + 1; h < PZIP_HEAP_SIZE; h++)
    {
        n = pS->mHeap[h];
        bits = p_tree[p_tree[n].PzipDad].PzipLen + 1;

        if (bits > max_length)
        {
            bits = max_length;
            overflow++;
        }

        /* We overwrite p_tree[n].PzipDad which is no longer needed */
        p_tree[n].PzipLen = (WORD) bits;

        if (n > max_code)
        {
            continue; /* not a leaf node */
        }

        pS->mBlCount[bits]++;
        xbits = 0;

        if (n >= base)
        {
            xbits = p_extra[n - base];
        }

        f = p_tree[n].PzipFreq;
        pS->mOptLen += (DWORD) f * (bits + xbits);

        if (p_stree)
        {
            pS->mStaticLen += (DWORD) f * (p_stree[n].PzipLen + xbits);
        }
    }

    if (overflow == 0)
    {
        return;
    }

    /* This happens for example on obj2 and pic of the Calgary corpus */

    /* Find the first bit length which could increase: */
    do
    {
        bits = max_length - 1;

        while (pS->mBlCount[bits] == 0)
        {
            bits--;
        }

        /* The brother of the overflow item also moves one step up,
         * but this does not affect mBlCount[max_length]
         */

        pS->mBlCount[bits]--;      /* move one leaf down the tree */
        pS->mBlCount[bits + 1] += 2; /* move one overflow item as its brother */
        pS->mBlCount[max_length]--;
        overflow -= 2;
    } while (overflow > 0);

    /* Now recompute all bit lengths, scanning in increasing frequency.
     * h is still equal to PZIP_HEAP_SIZE. (It is simpler to reconstruct all
     * lengths instead of fixing only the wrong ones. This idea is taken
     * from 'ar' written by Haruhiko Okumura.)
     */
    for (bits = max_length; bits != 0; bits--)
    {
        n = pS->mBlCount[bits];

        while (n != 0)
        {
            m = pS->mHeap[--h];

            if (m > max_code)
            {
                continue;
            }

            if (p_tree[m].PzipLen != bits)
            {
                pS->mOptLen += ((LONG) bits - (LONG) p_tree[m].PzipLen) *
                   (LONG) p_tree[m].PzipFreq;

                p_tree[m].PzipLen = (WORD)bits;
            }

            n--;
        }
    }
}

/*
 * Generate the codes for a given tree and bit counts (which need not be
 * optimal).
 * IN assertion: the array pBlCount contains the bit length statistics for
 * the given tree and the field mLen is set for all tree elements.
 * OUT assertion: the field code is set for all tree elements of non
 *     zero code length.
 */
void gen_codes( PzipCtData* pTree, LONG maxCode, WORD* pBlCount )
    /* pTree: the tree to decorate */
    /* maxCode: largest code with non zero frequency */
    /* pBlCount: number of codes at each bit length */
{
    WORD next_code[PZIP_MAX_BITS + 1];// next code value for each bit length
    WORD code = 0;              /* running code value */
    LONG bits;                  /* bit index */
    LONG n;                     /* code index */

    /* The distribution counts are first used to generate the code values
     * without bit reversal.
     */

    for (bits = 1; bits <= PZIP_MAX_BITS; bits++)
    {
        next_code[bits] = code = (WORD) ((code + pBlCount[bits - 1]) << 1);
    }

    /* Check that the bit counts in pBlCount are consistent. The last code
     * must be all ones.
     */

    for (n = 0; n <= maxCode; n++)
    {
        LONG len = pTree[n].PzipLen;

        if (len == 0)
        {
            continue;
        }

        /* Now reverse the bits */
        pTree[n].PzipCode = bi_reverse( next_code[len]++, len );
    }
}

/*
 * Construct one Huffman tree and assigns the code bit strings and lengths.
 * Update the total bit length for the current block.
 * IN assertion: the field mFreq is set for all tree elements.
 * OUT assertions: the fields mLen and code are set to the optimal bit length
 *     and corresponding code. The length mOptLen is updated; mStaticLen is
 *     also updated if p_stree is not null. The field max_code is set.
 */
void build_tree( PzipDeflateInternalState* pS, PzipTreeDesc* pDesc )
    /* pDesc: the tree descriptor */
{
    PzipCtData* p_tree = pDesc->mpDynTree;
    const PzipCtData* p_stree = pDesc->mpStatDesc->mpStaticTree;
    SIGNED elems = pDesc->mpStatDesc->mElems;
    SIGNED n, m;          /* iterate over heap elements */
    SIGNED max_code = -1; /* largest code with non zero frequency */
    SIGNED node;          /* new node being created */

    /* Construct the initial heap, with least frequent element in
     * mHeap[SMALLEST]. The sons of mHeap[n] are mHeap[2 * n] and
     * mHeap[2 * n + 1]. mHeap[0] is not used.
     */
    pS->mHeapLen = 0;
    pS->mHeapMax = PZIP_HEAP_SIZE;

    for (n = 0; n < elems; n++)
    {
        if (p_tree[n].PzipFreq != 0)
        {
            pS->mHeap[++(pS->mHeapLen)] = max_code = n;
            pS->mDepth[n] = 0;
        }
        else
        {
            p_tree[n].PzipLen = 0;
        }
    }

    /* The pkzip format requires that at least one distance code exists,
     * and that at least one bit should be sent even if there is only one
     * possible code. So to avoid special checks later on we force at least
     * two codes of non zero frequency.
     */
    while (pS->mHeapLen < 2)
    {
        if (max_code < 2)
        {
            node = pS->mHeap[++(pS->mHeapLen)] = ++max_code;
        }
        else
        {
            node = pS->mHeap[++(pS->mHeapLen)] = 0;
        }

        p_tree[node].PzipFreq = 1;
        pS->mDepth[node] = 0;
        pS->mOptLen--;

        if (p_stree)
        {
            /* node is 0 or 1 so it does not have extra bits */
            pS->mStaticLen -= p_stree[node].PzipLen;
        }
    }
    pDesc->mMaxCode = max_code;

    /* The elements mHeap[mHeapLen / 2 + 1 .. mHeapLen] are leaves of the
     * tree, establish sub-heaps of increasing lengths:
     */
    for (n = (SIGNED) (pS->mHeapLen / 2); n >= 1; n--)
    {
        pqdownheap( pS, p_tree, n );
    }

    /* Construct the Huffman tree by repeatedly combining the least two
     * frequent nodes.
     */

    node = elems;              /* next internal node of the tree */

    do
    {
        pqremove( pS, p_tree, n );  /* n = node of least frequency */
        m = pS->mHeap[SMALLEST]; /* m = node of next least frequency */

        pS->mHeap[--(pS->mHeapMax)] = n;// keep the nodes sorted by frequency
        pS->mHeap[--(pS->mHeapMax)] = m;

        /* Create a new node father of n and m */
        p_tree[node].PzipFreq = (WORD) (p_tree[n].PzipFreq + p_tree[m].PzipFreq);
        pS->mDepth[node] = (UCHAR) (MAX(pS->mDepth[n], pS->mDepth[m]) + 1);
        p_tree[n].PzipDad = p_tree[m].PzipDad = (WORD) node;

        /* and insert the new node in the heap */
        pS->mHeap[SMALLEST] = node++;
        pqdownheap( pS, p_tree, SMALLEST );
    } while (pS->mHeapLen >= 2);

    pS->mHeap[--(pS->mHeapMax)] = pS->mHeap[SMALLEST];

    /* At this point, the fields mFreq and mDad are set. We can now
     * generate the bit lengths.
     */
    gen_bitlen( pS, (PzipTreeDesc*) pDesc );

    /* The field mLen is now set, we can generate the bit codes */
    gen_codes( (PzipCtData*) p_tree, max_code, pS->mBlCount );
}

/*
 * Scan a literal or distance tree to determine the frequencies of the codes
 * in the bit length tree.
 */
void scan_tree( PzipDeflateInternalState* pS, PzipCtData* pTree,
                LONG maxCode )
    /* pTree: the tree to be scanned */
    /* maxCode: and its largest code of non zero frequency */
{
    LONG n;                     /* iterates over all tree elements */
    LONG prevlen = -1;          /* last emitted length */
    LONG curlen;                /* length of current code */
    LONG nextlen = pTree[0].PzipLen; /* length of next code */
    WORD count = 0;             /* repeat count of the current code */
    LONG max_count = 7;         /* max repeat count */
    LONG min_count = 4;         /* min repeat count */

    if (nextlen == 0)
    {
        max_count = 138;
        min_count = 3;
    }

    pTree[maxCode + 1].PzipLen = (WORD) 0xFFFF; /* guard */

    for (n = 0; n <= maxCode; n++)
    {
        curlen = nextlen;
        nextlen = pTree[n + 1].PzipLen;

        if (++count < max_count && curlen == nextlen)
        {
            continue;
        }
        else if (count < min_count)
        {
            pS->mBlTree[curlen].PzipFreq += count;
        }
        else if (curlen != 0)
        {
            if (curlen != prevlen)
            {
                pS->mBlTree[curlen].PzipFreq++;
            }

            pS->mBlTree[REP_3_6].PzipFreq++;
        }
        else if (count <= 10)
        {
            pS->mBlTree[REPZ_3_10].PzipFreq++;
        }
        else
        {
            pS->mBlTree[REPZ_11_138].PzipFreq++;
        }

        count = 0;
        prevlen = curlen;

        if (nextlen == 0)
        {
            max_count = 138;
            min_count = 3;
        }
        else if (curlen == nextlen)
        {
            max_count = 6;
            min_count = 3;
        }
        else
        {
            max_count = 7;
            min_count = 4;
        }
    }
}

/*
 * Send a literal or distance tree in compressed form, using the codes in
 * mBlTree.
 */
void send_tree( PzipDeflateInternalState* pS, PzipCtData* pTree,
                LONG maxCode )
    /* pTree: the tree to be scanned */
    /* maxCode: and its largest code of non zero frequency */
{
    LONG n;                     /* iterates over all tree elements */
    LONG prevlen = -1;          /* last emitted length */
    LONG curlen;                /* length of current code */
    LONG nextlen = pTree[0].PzipLen; /* length of next code */
    LONG count = 0;             /* repeat count of the current code */
    LONG max_count = 7;         /* max repeat count */
    LONG min_count = 4;         /* min repeat count */

    /* pTree[maxCode + 1].PzipLen = -1; */  /* guard already set */
    if (nextlen == 0)
    {
        max_count = 138;
        min_count = 3;
    }

    for (n = 0; n <= maxCode; n++)
    {
        curlen = nextlen;
        nextlen = pTree[n + 1].PzipLen;

        if (++count < max_count && curlen == nextlen)
        {
            continue;
        }
        else if (count < min_count)
        {
            do
            {
                send_code( pS, curlen, pS->mBlTree );
            } while (--count != 0);
        }
        else if (curlen != 0)
        {
            if (curlen != prevlen)
            {
                send_code( pS, curlen, pS->mBlTree );
                count--;
            }

            send_code( pS, REP_3_6, pS->mBlTree );
            send_bits( pS, count - 3, 2 );
        }
        else if (count <= 10)
        {
            send_code( pS, REPZ_3_10, pS->mBlTree );
            send_bits( pS, count - 3, 3 );
        }
        else
        {
            send_code( pS, REPZ_11_138, pS->mBlTree );
            send_bits( pS, count - 11, 7 );
        }

        count = 0;
        prevlen = curlen;

        if (nextlen == 0)
        {
            max_count = 138;
            min_count = 3;
        }
        else if (curlen == nextlen)
        {
            max_count = 6;
            min_count = 3;
        }
        else
        {
            max_count = 7;
            min_count = 4;
        }
    }
}

/*
 * Construct the Huffman tree for the bit lengths and return the index in
 * gPzipBlOrder of the last bit length code to send.
 */
LONG build_bl_tree( PzipDeflateInternalState* pS )
{
    LONG max_blindex;  /* index of last bit length code of non zero mFreq */

    /* Determine the bit length frequencies for literal and distance trees */
    scan_tree( pS, (PzipCtData*) pS->mDynLtree, pS->mLdesc.mMaxCode );
    scan_tree( pS, (PzipCtData*) pS->mDynDtree, pS->mDdesc.mMaxCode );

    /* Build the bit length tree: */
    build_tree( pS, (PzipTreeDesc*) (&(pS->mBlDesc)) );

    /* mOptLen now includes the length of the tree representations, except
     * the lengths of the bit lengths codes and the 5 + 5 + 4 bits for the
     * counts.
     */

    /* Determine the number of bit length codes to send. The pkzip format
     * requires that at least 4 bit length codes be sent. (appnote.txt says
     * 3 but the actual value used is 4.)
     */
    for (max_blindex = PZIP_BL_CODES - 1; max_blindex >= 3; max_blindex--)
    {
        if (pS->mBlTree[gPzipBlOrder[max_blindex]].PzipLen != 0)
        {
            break;
        }
    }

    /* Update mOptLen to include the bit length tree and counts */
    pS->mOptLen += 3*(max_blindex + 1) + 5 + 5 + 4;

    return max_blindex;
}

/*
 * Send the header for a block using dynamic Huffman trees: the counts, the
 * lengths of the bit length codes, the literal tree and the distance tree.
 * IN assertion: lcodes >= 257, dcodes >= 1, blcodes >= 4.
 */
void send_all_trees( PzipDeflateInternalState* pS, LONG lcodes, LONG dcodes,
                     LONG blcodes)
    /* lcodes, dcodes, blcodes: number of codes for each tree */
{
    LONG rank;                    /* index in gPzipBlOrder */

    send_bits( pS, lcodes - 257, 5 ); /* not +255 as stated in appnote.txt */
    send_bits( pS, dcodes - 1, 5 );
    send_bits( pS, blcodes - 4, 4 ); /* not -3 as stated in appnote.txt */

    for (rank = 0; rank < blcodes; rank++)
    {
        send_bits( pS, pS->mBlTree[gPzipBlOrder[rank]].PzipLen, 3 );
    }

    send_tree( pS, (PzipCtData*) pS->mDynLtree, lcodes - 1 );// literal tree

    send_tree( pS, (PzipCtData*) pS->mDynDtree, dcodes - 1 );// distance tree
}

/*
 * Send a stored block
 */
void PzipTrStoredBlock( PzipDeflateInternalState* pS, PEGCHAR* pBuf,
                        DWORD storedLen, LONG eof )
    /* pBuf: input block */
    /* storedLen: length of input block */
    /* eof: true if this is the last block for a file */
{
    send_bits( pS, (PZIP_STORED_BLOCK << 1) + eof, 3 ); /* send block type */
    copy_block( pS, pBuf, (WORD) storedLen, 1 ); /* with header */
}

/*
 * Send one empty static block to give enough lookahead for PzipInflate.
 * This takes 10 bits, of which 7 may remain in the bit buffer.
 * The current PzipInflate code requires 9 bits of lookahead. If the
 * last two codes for the previous block (real code plus EOB) were coded
 * on 5 bits or less, PzipInflate may have only 5+3 bits of lookahead to
 * decode the last real code. In this case we send two empty static blocks
 * instead of one. (There are no problems if the previous block is stored or
 * fixed.)  To simplify the code, we assume the worst case of last real code
 * encoded on one bit only.
 */
void PzipTrAlign( PzipDeflateInternalState* pS )
{
    send_bits( pS, PZIP_STATIC_TREES << 1, 3 );
    send_code( pS, END_BLOCK, gPzipStaticLtree );
    bi_flush( pS );

    /* Of the 10 bits for the empty block, we have already sent
     * (10 - mBiValid) bits. The lookahead for the last real code (before
     * the EOB of the previous block) was thus at least one plus the length
     * of the EOB plus what we have just sent of the empty static block.
     */
    if (1 + pS->mLastEobLen + 10 - pS->mBiValid < 9)
    {
        send_bits( pS, PZIP_STATIC_TREES << 1, 3 );
        send_code( pS, END_BLOCK, gPzipStaticLtree );
        bi_flush( pS );
    }

    pS->mLastEobLen = 7;
}

/*
 * Determine the best encoding for the current block: dynamic trees, static
 * trees or store, and output the encoded block to the zip file.
 */
void PzipTrFlushBlock( PzipDeflateInternalState* pS, PEGCHAR* pBuf,
                       DWORD storedLen, LONG eof )
    /* pBuf: input block, or NULL if too old */
    /* storedLen: length of input block */
    /* eof: true if this is the last block for a file */
{
    DWORD opt_lenb, static_lenb; /* mOptLen and mStaticLen in bytes */
    LONG max_blindex = 0;/* index of last bit length code of non zero mFreq */

    /* Build the Huffman trees unless a stored block is forced */
    if (pS->mLevel > 0)
    {
	    /* Check if the file is ascii or binary */
	    if (pS->mDataType == PZIP_UNKNOWN)
        {
            set_data_type( pS );
        }

	    /* Construct the literal and distance trees */
	    build_tree( pS, (PzipTreeDesc*) (&(pS->mLdesc)) );

	    build_tree( pS, (PzipTreeDesc*) (&(pS->mDdesc)) );

	    /* At this point, mOptLen and mStaticLen are the total bit lengths of
	     * the compressed block data, excluding the tree representations.
	     */

	    /* Build the bit length tree for the above two trees, and get the
         * index in gPzipBlOrder of the last bit length code to send.
	     */
	    max_blindex = build_bl_tree( pS );

	    /* Determine the best encoding. Compute first the block length in
         * bytes
         */
	    opt_lenb = (pS->mOptLen + 3 + 7) >> 3;
	    static_lenb = (pS->mStaticLen + 3 + 7) >> 3;

	    if (static_lenb <= opt_lenb)
        {
            opt_lenb = static_lenb;
        }
    }
    else
    {
	    opt_lenb = static_lenb = storedLen + 5; /* force a stored block */
    }

    /* 4: two words for the lengths */
    if (storedLen + 4 <= opt_lenb && pBuf != (PEGCHAR*) 0)
    {
        /* The test pBuf != NULL is only necessary if LIT_BUFSIZE > WSIZE.
         * Otherwise we can't have processed more than WSIZE input bytes since
         * the last block flush, because compression would have been
         * successful. If LIT_BUFSIZE <= WSIZE, it is never too late to
         * transform a block into a stored block.
         */
        PzipTrStoredBlock( pS, pBuf, storedLen, eof );
    }
    else if (static_lenb == opt_lenb)
    {
        send_bits( pS, (PZIP_STATIC_TREES << 1) + eof, 3 );

        compress_block( pS, (PzipCtData*) gPzipStaticLtree,
                        (PzipCtData*) gPzipStaticDtree );
    }
    else
    {
        send_bits( pS, (PZIP_DYN_TREES << 1) + eof, 3 );

        send_all_trees( pS, pS->mLdesc.mMaxCode + 1, pS->mDdesc.mMaxCode + 1,
                        max_blindex + 1 );

        compress_block( pS, (PzipCtData*) pS->mDynLtree,
                        (PzipCtData*) pS->mDynDtree );
    }
    /* The above check is made mod 2^32, for files larger than 512 MB
     * and DWORD implemented on 32 bits.
     */

    init_block( pS );

    if (eof)
    {
        bi_windup( pS );
    }
}

/*
 * Save the match info and tally the frequency counts. Return true if
 * the current block must be flushed.
 */
LONG PzipTrTally( PzipDeflateInternalState* pS, DWORD dist, DWORD lc )
    /* dist: distance of matched string */
    /* lc: match length - PZIP_MIN_MATCH or unmatched char (if dist == 0) */
{
    pS->mpDbuf[pS->mLastLit] = (WORD) dist;
    pS->mpLbuf[pS->mLastLit++] = (UCHAR) lc;

    if (dist == 0)
    {
        /* lc is the unmatched char */
        pS->mDynLtree[lc].PzipFreq++;
    }
    else
    {
        pS->mMatches++;

        /* Here, lc is the match length - PZIP_MIN_MATCH */

        dist--;             /* dist = match distance - 1 */

        pS->mDynLtree[gPzipLengthCode[lc] + PZIP_LITERALS + 1].PzipFreq++;
        pS->mDynDtree[PzipDcode( dist )].PzipFreq++;
    }

    /* We avoid equality with mLitBufsize because of wraparound at 64K
     * on 16 bit machines and because stored blocks are restricted to
     * 64K-1 bytes.
     */
    return pS->mLastLit == pS->mLitBufsize - 1;
}

/*
 * Send the block data compressed using the given Huffman trees
 */
void compress_block( PzipDeflateInternalState* pS, PzipCtData* pLtree,
                     PzipCtData* pDtree )
    /* pLtree: literal tree */
    /* pDtree: distance tree */
{
    LONG dist;      /* distance of matched string */
    LONG lc;             /* match length or unmatched char (if dist == 0) */
    WORD lx = 0;    /* running index in mpLbuf */
    WORD code;      /* the code to send */
    LONG extra;          /* number of extra bits to send */

    if (pS->mLastLit != 0)
    {
        do
        {
            dist = pS->mpDbuf[lx];
            lc = pS->mpLbuf[lx++];

            if (dist == 0)
            {
                send_code( pS, lc, pLtree ); /* send a literal byte */
            }
            else
            {
                /* Here, lc is the match length - PZIP_MIN_MATCH */
                code = gPzipLengthCode[lc];

                /* send the length code */
                send_code( pS, code + PZIP_LITERALS + 1, pLtree );

                extra = gPzipExtraLbits[code];

                if (extra != 0)
                {
                    lc -= gPzipBaseLength[code];
                    send_bits( pS, lc, extra );// send the extra length bits
                }

                dist--; /* dist is now the match distance - 1 */
                code = PzipDcode( dist );

                send_code( pS, code, pDtree );   /* send the distance code */
                extra = pPzipExtraDbits[code];

                if (extra != 0)
                {
                    dist -= gPzipBaseDist[code];

                    /* send the extra distance bits */
                    send_bits( pS, dist, extra );
                }
            } /* literal or match pair ? */

        // Check that the overlay between mpPendingBuf and mpDbuf + mpLbuf is
        //    ok:
        } while (lx < pS->mLastLit);
    }

    send_code( pS, END_BLOCK, pLtree );
    pS->mLastEobLen = pLtree[END_BLOCK].PzipLen;
}

/*
 * Set the data type to ASCII or BINARY, using a crude approximation:
 * binary if more than 20% of the bytes are <= 6 or >= 128, ascii otherwise.
 * IN assertion: the fields mFreq of mDynLtree are set and the total of all
 * frequencies does not exceed 64K (to fit in an LONG on 16 bit machines).
 */
void set_data_type( PzipDeflateInternalState* pS )
{
    LONG n = 0;
    WORD ascii_freq = 0;
    WORD bin_freq = 0;

    while (n < 7)
    {
        bin_freq += (WORD) (pS->mDynLtree[n++].PzipFreq);
    }

    while (n < 128)
    {
        ascii_freq += (WORD) (pS->mDynLtree[n++].PzipFreq);
    }

    while (n < PZIP_LITERALS)
    {
        bin_freq += (WORD) (pS->mDynLtree[n++].PzipFreq);
    }

    if (bin_freq > (ascii_freq >> 2))
    {
        pS->mDataType = (UCHAR) PZIP_BINARY;
    }
    else
    {
        pS->mDataType = (UCHAR) PZIP_ASCII;
    }
}

/*
 * Reverse the first len bits of a code, using straightforward code (a faster
 * method would use a table)
 * IN assertion: 1 <= len <= 15
 */
WORD bi_reverse( WORD code, LONG len )
    /* code: the value to invert */
    /* len: its bit length */
{
    register WORD res = 0;

    do
    {
        res |= code & 1;
        code >>= 1;
        res <<= 1;
    } while (--len > 0);

    return ((WORD) (res >> 1));
}

/*
 * Flush the bit buffer, keeping at most 7 bits in it.
 */
void bi_flush( PzipDeflateInternalState* pS )
{
    if (pS->mBiValid == 16)
    {
        put_short( pS, pS->mBiBuf );
        pS->mBiBuf = 0;
        pS->mBiValid = 0;
    }
    else if (pS->mBiValid >= 8)
    {
        PzipPutByte( pS, (UCHAR) pS->mBiBuf );
        pS->mBiBuf >>= 8;
        pS->mBiValid -= 8;
    }
}

/*
 * Flush the bit buffer and align the output on a byte boundary
 */
void bi_windup( PzipDeflateInternalState* pS )
{
    if (pS->mBiValid > 8)
    {
        put_short( pS, pS->mBiBuf );
    }
    else if (pS->mBiValid > 0)
    {
        PzipPutByte( pS, (UCHAR) pS->mBiBuf );
    }

    pS->mBiBuf = 0;
    pS->mBiValid = 0;
}

/*
 * Copy a stored block, storing first the length and its
 * one's complement if requested.
 */
void copy_block( PzipDeflateInternalState* pS, PEGCHAR* pBuf, WORD len,
                 LONG header )
    /* pBuf: the input data */
    /* len: its length */
    /* header: true if block header must be written */
{
    bi_windup( pS );        /* align on byte boundary */
    pS->mLastEobLen = 8; /* enough lookahead for PzipInflate */

    if (header)
    {
        put_short( pS, (WORD) len );
        put_short( pS, (WORD) ~len );
    }

    while (len--)
    {
        PzipPutByte( pS, *pBuf++ );
    }
}

#endif  // PEG_ZIP

#endif  // PEG_ZIP || PEG_UNZIP

