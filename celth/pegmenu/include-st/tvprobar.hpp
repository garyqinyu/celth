/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// tvprobar.hpp - Definition of TVProgressBar class.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "usertype.hpp"

#ifndef _TVPROGRESSBAR_
#define _TVPROGRESSBAR_

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class TVProgressBar : public PegProgressBar
{
    public:
				TVProgressBar(const PegRect &Rect, WORD wStyle, SIGNED iMin, SIGNED iMax, SIGNED iCurrent);
				SIGNED Message(const PegMessage &Mesg);
        virtual ~TVProgressBar() {}
				virtual SIGNED GetMinValue(void);
				virtual SIGNED GetMaxValue(void);
    protected:
				SIGNED mMinValue;
				SIGNED mMaxValue;
};

#endif

