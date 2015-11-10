
#include "peg.hpp"
#include "mlmesg.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MultiLineMessage::MultiLineMessage(SIGNED iLeft, SIGNED iTop, PEGCHAR *pMesg)
    : PegDialog(NULL)
{
    PegRect ChildRect;
    PegTextBox *pChild1;

    mReal.Set(iLeft, iTop, iLeft + 318, iTop + 186);
    InitClient();
    Add (new PegTitle("Notification", 0));
    AddStatus(PSF_MOVEABLE);

    ChildRect.Set(iLeft + 215, iTop + 156, iLeft + 294, iTop + 175);
    Add(new PegTextButton(ChildRect, "No", IDB_NO, AF_ENABLED|TJ_CENTER));
    ChildRect.Set(iLeft + 26, iTop + 156, iLeft + 105, iTop + 175);
    Add(new PegTextButton(ChildRect, "Yes", IDB_YES, AF_ENABLED|TJ_CENTER));
    ChildRect.Set(iLeft + 13, iTop + 30, iLeft + 305, iTop + 146);
    pChild1 = new PegTextBox(ChildRect, 0, FF_NONE|TT_COPY|EF_WRAP, pMesg);
    pChild1->SetColor(PCI_NORMAL, LIGHTGRAY);
    Add(pChild1);

    /* WB End Construction */
}