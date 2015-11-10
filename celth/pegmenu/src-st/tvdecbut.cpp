/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// tvdecbut.cpp - Definition of TVDecoratedButton class.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "usertype.hpp"
#include "tvdecbut.hpp"

/*#define LEAVE_TRAILS */ // Allow more than one button to be fading out??

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
TVDecoratedButton::TVDecoratedButton(PegRect &Size, const PEGCHAR *pText,
            PegBitmap *pBitmap, PegBitmap **ppAnimationBmp, UCHAR ucNbBitmaps,
						UCHAR FocusTextColor, UCHAR NoFocusTextColor,
						WORD wId, WORD wStyle, BOOL bFlyOver) :
    PegDecoratedButton(Size, pText, pBitmap, wId, wStyle, bFlyOver)
{
	Type(TYPE_TVDECORATEDBUT);

	SetSignals(SIGMASK(PSF_FOCUS_RECEIVED)|SIGMASK(PSF_FOCUS_LOST)|SIGMASK(PSF_CLICKED));

	mNbBitmaps = ucNbBitmaps;
	mpAnimationBitmaps = ppAnimationBmp;
	mFocusTextColor = FocusTextColor;
	mNoFocusTextColor = NoFocusTextColor;
  muCurrentBitmap = ucNbBitmaps  - 1;
}


#ifdef PEG_DRAW_FOCUS
/*--------------------------------------------------------------------------*/
void TVDecoratedButton::DrawFocusIndicator(BOOL bDraw)
{
		if (mpAnimationBitmaps != NULL)
		{
			KillTimer(ERASE_FOCUS_TIMER);
			muCurrentBitmap = mNbBitmaps - 2;
			SetColor(PCI_NTEXT, mFocusTextColor); /* change text color now */
      SetBitmap(mpAnimationBitmaps[muCurrentBitmap]);
      Invalidate();
      Draw();
      SetTimer(DRAW_FOCUS_TIMER, ONE_SECOND / 4, ONE_SECOND / 4);
		}
}

/*--------------------------------------------------------------------------*/
void TVDecoratedButton::EraseFocusIndicator(void)
{

   #if !defined(LEAVE_TRAILS)

    // If we don't want to see more than one button removing focus,
    // tell them to move directly to the NoFocus state:
		if (mpAnimationBitmaps != NULL)
		{
	    KillSibFocus();
		}
   #endif

    if (mpAnimationBitmaps != NULL && muCurrentBitmap != mNbBitmaps - 1)
		{
			KillTimer(DRAW_FOCUS_TIMER);
    	Invalidate();
			SetColor(PCI_NTEXT, mNoFocusTextColor); /* change text color now */
			SetBitmap(mpAnimationBitmaps[1]);
      muCurrentBitmap = 1;
      Draw();
      SetTimer(ERASE_FOCUS_TIMER, ONE_SECOND / 4, ONE_SECOND /4);
		}
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void TVDecoratedButton::KillSibFocus(void)
{
    // Check to see if another of my sibling buttons is still
    // erasing focus, and if so make him go immediately to the "no focus"
    // bitmap. This stops the animation from getting behind the user's
    // input selection:

    PegThing *pSib = Parent()->First();
    while(pSib)
    {
        if (pSib != this && pSib->Type() == TYPE_TVDECORATEDBUT)
        {
            TVDecoratedButton *pb = (TVDecoratedButton *) pSib;
            pb->NoFocus();
       }
       pSib = pSib->Next();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void TVDecoratedButton::NoFocus(void)
{
    KillTimer(0);

    if (muCurrentBitmap != mNbBitmaps - 1)
    {
        muCurrentBitmap = mNbBitmaps - 1;
        SetBitmap(mpAnimationBitmaps[muCurrentBitmap]);
        Invalidate();
        Parent()->Draw();
    }
}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED TVDecoratedButton::Message(const PegMessage &Mesg)
{
		PegMessage TmpMsg;

		switch (Mesg.wType)
    {
		 #ifdef PEG_DRAW_FOCUS
			case PM_CURRENT:
					Parent()->MoveToFront(this);
					PegThing::Message(Mesg);
					DrawFocusIndicator(TRUE);
					break;

			case PM_NONCURRENT:
					PegThing::Message(Mesg);
					EraseFocusIndicator();
					break;
		 #endif

			case PM_TIMER:
					switch(Mesg.iData)
          {
          case DRAW_FOCUS_TIMER:
						muCurrentBitmap--;
						SetBitmap(mpAnimationBitmaps[muCurrentBitmap]);
            Invalidate();
            Parent()->Draw();
            if (!muCurrentBitmap) 
            {   
							KillTimer(DRAW_FOCUS_TIMER);
             }
             break;

					case ERASE_FOCUS_TIMER:
						muCurrentBitmap++;
				    if (muCurrentBitmap > mNbBitmaps - 1)
            {
							KillTimer(ERASE_FOCUS_TIMER);
            }
            else
            {
					   	SetBitmap(mpAnimationBitmaps[muCurrentBitmap]);
              Invalidate();
              Parent()->Draw();
             }
             break;
					}
				break;

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
	        PegDecoratedButton::Message(Mesg); /* send also to parent class for handling */
				}
				else if (Mesg.iData == PK_LNDN)
        {/* catch Key Down and send it to parent for information */
					TmpMsg.pTarget = Parent();
					TmpMsg.pSource = this;
					TmpMsg.wType = PM_KEYDOWN;
					TmpMsg.iData = mwId;
					Parent()->Message(TmpMsg);
	        PegDecoratedButton::Message(Mesg); /* send also to parent class for handling */
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

			case PM_HIDE:
				PegDecoratedButton::Message(Mesg);
//				EraseFocusIndicator();
				KillTimer(0);   // kill all timers for this object
				break;

			default:
        PegDecoratedButton::Message(Mesg);
    }
    return 0;    
}


