/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vfondemo.cpp - Vector font demo window.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-1999 Swell Software 
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
#include "vfondemo.hpp"
#include "stdio.h"


/*--------------------------------------------------------------------------*/
// PegAppInitialize- called by the PEG library during program startup.
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresentation)
{
	// create the dialog and add it to PegPresentationManager:

	PegRect Rect;
	Rect.Set(0, 0, 400, 320);
	FontWin *pWin = new FontWin(Rect);
	pPresentation->Center(pWin);
	pPresentation->Add(pWin);
}

/*--------------------------------------------------------------------------*/
PegMenuDescription FontStyles[] = {
{"Italic", IDB_ITALIC_FONT, AF_ENABLED|BF_CHECKABLE, NULL},
{"Bold",   IDB_BOLD_FONT, AF_ENABLED|BF_CHECKABLE, NULL},
{"", 0, 0, NULL}
};


/*--------------------------------------------------------------------------*/
PegMenuDescription FontSizes[] = {
{"40", IDB_FS_40, AF_ENABLED|BF_DOTABLE, NULL},
{"36", IDB_FS_36, AF_ENABLED|BF_DOTABLE, NULL},
{"30", IDB_FS_30, AF_ENABLED|BF_DOTABLE, NULL},
{"24", IDB_FS_24, AF_ENABLED|BF_DOTABLE, NULL},
{"20", IDB_FS_20, AF_ENABLED|BF_DOTABLE, NULL},
{"18", IDB_FS_18, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
{"16", IDB_FS_16, AF_ENABLED|BF_DOTABLE, NULL},
{"14", IDB_FS_14, AF_ENABLED|BF_DOTABLE, NULL},
{"12", IDB_FS_12, AF_ENABLED|BF_DOTABLE, NULL},
{"10", IDB_FS_10, AF_ENABLED|BF_DOTABLE, NULL},
{"9", IDB_FS_9, AF_ENABLED|BF_DOTABLE, NULL},
{"8", IDB_FS_8, AF_ENABLED|BF_DOTABLE, NULL},
{"7", IDB_FS_7, AF_ENABLED|BF_DOTABLE, NULL},
{"6", IDB_FS_6, AF_ENABLED|BF_DOTABLE, NULL},
{"", 0, 0, NULL}
};

PegMenuDescription FontMenu[] = {
{"Size", 0, AF_ENABLED, FontSizes},
{"Style", 0, AF_ENABLED, FontStyles},
{"", 0, 0, NULL}
};

/*--------------------------------------------------------------------------*/
// FontWin- Vector font test/demo window.
/*--------------------------------------------------------------------------*/
FontWin::FontWin(const PegRect &Rect) :	PegDialog(Rect, "Vector Fonts", NULL,
    FF_THICK)
{
    AddStatus(PSF_SIZEABLE);
    Add(new PegMenuBar(FontMenu));

    PegRect ChildRect = mClient;
    ChildRect -= 10;

    mpText = new PegEditBox(ChildRect);
    mpText->SetScrollMode(WSM_AUTOVSCROLL|WSM_CONTINUOUS);
    //mpText = new PegEditBox(ChildRect, 0, FF_RECESSED|EF_EDIT);
    //mpText->SetScrollMode(WSM_AUTOHSCROLL);

    mpFont = Screen()->MakeFont(16);
    mpText->SetFont(mpFont);
    //mpText->DataSet("This is a bunch of text for my text box.");
    Add(mpText);

    // The following line can be used to automatically populate the
    // text box with text:

    //SetTimer(1, ONE_SECOND/2, ONE_SECOND/2);

    miSize = 16;
    mbBold = FALSE;
    mbItalic = FALSE;
    miCount = 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
FontWin::~FontWin()
{
    Screen()->DeleteFont(mpFont);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED FontWin::Message(const PegMessage &Mesg)
{
	switch(Mesg.wType)
	{
    case PM_SIZE:
        {
            PegDialog::Message(Mesg);
            PegRect NewSize = mClient;
            NewSize -= 10;
            mpText->Resize(NewSize);
        }
        break;

    case PM_TIMER:
        AppendLine();
        break;

    case PM_HIDE:
        KillTimer(1);
        break;

     case SIGNAL(IDB_BOLD_FONT, PSF_CHECK_OFF):
        mbBold = FALSE;
        ResetFont();
        break;

    case SIGNAL(IDB_BOLD_FONT, PSF_CHECK_ON):
        mbBold = TRUE;
        ResetFont();
        break;
        
    case SIGNAL(IDB_ITALIC_FONT, PSF_CHECK_ON):
        mbItalic = TRUE;
        ResetFont();
        break;
        
    case SIGNAL(IDB_ITALIC_FONT, PSF_CHECK_OFF):
        mbItalic = FALSE;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_6, PSF_DOT_ON):
        miSize = 6;
        ResetFont();
        break;

    case SIGNAL(IDB_FS_7, PSF_DOT_ON):
        miSize = 7;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_8, PSF_DOT_ON):
        miSize = 8;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_9, PSF_DOT_ON):
        miSize = 9;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_10, PSF_DOT_ON):
        miSize = 10;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_12, PSF_DOT_ON):
        miSize = 12;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_14, PSF_DOT_ON):
        miSize = 14;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_16, PSF_DOT_ON):
        miSize = 16;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_18, PSF_DOT_ON):
        miSize = 18;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_20, PSF_DOT_ON):
        miSize = 20;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_24, PSF_DOT_ON):
        miSize = 24;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_30, PSF_DOT_ON):
        miSize = 30;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_36, PSF_DOT_ON):
        miSize = 36;
        ResetFont();
        break;
        
    case SIGNAL(IDB_FS_40, PSF_DOT_ON):
        miSize = 40;
        ResetFont();
        break;

	default:
		return(PegDialog::Message(Mesg));
    }
	return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void FontWin::AppendLine()
{
    static WORD wCount = 1;
    char cTemp[80];
    sprintf(cTemp, "Line of text #%d\n", wCount);
    mpText->Append(cTemp);
    mpText->Draw();
    wCount++;

    if (wCount > 80)
    {
        KillTimer(1);
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void FontWin::ResetFont(void)
{
    Screen()->DeleteFont(mpFont);
    mpFont = Screen()->MakeFont((UCHAR) miSize, mbBold, mbItalic);
    mpText->SetFont(mpFont);
    mpText->Draw();
}


