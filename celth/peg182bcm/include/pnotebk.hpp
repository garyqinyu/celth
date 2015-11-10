/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pnotebk.hpp - Notebook window class definition.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2000 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// Notebook windows have tabs at the top or bottom. The number of tabs
// is defined when the notebook window is constructed. What appears on each
// tab may be either simple text or any PegThing derived object type. 
// Likewise, a PegWindow derived object should be associated with
// each tab. The Notebook window takes care of displaying the correct window
// as each tab is selcted.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGNOTEBOOK_
#define _PEGNOTEBOOK_

class PegNotebook : public PegWindow
{
    public:
        PegNotebook(const PegRect &Rect, WORD wStyle, UCHAR uNumTabs);
        ~PegNotebook();
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);
        void SetTabClient(UCHAR uIndex, PegThing *TabClient);
        void SetPageClient(UCHAR uIndex, PegThing *PageClient);
        PegThing *GetPageClient(UCHAR uIndex);
        PegThing *RemovePageClient(UCHAR uIndex);
        PegThing *GetTabClient(UCHAR uIndex);
        UCHAR GetCurrentPage(void) {return muSelectedTab;}
        void SetTabString(UCHAR uIndex, const PEGCHAR *Text);
        void ResetTabStyle(WORD wStyle);
        void ResetNumTabs(UCHAR uNum);
        void SetFont(PegFont *pFont) {mpFont = pFont;}
        void SelectTab(UCHAR uTab);

        enum MyIds {
            NB_SPIN = 1000
        };

    protected:
        BOOL CheckTabClick(PegPoint Click);
        void DrawTabs(BOOL bSelected);
        void RedrawTabs(void);
        void DrawFrame(BOOL bFill = TRUE);
        void CalculateTabPositions(void);
        void AddCurrentPageClient(BOOL bDraw = FALSE);

        PEGCHAR **mpTabStrings;
        PegThing **mpTabClients;
        PegThing **mpPageClients;
        PegFont *mpFont;
        PegSpinButton *mpSpin;

        UCHAR muSelectedTab;
        UCHAR muNumTabs;
        UCHAR muVisibleTabs;
        UCHAR muFirstTab;
        SIGNED miTabWidth;
        SIGNED miTabHeight;
};

#endif


