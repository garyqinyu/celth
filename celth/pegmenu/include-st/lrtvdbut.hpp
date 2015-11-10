/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// lrtvdbut.hpp - Definition of LRTVDecoratedButton class.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _TVLRDECORATEDBUT_
#define _TVLRDECORATEDBUT_

#include "tvdecbut.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class LRTVDecoratedButton : public TVDecoratedButton
{
	public:
		LRTVDecoratedButton(PegRect &Size, const PEGCHAR *pText,
			PegBitmap *pBitmap, PegBitmap **ppAnimationBmp = NULL, UCHAR ucNbBitmap = 0,
			UCHAR FocusTextColor = YELLOW, UCHAR NoFocusTextColor = WHITE,
			WORD wId = 0, WORD wStyle = AF_ENABLED, BOOL bFlyOver = FALSE);
		
		virtual ~LRTVDecoratedButton() {}
		virtual SIGNED Message(const PegMessage &Mesg);

	protected:       
};


#endif

