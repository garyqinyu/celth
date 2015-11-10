/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pdialog.cpp - Dialog window class definition.
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

#include "peg.hpp"

/*--------------------------------------------------------------------------*/
PegDialog::PegDialog(const PegRect &Rect, const PEGCHAR *Title, PegThing *pReport,
    WORD wStyle) : PegDecoratedWindow(Rect, wStyle)
{
    Type(TYPE_DIALOG);
    muColors[PCI_NORMAL] = PCLR_DIALOG;

    if (Title)
    {
        Add(new PegTitle(Title, TF_SYSBUTTON|TF_CLOSEBUTTON|TT_COPY));
        AddStatus(PSF_MOVEABLE);
    }
    mpReportTo = pReport;
    mbModal = FALSE;
}

/*--------------------------------------------------------------------------*/
PegDialog::PegDialog(const PEGCHAR *Title, PegThing *pReport, WORD wStyle) :
    PegDecoratedWindow(wStyle)
{
    Type(TYPE_DIALOG);
    muColors[PCI_NORMAL] = PCLR_DIALOG;

    if (Title)
    {
        Add(new PegTitle(Title, TF_SYSBUTTON|TF_CLOSEBUTTON|TT_COPY));
        AddStatus(PSF_MOVEABLE);
    }
    mpReportTo = pReport;
    mbModal = FALSE;
}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegDialog::~PegDialog()
{

}

/*--------------------------------------------------------------------------*/
SIGNED PegDialog::Message(const PegMessage &Mesg)
{
    SIGNED Result = 0;

    switch(Mesg.wType)
    {
    case PEG_SIGNAL(IDB_APPLY, PSF_CLICKED):
        if (!mbModal)
        {
            PegMessage NewMessage(mpReportTo, PM_DIALOG_NOTIFY);
            NewMessage.iData = Mesg.iData;
            NewMessage.pSource = this;
            mpReportTo->Message(NewMessage);
        }
        break;

    case PEG_SIGNAL(IDB_CLOSE, PSF_CLICKED):
    case PEG_SIGNAL(IDB_OK, PSF_CLICKED):
    case PEG_SIGNAL(IDB_CANCEL, PSF_CLICKED):
    case PEG_SIGNAL(IDB_YES, PSF_CLICKED):
    case PEG_SIGNAL(IDB_NO, PSF_CLICKED):
    case PEG_SIGNAL(IDB_RETRY, PSF_CLICKED):
    case PEG_SIGNAL(IDB_ABORT, PSF_CLICKED):
        if (!mbModal)
        {
            PegMessage NewMessage(mpReportTo, PM_DIALOG_NOTIFY);
            NewMessage.iData = Mesg.iData;
            NewMessage.pSource = this;
            mpReportTo->Message(NewMessage);
            Destroy(this);
        }
        else
        {
            Result = PegDecoratedWindow::Message(Mesg);
        }
        break;

    case PM_SHOW:
        PegDecoratedWindow::Message(Mesg);
        if (!(mpReportTo))
        {
            mpReportTo = Parent();
        }
        break;


    default:
        Result = PegDecoratedWindow::Message(Mesg);
    }
    return (Result);
}


// End of file
