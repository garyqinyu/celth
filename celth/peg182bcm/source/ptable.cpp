/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ptable.cpp - PegTable class definition.
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

/*--------------------------------------------------------------------------*/
PegTable::PegTable(SIGNED iLeft, SIGNED iTop, SIGNED iRows, SIGNED iCols) :
    PegWindow(FF_NONE | TS_DRAW_GRID)
{
    Type(TYPE_TABLE);
    RemoveStatus(PSF_SIZEABLE);
    
    miRows = iRows;
    miCols = iCols;
    miCellPad = 0;
    mpCellInfo = NULL;
    miGridWidth = PEG_FRAME_WIDTH;

    if (iRows)
    {
        mpCellHeights = new SIGNED[iRows];
        memset(mpCellHeights, 0, iRows * sizeof(SIGNED));
    }
    else
    {
        mpCellHeights = NULL;
    }

    if (iCols)
    {
        mpCellWidths = new SIGNED[iCols];
        memset(mpCellWidths, 0, iCols * sizeof(SIGNED));
    }
    else
    {
        mpCellWidths = NULL;
    }
    mReal.wLeft = iLeft;
    mReal.wTop = iTop;
    mReal.wRight = mReal.wLeft + 1;
    mReal.wBottom = mReal.wTop + 1;

    mClient = mReal;
}


/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegTable::~PegTable()
{
    if (mpCellWidths)
    {
        delete [] mpCellWidths;
    }
    if (mpCellHeights)
    {
        delete [] mpCellHeights;
    }

    while(mpCellInfo)
    {
        TableCellInfo *pCurrent = mpCellInfo->pNext;
        delete mpCellInfo;
        mpCellInfo = pCurrent;
    }
}


/*--------------------------------------------------------------------------*/
void PegTable::Draw(void)
{
    if (!miRows || !miCols)
    {
        return;
    }

    BeginDraw();

    if (!(Style() & AF_TRANSPARENT))
    {
        StandardBorder(muColors[PCI_NORMAL]);
    }
    
    if (Style() & TS_DRAW_GRID)//(miGridWidth)
    {
        DrawGrid();
    }
    
    DrawChildren();
    EndDraw();
}


/*--------------------------------------------------------------------------*/
void PegTable::DrawGrid(void)
{
    PegColor Color(PCLR_BORDER, muColors[PCI_NORMAL], CF_FILL);
    SIGNED iRow, iCol, xPos, yPos;

    if (!miRows || !miCols)
    {
        return;
    }

    // draw the vertical cell grid lines:

    xPos = mClient.wLeft;
    
    if(Style() & TS_DRAW_VERT_GRID)
    {
        for (iCol = 0; iCol < miCols; iCol++)
        {
            if (!mpCellWidths[iCol])
            {
                continue;
            }
                
            yPos = mClient.wTop;
            for (iRow = 0; iRow < miRows; iRow++)
            {
                if (mpCellHeights[iRow])
                {
                    if (!CellSpansCol(iRow, iCol))
                    {
                        // Draw the left side of this cell:
                        Color.uForeground = PCLR_BORDER;
                        Line(xPos, yPos, xPos,
                            yPos + mpCellHeights[iRow], Color, miGridWidth);
    
                        if (miGridWidth >= 4)
                        {
                            // draw the highlights:
                            Color.uForeground = PCLR_HIGHLIGHT;
                            Line(xPos + 1, yPos /*+ 1*/, xPos + 1,
                                yPos + mpCellHeights[iRow], Color);
    
                            Color.uForeground = PCLR_LOWLIGHT;
                            Line(xPos + miGridWidth - 1,
                                yPos /*+ miGridWidth - 1*/,
                                xPos + miGridWidth - 1,
                                yPos + mpCellHeights[iRow],
                                Color);
                        }
                    }
                }
                yPos += mpCellHeights[iRow];
            }
            xPos += mpCellWidths[iCol];
        }
    }

    // draw the horizontal cell grid lines:

    yPos = mClient.wTop;

    if(Style() & TS_DRAW_HORZ_GRID)
    {
        for (iRow = 0; iRow < miRows; iRow++)
        {
            if (!mpCellHeights[iRow])
            {
                continue;
            }
            xPos = mClient.wLeft;
    
            for (iCol = 0; iCol < miCols; iCol++)
            {
                if (mpCellWidths[iCol])
                {
                    if (!CellSpansRow(iRow, iCol))
                    {
                        // draw the top side of this cell:
                        Color.uForeground = PCLR_BORDER;
                        Line(xPos, yPos, xPos + mpCellWidths[iCol],
                            yPos, Color, miGridWidth);
    
                        if (miGridWidth >= 4)
                        {
                            // draw the highlights:
    
                            Color.uForeground = PCLR_HIGHLIGHT;
                            Line(xPos, yPos + 1, xPos + mpCellWidths[iCol] - 1,
                                 yPos + 1, Color);
    
                            Color.uForeground = PCLR_LOWLIGHT;
                            Line(xPos, yPos + miGridWidth - 1,
                                xPos + mpCellWidths[iCol],
                                yPos + miGridWidth - 1, Color);
                        }
                    }
                }
                xPos += mpCellWidths[iCol];
            }
            yPos += mpCellHeights[iRow];
        }
    }
    
    // clean up by drawing the right edge:
    xPos = mReal.wLeft;
    for (iCol = 0; iCol < miCols; iCol++)
    {
        xPos += mpCellWidths[iCol];
    }
    yPos = mReal.wTop;
    for (iRow = 0; iRow < miRows; iRow++)
    {
        yPos += mpCellHeights[iRow];
    }

    Color.uForeground = PCLR_BORDER;
    if(Style() & TS_DRAW_VERT_GRID)
    {
        Line(xPos, mReal.wTop, xPos, mReal.wBottom,
             Color, miGridWidth);
        if (miGridWidth >= 4)
        {
            Color.uForeground = PCLR_HIGHLIGHT;
            Line(xPos + 1, mReal.wTop + 1, xPos + 1,
                mReal.wBottom - miGridWidth + 1, Color);

            Color.uForeground = PCLR_LOWLIGHT;
            Line(mReal.wRight, mReal.wTop, mReal.wRight,
                 mReal.wBottom - 1, Color);
        }
    }

    // and the bottom edge
    Color.uForeground = PCLR_BORDER;
    if(Style() & TS_DRAW_HORZ_GRID)
    {
        Line(mReal.wLeft, yPos, mReal.wRight, yPos,
             Color, miGridWidth);

        if (miGridWidth >= 4)
        {
            Color.uForeground = PCLR_HIGHLIGHT;
            Line(mReal.wLeft + 1,
                yPos + 1, mReal.wRight - miGridWidth + 2,
                yPos + 1, Color);

            Color.uForeground = PCLR_LOWLIGHT;
            Line(mReal.wLeft, mReal.wBottom, mReal.wRight,
                 mReal.wBottom, Color);
        }
    }
}


