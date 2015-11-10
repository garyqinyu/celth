/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pstring.cpp - PegString class implmentation.
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

/*--------------------------------------------------------------------------*/
PegString::PegString(const PegRect &Rect, const PEGCHAR *Text, WORD wId,
    WORD wStyle, SIGNED iLen) :
    PegThing(Rect, wId, wStyle),
    PegTextThing(Text, wStyle & (TT_COPY|EF_EDIT), PEG_STRING_FONT)
{
    Type(TYPE_STRING);

   #if defined(PEG_KEYBOARD_SUPPORT)
    mpBackup = NULL;
   #endif

    if (!(wStyle & EF_EDIT))
    {
        RemoveStatus(PSF_SELECTABLE|PSF_ACCEPTS_FOCUS);
        SetSignals(0);
    }
    else
    {
        AddStatus(PSF_TAB_STOP);
        SetSignals(SIGMASK(PSF_TEXT_EDITDONE));
    }

    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_STEXT] = PCLR_HIGH_TEXT;
    muColors[PCI_SELECTED] = PCLR_HIGH_TEXT_BACK;
    muColors[PCI_NORMAL] = PCLR_NORM_TEXT_BACK;

    miMaxLen = iLen;
    miFirstVisibleChar = 0;
    mwFullState = 0;
    miMarkStart = miMarkEnd = -1;
    miCursor = 0;

    InitClient();
    mClient.wLeft++;
    mClient.wRight--;
}


/*--------------------------------------------------------------------------*/
PegString::PegString(SIGNED iLeft, SIGNED iTop, const PEGCHAR *Text, WORD wId, 
    WORD wStyle, SIGNED iLen) :
    PegThing(wId, wStyle),
    PegTextThing(Text, wStyle & (TT_COPY|EF_EDIT), PEG_STRING_FONT)
{
    Type(TYPE_STRING);

   #if defined(PEG_KEYBOARD_SUPPORT)
    mpBackup = NULL;
   #endif

    mReal.wLeft = iLeft;
    mReal.wTop = iTop;
    if (Text)
    {
        mReal.wRight = mReal.wLeft + TextWidth(Text, mpFont);
        mReal.wRight += 10;
        mReal.wBottom = mReal.wTop + TextHeight(Text, mpFont);
        if (wStyle & (FF_RECESSED|FF_RAISED))
        {
            mReal.wBottom += 4;
        }
        else
        {
            if (wStyle & FF_THIN)
            {
                mReal.wBottom += 2;
            }
        }
    }

    if (!(wStyle & EF_EDIT))
    {
        RemoveStatus(PSF_SELECTABLE|PSF_ACCEPTS_FOCUS);
        SetSignals(0);
    }
    else
    {
        AddStatus(PSF_TAB_STOP);
        SetSignals(SIGMASK(PSF_TEXT_EDITDONE));
    }

    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_STEXT] = PCLR_HIGH_TEXT;
    muColors[PCI_SELECTED] = PCLR_HIGH_TEXT_BACK;
    muColors[PCI_NORMAL] = PCLR_NORM_TEXT_BACK;

    miMaxLen = iLen;
    miFirstVisibleChar = 0;
    mwFullState = 0;
    miMarkStart = miMarkEnd = -1;
    miCursor = 0;
  
    InitClient();
    mClient.wLeft++;
    mClient.wRight--;

}


/*--------------------------------------------------------------------------*/
PegString::PegString(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, 
    const PEGCHAR *Text, WORD wId, WORD wStyle, SIGNED iLen) : 
    PegThing(wId, wStyle),
    PegTextThing(Text, wStyle & (TT_COPY|EF_EDIT), PEG_STRING_FONT)
{
    Type(TYPE_STRING);

   #if defined(PEG_KEYBOARD_SUPPORT)
    mpBackup = NULL;
   #endif

    mReal.wLeft = iLeft;
    mReal.wTop = iTop;
    mReal.wRight = mReal.wLeft + iWidth - 1;
    mReal.wBottom = mReal.wTop + TextHeight(lsTEST, mpFont);

    if (wStyle & (FF_RECESSED|FF_RAISED))
    {
        mReal.wBottom += 4;
    }
    else
    {
        if (wStyle & FF_THIN)
        {
            mReal.wBottom += 2;
        }
    }

    if (!(wStyle & EF_EDIT))
    {
        RemoveStatus(PSF_SELECTABLE|PSF_ACCEPTS_FOCUS);
        SetSignals(0);
    }
    else
    {
        AddStatus(PSF_TAB_STOP);
        SetSignals(SIGMASK(PSF_TEXT_EDITDONE));
    }

    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_STEXT] = PCLR_HIGH_TEXT;
    muColors[PCI_SELECTED] = PCLR_HIGH_TEXT_BACK;
    muColors[PCI_NORMAL] = PCLR_NORM_TEXT_BACK;

    miMaxLen = iLen;
    miFirstVisibleChar = 0;
    mwFullState = 0;
    miMarkStart = miMarkEnd = -1;
    miCursor = 0;

    InitClient();
    mClient.wLeft++;
    mClient.wRight--;

}

