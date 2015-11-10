/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pgifconv.hpp - GIF decompressor object.
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGGIFCONVERT_
#define _PEGGIFCONVERT_

#define	MAX_LWZ_BITS		         12
#define INTERLACE		0x40
#define LOCALCOLORMAP	0x80
#define GIFWORD(a, b)  	    (((b)<<8)|(a))

/*--------------------------------------------------------------------------*/
// GIFHEAD structure- can be retrieved by application level
//      software during conversion if desired. There is one GIFHEAD
//      structure produced for each .gif file converted.
/*--------------------------------------------------------------------------*/
typedef struct {
	WORD            wWidth;
	WORD        	wHeight;
	WORD        	wColors;
    UCHAR           uBackClrIndex;
    COLORVAL        cBackground;
	UCHAR           uAspectRatio;
    UCHAR           IsGif89;
} GIF_HEADER;


/*--------------------------------------------------------------------------*/
// GIF_IMAGE structure. One of these structures is created for each
// image in a .gif file.
/*--------------------------------------------------------------------------*/
typedef struct {
    SIGNED          xOffset;
    SIGNED          yOffset;
    WORD            wWidth;
    WORD            wHeight;
	WORD            tDelay;
    UCHAR           uHasTrans;
	UCHAR           uTransColor;
	UCHAR           uInputFlag;
	UCHAR           uDisposal;
} GIF_IMAGE_INFO;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegGifConvert : public PegImageConvert
{
    public:

        PegGifConvert(WORD wId = 0);
        ~PegGifConvert(void);
        GIF_HEADER *GetGifHeader(void) {return &mGifHead;}
        GIF_IMAGE_INFO *GetGifInfo(void) {return mpImageInfo;}
        void DestroyImages(void);

       #ifndef PIC_FILE_MODE

        BOOL ReadImage(SIGNED iCount = 10);

       #else

        BOOL ReadImage(FILE *fd, SIGNED iCount = 10);

       #endif

    private:

        SIGNED ReadColorMap(SIGNED number, UCHAR *pPalette);
        SIGNED DoExtension(SIGNED label);
        SIGNED GetDataBlock(UCHAR *buf);
        SIGNED GetCode(SIGNED code_size, SIGNED flag);
        SIGNED LWZReadByte(SIGNED flag, SIGNED input_code_size);
        BOOL ReadImage(SIGNED len, SIGNED height, SIGNED interlace);
        GIF_IMAGE_INFO *CurrentImageInfo(void);

        GIF_HEADER mGifHead;
        BOOL mbZeroDataBlock;
        BOOL mbFresh;

        SIGNED iInfoCount;
	    SIGNED mCurbit;
        SIGNED mLastbit;
        SIGNED mDone;
        SIGNED mLastByte;
    	SIGNED mCodeSize;
        SIGNED mSetCodeSize;
	    SIGNED mMaxCode;
        SIGNED mMaxCodeSize;
    	SIGNED mFirstCode;
        SIGNED mOldCode;
        SIGNED mClearCode;
        SIGNED mEndCode;
        SIGNED mTable[2][(1<< MAX_LWZ_BITS)];
	    SIGNED mStack[(1<<(MAX_LWZ_BITS))*2];
        SIGNED *mpStack;
        UCHAR *pBuf;
        GIF_IMAGE_INFO *mpImageInfo;

};

#endif