/*--------------------------------------------------------------------------*/
void PegTable::DrawChildren(void)
{
    // Before drawing each child, make sure it gets clipped to the cell size
    // that it is sitting within, just to prevent anything from drawing
    // where it shouldn't

    SIGNED iLoop;
    PegRect ChildRect;
    TableCellInfo *pCurrent = mpCellInfo;

    while(pCurrent)
    {
        ChildRect.wLeft = mClient.wLeft;

        for (iLoop = 0; iLoop < pCurrent->iCol; iLoop++)
        {
            ChildRect.wLeft += mpCellWidths[iLoop];
        }

        ChildRect.wRight = ChildRect.wLeft + mpCellWidths[iLoop];
        iLoop++;

        while(iLoop < pCurrent->iCol + pCurrent->iColSpan)
        {
            ChildRect.wRight += mpCellWidths[iLoop];
            iLoop++;
        }
      
        ChildRect.wTop = mClient.wTop;
        for (iLoop = 0; iLoop < pCurrent->iRow; iLoop++)
        {
            ChildRect.wTop += mpCellHeights[iLoop];
        }
        ChildRect.wBottom = ChildRect.wTop + mpCellHeights[iLoop];
        iLoop++;

        while(iLoop < pCurrent->iRow + pCurrent->iRowSpan)
        {
            ChildRect.wBottom += mpCellHeights[iLoop];
            iLoop++;
        }
        
        PegRect ClipRect = ChildRect;
        ClipRect -= miCellPad;
        ClipRect.wLeft += miGridWidth;
        ClipRect.wTop += miGridWidth;
        PegRect OldClip = pCurrent->pChild->mClip;
        pCurrent->pChild->mClip &= ClipRect;
        pCurrent->pChild->Draw();
        pCurrent->pChild->mClip = OldClip;
     
        pCurrent = pCurrent->pNext;
    }
}


/*--------------------------------------------------------------------------*/
void PegTable::SetGridWidth(SIGNED iWidth)
{
    miGridWidth = iWidth;
}


/*--------------------------------------------------------------------------*/
void PegTable::SetColWidth(SIGNED iCol, SIGNED iWidth, BOOL bForce)
{
    if (!miRows || !miCols)
    {
        return;
    }
    
    SIGNED iNewWidth = iWidth + miCellPad + miCellPad + miGridWidth;

    if (iNewWidth > mpCellWidths[iCol] || bForce)
    {
        mpCellWidths[iCol] = iNewWidth;
    }
}


