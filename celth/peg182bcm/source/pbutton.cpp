/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pbutton.cpp - All PegButton classes are implemented in this file.
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
//    The following classes are implemented in this file:
//
//    PegButton
//    PegBitmapButton
//    PegTextButton
//    PegCheckBox
//    PegRadioButton
//
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#if (ONE_SECOND >= 4)

#define BUTTON_REPEAT_RATE (ONE_SECOND / 4)

#else

#define BUTTON_REPEAT_RATE 1

#endif

extern PegBitmap gbRadioOnBitmap;
extern PegBitmap gbRadioOffBitmap;
extern PegBitmap gbCheckBoxOnBitmap;
extern PegBitmap gbCheckBoxOffBitmap;

#if defined(PEG_RUNTIME_COLOR_CHECK)
extern PegBitmap gbRadioOnDisabledBitmap;
extern PegBitmap gbRadioOffDisabledBitmap;
extern PegBitmap gbCheckBoxOnDisabledBitmap;
extern PegBitmap gbCheckBoxOffDisabledBitmap;
#elif (PEG_NUM_COLORS > 2)
extern PegBitmap gbRadioOnDisabledBitmap;
extern PegBitmap gbRadioOffDisabledBitmap;
extern PegBitmap gbCheckBoxOnDisabledBitmap;
extern PegBitmap gbCheckBoxOffDisabledBitmap;
#endif


extern PEGCHAR lsTEST[];

/*--------------------------------------------------------------------------*/
// PegButton base class definition:
/*--------------------------------------------------------------------------*/
PegButton::PegButton(const PegRect &Rect, WORD wId, WORD wStyle) :
    PegThing(Rect, wId, wStyle)
{
    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_SELECTED] = PCLR_BUTTON_FACE;
    muColors[PCI_STEXT] = PCLR_NORMAL_TEXT;

    Type(TYPE_BUTTON);
    AddStatus(PSF_MOVEABLE|PSF_SIZEABLE);
    SetSignals(SIGMASK(PSF_CLICKED));

    if (wStyle & AF_ENABLED)
    {
        AddStatus(PSF_SELECTABLE|PSF_TAB_STOP);
    }
    else
    {
        RemoveStatus(PSF_SELECTABLE|PSF_TAB_STOP);
    }
}

/*--------------------------------------------------------------------------*/
PegButton::PegButton(WORD wId, WORD wStyle) : PegThing(wId, wStyle)
{
    muColors[PCI_NORMAL] = PCLR_BUTTON_FACE;
    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_SELECTED] = PCLR_BUTTON_FACE;
    muColors[PCI_STEXT] = PCLR_NORMAL_TEXT;

    Type(TYPE_BUTTON);
    AddStatus(PSF_MOVEABLE|PSF_SIZEABLE);
    SetSignals(SIGMASK(PSF_CLICKED));

    if (wStyle & AF_ENABLED)
    {
        AddStatus(PSF_SELECTABLE|PSF_TAB_STOP);
    }
    else
    {
        RemoveStatus(PSF_SELECTABLE|PSF_TAB_STOP);
    }
}


/*--------------------------------------------------------------------------*/
SIGNED PegButton::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_LBUTTONDOWN:
        if (!(mwStyle & BF_SELECTED))
        {
            mwStyle |= BF_SELECTED;
            Invalidate(mClip);
            Draw();

            if (mwStyle & BF_DOWNACTION)
            {
                CheckSendSignal(PSF_CLICKED);
            }
            else
            {
                CapturePointer();
            }

            if (mwStyle & BF_REPEAT)
            {
                SetTimer(1, BUTTON_REPEAT_RATE, BUTTON_REPEAT_RATE);
            }
        }
        break;

    case PM_LBUTTONUP:
        if (StatusIs(PSF_OWNS_POINTER))
        {
            ReleasePointer();
            if (mwStyle & BF_REPEAT)
            {
                KillTimer(1);
            }
        }

        if (mwStyle & BF_SELECTED)
        {
            Invalidate(mClip);
            mwStyle &= ~BF_SELECTED;
            Draw();

            if (!(mwStyle & BF_DOWNACTION))
            {
                CheckSendSignal(PSF_CLICKED);
            }
        }
        break;

    case PM_POINTER_MOVE:
        if (mwStyle & BF_SELECTED)
        {
            if (!mReal.Contains(Mesg.Point))
            {
                mwStyle &= ~BF_SELECTED;
                Invalidate(mClip);
                Draw();
            }
        }
        else
        {
            if (StatusIs(PSF_OWNS_POINTER))
            {   
                if (!(mwStyle & BF_SELECTED) &&
                    mReal.Contains(Mesg.Point))
                {
                    mwStyle |= BF_SELECTED;
                    Invalidate(mClip);
                    Draw();
                }
            }
        }
        break;

    case PM_POINTER_ENTER:
        break;

    case PM_POINTER_EXIT:
        if (mwStyle & BF_SELECTED)
        {
            mwStyle &= ~BF_SELECTED;
            Invalidate(mClip);
            Draw();
        }
        if (mwStyle & BF_REPEAT)
        {
            KillTimer(1);
        }
        break;

    case PM_HIDE:
        if (mwStyle & BF_REPEAT)
        {
            KillTimer(1);
        }
        PegThing::Message(Mesg);
        break;

    case PM_TIMER:
        if (mwStyle & BF_REPEAT &&
            mwStyle & BF_SELECTED)
        {
            CheckSendSignal(PSF_CLICKED);
        }
        break;

    case PM_CURRENT:
        PegThing::Message(Mesg);

       #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
        if (muColors[PCI_NORMAL] != muColors[PCI_SELECTED] ||
            muColors[PCI_NTEXT] != muColors[PCI_STEXT])
        {
            Invalidate();
            Draw();
        }
       #endif

       #if !defined(PEG_KEYBOARD_SUPPORT) && defined(PEG_DRAW_FOCUS)
        DrawFocusIndicator(TRUE);
       #endif

        break;

    case PM_NONCURRENT:
        PegThing::Message(Mesg);

       #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
        if (muColors[PCI_NORMAL] != muColors[PCI_SELECTED] ||
            muColors[PCI_NTEXT] != muColors[PCI_STEXT])
        {
            Invalidate();
            Draw();
        }
       #endif

       #if !defined(PEG_KEYBOARD_SUPPORT) && defined(PEG_DRAW_FOCUS)
        EraseFocusIndicator();
       #endif
        break;


   #ifdef PEG_KEYBOARD_SUPPORT

   #if defined(PEG_DRAW_FOCUS)     
    case PM_GAINED_KEYBOARD:
        DrawFocusIndicator(TRUE);
        break;

    case PM_LOST_KEYBOARD:
        EraseFocusIndicator();
        break;
    #endif

    case PM_KEY:
        if (Mesg.iData == PK_CR ||
            Mesg.iData == PK_SPACE)
        {
            if (!(mwStyle & BF_SELECTED))
            {
                if (mwStyle & BF_DOWNACTION)
                {
                    CheckSendSignal(PSF_CLICKED);
                }
                else
                {
                    mwStyle |= BF_SELECTED;
                    Invalidate(mClip);
                    Draw();
                }
            }
        }
        else
        {
            return PegThing::Message(Mesg);
        }
        break;

    case PM_KEY_RELEASE:
        if (Mesg.iData == PK_CR ||
            Mesg.iData == PK_SPACE)
        {
            if (StatusIs(PSF_OWNS_POINTER))
            {
                ReleasePointer();
                if (mwStyle & BF_REPEAT)
                {
                    KillTimer(1);
                }
            }
            if (mwStyle & BF_SELECTED)
            {
                Invalidate(mClip);
                mwStyle &= ~BF_SELECTED;
                Draw();
                CheckSendSignal(PSF_CLICKED);
            }
        }
        else
        {
            return PegThing::Message(Mesg);
        }
        break;
   #endif

    default:
        return PegThing::Message(Mesg);
    }
    return(0);
}

