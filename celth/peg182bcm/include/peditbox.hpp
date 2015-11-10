/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// peditbox.hpp - Editable multi-line text box class definition.
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

#ifndef _PEGEDITBOX_
#define _PEGEDITBOX_


class PegEditBox : public PegTextBox
{
    public:

        PegEditBox(const PegRect &Rect, WORD wId = 0,
                   WORD wStyle = FF_RECESSED|EF_EDIT|EF_WRAP,
                   PEGCHAR *Text = NULL, WORD wMaxChars = 1000);

        virtual void Draw(void);
        virtual SIGNED Message(const PegMessage &Mesg);

        virtual void DataSet(const PEGCHAR *pText);
        virtual void Append(const PEGCHAR *pText, BOOL bDraw = TRUE);

        void DeleteMarkedText(void);
        void CopyToScratchPad(void);
        void PasteFromScratchPad(void);
        void HomeCursor(void);
        void InsertCharAtCursor(WORD iKey);
        void SetCursorRowCol(WORD wIndex);
        BOOL InEditMode(void) {return State.mbEditMode;}

    protected:

        void DrawTextLine(SIGNED iLine, PegPoint PutPoint, BOOL bFill = FALSE);
        void DrawMarkedText(SIGNED iLine, PegPoint PutPoint);
        void DrawOneLine(SIGNED iLine);

        void SetCursorPoint(void);
        void DrawCursor(void);
        void RemoveCursor(void);
        void DrawCharAtCursor(SIGNED iKey);
        PEGCHAR *GetCharAtCursor(void);
        void GetMarkStartAndEnd(PegPoint *StartMark, PegPoint *EndMark);
        void GetCursorRowColFromClick(PegPoint Where);
        void GetCursorPointFromRowCol(void);
        void SetCursorRowColFromIndex(WORD wIndex);
        void RemoveCharAtCursor(BOOL bBackSpace);
        void DrawSelectText(SIGNED iLine, SIGNED iFirst, SIGNED iLast, BOOL bSel);
        void CheckMarkMove(void);
        BOOL CheckAutoScrollDown(void);
        BOOL AutoScrollUp(void);

       #ifdef PEG_KEYBOARD_SUPPORT
        BOOL CheckControlKey(WORD wKey, SIGNED iCtrl);
       #endif

        void ExitEditMode(void);
        BOOL InsertKey(WORD wKey);
        void ReplaceMarkedText(WORD wKey);

        PegPoint mCursor;       // row, col of cursor
        PegPoint mMarkAnchor;   // row, col char position of mark anchor
        PegPoint mMarkEnd;      // row, col char position of mark end
        PegPoint mCursorPos;    // top, left pixel position of cursor

        struct {
            SIGNED mbEditMode:1;
            SIGNED mbMarked:1;
            SIGNED mbMarkMode:1;
            SIGNED bPointerSet:1;
        } State;
};

#endif