/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegString::~PegString()
{
   #if defined(PEG_KEYBOARD_SUPPORT)

    if (mpBackup)
    {
        delete mpBackup;
    }

   #endif
}

/*--------------------------------------------------------------------------*/
void PegString::SetFont(PegFont *Font)
{
    PegTextThing::SetFont(Font);

    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate(mReal);
    }
}

/*--------------------------------------------------------------------------*/
void PegString::DataSet(const PEGCHAR *Text)
{
    if (miMaxLen >= 0)
    {
        if (Text)
        {
            if ((SIGNED) strlen(Text) > miMaxLen)    // strlen + NULL
            {
                PEGCHAR *cTemp = new PEGCHAR[miMaxLen + 1];
                strncpy(cTemp, Text, miMaxLen);
                cTemp[miMaxLen] = (PEGCHAR)NULL;
                PegTextThing::DataSet(cTemp);
                delete cTemp;
            }
            else
            {
                PegTextThing::DataSet(Text);
            }
        }
        else
        {
            PegTextThing::DataSet(Text);
        }
    }
    else
    {
        PegTextThing::DataSet(Text);
    }

    if (State.mbFullSelect && Text)
    {
        miMarkStart = 0;
        miMarkEnd = strlen(DataGet());
        State.mbMarked = 1;
        miCursor = 0;
        State.mbEditMode = 0;
    }
    else
    {
        State.mbMarked = 0;
        State.mbFullSelect = 0;
    }

    State.mbChanged = 0;

    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate(mReal);
    }
}

/*--------------------------------------------------------------------------*/
void PegString::Style(WORD wStyle)
{
    PegThing::Style(wStyle);
    if (wStyle & EF_EDIT)
    {
        AddStatus(PSF_SELECTABLE);
    }
    else
    {
        RemoveStatus(PSF_SELECTABLE);
    }
}