#ifdef PEG_DRAW_FOCUS

/*--------------------------------------------------------------------------*/
void PegButton::DrawFocusIndicator(BOOL bDraw)
{
    PegColor RectColor(PCLR_FOCUS_INDICATOR, BLACK, CF_NONE);
    PegRect FocusRect = mClient;
    FocusRect--;

    if (bDraw)
    {
        Invalidate(FocusRect);
        BeginDraw();
        Rectangle(FocusRect, RectColor, 1);
        EndDraw();
    }
    else
    {
        Rectangle(FocusRect, RectColor, 1);
    }
}

/*--------------------------------------------------------------------------*/
void PegButton::EraseFocusIndicator(void)
{
    if (!StatusIs(PSF_VISIBLE))
    {
        return;
    }
    PegColor RectColor;

    if (Type() == TYPE_RADIOBUTTON || Type() == TYPE_CHECKBOX)
    {
        RectColor.Set(Parent()->muColors[PCI_NORMAL], BLACK, CF_NONE);
    }
    else
    {
        RectColor.Set(muColors[PCI_NORMAL], BLACK, CF_NONE);
    }

    if (Style() & AF_TRANSPARENT)
    {
        Invalidate();
        Parent()->Draw();
    }
    else
    {
        PegRect FocusRect = mClient;
        FocusRect--;
        Invalidate(FocusRect);
        BeginDraw();
        Rectangle(FocusRect, RectColor, 1);
        EndDraw();
    }
}

#endif


