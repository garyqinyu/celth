/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// peditbox.cpp - Multi-line edit control implementation.
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
//#include "ctype.h"
#include "peg.hpp"

extern PEGCHAR lsTEST[];
extern PEGCHAR lsSPACE[];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegEditBox::PegEditBox(const PegRect &Rect, WORD wId, WORD wStyle, PEGCHAR *Text,
    WORD wMaxChars) : PegTextBox(Rect, wId, wStyle, Text, wMaxChars)
{
    mCursor.x = 0;
    mCursor.y = 0;

    mwStyle &= ~TJ_MASK;
    mwStyle |= TJ_LEFT;
    Type(TYPE_EDITBOX);

    State.mbEditMode = FALSE;
    State.mbMarked = FALSE;
    State.mbMarkMode = FALSE;
    State.bPointerSet = FALSE;
    mMarkAnchor.x = mMarkAnchor.y = mMarkEnd.x = mMarkEnd.y = -1;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegEditBox::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    case PM_HIDE:
        if (State.mbEditMode)
        {
            ExitEditMode();
        }
        if (State.bPointerSet)
        {
            SetPointerType(PPT_NORMAL);
            State.bPointerSet = FALSE;
        }
        PegTextBox::Message(Mesg);
        break;

    case PM_LBUTTONDOWN:
        // figure out where in the string they clicked, set mCursor,
        // set EditMode:

        if (!(mwStyle & EF_EDIT))
        {
            return(PegWindow::Message(Mesg));
        }

        if (State.mbEditMode)
        {
            RemoveCursor();
        }
        else
        {
            if (State.mbMarked)
            {
                State.mbMarked = FALSE;
                Invalidate();
                Draw();
            }
        }

        CapturePointer();
        GetCursorRowColFromClick(Mesg.Point);
        mMarkAnchor = mCursor;
        mMarkEnd = mCursor;
        State.mbEditMode = FALSE;
        State.mbMarked = FALSE;
        State.mbMarkMode = TRUE;
        break;

    case PM_POINTER_MOVE:
        if (StatusIs(PSF_OWNS_POINTER))
        {
            if (mMarkAnchor != mMarkEnd &&
                Mesg.Point.y < mClient.wTop)
            {
                LineUp();
            }
            GetCursorRowColFromClick(Mesg.Point);
            CheckMarkMove();
        }
        else
        {
            return PegWindow::Message(Mesg);
        }
        break;

    case PM_LBUTTONUP:
        if (StatusIs(PSF_OWNS_POINTER))
        {
            State.mbMarkMode = FALSE;
            if (mMarkAnchor == mMarkEnd)
            {
                State.mbEditMode = TRUE;
                State.mbMarked = FALSE;
                DrawCursor();
            }
            else
            {    
                State.mbEditMode = FALSE;
                State.mbMarked = TRUE;
                CheckSendSignal(PSF_TEXT_SELECT);
            }
            ReleasePointer();

            if (State.bPointerSet)
            {
                if (!mReal.Contains(Mesg.Point))
                {
                    SetPointerType(PPT_IBEAM);
                    State.bPointerSet = FALSE;
                }
            }
        }
        break;

    case PM_POINTER_ENTER:
        if (mwStyle & EF_EDIT)
        {
            SetPointerType(PPT_IBEAM);
            State.bPointerSet = TRUE;
        }
        break;

    case PM_POINTER_EXIT:
        if (State.bPointerSet)
        {
            SetPointerType(PPT_NORMAL);
            State.bPointerSet = FALSE;
        }
        break;

   #if defined(PEG_KEYBOARD_SUPPORT)
    case PM_GAINED_KEYBOARD:
   #else
    case PM_CURRENT:
   #endif
        PegTextBox::Message(Mesg);
        if (mwStyle & EF_EDIT)
        {
            if (!State.mbMarkMode)
            {
                if (DataGet())
                {
                    SetCursorRowColFromIndex(strlen(DataGet()));
                }
                else
                {
                    SetCursorRowColFromIndex(0);
                }
                State.mbEditMode = TRUE;
                State.mbMarked = FALSE;
                DrawCursor();
            }
        }
        else
        {
            if (mwStyle & AF_ENABLED)
            {
                if (DataGet())
                {
                    SetCursorRowColFromIndex(0);
                    mMarkAnchor = mCursor;
                    SetCursorRowColFromIndex(strlen(DataGet()));
                    mMarkEnd = mCursor;
                    State.mbMarked = TRUE;
                    Invalidate();
                    Draw();
                }
            }
        }
        break;

   #if defined(PEG_KEYBOARD_SUPPORT)
    case PM_LOST_KEYBOARD:
   #else
    case PM_NONCURRENT:
   #endif
        PegTextBox::Message(Mesg);

        if (State.bPointerSet)
        {
            SetPointerType(PPT_NORMAL);
            State.bPointerSet = FALSE;
        }

        if (State.mbEditMode)
        {
            RemoveCursor();
            ExitEditMode();
        }
        else
        {
            if (State.mbMarked)
            {
                State.mbMarked = FALSE;
                Invalidate();
                Draw();
            }
        }
        break;

    case PM_COPY:
        CopyToScratchPad();
        break;

    case PM_PASTE:
        PasteFromScratchPad();
        CheckSendSignal(PSF_TEXT_EDIT);
        break;

    case PM_CUT:
        CopyToScratchPad();
        DeleteMarkedText();
        CheckSendSignal(PSF_TEXT_EDIT);
        break;

   #ifdef PEG_KEYBOARD_SUPPORT

    case PM_KEY:
        if ((Style() & EF_EDIT) && (State.mbEditMode || State.mbMarked))
        {
            if (Mesg.lData & (KF_SHIFT|KF_CTRL))
            {
                if (CheckControlKey((WORD) Mesg.iData, (SIGNED) Mesg.lData))
                {
                    return 0;
                }
            }
                
            if (InsertKey(Mesg.iData))
            {
                CheckSendSignal(PSF_TEXT_EDIT);
            }
            else
            {
                return PegTextBox::Message(Mesg);
            }
        }
        else
        {
            return PegTextBox::Message(Mesg);
        }
        break;
   #endif

    default:
        return(PegTextBox::Message(Mesg));
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegEditBox::Draw(void)
{
    BeginDraw();
    PegTextBox::Draw();
    DrawCursor();
    EndDraw();
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegEditBox::ExitEditMode()
{
    State.mbEditMode = FALSE;
    CheckSendSignal(PSF_TEXT_EDITDONE);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegEditBox::DataSet(const PEGCHAR *Text)
{
    PegTextThing::DataSet(Text);

    UpdateLineStarts();

    if (miTopLine &&
        miTopLine + miVisible >= miTotalLines)
    {
        SetTopLineToEnd();
    }

    if (muScrollMode & WSM_AUTOSCROLL)
    {
        if(CheckAutoScroll())
        {
            UpdateLineStarts();
            CheckAutoScroll();
        }
    }

    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate();
    }
    State.mbMarked = FALSE;

    if (!Text || mCursor.y >= miTotalLines)
    {
        mCursor.x = mCursor.y = 0;
    }
    GetCursorPointFromRowCol();
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegEditBox::DrawCursor(void)
{
    if (State.mbEditMode)
    {
        GetCursorPointFromRowCol();

       #ifdef PEG_RUNTIME_COLOR_CHECK
        PegColor Color(PCLR_CURSOR, BLACK, CF_NONE);
        if (NumColors() < 4)
        {
            Color.uForeground = BLACK;
        }
       #else
        PegColor Color(PCLR_CURSOR, BLACK, CF_NONE);
       #endif

        ClipToClient();
        Invalidate();
        BeginDraw();
        Line(mCursorPos.x, mCursorPos.y, 
            mCursorPos.x, mCursorPos.y + miLineHeight - 1, Color);
        EndDraw();
        EndClipToClient();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegEditBox::RemoveCursor(void)
{
    SIGNED iLen;

    if (State.mbEditMode)
    {
        ClipToClient();

        PegColor Color(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
        PegRect Invalid;
        Invalid.Set(mCursorPos.x, mCursorPos.y, mCursorPos.x + 2,
            mCursorPos.y + miLineHeight);
        Invalidate(Invalid);

        // redraw the character after the cursor, if there is one

        PEGCHAR *pGet = GetLineStart(mCursor.y, &iLen);

        BeginDraw();

        if (pGet && iLen > 0)
        {
            pGet += mCursor.x;

            if (*pGet && *pGet != 0x0d && *pGet != 0x0a &&
                mCursor.x != iLen)
            {
                PEGCHAR cTemp[2];
                cTemp[0] = *pGet;
                cTemp[1] = '\0';
                DrawText(mCursorPos, cTemp, Color, mpFont);
            }
            else
            {
                Color.uForeground = muColors[PCI_NORMAL];
                Line(mCursorPos.x, mCursorPos.y, 
                    mCursorPos.x, mCursorPos.y + miLineHeight, Color);
            }
        }
        else
        {
            Color.uForeground = muColors[PCI_NORMAL];
            Line(mCursorPos.x, mCursorPos.y, 
            mCursorPos.x, mCursorPos.y + miLineHeight, Color);
        }
        EndDraw();
        EndClipToClient();
    }
}

/*--------------------------------------------------------------------------*/
// DrawCharAtCursor: Draw a single character at the current cursor position.
/*--------------------------------------------------------------------------*/

void PegEditBox::DrawCharAtCursor(SIGNED iKey)
{
    if (iKey == 0x0d || iKey == 0x0a)
    {
        return;
    }
    ClipToClient();
    PegColor Color(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
    PEGCHAR cTemp[2];
    cTemp[0] = (PEGCHAR) iKey;
    cTemp[1] = '\0';
    SIGNED iWidth = TextWidth(cTemp, mpFont);
    PegRect Invalid;
    Invalid.Set(mCursorPos.x, mCursorPos.y, mCursorPos.x + iWidth,
        mCursorPos.y + miLineHeight);
    Invalidate(Invalid);
    BeginDraw();
    DrawText(mCursorPos, cTemp, Color, mpFont);
    EndDraw();
    EndClipToClient();
}


/*--------------------------------------------------------------------------*/
// DrawTextLine: Called from ScrollText and DrawAllText to draw an
// individual line of text. It is assumed that the background has been
// initialized to the correct color.
/*--------------------------------------------------------------------------*/
void PegEditBox::DrawTextLine(SIGNED iLine, PegPoint PutPoint, BOOL bFill)
{
    if (State.mbMarked)
    {
        // check to see if all or part of this line is marked:

        if ((mMarkAnchor.y >= iLine && mMarkEnd.y <= iLine) ||
            (mMarkAnchor.y <= iLine && mMarkEnd.y >= iLine))
        {
            DrawMarkedText(iLine, PutPoint);
            return;
        }
    }
    PegTextBox::DrawTextLine(iLine, PutPoint, bFill);
}

/*--------------------------------------------------------------------------*/
void PegEditBox::DrawOneLine(SIGNED iLine)
{
    PegPoint Put;
    //PegColor Color(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
    Put.x = mClient.wLeft + 2;
    Put.y = mClient.wTop + 2;
    Put.y += (iLine - miTopLine) * miLineHeight;
    PegRect Invalid;
    Invalid.Set(Put.x, Put.y, mClient.wRight - 2, Put.y + miLineHeight - 1);
    Invalidate(Invalid);
    ClipToClient();
    BeginDraw();
    //Rectangle(Invalid, Color, 0);
    DrawTextLine(iLine, Put, TRUE);
    EndDraw();
    EndClipToClient();
}

/*--------------------------------------------------------------------------*/
// DrawSelectText: Called when the user is in the process of selecting
// characters. Improves performance over simply re-drawing the entire line.
/*--------------------------------------------------------------------------*/
void PegEditBox::DrawSelectText(SIGNED iLine, SIGNED iFirst, SIGNED iLast,
    BOOL bSelected)
{
    SIGNED iLength;
    PegColor Color;
    
    if (bSelected)
    {
        Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_FILL);
    }
    else
    {
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
    }
        
    if (iFirst > iLast)
    {
        PEGSWAP(iFirst, iLast);
    }

    PegPoint Put;
    Put.x = mClient.wLeft + 2;
    Put.y = mClient.wTop + 2;

    Put.y += (iLine - miTopLine) * miLineHeight;
    PegRect ClearRect;
    ClearRect.wTop = Put.y;
    ClearRect.wBottom = ClearRect.wTop + miLineHeight;
    ClearRect.wLeft = mClient.wLeft + 2;
    ClearRect.wRight = mClient.wRight - 2;

    PEGCHAR *pGet = GetLineStart(iLine, &iLength);

    if (!pGet || iLength <= 0)
    {
        return;
    }
    CheckBufLen(iLength);
    strncpy(mpBuf, pGet, iLength);
    mpBuf[iLength] = '\0';
    pGet = mpBuf;
    
    if (*pGet)
    {
        ClipToClient();
        Invalidate(ClearRect);
        BeginDraw();

        if (iFirst > 0)
        {
            PEGCHAR cTemp = *(pGet + iFirst);
            *(pGet + iFirst) = '\0';
            Put.x += TextWidth(pGet, mpFont);
            *(pGet + iFirst) = cTemp;
        }
        mpBuf[iLast] = '\0';
        DrawText(Put, pGet + iFirst, Color, mpFont);
        EndDraw();
        EndClipToClient();
    }
}


/*--------------------------------------------------------------------------*/
void PegEditBox::CheckMarkMove(void)
{
    PegPoint OldEnd = mMarkEnd;
    SIGNED iLine;
   
    if (mCursor != mMarkEnd)
    {
        ClipToClient();
        Invalidate();
        BeginDraw();

        mMarkEnd = mCursor;
        State.mbEditMode = FALSE;
        State.mbMarked = TRUE;

#if 0
        // this version prevents marking past the end of visible text
        if (mMarkEnd.y >= miTopLine + miVisible)
        {
        //    mMarkEnd.y = miTopLine + miVisible - 1;
        }
#endif    
        if (mMarkEnd.y != OldEnd.y)
        {
            if (OldEnd.y < mMarkEnd.y)
            {

                for (iLine = OldEnd.y; iLine <= mMarkEnd.y; iLine++)
                {
                    if (iLine >= miTopLine + miVisible)
                    {
                        LineDown();
                    }
                    else
                    {
                        DrawOneLine(iLine);
                    }
                }
            }
            else
            {
                for (iLine = OldEnd.y; iLine >= mMarkEnd.y; iLine--)
                {
                    DrawOneLine(iLine);
                }
            }
        }
        else
        {
            // see if we are getting closer to or farther from the Anchor:

            PegPoint StartMark, EndMark;
            GetMarkStartAndEnd(&StartMark, &EndMark);
            BOOL bSelect = TRUE;

            if (EndMark == mMarkEnd)    // End is after anchor
            {
                if (OldEnd.x >= mMarkEnd.x ||
                    EndMark == StartMark)
                {
                    bSelect = FALSE;
                }
            }
            else                        // End is ahead of the anchor
            {
                if (OldEnd.x < mMarkEnd.x)
                {
                    bSelect = FALSE;
                }
            }
            DrawSelectText(mMarkEnd.y, OldEnd.x, mMarkEnd.x, bSelect);
        }
        EndDraw();
        EndClipToClient();
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegEditBox::DrawMarkedText(SIGNED iLine, PegPoint Put)
{
    PEGCHAR *pGet;
    PegColor Color(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
    SIGNED iLength;

    PegPoint StartMark, EndMark;

    GetMarkStartAndEnd(&StartMark, &EndMark);
    PEGCHAR *pLine = GetLineStart(iLine, &iLength);

    ClipToClient();

    if (iLength <=  0)
    {
        PegRect FillRect;
        FillRect.Set(Put.x, Put.y, mClient.wRight - 2, Put.y + miLineHeight - 1);
        Rectangle(FillRect, Color, 0);
        FillRect.wRight = FillRect.wLeft + TextWidth(lsSPACE, mpFont) - 1;
        Color.uBackground = muColors[PCI_SELECTED];
        Rectangle(FillRect, Color, 0);
        EndClipToClient();
        return;
    }
   
    // draw any leading unmarked characters on this line:
    
    CheckBufLen(iLength);
    if (StartMark.y == iLine && StartMark.x)
    {
        strncpy(mpBuf, pLine, StartMark.x);
        mpBuf[StartMark.x] = '\0';
        pGet = mpBuf;
        DrawText(Put, pGet, Color, mpFont);
        Put.x += TextWidth(pGet, mpFont);
    }

    // draw any marked characters on this line:

    strncpy(mpBuf, pLine, iLength);
    if (EndMark.y == iLine)
    {
        mpBuf[EndMark.x] = '\0';
    }
    else
    {
        mpBuf[iLength] = '\0';
    }

    if (StartMark.y == iLine)
    {
        pGet = mpBuf + StartMark.x;
    }
    else
    {
        pGet = mpBuf;
    }

    Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_FILL);
    DrawText(Put, pGet, Color, mpFont);
    Put.x += TextWidth(pGet, mpFont);

    // draw any trailing characters:

    if (EndMark.y == iLine && EndMark.x < (SIGNED) iLength)
    {
        strncpy(mpBuf, pLine, iLength);
        mpBuf[iLength] = '\0';
        pGet = mpBuf + EndMark.x;
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
        DrawText(Put, pGet, Color, mpFont);
        Put.x += TextWidth(pGet, mpFont);
    }

    if (Put.x < mClient.wRight - 2)
    {
        PegRect FillRect;
        FillRect.Set(Put.x, Put.y, mClient.wRight - 2, Put.y + miLineHeight - 1);
        Color.Set(muColors[PCI_NORMAL], muColors[PCI_NORMAL], CF_FILL);
        Rectangle(FillRect, Color, 0);
    }
    EndClipToClient();
}

#ifdef PEG_KEYBOARD_SUPPORT

BOOL PegEditBox::CheckControlKey(WORD iKey, SIGNED iCtrl)
{
    SIGNED iIndex;
    BOOL bMark = FALSE;

    if (iCtrl & KF_SHIFT)
    {
        if (!State.mbMarked)
        {
            mMarkAnchor = mCursor;
            mMarkEnd = mCursor;
        }
        switch(iKey)
        {
        case PK_LNUP:
            if (mCursor.y > miTopLine)
            {
                PegPoint NewPick = mCursorPos;
                NewPick.y -= miLineHeight / 2;
                GetCursorRowColFromClick(NewPick);
                GetCursorPointFromRowCol();
            }
            else
            {
                if (miTopLine && mpVScroll)
                {
                    mCursor.y--;
                    mCursor.x = 0;
                    AutoScrollUp();
                }
            }
            bMark = TRUE;
            break;

        case PK_LNDN:
            if (mCursor.y < miTotalLines - 1)
            {
                PegPoint NewPick = mCursorPos;
                NewPick.y += miLineHeight * 5 / 4;
                GetCursorRowColFromClick(NewPick);
                CheckAutoScrollDown();
                GetCursorPointFromRowCol();
            }
            bMark = TRUE;
            break;

        case PK_RIGHT:
            if (!DataGet())
            {
                return(FALSE);
            }
            GetLineStart(mCursor.y, &iIndex);

            if (mCursor.x < iIndex)
            {
                mCursor.x++;
            }
            else
            {
                if (GetLineStart(mCursor.y + 1, NULL))
                {
                    mCursor.y++;
                    mCursor.x = 0;
                    CheckAutoScrollDown();
                }
            }
            bMark = TRUE;
            break;

        case PK_LEFT:
            if (mCursor.x)
            {
                mCursor.x--;
                bMark = TRUE;
                break;
            }
            if (mCursor.y)
            {
                if (mCursor.y > miTopLine)
                {
                    mCursor.y--;
                    GetLineStart(mCursor.y, &mCursor.x);
                    if (mCursor.x)
                    {
                        mCursor.x--;
                    }
                }
                else
                {
                    if (miTopLine && mpVScroll)
                    {   
                        mCursor.y--;
                        GetLineStart(mCursor.y, &mCursor.x);
                        if (mCursor.x)
                        {
                            mCursor.x--;
                        }
                        AutoScrollUp();
                    }
                }
            }
            bMark = TRUE;
            break;

        case PK_HOME:
            if (mCursor.x)
            {
                mCursor.x = 0;
                bMark = TRUE;
            }
            break;
        }

        if (bMark)
        {
            if (!(State.mbMarked))
            {
                //RemoveCursor();
                State.mbEditMode = FALSE;
                State.mbMarked = TRUE;
            }
            CheckMarkMove();
        }
    }
    return bMark;
}

#endif

/*--------------------------------------------------------------------------*/
BOOL PegEditBox::InsertKey(WORD wKey)
{
    SIGNED iIndex;

    #ifdef PEG_UNICODE
    PEGCHAR cTemp[2];
    cTemp[0] = wKey;
    cTemp[1] = 0;
    #endif

    if (State.mbMarked)

    {
       #ifdef PEG_UNICODE
        if (wKey >= 0x20 && TextWidth(cTemp, mpFont) > 0)
       #else
        if (wKey < PK_DELETE && wKey >= 0x20)
       #endif
        {
            ReplaceMarkedText(wKey);
            State.mbMarked = FALSE;
            State.mbEditMode = TRUE;
            //mCursor = mMarkAnchor;
            //mCursor.x++;
            DrawCursor();
            return TRUE;
        }
        else
        {
            switch(wKey)
            {
            case PK_RIGHT:
            case PK_END:
            case PK_LEFT:
            case PK_HOME:
            case PK_LNUP:
            case PK_LNDN:
                // update flags and fall through to below:
                State.mbEditMode = TRUE;
                State.mbMarked = FALSE;
                Invalidate();
                Draw();
                break;

            case PK_DELETE:
                DeleteMarkedText();
                return TRUE;

            default:
                return FALSE;
            }
        }
    }

    #ifdef PEG_UNICODE
    if (wKey >= 0x20 && TextWidth(cTemp, mpFont) > 0)
    #else
    if (wKey < PK_DELETE && wKey >= 0x20)
    #endif
    {
        InsertCharAtCursor(wKey);
        return TRUE;
    }

    switch(wKey)        // check for special keys
    {
    case PK_BACKSPACE:
        if (mCursor.x || mCursor.y)
        {
            RemoveCharAtCursor(TRUE);
        }
        break;

    case PK_DELETE:
        if (DataGet())
        {
            RemoveCharAtCursor(FALSE);
        }
        break;

    case PK_HOME:
        if (mCursor.x)
        {
            RemoveCursor();
            mCursor.x = 0;
            DrawCursor();
        }
        break;

    case PK_END:
        RemoveCursor();
        GetLineStart(mCursor.y, &mCursor.x);
        DrawCursor();
        break;

    case PK_LNUP:
        if (mCursor.y > miTopLine)
        {
            RemoveCursor();
            PegPoint NewPick = mCursorPos;
            NewPick.y -= miLineHeight / 2;
            GetCursorRowColFromClick(NewPick);
            DrawCursor();
        }
        else
        {
            if (mCursor.y)
            {
                if (miTopLine && mpVScroll)
                {
                    RemoveCursor();
                    mCursor.y--;
                    mCursor.x = 0;
                    AutoScrollUp();
                    DrawCursor();
                }
            }
        }
        break;

    case PK_LNDN:
        if (mCursor.y < miTotalLines - 1)
        {
            RemoveCursor();
            PegPoint NewPick = mCursorPos;
            NewPick.y += miLineHeight * 3 / 2;
            GetCursorRowColFromClick(NewPick);
            CheckAutoScrollDown();
            DrawCursor();
        }
        break;

    case PK_RIGHT:
        GetLineStart(mCursor.y, &iIndex);
        RemoveCursor();
        if (mCursor.x < iIndex)
        {
            mCursor.x++;
        }
        else
        {
            if (GetLineStart(mCursor.y + 1, NULL))
            {
                mCursor.y++;
                mCursor.x = 0;
                CheckAutoScrollDown();
            }
        }
        DrawCursor();
        break;

    case PK_LEFT:
        if (mCursor.x)
        {
            RemoveCursor();
            mCursor.x--;
            DrawCursor();
            break;
        }
        if (mCursor.y)
        {
            RemoveCursor();
            if (mCursor.y > miTopLine)
            {
                mCursor.y--;
                GetLineStart(mCursor.y, &mCursor.x);
            }
            else
            {
                if (miTopLine && mpVScroll)
                {   
                    mCursor.y--;
                    GetLineStart(mCursor.y, &mCursor.x);
                    AutoScrollUp();
                }
            }
            DrawCursor();
        }
        break;

    case PK_CR:
        InsertCharAtCursor(wKey);
        mCursor.x = 0;
        DrawCursor();
        return TRUE;

    default:
        return FALSE;
        
    }
    return TRUE;
}


/*--------------------------------------------------------------------------*/
void PegEditBox::InsertCharAtCursor(WORD iKey)
{
    WORD wIndex, wOldIndex, wNextIndex;
    WORD wLength;
    
    PEGCHAR *cGet = DataGet();
    PEGCHAR *cBuff;
    RemoveCursor();

    wIndex = GetLineIndex(mCursor.y);

    if (cGet)
    {
	    wLength = strlen(cGet);
		if(wLength >= mwMaxChars)
		{
			DrawCursor();
//            SetCursorRowColFromIndex(wIndex);
			return;
		}

        wOldIndex = wIndex;
        wNextIndex = GetLineIndex(mCursor.y + 1);

        wIndex += mCursor.x;
        cBuff = new PEGCHAR[wLength + 4];

        if (wIndex != wLength)
        {
            strncpy(cBuff, cGet, wIndex);
            cBuff[wIndex] = '\0';
            *(cBuff + wIndex) = (PEGCHAR) iKey;
            strcpy(cBuff + wIndex + 1, cGet + wIndex);
        }
        else
        {
            strcpy(cBuff, cGet);
            *(cBuff + wIndex) = (PEGCHAR) iKey;
            *(cBuff + wIndex + 1) = '\0';
        }

        PegTextThing::DataSet(cBuff);
        delete cBuff;
        UpdateLineStarts();
        wIndex++;

        if (muScrollMode & WSM_AUTOSCROLL)
        {
            if (CheckAutoScroll())
            {
                // if we added or removed the scroll bar, we
                // have to do a full update.
                UpdateLineStarts();
                SetCursorRowColFromIndex(wIndex);

                if (mpVScroll)
                {
                    CheckAutoScrollDown();
                    Invalidate();
                    Draw();
                }
                return;
            }
        }

        SetCursorRowColFromIndex(wIndex);

        if (mpVScroll)
        {
            if (CheckAutoScrollDown())
            {
                DrawOneLine(mCursor.y - 1);
                DrawCursor();
                return;
            }
        }

        if (wOldIndex == GetLineIndex(mCursor.y))
        {
            wIndex = GetLineIndex(mCursor.y + 1);
            if (wIndex == 0xffff || wIndex  == wNextIndex + 1)
            {
                DrawOneLine(mCursor.y);
                DrawCursor();
            }
            else
            {
                Invalidate();
                Draw();
            }
        }
        else
        {
            Invalidate();
            Draw();
        }
    }
    else
    {
		if(mwMaxChars == 0)
		{
//            SetCursorRowColFromIndex(wIndex);
			DrawCursor();
			return;
		}
        PEGCHAR cTemp[2];
        cTemp[0] = (PEGCHAR) iKey;
        cTemp[1] = '\0';
        PegTextThing::DataSet(cTemp);
        FillLineStarts(0, TRUE);
        wIndex = 1;
        SetCursorRowColFromIndex(wIndex);
        BeginDraw();
        DrawCharAtCursor(iKey);
        DrawCursor();
        EndDraw();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegEditBox::CheckAutoScrollDown(void)
{
    if (!mpVScroll)
    {
        return FALSE;
    }

    PegScrollInfo si;
    GetVScrollInfo(&si);

    if (miTopLine + si.wVisible <= mCursor.y)
    {
        si.wCurrent = mCursor.y - si.wVisible + 1;
        mpVScroll->Reset(&si);
        mpVScroll->Draw();
        PegMessage ScrollMessage(this, PM_VSCROLL);
        ScrollMessage.lData = si.wCurrent;
        Message(ScrollMessage);
        return TRUE;
    }
    return FALSE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegEditBox::AutoScrollUp(void)
{
    if (!mpVScroll)
    {
        return FALSE;
    }

    if (miTopLine && mCursor.y < miTopLine)
    {
        PegScrollInfo si;
        GetVScrollInfo(&si);
        si.wCurrent = mCursor.y;
        mpVScroll->Reset(&si);
        mpVScroll->Draw();
        PegMessage ScrollMessage(this, PM_VSCROLL);
        ScrollMessage.lData = si.wCurrent;
        Message(ScrollMessage);
        return TRUE;
    }
    return FALSE;
}


/*--------------------------------------------------------------------------*/
void PegEditBox::RemoveCharAtCursor(BOOL bBackSpace)
{
    WORD wIndex, wNextIndex;
    PEGCHAR *cGet = DataGet();
    PEGCHAR *cBuff;
    SIGNED iLen, iCury, iTopLine;

    if (!cGet)
    {
        return;
    }

    iLen = strlen(cGet);

    if (iLen == 1)
    {
        DataSet(NULL);
        Draw();
        return;
    }

    wIndex = GetLineIndex(mCursor.y);
    wIndex += mCursor.x;
    wNextIndex = GetLineIndex(mCursor.y + 1);
    iCury = mCursor.y;
    iTopLine = miTopLine;

    if (bBackSpace)
    {
        wIndex--;
    }

    cBuff = new PEGCHAR[iLen + 2];
    strncpy(cBuff, cGet, wIndex);
    cBuff[wIndex] = '\0';

    if (iLen > wIndex)
    {
        strcat(cBuff, cGet + wIndex + 1);
    }
    PegTextThing::DataSet(cBuff);
    delete cBuff;

    UpdateLineStarts();

    if (muScrollMode & WSM_AUTOSCROLL)
    {
        if(CheckAutoScroll())
        {
           UpdateLineStarts();
           SetCursorRowColFromIndex(wIndex);
           Invalidate();
           Draw();
           return;
        }
    }

    SetCursorRowColFromIndex(wIndex);

    if (iTopLine == miTopLine)
    {
        wIndex = GetLineIndex(mCursor.y + 1);
        if ((wIndex == 0xffff && wNextIndex == 0xffff) ||
            (wNextIndex == GetLineIndex(mCursor.y + 1) - 1))
        {
            if (iCury != mCursor.y)
            {
                DrawOneLine(iCury);
            }
            DrawOneLine(mCursor.y);
            DrawCursor();
            return;
        }
    }
    Invalidate();
    Draw();
}


/*--------------------------------------------------------------------------*/
void PegEditBox::GetMarkStartAndEnd(PegPoint *StartMark, PegPoint *EndMark)
{
    if (mMarkAnchor.y < mMarkEnd.y)
    {
        *StartMark = mMarkAnchor;
        *EndMark = mMarkEnd;
    }
    else
    {
        if (mMarkAnchor.y > mMarkEnd.y)
        {
            *StartMark = mMarkEnd;
            *EndMark = mMarkAnchor;
        }
        else
        {
            if (mMarkAnchor.x < mMarkEnd.x)
            {
                *StartMark = mMarkAnchor;
                *EndMark = mMarkEnd;
            }
            else
            {
                *StartMark = mMarkEnd;
                *EndMark = mMarkAnchor;
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
void PegEditBox::ReplaceMarkedText(WORD iKey)
{
    PegPoint StartMark, EndMark;

    GetMarkStartAndEnd(&StartMark, &EndMark);

    WORD wStart = GetLineIndex(StartMark.y);
    wStart += StartMark.x;

    WORD wEnd = GetLineIndex(EndMark.y);
    wEnd += EndMark.x;

    if (wStart || wEnd < (WORD) strlen(DataGet()))
    {
        PEGCHAR *pTemp = new PEGCHAR[strlen(DataGet()) + 2];
        strncpy(pTemp, DataGet(), wStart);
        pTemp[wStart] = (PEGCHAR) iKey;
        pTemp[wStart + 1] = '\0';
        strcat(pTemp, DataGet() + wEnd);
        DataSet(pTemp);
        delete pTemp;
    }
    else
    {
        PEGCHAR cTemp[2];
        cTemp[0] = (PEGCHAR) iKey;
        cTemp[1] = '\0';
        DataSet(cTemp);
    }

    wStart++;
    SetCursorRowColFromIndex(wStart);

    if (muScrollMode & WSM_AUTOSCROLL)
    {
        if (CheckAutoScroll())
        {
            UpdateLineStarts();
        }
    }
    Invalidate();
    Draw();
}


/*--------------------------------------------------------------------------*/
void PegEditBox::CopyToScratchPad(void)
{
    if (!State.mbMarked)
    {
        return;
    }
    PegPoint StartMark, EndMark;
    GetMarkStartAndEnd(&StartMark, &EndMark);

    WORD wStart = GetLineIndex(StartMark.y);
    wStart += StartMark.x;

    WORD wEnd = GetLineIndex(EndMark.y);
    wEnd += EndMark.x;

    if (wEnd > wStart)
    {
        PEGCHAR *pTemp = new PEGCHAR[wEnd - wStart + 1];
        strncpy(pTemp, DataGet() + wStart, wEnd - wStart);
        pTemp[wEnd - wStart] = '\0';
        Presentation()->SetScratchPad(pTemp);
        delete pTemp;
    }
}

/*--------------------------------------------------------------------------*/
void PegEditBox::PasteFromScratchPad(void)
{
    if (!(Presentation()->GetScratchPad()))
    {
        return;
    }

	WORD wLength;
	PEGCHAR *pScratchPad = Presentation()->GetScratchPad();
	int iSPLen = strlen(pScratchPad);

    if (!DataGet())		// Empty EditBox
    {
		wLength = iSPLen + 2;
		if(wLength > mwMaxChars)
		{
			PEGCHAR *pTruncated = new PEGCHAR[mwMaxChars + 2];
			strncpy(pTruncated, pScratchPad, mwMaxChars);
			pTruncated[mwMaxChars] = '\0';
			DataSet(pTruncated);
			delete pTruncated;
		}
		else
		{
			DataSet(pScratchPad);
		}
        Draw();
        return;
    }

    RemoveCursor();

    PEGCHAR *pGet = DataGet();
	int wTruncLen;
	int iGetLen = strlen(pGet);
	wLength = iGetLen + iSPLen;
	if(wLength > mwMaxChars)
	{
		wTruncLen = iSPLen - (wLength - mwMaxChars);
		wLength = mwMaxChars;
	}
	else
	{
		wTruncLen = iSPLen;
	}
	if(wTruncLen <= 0)
	{
		Draw();
		return;
	}

    PEGCHAR *pTemp = new PEGCHAR[wLength + 2];

    wLength = GetLineIndex(mCursor.y);
    wLength += mCursor.x;
    strncpy(pTemp, pGet, wLength);
    pTemp[wLength] = '\0';
    strncat(pTemp, pScratchPad, wTruncLen);
	pTemp[wLength + wTruncLen] = '\0';
    WORD wNewIndex = strlen(pTemp);
	pTemp[wLength + wTruncLen] = '\0';
    strcat(pTemp, pGet + wLength);
	pTemp[iGetLen + wTruncLen] = '\0';
    DataSet(pTemp);
    delete pTemp;
    SetCursorRowColFromIndex(wNewIndex);
    Draw();
    CheckAutoScrollDown();
}


/*--------------------------------------------------------------------------*/
void PegEditBox::DeleteMarkedText(void)
{
    if (!State.mbMarked)
    {
        return;
    }
    PegPoint StartMark, EndMark;
    GetMarkStartAndEnd(&StartMark, &EndMark);

    WORD wStart = GetLineIndex(StartMark.y);
    wStart += StartMark.x;

    WORD wEnd = GetLineIndex(EndMark.y);
    wEnd += EndMark.x;

    if (wStart > 0 || wEnd < (WORD) strlen(DataGet()))
    {
        PEGCHAR *pTemp = new PEGCHAR[strlen(DataGet()) + 2];
        strncpy(pTemp, DataGet(), wStart);
        pTemp[wStart] = '\0';
        strcat(pTemp, DataGet() + wEnd);
        DataSet(pTemp);
        delete pTemp;
        mCursor = StartMark;
    }
    else
    {
        DataSet(NULL);
        mCursor.x = mCursor.y = 0;
    }

    if (muScrollMode & WSM_AUTOSCROLL)
    {
        if (CheckAutoScroll())
        {
            UpdateLineStarts();
        }
    }
    State.mbEditMode = TRUE;
    State.mbMarked = FALSE;
    mCursor = StartMark;

    if (mCursor.y >= miTotalLines)
    {
        while(mCursor.y >= miTotalLines)
        {
            mCursor.y--;
        }
        GetLineStart(mCursor.y, &mCursor.x);
    }
    Invalidate();
    Draw();
}


/*--------------------------------------------------------------------------*/
void PegEditBox::GetCursorRowColFromClick(PegPoint Where)
{
    // figure out which row we are on:

    SIGNED iTemp = mClient.wTop + 2 + miLineHeight;
    mCursor.y = miTopLine;

    while(iTemp < Where.y)
    {
        mCursor.y++;
        iTemp += miLineHeight;
    }

    if (mCursor.y >= miTotalLines)
    {
        if (miTotalLines)
        {
            mCursor.y = miTotalLines - 1;
            GetLineStart(mCursor.y, &mCursor.x);
        }
        else
        {
            mCursor.y = mCursor.x = 0;
        }
        return;
    }

    PEGCHAR cTemp[2];
    cTemp[1] = '\0';
    PEGCHAR *pGet = GetLineStart(mCursor.y, &iTemp);

    if (!pGet)
    {
        mCursor.y = mCursor.x = 0;
        return;
    }
    PEGCHAR *pStop = pGet + iTemp;
    mCursor.x = 0;

    SIGNED wTotWidth = 0;
    SIGNED wThisWidth = 0;
    SIGNED wPickWidth = Where.x - mClient.wLeft - 2;

    while(*pGet && pGet < pStop)
    {
        cTemp[0] = *pGet++;
        wThisWidth = TextWidth(cTemp, mpFont);
        if (wTotWidth + wThisWidth > wPickWidth)
        {
            break;
        }
        else
        {
            wTotWidth += wThisWidth;
            mCursor.x++;
        }
    }
    if (*pGet && pGet != pStop)
    {
        if (wPickWidth - wTotWidth > wTotWidth + wThisWidth - wPickWidth)
        {    
            mCursor.x++;
        }
    }
}

/*--------------------------------------------------------------------------*/
PEGCHAR *PegEditBox::GetCharAtCursor(void)
{
    PEGCHAR *pGet = GetLineStart(mCursor.y, NULL);
    if (pGet)
    {
        pGet += mCursor.x;
        return pGet;
    }
    return NULL;
}


/*--------------------------------------------------------------------------*/
void PegEditBox::HomeCursor(void)
{
    mCursor.x = 0;
}

/*--------------------------------------------------------------------------*/
void PegEditBox::GetCursorPointFromRowCol(void)
{
    SIGNED iLen;
    mCursorPos.x = mClient.wLeft + 2;

    if (mCursor.x)
    {
        PEGCHAR *pGet = GetLineStart(mCursor.y, &iLen);
        CheckBufLen(iLen + 3);

        if (pGet)
        {
            strncpy(mpBuf, pGet, mCursor.x);
            PEGCHAR *pGet = mpBuf;
            mpBuf[mCursor.x] = '\0';
            mCursorPos.x += TextWidth(pGet, mpFont);
        }
    }

    mCursorPos.y = mClient.wTop + 2;
    mCursorPos.y += (mCursor.y - miTopLine) * miLineHeight;
}

/*--------------------------------------------------------------------------*/
void PegEditBox::SetCursorRowColFromIndex(WORD wIndex)
{
    mCursor.y = 0;
    mCursor.x = 0;
    SIGNED iLoop;
    WORD wLineIndex;

    if (!DataGet())
    {
        return;
    }

    for (iLoop = 0; iLoop < miTotalLines; iLoop++)
    {
        wLineIndex = GetLineIndex(iLoop);
        if (wLineIndex == 0xffff || wLineIndex > wIndex)
        {
            break;
        }
    }

    if (iLoop)
    {
        mCursor.y = iLoop - 1;
    }
    mCursor.x = wIndex - GetLineIndex(mCursor.y);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegEditBox::Append(const PEGCHAR *pText, BOOL bDraw)
{
    RemoveCursor();
    mCursor.x = mCursor.y = 0;
    PegTextBox::Append(pText, FALSE);
    SetCursorRowColFromIndex(strlen(DataGet()));

    if (bDraw)
    {
        Draw();
    }
}

void PegEditBox::SetCursorRowCol(WORD index)
{
	SetCursorRowColFromIndex(index);
	SetTopLine(mCursor.y);
}
