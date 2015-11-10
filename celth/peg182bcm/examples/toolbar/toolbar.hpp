/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  Author: Jim DeLisle
//  
//    Copyright (c) 1997-2002, Swell Software, Inc.
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class ToolBarWindow : public PegDecoratedWindow
{
    public:
        ToolBarWindow(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            IDB_ALPHA_BUTTON = 1,
            IDB_BULL_BUTTON,
            IDB_BLUE_BUTTON,
            IDB_RED_BUTTON,
            IDB_GREEN_BUTTON,
            IDB_GREY_BUTTON,
            IDC_STRING_PANEL
        };

    private:
        void DisplayMessageWindow(WORD wForWho);
};
