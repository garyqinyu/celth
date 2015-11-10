/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pbitmwin.hpp - Definition of PegBitmapWindow class.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "usertype.hpp"

#ifndef _PEGBITMAPWINDOW_
#define _PEGBITMAPWINDOW_

enum CORNER_INDEXES {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_RIGHT,
    BOTTOM_LEFT
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegBitmapWindow : public PegWindow
{
    public:
				PegBitmapWindow(PegBitmap* pBitmap=NULL, UCHAR BitmapStyle=kAutoAdjustBitmapSize, UCHAR FillColor=PCLR_CLIENT, WORD wStyle = FF_NONE);
        virtual ~PegBitmapWindow() {}
        void Draw(void);
				void SetBitmap(PegBitmap* pNewBitmap);
				void SetBackgroundColor(UCHAR FillColor);
        SIGNED Message(const PegMessage &Mesg);

    protected:
			PegBitmap* mpBitmap;
			UCHAR mFillColor;
			UCHAR mBitmapStyle;
      PegCapture mCaptures[4];
      BOOL mbCaptured;
};


#endif