/*--------------------------------------------------------------------------*/
SIGNED PegString::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_HIDE:
        PegThing::Message(Mesg);
        mwFullState = 0;
        break;

   #if defined(PEG_KEYBOARD_SUPPORT)
    case PM_LOST_KEYBOARD:
   #else
    case PM_NONCURRENT:
   #endif
        PegThing::Message(Mesg);
        if (State.mbChanged)
        {
            CheckSendSignal(PSF_TEXT_EDITDONE);
        }
        mwFullState = 0;

        Invalidate(mReal);
        Draw();

       #ifdef PEG_KEYBOARD_SUPPORT
        if (mpBackup)
        {
            delete mpBackup;
            mpBackup = NULL;
        }
       #endif

        break;

   #if defined(PEG_KEYBOARD_SUPPORT)
    case PM_GAINED_KEYBOARD:
   #else
    case PM_CURRENT:
   #endif
        PegThing::Message(Mesg);
        if (DataGet())
        {
            miMarkStart = 0;
            miMarkEnd = strlen(DataGet());

           #ifdef PEG_KEYBOARD_SUPPORT
            if (mpBackup)
            {
                delete mpBackup;
            }
            mpBackup = new PEGCHAR[miMarkEnd + 1];
            strcpy(mpBackup, DataGet());
           #endif

            State.mbMarked = 1;
            State.mbFullSelect = 1;
            miCursor = 0;
            State.mbEditMode = 0;
            State.mbChanged = 0;
        }
        else
        {
            miCursor = 0;
            State.mbMarked = 0;
            State.mbFullSelect = 0;
            State.mbEditMode = 1;
            State.mbChanged = 0;
            mCursorPos.x = mClient.wLeft + 1;
            mCursorPos.y = mClient.wTop;
        }
        Invalidate(mClient);
        Draw();
        break;

   #ifdef PEG_MOUSE_SUPPORT

    case PM_LBUTTONDOWN:
        // figure out where in the string they clicked, set miCursor:
        if (!State.mbFullSelect)
        {
            SetCursorPos(Mesg.Point);

            miMarkAnchor = miMarkStart = miMarkEnd = miCursor;

            if (DataGet())
            {
                State.mbEditMode = 0;
                State.mbMarked = 0;
                State.mbMarkMode = 1;
            }
            else
            {
                State.mbEditMode = 1;
                State.mbMarked = 0;
                State.mbMarkMode = 0;
            }
            Invalidate(mClient);
            Draw();
        }
        break;

    case PM_POINTER_MOVE:
        if (State.mbMarkMode)
        {
            SIGNED iTemp = miCursor;
            SetCursorPos(Mesg.Point);

            if (iTemp == miCursor)
            {
                break;
            }

            if (miCursor < miMarkAnchor)
            {
                if (miMarkEnd > miMarkAnchor)
                {
                    miMarkEnd = miMarkAnchor;
                }
                miMarkStart = miCursor;
                State.mbEditMode = 0;
                State.mbMarked = 1;
                DrawMarked();
            }
            else
            {
                if (miCursor > miMarkAnchor)
                {
                    if (miMarkStart < miMarkAnchor)
                    {
                        miMarkStart = miMarkAnchor;
                    }

                    miMarkEnd = miCursor;
                    State.mbEditMode = 0;
                    State.mbMarked = 1;
                    DrawMarked();
                }
            }
        }
        break;

    case PM_LBUTTONUP:
        if (!State.mbFullSelect)
        {
            if (State.mbMarkMode)
            {
                State.mbMarkMode = 0;
                if (miMarkEnd == miMarkStart)
                {
                    State.mbEditMode = 1;
                    State.mbMarked = 0;
                    Invalidate(mClient);
                    Draw();
                }
                else
                {    
                    State.mbEditMode = 0;
                    State.mbMarked = 1;
                    CheckSendSignal(PSF_TEXT_SELECT);
                }
            }
        }
        else
        {
            State.mbFullSelect = 0;
        }
        break;

    case PM_POINTER_ENTER:
        if (StatusIs(PSF_ACCEPTS_FOCUS))
        {
            SetPointerType(PPT_IBEAM);
        }
        break;

    case PM_POINTER_EXIT:
        if (StatusIs(PSF_ACCEPTS_FOCUS))
        {
            SetPointerType(PPT_NORMAL);
        }
        break;

   #endif

    case PM_CUT:
        CopyToScratchPad();
        DeleteMarkedText();
        State.mbChanged = 1;
        Draw();
        break;

    case PM_COPY:
        CopyToScratchPad();
        break;

    case PM_PASTE:
        PasteFromScratchPad();
        State.mbChanged = 1;
        break;

   #ifdef PEG_KEYBOARD_SUPPORT

    case PM_KEY:
        if (Mesg.lData & (KF_CTRL|KF_SHIFT))
        {
            if (CheckControlKey(Mesg.iData, (SIGNED) Mesg.lData))
            {
                return 0;
            }
        }

        if (Style() & EF_EDIT)
        {
            if (InsertKey(Mesg.iData))
            {
                Draw();
                CheckSendSignal(PSF_TEXT_EDIT);
            }
            else
            {
                if (Mesg.iData == PK_CR)
                {
                    if (State.mbEditMode || State.mbFullSelect)
                    {
                        CheckSendSignal(PSF_TEXT_EDITDONE);
                        State.mbChanged = 0;

                        if (DataGet())
                        {
                            State.mbMarked = 1;
                            State.mbFullSelect = 1;
                            miMarkStart = 0;
                            miMarkEnd = strlen(DataGet());
                            State.mbEditMode = 0;
                            Invalidate();
                            Draw();
                        }
                    }
                }
                else
                {
                    if (Mesg.iData == PK_ESC && mpBackup)
                    {
                        DataSet(mpBackup);
                        miMarkStart = 0;
                        miMarkEnd = strlen(DataGet());
                        State.mbMarked = 1;
                        State.mbFullSelect = 1;
                        State.mbEditMode = 0;
                        Draw();
                    }
                    else
                    {
                        PegThing::Message(Mesg);
                    }
                }
            }
        }
        break;
   #endif

    default:
        PegThing::Message(Mesg);
        break;
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void PegString::Draw(void)
{
    if (!Parent())
    {
        return;
    }
    PegColor Color(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);

    BeginDraw();
    StandardBorder(Color.uBackground);
    PegPoint Point;
    Point.y = mClient.wTop;
    Point.x = mClient.wLeft + 1;

    // Draw the Text:

    PegRect OldClip = mClip;
    mClip &= mClient;

    if (mpText)
    {
        if (State.mbMarked)
        {
            DrawMarked();
        }
        else
        {
            DrawText(Point, mpText + miFirstVisibleChar,
                Color, mpFont);
        }
    }

    if (State.mbEditMode)
    {
       #ifdef PEG_RUNTIME_COLOR_CHECK
        if (NumColors() < 4)
        {
            Color.uForeground = BLACK;
        }
        else
        {
            Color.uForeground = PCLR_CURSOR;
        }
       #else
        Color.uForeground = PCLR_CURSOR;
       #endif
        Line(mCursorPos.x, mClient.wTop, 
            mCursorPos.x, mClient.wBottom, Color);
    }
    mClip = OldClip;

    if (First())
    {
        DrawChildren();
    }
    EndDraw();
}


