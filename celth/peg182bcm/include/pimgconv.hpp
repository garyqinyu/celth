/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pimgconv.cpp - Image conversion base class, used by GIF, BMP, and
//                JPG conversion classes.
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEG_IMAGE_CONVERT_
#define _PEG_IMAGE_CONVERT_

/*--------------------------------------------------------------------------*/
// PIC_FILE_MODE
//
// This defininition is CRITICAL to how the image conversion classes obtain
// the source (raw) image data. If PIC_FILE_MODE is turned on, the conversion
// classes use ANSI C file I/O to read the raw data directly. If PIC_FILE_MODE
// is turned off, the image conversion classes use a "callback" function
// supplied by the user to read image data.
/*--------------------------------------------------------------------------*/
#define PIC_FILE_MODE

#ifdef PIC_FILE_MODE
#include "stdio.h"
#endif

#ifdef PEG_QUANT
class PegQuant;     // forward reference
#endif


/*--------------------------------------------------------------------------*/
// Conversion State- These values are combined in the converter state value,
//   which can be retrieved via the member function State()
/*--------------------------------------------------------------------------*/

#define PIC_IDLE           0x01  // waiting for input data
#define PIC_ERROR          0x02  // an error has occured
#define PIC_HEADER_KNOWN   0x04  // the header (width, height, type) is known
#define PIC_PALETTE_KNOWN  0x08  // the palette is known
#define PIC_ONE_CONV_DONE  0x10  // at least one conversion complete
#define PIC_COMPLETE       0x20  // all conversions are complete

/*--------------------------------------------------------------------------*/
// Conversion Modes- Only one should be set
/*--------------------------------------------------------------------------*/

#define PIC_NO_CONVERT     0x00  // no inline conversion (custom palette mode)
#define PIC_INLINE_DITHER  0x01  // dither on the fly
#define PIC_INLINE_REMAP   0x02  // remap to best color on the fly
#define PIC_RGB_TRANS      0x10  // use RGB transparency
#define PIC_INDEX_TRANS    0x20  // for GIF, use index instead of RGB value
#define PIC_555_MODE       0x40  // for 16bpp, 5:6:5 or 5:5:5 mode?

/*--------------------------------------------------------------------------*/
// Image Types- can be used to upcast PegImgConvert to 
// PegGifConvert, PegBmpConvert, PegJpgConvert
/*--------------------------------------------------------------------------*/
#define PIC_TYPE_BMP  1
#define PIC_TYPE_GIF  2
#define PIC_TYPE_JPG  3
#define PIC_TYPE_PNG  4


/*--------------------------------------------------------------------------*/
// Default conversion values- some of these can be changed at run time
/*--------------------------------------------------------------------------*/
#define	MAXCOLORMAPSIZE		        256
#define PIC_INPUT_BUF_SIZE          560
#define PIC_INPUT_SLEEP_TIME         10
#define FS_SCALE 1024

#define HI_COLORS       650         // actually 65536
#define TRUE_COLORS     1600        // actually 16 million

/*--------------------------------------------------------------------------*/
// Portability macros
/*--------------------------------------------------------------------------*/
#define BitSet(byte, bit)	(((byte) & (bit)) == (bit))
#define	ReadOK(buffer,len) 	(ConsumeImageInput(buffer, len) == len)
#define ReadBytes(buffer, len) ConsumeImageInput(buffer, len)

#if defined(PEGSMX)
#define PIC_SLEEP  count(PIC_INPUT_SLEEP_TIME, ticks, INF);
#elif defined (PEGX)
#define PIC_SLEEP  tx_thread_sleep(PIC_INPUT_SLEEP_TIME);
#else
#define PIC_SLEEP
#endif

/*--------------------------------------------------------------------------*/

class PegPixel  {

    public:
        PegPixel(void);
        PegPixel(LONG r, LONG g, LONG b);
        BOOL operator != (const PegPixel &Pix) const;
        BOOL operator == (const PegPixel &Pix) const;

        LONG Red;
        LONG Green;
        LONG Blue;
};

/*--------------------------------------------------------------------------*/
struct ConversionData {
    PegBitmap *pBitmap;
    LONG lDataSize;
};


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

class PegImageConvert
{
    public:
        PegImageConvert(WORD wId);
        virtual ~PegImageConvert();

        void SetSystemPalette(UCHAR *pPal, WORD wPalSize, BOOL bFast = FALSE);

        void SetMode(WORD wMode)
        {
            mwMode = wMode;
        }

        void SetTransColor(PegPixel Pixel)
        {
            mTransColor = Pixel;
            mwMode |= PIC_RGB_TRANS;
            mwMode &= ~PIC_INDEX_TRANS;
        }

        void SetTransColor(UCHAR uIndex)
        {
            mTransColor.Red = uIndex;
            mwMode &= ~PIC_RGB_TRANS;
            mwMode |= PIC_INDEX_TRANS;
        }

        void SetGrayscale(BOOL bGray)
        {
            mbGrayscale = bGray;
        }

