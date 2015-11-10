/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// notebook.cpp - PegNotebook demonstration window.
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

#define NUM_TABS 4

class TabWindow : public PegDecoratedWindow
{
    public:
        TabWindow(const PegRect &Rect);
        SIGNED Message(const PegMessage &Mesg);

    private:

        enum  {
            IDB_RAISED_BORDER = 1,
            IDB_THICK_BORDER,
            IDB_TOP_TABS,
            IDB_BOTTOM_TABS,
            IDB_TEXT_TABS,
            IDB_CUSTOM_TABS,
            IDB_DIALOG_FILL,
            IDB_CLIENT_FILL,
        };

        void SetNotebookStyle(WORD wOrFlag, WORD wAndFlag);
        void SetTextTabs(void);
        void SetCustomTabs(void);
        void SetNotebookClients(void);
        void ResetNumPages(UCHAR);
        void UpdatePrompt(void);

        SIGNED miPromptVal;
        PegPrompt *mpPrompt;
        PegNotebook *mpNotebook;
};

class TestWindow : public PegWindow
{
    public:
        TestWindow(const PegRect &Size, WORD wStyle);
        void Draw(void);
};

