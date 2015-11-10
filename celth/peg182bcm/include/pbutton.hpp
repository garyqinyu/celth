/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pbutton.hpp - Definition of all PegButton classes.
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

#ifndef _PEGBUTTON_
#define _PEGBUTTON_

/*--------------------------------------------------------------------------*/
// The definitions below specify padding and seperation used by the button
// types when constructed with only the top-left position specified, and
// also the seperation between the checkbox and radio button indicators and
// the associated text. These values can be adjusted to preference.
/*--------------------------------------------------------------------------*/

#ifdef PEG_DRAW_FOCUS

#define TBUTTON_PADDING 5
#define BBUTTON_PADDING 3
#define RBUTTON_SPACING 8
#define CBOX_SPACING    8

#else

#define TBUTTON_PADDING 3
#define BBUTTON_PADDING 3
#define RBUTTON_SPACING 10
#define CBOX_SPACING    10

#endif

class PegButton : public PegThing
{
    public:
        PegButton(const PegRect &, WORD wId = 0, WORD wFlags = AF_ENABLED);
        PegButton(WORD wId = 0, WORD wFlags = AF_ENABLED);
        virtual ~PegButton() {}
        virtual void Draw(void);
        virtual SIGNED Message(const PegMessage &Mesg);
        void Enable(void);
        void Disable(void);
        virtual void Style(WORD wStyle);
        virtual WORD Style(void) {return PegThing::Style();}

    protected:
       #ifdef PEG_DRAW_FOCUS
        virtual void DrawFocusIndicator(BOOL bDraw);
        virtual void EraseFocusIndicator(void);
       #endif
};


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegBitmapButton : public PegButton
{
    public:
        PegBitmapButton(PegRect &, PegBitmap *bm, WORD wId = 0,
            WORD wStyle = AF_ENABLED);
        PegBitmapButton(SIGNED iLeft, SIGNED iTop,  
            PegBitmap *bm, WORD wId = 0, WORD wStyle = AF_ENABLED);
        virtual ~PegBitmapButton() {}
        virtual void Draw(void);
        virtual void SetBitmap(PegBitmap *nbm) { mpBitmap = nbm;}

    protected:

        PegBitmap *mpBitmap;

       #ifdef PEG_DRAW_FOCUS
        virtual void EraseFocusIndicator(void);
       #endif
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegTextButton : public PegButton, public PegTextThing
{
    public:
        PegTextButton(PegRect &, const PEGCHAR *text,
            WORD wId = 0, WORD wStyle = AF_ENABLED);

        PegTextButton(SIGNED wLeft, SIGNED wTop, SIGNED iWidth, 
            const PEGCHAR *text, WORD wId = 0, 
            WORD wStyle = AF_ENABLED);

        PegTextButton(SIGNED wLeft, SIGNED wTop, const PEGCHAR *text, 
            WORD wId = 0, WORD wStyle = AF_ENABLED);

        virtual ~PegTextButton() {}
        virtual void Draw(void);
        virtual void DataSet(const PEGCHAR *Text);
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegRadioButton : public PegButton, public PegTextThing
{
    public:
        PegRadioButton(PegRect &, const PEGCHAR *text, WORD wId = 0,
            WORD wStyle = AF_ENABLED);

        PegRadioButton(SIGNED iLeft, SIGNED itop, const PEGCHAR *text, 
            WORD wId = 0, WORD wStyle = AF_ENABLED);

        virtual ~PegRadioButton() {}
        virtual void Draw(void);
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void SetSelected(BOOL bSelected = TRUE);
        virtual BOOL IsSelected(void)
        {
            if (mwStyle & BF_SELECTED)
            {
                return TRUE;
            }
            return FALSE;
        }
        

    protected:
        void UnselectSiblings(void);
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegCheckBox : public PegButton, public PegTextThing
{
    public:
        PegCheckBox(PegRect &, const PEGCHAR *text, WORD wId = 0,
            WORD Style = AF_ENABLED);

        PegCheckBox(SIGNED iLeft, SIGNED iTop, const PEGCHAR *text,
            WORD wId = 0, WORD wStyle = AF_ENABLED);

        virtual ~PegCheckBox() {}
        virtual void Draw(void);
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void SetSelected(BOOL bSelected = TRUE);
        virtual BOOL IsChecked(void)
        {
            if (mwStyle & BF_SELECTED)
            {
                return TRUE;
            }
            return FALSE;
        }
};


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegSystemButton : public PegBitmapButton
{
    public:
        PegSystemButton(PegRect &, PegBitmap *, WORD wId);
        SIGNED Message(const PegMessage &Mesg);
};

#endif

