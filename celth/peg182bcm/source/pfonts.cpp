/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pfonts.cpp - PegTextThing class and font definitions.
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
//  PegTextThing is a simple intermediate class that provides some common
//  functionality for all Peg things that have the capability of displaying
//    text. These include PegButton, PegTitle, PegString, PegEdit, PegText, etc.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "string.h"
#include "peg.hpp"

PegFont *PegTextThing::mDefaultFonts[PEG_NUMBER_OF_DEFAULT_FONTS + 
                                     PEG_SPARE_FONT_INDEXES] = {
    DEFAULT_FONT_PTR,
    DEF_TITLE_FONT,
    DEF_MENU_FONT,
    DEF_TBUTTON_FONT,
    DEF_RBUTTON_FONT,
    DEF_CHECKBOX_FONT,
    DEF_PROMPT_FONT,
    DEF_STRING_FONT,
    DEF_TEXTBOX_FONT,
    DEF_GROUP_FONT,
    DEF_ICON_FONT,
    DEF_CELL_FONT,
    DEF_HEADER_FONT,
    DEF_TAB_FONT,
    DEF_MESGWIN_FONT,
    DEF_TREEVIEW_FONT
};

        
/*--------------------------------------------------------------------------*/
PegTextThing::PegTextThing(WORD wCopy, UCHAR uFontIndex)
{
    mwStrLen = 0;
    mwBufferLen = 0;
    mpText = NULL;
    mpFont = mDefaultFonts[uFontIndex];

    if (wCopy)
    {
        mbCopy = 1;
    }
    else
    {
        mbCopy = 0;
    }
}

/*--------------------------------------------------------------------------*/
PegTextThing::PegTextThing(const PEGCHAR *Text, WORD wCopy, UCHAR uFontIndex)
{
    mwBufferLen = 0;
    if (wCopy)
    {
        mbCopy = 1;
    }
    else
    {
        mbCopy = 0;
    }

    if (mbCopy)
    {
	    if (Text)
	    {
            if (*Text)
            {
	            mwStrLen = strlen(Text) + 1;
                mwBufferLen = mwStrLen + 16;
	            mpText = new PEGCHAR[mwBufferLen];
	            strcpy(mpText, Text);
	        }
	        else
	        {
    	        mwStrLen = 0;
	            mpText = NULL;
	        }
	    }
	    else
	    {
	        mpText = NULL;
	        mwStrLen = 0;
	    }
    }
    else
    {
        if (Text)
        {
            if (*Text)
            {
                mpText = (PEGCHAR *) Text;
                mwStrLen = strlen(Text) + 1;
            }
            else
            {
                mpText = NULL;
                mwStrLen = 0;
            }
        }
        else
        {
            mpText = NULL;
            mwStrLen = 0;
        }
    }
    mpFont = mDefaultFonts[uFontIndex];
}


/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegTextThing::~PegTextThing()
{
    if (mpText && mbCopy)
    {
        delete [] mpText;
    }    
}

/*--------------------------------------------------------------------------*/
void PegTextThing::DataSet(const PEGCHAR *pText)
{
    if (mbCopy)
    {
	    if (pText)
	    {
	        if (*pText)
	        {
	            mwStrLen = strlen(pText) + 1;
	            if (mwStrLen > mwBufferLen)
	            {
	                if (mpText)
	                {
	                    delete [] mpText;
	                }
	                mwBufferLen = mwStrLen + 32;
	
	                mpText = new PEGCHAR[mwBufferLen];
	                strcpy(mpText, pText);
	            }
	            else
	            {
	                strcpy(mpText, pText);
	            }
	            return;
	        }
	    }
	    if (mpText)
	    {
	        delete [] mpText;
	    }
	    mpText = NULL;
	    mwStrLen = 0;
        mwBufferLen = 0;
    }
    else
    {
        if (pText)
        {
            if (*pText)
            {
                mpText = (PEGCHAR *) pText;
                mwStrLen = strlen(pText) + 1;
            }
            else
            {
                mpText = NULL;
                mwStrLen = 0;
            }
        }
        else
        {
            mpText = NULL;
            mwStrLen = 0;
        }
    }
}


/*--------------------------------------------------------------------------*/
// SetCopyMode- Can be used to turn on copy mode after construction. Copy
// mode cannot be turned off after it is turned on.
/*--------------------------------------------------------------------------*/
void PegTextThing::SetCopyMode()
{
    if (mbCopy)
    {
        return;
    }

    if (mpText)
    {
        PEGCHAR *pTemp = mpText;
        mpText = NULL;
        mwStrLen = 0;
        mwBufferLen = 0;
        mbCopy = 1;
        DataSet(pTemp);
    }
}


