/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  Author: Kenneth G. Maxwell
//  
//    Copyright (c) 1998-1999 Swell Software
//                 3022 Lindsay Ln
//                 Port Huron, MI 48060
//  
//    All Rights Reserved
//  
//  Notes:
//
//  This program demonstrates usage of the PegImageConvert and PegQuant
//  run-time image conversion classes.
//
//  The first class in this file, ImageControlPanel, displays a dialog box
//  allowing you to select how to do the image conversion. Options include
//  dithering or not, and using the default palette or constructing an
//  optimal palette.
//
//  The actual work of reading and decoding an image is contained in the 
//  function OpenImageWin(). This function creates a PegImageConvert objects,
//  tells it to read and convert the image, and then retrieves the output
//  for display in a window. Note that typical run-time usage is simpler
//  than this function, because you know ahead of time what conversion mode
//  you want to use, and you don't need to query dialog selections as this
//  program does.
//
//  This module also contains a class called BitmapWin. This is a handy little
//  window class that sizes itself to fit an incoming bitmap, and also knows
//  how to handle multiple images (i.e. animated GIFs).
//
//  This progam will run in either PIC_FILE_MODE or !PIC_FILE_MODE.
//  When PIC_FILE_MODE is disabled,
//  this program supplies a callback function to the converter that
//  simply reads data from a file. While this defeats the intention of providing
//  data from a seperate task, this at least demonstrates how to
//  set up a callback function and monitor the conversion process.
//
//  When running in PIC_FILE_MODE, the conversion class reads directly
//  from the graphic file. This is the simplest form to use.
//
//  In PIC_FILE_MODE the user can select generating an optimal palette
//  or using a fixed palette. When PIC_FILE_MODE is diabled, the conversion
//  always does an inline conversion to a fixed palette, while the dithering
//  option still applies.
//
//  ** This program assumes hard-coded image names exist, and they must be
//  provided by you.
//
//  This Program does NOT provide a file browser at this time. Instead, image
//  files named "gif1.gif" through "gif14.gif", and "bmp1.bmp" through
//  "bmp9.bmp" should be placed in the IMAGEPATH directory defined below.
//  It is up to you to provide the image files.
//
//  In order to run this program, you should find some image files that you
//  would like to see converted and displayed in PEG, and copy them into 
//  the IMAGEPATH directory. Rename them to match the hard coded names
//  assumed by this program.
//
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "peg.hpp"

#include "imgbrows.hpp"

#define IMAGEPATH "\\peg\\examples\\imgbrows\\"

#ifndef PIC_FILE_MODE

FILE *gpSrc;

#endif


