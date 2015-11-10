/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  
//    Copyright (c) Swell Software, Inc.
//                 3022 Lindsay Ln
//                 Port Huron, MI 48060
//  
//    All Rights Reserved
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PopupMenuWin : public PegDialog
{
    public:
        PopupMenuWin(SIGNED iLeft, SIGNED iTop);
        ~PopupMenuWin();
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            IDB_POPMENU = 1,
            IDB_SET_COLOR_BLACK,    
            IDB_SET_COLOR_LIGHTGRAY,
            IDB_SET_COLOR_RED,    
            IDB_SET_COLOR_BLUE,     
            IDB_SET_COLOR_DARKGRAY
        };

    private:
        PegMenu *mpMenu;

        void AddPopupMenu(void);
        void RemovePopupMenu(void);
        void SetClientColor(COLORVAL Color);
};
