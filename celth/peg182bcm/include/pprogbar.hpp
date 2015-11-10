/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pprogbar.cpp - PegProgressBar class.
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

#ifndef _PP_BAR_
#define _PP_BAR_

class PegProgressBar : public PegThing
{
    public:
        PegProgressBar(const PegRect &Rect,
            WORD wStyle = FF_THIN|PS_SHOW_VAL|PS_PERCENT, SIGNED iMin = 0,
            SIGNED iMax = 100, SIGNED iCurrent = 0);
        void Draw(void);
        void SetFont(PegFont *pFont) {mpFont = pFont;}
        void Update(SIGNED iVal, BOOL bDraw = TRUE);
        SIGNED Value(void) {return miCurrent;}

    protected:
        PegButton *mpButton;
        PegFont   *mpFont;
        PegRect   mButtonRect;
        SIGNED    miMin;
        SIGNED    miMax;
        SIGNED    miCurrent;

        void GetButtonRect(void);
};


#endif