#ifndef PEG_RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegButton::Draw(void)
{
    if (PegThing::Style() & AF_TRANSPARENT)
    {
        return;
    }

    BeginDraw();

   #if (PEG_NUM_COLORS >= 4)

    PegColor Color(PCLR_SHADOW, muColors[PCI_NORMAL], CF_FILL);

   #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
    if (StatusIs(PSF_CURRENT))
    {
        Color.uBackground = muColors[PCI_SELECTED];
    }
   #endif

    // fill in the face:

    mClient = mReal;
    mClient--;

    Rectangle(mReal, Color, 0);

    if (Style() & FF_NONE)
    {
       #ifdef PEG_DRAW_FOCUS
        if (StatusIs(PSF_CURRENT))
        {
            DrawFocusIndicator(FALSE);
        }
       #endif
        EndDraw();
        return;
    }

    // draw the highlights:

    Color.uForeground = PCLR_LOWLIGHT;

    if (mwStyle & BF_SELECTED)
    {
        Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wRight - 1,
            mReal.wTop + 1, Color);
        Line(mReal.wLeft + 1, mReal.wTop + 2, mReal.wLeft + 1,
            mReal.wBottom - 1, Color);
        Color.uForeground = PCLR_SHADOW;
        mClient.wTop++;
        mClient.wLeft++;
    }
    else
    {
        Line(mReal.wRight - 1, mReal.wTop + 1, mReal.wRight - 1,
            mReal.wBottom - 1, Color);
        Line(mReal.wLeft + 1, mReal.wBottom - 1, mReal.wRight - 2,
            mReal.wBottom - 1, Color);
        Color.uForeground = PCLR_HIGHLIGHT;
        mClient.wRight--;
        mClient.wBottom--;
    }

    if (mwStyle & BF_FULLBORDER)
    {
        Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wLeft + 1,
             mReal.wBottom, Color);
        Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wRight - 1,
             mReal.wTop + 1, Color);
        mClient.wTop++;
        mClient.wLeft++;
    }
    else
    {
        Line(mReal.wLeft, mReal.wTop, mReal.wLeft,
             mReal.wBottom, Color);
        Line(mReal.wLeft, mReal.wTop, mReal.wRight,
             mReal.wTop, Color);
    }

    if (mwStyle & BF_SELECTED)
    {
        Color.uForeground = PCLR_HIGHLIGHT;
    }
    else
    {
        Color.uForeground = PCLR_SHADOW;
    }

    Line(mReal.wLeft, mReal.wBottom, mReal.wRight,
         mReal.wBottom, Color);
    Line(mReal.wRight, mReal.wTop, mReal.wRight,
         mReal.wBottom, Color);

   #else

    // here for monochrome:

    PegColor Color(PCLR_SHADOW, muColors[PCI_NORMAL], CF_FILL);

   #if defined(ACTIVE_BUTTON_COLOR_CHANGE)

    if (StatusIs(PSF_CURRENT))
    {
        Color.uBackground = muColors[PCI_SELECTED];
    }
   #endif

    mClient = mReal;

    #ifdef MONO_BUTTONSTYLE_3D
    
    Rectangle(mReal, Color, 1);
    mClient--;
     
    if (mwStyle & BF_SELECTED)
    {
        mClient.wTop++;
        mClient.wLeft++;
        Line(mReal.wLeft, mReal.wTop + 1, mReal.wRight, mReal.wTop + 1,
            Color);
        Line(mReal.wLeft + 1, mReal.wTop + 2, mReal.wLeft + 1, mReal.wBottom,
            Color);
    }
    else
    {
        mClient.wRight--;
        mClient.wBottom--;
        Line(mReal.wLeft, mReal.wBottom - 1, mReal.wRight, mReal.wBottom - 1,
            Color);
        Line(mReal.wRight - 1, mReal.wTop + 1, mReal.wRight - 1,
            mReal.wBottom - 1, Color);
    }

    #else   // no 3D monochrome buttons

    if (mwStyle & BF_SELECTED)
    {
        Rectangle(mReal, Color, 2);
        mClient -= 2;
    }
    else
    {
        Rectangle(mReal, Color, 1);
        mClient--;
    }
    

    #endif // end of 3D monochrome test
   #endif  // end of color vs. monochrome test

   #ifdef PEG_DRAW_FOCUS
    if (StatusIs(PSF_CURRENT))
    {
        DrawFocusIndicator(FALSE);
    }
   #endif
    EndDraw();
}

#else  // here for run-time color depth checking

/*--------------------------------------------------------------------------*/
void PegButton::Draw(void)
{
    if (PegThing::Style() & AF_TRANSPARENT)
    {
        return;
    }
    PegColor Color;

    BeginDraw();

    if (NumColors() >= 4)
    {
        Color.Set(PCLR_SHADOW, muColors[PCI_NORMAL], CF_FILL);

       #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
        if (StatusIs(PSF_CURRENT))
        {
            Color.uBackground = muColors[PCI_SELECTED];
        }
       #endif

	    // fill in the face:
	
	    mClient = mReal;
	    mClient--;
	
	    Rectangle(mReal, Color, 0);
	
	    if (mwStyle & FF_NONE)
	    {
           #ifdef PEG_DRAW_FOCUS
            if (StatusIs(PSF_CURRENT))
            {
                DrawFocusIndicator(FALSE);
            }
           #endif

	        EndDraw();
	        return;
	    }
	
	    // draw the highlights:
	
	    Color.uForeground = PCLR_LOWLIGHT;
	
	    if (mwStyle & BF_SELECTED)
	    {
	        Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wRight - 1,
	            mReal.wTop + 1, Color);
	        Line(mReal.wLeft + 1, mReal.wTop + 2, mReal.wLeft + 1,
	            mReal.wBottom - 1, Color);
	        Color.uForeground = PCLR_SHADOW;
	        mClient.wTop++;
	        mClient.wLeft++;
	    }
	    else
	    {
	        Line(mReal.wRight - 1, mReal.wTop + 1, mReal.wRight - 1,
	            mReal.wBottom - 1, Color);
	        Line(mReal.wLeft + 1, mReal.wBottom - 1, mReal.wRight - 2,
	            mReal.wBottom - 1, Color);
	        Color.uForeground = PCLR_HIGHLIGHT;
	        mClient.wRight--;
	        mClient.wBottom--;
	    }
	
	    if (mwStyle & BF_FULLBORDER)
	    {
	        Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wLeft + 1,
	             mReal.wBottom, Color);
	        Line(mReal.wLeft + 1, mReal.wTop + 1, mReal.wRight - 1,
	             mReal.wTop + 1, Color);
	        mClient.wTop++;
	        mClient.wLeft++;
	    }
	    else
	    {
	        Line(mReal.wLeft, mReal.wTop, mReal.wLeft,
	             mReal.wBottom, Color);
	        Line(mReal.wLeft, mReal.wTop, mReal.wRight,
	             mReal.wTop, Color);
	    }
	
	    if (mwStyle & BF_SELECTED)
	    {
	        Color.uForeground = PCLR_HIGHLIGHT;
	    }
	    else
	    {
	        Color.uForeground = PCLR_SHADOW;
	    }
	
	    Line(mReal.wLeft, mReal.wBottom, mReal.wRight,
	         mReal.wBottom, Color);
	    Line(mReal.wRight, mReal.wTop, mReal.wRight,
	         mReal.wBottom, Color);
    }
    else
    {
	    // here for monochrome:
	    PegColor Color(PCLR_SHADOW, muColors[PCI_NORMAL], CF_FILL);

       #if defined(ACTIVE_BUTTON_COLOR_CHANGE)

        if (StatusIs(PSF_CURRENT))
        {
            Color.uBackground = muColors[PCI_SELECTED];
        }
       #endif

	    mClient = mReal;
	
	    Rectangle(mReal, Color, 1);
	    mClient--;
	     
	    if (mwStyle & BF_SELECTED)
	    {
	        mClient.wTop++;
	        mClient.wLeft++;
	        Line(mReal.wLeft, mReal.wTop + 1, mReal.wRight, mReal.wTop + 1,
	            Color);
	        Line(mReal.wLeft + 1, mReal.wTop + 2, mReal.wLeft + 1, mReal.wBottom,
	            Color);
	    }
	    else
	    {
	        mClient.wRight--;
	        mClient.wBottom--;
	        Line(mReal.wLeft, mReal.wBottom - 1, mReal.wRight, mReal.wBottom - 1,
	            Color);
	        Line(mReal.wRight - 1, mReal.wTop + 1, mReal.wRight - 1,
	            mReal.wBottom - 1, Color);
	    }
    }

   #ifdef PEG_DRAW_FOCUS
    if (StatusIs(PSF_CURRENT))
    {
        DrawFocusIndicator(FALSE);
    }
   #endif

    EndDraw();
}

