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
class ExampleTwo : public PegDecoratedWindow
{
    public:
        ExampleTwo(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            IDB_HELPINDEX = 1,
            IDB_ABOUT,
            IDB_CLOSE,
            IDB_FILESAVE,
            IDB_OPENFILE,
            IDP_RATEVAL,
            IDSL_RATE,
            IDRB_DAILY,
            IDRB_HOURLY,
            IDRB_MINUTE,
            IDRB_ALLDATA,
            IDB_STOPBUTTON,
            IDB_STARTBUTTON,
        };

    private:
        PegPrompt *mpRatePrompt;
};
