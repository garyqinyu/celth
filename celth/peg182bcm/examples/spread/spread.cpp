/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// spread.cpp - PegSpreadSheet demonstration window.
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

#include "peg.hpp"
#include "spread.hpp"
#include "stdio.h"

#if defined(POSIXPEG) || defined(PEGX11)
#include <string.h>
#endif


/*--------------------------------------------------------------------------*/
void PegAppInitialize(PegPresentationManager *pPresent)
{
	PegRect Size;
	Size.Set(20, 20, 520, 400);
	pPresent->Add(new SpreadSheetWin(Size));
}

/*--------------------------------------------------------------------------*/
SpreadSheetWin::SpreadSheetWin(const PegRect &Size) :
    PegDecoratedWindow(Size, FF_THICK)
{
    static PegMenuDescription ColorSchemes[] = {
        {"Default",       IDM_COLOR_DEFAULT, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
        {"Blues",         IDM_COLOR_BLUES,   AF_ENABLED|BF_DOTABLE, NULL},
        {"Reds",          IDM_COLOR_REDS,    AF_ENABLED|BF_DOTABLE, NULL},
        {"Greens",        IDM_COLOR_GREENS,  AF_ENABLED|BF_DOTABLE, NULL},
        {"", 0, 0, NULL}
    };

    static PegMenuDescription StyleOptions[] = {
        {"Colors",         0,       AF_ENABLED,   ColorSchemes},
        {"",               0,       BF_SEPERATOR, NULL},
        {"Draw Partial Column",     IDM_PARTIAL_COL,    AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
        {"Multiple Row Select",     IDM_MULT_ROW_SEL,   AF_ENABLED|BF_CHECKABLE, NULL},
        {"Multiple Column Select",  IDM_MULT_COL_SEL,   AF_ENABLED|BF_CHECKABLE, NULL},
        {"Allow Cell Select",       IDM_CELL_SELECT,    AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
        {"", 0, 0, NULL}
    };

    static PegMenuDescription RowOptions[] = {
        {"Allow Select",  IDM_ROW_SEL, AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
        {"",              0,                 BF_SEPERATOR, NULL},
        {"Remove Row",    IDM_REMOVE_ROW,    AF_ENABLED, NULL},
        {"Insert Row",    IDM_INSERT_ROW,    AF_ENABLED, NULL},
        {"Add Row",       IDM_ADD_ROW,       AF_ENABLED, NULL},
        {"", 0, 0, NULL}
    };

    static PegMenuDescription ColOptions[] = {
        {"Align Right",   IDM_JUSTIFY_RIGHT, AF_ENABLED, NULL},
        {"Align Center",  IDM_JUSTIFY_CENTER,AF_ENABLED, NULL},
        {"Align Left",    IDM_JUSTIFY_LEFT,  AF_ENABLED, NULL},
        {"",              0,                 BF_SEPERATOR, NULL},
        {"Allow Select",  IDM_COL_SEL,       AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
        {"Draw Partial",  0,                 AF_ENABLED|BF_CHECKABLE|BF_CHECKED, NULL},
        {"",              0,                 BF_SEPERATOR, NULL},
        {"Remove Column",  IDM_REMOVE_COLUMN, AF_ENABLED, NULL},
        {"Insert Column",  IDM_INSERT_COLUMN, AF_ENABLED, NULL},
        {"Add Column",    IDM_ADD_COLUMN,    AF_ENABLED, NULL},
        {"",              0,                 BF_SEPERATOR, NULL},
        {"Raised",        IDM_CELL_RAISED,   AF_ENABLED|BF_DOTABLE, NULL},
        {"Recessed",      IDM_CELL_RECESSED, AF_ENABLED|BF_DOTABLE|BF_DOTTED, NULL},
        {"Flat",          IDM_CELL_FLAT,     AF_ENABLED|BF_DOTABLE, NULL},
        {"", 0, 0, NULL}
    };

    static PegMenuDescription HelpMenu[] = {
        {"About",         IDM_ABOUT,        AF_ENABLED, NULL},
        {"", 0, 0, NULL}
    };
        

    static PegMenuDescription MainMenu[] = {
        {"Help",            0,   AF_ENABLED, HelpMenu},
        {"Row Settings",    0,   AF_ENABLED, RowOptions},
        {"Column Settings", 0,   AF_ENABLED, ColOptions},
        {"Style Options",   0,   AF_ENABLED, StyleOptions},        
        {"", 0, 0, NULL}
        };
        
    Add(new PegTitle("PegSpreadSheet Example"));
    Add(new PegMenuBar(MainMenu));

    PegStatusBar *pBar = new PegStatusBar();
    mpStatPrompt = pBar->AddTextField(mReal.Width() - 50, 1, NULL);
    Add(pBar);

    PegRect SheetRect = mClient;
    SheetRect -= 10;

    SIGNED iVal, iVal1;
    char cTemp[40];
    char cTemp1[20];

    mpSpread = new PegSpreadSheet(SheetRect, SS_ROWS, SS_COLS, ID_SPREADSHEET);

    // initialize the column headers:

    strcpy(cTemp, "Column");

    for (iVal = 0; iVal < SS_COLS; iVal++)
    {
        if (iVal == 3)
        {
            continue;
        }
        ltoa(iVal, cTemp1, 10);
        strcpy(cTemp + 6, cTemp1);
        mpSpread->SetHeader(1, iVal, cTemp);
        if (iVal > 3)
        {
             mpSpread->SetColumnStyle(iVal, FF_RECESSED|TJ_CENTER|SCF_ALLOW_SELECT|SCF_CELL_SELECT);
        }
    }

    // initialize the row headers:

    strcpy(cTemp, "Row ");
    for (iVal = 0; iVal < SS_ROWS; iVal++)
    {
        ltoa(iVal, cTemp1, 10);
        strcpy(cTemp + 4, cTemp1);
        mpSpread->SetRowHeader(iVal, cTemp);
        mpSpread->SetRowStyle(iVal, SRF_ALLOW_SELECT);
    }

    // make column 3 a separator:

    mpSpread->SetColumnStyle(3, SCF_SEPARATOR);

    // start scrolling with column 4

    mpSpread->SetScrollStartCol(4);

    // fill in some dummy cell data:

    for (iVal = 0; iVal < SS_ROWS; iVal++)
    {
        for (iVal1 = 0; iVal1 < SS_COLS; iVal1++)
        {
            if (iVal1 == 3)     // no data for seperator column
            {
                continue;
            }
            ltoa((iVal + 1) * (iVal1 + 3), cTemp, 10);
            mpSpread->SetCellData(iVal, iVal1, cTemp);
        }
    }
    Add(mpSpread);
    mCount = 0;
}

COLORVAL NormalColors[10] = {
    PCLR_DIALOG,
    PCLR_NORMAL_TEXT,
    PCLR_HIGH_TEXT_BACK,
    PCLR_HIGH_TEXT,
    PCLR_DIALOG,
    PCLR_HIGH_TEXT,
    PCLR_HIGH_TEXT,
    PCLR_NORMAL_TEXT,
    PCLR_LOWLIGHT,
    PCLR_DIALOG
};


COLORVAL GreenColors[10] = {
    28,
    WHITE,
    BLACK,
    157,
    WHITE,
    34,
    WHITE,
    34,
    40,
    34,
};

COLORVAL RedColors[10] = {
    WHITE,
    18,
    54,
    WHITE,
    17,
    WHITE,
    17,
    WHITE,
    21,
    18,
};

COLORVAL BlueColors[10] = {
    BLACK,
    WHITE,
    230,
    BLACK,
    202,
    226,
    215,
    BLACK,
    215,
    215,
};

/*--------------------------------------------------------------------------*/
SIGNED SpreadSheetWin::Message(const PegMessage &Mesg)
{
    SIGNED wTemp, wLoop;

    switch(Mesg.wType)
    {
    case PM_SHOW:
        PegDecoratedWindow::Message(Mesg);
        SetTimer(1, ONE_SECOND / 4, ONE_SECOND / 4);
        break;

    case PM_TIMER:
        UpdateDynamicCell();
        break;

    case SIGNAL(ID_SPREADSHEET, PSF_CELL_SELECT):
        DisplayCellSelect(Mesg.Point);
        break;

    case SIGNAL(ID_SPREADSHEET, PSF_COL_SELECT):
        DisplayColSelect(Mesg.Point.x);
        break;

    case SIGNAL(ID_SPREADSHEET, PSF_ROW_SELECT):
        DisplayRowSelect(Mesg.Point.y);
        break;

    case SIGNAL(IDM_COLOR_DEFAULT, PSF_DOT_ON):
        ChangeColors(NormalColors);
        break;

    case SIGNAL(IDM_COLOR_BLUES, PSF_DOT_ON):
        ChangeColors(BlueColors);
        break;

    case SIGNAL(IDM_COLOR_REDS, PSF_DOT_ON):
        ChangeColors(RedColors);
        break;

    case SIGNAL(IDM_COLOR_GREENS, PSF_DOT_ON):
        ChangeColors(GreenColors);
        break;

    case SIGNAL(IDM_JUSTIFY_LEFT, PSF_CLICKED):
        SetSelectedColumnJustification(TJ_LEFT);
        break;

    case SIGNAL(IDM_JUSTIFY_CENTER, PSF_CLICKED):
        SetSelectedColumnJustification(TJ_CENTER);
        break;

    case SIGNAL(IDM_JUSTIFY_RIGHT, PSF_CLICKED):
        SetSelectedColumnJustification(TJ_RIGHT);
        break;

    case SIGNAL(IDM_CELL_SELECT, PSF_CHECK_ON):
        mpSpread->Style(mpSpread->Style() | SS_CELL_SELECT);
        break;        

    case SIGNAL(IDM_CELL_SELECT, PSF_CHECK_OFF):
        mpSpread->UnselectAll();
        mpSpread->Style(mpSpread->Style() & ~SS_CELL_SELECT);
        break;

    case SIGNAL(IDM_CELL_RAISED, PSF_DOT_ON):
        SetSelectedColumnFrame(FF_RAISED);
        break;

    case SIGNAL(IDM_CELL_RECESSED, PSF_DOT_ON):
        SetSelectedColumnFrame(FF_RECESSED);
        break;

    case SIGNAL(IDM_CELL_FLAT, PSF_DOT_ON):
        SetSelectedColumnFrame(FF_NONE);
        break;

    case SIGNAL(IDM_ROW_SEL, PSF_CHECK_ON):
        wTemp = mpSpread->GetRows();
        for (wLoop = 0; wLoop < wTemp; wLoop++)
        {
            mpSpread->SetRowStyle(wLoop, 
                mpSpread->GetRowStyle(wLoop) | SRF_ALLOW_SELECT);
        }
        break;

    case SIGNAL(IDM_ROW_SEL, PSF_CHECK_OFF):
        wTemp = mpSpread->GetRows();
        for (wLoop = 0; wLoop < wTemp; wLoop++)
        {
            mpSpread->SetRowStyle(wLoop, 
                mpSpread->GetRowStyle(wLoop) & ~SRF_ALLOW_SELECT);
        }
        break;

    case SIGNAL(IDM_COL_SEL, PSF_CHECK_ON):
        wTemp = mpSpread->GetCols();
        for (wLoop = 0; wLoop < wTemp; wLoop++)
        {
            mpSpread->SetColumnStyle(wLoop, 
                mpSpread->GetColumnStyle(wLoop) | SCF_ALLOW_SELECT);
        }
        break;

    case SIGNAL(IDM_COL_SEL, PSF_CHECK_OFF):
        wTemp = mpSpread->GetCols();
        for (wLoop = 0; wLoop < wTemp; wLoop++)
        {
            mpSpread->SetColumnStyle(wLoop, 
                mpSpread->GetColumnStyle(wLoop) & ~SCF_ALLOW_SELECT);
        }
        break;

    case SIGNAL(IDM_PARTIAL_COL, PSF_CHECK_ON):
        mpSpread->Style(mpSpread->Style() | SS_PARTIAL_COL);
        mpSpread->UpdateColLayout();
        mpSpread->Invalidate();
        mpSpread->Draw();
        break;

    case SIGNAL(IDM_PARTIAL_COL, PSF_CHECK_OFF):
        mpSpread->Style(mpSpread->Style() & ~SS_PARTIAL_COL);
        mpSpread->UpdateColLayout();
        mpSpread->Invalidate();
        mpSpread->Draw();
        break;

    case SIGNAL(IDM_MULT_ROW_SEL, PSF_CHECK_ON):
        mpSpread->Style(mpSpread->Style() | SS_MULTI_ROW_SELECT);
        break;

    case SIGNAL(IDM_MULT_ROW_SEL, PSF_CHECK_OFF):
        mpSpread->UnselectAll();
        mpSpread->Style(mpSpread->Style() & ~SS_MULTI_ROW_SELECT);
        break;

    case SIGNAL(IDM_MULT_COL_SEL, PSF_CHECK_ON):
        mpSpread->Style(mpSpread->Style() | SS_MULTI_COL_SELECT);
        break;

    case SIGNAL(IDM_MULT_COL_SEL, PSF_CHECK_OFF):
        mpSpread->UnselectAll();
        mpSpread->Style(mpSpread->Style() & ~SS_MULTI_COL_SELECT);
        break;

    case SIGNAL(IDM_ABOUT, PSF_CLICKED):
        {
            PegMessageWindow *pWin = new PegMessageWindow("PegSpreadSheet About",
                Version(), MW_OK|FF_RAISED);
            Presentation()->Center(pWin);
            Presentation()->Add(pWin);
            pWin->Execute();
        }
        break;

    case SIGNAL(IDM_ADD_ROW, PSF_CLICKED):
        mpSpread->AddRow("RowN");
        mpSpread->Invalidate();
        mpSpread->Draw();
        break;

    case SIGNAL(IDM_REMOVE_ROW, PSF_CLICKED):
        RemoveSelectedRows();
        break;

    case SIGNAL(IDM_INSERT_ROW, PSF_CLICKED):
        wTemp = mpSpread->GetSelectedRow();
        if (wTemp >= 0)
        {
            mpSpread->InsertRow(wTemp, "RowN");
            mpSpread->Invalidate();
            mpSpread->Draw();
        }
        break;        

    case SIGNAL(IDM_ADD_COLUMN, PSF_CLICKED):
        wLoop = TextWidth(" New Column ", &SysFont);
        wTemp = mpSpread->AddColumn(wLoop + 4, "New Column");
        mpSpread->SetColumnStyle(wTemp - 1,
                    mpSpread->GetColumnStyle(wTemp - 2) & ~(SCF_SELECTED|SCF_SEPARATOR));
        mpSpread->Invalidate();
        mpSpread->Draw();
        break;

    case SIGNAL(IDM_REMOVE_COLUMN, PSF_CLICKED):
        RemoveSelectedColumns();
        break;

    case SIGNAL(IDM_INSERT_COLUMN, PSF_CLICKED):
        wTemp = mpSpread->GetSelectedColumn();
        if (wTemp >= 0)
        {
            wLoop = TextWidth(" New Column ", &SysFont);
            //mpSpread->InsertColumn(wTemp, wLoop + 4, "New Column");
            mpSpread->InsertColumn(wTemp, 50, NULL);
            mpSpread->SetHeader(1, wTemp, "New Column");

            // copy the style of current column:
            
            if (wTemp)
            {
                mpSpread->SetColumnStyle(wTemp,
                    mpSpread->GetColumnStyle(wTemp - 1) & ~(SCF_SELECTED|SCF_SEPARATOR));
            }
            else
            {
                mpSpread->SetColumnStyle(wTemp,
                    mpSpread->GetColumnStyle(wTemp + 1) & ~(SCF_SELECTED|SCF_SEPARATOR));
            }
            mpSpread->Invalidate();
            mpSpread->Draw();
        }
        break;

    default:
        return PegDecoratedWindow::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void SpreadSheetWin::UpdateDynamicCell(void)
{
    char cTemp[40];

    // a couple of check in case the user deletes the cell we want to update:
    if (mpSpread->GetCols() > 4 && mpSpread->GetRows() > 3)
    {
        if (!(mpSpread->GetColumnStyle(4) & SCF_SEPARATOR))
        {
            ltoa(mCount, cTemp, 10);
            mpSpread->SetCellData(3, 4, cTemp, TRUE);
            mCount++;
        }
    }
}


/*--------------------------------------------------------------------------*/
void SpreadSheetWin::ChangeColors(COLORVAL *pClr)
{
    mpSpread->SetColor(PCI_NORMAL, *pClr);
    mpSpread->SetColor(PCI_NTEXT, *(pClr + 1));
    mpSpread->SetColor(PCI_SELECTED, *(pClr + 2));
    mpSpread->SetColor(PCI_STEXT, *(pClr + 3));
    mpSpread->SetColor(PCI_SS_COLHEADBACK, *(pClr + 4));
    mpSpread->SetColor(PCI_SS_COLHEADTEXT, *(pClr + 5));
    mpSpread->SetColor(PCI_SS_ROWHEADBACK, *(pClr + 6));
    mpSpread->SetColor(PCI_SS_ROWHEADTEXT, *(pClr + 7));
    mpSpread->SetColor(PCI_SS_DIVIDER, *(pClr + 8));
    mpSpread->SetColor(PCI_SS_BACKGROUND, *(pClr + 9));
    Invalidate(mpSpread->mReal);
    mpSpread->Draw();
}

/*--------------------------------------------------------------------------*/
void SpreadSheetWin::SetSelectedColumnJustification(WORD wJustify)
{
    SIGNED iIndex, iColumn;
    WORD wStyle;
    BOOL bFound = FALSE;

    iIndex = 0;
    iColumn = mpSpread->GetSelectedColumn(iIndex);

    while(iColumn >= 0)
    {
        bFound = TRUE;      // found a selected column
        wStyle = mpSpread->GetColumnStyle(iColumn);
        wStyle &= ~TJ_MASK;
        wStyle |= wJustify;
        mpSpread->SetColumnStyle(iColumn, wStyle);
        iIndex++;
        iColumn = mpSpread->GetSelectedColumn(iIndex);
    }

    if (bFound)
    {
        mpSpread->Invalidate();
        mpSpread->Draw();
    }
    else
    {
        PegMessageWindow *pWin = new PegMessageWindow("No Column(s) Selected",
            "Click on a column header to select a column.", FF_RAISED|MW_OK);

        Presentation()->Center(pWin);
        Presentation()->Add(pWin);
        pWin->Execute();
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SpreadSheetWin::SetSelectedColumnFrame(WORD wFrame)
{
    SIGNED iIndex, iColumn;
    WORD wStyle;
    BOOL bFound = FALSE;

    iIndex = 0;
    iColumn = mpSpread->GetSelectedColumn(iIndex);

    while(iColumn >= 0)
    {
        bFound = TRUE;      // found a selected column
        wStyle = mpSpread->GetColumnStyle(iColumn);
        wStyle &= ~FF_MASK;
        wStyle |= wFrame;
        mpSpread->SetColumnStyle(iColumn, wStyle);
        iIndex++;
        iColumn = mpSpread->GetSelectedColumn(iIndex);
    }

    if (bFound)
    {
        mpSpread->Invalidate();
        mpSpread->Draw();
    }
    else
    {
        PegMessageWindow *pWin = new PegMessageWindow("No Column(s) Selected",
            "Click on a column header to select a column.", FF_RAISED|MW_OK);

        Presentation()->Center(pWin);
        Presentation()->Add(pWin);
        pWin->Execute();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SpreadSheetWin::RemoveSelectedColumns(void)
{
    SIGNED iColumn;
    BOOL bFound = FALSE;

    iColumn = mpSpread->GetSelectedColumn(0);

    while(iColumn >= 0)
    {
        bFound = TRUE;      // found a selected column
        if (mpSpread->GetCols() > 1)
        {
            mpSpread->DeleteColumn(iColumn);
        }
        else
        {
            break;
        }
        iColumn = mpSpread->GetSelectedColumn(0);
    }

    if (bFound)
    {
        mpSpread->Invalidate();
        mpSpread->Draw();
    }
    else
    {
        PegMessageWindow *pWin = new PegMessageWindow("No Column(s) Selected",
            "Click on a column header to select a column.", FF_RAISED|MW_OK);

        Presentation()->Center(pWin);
        Presentation()->Add(pWin);
        pWin->Execute();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SpreadSheetWin::RemoveSelectedRows(void)
{
    SIGNED iRow;
    BOOL bFound = FALSE;

    iRow = mpSpread->GetSelectedRow(0);

    while(iRow >= 0)
    {
        bFound = TRUE;      // found a selected column
        if (mpSpread->GetRows() > 1)
        {
            mpSpread->DeleteRow(iRow);
        }
        else
        {
            break;
        }
        iRow = mpSpread->GetSelectedRow(0);
    }

    if (bFound)
    {
        mpSpread->Invalidate();
        mpSpread->Draw();
    }
    else
    {
        PegMessageWindow *pWin = new PegMessageWindow("No Row(s) Selected",
            "Click on a row header to select a row.", FF_RAISED|MW_OK);

        Presentation()->Center(pWin);
        Presentation()->Add(pWin);
        pWin->Execute();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SpreadSheetWin::DisplayCellSelect(PegPoint Cell)
{
    char cTemp[80];
    char cTemp1[40];
    strcpy(cTemp, "Cell Selected: (");
    //ltoa(Cell.x, cTemp1, 10);
    ltoa(Cell.y, cTemp1, 10);
    strcat(cTemp, cTemp1);
    strcat(cTemp, ",");
    //ltoa(Cell.y, cTemp1, 10);
    ltoa(Cell.x, cTemp1, 10);
    strcat(cTemp, cTemp1);
    strcat(cTemp, ")");
    mpStatPrompt->DataSet(cTemp);
    mpStatPrompt->Draw();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SpreadSheetWin::DisplayColSelect(SIGNED iCol)
{
    char cTemp[80];
    char cTemp1[40];
    strcpy(cTemp, "Column Selected: ");
    ltoa(iCol, cTemp1, 10);
    strcat(cTemp, cTemp1);
    mpStatPrompt->DataSet(cTemp);
    mpStatPrompt->Draw();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SpreadSheetWin::DisplayRowSelect(SIGNED iRow)
{
    char cTemp[80];
    char cTemp1[40];
    strcpy(cTemp, "Row Selected: ");
    ltoa(iRow, cTemp1, 10);
    strcat(cTemp, cTemp1);
    mpStatPrompt->DataSet(cTemp);
    mpStatPrompt->Draw();
}










