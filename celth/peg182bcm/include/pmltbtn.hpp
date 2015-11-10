/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pmltbtn.hpp - Definition of Multi-Line text button class.
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

#ifndef _PEGMLBUTTON_
#define _PEGMLBUTTON_

#define DEF_ML_MARKER  0x7c        // the '|' character

class PegMLTextButton : public PegButton
{
    public:
        PegMLTextButton(PegRect &, const PEGCHAR *text,
            PEGCHAR cMarker  = DEF_ML_MARKER,
            WORD wId = 0, WORD wStyle = TJ_CENTER|AF_ENABLED);

        PegMLTextButton(SIGNED wLeft, SIGNED wTop, SIGNED iWidth, 
            const PEGCHAR *text, PEGCHAR cMarker = DEF_ML_MARKER,
            WORD wId = 0, WORD wStyle = AF_ENABLED);

        PegMLTextButton(SIGNED wLeft, SIGNED wTop, const PEGCHAR *text, 
            PEGCHAR cMarker = DEF_ML_MARKER, WORD wId = 0,
            WORD wStyle = AF_ENABLED);

        virtual ~PegMLTextButton();
        virtual void Draw(void);
        virtual void DataSet(const PEGCHAR *Text);
        virtual void SetFont(PegFont *pFont);
        PegFont *GetFont(void) {return mpFont;}

    protected:
        SIGNED ParseButtonText(const PEGCHAR *Text);
        void FreeTextInfo(void);

        PEGCHAR **mpText;
        SIGNED  *miWidths;
        PegFont *mpFont;
        WORD    mwNumRows;
        PEGCHAR mcMarker;
};


#endif




