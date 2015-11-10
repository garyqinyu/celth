/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pspread.hpp - PegSpreadSheet class definition.
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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGSPREADSHEET_
#define _PEGSPREADSHEET_

#define SHEET_BORDER 6
#define SHEET_HEADER_SPACE 4
#define SHEET_CELL_PADDING 8
#define SHEET_SEPERATOR_WIDTH 4
#define SHEET_MAX_HEADER_LINES 4


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegSpreadSheetCell
{
    public:
        PegSpreadSheetCell();
        ~PegSpreadSheetCell();
        void DataSet(const PEGCHAR *pData);
        PEGCHAR *mpString;
        BOOL bSelected;   
        SIGNED iWidth;
};


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegSpreadSheetRow
{
    public:

        PegSpreadSheetRow(SIGNED iCols);
        ~PegSpreadSheetRow(void);
    
        PEGCHAR *DataGet(SIGNED iCol);
        void  DataSet(SIGNED iCol, const PEGCHAR *data);
        void  SetRowSelected (BOOL bSelect);

        BOOL  RowSelected(void)
        {
            return ((mwRowStyle & SRF_SELECTED) ? TRUE : FALSE);
        }

        WORD  RowStyle(void) {return(mwRowStyle);}
        void  SetRowStyle(WORD wStyle) {mwRowStyle = wStyle;}
        void  SetCellSelected (SIGNED iCol, BOOL bSelect);
        BOOL  GetCellSelected (SIGNED iCol);
        void  SetRowHeader (const PEGCHAR *header);
        PEGCHAR* GetRowHeader (void) {return mpRowHeader;} 

        void DeleteColumn(SIGNED iCol);
        void InsertColumn(SIGNED iCol, PEGCHAR *pData = NULL);
    
    protected:
        PegSpreadSheetCell **mpCellData;
        SIGNED      miColumns;
        WORD        mwRowStyle;    
        SIGNED      miRowDataLength;
        PEGCHAR     *mpRowHeader;
};



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegSpreadSheet : public PegWindow
{
    public:

        PegSpreadSheet(const PegRect &Rect, SIGNED iRows, SIGNED iCols,
            WORD wId = 0,
            WORD wStyle = FF_RAISED|SS_CELL_SELECT|SS_PARTIAL_COL,
            PegThing *pOwner= NULL);

        virtual ~PegSpreadSheet(void);
        virtual void Draw(void);
        virtual SIGNED Message(const PegMessage &Mesg);

        void SetCellFont(PegFont *pFont);
        void SetColumnFont(SIGNED iCol, PegFont *pFont);
        void SetHeaderFont(PegFont *pFont);
        void SetColor(const UCHAR index, const COLORVAL Color);

        void SetCellData(const SIGNED iRow, const SIGNED iCol,
            const PEGCHAR *Text, BOOL bRedraw = FALSE);
//        SIGNED  GetSelectedCellRow() const;
//        SIGNED  GetSelectedCellCol() const;

        PEGCHAR *GetCellData(const SIGNED iRow, const SIGNED iCol) const;
        PEGCHAR *GetRowHeader(const SIGNED iRow) const;
        PEGCHAR *GetColHeader(const SIGNED iCol) const;

        void SetHeader(const SIGNED line, const SIGNED iCol, const PEGCHAR *Text);
        void SetRowHeader(const SIGNED iRow, const PEGCHAR *charVal);
        void SetFooter(const SIGNED line, const SIGNED iCol, const PEGCHAR *Text);

        void SetColumnStyle(const SIGNED iCol, const WORD wFlags);
        WORD GetColumnStyle(const SIGNED iCol)
        {
            return mpColFlags[iCol];
        }

        void SetRowStyle(const SIGNED iRow, const WORD wFlags);
        WORD GetRowStyle(const SIGNED iRow);

        void SetScrollStartCol(const SIGNED iCol);
        void ForceColWidth(SIGNED col, SIGNED iWidth);

        SIGNED GetOptimumWidth(void);
        SIGNED GetOptimumHeight(void);
        void Resize(PegRect Rect);

        SIGNED GetRows(void) const { return miRows; }
        SIGNED GetCols(void) const { return miCols; }
        void SetSize(const SIGNED iRows, const SIGNED iCols);

        SIGNED AddColumn(SIGNED iWidth, PEGCHAR *pHeader = NULL);
        SIGNED InsertColumn(SIGNED iColumn, SIGNED iWidth, PEGCHAR *pHeader);
        SIGNED DeleteColumn(SIGNED iColumn);

        SIGNED AddRow(PEGCHAR *pHeader = NULL);
        SIGNED InsertRow(SIGNED iRow, PEGCHAR *pHeader = NULL);
        SIGNED DeleteRow(SIGNED iRow);

        SIGNED GetDispRows(void) {return miVisibleRows;}            
        SIGNED GetDispCols(void) {return miVisibleCols;} 
        SIGNED GetSelectedColumn(SIGNED iIndex = 0);
        SIGNED GetSelectedRow(SIGNED iIndex = 0);

        void SelectRow(const SIGNED iRow, BOOL bSet = TRUE);
        void SelectColumn(const SIGNED iCol, BOOL bSet = TRUE);
        void SelectCell(SIGNED iRow, SIGNED iCol, BOOL bSet = TRUE);

        BOOL RowSelected(const SIGNED iRow);
        BOOL ColSelected(const SIGNED iCol);
        BOOL CellSelected(const SIGNED iRow, const SIGNED iCol);

        void UnselectAll(void);
        BOOL UnselectRows(void);
        BOOL UnselectColumns(void);
		BOOL UnselectCells(void);

        PegRect GetCellRect(const SIGNED iRow, const SIGNED iCol);
        BOOL RedrawOneCell(const SIGNED iRow, const SIGNED iCol);
        SIGNED  UpdateRowLayout(BOOL bForceVertical = FALSE);
        SIGNED  UpdateColLayout();
        void UpdateScrollBars(void);
        void VScrollIntoView(const SIGNED iRow);


        void DrawAllCells(void);
        void DrawRowHeaders (void);
        void DrawHeaders (void);
        void DrawFooters (void);

    protected:

        enum ScrollIds {
            IDB_VSCROLL = 1000,
            IDB_HSCROLL
        };

        void SetColWidth(SIGNED col, const PEGCHAR *str, PegFont *pFont);
        void SelectCell(const PegPoint &pos);
        void CheckCellSelect(const PegPoint &pos);
        BOOL CheckColumnSelect(const PegPoint &pos);
        void FoundColumnSelect(SIGNED iCol);
        BOOL CheckRowSelect(const PegPoint &pos);
        SIGNED TopBorder(void);
        inline SIGNED LeftBorder(void);

        COLORVAL GetCellBackColor(const SIGNED iRow, const SIGNED iCol) const;
        COLORVAL GetCellTextColor(const SIGNED iRow, const SIGNED iCol) const;

        void DrawCellBorder(PegRect &CellRect, COLORVAL BackColor, SIGNED row,
            SIGNED column);
        void DrawFlat(PegRect &CellRect, COLORVAL BackColor);
        void DrawRecessed(PegRect &CellRect, COLORVAL BackColor, SIGNED row,
            SIGNED column);
        void DrawRaised(PegRect &CellRect, COLORVAL BackColor, SIGNED row,
            SIGNED column);

        void UpdateCell(PegRect &CellRect, const PEGCHAR *Text, WORD wJustify,
            PegFont *pFont, COLORVAL InColor);
     
        void DoVerticalScrolling  (const SIGNED iAmount);
        void DoHorizontalScrolling(const SIGNED iAmount);
    
        void ReconstructHeaderData(const SIGNED line);
        void ReconstructFooterData(const SIGNED line); 
        void CheckColWidths(SIGNED col, SIGNED iWidth);

        void ClipToFace(void);
        void EndClipToFace(void);

        PegSpreadSheetRow **mpRowData;       
        PegSpreadSheetRow **mpHeaderData;  
        PegSpreadSheetRow **mpFooterData;  
        WORD *mpColFlags;    
        PegFont **mpColFonts;
        PegFont *mpHeaderFont;
        SIGNED  *mpColWidths;
        PegThing *mpOwner;
        PegVScroll *mpVScroll;
        PegHScroll *mpHScroll;

        SIGNED miRows;
        SIGNED miCols;
        SIGNED miClipCount;
        SIGNED miStartScroll;
        SIGNED miNonScrollWidth;
        SIGNED miStartRow;        
        SIGNED miEndRow;        
        SIGNED miStartCol;      
        SIGNED miEndCol;        
        SIGNED miVisibleRows;       
        SIGNED miVisibleCols;
        SIGNED miDispColsWidth;    
        SIGNED miHeaderLines;      
        SIGNED miFooterLines;      
        SIGNED miMaxRowHeaderWidth;
        BOOL mbLastColClipped;
        BOOL mbNeedHScroll;

        SIGNED miCellHeight;
        SIGNED miHeaderFontHeight;
        PegRect mOldClip;
    
        COLORVAL   muTblColors[9];
};


#endif


