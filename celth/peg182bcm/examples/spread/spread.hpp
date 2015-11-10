/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// spread.hpp - PegSpreadSheet demonstration window.
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


#define SS_ROWS 16
#define SS_COLS 12

class SpreadSheetWin : public PegDecoratedWindow
{
    public:
        SpreadSheetWin(const PegRect &Rect);
        SIGNED Message(const PegMessage &Mesg);

    private:

        enum  {
            ID_SPREADSHEET = 1,
            IDM_COLOR_DEFAULT,
            IDM_COLOR_BLUES,
            IDM_COLOR_REDS,
            IDM_COLOR_GREENS,
            IDM_CELL_RAISED,
            IDM_CELL_RECESSED,
            IDM_CELL_FLAT,
            IDM_JUSTIFY_LEFT,
            IDM_JUSTIFY_RIGHT,
            IDM_JUSTIFY_CENTER,
            IDM_ADD_COLUMN,
            IDM_REMOVE_COLUMN,
            IDM_INSERT_COLUMN,
            IDM_ADD_ROW,
            IDM_REMOVE_ROW,
            IDM_INSERT_ROW,
            IDM_ABOUT,
            IDM_ROW_SEL,
            IDM_COL_SEL,
            IDM_CELL_SELECT,
            IDM_MULT_ROW_SEL,
            IDM_MULT_COL_SEL,
            IDM_PARTIAL_COL
        };

        PegSpreadSheet *mpSpread;
        PegPrompt *mpStatPrompt;
        LONG mCount;

        void ChangeColors(COLORVAL *pClr);
        void SetSelectedColumnJustification(WORD wJustify);
        void SetSelectedColumnFrame(WORD wFrame);
        void RemoveSelectedColumns(void);
        void RemoveSelectedRows(void);
        void UpdateDynamicCell(void);

        void DisplayCellSelect(PegPoint Cell);
        void DisplayColSelect(SIGNED iCol);
        void DisplayRowSelect(SIGNED iRow);

};