        PegPixel GetTransColor()
        {
            return mTransColor;
        }

        virtual PegPixel GetULCColor(void);
        virtual PegPixel GetPixelColor(UCHAR *pStart, WORD wYPos, WORD wIndex,
                        WORD wImgWidth);
        virtual PegPixel GetPixelColor(UCHAR *pGet, WORD wIndex);
        virtual void DestroyImages(void);
        virtual void RotateImages(SIGNED iRotation);
        WORD GetByteWidthForInternalColorDepth(WORD wImgWidth);
        WORD GetByteWidthForOutputColorDepth(WORD wImgWidth);
        UCHAR GetOutputBitsPerPix(void){return muOutBitsPerPix;}

       #ifdef PEG_QUANT
        virtual void CountColors(PegQuant *pQuant);
       #endif

        WORD ConsumeImageInput(UCHAR *Buffer, WORD wLength);
        BOOL RleEncode(BOOL bForce = FALSE);
        WORD RleEncode(UCHAR *pData, UCHAR *pPut, WORD wWidth);

       #ifdef PIC_FILE_MODE

        virtual BOOL ReadImage(FILE *pFile, SIGNED iCount = 100) = 0;

       #else

        virtual BOOL ReadImage(SIGNED iCount = 100) = 0;
        void SetIdleCallback(BOOL (*pFunc)(WORD, WORD, PegImageConvert *));
        WORD SendData(UCHAR *Get, WORD wSize);
        WORD InputFreeSpace(void);

       #endif

        UCHAR *GetLocalPalette(void){return mpLocalPalette;}
        PEGCHAR *GetErrorString(void){return msError;}
        void Id(WORD wId) {mwId = wId;}
        WORD Id(void) {return mwId;}
        LONG GetDataSize(int iIndex = 0);

        PegBitmap *GetBitmapPointer(int iIndex);
        WORD GetBitmapCount(void) {return mwImageCount;}
        WORD GetState(void) {return mwState;}
        WORD GetRowsConverted(void) {return mwRowsConverted;}
        WORD GetMode(void) {return mwMode;}
        UCHAR ImageType(void) {return muImageType;}
        BOOL DitherBitmap(void);
        BOOL RemapBitmap(void);
        //void Write6BitMapFile(void);

    protected:

        PegBitmap *AddNewBitmapToList(LONG lSize);
        BOOL SetupInlineBuf(WORD wWidth);
        BOOL InitDither(WORD cols);
        void EndDither(void);
        void EndRemapping(PegBitmap *pMap);
        void SendState(WORD wState);
        BOOL InlineMode(void)
        {
            if (mwMode & (PIC_INLINE_DITHER|PIC_INLINE_REMAP))
            {
                return TRUE;
            }
            return FALSE;
        }

        BOOL TransMode(void)
        {
            if (mwMode && (PIC_INDEX_TRANS|PIC_RGB_TRANS))
            {
                return TRUE;
            }
            return FALSE;
        }

        virtual void DitherBitmapRow(UCHAR *pGet, UCHAR *pPut, WORD wWidth);
        virtual void RemapBitmapRow(UCHAR *pGet, UCHAR *pPut, WORD wWidth);

        UCHAR LookupBestColor(PegPixel Pixel, PegPixel *pNewPixel = NULL);
        LONG GetRawPixValue(UCHAR *pData, WORD wIndex);
        PegPixel WriteBestRowPixelValue(UCHAR *pLine, WORD wPixIndex, PegPixel Pixel);
        void WriteTransValue(UCHAR *pPut, WORD wPixIndex);

        void pm_error(PEGCHAR *);
        void pm_message(PEGCHAR *);

        UCHAR *mpInlineReadBuf;     // storage for inline conversion
        UCHAR *mpSystemPalette;
        UCHAR *mpLocalPalette;
        UCHAR muOutBitsPerPix;
        UCHAR muInternalBitsPerPix;
        WORD  mwSysPalSize;
        WORD  mwLocalPalSize;

        LONG mlDataSize;
        LONG *mpThisRerr;
        LONG *mpNextRerr;
        LONG *mpThisGerr;
        LONG *mpNextGerr;
        LONG *mpThisBerr;
        LONG *mpNextBerr;
        WORD mwRowsConverted;
        WORD   mwImageCount;
        WORD   mwState;
        WORD   mwMode;
        WORD   mwId;
        PEGCHAR   msError[82];
        PegPixel mTransColor;
        UCHAR  muImageType;
        BOOL mbFSDir;
        BOOL mbUseFastLookup;
        BOOL mbGrayscale;
        WORD CountDuplicates(UCHAR *Start, WORD wIndex, WORD wWidth);

        ConversionData *mpConversionData;

       #ifdef PIC_FILE_MODE

        FILE *mFile;

       #else

        BOOL (*mpCallBack) (WORD, WORD, PegImageConvert *);
        UCHAR *mpInBuf;
        UCHAR *mpPutInData;
        UCHAR *mpGetInData;
        UCHAR *mpInBufStop;

       #endif
};


#endif








