/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// pthing.hpp - Base PEG GUI object class definition.
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
// This class identifies the base functionality inherited by all GUI objects.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGTHING_
#define _PEGTHING_

class PegPresentationManager;        // forward references;

class PegThing
{
    friend class PegScreen;
    
    #ifdef PEG_PRINTER_SUPPORT
    friend class PegPrint;
    #endif
 
    public:

        PegThing(const PegRect &Rect, WORD wId = 0,
            WORD wStyle = FF_NONE);
        PegThing(WORD wId = 0, WORD wStyle = FF_NONE);
        virtual ~PegThing();
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void   Draw(void);
        virtual void Add(PegThing *Who, BOOL bDraw = TRUE);
        virtual void AddToEnd(PegThing *Who, BOOL bDraw = TRUE);
        virtual void MoveToFront(PegThing *Who, BOOL bDraw = TRUE);
        virtual PegThing *Remove(PegThing *Who, BOOL bDraw = TRUE);
        virtual void SetColor(const UCHAR uIndex, const COLORVAL uColor);
        virtual COLORVAL GetColor(const UCHAR uIndex);
        virtual void Center(PegThing *Who);
        virtual void DrawChildren(void);
        virtual void Resize(PegRect Rect);
        void Destroy(PegThing *Who);
        const PEGCHAR *Version(void);

       #if defined(PEG_PRINTER_SUPPORT)
        void Print(void);
       #endif

        PegThing *Parent (void) const {return mpParent;}
        PegThing *First (void) const {return mpFirst;}
        PegThing *Next (void) const {return mpNext;}
        PegThing *Previous(void) const {return mpPrev;}

        static PegPresentationManager *Presentation() 
        {
            return mpPresentation;
        }

        static PegMessageQueue *MessageQueue() 
        {
            return mpMessageQueue;
        }

        static PegScreen *Screen() {return mpScreen;}

       #ifdef PEG_PRINTER_SUPPORT
        static PegScreen *Printer() {return mpPrint;}
       #endif

        UCHAR Type(void) { return muType; }
        void  Type(UCHAR uSet) {muType = uSet;}

        WORD Id(void) {return mwId;}
        void Id(WORD wId) {mwId = wId;}

        virtual PegThing *Find(WORD wId, BOOL bRecursive = TRUE);

        void SetSignals(WORD wMask) {mwSignalMask = wMask;}
        
        void SetSignals(WORD wId, WORD wMask)
        {
            mwId = wId;
            mwSignalMask = wMask;
        }

        WORD GetSignals(void) {return mwSignalMask;}
        
        BOOL CheckSendSignal(UCHAR uSignal)
        {
            if (mpParent && mwId && (mwSignalMask & SIGMASK((WORD)uSignal)) &&
                mpParent->StatusIs(PSF_VISIBLE))
            {
                SendSignal(uSignal);
                return TRUE;
            }
            return FALSE;
        }
        
        BOOL  StatusIs(WORD wMask)
        {
            if (mwStatus & wMask)
            {
                return TRUE;
            }
            return FALSE;
        }

        virtual void  AddStatus(WORD wOrVal) {mwStatus |= wOrVal;}
        virtual void  RemoveStatus(WORD wAndVal) {mwStatus &= ~wAndVal;}
        WORD  GetStatus(void) {return mwStatus;}

        void CapturePointer(void);
        void ReleasePointer(void);

        void  FrameStyle(WORD wStyle)
        {
            wStyle &= FF_MASK;
            mwStyle &= ~FF_MASK;
            mwStyle |= wStyle;
        }

        WORD FrameStyle(void) {return (mwStyle & FF_MASK);}
        virtual WORD Style(void) {return mwStyle;}
        virtual void Style(WORD wStyle) {mwStyle = wStyle;}
        virtual void InitClient(void);
        virtual PegPoint CenterOf(PegThing *Who);

        virtual void SendSignal(UCHAR uSignal);
        void StandardBorder(COLORVAL bFillColor);
        void MessageChildren(const PegMessage &Mesg);

        void UpdateChildClipping(void);
        virtual void ParentShift(SIGNED x, SIGNED y);

        static void SetScreenPtr(PegScreen *ps) {mpScreen = ps;}

       #ifdef PEG_PRINTER_SUPPORT
        static void SetPrintPtr(PegScreen *pp) {mpPrint = pp;}
       #endif

        static void SetMessageQueuePtr(PegMessageQueue *pq)
        {
            mpMessageQueue = pq;
        }
        static void SetPresentationManagerPtr(PegPresentationManager *pm)
        {
            mpPresentation = pm;
        }

       #ifdef PEG_FULL_CLIPPING
        Viewport *ViewportList(void) {return mpViewportList;}
       #endif

        // ------------------------------------------------------
        // Default key handling functions:
        // ------------------------------------------------------

       #ifdef PEG_KEYBOARD_SUPPORT

        virtual SIGNED DefaultKeyHandler(const PegMessage &InMesg);

       #if defined (PEG_ARROW_KEY_SUPPORT)

