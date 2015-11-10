/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ptree.hpp - PegTreeView window class definition.
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
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef _PEGTREE_
#define _PEGTREE_

class PegTreeView;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PegTreeNode : public PegTextThing
{
    friend class PegTreeView;

    public:
        PegTreeNode(const PEGCHAR *Text, PegBitmap *map = NULL);
        ~PegTreeNode();

        void Add(PegTreeNode *Child);
        void Insert(PegTreeNode *Sib);
        SIGNED BranchHeight(PegTreeView *Parent);
        SIGNED NodeHeight(PegTreeView *Parent);
        SIGNED BranchWidth(PegTreeView *Parent);
        SIGNED NodeWidth(PegTreeView *Parent);
        void SetMap(PegBitmap *pMap) {mpMap = pMap;}
        PegBitmap *GetMap(void) {return mpMap;}
        PegTreeNode *First(void) {return mpFirst;}
        PegTreeNode *Next(void)  {return mpNext;}
        PegTreeNode *Parent(void) {return mpParent;}
        void SetParent(PegTreeNode *pNode) {mpParent = pNode;}
        PegTreeNode *NodeAbove(void);
        PegTreeNode *NodeBelow(void);
        PegTreeNode *NodeBottom(void);
        void SetFirst(PegTreeNode *pFirst) {mpFirst = pFirst;}
        void SetNext(PegTreeNode *pNext) {mpNext = pNext;}
        void MoveToTop(PegTreeNode *pChild);
        SIGNED Count(void);
        void Open(void) {mbOpen = TRUE;}
        void Close(void) {mbOpen = FALSE;}
        BOOL IsOpen(void) {return mbOpen;}
        void SetSelected(BOOL bSelected){mbSelected = bSelected;}
        BOOL IsSelected(void) {return mbSelected;}

        PegRect mPos;

    protected:
        PegTreeNode *Remove(PegTreeNode *Child);
        void Destroy(PegTreeNode *Child);

        PegTreeNode *mpFirst;
        PegTreeNode *mpNext;
        PegTreeNode *mpParent;

        PegBitmap   *mpMap;
        BOOL mbOpen;
        BOOL mbSelected;
};



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

class PegTreeView : public PegWindow
{
    public:
        PegTreeView(const PegRect &Rect, WORD wStyle, const PEGCHAR *Text,
            PegBitmap *pMap = NULL);
        ~PegTreeView();
        SIGNED Message(const PegMessage &Mesg);
        void Draw(void);

        PegTreeNode *TopNode(void) {return mpTopNode;}
        void Select(PegTreeNode *, BOOL bDraw = TRUE);
        PegTreeNode *GetSelected(void) {return mpSelected;}
        PegTreeNode *FindNode(SIGNED iLevel, PEGCHAR *Text);
        void Reset(const PEGCHAR *Top);

        void SetIndent(SIGNED iVal) {miIndent = iVal;}
        SIGNED GetIndent(void) {return miIndent;}
        void GetVScrollInfo(PegScrollInfo *Put);
        void GetHScrollInfo(PegScrollInfo *Put);

        void ToggleBranch(PegTreeNode *);
        PegTreeNode *RemoveNode(PegTreeNode *Who);
        void DestroyNode(PegTreeNode *Who);
        void DrawNode(PegTreeNode *pStart, PegPoint Put, SIGNED iMaxMapWidth);

    protected:
        BOOL CheckNodeClick(PegPoint Click, PegTreeNode *pStart);
        PegTreeNode *FindSubNode(PegTreeNode *pTop, SIGNED iCurLevel, SIGNED iFindLevel, PEGCHAR *pText);
        PegTreeNode *mpTopNode;
        PegTreeNode *mpSelected;
        SIGNED miTopOffset;
        SIGNED miLeftOffset;
        SIGNED miIndent;
        SIGNED miSearchLevel;
};



#endif