/*--------------------------------------------------------------------------*/
void PegTable::SetRowHeight(SIGNED iRow, SIGNED iHeight, BOOL bForce)
{
    if (!miRows || !miCols)
    {
        return;
    }
    
    SIGNED iNewHeight = iHeight + miCellPad + miCellPad + miGridWidth;

    if (iNewHeight > mpCellHeights[iRow] || bForce)
    {
        mpCellHeights[iRow] = iNewHeight;
    }
}


/*--------------------------------------------------------------------------*/
void PegTable::SetCellClient(SIGNED iRow, SIGNED iCol, PegThing *Child,
    SIGNED iRowSpan, SIGNED iColSpan, WORD wStyle)
{
    if (!miRows || !miCols)
    {
        return;
    }

    // make sure we don't have this guy already:

    TableCellInfo *pCurrent = mpCellInfo;
    TableCellInfo *pLast = NULL;

    while(pCurrent)
    {
        if (pCurrent->iRow == iRow && pCurrent->iCol == iCol)
        {
            if (pCurrent->pChild)
            {
                Destroy(pCurrent->pChild);
            }
            pCurrent->pChild = Child;

            if (Child && StatusIs(PSF_VISIBLE))
            {
                Layout();
            }

            pCurrent->iRowSpan = iRowSpan;
            pCurrent->iColSpan = iColSpan;
            //pCurrent->bForceFit = bForceFit;
            pCurrent->wStyle = wStyle;
            break;
        }
        pLast = pCurrent;
        pCurrent = pCurrent->pNext;
    }

    if (!pCurrent)      // we didn't find a match
    {
        if (pLast)
        {
            pLast->pNext = new TableCellInfo;
            pCurrent = pLast->pNext;
        }
        else
        {
            mpCellInfo = new TableCellInfo;
            pCurrent = mpCellInfo;
        }
        pCurrent->iRow = iRow;
        pCurrent->iCol = iCol;
        pCurrent->iRowSpan = iRowSpan;
        pCurrent->iColSpan = iColSpan;
        //pCurrent->bForceFit = bForceFit;
        pCurrent->wStyle = wStyle;
        pCurrent->pChild = Child;

        if (Child && StatusIs(PSF_VISIBLE))
        {
            Layout();
        }
    }
}

/*--------------------------------------------------------------------------*/
PegThing *PegTable::GetCellClient(SIGNED iRow, SIGNED iCol)
{
    TableCellInfo *pCurrent = mpCellInfo;

    while(pCurrent)
    {
        if (pCurrent->iRow == iRow && pCurrent->iCol == iCol)
        {
            return (pCurrent->pChild);
        }
        pCurrent = pCurrent->pNext;
    }
    return NULL;
}

/*--------------------------------------------------------------------------*/
BOOL PegTable::GetRowCol(PegThing *pChild, SIGNED *pRow, SIGNED *pCol)
{
    // make sure we don't have this guy already:

    TableCellInfo *pCurrent = mpCellInfo;

    while(pCurrent)
    {
        if (pCurrent->pChild == pChild)
        {
            *pRow = pCurrent->iRow;
            *pCol = pCurrent->iCol;
            return TRUE;
        }
        pCurrent = pCurrent->pNext;
    }
    return FALSE;
}

/*--------------------------------------------------------------------------*/
PegThing *PegTable::RemoveCellClient(SIGNED iRow, SIGNED iCol)
{
    // make sure we don't have this guy already:

    TableCellInfo *pCurrent = mpCellInfo;
    TableCellInfo *pLast = NULL;
    PegThing *pChild = NULL;

    while(pCurrent)
    {
        if (pCurrent->iRow == iRow && pCurrent->iCol == iCol)
        {
            if (pLast)
            {
                pLast->pNext = pCurrent->pNext;
            }
            else
            {
                mpCellInfo = pCurrent->pNext;
            }
            if (pCurrent->pChild)
            {
                pChild = pCurrent->pChild;

                if (Style() & AF_TRANSPARENT)
                {
                    Remove(pChild, FALSE);
                    if (Parent())
                    {
                        Parent()->Draw();
                    }
                }
                else
                {
                    Remove(pChild);
                }
                delete pCurrent;
                return (pChild);
            }
        }
        pLast = pCurrent;
        pCurrent = pCurrent->pNext;
    }
    return pChild;
}


