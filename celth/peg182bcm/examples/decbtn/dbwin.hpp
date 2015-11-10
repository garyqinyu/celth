/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// dbwin.hpp - Example PegDecoratedButtons window
//
// Author: Jim DeLisle
//
// Copyright (c) 1998-2002 Swell Software 
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

class DecButtonWindow : public PegDecoratedWindow
{
    public:
        DecButtonWindow(SIGNED iLeft, SIGNED iBottom);
        virtual ~DecButtonWindow() {}

        enum ChildIDs {
            IDB_BUTTON_TEXT = 1,
            IDB_BUTTON_BITMAP,
            IDB_BUTTON_BOTH
        };

        PegDecoratedButton* mpDBText;
        PegDecoratedButton* mpDBBitmap;
        PegDecoratedButton* mpDBBoth;
};



