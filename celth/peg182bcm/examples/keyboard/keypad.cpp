/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//    Copyright (c) 1999-2002, Swell Software, Inc.
//  
//    All Rights Reserved
//  
//  Notes:
//
//  This window class presents a QWERTY keyboard in a 320x240 window area.
//  
//  The user can operate the keyboard with a mouse or touch screen, -OR-
// 
//  If PEG_KEYBOARD_SUPPORT is turned on, the keyboard will operate with
//  only UP, DOWN, LEFT, RIGHT, and ENTER keys.
//
//  *** This example is designed to run with the     ***
//  *** configuration flag PEG_DRAW_FOCUS turned on. ***
//
//  This example also demonstrates one method of overriding the default
//  "DrawFocusIndicator()" operation. Rather than the default (adding a gray
//  rectangle), we will change the color of each button when it has focus.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#include "keypad.hpp"

/*--------------------------------------------------------------------------*/
// PegAppInitialize- called by the PEG library during program startup.
/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresentation)
{
	// create the dialog and add it to PegPresentationManager:

	pPresentation->Add(new KeyboardWin());
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// KeypadButton class- PegTextButton derived class that draws in a different
// color when it has focus.
/*--------------------------------------------------------------------------*/

KeypadButton::KeypadButton(PegRect &Where, char *text, WORD wId, WORD wStyle) :
    PegTextButton(Where, text, wId, wStyle)
{
    RemoveStatus(PSF_ACCEPTS_FOCUS);
}

/*--------------------------------------------------------------------------*/
// Override the default operation to prevent the gray rectangle focus
// indicator.
/*--------------------------------------------------------------------------*/
void KeypadButton::DrawFocusIndicator(BOOL bDraw)
{
    SetColor(PCI_NORMAL, BLUE);
    SetColor(PCI_NTEXT, WHITE);
    if (bDraw)
    {
        Invalidate();
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
// Override the default operation to prevent the gray rectangle focus
// indicator
/*--------------------------------------------------------------------------*/
void KeypadButton::EraseFocusIndicator(void)
{
    SetColor(PCI_NORMAL, PCLR_BUTTON_FACE);
    SetColor(PCI_NTEXT, BLACK);
    Invalidate();
    Draw();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
KeyboardWin::KeyboardWin() : PegDialog()
{
    PegRect ChildRect;

    mReal.Set(0, 0, 319, 239);
    InitClient();
    Add (new PegTitle("Enter Value", TF_CLOSEBUTTON));

    ChildRect.Set(205, 210, 284, 231);
    Add(new KeypadButton(ChildRect, "Cancel", IDB_CANCEL, AF_ENABLED));
    ChildRect.Set(121, 210, 200, 231);
    Add(new KeypadButton(ChildRect, "Clear", IDB_CLEAR, AF_ENABLED));
    ChildRect.Set(37, 210, 116, 231);
    Add(new KeypadButton(ChildRect, "OK", IDB_OK, AF_ENABLED));
    ChildRect.Set(33, 178, 81, 199);
    Add(new KeypadButton(ChildRect, "Shift", IDB_SHIFT));

    ChildRect.Set(85, 178, 236, 199);
    mpSpaceKey = new KeypadButton(ChildRect, " ", IDB_LETTER);
    Add(mpSpaceKey);

    ChildRect.Set(241, 178, 289, 199);
    Add(new KeypadButton(ChildRect, "BS", IDB_BACK));

    ChildRect.Set(33, 29, 286, 48);
//    mpValue = new PegPrompt(ChildRect, "_", IDP_VALUE, FF_RECESSED|TJ_LEFT|TT_COPY);
//    Add(mpValue);
    
    mpValue = new PegString(ChildRect, NULL, IDP_VALUE, FF_RECESSED|TJ_LEFT|TT_COPY|EF_EDIT);
//    Add(mpValue);

    ChildRect.Set(292, 61, 313, 82);
    Add(new KeypadButton(ChildRect, "+", IDB_LETTER));
    ChildRect.Set(266, 61, 287, 82);
    Add(new KeypadButton(ChildRect, "_", IDB_LETTER));
    ChildRect.Set(240, 61, 261, 82);
    Add(new KeypadButton(ChildRect, ")", IDB_LETTER));
    ChildRect.Set(214, 61, 235, 82);
    Add(new KeypadButton(ChildRect, "(", IDB_LETTER));
    ChildRect.Set(188, 61, 209, 82);
    Add(new KeypadButton(ChildRect, "*", IDB_LETTER));
    ChildRect.Set(162, 61, 183, 82);
    Add(new KeypadButton(ChildRect, "&", IDB_LETTER));
    ChildRect.Set(136, 61, 157, 82);
    Add(new KeypadButton(ChildRect, "^", IDB_LETTER));
    ChildRect.Set(110, 61, 131, 82);
    Add(new KeypadButton(ChildRect, "%", IDB_LETTER));
    ChildRect.Set(84, 61, 105, 82);
    Add(new KeypadButton(ChildRect, "$", IDB_LETTER));
    ChildRect.Set(58, 61, 79, 82);
    Add(new KeypadButton(ChildRect, "#", IDB_LETTER));
    ChildRect.Set(32, 61, 53, 82);
    Add(new KeypadButton(ChildRect, "@", IDB_LETTER));

    ChildRect.Set(6, 61, 27, 82);
    mpOneKey = new KeypadButton(ChildRect, "!", IDB_LETTER);
    Add(mpOneKey);

    ChildRect.Set(267, 150, 288, 171);
    mpQuestionKey = new KeypadButton(ChildRect, "?", IDB_LETTER);
    Add(mpQuestionKey);

    ChildRect.Set(241, 150, 262, 171);
    Add(new KeypadButton(ChildRect, ">", IDB_LETTER));
    ChildRect.Set(215, 150, 236, 171);
    Add(new KeypadButton(ChildRect, "<", IDB_LETTER));
    ChildRect.Set(189, 150, 210, 171);
    Add(new KeypadButton(ChildRect, "M", IDB_LETTER));
    ChildRect.Set(163, 150, 184, 171);
    Add(new KeypadButton(ChildRect, "N", IDB_LETTER));
    ChildRect.Set(137, 150, 158, 171);
    Add(new KeypadButton(ChildRect, "B", IDB_LETTER));
    ChildRect.Set(111, 150, 132, 171);
    Add(new KeypadButton(ChildRect, "V", IDB_LETTER));
    ChildRect.Set(85, 150, 106, 171);
    Add(new KeypadButton(ChildRect, "C", IDB_LETTER));
    ChildRect.Set(59, 150, 80, 171);
    Add(new KeypadButton(ChildRect, "X", IDB_LETTER));

    ChildRect.Set(33, 150, 54, 171);
    mpZKey = new KeypadButton(ChildRect, "Z", IDB_LETTER);
    Add(mpZKey);

    ChildRect.Set(280, 122, 301, 143);
    Add(new KeypadButton(ChildRect, "\"", IDB_LETTER));
    ChildRect.Set(254, 122, 275, 143);
    Add(new KeypadButton(ChildRect, ":", IDB_LETTER));
    ChildRect.Set(228, 122, 249, 143);
    Add(new KeypadButton(ChildRect, "L", IDB_LETTER));
    ChildRect.Set(202, 122, 223, 143);
    Add(new KeypadButton(ChildRect, "K", IDB_LETTER));
    ChildRect.Set(176, 122, 197, 143);
    Add(new KeypadButton(ChildRect, "J", IDB_LETTER));
    ChildRect.Set(150, 122, 171, 143);
    Add(new KeypadButton(ChildRect, "H", IDB_LETTER));
    ChildRect.Set(124, 122, 145, 143);
    Add(new KeypadButton(ChildRect, "G", IDB_LETTER));
    ChildRect.Set(98, 122, 119, 143);
    Add(new KeypadButton(ChildRect, "F", IDB_LETTER));
    ChildRect.Set(72, 122, 93, 143);
    Add(new KeypadButton(ChildRect, "D", IDB_LETTER));
    ChildRect.Set(46, 122, 67, 143);
    Add(new KeypadButton(ChildRect, "S", IDB_LETTER));
    ChildRect.Set(20, 122, 41, 143);
    Add(new KeypadButton(ChildRect, "A", IDB_LETTER));
    ChildRect.Set(292, 94, 313, 115);
    Add(new KeypadButton(ChildRect, "}", IDB_LETTER));
    ChildRect.Set(266, 94, 287, 115);
    Add(new KeypadButton(ChildRect, "{", IDB_LETTER));
    ChildRect.Set(240, 94, 261, 115);
    Add(new KeypadButton(ChildRect, "P", IDB_LETTER));
    ChildRect.Set(214, 94, 235, 115);
    Add(new KeypadButton(ChildRect, "O", IDB_LETTER));
    ChildRect.Set(188, 94, 209, 115);
    Add(new KeypadButton(ChildRect, "I", IDB_LETTER));
    ChildRect.Set(162, 94, 183, 115);
    Add(new KeypadButton(ChildRect, "U", IDB_LETTER));
    ChildRect.Set(136, 94, 157, 115);
    Add(new KeypadButton(ChildRect, "Y", IDB_LETTER));
    ChildRect.Set(110, 94, 131, 115);
    Add(new KeypadButton(ChildRect, "T", IDB_LETTER));
    ChildRect.Set(84, 94, 105, 115);
    Add(new KeypadButton(ChildRect, "R", IDB_LETTER));
    ChildRect.Set(58, 94, 79, 115);
    Add(new KeypadButton(ChildRect, "E", IDB_LETTER));
    ChildRect.Set(32, 94, 53, 115);
    Add(new KeypadButton(ChildRect, "W", IDB_LETTER));
    ChildRect.Set(6, 94, 27, 115);
    Add(new KeypadButton(ChildRect, "Q", IDB_LETTER));
    /* WB End Construction */

    mbCaps = TRUE;
    Add(mpValue);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED KeyboardWin::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case SIGNAL(IDB_CLEAR, PSF_CLICKED):
        //mpValue->DataSet("_");
        mpValue->DataSet(NULL);
        mpValue->Draw();
        // Enter your code here:
        break;

    case SIGNAL(IDB_OK, PSF_CLICKED):
        // Enter your code here:

        // here is where you would catch the value from mpValue using
        // DataGet(), and do something with it. 

        return PegDialog::Message(Mesg);

    case SIGNAL(IDB_LETTER, PSF_CLICKED):
        // Enter your code here:
        
        //if (strlen(mpValue->DataGet()) < 24)
        {
            KeypadButton *pb = (KeypadButton *) Mesg.pSource;
//            char cTemp[28];
//            strcpy(cTemp, mpValue->DataGet());
//            cTemp[strlen(cTemp) - 1] = *(pb->DataGet());
//            strcat(cTemp, "_");
//            mpValue->DataSet(cTemp);
            
//            mpValue->Draw();
            PegMessage KeyMesg(mpValue, PM_KEY);
            KeyMesg.iData = *(pb->DataGet());
            KeyMesg.lData = 0;
            mpValue->Message(KeyMesg);
        }
        break;

    case SIGNAL(IDB_BACK, PSF_CLICKED):
        // Enter your code here:
        if (strlen(mpValue->DataGet()) > 1)
        {
            char cTemp[120];
            strcpy(cTemp, mpValue->DataGet());
            cTemp[strlen(cTemp) - 2] = '_';
            cTemp[strlen(cTemp) - 1] = '\0';
            mpValue->DataSet(cTemp);
            mpValue->Draw();
        }
        break;

    case SIGNAL(IDB_SHIFT, PSF_CLICKED):
        // Enter your code here:
        ToggleShift();
        break;

    default:
        return PegDialog::Message(Mesg);
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

// KeyTable contains the character placed on each button, for both shift
// unshifted state.

struct KeyEntry {
    PEGCHAR cUpper;
    PEGCHAR cLower;
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
KeyEntry KeyTable[] = {

{ '!', '1'},     
{ '@', '2'},     
{ '#', '3'},     
{ '$', '4'},     
{ '%', '5'},     
{ '^', '6'},     
{ '&', '7'},     
{ '*', '8'},     
{ '(', '9'},     
{ ')', '0'},     
{ '_', '-'},     
{ '+', '='},     
{ 'Q', 'q'},     
{ 'W', 'w'},     
{ 'E', 'e'},     
{ 'R', 'r'},     
{ 'T', 't'},     
{ 'Y', 'y'},     
{ 'U', 'u'},     
{ 'I', 'i'},     
{ 'O', 'o'},     
{ 'P', 'p'}, 
{ '{', '['},     
{ '}', ']'},     
{ 'A', 'a'},     
{ 'S', 's'},     
{ 'D', 'd'},     
{ 'F', 'f'},     
{ 'G', 'g'},     
{ 'H', 'h'},     
{ 'J', 'j'},     
{ 'K', 'k'},     
{ 'L', 'l'},     
{ ':', ';'},     
{ '\"', '\''},     
{ 'Z', 'z'},     
{ 'X', 'x'},     
{ 'C', 'c'},     
{ 'V', 'v'},     
{ 'B', 'b'},     
{ 'N', 'n'},     
{ 'M', 'm'},     
{ '<', ','},     
{ '>', '.'},     
{ '?', '/'},
{ ' ', ' '},        // the space button
{ '\0', '\0'}    // array terminator

};



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void KeyboardWin::ToggleShift(void)
{
    KeypadButton *pButton;
    KeyEntry *pEntry;
    char cTemp[2];
    cTemp[1] = '\0';

    // for each key, find it's table entry and change the letter on the key:

    PegThing *pTest = First();

    while(pTest)
    {
        if (pTest->Type() == TYPE_TEXTBUTTON)
        {
            pButton = (KeypadButton *) pTest;
            char *cGet = pButton->DataGet();
            if (strlen(cGet) == 1)
            {
                // try to find this guy in the table:
                pEntry = KeyTable;
                while(pEntry->cUpper)
                {
                    if (mbCaps)
                    {
                        if (pEntry->cUpper == *cGet)
                        {
                            cTemp[0] = pEntry->cLower;
                            pButton->DataSet(cTemp);
                            break;
                        }
                    }
                    else
                    {
                        if (pEntry->cLower == *cGet)
                        {
                            cTemp[0] = pEntry->cUpper;
                            pButton->DataSet(cTemp);
                            break;
                        }
                    }
                    pEntry++;
                }
            }
        }
        pTest = pTest->Next();
    }
    Invalidate();
    Draw();
    mbCaps = !mbCaps;
}




