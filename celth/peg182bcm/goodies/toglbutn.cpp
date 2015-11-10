



#include "peg.hpp"
#include "toglbutn.hpp"



ToggleButton::ToggleButton(PegRect &Size, PEGCHAR *pText,
    WORD wId, WORD wStyle) :
    PegTextButton(Size, pText, wId, wStyle)
{

}


SIGNED ToggleButton::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_LBUTTONDOWN:
        Style(Style() ^ BF_SELECTED);
        Invalidate(mReal);
        Draw();
        break;

    case PM_LBUTTONUP:
    case PM_POINTER_ENTER:
    case PM_POINTER_EXIT:
        break;

    default:
        return PegTextButton::Message(Mesg);
    }
    return 0;
}