/*--------------------------------------------------------------------------*/
void PegTable::Reconfigure(SIGNED iRows, SIGNED iCols, SIGNED iGridWidth,
    SIGNED iCellPadding)
{
    miRows = iRows;
    miCols = iCols;
    miCellPad = iCellPadding;
    miGridWidth = iGridWidth;

    if (mpCellHeights)
    {
        delete mpCellHeights;
    }

    if (mpCellWidths)
    {
        delete mpCellWidths;
    }

    if (iRows)
    {
        mpCellHeights = new SIGNED[iRows];
        memset(mpCellHeights, 0, iRows * sizeof(SIGNED));
    }
    else
    {
        mpCellHeights = NULL;
    }

    if (iCols)
    {
        mpCellWidths = new SIGNED[iCols];
        memset(mpCellWidths, 0, iCols * sizeof(SIGNED));
    }
    else
    {
        mpCellWidths = NULL;
    }
    Layout();
}


/*--------------------------------------------------------------------------*/
void PegTable::Layout(void)
{
    SIGNED iLoop, iLoop1;
    SIGNED iTotal, iTemp, iTemp1, iRow, iCol;

    TableCellInfo *pCurrent = mpCellInfo;
    PegThing *pChild;

    // set all the row heights and column widths for things that don't
    // span multiple columns or rows:

    while(pCurrent)
    {
        if ((pCurrent->wStyle & TCF_FORCEFIT) && pCurrent->iRowSpan < 2 && 
                pCurrent->iColSpan < 2)
        {
            pChild = pCurrent->pChild;
            if(pChild)
            {
                SetRowHeight(pCurrent->iRow, pChild->mReal.Height());
                SetColWidth(pCurrent->iCol, pChild->mReal.Width());
            }
        }
        pCurrent = pCurrent->pNext;
    }

    pCurrent = mpCellInfo;

    // set all row heights and column widths for things that do span
    // multiple columns or rows:

    while(pCurrent)
    {
        if ((pCurrent->wStyle & TCF_FORCEFIT) && ((pCurrent->iRowSpan > 1) ||
            (pCurrent->iColSpan > 1)))
        {
            pChild = pCurrent->pChild;

            if(!pChild)
            {
                continue;
            }

            // add up the cell heights for the cells spanned by this child, 
            // and make bigger if necessary:

            iRow = pCurrent->iRow;
            iTotal = mpCellHeights[iRow];

            for (iTemp = iRow + 1; iTemp < iRow + pCurrent->iRowSpan; iTemp++)
            {
                iTotal += mpCellHeights[iTemp];
            }
    
            if (iTotal)
            {
                iTotal -= miCellPad + miCellPad + miGridWidth;
            }
  
            if (iTotal < pChild->mReal.Height())
            {

                // Look for a zero height cell spanned by this element.
                // If there is one, add the extra required height to this cell.

                iTemp = pChild->mReal.Height() - iTotal;

                for (iLoop1 = 0; iLoop1 < pCurrent->iRowSpan; iLoop1++)
                {
                    if (!mpCellHeights[iRow + iLoop1])
                    {
                        SetRowHeight(iRow + iLoop1, iTemp);
                        iTemp = 0;
                        break;
                    }
                }
                // add an equal amount to each spanned row, so 
                // that the element will fit:

                if (iTemp)
                {
                    iTemp1 = iTemp / pCurrent->iRowSpan;
                    iTemp1++;

                    for (iLoop1 = 0; iLoop1 < pCurrent->iRowSpan; iLoop1++)
                    {
                        SetRowHeight(iRow + iLoop1, mpCellHeights[iRow + iLoop1] + iTemp1);
                    }
                }
            }

            iCol = pCurrent->iCol;

            iTotal = mpCellWidths[iCol];

            for (iTemp = iCol + 1; iTemp < iCol + pCurrent->iColSpan; iTemp++)
            {
                iTotal += mpCellWidths[iTemp];
            }

            if (iTotal)
            {
                iTotal -= miCellPad + miCellPad + miGridWidth;
            }
            if (iTotal < pChild->mReal.Width())
            {
                // Look for a zero width column spanned by this element.
                // If there is one, add required width to that column:

                iTemp = pChild->mReal.Width() - iTotal;

                for (iLoop1 = 0; iLoop1 < pCurrent->iColSpan; iLoop1++)
                {
                    if (!mpCellWidths[iCol + iLoop1])
                    {
                        SetColWidth(iCol + iLoop1, iTemp);
                        iTemp = 0;
                        break;
                    }
                }
                // add an equal amount to each spanned column, so that
                // the element will fit:

                if (iTemp)
                {
                    iTemp1 = iTemp / pCurrent->iColSpan;
                    iTemp1++;

                    for (iLoop1 = 0; iLoop1 < pCurrent->iColSpan; iLoop1++)
                    {
                        SetColWidth(iCol + iLoop1, mpCellWidths[iCol + iLoop1] + iTemp1);
                    }
                }
            }
        }
        pCurrent = pCurrent->pNext;
    }

    // add up the column widths and cell heights and resize myself:

    PegRect NewSize = mReal;

    NewSize.wRight = NewSize.wLeft + miGridWidth - 1;
    NewSize.wBottom = NewSize.wTop + miGridWidth - 1;

    for (iLoop = 0; iLoop < miCols; iLoop++)
    {
        NewSize.wRight += mpCellWidths[iLoop];
    }
    for (iLoop = 0; iLoop < miRows; iLoop++)
    {
        NewSize.wBottom += mpCellHeights[iLoop];
    }
    Resize(NewSize);

    // position each of my children-
    
    pCurrent = mpCellInfo;

    while(pCurrent)
    {
        SIGNED iCellOffset = 0;
        SIGNED xPos = mReal.wLeft + miGridWidth;

        for (iLoop = 0; iLoop < pCurrent->iCol; iLoop++)
        {
            xPos += mpCellWidths[iLoop];
        }
      
        if(pCurrent->wStyle & TCF_HCENTER && pCurrent->pChild)
        {
            iCellOffset = ((mpCellWidths[iLoop] - (miCellPad * 2) -
                           pCurrent->pChild->mReal.Width()) >> 1) - 1;
        }
        else if(pCurrent->wStyle & TCF_HRIGHT && pCurrent->pChild)
        {
            iCellOffset = mpCellWidths[iLoop] - (miCellPad * 2) -
                          pCurrent->pChild->mReal.Width();
        }

        xPos += iCellOffset;

        SIGNED yPos = mClient.wTop + miGridWidth;
        for (iLoop = 0; iLoop < pCurrent->iRow; iLoop++)
        {
            yPos += mpCellHeights[iLoop];
        }

        if(pCurrent->wStyle & TCF_VCENTER && pCurrent->pChild)
        {
            iCellOffset = ((mpCellHeights[iLoop] - (miCellPad * 2) -
                            pCurrent->pChild->mReal.Height()) >> 1) - 1;
        }
        else if(pCurrent->wStyle & TCF_VBOTTOM && pCurrent->pChild)
        {
            iCellOffset = mpCellHeights[iLoop] - (miCellPad * 2) -
                          pCurrent->pChild->mReal.Height();
        }

        yPos += iCellOffset;

        if(pCurrent->pChild)
        {
            PegRect mPut = pCurrent->pChild->mReal;
            mPut.MoveTo(xPos + miCellPad, yPos + miCellPad);
            pCurrent->pChild->Resize(mPut);
            Add(pCurrent->pChild);
        }

        pCurrent = pCurrent->pNext;
    }
}

