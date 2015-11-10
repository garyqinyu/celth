/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pdialog.hpp - Basic Peg dialog window class.
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

#ifndef _PEGDIALOG_
#define _PEGDIALOG_

/*--------------------------------------------------------------------------*/

class PegDialog : public PegDecoratedWindow
{
    public:
        PegDialog(const PegRect &Rect, const PEGCHAR *Title = NULL,
            PegThing *ReportTo = NULL, WORD wStyle = FF_RAISED);
        PegDialog(const PEGCHAR *Title = NULL, PegThing *ReportTo = NULL,
            WORD wStyle = FF_RAISED);
        virtual ~PegDialog();
        virtual SIGNED Message(const PegMessage &Mesg);

    protected:
        PegThing *mpReportTo;
};


#endif

