/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pegdemo.hpp - Example program that makes use of a lot of the basic
//  PEG control and window types.
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
//	This example has not been completed. Contact Swell Software to
//	receive an updated version of the PEG library..
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


#ifndef PEGAPP
#define PEGAPP

/*--------------------------------------------------------------------------*/
// Demo Application demonstrating PEG control types.
/*--------------------------------------------------------------------------*/

#define MAIN_APP_WINDOW 500

#ifdef INCLUDE_ROBOT_DEMO
#define ID_ROBOT_WINDOW 501
#endif

enum AppMessage {
    CHILD_CLONE_CLOSED = FIRST_USER_MESSAGE
};

/*--------------------------------------------------------------------------*/
class MainDemoDialog : public PegDialog
{
    public:
        MainDemoDialog(const PegRect &Rect, char *Title);
        SIGNED Message(const PegMessage &Mesg);

    private:

        enum ButtonIds {
            IDB_CONTINUE = 1
        };

        PegTextBox *mpTextBox;
        PegTextButton *mpContinueButton;
        BOOL mbComplete;
        WORD mwStep;
        void CloseLastWindow(void);
};

/*--------------------------------------------------------------------------*/
class DemoDrawWin : public PegWindow
{
    public:
        DemoDrawWin();
        virtual ~DemoDrawWin(){}
        void Draw(void);
        SIGNED Message(const PegMessage &Mesg);

    private:
};

/*--------------------------------------------------------------------------*/
class ButtonDialog : public PegDialog
{
    public:
        ButtonDialog(PegRect &Rect, char *Title);
        SIGNED Message(const PegMessage &Mesg);

    private:
        enum MyButtons {
            IDB_MOVEABLE = 1,
            IDB_FIXED,
            IDB_TOGGLE_GROUP,
            IDB_BITMAP_BTN
        };
            
        PegGroup *mpGroup;

};

/*--------------------------------------------------------------------------*/
class ControlPanelWindow : public PegDecoratedWindow
{
    public:
        ControlPanelWindow (const PegRect &Rect, UCHAR uFlags);
        void Draw(void);
        SIGNED Message(const PegMessage &Mesg);

    private:
        
        enum MyButtons {
            IDB_START = 1,
            IDB_STOP
        };

        void DrawCount(void);
        PegRect mRpmRect;
        PegRect mCountRect;
        PegRect mDividerRect;
        WORD mwCount;
        PegPrompt *mpPrompt;
};

/*--------------------------------------------------------------------------*/
class FloatingWindow : public PegDecoratedWindow
{
    public:
        FloatingWindow(const PegRect &Rect);
        void Draw(void);
        SIGNED Message(const PegMessage &Mesg);
};

/*--------------------------------------------------------------------------*/
class MainAppWindow : public PegDecoratedWindow
{
    public:
        MainAppWindow(PegRect Rect, char *Title, BOOL bFirst = TRUE);
        virtual ~MainAppWindow();
        SIGNED Message(const PegMessage &Mesg);

    private:
        // these are the menu command Ids for the main window menu.

        enum MyButtons {

            IDB_SET_COLOR_BLACK = 1,
            IDB_SET_COLOR_WHITE,
            IDB_SET_COLOR_BLUE,
            IDB_SET_COLOR_DARKGRAY,
            IDB_DESK_COLOR_BLACK,
            IDB_DESK_COLOR_RED,
            IDB_DESK_COLOR_GREEN,
            IDB_DESK_COLOR_BROWN,
            IDB_DESK_COLOR_MAGENTA,
            IDB_DESK_COLOR_CYAN,
            IDB_SET_SAND_PAPER, 
            IDB_SET_POOL_PAPER,
            IDB_SET_NO_PAPER,
            IDB_SET_SIZEABLE,
            IDB_SET_MOVEABLE,
            IDB_DEMO_EXIT,
            IDB_ADD_BUTTON_DIALOG,
            IDB_ADD_PROGWIN,
            IDB_ADD_TWIN,
            IDB_ADD_CONTROL_WIN,
            IDB_ADD_FLOATING_CHILD,
            IDB_ADD_CUSTOM_CHILD,
            IDB_RESTORE,
            IDB_MINIMIZE,
            IDB_MAXIMIZE,
            IDB_SET_STATUS_BAR,
            #ifdef INCLUDE_ROBOT_DEMO
            IDB_ANIMATION,
            #endif
            IDB_HELP_ABOUT

        };

        enum MyMessages {
            TWIN_CLOSED = CHILD_CLONE_CLOSED + 1
        };


        DemoDrawWin *mpDemoWin;
        ControlPanelWindow *mpControlWin;
        FloatingWindow *mpFloater;
        PegMenuButton *mpTwinButton;
};


/*--------------------------------------------------------------------------*/
class DemoListWindow : public PegDecoratedWindow
{
    public:

        DemoListWindow(PegRect Rect, char *Title);
        SIGNED Message(const PegMessage &Mesg);

        enum ChildIds {
            ID_VERT_LIST = 1,
            ID_COMBO_BOX,
            ID_BUTTON1,
            ID_BUTTON2,
            ID_BUTTON3,
            ID_BUTTON4,
            ID_BUTTON5,
            ID_BUTTON6,
            ID_BUTTON7,
            ID_BUTTON8
        };

    private:
};

/*--------------------------------------------------------------------------*/
class DemoSliderWindow : public PegDecoratedWindow
{
    public:
        DemoSliderWindow(PegRect Rect, char *Title);
        SIGNED Message(const PegMessage &Mesg);

    private:
        enum SliderIds {
            IDS_SLIDER1 = 1,
            IDS_SLIDER2,
            IDS_SLIDER3,
            IDS_SLIDER4,
            IDS_SLIDER5,
            IDS_SLIDER6,
            IDS_SLIDER7,
            IDS_SLIDER8
        };
        PegSlider *mpSlider;
        void UpdateSliderPrompt(WORD wId, LONG lVal);

        SIGNED miSlider5Val;
};


/*--------------------------------------------------------------------------*/
class StringWindow : public PegDecoratedWindow
{
    public:
        StringWindow(PegRect Rect, char *Title);
        SIGNED Message(const PegMessage &Mesg);

    private:
        enum MyButtons {
            IDB_TBCLR_WHITEONRED = 1,
            IDB_TBCLR_GREENONBLACK,
            IDB_TBCLR_BLACKONWHITE,
            IDB_TBFONT_SCRIPT,
            IDB_TBFONT_SYSTEM,
            IDB_ALLOW_TB_EDIT,
            IDB_STRCLR_WHITEONRED,
            IDB_STRCLR_GREENONBLACK,
            IDB_STRCLR_BLACKONWHITE,
            IDB_STRFONT_MENU, 
            IDB_STRFONT_SYSTEM,
            IDB_ALLOW_STRING_EDIT
        };

        PegString *mpString;
        PegTextBox *mpTextBox;
};

/*--------------------------------------------------------------------------*/
#define NUM_PROG_BARS 6

class ProgBarWindow : public PegDecoratedWindow
{
    public:
        ProgBarWindow(PegRect Rect, char *Title);
        SIGNED Message(const PegMessage &Mesg);

    private:
        PegProgressBar *mpProgBars[NUM_PROG_BARS];
        SIGNED miValue;
};

#endif



