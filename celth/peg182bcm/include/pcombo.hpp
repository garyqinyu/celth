/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pcombo.hpp - PegComboBox class definition.
//
// Author: Kenneth G. Maxwell
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

#ifndef _PEGCOMBO_
#define _PEGCOMBO_

class PegComboBox : public PegVertList
{
    public:
        PegComboBox(const PegRect &Rect, WORD wId = 0, WORD wStyle = FF_THIN);
        virtual ~PegComboBox();
        virtual SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        void Resize(PegRect NewSize);
        SIGNED GetOpenHeight(void) {return miOpenHeight;}
        BOOL IsOpen(void) {return mbOpen;}

        enum ButtonIds {
            IDB_OPEN = 1000
        };

    protected:
        void CalcCloseHeight(void);

        SIGNED miOpenHeight;
        SIGNED miCloseHeight;

        BOOL mbOpen;
        PegBitmapButton *mpOpenButton;
        PegPrompt *mpPrompt;
};


#endif


