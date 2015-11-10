/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pscroll.hpp - Scroll bar class definitions.
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

#ifndef _PEGSCROLL_
#define _PEGSCROLL_

#define PEG_SCROLL_WIDTH 16     // default scroll bar width
//#define PEG_SCROLL_WIDTH  9    // for very narrow scroll bars.
//#define PEG_SCROLL_WIDTH 32   // for very wide scroll bars

#define PSB_VERTICAL 1
#define PSB_HORIZONTAL 0

class PEG_WINDOW;

struct PegScrollInfo 
{
    SIGNED wMin;
    SIGNED wMax;
    SIGNED wCurrent;
    SIGNED wStep;
    SIGNED wVisible;
};


/*--------------------------------------------------------------------------*/
class PegScrollButton : public PegThing
{
    public:
        PegScrollButton(BOOL bAmVertical);
        virtual ~PegScrollButton();
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void Draw(void);

    protected:

        BOOL MoveDraw(SIGNED  iPos);

        BOOL mbAmVertical;
        BOOL mbMoveMode;
        SIGNED mwLastMove;
        SIGNED mwMoveStart;
        PegRect mwPosRect;
};


/*--------------------------------------------------------------------------*/
class PegVScroll : public PegThing
{
    public:

        PegVScroll();
        PegVScroll(const PegRect &InRect, PegScrollInfo *si, WORD wId = 0);
        virtual ~PegVScroll();
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void Draw(void);
        PegScrollInfo* GetScrollInfo(void) {return &mScrollInfo;}
        void Reset(void);
        void Reset(PegScrollInfo *);
        void Resize(PegRect NewSize);

        enum MyButtonIds {
          IDB_UPBUTTON = 1000,
          IDB_DOWNBUTTON
        };

    protected:

        void SetScrollButtonPos(void);
        void CheckScrollLimits(void);
        PegBitmapButton *mpUpButton;
        PegBitmapButton *mpDownButton;
        PegScrollButton *mpScrollButton;
        PegScrollInfo mScrollInfo;
};

/*--------------------------------------------------------------------------*/
class PegHScroll : public PegThing
{
    public:

        PegHScroll();
        PegHScroll(const PegRect &InRect, PegScrollInfo *si, WORD wId = 0);
        virtual ~PegHScroll();
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void Draw(void);
        PegScrollInfo *GetScrollInfo(void) {return &mScrollInfo;}
        void Reset(void);
        void Reset(PegScrollInfo *);
        void Resize(PegRect Rect);

        enum MyButtonIds {
          IDB_LEFTBUTTON = 1000,
          IDB_RIGHTBUTTON
        };

    protected:

        void SetScrollButtonPos(void);
        void CheckScrollLimits(void);
        PegBitmapButton *mpLeftButton;
        PegBitmapButton *mpRightButton;
        PegScrollButton *mpScrollButton;
        PegScrollInfo mScrollInfo;
};

#endif

