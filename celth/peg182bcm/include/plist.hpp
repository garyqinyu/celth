/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plist.hpp - PegList, PegVertList, and PegHorzList class definitions.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2001 Swell Software, Inc.
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software, Inc.
// is strictly prohibited.
//
// Notes:
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGVLIST_
#define _PEGVLIST_

class PegList : public PegWindow
{
    public:
        PegList(const PegRect &Rect, WORD wId = 0, 
            WORD wStyle = FF_THIN);
        virtual ~PegList();
        virtual SIGNED Message(const PegMessage &Mesg);
        virtual void Add(PegThing *What, BOOL bDraw = TRUE);
        virtual void AddToEnd(PegThing *What, BOOL bDraw = TRUE);
		virtual void Insert(PegThing *What, SIGNED Where, BOOL bSelect=TRUE, BOOL bDraw = TRUE);
        virtual PegThing *Remove(PegThing *What, BOOL bDraw = TRUE);
        virtual PegThing *GetSelected(void) {return mpLastSelected;}
        virtual void SetSelected(PegThing *Who);
        virtual SIGNED GetIndex(PegThing *);
        virtual SIGNED GetSelectedIndex(void);
        virtual PegThing *SetSelected(SIGNED iIndex);
        virtual SIGNED Clear();
        PegThing *SelectNext(void);
        PegThing *SelectPrevious(void);
        PegThing *PageDown(void);
        PegThing *PageUp(void);
        void SetSeperation(SIGNED iSep) {miChildSeperation = iSep;}
        SIGNED GetNumItems(void);

    protected:
        virtual void PositionChildren(void) = 0;
        void ScrollCurrentIntoView(BOOL bDraw = TRUE);
        PegThing *mpLastSelected;
        SIGNED miChildSeperation;

        BOOL    mbNewLast;
};


/*--------------------------------------------------------------------------*/
class PegVertList : public PegList
{
    public:
        PegVertList(const PegRect &Rect, WORD wId = 0,
            WORD wStyle = FF_THIN);

    protected:
        virtual void PositionChildren(void);
        
};
/*--------------------------------------------------------------------------*/
class PegHorzList : public PegList
{
    public:
        PegHorzList(const PegRect &Rect, WORD wId = 0,
            WORD wStyle = FF_THIN);

    protected:
        virtual void PositionChildren(void);
};

/*--------------------------------------------------------------------------*/


#endif