/*--------------------------------------------------------------------------*/
void PegString::DrawMarked(void)
{
    Invalidate();
    BeginDraw();
    PegColor Color(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
    
    PegPoint Point;
    Point.y = mClient.wTop;
    Point.x = mClient.wLeft + 1;

    PegRect OldClip = mClip;
    mClip &= mClient;

    WORD wLength = strlen(mpText);
    PEGCHAR *pTemp = new PEGCHAR[wLength + 1];

    if (miMarkEnd > wLength)
    {
        miMarkEnd = wLength;
    }

    // draw the unmarked characters, if any:
    
    if (miMarkStart > miFirstVisibleChar)
    {
        strcpy(pTemp, mpText);
        *(pTemp + miMarkStart) = '\0';
        DrawText(Point, pTemp + miFirstVisibleChar, Color, mpFont);
        Point.x += TextWidth(pTemp + miFirstVisibleChar, mpFont);
    }
    
    // draw the visble marked characters:

    if (miMarkEnd > miFirstVisibleChar)
    {
         Color.uForeground = muColors[PCI_STEXT];
         Color.uBackground = muColors[PCI_SELECTED];

         if (miMarkStart >= miFirstVisibleChar)
         {
             strcpy(pTemp, mpText + miMarkStart);
             pTemp[miMarkEnd - miMarkStart] = '\0';
         }
         else
         {
             strcpy(pTemp, mpText + miFirstVisibleChar);
             pTemp[miMarkEnd - miFirstVisibleChar] = '\0';
         }
         DrawText(Point, pTemp, Color, mpFont);
         Point.x += TextWidth(pTemp, mpFont);
    }
    
    // draw any characters after the mark:
    
    if (miMarkEnd < wLength)
    {
        strcpy(pTemp, mpText + miMarkEnd);
        Color.uForeground = muColors[PCI_NTEXT];
        Color.uBackground = muColors[PCI_NORMAL];
        DrawText(Point, pTemp, Color, mpFont);
    }
    delete pTemp;
    mClip = OldClip;
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegString::SetCursorPos(PegPoint PickPoint)
{
    WORD wThisWidth;
    WORD wTotWidth = 1;
    WORD wPickWidth = PickPoint.x - mClient.wLeft;
    PEGCHAR *pText = DataGet();

    if (!pText)
    {
        miCursor = 0;
        mCursorPos.x = mClient.wLeft + 1;
        mCursorPos.y = mClient.wTop;
        return;
    }
    PEGCHAR cTemp[2];
    miCursor = miFirstVisibleChar;

    cTemp[1] = '\0';

    while(1)
    {
        cTemp[0] = *(pText + miCursor);

        if (!cTemp[0])
        {
            wThisWidth = 0;
            break;
        }
        wThisWidth = TextWidth(cTemp, mpFont);
        if (wTotWidth + wThisWidth > wPickWidth)
        {
            break;
        }
        else
        {
            wTotWidth += wThisWidth;
            miCursor++;
        }
    }

    if (wThisWidth)
    {
        if (wPickWidth - wTotWidth > wTotWidth + wThisWidth - wPickWidth)
        {    
            wTotWidth += wThisWidth;
            miCursor++;
        }
    }
    mCursorPos.x = mClient.wLeft + wTotWidth;
    mCursorPos.y = mClient.wTop;
}

/*--------------------------------------------------------------------------*/
BOOL PegString::InsertKey(SIGNED iKey)
{
    PEGCHAR *cBuff;
    PEGCHAR *cGet;
    cGet = DataGet();

    #ifdef PEG_UNICODE
    PEGCHAR cTemp[2];
    cTemp[0] = iKey;
    cTemp[1] = 0;
    #endif

    if (State.mbMarked)
    {
        #ifdef PEG_UNICODE
        if (iKey >= 0x20 && TextWidth(cTemp, mpFont) > 0)
        #else
        if (iKey < PK_DELETE && iKey >= 0x20)
        #endif
        {
            ReplaceMarkedText(iKey);
            State.mbFullSelect = 0;
            State.mbChanged = 1;
            return TRUE;
        }
        else
        {
            switch(iKey)
            {
            case PK_RIGHT:
            case PK_END:
                iKey = PK_END;
                State.mbEditMode = 1;
                State.mbMarked = 0;
                State.mbFullSelect = 0;
                break;

            case PK_LEFT:
            case PK_HOME:
                miCursor = 1;
                iKey = PK_HOME;
                State.mbEditMode = 1;
                State.mbMarked = 0;
                State.mbFullSelect = 0;
                break;

            case PK_BACKSPACE:
            case PK_DELETE:
                DeleteMarkedText();
                State.mbFullSelect = 0;
                return TRUE;

            default:
                return (FALSE);
            }
        }
    }

    #ifdef PEG_UNICODE
    if (iKey >= 0x20 && TextWidth(cTemp, mpFont) > 0)
    #else
    if (iKey < PK_DELETE && iKey >= 0x20)
    #endif
    {
        if (miMaxLen > 0)
        {
            if (cGet)
            {
                if ((SIGNED) strlen(cGet) >= miMaxLen)
                {
                    return FALSE;
                }
            }
        }

        if (cGet)
        {
            cBuff = new PEGCHAR[strlen(cGet) + 2];
            strcpy(cBuff, cGet);
            *(cBuff + miCursor) = (PEGCHAR) iKey;
            strcpy(cBuff + miCursor + 1, cGet + miCursor);
            DataSet(cBuff);
            delete cBuff;
        }
        else
        {
            PEGCHAR cTemp[2];
            cTemp[0] = (PEGCHAR) iKey;
            cTemp[1] = '\0';
            DataSet(cTemp);
        }
        miCursor++;
        AdvanceCursor(iKey);
        State.mbChanged = 1;
        return TRUE;
    }
    else
    {
        switch(iKey)
        {
        case PK_BACKSPACE:
            if (miCursor && cGet)
            {
                cBuff = new PEGCHAR[strlen(cGet) + 2];
                strcpy(cBuff, cGet);
                RetardCursor(*(cBuff + miCursor - 1));
                strcpy(cBuff + miCursor - 1, cGet + miCursor);
                miCursor--;
                DataSet(cBuff);
                delete cBuff;
            }
            break;

        case PK_DELETE:
            if (cGet)
            {
                if (miCursor < (SIGNED) strlen(cGet))
                {
                    strcpy(cGet + miCursor, cGet + miCursor + 1);
                    Invalidate();
                }
            }
            break;

        case PK_HOME:
            if (miCursor)
            {
                miCursor = 0;
                miFirstVisibleChar = 0;
                mCursorPos.x = mClient.wLeft;
                Invalidate();
            }
            break;

        case PK_END:
            if (cGet)
            {
                miCursor = strlen(cGet);
                mCursorPos.x = mClient.wLeft + TextWidth(cGet, mpFont);
                miFirstVisibleChar = 0;

                while (mCursorPos.x >= mClient.wRight)
                {
                    RetardCursor(*(cGet + miFirstVisibleChar));
                    miFirstVisibleChar++;
                }
                Invalidate();
            }
            break;

        case PK_RIGHT:
            if (cGet)
            {
                if (miCursor < (SIGNED) strlen(cGet))
                {
                    AdvanceCursor(*(cGet + miCursor));
                    miCursor++;
                    Invalidate();
                }
            }
            break;

        case PK_LEFT:
            if (miCursor && cGet)
            {
                RetardCursor(*(cGet + miCursor - 1));
                miCursor--;
                Invalidate();
            }
            break;

        //case PK_TAB:
        //case PK_CR:
        default:
            return FALSE;
        }
    }
    State.mbChanged = 1;
    return (TRUE);
}


/*--------------------------------------------------------------------------*/
void PegString::SetMark(SIGNED iStart, SIGNED iEnd)
{
    PEGCHAR *pText = DataGet();
    if (!pText)
    {
        return;
    }

    SIGNED iLen = strlen(pText);
    State.mbEditMode = 0;
    State.mbFullSelect = 0;

    if (iStart >= 0 && iStart < iLen &&
        iEnd > iStart && iEnd <= iLen)
    {
        miMarkStart = iStart;
        miMarkEnd = iEnd;
        State.mbMarked = 1;
        DrawMarked();
    }
    else
    {
        State.mbMarked = 0;
        Invalidate();
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
void PegString::SetMark(PEGCHAR *MarkStart, PEGCHAR *MarkEnd)
{
    PEGCHAR *pText = DataGet();
    if (!pText)
    {
        return;
    }

    SIGNED iLen = strlen(pText);
    PEGCHAR *pEnd = pText + iLen - 1;

    if (MarkStart >= pText && MarkStart < pEnd &&
        MarkEnd > MarkStart && MarkEnd <= pEnd)
    {
        miMarkStart = MarkStart - pText;
        miMarkEnd = MarkEnd - pText;
        State.mbMarked = 1;
        State.mbEditMode = 0;
        State.mbFullSelect = 0;
        DrawMarked();
    }
}


#ifdef PEG_KEYBOARD_SUPPORT

/*--------------------------------------------------------------------------*/
BOOL PegString::CheckControlKey(SIGNED iKey, SIGNED iFlags)
{
    PEGCHAR *cGet = DataGet();

    if (iFlags & KF_SHIFT)
    {
	    switch(iKey)
	    {
	    case PK_RIGHT:
	        if (cGet)
	        {
	            if (miCursor < (SIGNED) strlen(cGet))
	            {
	                if (!State.mbMarked)
	                {
	                    miMarkStart = miCursor;
	                    State.mbMarked = 1;
	                }
                    SIGNED iTemp = miFirstVisibleChar;
	                AdvanceCursor(*(cGet + miCursor));
	                miCursor++;
	                miMarkEnd = miCursor;

                    if (iTemp == miFirstVisibleChar)
                    {
	                    DrawMarked();
                    }
                    else
                    {
                        Invalidate();
                        Draw();
                    }
                    CheckSendSignal(PSF_TEXT_SELECT);
	            }
	        }
	        return TRUE;
	
	    case PK_LEFT:
	        if (miCursor && cGet)
	        {
	            if (!State.mbMarked)
	            {
	                miMarkEnd = miCursor;
	                State.mbMarked = 1;
	            }
	            RetardCursor(*(cGet + miCursor - 1));
	            miCursor--;
	            miMarkStart = miCursor;
	            DrawMarked();
                CheckSendSignal(PSF_TEXT_SELECT);
	        }
	        return TRUE;
        }
    }
    return FALSE;
}
#endif


/*--------------------------------------------------------------------------*/
void PegString::CopyToScratchPad(void)
{
    if (!(State.mbMarked))
    {
        return;
    }
    PEGCHAR *cGet = DataGet();
    if (!cGet)
    {
        return;
    }

    if (miMarkStart > 0 || miMarkEnd < (SIGNED) strlen(cGet))
    {
        PEGCHAR *cTemp = new PEGCHAR[strlen(cGet) + 1];
        strcpy(cTemp, cGet + miMarkStart);
        cTemp[miMarkEnd - miMarkStart] = '\0';
        Presentation()->SetScratchPad(cTemp);
        delete cTemp;
    }
    else
    {
        Presentation()->SetScratchPad(cGet);
    }
}

/*--------------------------------------------------------------------------*/
void PegString::PasteFromScratchPad(void)
{
    if (!Presentation()->GetScratchPad())
    {
        return;
    }
    PEGCHAR *cGet = DataGet();


    if (!cGet)
    {
        DataSet(Presentation()->GetScratchPad());
        miCursor = strlen(DataGet());
    }
    else
    {
        if (State.mbMarked || State.mbFullSelect)
        {
            DeleteMarkedText();
            cGet = DataGet();
        }
    
        if (cGet)
        {
            WORD wLength = strlen(cGet) + strlen(Presentation()->GetScratchPad()) + 2;
            PEGCHAR *pTemp = new PEGCHAR[wLength];

            strncpy(pTemp, cGet, miCursor);
            pTemp[miCursor] = '\0';
            strcat(pTemp, Presentation()->GetScratchPad());

            if (miCursor < (SIGNED) strlen(cGet))
            {
                strcat(pTemp, cGet + miCursor);
            }
            DataSet(pTemp);
            delete(pTemp);
        }
        else
        {
            DataSet(Presentation()->GetScratchPad());
        }
    }
    State.mbMarked = 0;
    State.mbEditMode = 1;
    Draw();
}

/*--------------------------------------------------------------------------*/
void PegString::DeleteMarkedText(void)
{
    if (!State.mbMarked)
    {
        return;
    }

    PEGCHAR *cGet = DataGet();
    mCursorPos.y = mClient.wTop;
    State.mbMarked = 0;
    State.mbEditMode = 1;

    if (miMarkStart > 0 || miMarkEnd < (SIGNED) strlen(cGet))
    {
        PEGCHAR *cTemp = new PEGCHAR[strlen(cGet) + 1];
        strcpy(cTemp, cGet);
        cTemp[miMarkStart] = '\0';
        mCursorPos.x = mClient.wLeft + TextWidth(cTemp + miFirstVisibleChar, mpFont);
        miCursor = miMarkStart;
        strcat(cTemp, cGet + miMarkEnd);
        DataSet(cTemp);
        delete cTemp;
        
    }
    else
    {
        DataSet(NULL);
        miCursor = 0;
        miFirstVisibleChar = 0;
        mCursorPos.x = mClient.wLeft;
    }
}

/*--------------------------------------------------------------------------*/
void PegString::ReplaceMarkedText(SIGNED iKey)
{
    if (!State.mbMarked)
    {
        return;
    }

    PEGCHAR *cGet = DataGet();
    mCursorPos.y = mClient.wTop;

    if (miMarkEnd <  (SIGNED) strlen(cGet) || miMarkStart > 0 )
    {
        PEGCHAR *cTemp = new PEGCHAR[strlen(cGet) + 2];

        strcpy(cTemp, cGet);
        cTemp[miMarkStart] = (PEGCHAR) iKey;
        cTemp[miMarkStart + 1] = '\0';
        mCursorPos.x = mClient.wLeft + TextWidth(cTemp + miFirstVisibleChar, mpFont);
        miCursor = miMarkStart + 1;
        strcat(cTemp, cGet + miMarkEnd);
        DataSet(cTemp);
        delete cTemp;
    }
    else
    {
        PEGCHAR cTemp[2];
        cTemp[0] = (PEGCHAR) iKey;
        cTemp[1] = '\0';
        DataSet(cTemp);
        miCursor = 1;
        miFirstVisibleChar = 0;
        mCursorPos.x = mClient.wLeft + TextWidth(cTemp, mpFont);
    }
    State.mbMarked = 0;
    State.mbEditMode = 1;
}

/*--------------------------------------------------------------------------*/
void PegString::AdvanceCursor(SIGNED iNew)
{
    PEGCHAR cTemp[2];
    cTemp[1] = '\0';
    cTemp[0] = (PEGCHAR) iNew;
    mCursorPos.x += TextWidth(cTemp, mpFont);

    while (mCursorPos.x >= mClient.wRight)
    {
        RetardCursor(*(DataGet() + miFirstVisibleChar));
        miFirstVisibleChar++;
    }
}

/*--------------------------------------------------------------------------*/
void PegString::RetardCursor(SIGNED iNew)
{
    PEGCHAR cTemp[2];
    cTemp[1] = '\0';
    cTemp[0] = (PEGCHAR) iNew;
    mCursorPos.x -= TextWidth(cTemp, mpFont);

    while (mCursorPos.x < mClient.wLeft)
    {
        if (!miFirstVisibleChar)
        {
            break;
        }
        miFirstVisibleChar--;
        cTemp[0] = *(DataGet() + miFirstVisibleChar);
        mCursorPos.x += TextWidth(cTemp, mpFont);
    }
}


// End of file