#endif   // RUNTIME_COLOR_CHECK if


/*--------------------------------------------------------------------------*/
void PegButton::Enable()
{
    mwStyle |= AF_ENABLED;
    AddStatus(PSF_SELECTABLE|PSF_TAB_STOP);

    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate(mReal);
    }
}

/*--------------------------------------------------------------------------*/
void PegButton::Disable()
{
    mwStyle &= ~AF_ENABLED;
    RemoveStatus(PSF_SELECTABLE|PSF_TAB_STOP);

    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate(mReal);
    }
}

/*--------------------------------------------------------------------------*/
void PegButton::Style(WORD wStyle)
{
    PegThing::Style(wStyle);
    if (wStyle & AF_ENABLED)
    {
        AddStatus(PSF_SELECTABLE|PSF_TAB_STOP);
    }
    else
    {
        RemoveStatus(PSF_SELECTABLE|PSF_TAB_STOP);
    }
}

/*--------------------------------------------------------------------------*/
// PegBitmapButton class implementation:
/*--------------------------------------------------------------------------*/
PegBitmapButton::PegBitmapButton(PegRect &Rect, PegBitmap *Bitmap,
    WORD wId, WORD wStyle) : PegButton(Rect, wId, wStyle)
{
    Type(TYPE_BMBUTTON);
    mpBitmap = Bitmap;
}

/*--------------------------------------------------------------------------*/
PegBitmapButton::PegBitmapButton(SIGNED iLeft, SIGNED iTop,
 PegBitmap *Bitmap, WORD wId, WORD wStyle) :  PegButton(wId, wStyle)
{
    Type(TYPE_BMBUTTON);
    mpBitmap = Bitmap;
    mReal.wLeft = iLeft;
    mReal.wTop = iTop;

    if (Bitmap)
    {
        mReal.wRight = mReal.wLeft + Bitmap->wWidth + BBUTTON_PADDING;    
        mReal.wBottom = mReal.wTop + Bitmap->wHeight + BBUTTON_PADDING;
    }
    else
    {
        mReal.wRight = mReal.wLeft + BBUTTON_PADDING;
        mReal.wBottom = mReal.wTop + BBUTTON_PADDING;
    }
}

/*--------------------------------------------------------------------------*/
void PegBitmapButton::Draw(void)
{
    BeginDraw();
    PegButton::Draw();

    PegRect OldClip = mClip;
    mClip &= mClient;

    // now draw the bitmap:

    PegPoint Put;
    Put.x = mClient.wLeft + (mClient.Width() - mpBitmap->wWidth) / 2;
    Put.y = mClient.wTop + (mClient.Height() - mpBitmap->wHeight) / 2;
    Bitmap(Put, mpBitmap);
    mClip = OldClip;

    if (First())
    {
        DrawChildren();
    }

    EndDraw();
}


/*--------------------------------------------------------------------------*/
#ifdef PEG_DRAW_FOCUS
void PegBitmapButton::EraseFocusIndicator(void)
{
    Invalidate();
    Draw();
}
#endif

/*--------------------------------------------------------------------------*/
// PegTextButton class implementation:
/*--------------------------------------------------------------------------*/
PegTextButton::PegTextButton(PegRect &Rect, const PEGCHAR *Text, WORD wId, 
    WORD wStyle) : PegButton(Rect, wId, wStyle),
    PegTextThing(Text, wStyle & TT_COPY, PEG_TBUTTON_FONT)
{
    Type(TYPE_TEXTBUTTON);
}


/*--------------------------------------------------------------------------*/
PegTextButton::PegTextButton(SIGNED iLeft, SIGNED iTop, SIGNED iWidth, 
    const PEGCHAR *Text, WORD wId, WORD wStyle) : PegButton(wId, wStyle),
    PegTextThing(Text, wStyle & TT_COPY, PEG_TBUTTON_FONT)
{
    Type(TYPE_TEXTBUTTON);

    mReal.wLeft = iLeft;
    mReal.wTop = iTop;
    mReal.wRight = mReal.wLeft + iWidth - 1;

    if (Text)
    {
        mReal.wBottom = mReal.wTop + TextHeight(Text, mpFont) + TBUTTON_PADDING;
    }
    else
    {
        mReal.wBottom = mReal.wTop + TBUTTON_PADDING;
    }
    mClient = mReal;
}

