/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pprogwin.hpp - Progress Window- a PegMessageWindow with a progress bar.
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
/*--------------------------------------------------------------------------*/


#ifndef _PEGPROGWIN_
#define _PEGPROGWIN_

class PegProgressWindow : public PegMessageWindow
{
    public:

        PegProgressWindow(const PegRect &Rect, const PEGCHAR *Title, 
            const PEGCHAR *Message, WORD wMesgStyle, 
            WORD wProgStyle = FF_THIN|PS_SHOW_VAL|PS_PERCENT,
            PegBitmap *pIcon = NULL, PegThing *Owner = NULL);

        PegProgressWindow(const PEGCHAR *Title,  const PEGCHAR *Message,
            WORD wMesgStyle, WORD wProgStyle = FF_THIN|PS_SHOW_VAL|PS_PERCENT,
            PegBitmap *pIcon = NULL, PegThing *Owner = NULL);

        virtual ~PegProgressWindow();

        PegProgressBar *Bar(void) {return mpProgBar;}

    protected:

        void AddProgBar(void);
        PegProgressBar *mpProgBar;
        WORD mwProgStyle;
};



#endif



