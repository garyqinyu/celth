/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pbitmwin.cpp - Definition of PegBitmapWindow class.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "usertype.hpp"
#include "pbitmwin.hpp"

#define CORNER_RADIUS  30

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegBitmapWindow::PegBitmapWindow(PegBitmap* pBitmap, UCHAR BitmapStyle , UCHAR FillColor, WORD wStyle) :
		PegWindow(wStyle)
{
	Type(TYPE_PEGBITMAPWINDOW);
	mpBitmap = pBitmap;
	mFillColor = FillColor;
	mBitmapStyle = BitmapStyle;

  if (mBitmapStyle == kAutoAdjustBitmapSize && mpBitmap)
	{ /* auto adjust window size to fit bitmap */

		mReal.wBottom = mReal.wTop + mpBitmap->wHeight - 1;
    mReal.wRight = mReal.wLeft + mpBitmap->wWidth - 1;
    InitClient();
   }
   mbCaptured = FALSE;
}

/*--------------------------------------------------------------------------*/
void PegBitmapWindow::Draw()
{
	PegPoint tPutBitmap;
	PegRect CaptureRect;

  if (!mbCaptured)
  {
      // Capture the four bitmaps on my corners
      CaptureRect.wLeft = mReal.wLeft;
      CaptureRect.wTop = mReal.wTop;
      CaptureRect.wRight = CaptureRect.wLeft + CORNER_RADIUS;
      CaptureRect.wBottom = CaptureRect.wTop + CORNER_RADIUS;
      Screen()->Capture(&mCaptures[TOP_LEFT], CaptureRect);

			CaptureRect.Shift(mReal.Width() - CORNER_RADIUS, 0);
      Screen()->Capture(&mCaptures[TOP_RIGHT], CaptureRect);

			CaptureRect.Shift(0, mReal.Height() - CORNER_RADIUS);
      Screen()->Capture(&mCaptures[BOTTOM_RIGHT], CaptureRect);

      CaptureRect.Shift(-(mReal.Width() - CORNER_RADIUS), 0);
      Screen()->Capture(&mCaptures[BOTTOM_LEFT], CaptureRect);
      mbCaptured = TRUE;
  }

	BeginDraw();

  muColors[PCI_NORMAL] = mFillColor; /* set default value with given color */

	if (mpBitmap == NULL)
	{ /* no bitmap given, so fill frame with given color */
		DrawFrame();
	}
	else
	{
		Screen()->Restore(this, &mCaptures[TOP_LEFT]);
    Screen()->Restore(this, &mCaptures[TOP_RIGHT]);
    Screen()->Restore(this, &mCaptures[BOTTOM_RIGHT]);
    Screen()->Restore(this, &mCaptures[BOTTOM_LEFT]);

		if (mBitmapStyle == kCenterBitmap)
		{ /* center bitmap in window */
			tPutBitmap.x = mReal.wLeft + ((mReal.wRight - mReal.wLeft) - mpBitmap->wWidth)/2;
			tPutBitmap.y = mReal.wTop + ((mReal.wBottom - mReal.wTop) - mpBitmap->wHeight)/2;
		}
		else
    {
      tPutBitmap.x = mReal.wLeft;
      tPutBitmap.y = mReal.wTop;
    }

		Bitmap(tPutBitmap, mpBitmap); /* set bitmap */
	}

	DrawChildren(); /* draw buttons */
	EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegBitmapWindow::SetBitmap(PegBitmap* pNewBitmap)
{
		mpBitmap = pNewBitmap;
	  Invalidate(mReal);
		Draw();
}

/*--------------------------------------------------------------------------*/
void PegBitmapWindow::SetBackgroundColor(UCHAR FillColor)
{
		mFillColor = FillColor;
	  Invalidate(mReal);
		Draw();
}

/*--------------------------------------------------------------------------*/
SIGNED PegBitmapWindow::Message(const PegMessage &Mesg)
{

	switch (Mesg.wType)
	{
		default:
				return PegWindow::Message(Mesg);
	}
	return 0;
}