/*--------------------------------------------------------------------------*/
PegTextButton::PegTextButton(SIGNED iLeft, SIGNED iTop, const PEGCHAR *Text,
    WORD wId, WORD wStyle) : PegButton(wId, wStyle),
    PegTextThing(Text, wStyle & TT_COPY, PEG_TBUTTON_FONT)
{
    Type(TYPE_TEXTBUTTON);

    mReal.wLeft = iLeft;
    mReal.wTop = iTop;

    if (Text)
    {
        mReal.wRight = mReal.wLeft + TextWidth(Text, mpFont) + TBUTTON_PADDING + 3;
        mReal.wBottom = mReal.wTop + TextHeight(Text, mpFont) + TBUTTON_PADDING;
    }
    else
    {
        mReal.wRight = mReal.wLeft + TBUTTON_PADDING;
        mReal.wBottom = mReal.wTop + TBUTTON_PADDING;
    }
    mClient = mReal;
}

#ifndef PEG_RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegTextButton::Draw(void)
{
    PegColor Color;
    BeginDraw();
    PegButton::Draw();

   #if defined(ACTIVE_BUTTON_COLOR_CHANGE)    

   #if (PEG_NUM_COLORS > 2)
    if (StatusIs(PSF_CURRENT))
    {
        Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_NONE);
    }
    else
    {
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
    }
   #else
    if (StatusIs(PSF_CURRENT))
    {
        Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_FILL);
    }
    else
    {
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
    }
   #endif
   #else
   #if (PEG_NUM_COLORS > 2)
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
   #else
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
   #endif

   #endif

    if (!(Style() & AF_ENABLED))
    {
        Color.uForeground = PCLR_LOWLIGHT;
    }

    // now draw the text:

    PegPoint Put;
    SIGNED iSize;

    if (Style() & TJ_LEFT)
    {
       #ifdef PEG_DRAW_FOCUS
        Put.x = mClient.wLeft + 3;
       #else
        Put.x = mClient.wLeft + 1;
       #endif
    }
    else
    {
        iSize = TextWidth(mpText, mpFont);

        if (Style() & TJ_RIGHT)
        {
           #ifdef PEG_DRAW_FOCUS
            Put.x = mClient.wRight - 3 - iSize;
           #else
            Put.x = mClient.wRight - 1 - iSize;
           #endif
            if (!(mwStyle | FF_NONE))
            {
                Put.x--;
            }
        }
        else
        {
            Put.x = mClient.wLeft + (mClient.Width() - iSize) / 2;
        }
    }

    iSize = TextHeight(mpText, mpFont);
    if (iSize <= mClient.Height())
    {
        Put.y = mClient.wTop + (mClient.Height() - iSize) / 2;
    }
    else
    {
        Put.y = mClient.wTop;
    }

    DrawText(Put, mpText, Color, mpFont);

   #if (PEG_NUM_COLORS == 2)
    if (!(Style() & AF_ENABLED))
    {
        SIGNED yCenter = (mClient.wTop + mClient.wBottom) / 2;
        Line(Put.x, yCenter, Put.x + TextWidth(mpText, mpFont), yCenter, Color);
    }
   #endif

    if (First())
    {
        DrawChildren();
    }
    EndDraw();
}

#else   // here for RUNTIME_COLOR_CHECK
/*--------------------------------------------------------------------------*/
void PegTextButton::Draw(void)
{
    BeginDraw();
    PegButton::Draw();
    PegColor Color;

   #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
    if (StatusIs(PSF_CURRENT))
    {
        Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_NONE);
    }
    else
    {
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
    }
   #else
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
   #endif

    if (NumColors() < 4)
    {
        Color.uFlags = CF_FILL;
    }

    if (!(Style() & AF_ENABLED))
    {
        Color.uForeground = PCLR_LOWLIGHT;
    }

    // now draw the text:

    PegPoint Put;
    SIGNED iSize;

    if (Style() & TJ_LEFT)
    {
       #ifdef PEG_DRAW_FOCUS
        Put.x = mClient.wLeft + 3;
       #else
        Put.x = mClient.wLeft + 1;
       #endif
    }
    else
    {
        iSize = TextWidth(mpText, mpFont);

        if (Style() & TJ_RIGHT)
        {
           #ifdef PEG_DRAW_FOCUS
            Put.x = mClient.wRight - iSize - 3;
           #else
            Put.x = mClient.wRight - iSize - 1;
           #endif
        }
        else
        {
            Put.x = mClient.wLeft + (mClient.Width() - iSize) / 2;
        }
    }

    iSize = TextHeight(mpText, mpFont);
    if (iSize <= mClient.Height())
    {
        Put.y = mClient.wTop + (mClient.Height() - iSize) / 2;
    }
    else
    {
        Put.y = mClient.wTop;
    }

    DrawText(Put, mpText, Color, mpFont);

    if (NumColors() < 4)
    {
        if (!(Style() & AF_ENABLED))
        {
            SIGNED yCenter = (mClient.wTop + mClient.wBottom) / 2;
            Line(Put.x, yCenter, Put.x + TextWidth(mpText, mpFont), yCenter, Color);
        }
    }

    if (First())
    {
        DrawChildren();
    }
    EndDraw();
}

#endif  // RUNTIME_COLOR_CHECK if

/*--------------------------------------------------------------------------*/
void PegTextButton::DataSet(const PEGCHAR *Text)
{
    PegTextThing::DataSet(Text);

    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate();
    }
}

/*--------------------------------------------------------------------------*/
// PegRadioButton class implementation:
/*--------------------------------------------------------------------------*/
PegRadioButton::PegRadioButton(PegRect &Rect, const PEGCHAR *Text, WORD wId, 
    WORD wStyle) : PegButton(Rect, wId, wStyle),
    PegTextThing(Text, wStyle & TT_COPY, PEG_RBUTTON_FONT)
{
    Type(TYPE_RADIOBUTTON);
    SetSignals(SIGMASK(PSF_DOT_ON));
}

