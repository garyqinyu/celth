//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: PEGMainMenu
//  
//  Notes:

class PEGMainMenu1 : public PegWindow
{
    public:
        PEGMainMenu1(SIGNED iLeft, SIGNED iTop);
        SIGNED Message(const PegMessage &Mesg);
        enum ChildIds {
            ID_Button_2 = 1,
            ID_Button_3,
            ID_Button_1,
        };

    private:
		void DisplayChildWindow(int WindowID);
        PegDecoratedButton *pMainmenuButton_2;
        PegDecoratedButton *pMainmenuButton_3;
        PegDecoratedButton *pMainmenuButton_1;
};
