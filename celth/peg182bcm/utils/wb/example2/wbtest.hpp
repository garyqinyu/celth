/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  
//    Copyright (c) swell Software
//                 3022 Lindsay Ln
//                 Port Huron, MI 48060
//  
//    All Rights Reserved
//  
//  Notes:
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
class TestDialog : public PegDialog
{
    public:
        TestDialog();
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            IDB_HELLOTHERE = 1,
            IDB_HELLO,
            IDB_SAVE,
            IDB_CLOSE,
        };

    private:
};
