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

#include "peg.hpp"
#include "wbtest.hpp"


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
TestDialog::TestDialog() : PegDialog()
{
    static PegMenuDescription ConfigureMenu [] = {
    {"HelloThere",  TestDialog::IDB_HELLOTHERE,  AF_ENABLED, NULL},
    {"", 0, 0, NULL}
    };
    static PegMenuDescription FileMenu [] = {
    {"Hello",  TestDialog::IDB_HELLO,  BF_SEPERATOR, NULL},
    {"Save",  TestDialog::IDB_SAVE,  BF_CHECKABLE, NULL},
    {"Close",  TestDialog::IDB_CLOSE,  BF_DOTABLE|BF_DOTTED, NULL},
    {"", 0, 0, NULL}
    };
    static PegMenuDescription TopMenu [] = {
    {"Configure",  0,  AF_ENABLED, ConfigureMenu},
    {"Help",  0,  AF_ENABLED, NULL},
    {"File",  0,  AF_ENABLED, FileMenu},
    {"", 0, 0, NULL}
    };
    PegRect ChildRect;
    PegThing *pChild1, *pChild2, *pChild3;

    mReal.Set(55, 17, 386, 279);
    InitClient();
    Add (new PegTitle("Test Dialog", TF_SYSBUTTON|TF_CLOSEBUTTON));
    Add(new PegMenuBar(TopMenu));

    ChildRect.Set(75, 71, 358, 257);
    pChild1 = new PegNotebook(ChildRect);, 4);;
    ((PegNotebook *) pChild1)->SetTabString(0, Mon);
    ((PegNotebook *) pChild1)->SetTabString(1, Tue);
    ((PegNotebook *) pChild1)->SetTabString(2, Wed);
    ((PegNotebook *) pChild1)->SetTabString(3, Thu);
    ChildRect.Set(79, 96, 354, 253);
    pChild1->Add(new PegTextBox(ChildRect, 0));
    ChildRect.Set(-26, 10, 249, 167);
    pChild2 = new PegGroup(ChildRect, "Group");
    ChildRect.Set(-6, 31, 27, 64);
    pChild2->Add(new PegBitmapButton(ChildRect, &tbb));
    ((PegNotebook *) pChild1)->SetPageClient(pChild2);

    ChildRect.Set(-9049, -8850, -9049, -8850);
    pChild2 = new PegWindow(ChildRect, FF_NONE);;
    pChild2->SetColor(PCI_NORMAL, LIGHTGRAY);
    ChildRect.Set(-310, -111, -310, -111);
    pChild2->Add(new PegGroup(ChildRect, "Group"));
    ((PegNotebook *) pChild1)->SetPageClient(pChild2);

    ChildRect.Set(-26, 10, 249, 167);
    pChild2 = new PegWindow(ChildRect, FF_NONE);;
    pChild2->SetColor(PCI_NORMAL, LIGHTGRAY);
    ChildRect.Set(86, 10, 184, 148);
    pChild3 = new PegGroup(ChildRect, "Group");
    ChildRect.Set(98, 112, 171, 131);
    pChild3->Add(new PegCheckBox(ChildRect, "CheckBox"));
    ChildRect.Set(98, 48, 171, 67);
    pChild3->Add(new PegCheckBox(ChildRect, "CheckBox"));
    ChildRect.Set(98, 92, 171, 111);
    pChild3->Add(new PegCheckBox(ChildRect, "CheckBox"));
    ChildRect.Set(98, 70, 171, 89);
    pChild3->Add(new PegCheckBox(ChildRect, "CheckBox"));
    ChildRect.Set(98, 28, 171, 47);
    pChild3->Add(new PegCheckBox(ChildRect, "CheckBox"));
    pChild2->Add(pChild3);

    ChildRect.Set(-21, 10, 74, 148);
    pChild3 = new PegGroup(ChildRect, "Group");
    ChildRect.Set(-11, 112, 41, 131);
    pChild3->Add(new PegRadioButton(ChildRect, "Radio"));
    ChildRect.Set(-11, 92, 41, 111);
    pChild3->Add(new PegRadioButton(ChildRect, "Radio"));
    ChildRect.Set(-11, 70, 41, 89);
    pChild3->Add(new PegRadioButton(ChildRect, "Radio"));
    ChildRect.Set(-11, 48, 43, 67);
    pChild3->Add(new PegRadioButton(ChildRect, "Radio"));
    ChildRect.Set(-11, 28, 42, 47);
    pChild3->Add(new PegRadioButton(ChildRect, "Radio"));
    pChild2->Add(pChild3);

    ((PegNotebook *) pChild1)->SetPageClient(pChild2);

    Add(pChild1);

}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
SIGNED TestDialog::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(IDB_HELLOTHERE, PSF_CLICKED):
        // Enter your code here:
        break;

    case SIGNAL(IDB_SAVE, PSF_CHECK_ON):
        // Enter your code here:
        break;

    case SIGNAL(IDB_SAVE, PSF_CHECK_OFF):
        // Enter your code here:
        break;

    case SIGNAL(IDB_CLOSE, PSF_DOT_ON):
        // Enter your code here:
        break;

    case SIGNAL(IDB_CLOSE, PSF_DOT_OFF):
        // Enter your code here:
        break;

    default:
        return PegDialog::Message(Mesg);
    }
    return 0;
}