/*--------------------------------------------------------------------------*/
// PegAppInitialize- called by the PEG library during program startup.
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresentation)
{
    pPresentation->Add(new ImageControlPanel());
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
ImageControlPanel::ImageControlPanel() : PegDialog()
{
    PegRect ChildRect;
    PegThing *pChild1;

    AddStatus(PSF_MOVEABLE);

    mReal.Set(135, 116, 409, 400);
    InitClient();
    Add (new PegTitle("Image Control Panel", TF_CLOSEBUTTON));

    ChildRect.Set(232, 320, 396, 362);
    pChild1 = new PegHorzList(ChildRect);
    ((PegWindow *) pChild1)->SetScrollMode(WSM_HSCROLL);
    ChildRect.Set(268, 229, 312, 248);
    pChild1->Add(new PegTextButton(ChildRect, "JPEG5", IDB_JPG5));
    pChild1->Add(new PegTextButton(ChildRect, "JPEG4", IDB_JPG4));
    pChild1->Add(new PegTextButton(ChildRect, "JPEG3", IDB_JPG3));
    pChild1->Add(new PegTextButton(ChildRect, "JPEG2", IDB_JPG2));
    pChild1->Add(new PegTextButton(ChildRect, "JPEG1", IDB_JPG1));
    Add(pChild1);

    ChildRect.Set(232, 270, 396, 312);
    pChild1 = new PegHorzList(ChildRect);
    ((PegWindow *) pChild1)->SetScrollMode(WSM_HSCROLL);
    ChildRect.Set(268, 229, 302, 248);
    pChild1->Add(new PegTextButton(ChildRect, "Gif18", IDB_GIF18));
    pChild1->Add(new PegTextButton(ChildRect, "Gif17", IDB_GIF17));
    pChild1->Add(new PegTextButton(ChildRect, "Gif16", IDB_GIF16));
    pChild1->Add(new PegTextButton(ChildRect, "Gif15", IDB_GIF15));
    pChild1->Add(new PegTextButton(ChildRect, "Gif14", IDB_GIF14));
    pChild1->Add(new PegTextButton(ChildRect, "Gif13", IDB_GIF13));
    pChild1->Add(new PegTextButton(ChildRect, "Gif12", IDB_GIF12));
    pChild1->Add(new PegTextButton(ChildRect, "Gif11", IDB_GIF11));
    pChild1->Add(new PegTextButton(ChildRect, "Gif10", IDB_GIF10));
    pChild1->Add(new PegTextButton(ChildRect, "Gif9", IDB_GIF9));
    pChild1->Add(new PegTextButton(ChildRect, "Gif8", IDB_GIF8));
    pChild1->Add(new PegTextButton(ChildRect, "Gif7", IDB_GIF7));
    pChild1->Add(new PegTextButton(ChildRect, "Gif6", IDB_GIF6));
    pChild1->Add(new PegTextButton(ChildRect, "Gif5", IDB_GIF5));
    pChild1->Add(new PegTextButton(ChildRect, "Gif4", IDB_GIF4));
    pChild1->Add(new PegTextButton(ChildRect, "Gif3", IDB_GIF3));
    pChild1->Add(new PegTextButton(ChildRect, "Gif2", IDB_GIF2));
    pChild1->Add(new PegTextButton(ChildRect, "Gif1", IDB_GIF1));
    Add(pChild1);

    ChildRect.Set(157, 330, 212, 349);
    Add(new PegPrompt(ChildRect, "JPEG"));
    ChildRect.Set(157, 281, 212, 300);
    Add(new PegPrompt(ChildRect, "GIFs"));
    ChildRect.Set(153, 232, 218, 251);
    Add(new PegPrompt(ChildRect, "Bitmaps"));
    ChildRect.Set(272, 142, 398, 207);
    pChild1 = new PegGroup(ChildRect, "Color Mapping");
    ChildRect.Set(281, 184, 377, 203);
    pChild1->Add(new PegRadioButton(ChildRect, "Best Match"));
    ChildRect.Set(281, 160, 370, 179);
    pChild1->Add(new PegRadioButton(ChildRect, "Dither", IDRB_DITHER, FF_RAISED|AF_ENABLED|BF_SELECTED));
    Add(pChild1);

    ChildRect.Set(232, 220, 396, 262);
    pChild1 = new PegHorzList(ChildRect);
    ((PegWindow *) pChild1)->SetScrollMode(WSM_HSCROLL);
    ChildRect.Set(268, 229, 302, 248);
    pChild1->Add(new PegTextButton(ChildRect, "Bmp11", IDB_BMP11));
    pChild1->Add(new PegTextButton(ChildRect, "Bmp10", IDB_BMP10));
    pChild1->Add(new PegTextButton(ChildRect, "Bmp9", IDB_BMP9));
    pChild1->Add(new PegTextButton(ChildRect, "Bmp8", IDB_BMP8));
    pChild1->Add(new PegTextButton(ChildRect, "Bmp7", IDB_BMP7));
    pChild1->Add(new PegTextButton(ChildRect, "Bmp6", IDB_BMP6));
    pChild1->Add(new PegTextButton(ChildRect, "Bmp5", IDB_BMP5));
    pChild1->Add(new PegTextButton(ChildRect, "Bmp4", IDB_BMP4));
    pChild1->Add(new PegTextButton(ChildRect, "Bmp3", IDB_BMP3));
    pChild1->Add(new PegTextButton(ChildRect, "Bmp2", IDB_BMP2));
    pChild1->Add(new PegTextButton(ChildRect, "Bmp1", IDB_BMP1));
    Add(pChild1);
    Add(pChild1);

    ChildRect.Set(289, 370, 368, 391);
    Add(new PegTextButton(ChildRect, "Exit", IDB_CLOSE));
    ChildRect.Set(169, 370, 248, 391);
    Add(new PegTextButton(ChildRect, "Reset", IDB_RESET));
    ChildRect.Set(142, 142, 257, 207);
    pChild1 = new PegGroup(ChildRect, "Palette Mode");
    ChildRect.Set(152, 184, 246, 203);

   #ifdef PIC_FILE_MODE
    pChild1->Add(new PegRadioButton(ChildRect, "Optimal Palette"));
   #else
    pChild1->Add(new PegRadioButton(ChildRect, "Optimal Palette", 0, 0));
   #endif

    ChildRect.Set(152, 160, 237, 179);
    pChild1->Add(new PegRadioButton(ChildRect, "Fixed Palette", IDRB_FIXED_PAL, FF_RAISED|AF_ENABLED|BF_SELECTED));
    Add(pChild1);

    mwXPos = mClient.wLeft + 10;
    mwYPos = mClient.wTop + 10;
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
SIGNED ImageControlPanel::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(IDB_BMP1, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP1, "bmp1.bmp");
        break;

    case SIGNAL(IDB_BMP2, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP2, "bmp2.bmp");
        break;

    case SIGNAL(IDB_BMP3, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP3, "bmp3.bmp");
        break;

    case SIGNAL(IDB_BMP4, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP4, "bmp4.bmp");
        break;

    case SIGNAL(IDB_BMP5, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP5, "bmp5.bmp");
        break;

    case SIGNAL(IDB_BMP6, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP6, "bmp6.bmp");
        break;

    case SIGNAL(IDB_BMP7, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP7, "bmp7.bmp");
        break;

    case SIGNAL(IDB_BMP8, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP8, "bmp8.bmp");
        break;

    case SIGNAL(IDB_BMP9, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP9, "bmp9.bmp");
        break;

    case SIGNAL(IDB_BMP10, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP10, "bmp10.bmp");
        break;

    case SIGNAL(IDB_BMP11, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_BMP11, "bmp11.bmp");
        break;

    case SIGNAL(IDB_GIF1, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF1, "gif1.gif");
        break;

    case SIGNAL(IDB_GIF2, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF2, "gif2.gif");
        break;

    case SIGNAL(IDB_GIF3, PSF_CLICKED):
        OpenImageWin(IDB_GIF3, "gif3.gif");
        // Enter your code here:
        break;

    case SIGNAL(IDB_GIF4, PSF_CLICKED):
        OpenImageWin(IDB_GIF4, "gif4.gif");
        // Enter your code here:
        break;

    case SIGNAL(IDB_GIF5, PSF_CLICKED):
        OpenImageWin(IDB_GIF5, "gif5.gif");
        // Enter your code here:
        break;

    case SIGNAL(IDB_GIF6, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF6, "gif6.gif");
        break;

    case SIGNAL(IDB_GIF7, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF7, "gif7.gif");
        break;

    case SIGNAL(IDB_GIF8, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF8, "gif8.gif");
        break;

    case SIGNAL(IDB_GIF9, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF9, "gif9.gif");
        break;

    case SIGNAL(IDB_GIF10, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF10, "gif10.gif");
        break;

    case SIGNAL(IDB_GIF11, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF11, "gif11.gif");
        break;

    case SIGNAL(IDB_GIF12, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF12, "gif12.gif");
        break;

    case SIGNAL(IDB_GIF13, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF13, "gif13.gif");
        break;

    case SIGNAL(IDB_GIF14, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF14, "gif14.gif");
        break;

    case SIGNAL(IDB_GIF15, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF15, "gif15.gif");
        break;

    case SIGNAL(IDB_GIF16, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF16, "gif16.gif");
        break;

    case SIGNAL(IDB_GIF17, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF17, "gif17.gif");
        break;

    case SIGNAL(IDB_GIF18, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_GIF18, "gif18.gif");
        break;

    case SIGNAL(IDB_JPG1, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_JPG1, "jpeg1.jpg");
        break;

    case SIGNAL(IDB_JPG2, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_JPG2, "jpeg2.jpg");
        break;

    case SIGNAL(IDB_JPG3, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_JPG3, "jpeg3.jpg");
        break;

    case SIGNAL(IDB_JPG4, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_JPG4, "jpeg4.jpg");
        break;

    case SIGNAL(IDB_JPG5, PSF_CLICKED):
        // Enter your code here:
        OpenImageWin(IDB_JPG5, "jpeg5.jpg");
        break;

    case SIGNAL(IDB_CLOSE, PSF_CLICKED):
    case PM_CLOSE:
        // Enter your code here:
        CloseImageWindows();
        PegDialog::Message(Mesg);
        break;

    case SIGNAL(IDB_RESET, PSF_CLICKED):
        // Enter your code here:
        CloseImageWindows();

        // re-initialize the palette:

        Screen()->ResetPalette();
        mwXPos = mClient.wLeft + 10;
        mwYPos = mClient.wTop + 10;
        break;

    default:
        return PegDialog::Message(Mesg);
    }
    return 0;
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void ImageControlPanel::CloseImageWindows(void)
{
    // Close all windows except this one:

    PegThing *pThing = Presentation()->First();

    while(pThing)
    {
        if (pThing != this)
        {
            Destroy(pThing);
            pThing = Presentation()->First();
        }
        else
        {
            pThing = pThing->Next();
        }
    }
}

#ifndef PIC_FILE_MODE
SIGNED giBytesPushed = 0;
/*----------------------------------------------------------------------------*/
// This is the callback function we supply to the converter when PIC_FILE_MODE
// is disabled. In this mode, the converter calls us back when it needs more
// data or the status changes. In this case we simply read more data from the
// source file. The intention, of course, is that the data source can be a
// completely seperate task, such as a TCP/IP task.
/*----------------------------------------------------------------------------*/
BOOL ReadImageData(WORD wId, WORD wState, PegImageConvert *pConvert)
{
    if (wState & PIC_IDLE)  // need more data?
    {
        UCHAR uBuf[500];
        SIGNED iSize = fread(uBuf, 1, 500, gpSrc);
        if (iSize > 0)
        {
            giBytesPushed += pConvert->SendData(uBuf, iSize);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (wState & PIC_COMPLETE) // all done?
        {
            // add a window to display the graphic:

            PegThing *pt = NULL;
            pt->Presentation()->Add(new BitmapWin(20, 20, "Unknown", 
                pConvert));

            fclose (gpSrc);
            delete pConvert;
            return FALSE;
        }
        else
        {
            if (wState & PIC_ERROR)  // was there an error?
            {
                PegMessageWindow *mw = new PegMessageWindow("Error",
                    pConvert->GetErrorString(),
                    MW_OK|FF_RAISED);

                mw->Presentation()->Center(mw);
                mw->Presentation()->Add(mw);
                mw->Execute();
                return FALSE;
            }
        }
    }
    return TRUE;
}

#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void ImageControlPanel::OpenImageWin(WORD wId, char *Path)
{
    DWORD wPalSize;
    UCHAR *pPalette;
    PegRadioButton *pFixedPal;
    PegImageConvert *pConvert;

    PegRadioButton *pDither;

    pFixedPal = (PegRadioButton *) Find(IDRB_FIXED_PAL);

    if (!pFixedPal->IsSelected())
    {
        CloseImageWindows();
    }

   #ifdef IMAGEPATH

    char sFullName[120];
    strcpy(sFullName, IMAGEPATH);
    strcat(sFullName, Path);
    FILE *pSrc = fopen(sFullName, "rb");

   #else

    FILE *pSrc = fopen(Path, "rb");

   #endif


    if (pSrc)
    {
       #ifdef PIC_FILE_MODE

        // Put up the "processing" message:

        PegMessageWindow *pWin = new PegMessageWindow("Please Wait..",
            "Processing Image File...", FF_RAISED);
        Center(pWin);
        Presentation()->Add(pWin);

        if (wId <= IDB_GIF18)
        {
            pConvert = new PegGifConvert();
        }
        else
        {
            if (wId <= IDB_BMP11)
            {
                pConvert = new PegBmpConvert();
            }
            else
            {
                pConvert = new PegJpgConvert();
            }
        }

        pDither = (PegRadioButton *) Find(IDRB_DITHER);
        pPalette = Screen()->GetPalette(&wPalSize);

        if (wPalSize > 256)
        {
            if (wPalSize == 65535)
            {
                pConvert->SetSystemPalette(NULL, HI_COLORS);
            }
            else
            {
                pConvert->SetSystemPalette(NULL, TRUE_COLORS);
            }
        }
        else
        {
            if (pFixedPal->IsSelected())
            {
                // pick either INLINE_DITHER or INLINE_REMAP mode:

                if (pDither->IsSelected())
                {
                    pConvert->SetMode(PIC_INLINE_DITHER);
                }
                else
                {
                    pConvert->SetMode(PIC_INLINE_REMAP);
                }
                if (wPalSize == 232)
                {
                    pConvert->SetSystemPalette(pPalette, (WORD) wPalSize, TRUE);
                }
                else
                {
                    pConvert->SetSystemPalette(pPalette, (WORD) wPalSize, FALSE);
                }
                #ifdef GRAYSCALE
                pConvert->SetGrayscale(TRUE);
                #else
                if (wPalSize <= 4)
                {
                    pConvert->SetGrayscale(TRUE);
                }
                #endif
            }
        }

        // read and convert the image:

        if (!(pConvert->ReadImage(pSrc)))
        {
            Destroy(pWin);

            PegMessageWindow *pMesgWin = new PegMessageWindow("Error",
                "Bad or corrupt source file", FF_RAISED|MW_OK);
            Presentation()->Center(pMesgWin);
            Presentation()->Add(pMesgWin);
            pMesgWin->Execute();
            return;
        }

        if (wPalSize > 256)
        {
            pConvert->RemapBitmap();
        }

        if (!pFixedPal->IsSelected() && wPalSize <= 256)
        {
            PegQuant *pQuant = new PegQuant;
            pConvert->CountColors(pQuant);
            WORD wPalSize = pQuant->ReduceColors();
            Screen()->SetPalette(0, wPalSize, pQuant->GetPalette());
            pConvert->SetSystemPalette(pQuant->GetPalette(), wPalSize, FALSE);

            if (pDither->IsSelected())
            {
                pConvert->DitherBitmap();
            }
            else
            {
                pConvert->RemapBitmap();
            }
        }

        // add a window to display the image(s):

        Presentation()->Add(new BitmapWin(mwXPos, mwYPos, Path, pConvert));

        mwXPos += 10;
        mwYPos += 10;
        fclose (pSrc);
        delete pConvert;
        Destroy(pWin);

       #else    // not PIC_FILE_MODE, we have to supply data

        gpSrc = pSrc;

        if (wId <= IDB_GIF18)
        {
            pConvert = new PegGifConvert();
        }
        else
        {
            if (wId <= IDB_BMP11)
            {
                pConvert = new PegBmpConvert();
            }
            else
            {
                pConvert = new PegJpgConvert();
            }
        }

        pDither = (PegRadioButton *) Find(IDRB_DITHER);
        pPalette = Screen()->GetPalette(&wPalSize);

        if (wPalSize > 256)
        {
            if (wPalSize == 65535)
            {
                pConvert->SetSystemPalette(NULL, HI_COLORS);
            }
            else
            {
                pConvert->SetSystemPalette(NULL, TRUE_COLORS);
            }
        }
        else
        {
            if (pFixedPal->IsSelected())
            {
                // pick either INLINE_DITHER or INLINE_REMAP mode:

                if (pDither->IsSelected())
                {
                    pConvert->SetMode(PIC_INLINE_DITHER);
                }
                else
                {
                    pConvert->SetMode(PIC_INLINE_REMAP);
                }
                if (wPalSize == 232)
                {
                    pConvert->SetSystemPalette(pPalette, (WORD) wPalSize, TRUE);
                }
                else
                {
                    pConvert->SetSystemPalette(pPalette, (WORD) wPalSize, FALSE);
                }
            }
        }

        pConvert->SetIdleCallback(ReadImageData);
        pConvert->ReadImage();
       #endif       // PIC_FILE_MODE if

    }
    else
    {
        PegMessageWindow *pWin = new PegMessageWindow("File Error!", 
            "Unable to open source file.", FF_RAISED|MW_OK);
        Presentation()->Add(pWin);
        pWin->Execute();
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BitmapWin::BitmapWin(SIGNED xPos, SIGNED yPos, char *Path,
    PegImageConvert *pConvert) : PegDecoratedWindow(FF_THIN)
{
    Add(new PegTitle(Path, 0));
    AddStatus(PSF_MOVEABLE|PSF_SIZEABLE);
    SetColor(PCI_NORMAL, BLACK);
    PegRect NewSize = mReal;

    // retrieve data pointers from the conversion object:

    mpMapList = pConvert->GetBitmapList();
    mwMapCount = pConvert->GetBitmapCount();
    muImageType = pConvert->ImageType();

    if (mwMapCount && (muImageType != PIC_TYPE_GIF || mwMapCount == 1))
    {
        mpScaledMapList = new PegBitmap *[mwMapCount];

        for (WORD wLoop = 0; wLoop < mwMapCount; wLoop++)
        {
            mpScaledMapList[wLoop] = NULL;
        }
    }
    else
    {
        mpScaledMapList = NULL;
    }


    if (muImageType == PIC_TYPE_GIF)
    {
        // retrive GIF_IMAGE_INFO structures:
        PegGifConvert *pGif = (PegGifConvert *) pConvert;
        mpGifInfo = pGif->GetGifInfo();
        SetColor(PCI_NORMAL, pGif->GetGifHeader()->cBackground);
    }
    else
    {
        mpGifInfo = NULL;
    }

    SIGNED iAdjust = mpMapList[0]->wWidth - mClient.Width();
    NewSize.wRight += iAdjust + 1;
    iAdjust = mpMapList[0]->wHeight - mClient.Height();
    NewSize.wBottom += iAdjust + 1;
    NewSize.MoveTo(xPos, yPos);
    Resize(NewSize);
    mwMapIndex = 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BitmapWin::~BitmapWin()
{
    WORD wLoop;

    if (mpMapList && mwMapCount)
    {
        for (wLoop = 0; wLoop < mwMapCount; wLoop++)
        {
            if (mpMapList[wLoop]->pStart)
            {
                delete mpMapList[wLoop]->pStart;
            }
        }
        delete [] mpMapList;
    }

    if (mpScaledMapList && mwMapCount)
    {
        for (wLoop = 0; wLoop < mwMapCount; wLoop++)
        {
            if (mpScaledMapList[wLoop])
            {
                if (mpScaledMapList[wLoop]->pStart)
                {
                    delete mpScaledMapList[wLoop]->pStart;
                }
                delete mpScaledMapList[wLoop];
            }
        }
        delete mpScaledMapList;
    }

    if (mpGifInfo)
    {
        delete mpGifInfo;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED BitmapWin::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case PM_SHOW:
        if (mwMapCount > 1)
        {
            SetTimer(1, ONE_SECOND / 4, ONE_SECOND / 4);
        }
        PegDecoratedWindow::Message(Mesg);
        break;

    case PM_SIZE:
        PegDecoratedWindow::Message(Mesg);
        if (StatusIs(PSF_VISIBLE) && mwMapCount &&
            (!mpGifInfo || mwMapCount == 1))
        {
            for (WORD wLoop = 0; wLoop < mwMapCount; wLoop++)
            {
                if (mpMapList[wLoop])
                {
                    if (mpScaledMapList[wLoop])
                    {
                        delete mpScaledMapList[wLoop]->pStart;
                        delete mpScaledMapList[wLoop];
                        mpScaledMapList[wLoop] = NULL;
                    }
                    mpScaledMapList[wLoop] = Screen()->ResizeImage(mpMapList[wLoop],
                        mClient.Width(), mClient.Height());
                }
            }
        }
        break;

    case PM_TIMER:
        if (mwMapCount > 0)
        {
            mwMapIndex++;

            if (mwMapIndex >= mwMapCount)
            {
                mwMapIndex = 0;
            }
            Invalidate();
            UpdateBitmap();
        }
        break;

    case PM_HIDE:
        KillTimer(1);
        PegDecoratedWindow::Message(Mesg);
        break;

    default:
        return PegDecoratedWindow::Message(Mesg);
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void BitmapWin::Draw(void)
{
    BeginDraw();
    PegDecoratedWindow::Draw();
    DrawBitmap();
    EndDraw();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void BitmapWin::DrawBitmap(void)
{
    if (mwMapCount)
    {
        BeginDraw();
        PegPoint PutMap;
        PutMap.x = mClient.wLeft;
        PutMap.y = mClient.wTop;
        PegBitmap *pGetMap = mpMapList[mwMapIndex];

        if (mpGifInfo)
        {
            PutMap.x += mpGifInfo[mwMapIndex].xOffset;
            PutMap.y += mpGifInfo[mwMapIndex].yOffset;
        }
        if (mpScaledMapList)
        {
            if (mpScaledMapList[mwMapIndex])
            {
                pGetMap = mpScaledMapList[mwMapIndex];
            }
        }

        Bitmap(PutMap, pGetMap);
        EndDraw();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void BitmapWin::UpdateBitmap(void)
{
    PegColor Color(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    PegPoint PutMap;

    if (mwMapCount)
    {
        BeginDraw();

        if (mpGifInfo)
        {
            if (mwMapIndex)
            {
                // use the Disposal value to determine whether or not
                // to clear the background:

                switch(mpGifInfo[mwMapIndex - 1].uDisposal)
                {
                case 1:         // the 'do nothing' value
                    break;

                case 4:         // overwrite with previous value
                    PutMap.x = mClient.wLeft;
                    PutMap.y = mClient.wTop;
                    PutMap.x += mpGifInfo[mwMapIndex - 1].xOffset;
                    PutMap.y += mpGifInfo[mwMapIndex - 1].yOffset;    
                    Bitmap(PutMap, mpMapList[mwMapIndex - 1]);
                    break;
                    
                case 2:
                default:
                    // overwrite with background color:
                    Rectangle(mClient, Color, 0);
                }
            }
            else
            {
                Rectangle(mClient, Color, 0);
            }
        }
        PutMap.x = mClient.wLeft;
        PutMap.y = mClient.wTop;

        if (mpGifInfo)
        {
            PutMap.x += mpGifInfo[mwMapIndex].xOffset;
            PutMap.y += mpGifInfo[mwMapIndex].yOffset;
        }
        Bitmap(PutMap, mpMapList[mwMapIndex]);
        EndDraw();
    }
}

