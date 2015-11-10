/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  
//    Copyright (c) Swell Software
//                 3022 Lindsay Ln
//                 Port Huron, MI 48060
//  
//    All Rights Reserved
//  
//  Notes:
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
class ImageControlPanel : public PegDialog
{
    public:
        ImageControlPanel();
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            IDB_GIF1 = 1,
            IDB_GIF2,
            IDB_GIF3,
            IDB_GIF4,
            IDB_GIF5,
            IDB_GIF6,
            IDB_GIF7,
            IDB_GIF8,
            IDB_GIF9,
            IDB_GIF10,
            IDB_GIF11,
            IDB_GIF12,
            IDB_GIF13,
            IDB_GIF14,
            IDB_GIF15,
            IDB_GIF16,
            IDB_GIF17,
            IDB_GIF18,

            IDB_BMP1,
            IDB_BMP2,
            IDB_BMP3,
            IDB_BMP4,
            IDB_BMP5,
            IDB_BMP6,
            IDB_BMP7,
            IDB_BMP8,
            IDB_BMP9,
            IDB_BMP10,
            IDB_BMP11,

            IDB_JPG1,
            IDB_JPG2,
            IDB_JPG3,
            IDB_JPG4,
            IDB_JPG5,

            IDB_RESET,
            IDRB_DITHER,
            IDRB_FIXED_PAL,
        };

    private:

        void OpenImageWin(WORD wId, char *Path);
        void CloseImageWindows(void);
        WORD mwXPos;
        WORD mwYPos;
};


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
class BitmapWin : public PegDecoratedWindow
{
    public:
        BitmapWin(SIGNED iLeft, SIGNED iTop, char *Title,
            PegImageConvert *pConvert);
        ~BitmapWin();
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);

    private:
        PegBitmap mpMap;
        void DrawBitmap(void);
        void UpdateBitmap(void);
        WORD mwMapCount;
        WORD mwMapIndex;
        PegBitmap **mpMapList;
        PegBitmap **mpScaledMapList;
        GIF_IMAGE_INFO *mpGifInfo;
        UCHAR muImageType;
};


