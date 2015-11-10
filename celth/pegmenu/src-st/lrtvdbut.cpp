/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// tvlrdbut.cpp - Definition of LRTVDecoratedButton class.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


#include "peg.hpp"
#include "lrtvdbut.hpp"
#include "usertype.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LRTVDecoratedButton::LRTVDecoratedButton(PegRect &Size, const PEGCHAR *pText,
    PegBitmap *pBitmap, PegBitmap **ppAnimationBmp, UCHAR ucNbBitmaps,
		UCHAR FocusTextColor, UCHAR NoFocusTextColor,
		WORD wId, WORD wStyle, BOOL bFlyOver) :
  TVDecoratedButton(Size, pText, pBitmap, ppAnimationBmp, ucNbBitmaps, 
		FocusTextColor, NoFocusTextColor, wId, wStyle, bFlyOver)
{
	
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED LRTVDecoratedButton::Message(const PegMessage &Mesg)
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
		{/* catch Key Up and send it to parent for information */
			TmpMsg.pTarget = Parent();
			TmpMsg.pSource = this;
			TmpMsg.wType = PM_KEYUP;
			TmpMsg.iData = mwId;
			Parent()->Message(TmpMsg);	        
		}
		else if (Mesg.iData == PK_LNDN)
		{/* catch Key Down and send it to parent for information */
			TmpMsg.pTarget = Parent();
			TmpMsg.pSource = this;
			TmpMsg.wType = PM_KEYDOWN;
			TmpMsg.iData = mwId;
			Parent()->Message(TmpMsg);	        
		}
		else
		{
			PegDecoratedButton::Message(Mesg);
		}
		break;

	case PM_GAINED_KEYBOARD:
	case PM_LOST_KEYBOARD:
		break;

	#endif

	default:
		TVDecoratedButton::Message(Mesg);
	}
	return 0;    
}


