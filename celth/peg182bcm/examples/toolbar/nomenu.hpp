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
class NoMenuWindow : public PegDecoratedWindow
{
    public:
        NoMenuWindow(SIGNED iLeft, SIGNED iTop);

        SIGNED Message(const PegMessage& Mesg);

        enum ChildIds {
            IDC_SLIDER = 1,
            IDC_SLIDER_VAL
        };

    private:
};
