#include "peg.hpp"
#include "tvbitmapbutton.hpp"
#include "usertype.hpp"
#include "tooltip.hpp"

TVBitmapButton::TVBitmapButton(PegRect &Rect, PegBitmap *bm, WORD wId, WORD wStyle)
:PegBitmapButton(Rect, bm, wId, wStyle)
{
	m_pTipString = NULL;
	m_pToolTip = NULL;
}

TVBitmapButton::~TVBitmapButton()
{
	m_pTipString = NULL;
	m_pToolTip = NULL;
}

SIGNED TVBitmapButton::Message(const PegMessage &Mesg)
{
	PegMessage TmpMsg;
	
	switch (Mesg.wType)
	{
#ifdef PEG_DRAW_FOCUS
	case PM_CURRENT:
	case PM_NONCURRENT:
		{
			PegThing::Message(Mesg);
			Invalidate(mClip);
			Draw();
			break;
		}
#endif

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
				PegBitmapButton::Message(Mesg); /* send also to parent class for handling */
			}
			else if (Mesg.iData == PK_LNDN)
			{/* catch Key Down and send it to parent for information */
				TmpMsg.pTarget = Parent();
				TmpMsg.pSource = this;
				TmpMsg.wType = PM_KEYDOWN;
				TmpMsg.iData = mwId;
				Parent()->Message(TmpMsg);
				PegBitmapButton::Message(Mesg); /* send also to parent class for handling */
			}
			else
			{
				PegBitmapButton::Message(Mesg);
			}
			break;
			
		case PM_GAINED_KEYBOARD:
		case PM_LOST_KEYBOARD:
			break;
			
#endif
			
		default:
			PegBitmapButton::Message(Mesg);
	}
	return 0;    
}

void TVBitmapButton::ShowTip()
{	
	m_pToolTip->DeleteTipGraphics();

	if(NULL == m_pTipString || NULL == m_pToolTip)
	{
		return;
	}

	SIGNED nTxtWidth = 0;
	PegRect rect;
	nTxtWidth = m_pToolTip->GetTextWidth(m_pTipString);	
	rect.Set(mReal.wRight + 1, mReal.wTop, mReal.wRight + nTxtWidth + 1 + 6 + 40, mReal.wTop + 50);
	m_pToolTip->Resize(rect);
	m_pToolTip->SetTipString(m_pTipString);	//set tip string & redraw ToolTip
}

void TVBitmapButton::SetTipString(const PEGCHAR * pTipString)
{
	m_pTipString = pTipString;
}

void TVBitmapButton::SetToolTip(ToolTip * pToolTip)
{
	m_pToolTip = pToolTip;
}

void TVBitmapButton::Draw(void)
{
	BeginDraw();
	PegBitmapButton::Draw();

	//draw border
	PegColor colorTopLeft(BLACK, BLACK, CF_NONE);
	PegColor colorRightBottom(WHITE, WHITE, CF_NONE);
	Rectangle(mReal, colorTopLeft, 1);

	if (StatusIs(PSF_CURRENT))
	{
		colorTopLeft.uForeground = DARKGRAY;		
		colorRightBottom.uForeground = WHITE;
	}
	else
	{
		colorTopLeft.uForeground = WHITE;
		colorRightBottom.uForeground = DARKGRAY;
	}

//	Line(mReal.wLeft, mReal.wTop, mReal.wRight, mReal.wTop, colorTopLeft);
	Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wRight - 1, mReal.wTop + 1, colorTopLeft);
	Line(mReal.wLeft + 1, mReal.wTop + 2, mReal.wRight - 2, mReal.wTop + 2, colorTopLeft);
	Line(mReal.wLeft + 1, mReal.wTop + 3, mReal.wRight - 3, mReal.wTop + 3, colorTopLeft);
//	Line(mReal.wLeft, mReal.wTop + 2, mReal.wLeft, mReal.wBottom - 1, colorTopLeft);
	Line(mReal.wLeft + 1, mReal.wTop + 2, mReal.wLeft + 1, mReal.wBottom - 2, colorTopLeft);
	Line(mReal.wLeft + 2, mReal.wTop + 2, mReal.wLeft + 2, mReal.wBottom - 3, colorTopLeft);
	Line(mReal.wLeft + 3, mReal.wTop + 2, mReal.wLeft + 3, mReal.wBottom - 4, colorTopLeft);

//	Line(mReal.wLeft, mReal.wBottom, mReal.wRight, mReal.wBottom, colorRightBottom);
	Line(mReal.wLeft + 1, mReal.wBottom - 1, mReal.wRight - 1, mReal.wBottom - 1, colorRightBottom);
	Line(mReal.wLeft + 2, mReal.wBottom - 2, mReal.wRight - 1, mReal.wBottom - 2, colorRightBottom);
	Line(mReal.wLeft + 3, mReal.wBottom - 3, mReal.wRight - 1, mReal.wBottom - 3, colorRightBottom);
//	Line(mReal.wRight, mReal.wBottom - 2, mReal.wRight, mReal.wTop + 1, colorRightBottom);
	Line(mReal.wRight - 1, mReal.wBottom - 2, mReal.wRight - 1, mReal.wTop + 2, colorRightBottom);
	Line(mReal.wRight - 2, mReal.wBottom - 2, mReal.wRight - 2, mReal.wTop + 3, colorRightBottom);
	Line(mReal.wRight - 3, mReal.wBottom - 2, mReal.wRight - 3, mReal.wTop + 4, colorRightBottom);

	//

	EndDraw();
}