/*--------------------------------------------------------------------------*/
PegRadioButton::PegRadioButton(SIGNED iLeft, SIGNED iTop,
     const PEGCHAR *Text, WORD wId, WORD wStyle) :
     PegButton(wId, wStyle),
     PegTextThing(Text, wStyle & TT_COPY, PEG_RBUTTON_FONT)
{
    Type(TYPE_RADIOBUTTON);

    SetSignals(SIGMASK(PSF_DOT_ON));

    mReal.wLeft = iLeft;
    mReal.wTop = iTop;

    SIGNED iHeight = TextHeight(lsTEST, mpFont);
    if (gbRadioOnBitmap.wHeight > iHeight)
    {
        iHeight = gbRadioOnBitmap.wHeight;
    }
    iHeight += 4;

    if (Text)
    {
        mReal.wRight = mReal.wLeft + TextWidth(Text, mpFont) + 2;
        mReal.wRight += gbRadioOnBitmap.wWidth + RBUTTON_SPACING;
        mReal.wBottom = mReal.wTop + iHeight;
    }
    else
    {
        mReal.wRight = mReal.wLeft + 3;
        mReal.wBottom = mReal.wTop + iHeight;
    }
    mClient = mReal;
}

/*--------------------------------------------------------------------------*/
SIGNED PegRadioButton::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_LBUTTONDOWN:
        if (mwStyle & AF_ENABLED)
        {
            CapturePointer();
        }
        break;

    case PM_LBUTTONUP:
        if (StatusIs(PSF_OWNS_POINTER))
        {
            if (mReal.Contains(Mesg.Point))
            {
                SetSelected(TRUE);
            }
            ReleasePointer();
        }
        break;

    case PM_POINTER_MOVE:
    case PM_POINTER_EXIT:
        break;

   #ifdef PEG_KEYBOARD_SUPPORT

    case PM_KEY:
        if ((Mesg.iData == PK_CR || Mesg.iData == PK_SPACE) &&
            (mwStyle & AF_ENABLED))
        {
            if (!(mwStyle & BF_SELECTED))
            {
                SetSelected(TRUE);
            }
        }
        else
        {
            PegThing::Message(Mesg);
        }
        break;

    case PM_KEY_RELEASE:
        if (Mesg.iData == PK_CR || Mesg.iData == PK_SPACE)
        {
            break;
        }
        else
        {
            PegThing::Message(Mesg);
        }
        break;
   #endif

    default:
        PegButton::Message(Mesg);
        break;
    }
    return(0);
}

#ifndef PEG_RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegRadioButton::Draw(void)
{
    PegBitmap *pbm;
    PegColor Color;

   #if (PEG_NUM_COLORS > 2)
   #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
    if (StatusIs(PSF_CURRENT))
    {
        Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_NONE);
    }
    else
    {
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
    }
   #else
    Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
   #endif

    if (!(mwStyle & AF_ENABLED))
    {
        Color.uForeground = PCLR_LOWLIGHT;

        if (mwStyle & BF_SELECTED)
        {
            pbm = &gbRadioOnDisabledBitmap;
        }
        else
        {
            pbm = &gbRadioOffDisabledBitmap;
        }
    }
    else
    {
        if (mwStyle & BF_SELECTED)
        {
            pbm = &gbRadioOnBitmap;
        }
        else
        {
            pbm = &gbRadioOffBitmap;
        }
    }
   #else

   #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
    if (StatusIs(PSF_CURRENT))
    {
        Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_FILL);
    }
    else
    {
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
    }
   #else
    Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
   #endif

    if (mwStyle & BF_SELECTED)
    {
        pbm = &gbRadioOnBitmap;
    }
    else
    {
        pbm = &gbRadioOffBitmap;
    }
   #endif

    BeginDraw();

    SIGNED yCenter = (mReal.wTop + mReal.wBottom) / 2;
    PegPoint Put;
   
   #ifdef PEG_DRAW_FOCUS
    Put.x = mReal.wLeft + 3;
   #else
    Put.x = mReal.wLeft;
   #endif
    Put.y = yCenter - (pbm->wHeight / 2);
    Bitmap(Put, pbm);

    // now draw the text:

    Put.x = mReal.wLeft + pbm->wWidth + RBUTTON_SPACING;
    SIGNED iSize = TextHeight(mpText, mpFont);
    Put.y = yCenter - (iSize / 2);
    DrawText(Put, mpText, Color, mpFont);

   #if (PEG_NUM_COLORS == 2)
    if (!(mwStyle & AF_ENABLED))
    {
        Line(Put.x, yCenter, Put.x + TextWidth(mpText, mpFont) - 2, yCenter, Color);
    }
   #endif

   #ifdef PEG_DRAW_FOCUS
    if (StatusIs(PSF_CURRENT))
    {
        DrawFocusIndicator(FALSE);
    }
   #endif

    EndDraw();
}

