/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pspread.cpp - PegSpreadSheet class definition.
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

#include "string.h"
#include "peg.hpp"

extern PEGCHAR lsTEST[];
extern PEGCHAR lsSPACE[];

PegSpreadSheet::PegSpreadSheet (const PegRect &InRect, SIGNED iRows,
    SIGNED iCols, WORD wId, WORD wStyle, PegThing *pOwner) :
    PegWindow(InRect, wStyle),
    mpRowData(NULL),
    mpHeaderData(NULL),
    mpFooterData(NULL),
    mpColFlags(NULL),
    mpColFonts(NULL),
    mpHeaderFont(PegTextThing::GetDefaultFont(PEG_HEADER_FONT)),
    mpColWidths(NULL),
    mpOwner(pOwner),
    mpVScroll(NULL),
    mpHScroll(NULL),
    miRows(iRows),
    miCols(iCols),
    miClipCount(0),
    miStartScroll(0),
    miNonScrollWidth(0),
    miStartRow(0),
    miEndRow(0),    
    miStartCol(0),
    miEndCol(0),
    miVisibleRows(0),    
    miVisibleCols(0),
    miDispColsWidth(0),
    miHeaderLines(0),
    miFooterLines(0),
    miMaxRowHeaderWidth(0),
    mbLastColClipped(FALSE),
    mbNeedHScroll(FALSE)
{
    Id(wId);
    miHeaderFontHeight = TextHeight(lsTEST, mpHeaderFont);
    SIGNED iHeight = TextHeight(lsTEST, PegTextThing::GetDefaultFont(PEG_CELL_FONT));
    miCellHeight = iHeight + iHeight / 2;

    Type(TYPE_SPREADSHEET);

    // set the default colors:

    muColors[PCI_NORMAL]   = PCLR_DIALOG;
    muColors[PCI_NTEXT]    = PCLR_NORMAL_TEXT;
    muColors[PCI_SELECTED] = PCLR_HIGH_TEXT_BACK;
    muColors[PCI_STEXT]    = PCLR_HIGH_TEXT;

    muTblColors[PCI_SS_COLHEADBACK - THING_COLOR_LIST_SIZE] = PCLR_DIALOG;
    muTblColors[PCI_SS_COLHEADTEXT - THING_COLOR_LIST_SIZE] = PCLR_HIGH_TEXT;
    muTblColors[PCI_SS_ROWHEADBACK - THING_COLOR_LIST_SIZE] = PCLR_HIGH_TEXT;
    muTblColors[PCI_SS_ROWHEADTEXT - THING_COLOR_LIST_SIZE] = PCLR_NORMAL_TEXT;
    muTblColors[PCI_SS_DIVIDER - THING_COLOR_LIST_SIZE]   = PCLR_LOWLIGHT;
    muTblColors[PCI_SS_BACKGROUND - THING_COLOR_LIST_SIZE] = PCLR_DIALOG;
    SetSize(miRows,miCols);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegSpreadSheet::~PegSpreadSheet(void)
{
    if (mpColWidths)
    {
        delete [] mpColWidths;
    }
    if (mpColFlags)
    {
        delete [] mpColFlags;
    }
    if (mpColFonts)
    {
        delete mpColFonts;
    }
    
    if (mpRowData)    
    {
        for (SIGNED i = 0; i < miRows; i++)
        {
            PegSpreadSheetRow *pDelThis = mpRowData[i];
            if (pDelThis)
            {
                delete pDelThis;
            }
        }
        delete mpRowData;
    }
    
    if (mpHeaderData)    
    {
        for (SIGNED i = 0; i < miHeaderLines; i++)
        {
            PegSpreadSheetRow *pDelThis = mpHeaderData[i];
            if (pDelThis)
            {
                delete pDelThis;
            }
        }
        delete mpHeaderData;
    }
    
    if (mpFooterData)
    {
        for (SIGNED i = 0; i < miFooterLines; i++)
        {
            PegSpreadSheetRow *pDelThis = mpFooterData[i];
            if (pDelThis)
            {
                delete pDelThis;
            }
        }
        delete mpFooterData;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_SHOW:
        PegWindow::Message(Mesg);
        UpdateScrollBars();

        if (!mpOwner)
        {
            mpOwner = Parent();
        }
        break;

    case PM_LBUTTONDOWN:
        SelectCell(Mesg.Point);
        break;

    case PEG_SIGNAL(IDB_VSCROLL, PSF_SCROLL_CHANGE):
        DoVerticalScrolling((SIGNED) Mesg.lData);
        break;
        
    case PEG_SIGNAL(IDB_HSCROLL, PSF_SCROLL_CHANGE):
        DoHorizontalScrolling((SIGNED) Mesg.lData);
        break;
    
    default:
        return(PegWindow::Message(Mesg));
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::Draw(void)
{
    COLORVAL uTemp = muColors[PCI_NORMAL];
    muColors[PCI_NORMAL] = muTblColors[PCI_SS_BACKGROUND - THING_COLOR_LIST_SIZE];
    BeginDraw();
    DrawFrame();
    muColors[PCI_NORMAL] = uTemp;

    ClipToFace();
    DrawHeaders  ();
    DrawAllCells ();
    DrawFooters  ();
    DrawRowHeaders ();
    EndClipToFace();

    DrawChildren();
    EndDraw();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::ClipToFace(void)
{
    if (!miClipCount)
    {
        mOldClip = mClip;
        mClip = mClip & mClient;
        mClip -= SHEET_BORDER;

        if (mpVScroll)
        {
            if (mClip.wRight >= mpVScroll->mReal.wLeft)
            {
                mClip.wRight = mpVScroll->mReal.wLeft - 3;
            }
        }
    }
    miClipCount++;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::EndClipToFace(void)
{
    miClipCount--;
    if (!miClipCount)
    {
        mClip = mOldClip;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetCellFont(PegFont *pFont)
{
    for (WORD wLoop = 0; wLoop < miCols; wLoop++)
    {
        mpColFonts[wLoop] = pFont;
    }
    SIGNED iHeight = TextHeight(lsTEST, pFont);
    miCellHeight = iHeight + iHeight / 2;

    if (StatusIs(PSF_VISIBLE))
    {
        UpdateScrollBars();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetColumnFont(SIGNED iCol, PegFont *pFont)
{
    if (iCol < 0 || iCol >= miCols)
    {
        return;
    }
    mpColFonts[iCol] = pFont;
    SIGNED iHeight = 0;

    for (WORD wLoop = 0; wLoop < miCols; wLoop++)
    {
        SIGNED iTest = TextHeight(lsTEST, mpColFonts[wLoop]);
        if (iTest > iHeight)
        {
            iHeight = iTest;
        }
    }
    miCellHeight = iHeight + iHeight / 2;

    if (StatusIs(PSF_VISIBLE))
    {
        UpdateScrollBars();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetHeaderFont(PegFont *pFont)
{
    mpHeaderFont = pFont;
    miHeaderFontHeight = TextHeight(lsTEST, mpHeaderFont);

    if (StatusIs(PSF_VISIBLE))
    {
        UpdateScrollBars();
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SelectCell(SIGNED iRow, SIGNED iCol, BOOL bSel)
{
    if (iRow < 0 || iRow >= miRows ||
        iCol < 0 || iCol >= miCols)
    {
        return;
    }

    PegSpreadSheetRow *pSel = mpRowData[iRow];

    UnselectAll();

    if (bSel)
    {
        pSel->SetCellSelected(iCol, TRUE);
        RedrawOneCell(iRow, iCol);
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SelectCell(const PegPoint &pos)
{
    //check to see if a row header was selected

    if (CheckRowSelect(pos))
    {
        return;
    }

    if (CheckColumnSelect(pos))
    {
        return;
    }

    if (Style() & SS_CELL_SELECT)
    {
        CheckCellSelect(pos);
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::TopBorder()
{
    if (miHeaderLines)
    {
        return ((miHeaderLines * miHeaderFontHeight) +
            SHEET_HEADER_SPACE + SHEET_BORDER);
    }
    else
    {
        return SHEET_BORDER;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::LeftBorder()
{
    return SHEET_BORDER + miMaxRowHeaderWidth;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::CheckCellSelect(const PegPoint &pos)
{
    // check to see if an individual cell was selected:

    SIGNED iTopBorder = mClient.wTop + TopBorder(); 
    
    SIGNED iLeftBorder =  mClient.wLeft + LeftBorder();
    SIGNED iRightBorder = iLeftBorder + miNonScrollWidth;
        
    SIGNED iTotalHeight = (iTopBorder + miCellHeight * miVisibleRows);
        
    BOOL bInCell = FALSE;
    SIGNED iCol = 0;

    // look for click in non-scrollable region

    if (pos.y >= iTopBorder && pos.y <= iTotalHeight && 
        pos.x >= iLeftBorder && pos.x <= iRightBorder)
    {
        SIGNED left = iLeftBorder;

        for (iCol = 0; iCol < miStartScroll; iCol++)
        {
            if (pos.x >= left && pos.x < left + mpColWidths[iCol])
            {
                bInCell = TRUE;
                break;
            }
            else
            {
                left += mpColWidths[iCol];
            }
        }
    }
        
    if (!bInCell)
    {
        // look for click in scrollable region

        iLeftBorder =  mClient.wLeft + SHEET_BORDER + miMaxRowHeaderWidth + miNonScrollWidth;
        iRightBorder = mClient.wLeft + SHEET_BORDER + miMaxRowHeaderWidth + miDispColsWidth; 
        
        if (pos.y >= iTopBorder && pos.y <= iTotalHeight && 
            pos.x >= iLeftBorder && pos.x <= iRightBorder)
        {
            SIGNED left= iLeftBorder;

            for (iCol = miStartCol; iCol <= miEndCol; iCol++)
            {
                if (pos.x >= left && pos.x < left + mpColWidths[iCol])
                {
                    bInCell = TRUE;
                    break;
                }
                else
                {
                    left += mpColWidths[iCol];
                }
            }
        }
    }

    if (bInCell && (mpColFlags[iCol] & SCF_CELL_SELECT))
    {
        SIGNED iTopBorder = mClient.wTop + TopBorder();
  
        SIGNED iRow = pos.y - iTopBorder;
        iRow /= miCellHeight;
                 
        // In one of the Rows
        
        iRow += miStartRow;
        if (iRow > miEndRow)
        {
            iRow = miEndRow;
        }

        PegSpreadSheetRow *pSel = mpRowData[iRow];

        BOOL bSelected = pSel->GetCellSelected(iCol);
        UnselectAll();

        if (!bSelected)
        {
            pSel->SetCellSelected(iCol, TRUE);
            RedrawOneCell(iRow, iCol);
        }
        
        // send a message to the parent window to let it know that user selected a cell

        if (Id())
        {
            PegMessage NewMessage;
            NewMessage.wType = PEG_SIGNAL(Id(), PSF_CELL_SELECT);
            NewMessage.Point.x = iCol;
            NewMessage.Point.y = iRow;

            if (mpOwner)
            {
                mpOwner->Message(NewMessage);
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::UnselectAll(void)
{
    if (!UnselectRows())
    {
        if (!UnselectColumns())
        {
            UnselectCells();
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegSpreadSheet::UnselectRows(void)
{
    // look for any selected rows and un-select them:
    BOOL bDone = FALSE;
    SIGNED iRow = GetSelectedRow();
    while(iRow >= 0)
    {
        bDone = TRUE;
        SelectRow(iRow, FALSE);
        iRow = GetSelectedRow();
    }
    return bDone;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegSpreadSheet::UnselectColumns(void)
{
    BOOL bDone = FALSE;
    // look for any selected columns and unselect them:

    SIGNED iCol = GetSelectedColumn();
    while(iCol >= 0)
    {
        SelectColumn(iCol, FALSE);
        bDone = TRUE;
        iCol = GetSelectedColumn();
    }
    return bDone;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegSpreadSheet::UnselectCells(void)
{
    SIGNED iRow, iCol;
    // do a long search to see if any cells are selected:

    for (iRow = 0; iRow < miRows; iRow++)
    {
        PegSpreadSheetRow *pSel = mpRowData[iRow];

        for (iCol = 0; iCol < miCols; iCol++)
        {
            if(pSel->GetCellSelected(iCol))
            {
                pSel->SetCellSelected(iCol, FALSE);
                RedrawOneCell(iRow, iCol);
                return TRUE;
            }
        }
    }
    return FALSE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegSpreadSheet::CheckColumnSelect(const PegPoint &pos)
{
    // check to see if a column header was selected:

    SIGNED iLeftBorder =  mClient.wLeft + LeftBorder();
    SIGNED iRightBorder = iLeftBorder + miDispColsWidth; 

    if (pos.x < iLeftBorder || pos.x > iRightBorder)
    {
        return FALSE;
    }
    SIGNED iTopBorder = mClient.wTop + SHEET_BORDER;
    SIGNED iBottomBorder = iTopBorder + miHeaderLines * miHeaderFontHeight;

    if (pos.y < iTopBorder || pos.y > iBottomBorder)
    {
        return FALSE;
    }

    // they clicked in the column header area, see if it is a valid
    // column selection:

    SIGNED left = iLeftBorder;
    SIGNED iCol;

    for (iCol = 0; iCol < miStartScroll; iCol++)
    {
        if (pos.x >= left && pos.x < left + mpColWidths[iCol])
        {
            if (!(mpColFlags[iCol] & SCF_SEPARATOR))
            {
                FoundColumnSelect(iCol);
            }
            return TRUE;
        }
        left += mpColWidths[iCol];
    }
        

    for (iCol = miStartCol; iCol <= miEndCol; iCol++)
    {
        if (pos.x >= left && pos.x < left + mpColWidths[iCol])
        {
            if (!(mpColFlags[iCol] & SCF_SEPARATOR))
            {
                FoundColumnSelect(iCol);
            }
            break;
        }
        left += mpColWidths[iCol];
    }
    return TRUE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::FoundColumnSelect(SIGNED iCol)
{
    if (mpColFlags[iCol] & SCF_ALLOW_SELECT)
    {
        if (mpColFlags[iCol] & SCF_SELECTED)
        {
            SelectColumn(iCol, FALSE);
        }
        else
        {
            SelectColumn(iCol, TRUE);
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegSpreadSheet::CheckRowSelect(const PegPoint &pos)
{
    SIGNED iLeftBorder  = mClient.wLeft + SHEET_BORDER;
    SIGNED iRightBorder = iLeftBorder + miMaxRowHeaderWidth;

    if (pos.x >= iLeftBorder && pos.x < iRightBorder)
    {
        SIGNED iTopBorder = mClient.wTop + TopBorder();
    
        SIGNED iRow = pos.y - iTopBorder;
        iRow /= miCellHeight;

        if (iRow < 0)
        {
            return FALSE;
        }                 

        iRow += miStartRow;

        if (iRow > miEndRow)
        {
            iRow = miEndRow;
        }

        PegSpreadSheetRow *pSel = mpRowData[iRow];

        if (GetRowStyle(iRow) & SRF_ALLOW_SELECT)
        {
            if (pSel->RowSelected())
            {        
                SelectRow(iRow, FALSE);        
            }
            else
            {
                SelectRow(iRow);
            }
        }
        return TRUE;
    }
    return FALSE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SelectColumn(const SIGNED iCol, BOOL bset)
{
    PegSpreadSheetRow *pSel;
    SIGNED iRow;

    if (bset)
    {
        if (!(Style() & SS_MULTI_COL_SELECT))
        {
            UnselectAll();
        }
        else
        {
            // Unselect any selected rows:
            if (!UnselectRows())
            {
                if (GetSelectedColumn() < 0)
                {
                    UnselectCells();
                }
            }
        }

        mpColFlags[iCol] |= SCF_SELECTED;
    }
    else
    {
        if (!(mpColFlags[iCol] & SCF_SELECTED))
        {
            return;
        }
        mpColFlags[iCol] &= ~SCF_SELECTED;
    }

    for (iRow = 0; iRow < miRows; iRow++)
    {
        pSel = *(mpRowData + iRow);
        pSel->SetCellSelected(iCol, bset);
    }

    BOOL bContinue = TRUE;

    ClipToFace();
    for (iRow = miStartRow; iRow <= miEndRow && bContinue; iRow++)
    {
        bContinue = RedrawOneCell(iRow, iCol);
    }
    EndClipToFace();

    if (Id() && mpOwner)
    {
        PegMessage NewMessage;

        if (bset)
        {
            NewMessage.wType = PEG_SIGNAL(Id(), PSF_COL_SELECT);
        }
        else
        {
            NewMessage.wType = PEG_SIGNAL(Id(), PSF_COL_DESELECT);
        }
        NewMessage.Point.x = iCol;
        NewMessage.Point.y = 0;
        mpOwner->Message(NewMessage);
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegSpreadSheet::ColSelected(const SIGNED iCol)
{
    if (mpColFlags[iCol] & SCF_SELECTED)
    {
        return TRUE;
    }
    return FALSE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SelectRow(const SIGNED iRow, BOOL bset)
{
    PegSpreadSheetRow *pSel = mpRowData[iRow];

    if (bset)
    {
        // if the multiple-row select flag is clear, unselect any other 
        // selected rows:

        if (!(Style() & SS_MULTI_ROW_SELECT))
        {
            UnselectAll();
        }
        else
        {
            if (!UnselectColumns())
            {
                if (GetSelectedRow() < 0)
                {
                    UnselectCells();
                }
            }
        }
    }

    pSel->SetRowSelected(bset);
    SIGNED iCol;

    // Redraw this row of cells:

    BOOL bContinue = TRUE;
    ClipToFace();

    if (miStartScroll)
    {
        for (iCol = 0; iCol < miStartScroll && bContinue; iCol++)
        {
            bContinue = RedrawOneCell(iRow, iCol);
        }
    }

    for (iCol = miStartCol; iCol <= miEndCol && bContinue; iCol++)
    {
        bContinue = RedrawOneCell(iRow, iCol);
    }
    EndClipToFace();

    if (Id() && mpOwner)
    {
        PegMessage NewMessage;

        if (bset)
        {
            NewMessage.wType = PEG_SIGNAL(Id(), PSF_ROW_SELECT);
        }
        else
        {
            NewMessage.wType = PEG_SIGNAL(Id(), PSF_ROW_DESELECT);
        }
        NewMessage.Point.x = 0;
        NewMessage.Point.y = iRow;
        mpOwner->Message(NewMessage);
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegSpreadSheet::RowSelected(const SIGNED iRow)
{
    PegSpreadSheetRow *pSel = mpRowData[iRow];
    return (pSel->RowSelected());
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::Resize(PegRect NewSize)
{
    PegThing *Child;
    PegMessage NewMessage;
    PegThing::Resize(NewSize);

    miStartCol = miStartScroll;
    UpdateScrollBars();
    
    Child = First();

    while(Child)
    {
        NewMessage.wType = PM_PARENTSIZED;
        Child->Message(NewMessage);
        Child = Child->Next();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetScrollStartCol(const SIGNED iCol)
{
    if (iCol <= miCols)
    {
        miStartScroll = iCol;
        miStartCol = miStartScroll;
    }

    if (StatusIs(PSF_VISIBLE))
    {
        UpdateScrollBars();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::UpdateScrollBars(void)
{
    SIGNED iTop = UpdateRowLayout();
    SIGNED iLeft = UpdateColLayout();
    PegRect ScrollRect;
    PegScrollInfo si;

    if (miVisibleRows == miRows) // don't need the scroll bar anymore
    {
        if (mpVScroll)
        {
            Remove(mpVScroll, FALSE);
            delete mpVScroll;
            mpVScroll= NULL;
        }
    }
    else  // we need the vertical scroll
    {
        ScrollRect.wTop = mClient.wTop + iTop;
        ScrollRect.wLeft = mClient.wLeft + SHEET_BORDER + 
            miDispColsWidth + miMaxRowHeaderWidth + 1;
        ScrollRect.wRight = ScrollRect.wLeft + PEG_SCROLL_WIDTH;
        ScrollRect.wBottom = ScrollRect.wTop + (miVisibleRows * miCellHeight);

        si.wMin = 0;
        si.wMax = miRows - 1;
        si.wCurrent = miStartRow;
        si.wStep = 1;
        si.wVisible = miVisibleRows;

        if (!mpVScroll) 
        {
            mpVScroll = new PegVScroll(ScrollRect, &si, IDB_VSCROLL);
            Add(mpVScroll, FALSE);
        }
        else
        {
            mpVScroll->Resize(ScrollRect);
            mpVScroll->Reset(&si);
        }
    }


    if (!mbNeedHScroll) // don't need the scroll bar anymore
    {
        if (mpHScroll)  // currently has a horizontal scroll bar
        {
            Remove(mpHScroll, FALSE);
            delete mpHScroll;
            mpHScroll = NULL;
        }
    }
    else  
    {
        // set up and add a horizontal scroll bar
            
        ScrollRect.wTop= mClient.wTop + TopBorder() +
            miVisibleRows * miCellHeight +
            miFooterLines * miHeaderFontHeight + 1;

        ScrollRect.wLeft = mClient.wLeft + iLeft;
        ScrollRect.wRight = mClient.wRight - SHEET_BORDER;

        if (mpVScroll)
        {
            ScrollRect.wRight -= PEG_SCROLL_WIDTH;
        }
            
        ScrollRect.wBottom = ScrollRect.wTop + PEG_SCROLL_WIDTH;
                         
        si.wMin = miStartScroll;
        si.wMax = miCols;
        si.wCurrent = miStartCol;
        si.wStep = 1;
        si.wVisible = miVisibleCols - miStartScroll;

        if (mbLastColClipped)
        {
            si.wVisible -= 1;
        }

        if (!mpHScroll) // don't need one either!
        {
            mpHScroll = new PegHScroll(ScrollRect, &si, IDB_HSCROLL);
            Add(mpHScroll, FALSE);
        }
        else
        {
            mpHScroll->Resize(ScrollRect);
            mpHScroll->Reset(&si);
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::UpdateColLayout()
{
    // find left edge of first scrollable data column:

    miNonScrollWidth= 0;
    for (SIGNED k = 0; k < miStartScroll; ++k)
    {
        miNonScrollWidth += mpColWidths[k];
    }

    SIGNED iLeft= SHEET_BORDER + miMaxRowHeaderWidth + miNonScrollWidth;

    miVisibleCols= 0;
    miDispColsWidth= miNonScrollWidth;
    SIGNED c= miStartCol;
    SIGNED iRight= iLeft;
    SIGNED iRightLimit = mClient.Width() - SHEET_BORDER;

    if (miVisibleRows != miRows)
    {
        iRightLimit -= PEG_SCROLL_WIDTH;
    }
    
    // see if starting from the startcol,
    // how many columns can be displayed while not going over the limit

    mbLastColClipped = TRUE;
    
    while (iRight <= iRightLimit)
    {
        if (c >= miCols)
        {
            mbLastColClipped = FALSE;
            break;
        }

        iRight += mpColWidths[c];
        miDispColsWidth += mpColWidths[c];
        ++c;
    }
    
    miVisibleCols = (c - miStartCol) + miStartScroll;
    
    if (!((c >= miCols) && (iRight <= iRightLimit)))
    {
        miDispColsWidth -= iRight - iRightLimit;
    }
    miEndCol= ((miStartCol + miVisibleCols) - 1) - miStartScroll;

    if (!(Style() & SS_PARTIAL_COL))
    {
        if (mbLastColClipped)
        {
            miEndCol--;
        }
    }

    return iLeft;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::UpdateRowLayout(BOOL bForceVertical)
{
    // figure out if I will need a horizontal scroll bar:

    mbNeedHScroll = FALSE;
    SIGNED iRightEdge = mClient.wLeft + SHEET_BORDER + miMaxRowHeaderWidth;

    if (mpVScroll || bForceVertical)
    {
        iRightEdge += PEG_SCROLL_WIDTH;
    }

    for (SIGNED iLoop = 0; iLoop < miCols && iRightEdge < mClient.wRight; iLoop++)
    {
        iRightEdge += mpColWidths[iLoop];
    }
    
    if (iRightEdge > mClient.wRight - SHEET_BORDER)
    {
        mbNeedHScroll = TRUE;
    }

    // figure out how many rows I can display at one time:

    SIGNED iTop = TopBorder();

    miVisibleRows = mClient.Height() - iTop - SHEET_BORDER;
    miVisibleRows -= miFooterLines * miHeaderFontHeight;

    if (mbNeedHScroll)
    {
        miVisibleRows -= PEG_SCROLL_WIDTH;
    }
    
    miVisibleRows /= miCellHeight;

    if (miVisibleRows >= miRows) // see it all!
    {
        miVisibleRows = miRows;
        miStartRow= 0;
        miEndRow= miRows-1;
    }
    else
    {
        // try to just extend the endrow, leaving the start the same

        miEndRow = (miStartRow + miVisibleRows) - 1;
    }
        
    if (miEndRow > miRows - 1)
    {
        // set back the start row and end row so bounds fall
        // within 0..miRows-1

        miStartRow -= ((miEndRow-miRows)+1);
        miEndRow   -= ((miEndRow-miRows)+1);
        miVisibleRows= (miEndRow-miStartRow)+1;
    }

    if (miVisibleRows != miRows && !mpVScroll && !bForceVertical)
    {
        // here if we haven't figured out that we need a vertical 
        // scroll bar yet. Call myself again, but force the fact that
        // a vertical scroll is needed:

        return(UpdateRowLayout(TRUE));
    }

    return(iTop);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::VScrollIntoView(const SIGNED iRow)
{
    if (!mpVScroll)
    {
        return;
    }

    SIGNED iTop = iRow - miVisibleRows / 2;
    SIGNED iEnd = iTop + miVisibleRows - 1;

    if (iTop < 0)
    {
        iTop = 0;
    }
    else
    {
        if (iEnd >= miRows)
        {
            iTop -= iEnd - miRows + 1;
        }
    }

    PegScrollInfo *si;
    si = mpVScroll->GetScrollInfo();
    si->wCurrent = iTop;
    mpVScroll->Reset(si);
    DoVerticalScrolling(iTop);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::DoVerticalScrolling(const SIGNED iCurrent)
{
    SIGNED Delta = iCurrent - miStartRow;
    miStartRow = iCurrent;
    miEndRow += Delta;

    if (miEndRow >= miRows)
    {
        miEndRow = miRows - 1;
    }

    PegRect ToDraw = mClient;
    ToDraw.wBottom -= SHEET_BORDER;
    ToDraw.wTop += TopBorder();

    if (mpHScroll)
    {
        ToDraw.wBottom = mpHScroll->mReal.wTop - 1;
    }

    ToDraw.wRight = mpVScroll->mReal.wLeft - 1;
    Invalidate(ToDraw);
    Draw();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::DoHorizontalScrolling(const SIGNED iCurrent)
{
    if (miStartCol == iCurrent)
    {
        return;
    }
    miStartCol = iCurrent;
    UpdateColLayout();

    PegScrollInfo si;

    si.wMin = miStartScroll;
    si.wMax = miCols;
    si.wCurrent = miStartCol;
    si.wStep = 1;
    si.wVisible = miVisibleCols - miStartScroll;

    if (mbLastColClipped)
    {
        si.wVisible -= 1;
    }

    mpHScroll->Reset(&si);

    PegRect ToDraw = mClient;
    ToDraw.wRight -= SHEET_BORDER;

    if (mpVScroll)
    {
        ToDraw.wRight = mpVScroll->mReal.wLeft - 1;
    }

    ToDraw.wLeft += SHEET_BORDER + miMaxRowHeaderWidth + miNonScrollWidth;
    ToDraw.wTop += SHEET_BORDER;
    ToDraw.wBottom = mpHScroll->mReal.wTop - 1;

    Invalidate(ToDraw);
    Draw();
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
COLORVAL PegSpreadSheet::GetCellBackColor(const SIGNED iRow, const SIGNED iCol) const
{
    PegSpreadSheetRow *pSel = mpRowData[iRow];

    if (pSel->GetCellSelected(iCol) &&
        !(mpColFlags[iCol] & SCF_SEPARATOR))
    {
        return muColors[PCI_SELECTED];
    }
    return muColors[PCI_NORMAL];
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
COLORVAL PegSpreadSheet::GetCellTextColor(const SIGNED iRow, const SIGNED iCol) const
{
    PegSpreadSheetRow *pSel = mpRowData[iRow];

    if (pSel->GetCellSelected(iCol))
    {
        return muColors[PCI_STEXT];
    }
    return muColors[PCI_NTEXT];
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::DrawCellBorder(PegRect &CellRect, COLORVAL CellBack,
    SIGNED iRow, SIGNED iCol)
{
    if (mpColFlags[iCol] & FF_RECESSED)
    {
        DrawRecessed(CellRect, CellBack, iRow, iCol);
    }
    else
    {
        if (mpColFlags[iCol] & FF_RAISED)
        {
            DrawRaised(CellRect, CellBack, iRow, iCol);
        }
        else
        {
            DrawFlat(CellRect, CellBack);
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegRect PegSpreadSheet::GetCellRect(const SIGNED iRow, const SIGNED iCol)
{
    PegRect CellRect;

    CellRect.wTop = mClient.wTop + TopBorder() +
        (iRow - miStartRow) * miCellHeight;
    
    SIGNED iStart;
    SIGNED iCurCol;

    if (miStartScroll != 0 && iCol >= 0 && iCol < miStartScroll)
    {
        iStart= 0;
        CellRect.wLeft = mClient.wLeft + SHEET_BORDER + miMaxRowHeaderWidth;
    }
    else // in scrollable region
    {
        iStart = miStartCol;
        CellRect.wLeft= mClient.wLeft + SHEET_BORDER + miMaxRowHeaderWidth + miNonScrollWidth;
    }
            
    for (iCurCol = iStart; iCurCol < iCol; iCurCol++)
    {
        CellRect.wLeft += mpColWidths[iCurCol];
    }

    CellRect.wRight = CellRect.wLeft + mpColWidths[iCurCol] - 2;
    CellRect.wBottom= CellRect.wTop + miCellHeight - 2;
    return CellRect;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegSpreadSheet::RedrawOneCell(const SIGNED iRow, const SIGNED iCol)
{
    // find coordinates of the cell, and update it

    if (iRow < miStartRow || iRow > miEndRow || iCol < 0 ||
        (iCol >= miStartScroll &&  (iCol < miStartCol || iCol > miEndCol)))
    {
        return FALSE;
    }

    COLORVAL cellBack;
    COLORVAL cellText;
    PegRect CellRect = GetCellRect(iRow, iCol);
    
    ClipToFace();

    if (CellRect.wLeft > mClip.wRight || CellRect.wTop > mClip.wBottom)
    {
        EndClipToFace();
        return FALSE;
    }

    if (CellRect.wRight < mClip.wLeft || CellRect.wBottom < mClip.wTop)
    {
        EndClipToFace();
        return TRUE;
    }
    
    cellBack = GetCellBackColor(iRow, iCol);
    cellText = GetCellTextColor(iRow, iCol);
    
    if (((miStartScroll != 0 && iCol <= miStartScroll) ||
        (iCol >= miStartCol && iCol <= miEndCol)) &&
        iRow >= miStartRow && iRow <= miEndRow)
    {
        Invalidate(CellRect);
        BeginDraw();
        DrawCellBorder(CellRect, cellBack, iRow, iCol);
        UpdateCell(CellRect, mpRowData[iRow]->DataGet(iCol),
            mpColFlags[iCol], mpColFonts[iCol], cellText);
        EndDraw();
    }
    EndClipToFace();
    return TRUE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::DrawAllCells(void)
{
    PegRect CellRect;

    COLORVAL cellBack;
    COLORVAL cellText;
    SIGNED iCol, iRow;
    SIGNED iTop, iBottom;

    ClipToFace();

    // draw out cells from non-scrollable columns, if there are any

    if (miStartScroll)
    {
        CellRect = GetCellRect(miStartRow, 0);
        iTop = CellRect.wTop;
        iBottom = CellRect.wBottom;

        for (iCol = 0; iCol < miStartScroll && CellRect.wLeft < mClip.wRight;
             iCol++)
        {
            if (CellRect.wRight > mClip.wLeft)
            {
                for (iRow = miStartRow;
                     iRow <= miEndRow && CellRect.wTop < mClip.wBottom;
                     iRow++)
                {
                    if (CellRect.wBottom > mClip.wTop)
                    {
                        cellBack = GetCellBackColor(iRow, iCol);
                        cellText = GetCellTextColor(iRow, iCol);
                        DrawCellBorder(CellRect, cellBack, iRow, iCol);
                        UpdateCell(CellRect, mpRowData[iRow]->DataGet(iCol),
                            mpColFlags[iCol], mpColFonts[iCol], cellText);
                    }

                    CellRect.wTop += miCellHeight;
                    CellRect.wBottom = CellRect.wTop + miCellHeight - 2;
                }
            }
            if (iCol < miStartScroll - 1)
            {
                CellRect.wTop = iTop;
                CellRect.wBottom = iBottom;
                CellRect.wLeft += mpColWidths[iCol];
                CellRect.wRight = CellRect.wLeft + mpColWidths[iCol + 1] - 2;
            }
        }
    }

    // now draw out cells from scrollable region

    CellRect = GetCellRect(miStartRow, miStartCol);
    iTop = CellRect.wTop;
    iBottom = CellRect.wBottom;

    for (iCol = miStartCol; iCol <= miEndCol && CellRect.wLeft < mClip.wRight;
         iCol++)
    {
        if (CellRect.wRight > mClip.wLeft)
        {
            for (iRow = miStartRow;
                 iRow <= miEndRow && CellRect.wTop < mClip.wBottom; iRow++)
            {
                if (CellRect.wBottom > mClip.wTop)
                {
                    cellBack = GetCellBackColor(iRow, iCol);
                    cellText = GetCellTextColor(iRow, iCol);
                    DrawCellBorder(CellRect, cellBack, iRow, iCol);
                    UpdateCell(CellRect, mpRowData[iRow]->DataGet(iCol),
                        mpColFlags[iCol], mpColFonts[iCol], cellText);
                }

                CellRect.wTop += miCellHeight;
                CellRect.wBottom = CellRect.wTop + miCellHeight - 2;
            }
        }

        if (iCol < miCols - 1)
        {
            CellRect.wTop = iTop;
            CellRect.wBottom = iBottom;
            CellRect.wLeft += mpColWidths[iCol];
            CellRect.wRight = CellRect.wLeft + mpColWidths[iCol + 1] - 2;
        }
    }
    EndClipToFace();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::DrawRowHeaders ()
{
    // output the row headers for visible rows right justified

    PegRect CellRect;

    // ok, draw each cell:
       
    SIGNED iLeftBorder= SHEET_BORDER;
    SIGNED iTopBorder = TopBorder();

    CellRect.wTop = mClient.wTop + iTopBorder - miCellHeight;

    COLORVAL cellBack;
    COLORVAL cellText;
        
    for (SIGNED r = miStartRow; r <= miEndRow; r++)
    {
        PEGCHAR *pData = mpRowData[r]->GetRowHeader();

        if (!pData)
        {
            continue;
        }
        CellRect.wTop += miCellHeight;
        CellRect.wBottom = CellRect.wTop + miCellHeight - 2;
        CellRect.wLeft = mClient.wLeft + iLeftBorder;
        CellRect.wRight =  CellRect.wLeft + miMaxRowHeaderWidth - 2;

        cellBack = muTblColors[PCI_SS_ROWHEADBACK - THING_COLOR_LIST_SIZE];
        cellText = muTblColors[PCI_SS_ROWHEADTEXT - THING_COLOR_LIST_SIZE];
        DrawFlat(CellRect, cellBack);
        UpdateCell(CellRect, pData, TJ_RIGHT, mpHeaderFont, cellText);
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::DrawHeaders ()
{
    PegRect CellRect;
    SIGNED k;

    SIGNED iLeftBorder= SHEET_BORDER + miMaxRowHeaderWidth;
    SIGNED iTopBorder = SHEET_BORDER;

    for (k = 0; k < miHeaderLines; ++k)
    {
        CellRect.wTop = mClient.wTop + iTopBorder;
        CellRect.wBottom = CellRect.wTop + miHeaderFontHeight;
        CellRect.wLeft = mClient.wLeft + iLeftBorder + 1;
            
        for (SIGNED h = 0; h < miStartScroll; ++h)
        {
            CellRect.wRight = CellRect.wLeft + mpColWidths[h] - 2;
            PEGCHAR *val= mpHeaderData[k]->DataGet(h);
            DrawFlat(CellRect, muTblColors[PCI_SS_COLHEADBACK - THING_COLOR_LIST_SIZE]);
            UpdateCell(CellRect, val, TJ_CENTER, mpHeaderFont,
                muTblColors[PCI_SS_COLHEADTEXT - THING_COLOR_LIST_SIZE]);
            CellRect.wLeft += mpColWidths[h];
        }
            
        CellRect.wTop += miHeaderFontHeight;
        CellRect.wBottom += miHeaderFontHeight;
        iTopBorder += miHeaderFontHeight;
    }

    iLeftBorder= SHEET_BORDER + miMaxRowHeaderWidth + miNonScrollWidth;
    iTopBorder = SHEET_BORDER;

    for (k = 0; k < miHeaderLines; ++k)
    {
        CellRect.wTop = mClient.wTop + iTopBorder;
        CellRect.wBottom = CellRect.wTop + miHeaderFontHeight;
        CellRect.wLeft = mClient.wLeft + iLeftBorder + 1;
            
        for (SIGNED h = miStartCol; h <= miEndCol; ++h)
        {
            CellRect.wRight = CellRect.wLeft + mpColWidths[h] - 2;
            PEGCHAR *val= mpHeaderData[k]->DataGet(h);
            DrawFlat(CellRect, muTblColors[PCI_SS_COLHEADBACK - THING_COLOR_LIST_SIZE]);
            UpdateCell(CellRect, val, TJ_CENTER, mpHeaderFont,
                muTblColors[PCI_SS_COLHEADTEXT - THING_COLOR_LIST_SIZE]);
            CellRect.wLeft += mpColWidths[h];
        }
            
        CellRect.wTop += miHeaderFontHeight;
        CellRect.wBottom += miHeaderFontHeight;
        iTopBorder += miHeaderFontHeight;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::DrawFooters ()
{
    PegRect CellRect;
  #ifdef PEG_RUNTIME_COLOR_CHECK
    PegColor Color(LIGHTGRAY, LIGHTGRAY, CF_FILL);
  #else
   #if (PEG_NUM_COLORS >= 4)
    PegColor Color(LIGHTGRAY, LIGHTGRAY, CF_FILL);
   #else
    PegColor Color(WHITE, WHITE, CF_FILL);
   #endif
  #endif

    if (!miFooterLines)
    {
        return;
    }

    SIGNED iTopSpace = TopBorder() + miVisibleRows * miCellHeight;
    CellRect.wTop= mClient.wTop + iTopSpace;
    
    CellRect.wLeft= mClient.wLeft + SHEET_BORDER + miMaxRowHeaderWidth +
        miNonScrollWidth;
    CellRect.wRight= CellRect.wLeft + miDispColsWidth;
    CellRect.wBottom= CellRect.wTop + miFooterLines * miHeaderFontHeight;

    Rectangle(CellRect, Color, 0);

    SIGNED iLeftBorder= SHEET_BORDER + miMaxRowHeaderWidth;
    SIGNED iTopBorder = iTopSpace;

    SIGNED k;

    for (k = 0; k < miFooterLines; ++k)
    {
        CellRect.wTop = mClient.wTop + iTopBorder;
        CellRect.wBottom = CellRect.wTop + miHeaderFontHeight;
        CellRect.wLeft = mClient.wLeft + iLeftBorder;

        for (SIGNED h = 0; h < miStartScroll; h++)
        {
            CellRect.wRight = CellRect.wLeft + mpColWidths[h];
            PEGCHAR *val= mpFooterData[k]->DataGet(h);
            UpdateCell(CellRect, val, TJ_CENTER, mpHeaderFont,
                muTblColors[PCI_SS_COLHEADTEXT - THING_COLOR_LIST_SIZE]);
            CellRect.wLeft += mpColWidths[h];
        }
            
        iTopBorder += miHeaderFontHeight;
    }

    iLeftBorder= SHEET_BORDER + miMaxRowHeaderWidth + miNonScrollWidth;
    iTopBorder = iTopSpace;

    for (k = 0; k < miFooterLines; ++k)
    {
        CellRect.wTop = mClient.wTop + iTopBorder;
        CellRect.wBottom = CellRect.wTop + miHeaderFontHeight;
        CellRect.wLeft = mClient.wLeft + iLeftBorder;

        for (SIGNED h = miStartCol; h <= miEndCol; h++)
        {
            CellRect.wRight = CellRect.wLeft + mpColWidths[h];
            PEGCHAR *val= mpFooterData[k]->DataGet(h);
            UpdateCell(CellRect, val, TJ_CENTER, mpHeaderFont,
                muTblColors[PCI_SS_COLHEADTEXT - THING_COLOR_LIST_SIZE]);
            CellRect.wLeft += mpColWidths[h];
        }
        iTopBorder += miHeaderFontHeight;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::DrawFlat(PegRect &CellRect, COLORVAL BackColor)
{
    PegColor Color(BackColor, BackColor, CF_FILL);
    Rectangle(CellRect, Color, 0);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::DrawRecessed(PegRect &CellRect, COLORVAL BackColor,
    SIGNED, SIGNED column)
{
    PegColor Color;
    PegRect DrawRect = CellRect;

    if (mpColFlags[column] & SCF_SEPARATOR)
    {
        Color.uForeground = muTblColors[PCI_SS_DIVIDER - THING_COLOR_LIST_SIZE];
        Color.uBackground = muTblColors[PCI_SS_DIVIDER - THING_COLOR_LIST_SIZE];
        
        ++DrawRect.wRight;
        --DrawRect.wLeft;
        Rectangle(DrawRect, Color, 0);
        return;
    }
    
    Color.uForeground = PCLR_LOWLIGHT;
    Color.uBackground = BLACK;

    Line(DrawRect.wLeft, DrawRect.wTop, DrawRect.wRight,
        DrawRect.wTop, Color);
    Line(DrawRect.wLeft, DrawRect.wTop, DrawRect.wLeft,
        DrawRect.wBottom, Color);

    Color.uForeground = PCLR_HIGHLIGHT;
        
    Line(DrawRect.wLeft, DrawRect.wBottom, DrawRect.wRight,
        DrawRect.wBottom, Color);
    Line(DrawRect.wRight, DrawRect.wTop, DrawRect.wRight,
        DrawRect.wBottom, Color);
    
    Color.uForeground = BackColor;
    Color.uBackground = BackColor;
    Color.uFlags = CF_FILL;
    DrawRect--;
    Rectangle(DrawRect, Color, 0);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::DrawRaised(PegRect &CellRect, COLORVAL BackColor,
    SIGNED, SIGNED column)
{
    PegColor Color;
    PegRect DrawRect = CellRect;

    if (mpColFlags[column] & SCF_SEPARATOR)
    {
        Color.uForeground = muTblColors[PCI_SS_DIVIDER - THING_COLOR_LIST_SIZE];
        Color.uBackground = muTblColors[PCI_SS_DIVIDER - THING_COLOR_LIST_SIZE];
        
        ++DrawRect.wRight;
        --DrawRect.wLeft;
        Rectangle(DrawRect, Color, 0);
        return;
    }
    
    Color.uForeground = PCLR_HIGHLIGHT;
    Color.uBackground = BLACK;

    Line(DrawRect.wLeft, DrawRect.wTop, DrawRect.wRight,
        DrawRect.wTop, Color);
    Line(DrawRect.wLeft, DrawRect.wTop, DrawRect.wLeft,
        DrawRect.wBottom, Color);

    Color.uForeground = PCLR_LOWLIGHT;
        
    Line(DrawRect.wLeft, DrawRect.wBottom, DrawRect.wRight,
        DrawRect.wBottom, Color);
    Line(DrawRect.wRight, DrawRect.wTop, DrawRect.wRight,
        DrawRect.wBottom, Color);
    
    Color.uForeground = BackColor;
    Color.uBackground = BackColor;
    Color.uFlags = CF_FILL;
    DrawRect--;
    Rectangle(DrawRect, Color, 0);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::UpdateCell(PegRect &CellRect, const PEGCHAR *Text,
    WORD wJustify, PegFont *pFont, COLORVAL InColor)
{
    if (!Text)
    {
        return;
    }
    PegColor Color(InColor, BLACK, CF_NONE);
    PegPoint PutText;
    PutText.y = CellRect.wTop + ((CellRect.Height() - pFont->uHeight) / 2);

    PegRect OldClip = mClip;
    mClip &= CellRect;

    if (wJustify & TJ_RIGHT)
    {
        SIGNED iWidth = TextWidth(Text, pFont);
        PutText.x = CellRect.wRight - 2 - iWidth;
        DrawText(PutText, Text, Color, pFont);
    }
    else
    {
        if (wJustify & TJ_CENTER)
        {
            SIGNED iWidth = (CellRect.Width() - TextWidth(Text, pFont)) / 2;
            PutText.x = CellRect.wLeft + iWidth;
            DrawText(PutText, Text, Color, pFont);
        }
        else
        {
            // default, wLeft justify
            PutText.x = CellRect.wLeft + 2;
            DrawText(PutText, Text, Color, pFont);
        }
    }
    mClip = OldClip;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::AddColumn(SIGNED iWidth, PEGCHAR *pHeader)
{
    InsertColumn(miCols, iWidth, pHeader);
    return miCols;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::InsertColumn(SIGNED iColumn, SIGNED iWidth, PEGCHAR *pHeader)
{
    if (iColumn > miCols)
    {
        return -1;
    }

    // save the old column flags:
    
    SIGNED iLoop;
    WORD *pOldFlags = mpColFlags;
    mpColFlags = new WORD[miCols + 1];
    SIGNED *pOldWidths = mpColWidths;
    mpColWidths = new SIGNED[miCols + 1];
    PegFont **pOldFonts = mpColFonts;
    mpColFonts = new PegFont *[miCols + 1];

    for (iLoop = 0; iLoop < iColumn; iLoop++)
    {
        mpColFlags[iLoop] = pOldFlags[iLoop]; 
        mpColWidths[iLoop] = pOldWidths[iLoop];
        mpColFonts[iLoop] = pOldFonts[iLoop];
    }
    for (iLoop = miCols - 1; iLoop >= iColumn; iLoop--)
    {
        mpColFlags[iLoop + 1] = pOldFlags[iLoop]; 
        mpColWidths[iLoop + 1] = pOldWidths[iLoop];
        mpColFonts[iLoop + 1] = pOldFonts[iLoop];
    }

    if (Style() & SS_CELL_SELECT)
    {
        mpColFlags[iColumn] = SCF_CELL_SELECT;
    }
    else
    {
        mpColFlags[iColumn] = 0;
    }
    mpColWidths[iColumn] = iWidth;
    mpColFonts[iColumn] = PegTextThing::GetDefaultFont(PEG_CELL_FONT);
    delete pOldFlags;
    delete pOldWidths;
    delete pOldFonts;

    // tell the row data to insert this column:

    for (iLoop = 0; iLoop < miRows; iLoop++)
    {
        mpRowData[iLoop]->InsertColumn(iColumn);
    }
    for (iLoop = 0; iLoop < miHeaderLines; iLoop++)
    {
        mpHeaderData[iLoop]->InsertColumn(iColumn, pHeader);
    }
    for (iLoop = 0; iLoop < miFooterLines; iLoop++)
    {
        mpFooterData[iLoop]->InsertColumn(iColumn);
    }
    miCols++;
    UpdateScrollBars();
    return miCols;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::DeleteColumn(SIGNED iColumn)
{
    if (miCols <= 1 || !mpColFlags || iColumn >= miCols)
    {
        return -1;
    }
    // save the old column flags:
    
    SIGNED iLoop;
    WORD *pOldFlags = mpColFlags;
    mpColFlags = new WORD[miCols - 1];
    SIGNED *pOldWidths = mpColWidths;
    mpColWidths = new SIGNED[miCols - 1];
    PegFont **pOldFonts = mpColFonts;
    mpColFonts = new PegFont *[miCols - 1];

    for (iLoop = 0; iLoop < iColumn; iLoop++)
    {
        mpColFlags[iLoop] = pOldFlags[iLoop]; 
        mpColWidths[iLoop] = pOldWidths[iLoop];
        mpColFonts[iLoop] = pOldFonts[iLoop];
    }
    for (iLoop = iColumn; iLoop < miCols - 1; iLoop++)
    {
        mpColFlags[iLoop] = pOldFlags[iLoop + 1]; 
        mpColWidths[iLoop] = pOldWidths[iLoop + 1];
        mpColFonts[iLoop] = pOldFonts[iLoop + 1];
    }
    delete pOldFlags;
    delete pOldWidths;
    delete pOldFonts;

    // tell the row data to delete this column:

    for (iLoop = 0; iLoop < miRows; iLoop++)
    {
        mpRowData[iLoop]->DeleteColumn(iColumn);
    }
    for (iLoop = 0; iLoop < miHeaderLines; iLoop++)
    {
        mpHeaderData[iLoop]->DeleteColumn(iColumn);
    }
    for (iLoop = 0; iLoop < miFooterLines; iLoop++)
    {
        mpFooterData[iLoop]->DeleteColumn(iColumn);
    }
    miCols--;
    UpdateScrollBars();
    return miCols;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::AddRow(PEGCHAR *pHeader)
{
    InsertRow(miRows, pHeader);
    return miRows;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::InsertRow(SIGNED iRow, PEGCHAR *pHeader)
{
    SIGNED wLoop;

    if (!miRows && !iRow)
    {
        mpRowData = new PegSpreadSheetRow *[1];
        mpRowData[0] = new PegSpreadSheetRow(miCols);
        miRows++;

        if (pHeader)
        {
            SetRowHeader(iRow, pHeader);
        }
        UpdateScrollBars();
        return miRows;
    }

    if (!miRows)
    {
        return miRows;
    }

    PegSpreadSheetRow **pOldRowData = mpRowData;
    mpRowData = new PegSpreadSheetRow *[miRows + 1];
    
    for (wLoop = 0; wLoop < iRow; wLoop++)
    {
        mpRowData[wLoop] = pOldRowData[wLoop];
    }
    for (wLoop = miRows - 1; wLoop >= iRow; wLoop--)
    {
        mpRowData[wLoop + 1] = pOldRowData[wLoop];
    }
    delete pOldRowData;
    mpRowData[iRow] = new PegSpreadSheetRow(miCols);
    miRows++;

    if (pHeader)
    {
        SetRowHeader(iRow, pHeader);
    }
    UpdateScrollBars();
    return miRows;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::DeleteRow(SIGNED iRow)
{
    SIGNED iLoop;

    if (!miRows || iRow < 0 || iRow >= miRows)
    {
        return miRows;
    }
    if (miRows == 1)
    {
        if (iRow)   // the only valid row is 0
        {
            return miRows;
        }
        delete mpRowData;
        mpRowData = NULL;
        miRows--;
        UpdateScrollBars();
        return miRows;
    }

    PegSpreadSheetRow **pOldRowData = mpRowData;
    mpRowData = new PegSpreadSheetRow *[miRows - 1];
    
    for (iLoop = 0; iLoop < iRow; iLoop++)
    {
        mpRowData[iLoop] = pOldRowData[iLoop];
    }
    for (iLoop = iRow; iLoop < miRows - 1; iLoop++)
    {
        mpRowData[iLoop] = pOldRowData[iLoop + 1];
    }
    delete pOldRowData;
    miRows--;
    UpdateScrollBars();
    return miRows;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0
void PegSpreadSheet::ResetAllCells(void)
{

}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetSize(const SIGNED Rows, const SIGNED Cols)
{
    if (mpRowData)        // cleanup old data array if there is one
    {
        for (SIGNED i = 0; i < miRows; i++)
        {
            PegSpreadSheetRow *DelThis = mpRowData[i];
            if (DelThis)
            {
                delete DelThis;
            }
        }
        delete [] mpRowData;
    }

    miRows = Rows;    
    miCols = Cols;

    mpRowData = new PegSpreadSheetRow *[miRows];

    for (SIGNED row= 0; row < miRows; row++)
    {
        mpRowData[row]= new PegSpreadSheetRow(miCols);
    }
               
    if (mpColWidths)
    {
        delete [] mpColWidths;
    }
    mpColWidths = new SIGNED[miCols];
    
    if (mpColFlags)
    {
        delete [] mpColFlags;
    }
    
    mpColFlags = new WORD[miCols];

    if (mpColFonts)
    {
        delete [] mpColFonts;
    }

    mpColFonts = new PegFont *[miCols];
    
    for (SIGNED j = 0; j < miCols; j++)
    {
        mpColFonts[j] = PegTextThing::GetDefaultFont(PEG_CELL_FONT);
        mpColWidths[j] = TextWidth(lsSPACE, mpColFonts[j]) +
            SHEET_CELL_PADDING;
        mpColFlags[j] =  TJ_CENTER|FF_RECESSED|SCF_CELL_SELECT;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetColumnStyle(const SIGNED iCol, const WORD wFlags)
{
    if (iCol < miCols)
    {
        mpColFlags[iCol] = wFlags;
    }
    
    if (wFlags & SCF_SEPARATOR)
    {
        mpColWidths[iCol]= SHEET_SEPERATOR_WIDTH;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::GetSelectedColumn(SIGNED iIndex)
{
    for (SIGNED iLoop = 0; iLoop < miCols; iLoop++)
    {
        if (mpColFlags[iLoop] & SCF_SELECTED)
        {
            if (!iIndex)
            {
                return iLoop;
            }
            iIndex--;
        }
    }
    return -1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::GetSelectedRow(SIGNED iIndex)
{
    for (SIGNED iLoop = 0; iLoop < miRows; iLoop++)
    {
        if (mpRowData[iLoop]->RowSelected())
        {
            if (!iIndex)
            {
                return iLoop;
            }
            iIndex--;
        }
    }
    return -1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
WORD PegSpreadSheet::GetRowStyle(const SIGNED iRow)
{
    if (iRow < miRows)
    {
        return (mpRowData[iRow]->RowStyle());
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetRowStyle(const SIGNED iRow, const WORD wFlags)
{
    if (iRow < miRows)
    {
        mpRowData[iRow]->SetRowStyle(wFlags);
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetColWidth(SIGNED col, const PEGCHAR *str, PegFont *pFont)
{
    // update my column widths if necessary:

    SIGNED iWidth = TextWidth(str, pFont) + SHEET_CELL_PADDING;
    CheckColWidths(col, iWidth);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::CheckColWidths(SIGNED col, SIGNED iWidth)
{
    if (iWidth > mpColWidths[col])
    {
        mpColWidths[col] = iWidth;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::ForceColWidth(SIGNED col, SIGNED iWidth)
{
    mpColWidths[col] = iWidth;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetCellData(const SIGNED iRow, const SIGNED iCol, 
    const PEGCHAR *charVal, BOOL bRedraw)
{
    if (iRow < miRows && iCol < miCols && iRow >= 0 && iCol >= 0)
    {
        mpRowData[iRow]->DataSet(iCol, charVal);
        if (StatusIs(PSF_VISIBLE))
        {
            if (bRedraw)
            {
                RedrawOneCell(iRow, iCol);
            }
        }
        else
        {
            if (charVal)
            {
                SIGNED Length = strlen(charVal);

                if (Length)
                {
                    SetColWidth(iCol, charVal, mpColFonts[iCol]);
                }
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::ReconstructHeaderData(const SIGNED line)
{
    if (line > miHeaderLines)  
    {
        PegSpreadSheetRow **temp;
        
        temp = new PegSpreadSheetRow *[line];   
        
        if (!mpHeaderData)
        {
            for (SIGNED i= 0; i < line - 1; i++)
            {
                temp[i]= new PegSpreadSheetRow(miCols);
            }
        }
        else    
        {
            for (SIGNED i= 0; i < line - 1; i++)
            {
                if (mpHeaderData[i])
                {
                    temp[i]= mpHeaderData[i]; 
                }
                else
                {
                    temp[i]= new PegSpreadSheetRow(miCols);
                }
            }
        }
        
        temp[line-1]= new PegSpreadSheetRow(miCols);  
    
        if (mpHeaderData)
        {
            delete [] mpHeaderData;   
        }
        
        mpHeaderData= temp;     
        miHeaderLines= line;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::ReconstructFooterData(const SIGNED line)
{
    if (line > miFooterLines)  
    {
        PegSpreadSheetRow **temp;
        
        temp = new PegSpreadSheetRow *[line];   
        
        if (!mpFooterData)
        {
            for (SIGNED i= 0; i<line-1; ++i)
            {
                temp[i]= new PegSpreadSheetRow(miCols);
            }
        }
        else    
        {
            for (SIGNED i= 0; i<line-1; ++i)
            {
                if (mpFooterData[i])
                {
                    temp[i]= mpFooterData[i];
                }
                else
                {
                    temp[i]= new PegSpreadSheetRow(miCols);
                }
            }
        }
        
        temp[line-1]= new PegSpreadSheetRow(miCols);  
    
        if (mpFooterData)
        {
            delete [] mpFooterData;  
        }
        
        mpFooterData= temp;       
        miFooterLines= line;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetHeader(const SIGNED line, const SIGNED iCol,
    const PEGCHAR *charVal)
{
    if (iCol < 0 || iCol > miCols || line < 1 ||
        line > SHEET_MAX_HEADER_LINES)
    {
        return;
    }
    
    ReconstructHeaderData(line);
    WORD wTemp = mpColWidths[iCol];
    SetColWidth(iCol, charVal, mpHeaderFont);
    mpHeaderData[line-1]->DataSet(iCol, charVal);

    if (StatusIs(PSF_VISIBLE) &&
        mpColWidths[iCol] != wTemp)
    {
        UpdateScrollBars();
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetRowHeader(const SIGNED iRow, const PEGCHAR *header)
{
    if (iRow < 0 || iRow >= miRows) return;

    mpRowData[iRow]->SetRowHeader(header);
    SIGNED iWidth = TextWidth(header, mpHeaderFont) + SHEET_CELL_PADDING;

    if (iWidth > miMaxRowHeaderWidth)
    {
        miMaxRowHeaderWidth= iWidth;

        if (StatusIs(PSF_VISIBLE))
        {
            UpdateScrollBars();
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetFooter(const SIGNED line, const SIGNED iCol, const PEGCHAR *charVal)
{
    if (iCol < 0 || iCol > miCols || line < 1 ||
        line > SHEET_MAX_HEADER_LINES) return;
    
    ReconstructFooterData(line);
    SetColWidth(iCol, charVal, mpHeaderFont);
    mpFooterData[line-1]->DataSet(iCol, charVal);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheet::SetColor(const UCHAR index, const COLORVAL Color)
{
    if (index < THING_COLOR_LIST_SIZE)
    {
        muColors[index] = Color;
    }
    else
    {
        muTblColors[index - THING_COLOR_LIST_SIZE] = Color;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR* PegSpreadSheet::GetCellData(const SIGNED iRow, const SIGNED iCol) const
{
    PEGCHAR *GetData = NULL;

    if (iRow >= 0 && iCol >= 0 && iRow <miRows && iCol < miCols)
    {
        if (mpRowData[iRow])
        {
            GetData = mpRowData[iRow]->DataGet(iCol);
        }
    }

    return (GetData);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR* PegSpreadSheet::GetRowHeader(const SIGNED iRow) const
{
    if (iRow >= 0 && iRow < miRows)
    {
        return mpRowData[iRow]->GetRowHeader();
    }
    return NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR* PegSpreadSheet::GetColHeader(const SIGNED iCol) const
{
    if (iCol >= 0 && iCol < miCols)
    {
        return mpHeaderData[0]->DataGet(iCol);
    }
    return NULL;    
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::GetOptimumWidth(void)
{
    SIGNED iTotalWidth = 0;

    for (SIGNED col= 0; col<miCols; ++ col)
    {
        iTotalWidth += mpColWidths[col];
    }
    
    iTotalWidth += SHEET_BORDER + miMaxRowHeaderWidth + SHEET_BORDER + 4;
    return iTotalWidth;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSpreadSheet::GetOptimumHeight(void)
{
     return  TopBorder() + miRows * miCellHeight +
             miFooterLines * miHeaderFontHeight +
             SHEET_BORDER + 4;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegSpreadSheetRow::PegSpreadSheetRow(SIGNED columns) :
    miColumns(columns), mwRowStyle(0), mpRowHeader(NULL)
{
    mpCellData= new PegSpreadSheetCell *[columns];

    for (WORD wLoop = 0; wLoop < columns; wLoop++)
    {
        mpCellData[wLoop] = new PegSpreadSheetCell();
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegSpreadSheetRow::~PegSpreadSheetRow (void)
{
    if (mpCellData)
    {
        for (WORD wLoop = 0; wLoop < miColumns; wLoop++)
        {
            delete mpCellData[wLoop];
        }
        delete mpCellData;
    }
    if (mpRowHeader)
    {
        delete mpRowHeader;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PEGCHAR* PegSpreadSheetRow::DataGet(SIGNED col)
{
    if (col >= miColumns)
    {
        return NULL;
    }
    
    return mpCellData[col]->mpString;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheetRow::DeleteColumn(SIGNED iCol)
{
    if (iCol >= miColumns)
    {
        return;
    }
    delete mpCellData[iCol];

    for (SIGNED iLoop = iCol; iLoop < miColumns - 1; iLoop++)
    {
        mpCellData[iLoop] = mpCellData[iLoop + 1];
    }
    miColumns--;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheetRow::InsertColumn(SIGNED iCol, PEGCHAR *pData)
{
    SIGNED iLoop;
    if (iCol > miColumns || !miColumns)
    {
        return;
    }

    PegSpreadSheetCell **pOldData = mpCellData;
    mpCellData = new PegSpreadSheetCell *[miColumns + 1];

    for (iLoop = 0; iLoop < iCol; iLoop++)
    {
        mpCellData[iLoop] = pOldData[iLoop];
    }

    for (iLoop = miColumns - 1; iLoop >= iCol; iLoop--)
    {
        mpCellData[iLoop + 1] = pOldData[iLoop];
    }

    mpCellData[iCol] = new PegSpreadSheetCell();

    if (pData)
    {
        mpCellData[iCol]->DataSet(pData);
    }
    miColumns++;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheetRow::DataSet(SIGNED col, const PEGCHAR *data)
{
    if (col >= miColumns)
    {
        return;
    }
    mpCellData[col]->DataSet(data);    
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheetRow::SetRowSelected (BOOL bSelected)
{
    if (bSelected)
    {
        mwRowStyle |= SRF_SELECTED;
    }
    else
    {
        mwRowStyle &= ~SRF_SELECTED;
    }

    for (SIGNED i= 0; i < miColumns; ++i)
    {
        SetCellSelected(i, bSelected);
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheetRow::SetCellSelected (SIGNED col, BOOL bSelected)
{
    mpCellData[col]->bSelected= bSelected;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegSpreadSheetRow::GetCellSelected (SIGNED col)
{
    return mpCellData[col]->bSelected;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheetRow::SetRowHeader (const PEGCHAR *pHeader)
{
    if (mpRowHeader)
    {
        delete mpRowHeader;
        mpRowHeader = NULL;
    }

    if (pHeader)
    {
        mpRowHeader= new PEGCHAR[strlen(pHeader)+1];
        strcpy(mpRowHeader, pHeader);
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegSpreadSheetCell::PegSpreadSheetCell() :
    mpString(NULL), bSelected(FALSE), iWidth(0)
{
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegSpreadSheetCell::~PegSpreadSheetCell()
{
    if (mpString)
    {
        delete [] mpString;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegSpreadSheetCell::DataSet(const PEGCHAR *pData)
{
    if (pData)
    {
        if (iWidth > (SIGNED) strlen(pData))
        {
            strcpy(mpString, pData);
        }
        else
        {
            if (mpString)
            {
                delete [] mpString;
            }
            iWidth = strlen(pData) + 8;
            mpString = new PEGCHAR[iWidth];
            strcpy(mpString, pData);
        }
    }
    else
    {
        if (mpString)
        {
            delete [] mpString;
            mpString = NULL;
        }
        iWidth = 0;
    }
}





// end of file


