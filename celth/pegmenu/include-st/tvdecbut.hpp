/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// cbutton.hpp - Definition of PegBitmapColorButton class.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGDECORATEDBUT_
#define _PEGDECORATEDBUT_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class TVDecoratedButton : public PegDecoratedButton
{
    public:
				TVDecoratedButton(PegRect &Size, const PEGCHAR *pText,
            PegBitmap *pBitmap, PegBitmap **ppAnimationBmp = NULL, UCHAR ucNbBitmap = 0,
						UCHAR FocusTextColor = YELLOW, UCHAR NoFocusTextColor = WHITE,
						WORD wId = 0, WORD wStyle = AF_ENABLED, BOOL bFlyOver = FALSE);
        virtual ~TVDecoratedButton() {}
        virtual SIGNED Message(const PegMessage &Mesg);

		protected:
       #ifdef PEG_DRAW_FOCUS
        virtual void DrawFocusIndicator(BOOL bDraw);
        virtual void EraseFocusIndicator(void);
        virtual void KillSibFocus(void);
        virtual void NoFocus(void);
       #endif
			UCHAR mNbBitmaps;
			PegBitmap **mpAnimationBitmaps;
			UCHAR mFocusTextColor;
			UCHAR mNoFocusTextColor;
      UCHAR muCurrentBitmap;
};


#endif