        virtual BOOL CheckDirectionalMove(SIGNED iKey);

        virtual PegThing *FindNearestNeighbor(SIGNED iKey,
            PegThing *pStart, PegPoint iCenter);

        PegThing *FindNearestNeighbor(PegThing *pStart, LONG *pPutDist, SIGNED iKey,
            PegPoint CenterThis, BOOL bLoose);

        LONG Distance(PegPoint p1, PegPoint p2);
       #endif
        
       #if defined(PEG_KEYBOARD_SUPPORT) && defined(PEG_TAB_KEY_SUPPORT)
        PegThing *PrevTabLink(void);
        PegThing *NextTabLink(void);
        void SetTabOrder(WORD *pIds);
        void SetDefaultTabLinks(void);
        void SetTabLink(PegThing *pNext) {mpTabLink = pNext;}
       #endif

       #endif

        // ------------------------------------------------------
        // Wrappers for calling PegMessageQueue functions:
        // ------------------------------------------------------

        inline void SetTimer(WORD wId, LONG lCount, LONG lReset)
        {
            MessageQueue()->SetTimer(this, wId, lCount, lReset);
        }

        inline void KillTimer(WORD wId)
        {
            MessageQueue()->KillTimer(this, wId);
        }

        // ------------------------------------------------------
        // Wrappers for calling PegScreen functions:
        // ------------------------------------------------------

        inline void BeginDraw(void){Screen()->BeginDraw(this);}
        inline void BeginDraw(PegBitmap *pbm) {Screen()->BeginDraw(this, pbm);}
        inline void EndDraw(void)  {Screen()->EndDraw();}
        inline void EndDraw(PegBitmap *pbm) {Screen()->EndDraw(pbm);}

        inline void Line(SIGNED wXStart, SIGNED wYStart,
            SIGNED wXEnd, SIGNED wYEnd, const PegColor &Color,
            SIGNED wWidth = 1)
        {
                Screen()->Line(this, wXStart, wYStart, wXEnd, wYEnd,
                    Color, wWidth);
        }

        inline void Rectangle(const PegRect &Rect, const PegColor &Color,
            SIGNED wWidth = 1)
        {
                Screen()->Rectangle(this, Rect, Color, wWidth);
        }

        inline void Bitmap(PegPoint Where, PegBitmap *Getmap,
            BOOL bOnTop = FALSE)
        {
                Screen()->Bitmap(this, Where, Getmap, bOnTop);
        }

        inline void BitmapFill(PegRect Rect, PegBitmap *Getmap)
        {
            Screen()->BitmapFill(this, Rect, Getmap);
        }

        inline void RectMove(PegRect Get, PegPoint Put)
        {
            Screen()->RectMove(this, Get, Put);
        }

        inline void DrawText(PegPoint Where, const PEGCHAR *Text,
            PegColor &Color, PegFont *Font, SIGNED Count = -1)
        {
            Screen()->DrawText(this, Where, Text, Color, Font, Count);
        }

        inline SIGNED TextHeight(const PEGCHAR *Text, PegFont *Font)
        {
            return Screen()->TextHeight(Text, Font);

        }
        inline SIGNED TextWidth(const PEGCHAR *Text, PegFont *Font)
        {
            return Screen()->TextWidth(Text, Font);
        }
        
        inline void Invalidate(const PegRect &Rect)
        {
            Screen()->Invalidate(Rect);
        }

        inline void Invalidate(void)
        {
            Screen()->Invalidate(mClient);
        }

        inline void SetPointerType(UCHAR bType)
        {
            Screen()->SetPointerType(bType);
        }

       #ifdef PEG_RUNTIME_COLOR_CHECK
        inline DWORD NumColors(void)
        {
            return(Screen()->NumColors(this));
        }
       #endif

       #ifdef PEG_FULL_GRAPHICS
        inline void Circle(SIGNED xCenter, SIGNED yCenter, SIGNED radius,
            PegColor &Color, SIGNED iWidth)
        {
            Screen()->Circle(this, xCenter, yCenter, radius, Color, iWidth);
        }
       #endif

        COLORVAL muColors[4];
        PegRect mReal;
        PegRect mClient;
        PegRect mClip;

    protected:

        void KillFocus(PegThing *);
        void MoveFocusToFirstClientChild(void);

        UCHAR   muType;
        WORD    mwStyle;
        WORD    mwId;
        WORD    mwStatus;
        WORD    mwSignalMask;
        PegThing *mpParent;
        PegThing *mpFirst;
        PegThing *mpNext;
        PegThing *mpPrev;

       #if defined(PEG_KEYBOARD_SUPPORT) && defined(PEG_TAB_KEY_SUPPORT)
        PegThing *mpTabLink;
       #endif

       #ifdef PEG_FULL_CLIPPING
        Viewport *mpViewportList;
       #endif

        static PegScreen *mpScreen;
        static PegPresentationManager *mpPresentation;
        static PegMessageQueue *mpMessageQueue;

       #ifdef PEG_PRINTER_SUPPORT
        static PegScreen *mpPrint;
       #endif

};

#endif

// End of file


