/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ptable.hpp - PegTable class definition.
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
// PEG Version:  1.8.0
// File Version: 1.8.0.2
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGTABLE_
#define _PEGTABLE_

struct TableCellInfo
{
    TableCellInfo() 
    {
        pChild = NULL;
        pNext = NULL; 
        iRowSpan = 1; 
        iColSpan = 1; 
        wStyle = TCF_HCENTER | TCF_VCENTER;
    }
    PegThing *pChild;
    TableCellInfo *pNext;
    SIGNED iRow;
    SIGNED iCol;
    SIGNED iRowSpan;
    SIGNED iColSpan;
    //BOOL bForceFit;
    WORD wStyle;    // 0x0001 bit is for ForceFit, upper byte for justification
};


class PegTable : public PegWindow
{
    public:

        PegTable(SIGNED iLeft, SIGNED iTop, SIGNED iRows, SIGNED iCols);
        ~PegTable();
        virtual void Draw(void);
        virtual void DrawGrid(void);
        virtual void DrawChildren(void);
        virtual void SetCellPadding(SIGNED iPad) {miCellPad = iPad;}
        void SetGridWidth(SIGNED iWidth = PEG_FRAME_WIDTH);
        virtual void SetColWidth(SIGNED iCol, SIGNED iWidth, 
                                 BOOL bForce = FALSE);
        virtual void SetRowHeight(SIGNED iRow, SIGNED iHeight, 
                                  BOOL bForce = FALSE);
        virtual void SetCellClient(SIGNED iRow, SIGNED iCol, PegThing *Child,
            SIGNED iRowSpan = 1, SIGNED iColSpan = 1, 
            WORD wStyle = TCF_HCENTER | TCF_VCENTER);

        virtual void Reconfigure(SIGNED iRows, SIGNED iCols,
            SIGNED iGridWidth, SIGNED iCellPad);

        PegThing *GetCellClient(SIGNED iRow, SIGNED iCol);
        virtual void Layout(void);
        SIGNED GetRows(void) {return miRows;}
        SIGNED GetColumns(void) {return miCols;}
        SIGNED GetGridWidth(void) {return miGridWidth;}
        SIGNED GetCellPadding(void) {return miCellPad;}
        BOOL GetRowCol(PegThing *pChild, SIGNED *pRow, SIGNED *pCol);
        PegThing *RemoveCellClient(SIGNED iRow, SIGNED iCol);


    protected:

        BOOL CellSpansRow(SIGNED iRow, SIGNED iCol);
        BOOL CellSpansCol(SIGNED iRow, SIGNED iCol);

        SIGNED *mpCellHeights;
        SIGNED *mpCellWidths;
        TableCellInfo *mpCellInfo;
        SIGNED miCellPad;
        SIGNED miGridWidth;
        SIGNED miRows;
        SIGNED miCols;
};

#endif