/*--------------------------------------------------------------------------*/
BOOL PegTable::CellSpansRow(SIGNED iRow, SIGNED iCol)
{
    // go through all of the child cells and see if any of them span this
    // row:

    if (!iRow)
    {
        return FALSE;
    }

    TableCellInfo *pCurrent = mpCellInfo;

    while(pCurrent)
    {
        if (pCurrent->iRow >= iRow || pCurrent->iCol > iCol)
        {
            pCurrent = pCurrent->pNext;
            continue;
        }

        if (pCurrent->iCol + pCurrent->iColSpan >= iCol)
        {
            // this cell is in the right column, check the rows

            if (pCurrent->iRow + pCurrent->iRowSpan - 1 >= iRow)
            {
                return TRUE;
            }
        }
        pCurrent = pCurrent->pNext;
    }
    return FALSE;
}

/*--------------------------------------------------------------------------*/
BOOL PegTable::CellSpansCol(SIGNED iRow, SIGNED iCol)
{
    // go through all of the child cells and see if any of them span this
    // column:

    if (!iCol)
    {
        return FALSE;
    }

    TableCellInfo *pCurrent = mpCellInfo;

    while(pCurrent)
    {
        if (pCurrent->iRow > iRow || pCurrent->iCol >= iCol)
        {
            pCurrent = pCurrent->pNext;
            continue;
        }

        if (pCurrent->iRow + pCurrent->iRowSpan - 1 >= iRow)
        {
            // this cell is in the right row, check the columns

            if (pCurrent->iCol + pCurrent->iColSpan > iCol)
            {
                return TRUE;
            }
        }
        pCurrent = pCurrent->pNext;
    }
    return FALSE;
}




// End of file

