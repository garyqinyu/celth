/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pbmpconv.hpp - BMP decompressor object.
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
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGBMPCONVERT_
#define _PEGBMPCONVERT_

typedef struct 
{
	WORD 	id;
	DWORD 	file_size;
	WORD  	reserved[2];
	DWORD   image_offset;
	WORD	header_size;
	WORD	not_used;
	DWORD	xres;
	DWORD   yres;
	WORD	numplanes;
	WORD    bits_per_pix;
	DWORD	compression;
	DWORD   bit_map_size;
	DWORD   hor_res;
	DWORD   vert_res;
	DWORD	number_of_colors;
	DWORD   num_sig_colors;
} BmpHeader;

typedef struct
{	
	UCHAR	Blue;  
	UCHAR	Green;
	UCHAR	Red;
	UCHAR	Count;
} BmpPalEntry;


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegBmpConvert : public PegImageConvert
{
    public:

        PegBmpConvert(WORD wId = 0);
        ~PegBmpConvert(void);
        BmpHeader *GetBmpHeader(void) {return &mBmpHeader;}
        virtual PegPixel GetULCColor(void);

       #ifndef PIC_FILE_MODE

        BOOL ReadImage(SIGNED iCount = 1);

       #else

        BOOL ReadImage(FILE *fd, SIGNED iCount = 1);

       #endif

    protected:
        //PIXEL GetPixelColor(UCHAR *pGet, WORD wIndex);
        //virtual void DitherBitmapRow(UCHAR *pGet, UCHAR *pPut, WORD wWidth);

    private:

        void ReadBitmapHeader();
        void ReadBitmapPalette(void);
        BOOL ReadBitmapData(void);
        void ReadBitmapScanLine(UCHAR *pPut, WORD wActual);
        UCHAR GetBitmapPaddingBytes();

       #ifdef PEG_QUANT
        void CountColors(PegQuant *pQuant);
       #endif

       #ifndef PIC_FILE_MODE
        void SkipToBitmapOffset(DWORD dOffset);
       #endif
        
//        void RemapBitmapRow(UCHAR *pGet, UCHAR *pPut, WORD wWidth);

        BmpHeader mBmpHeader;
        DWORD mdBytesRead;
};

#endif



