// TVAnimateWindow.cpp: implementation of the TVAnimateWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "peg.hpp"
#include "TVAnimateWindow.hpp"
#include "anim.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TVAnimateWindow::TVAnimateWindow(PegThing* pCaller, PegThing** pptWindow, 
			const PegRect &Rect, PegBitmap *pBackground, PegBitmap **FrameList, 
			UCHAR uNumFrames, SIGNED xPos, SIGNED yPos, UCHAR *Palette,  WORD wStyle)
: PegAnimationWindow(Rect, pBackground, FrameList, uNumFrames, xPos, yPos, Palette, wStyle)
{
	int iIndex = 0;
	while (*pptWindow != NULL) /* save table list */
	{
		ptHiddenWindow[iIndex++] = *pptWindow++;
	}
	ptHiddenWindow[iIndex]  = NULL;
	
	pCallerObject = pCaller;
}

TVAnimateWindow::~TVAnimateWindow()
{

}

SIGNED TVAnimateWindow::Message(const PegMessage &Mesg)
{
	int iIndex;
	PegMessage tmpMsg;
	
    switch (Mesg.wType)
    {	
	case PM_TIMER:
        {
			muAnimationFrame++;
			
			if (muAnimationFrame >= muNumFrames)
			{
				muAnimationFrame = 0;
			}

			DrawTVAnimationFrame();
			break;
		}

#ifdef PEG_KEYBOARD_SUPPORT
	case PM_KEY:
		{
			if (PK_LNUP == Mesg.iData || PK_LNDN == Mesg.iData)
			{
				Destroy(this);
				BeginDraw();
				iIndex = 0;
				while(ptHiddenWindow[iIndex])
					iIndex++;
				
				if (iIndex > 0)
				{ /* some window must be added */
					iIndex--; /* point to last not NULL index */
					
					while(iIndex >= 0)
					{
						PegPresentationManager::Presentation()->Add(ptHiddenWindow[iIndex--]);
					}
				}
				MoveToFront(pCallerObject);
				EndDraw();	
				
				tmpMsg.pTarget = pCallerObject->Parent();
				tmpMsg.pSource = this;
				tmpMsg.wType = PM_REFRESHHELPWND;
				pCallerObject->Parent()->Message(tmpMsg);				
			}
			else if(PK_RIGHT == Mesg.iData)
			{
				//end tuning
				SetAnimateFrames(ppBmpAutoFineEnd, 2);
				Run(ONE_SECOND / 3);
			}
			else if(PK_LEFT == Mesg.iData)
			{
				//start tuning
				SetAnimateFrames(ppBmpAutoFineStart, 3);
				Run(ONE_SECOND / 3);
			}

			break;
		}
#endif
		
    default:
        return PegAnimationWindow::Message(Mesg);
    }
    return 0;
}

void TVAnimateWindow::DrawTVAnimationFrame(void)
{
    PegBitmap *pNewFrame = mpAnimations[muAnimationFrame];

    PegRect  FillRect;
    PegPoint PutFrame;

    PutFrame.x = mClient.wLeft + mFramePoint.x;
    PutFrame.y = mClient.wTop + mFramePoint.y;

    FillRect.wLeft = PutFrame.x;
    FillRect.wRight = FillRect.wLeft + pNewFrame->wWidth;
    FillRect.wTop = PutFrame.y;
    FillRect.wBottom = FillRect.wTop + pNewFrame->wHeight;
    Invalidate(FillRect);
    BeginDraw();
	PegColor color(PegTransparentColor, PegTransparentColor, CF_FILL);
	Rectangle(mReal, color);
    Bitmap(PutFrame, pNewFrame);
    EndDraw();
}

void TVAnimateWindow::SetAnimateFrames(PegBitmap **FrameList, UCHAR uNumFrames)
{
	Stop();
	muAnimationFrame = 0;
	muNumFrames = uNumFrames;
	mpAnimations = FrameList;
}