#else // here for RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegRadioButton::Draw(void)
{
    PegBitmap *pbm;
    PegColor Color;

   #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
    if (StatusIs(PSF_CURRENT))
    {
        Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_NONE);
    }
    else
    {
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
    }

   #else
    Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
   #endif

    if (NumColors() < 4)
    {
        Color.uFlags = CF_FILL;
    }

    if (!(mwStyle & AF_ENABLED))
    {
        Color.uForeground = PCLR_LOWLIGHT;
        if (mwStyle & BF_SELECTED)
        {
            pbm = &gbRadioOnDisabledBitmap;
        }
        else
        {
            pbm = &gbRadioOffDisabledBitmap;
        }

    }
    else
    {
        if (mwStyle & BF_SELECTED)
        {
            pbm = &gbRadioOnBitmap;
        }
        else
        {
            pbm = &gbRadioOffBitmap;
        }
    }

    BeginDraw();

    SIGNED yCenter = (mReal.wTop + mReal.wBottom) / 2;
    PegPoint Put;

   #ifdef PEG_DRAW_FOCUS
    Put.x = mReal.wLeft + 3;
   #else
    Put.x = mReal.wLeft;
   #endif
    Put.y = yCenter - (pbm->wHeight / 2);
    Bitmap(Put, pbm);

    // now draw the text:

    Put.x = mReal.wLeft + pbm->wWidth + RBUTTON_SPACING;
    SIGNED iSize = TextHeight(mpText, mpFont);
    Put.y = yCenter - (iSize / 2);
    DrawText(Put, mpText, Color, mpFont);

    if (NumColors() < 4)
    {
        if (!(mwStyle & AF_ENABLED))
        {
            Line(Put.x, yCenter, Put.x + TextWidth(mpText, mpFont) - 2, yCenter, Color);
        }
    }

   #ifdef PEG_DRAW_FOCUS
    if (StatusIs(PSF_CURRENT))
    {
        DrawFocusIndicator(FALSE);
    }
   #endif

    EndDraw();
}

#endif // RUNTIME_COLOR_CHECK if

/*--------------------------------------------------------------------------*/
void PegRadioButton::SetSelected(BOOL bSelect)
{
    if (bSelect)
    {
        if (!(mwStyle & BF_SELECTED))
        {
            UnselectSiblings();
            mwStyle |= BF_SELECTED;
            if (StatusIs(PSF_VISIBLE))
            {
                Invalidate(mClip);
                Draw();
                CheckSendSignal(PSF_DOT_ON);
            }
        }
    }
    else
    {
        if (mwStyle & BF_SELECTED)
        {
            mwStyle &= ~BF_SELECTED;

            if (StatusIs(PSF_VISIBLE))
            {
                Invalidate(mClip);
                Draw();
                CheckSendSignal(PSF_DOT_OFF);
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
void PegRadioButton::UnselectSiblings(void)
{
    PegThing *pTest;

    if (Parent())
    {
        pTest = Parent()->First();

        while(pTest)
        {
            if (pTest->Type() == TYPE_RADIOBUTTON)
            {
                PegRadioButton *pButton = (PegRadioButton *) pTest;
                pButton->SetSelected(FALSE);
            }
            pTest = pTest->Next();
        }
    }
}


/*--------------------------------------------------------------------------*/
// PegRadioButton class implementation:
/*--------------------------------------------------------------------------*/
PegCheckBox::PegCheckBox(PegRect &Rect, const PEGCHAR *Text, WORD wId, WORD wStyle) :
    PegButton(Rect, wId, wStyle),
    PegTextThing(Text, wStyle & TT_COPY, PEG_CHECKBOX_FONT)
{
    Type(TYPE_CHECKBOX);
    SetSignals(SIGMASK(PSF_CHECK_ON)|SIGMASK(PSF_CHECK_OFF));
}

/*--------------------------------------------------------------------------*/
PegCheckBox::PegCheckBox(SIGNED iLeft, SIGNED iTop, const PEGCHAR *Text, WORD wId, 
    WORD wStyle) : PegButton(wId, wStyle),
    PegTextThing(Text, wStyle & TT_COPY, PEG_CHECKBOX_FONT)
{
    Type(TYPE_CHECKBOX);

    SetSignals(SIGMASK(PSF_CHECK_ON)|SIGMASK(PSF_CHECK_OFF));

    mReal.wLeft = iLeft;
    mReal.wTop = iTop;

    if (Text)
    {
        mReal.wRight = mReal.wLeft + TextWidth(Text, mpFont);
        mReal.wRight += gbCheckBoxOnBitmap.wWidth + CBOX_SPACING + 2;
        mReal.wBottom = mReal.wTop + gbCheckBoxOnBitmap.wHeight + 4;
    }
    else
    {
        mReal.wRight = mReal.wLeft + 3;
        mReal.wBottom = mReal.wTop + 3;
    }
    mClient = mReal;
}

/*--------------------------------------------------------------------------*/
SIGNED PegCheckBox::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_LBUTTONDOWN:
        if (mwStyle & AF_ENABLED)
        {
            CapturePointer();
        }
        break;

    case PM_LBUTTONUP:
        if (StatusIs(PSF_OWNS_POINTER))
        {
            if (mReal.Contains(Mesg.Point))
            {
                if (mwStyle & BF_SELECTED)
                {
                    SetSelected(FALSE);
                }
                else
                {
                    SetSelected(TRUE);
                }
            }
            ReleasePointer();
        }
        break;

    case PM_POINTER_MOVE:
    case PM_POINTER_EXIT:
        break;

   #ifdef PEG_KEYBOARD_SUPPORT

    case PM_KEY:
        if ((Mesg.iData == PK_CR || Mesg.iData == PK_SPACE) &&
            (mwStyle & AF_ENABLED))
        {
            if (!(mwStyle & BF_SELECTED))
            {
                SetSelected(TRUE);
            }
            else
            {
                SetSelected(FALSE);
            }
        }
        else
        {
            PegThing::Message(Mesg);
        }
        break;

    case PM_KEY_RELEASE:
        if (Mesg.iData == PK_CR)
        {
            break;
        }
        else
        {
            PegThing::Message(Mesg);
        }
        break;
   #endif

    default:
        PegButton::Message(Mesg);
        break;
    }
    return(0);
}

#ifndef PEG_RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegCheckBox::Draw(void)
{
    PegBitmap *pbm;
    PegColor Color;

   #if (PEG_NUM_COLORS > 2)
   #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
    if (StatusIs(PSF_CURRENT))
    {
        Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_NONE);
    }
    else
    {
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
    } 
   #else
    Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
   #endif

    if (!(mwStyle & AF_ENABLED))
    {
        Color.uForeground = PCLR_LOWLIGHT;
        if (mwStyle & BF_SELECTED)
        {
            pbm = &gbCheckBoxOnDisabledBitmap;
        }
        else
        {
            pbm = &gbCheckBoxOffDisabledBitmap;
        }
    }
    else
    {
        if (mwStyle & BF_SELECTED)
        {
            pbm = &gbCheckBoxOnBitmap;
        }
        else
        {
            pbm = &gbCheckBoxOffBitmap;
        }
    }
   #else

   #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
    if (StatusIs(PSF_CURRENT))
    {
        Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_FILL);
    }
    else
    {
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
    }
   #else
    Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_FILL);
   #endif

    if (mwStyle & BF_SELECTED)
    {
        pbm = &gbCheckBoxOnBitmap;
    }
    else
    {
        pbm = &gbCheckBoxOffBitmap;
    }
   #endif

    BeginDraw();

    SIGNED yCenter = (mReal.wTop + mReal.wBottom) / 2;
    PegPoint Put;

   #ifdef PEG_DRAW_FOCUS
    Put.x = mReal.wLeft + 3;
   #else
    Put.x = mReal.wLeft;
   #endif
   
    Put.y = yCenter - (pbm->wHeight / 2);
    Bitmap(Put, pbm);

    // now draw the text:

    Put.x = mReal.wLeft + pbm->wWidth + CBOX_SPACING;
    WORD wSize = TextHeight(mpText, mpFont);
    Put.y = yCenter - (wSize / 2);
    DrawText(Put, mpText, Color, mpFont);

   #if (PEG_NUM_COLORS == 2)
    if (!(mwStyle & AF_ENABLED))
    {
        Line(Put.x, yCenter, Put.x + TextWidth(mpText, mpFont) - 2, yCenter, Color);
    }
   #endif

   #ifdef PEG_DRAW_FOCUS
    if (StatusIs(PSF_CURRENT))
    {
        DrawFocusIndicator(FALSE);
    }
   #endif

    EndDraw();
}

