/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// table.hpp - Demonstration of PegTable class definition.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2002 Swell Software 
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


class TableFrameWin : public PegDecoratedWindow
{
    public:
        TableFrameWin(const PegRect &Rect);
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);

    private:

        enum  {
            IDB_HELP_ABOUT = 1,
            IDB_GRID_WIDTH,
            IDB_TBUTTON
        };

        void DisplayHelpWindow(void);
        void AddTableClients(void);
        PegTable *mpTable;
        PegWindow *mpScrollWin;
};

