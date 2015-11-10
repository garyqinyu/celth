/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// tvprobar.cpp - Definition of TVProgressBar class.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "usertype.hpp"
#include "tvprobar.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
TVProgressBar::TVProgressBar(const PegRect &Rect, WORD wStyle, SIGNED iMin, SIGNED iMax, SIGNED iCurrent) :
		PegProgressBar(Rect, wStyle, iMin, iMax, iCurrent)
{
	mMinValue = iMin;
	mMaxValue = iMax;
}


/*--------------------------------------------------------------------------*/
SIGNED TVProgressBar::GetMinValue(void)
{
	return (mMinValue);
}

/*--------------------------------------------------------------------------*/
SIGNED TVProgressBar::GetMaxValue(void)
{
	return (mMaxValue);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED TVProgressBar::Message(const PegMessage &Mesg)
{
		PegMessage TmpMsg;

		switch (Mesg.wType)
    {
	   #ifdef PEG_KEYBOARD_SUPPORT
		  case PM_KEY:
        if (Mesg.iData == PK_LEFT)
        { /* catch Key Left and send it to parent for handling */
					TmpMsg.pTarget = Parent();
					TmpMsg.pSource = this;
					TmpMsg.wType = PM_KEYLEFT;
					TmpMsg.iData = mwId;
					Parent()->Message(TmpMsg);
				}
				else if (Mesg.iData == PK_RIGHT)
        {/* catch Key Right and send it to parent for handling */
					TmpMsg.pTarget = Parent();
					TmpMsg.pSource = this;
					TmpMsg.wType = PM_KEYRIGHT;
					TmpMsg.iData = mwId;
					Parent()->Message(TmpMsg);
				}
				else if (Mesg.iData == PK_LNUP)
        {/* catch Key Upt and send it to parent for handling */
					TmpMsg.pTarget = Parent();
					TmpMsg.pSource = this;
					TmpMsg.wType = PM_KEYUP;
					TmpMsg.iData = mwId;
					Parent()->Message(TmpMsg);
				}
				else if (Mesg.iData == PK_LNDN)
        {/* catch Key Down and send it to parent for handling */
					TmpMsg.pTarget = Parent();
					TmpMsg.pSource = this;
					TmpMsg.wType = PM_KEYDOWN;
					TmpMsg.iData = mwId;
					Parent()->Message(TmpMsg);
				}
				else
				{
	        PegProgressBar::Message(Mesg);
				}
				break;
			#endif

			default:
        PegProgressBar::Message(Mesg);
    }
    return 0;    
}