#else  // here for RUNTIME_COLOR_CHECK

/*--------------------------------------------------------------------------*/
void PegCheckBox::Draw(void)
{
    PegBitmap *pbm;
    PegColor Color;

   #if defined(ACTIVE_BUTTON_COLOR_CHANGE)
    if (StatusIs(PSF_CURRENT))
    {
        Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_NONE);
    }
    else
    {
        Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
    }
   #else
    Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
   #endif

    if (NumColors() < 4)
    {
        Color.uFlags = CF_FILL;
    }

    if (!(mwStyle & AF_ENABLED))
    {
        Color.uForeground = PCLR_LOWLIGHT;
        if (mwStyle & BF_SELECTED)
        {
            pbm = &gbCheckBoxOnDisabledBitmap;
        }
        else
        {
            pbm = &gbCheckBoxOffDisabledBitmap;
        }
    }
    else
    {
        if (mwStyle & BF_SELECTED)
        {
            pbm = &gbCheckBoxOnBitmap;
        }
        else
        {
            pbm = &gbCheckBoxOffBitmap;
        }
    }
    BeginDraw();

    SIGNED yCenter = (mReal.wTop + mReal.wBottom) / 2;
    PegPoint Put;

   #ifdef PEG_DRAW_FOCUS
    Put.x = mReal.wLeft + 3;
   #else
    Put.x = mReal.wLeft;
   #endif
    Put.y = yCenter - (pbm->wHeight / 2);
    Bitmap(Put, pbm);

    // now draw the text:

    Put.x = mReal.wLeft + pbm->wWidth + CBOX_SPACING;
    WORD wSize = TextHeight(mpText, mpFont);
    Put.y = yCenter - (wSize / 2);
    DrawText(Put, mpText, Color, mpFont);

    if (NumColors() < 4)
    {
        if (!(mwStyle & AF_ENABLED))
        {
            Line(Put.x, yCenter, Put.x + TextWidth(mpText, mpFont) - 2, yCenter, Color);
        }
    }

   #ifdef PEG_DRAW_FOCUS
    if (StatusIs(PSF_CURRENT))
    {
        DrawFocusIndicator(FALSE);
    }
   #endif
    EndDraw();
}

#endif      // RUNTIME_COLOR_CHECK if

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegCheckBox::SetSelected(BOOL bSelect)
{
    if (bSelect)
    {
        if (!(mwStyle & BF_SELECTED))
        {
            mwStyle |= BF_SELECTED;

            if (StatusIs(PSF_VISIBLE))
            {
                Invalidate(mClip);
                Draw();
                CheckSendSignal(PSF_CHECK_ON);
            }
        }
    }
    else
    {
        if (mwStyle & BF_SELECTED)
        {
            mwStyle &= ~BF_SELECTED;

            if (StatusIs(PSF_VISIBLE))
            {
                Invalidate(mClip);
                Draw();
                CheckSendSignal(PSF_CHECK_OFF);
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegSystemButton::PegSystemButton(PegRect &Rect, PegBitmap *Bitmap, WORD wId) :
    PegBitmapButton(Rect, Bitmap, wId)
{
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegSystemButton::Message(const PegMessage &Mesg)
{
    if (Mesg.wType == PM_LBUTTONDOWN)
    {
        CheckSendSignal(PSF_CLICKED);
        return 0;
    }
    return PegBitmapButton::Message(Mesg);
}

// End of file
