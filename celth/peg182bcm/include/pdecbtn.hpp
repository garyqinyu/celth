/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pdecbtn.hpp - PegDecoratedButton. Derived button that supports both
//               text and a bitmap.
//
// Author: Jim DeLisle
//
// Copyright (c) 1997-2000 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGDECBUTTON_
#define _PEGDECBUTTON_

#define BMP_TEXT_PADDING    2

class PegDecoratedButton : public PegButton, public PegTextThing
{
    public:
        PegDecoratedButton(PegRect &Size, const PEGCHAR *pText,
            PegBitmap* pBitmap, WORD wId = 0, WORD wStyle = AF_ENABLED,
            BOOL bFlyOver = FALSE);

        PegDecoratedButton(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, 
            const PEGCHAR *pText, PegBitmap* pBitmap, WORD wId = 0, 
            WORD wStyle = AF_ENABLED, BOOL bFlyOver = FALSE);

        PegDecoratedButton(SIGNED iLeft, SIGNED iTop, const PEGCHAR *pText, 
            PegBitmap* pBitmap, WORD wId = 0, WORD wStyle = AF_ENABLED,
            BOOL bFlyOver = FALSE);

        virtual ~PegDecoratedButton() {}
        
        virtual void Draw(void);
        virtual SIGNED Message(const PegMessage& Mesg);
        
        virtual void SetBitmap(PegBitmap* pNewMap) { mpBitmap = pNewMap; }
        
        BOOL GetFlyOver() const { return mbFlyOver; }
        void SetFlyOver(BOOL bFlyOver) { mbFlyOver = bFlyOver; }

        BOOL InFlyOver() const { return mbInFlyOver; }

    protected:
        PegBitmap* mpBitmap;

        BOOL       mbFlyOver;
        BOOL       mbInFlyOver;
};

#endif // _PEGDECBUTTON_


