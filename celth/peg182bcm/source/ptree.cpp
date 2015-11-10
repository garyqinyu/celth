/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ptree.cpp - PegTreeView window class implementation.
//
// Author: Kenneth G. Maxwell
//
// Copyright (c) 1997-2002 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
//
// PegTree view displays a hierachial representation of nodes. The nodes may
// represent files, groups of users, etc.. Each node level may be assigned
// a bitmap image to display, and each node may have a text string associated
// with it.
//
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "string.h"
#include "peg.hpp"

extern PEGCHAR lsTEST[];

/*--------------------------------------------------------------------------*/
PegTreeView::PegTreeView(const PegRect &Rect, WORD wStyle, const PEGCHAR *Text,
    PegBitmap *TopMap) : PegWindow(Rect, wStyle)
{
    Type(TYPE_TREEVIEW);
    RemoveStatus(PSF_SIZEABLE);
    SetScrollMode(WSM_AUTOVSCROLL);
    SetSignals(SIGMASK(PSF_NODE_SELECT)|SIGMASK(PSF_NODE_DELETE));
    mpTopNode = new PegTreeNode(Text, TopMap);
    mpTopNode->Open();

    muColors[PCI_NTEXT] = PCLR_NORMAL_TEXT;
    muColors[PCI_STEXT] = PCLR_HIGH_TEXT;
    muColors[PCI_SELECTED] = PCLR_HIGH_TEXT_BACK;
    muColors[PCI_NORMAL] = PCLR_CLIENT;

    miTopOffset = 0;
    miLeftOffset = 0;
    miIndent = 14;
    mpSelected = NULL;
}



/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegTreeView::~PegTreeView()
{
    PegTreeNode *pNext;
    PegTreeNode *pDel = mpTopNode->First();

    while(pDel)
    {
        pNext = pDel->Next();
        delete pDel;
        pDel = pNext;
    }
    mpTopNode->SetFirst(NULL);
    delete mpTopNode;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegTreeView::Message(const PegMessage &Mesg)
{
    switch(Mesg.wType)
    {
    case PM_LBUTTONDOWN:
        PegWindow::Message(Mesg);
        if (CheckNodeClick(Mesg.Point, mpTopNode))
        {
            if (Id())
            {
                PegMessage NewMessage(Parent(), PEG_SIGNAL(Id(), PSF_NODE_SELECT));
                NewMessage.pSource = this;
                NewMessage.iData = Id();
                NewMessage.pData = GetSelected();
                MessageQueue()->Push(NewMessage);
            }
        }
        break;

    case PM_RBUTTONUP:
        if (mpSelected)
        {
            if (mpSelected->mPos.Contains(Mesg.Point) && Id())
            {
                PegMessage NewMessage(Parent(), PEG_SIGNAL(Id(), PSF_NODE_RCLICK));
                NewMessage.pSource = this;
                NewMessage.iData = Id();
                NewMessage.pData = mpSelected;
                MessageQueue()->Push(NewMessage);                 
            }
        }
        break;

    case PM_VSCROLL:
        miTopOffset = (SIGNED) Mesg.lData;
        Invalidate();
        Draw();
        break;

    case PM_HSCROLL:
        miLeftOffset = (SIGNED) Mesg.lData;
        Invalidate();
        Draw();
        break;

#ifdef PEG_KEYBOARD_SUPPORT
    case PM_GAINED_KEYBOARD:
        if (mpSelected)
        {
            Invalidate(mpSelected->mPos);
            mpSelected->SetSelected(TRUE);
            Draw();
        }
        else
        {
            if (mpTopNode)
            {
                Select(mpTopNode);
            }
        }
        break;

    case PM_LOST_KEYBOARD:
        if (mpSelected)
        {
            Invalidate(mpSelected->mPos);
            mpSelected->SetSelected(FALSE);
            Draw();
        }
        break;

    case PM_KEY:
        switch(Mesg.iData)
        {
        case PK_DELETE:
            if (Id() && GetSelected())
            {
                PegMessage NewMessage(Parent(), PEG_SIGNAL(Id(), PSF_NODE_DELETE));
                NewMessage.pSource = this;
                NewMessage.iData = Id();
                NewMessage.pData = GetSelected();
                MessageQueue()->Push(NewMessage);
            }
            break;

        case PK_LNUP:
            if (GetSelected())
            {
                PegTreeNode *pAbove = GetSelected()->NodeAbove();

                if (pAbove)
                {
                    Invalidate(GetSelected()->mPos);
                    Select(pAbove);
                    Invalidate(pAbove->mPos);
                    Draw();
                    if (Id())
                    {
                        PegMessage NewMessage(Parent(), PEG_SIGNAL(Id(), PSF_NODE_SELECT));
                        NewMessage.pSource = this;
                        NewMessage.iData = Id();
                        NewMessage.pData = GetSelected();
                        MessageQueue()->Push(NewMessage);
                    }
                }
               #if !defined(PEG_TAB_KEY_SUPPORT)
                else
                {
                    return PegThing::Message(Mesg);
                }
               #endif
            }
            break;

        case PK_LNDN:
            if (GetSelected())
            {
                PegTreeNode *pBelow = GetSelected()->NodeBelow();

                if (pBelow)
                {
                    Invalidate(GetSelected()->mPos);
                    Select(pBelow);
                    Invalidate(pBelow->mPos);
                    Draw();

                    if (Id())
                    {
                        PegMessage NewMessage(Parent(), PEG_SIGNAL(Id(), PSF_NODE_SELECT));
                        NewMessage.pSource = this;
                        NewMessage.iData = Id();
                        NewMessage.pData = GetSelected();
                        MessageQueue()->Push(NewMessage);
                    }
                }
               #if !defined(PEG_TAB_KEY_SUPPORT)
                else
                {
                    return PegThing::Message(Mesg);
                }
               #endif
            }
            break;

        case PK_RIGHT:
            if (GetSelected())
            {
                if (!GetSelected()->IsOpen())
                {
                    ToggleBranch(GetSelected());
                }
               #if !defined(PEG_TAB_KEY_SUPPORT)
                else
                {
                    return PegThing::Message(Mesg);
                }
               #endif
            }
            break;

        case PK_LEFT:
            if (GetSelected())
            {
                if (GetSelected()->IsOpen())
                {
                    ToggleBranch(GetSelected());
                }
               #if !defined(PEG_TAB_KEY_SUPPORT)
                else
                {
                    return PegThing::Message(Mesg);
                }
               #endif
            }
            break;

        case PK_CR:
        case PK_SPACE:
            if (GetSelected())
            {
                ToggleBranch(GetSelected());
            }
            break;

        default:
            PegWindow::Message(Mesg);
            break;
        }
        break;
#endif

    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTreeView::Draw(void)
{
    PegPoint Put;
    Put.x = mClient.wLeft  + 2 - miLeftOffset;
    Put.y = mClient.wTop + 2 - miTopOffset;
    BeginDraw();
    PegWindow::Draw();
    PegRect OldClip = mClip;
    mClip &= mClient;

    if (mpTopNode->GetMap())
    {
        DrawNode(mpTopNode, Put, mpTopNode->GetMap()->wWidth + 2);
    }
    else
    {
        DrawNode(mpTopNode, Put, 2);
    }
    mClip = OldClip;
    EndDraw();
}

/*--------------------------------------------------------------------------*/
void PegTreeView::Reset(const PEGCHAR *Top)
{
    PegTreeNode *pNext;
    PegTreeNode *pDel = mpTopNode->First();

    while(pDel)
    {
        pNext = pDel->Next();
        delete pDel;
        pDel = pNext;
    }
    mpTopNode->SetFirst(NULL);
    mpTopNode->SetNext(NULL);
    mpTopNode->DataSet(Top);
    mpSelected = NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTreeView::DrawNode(PegTreeNode *pStart, PegPoint UL, SIGNED iMapWidth)
{
  #ifdef PEG_RUNTIME_COLOR_CHECK
    PegColor Color(LIGHTGRAY, LIGHTGRAY, CF_NONE);
    if (NumColors() < 4)
    {
        Color.Set(WHITE, WHITE, CF_NONE);
    }
  #else
   #if (PEG_NUM_COLORS >= 4)
    PegColor Color(LIGHTGRAY, LIGHTGRAY, CF_NONE);
   #else
    PegColor Color(WHITE, WHITE, CF_NONE);
   #endif
  #endif

    // Determine the vertical center
    SIGNED iNodeHeight = pStart->NodeHeight(this);
    SIGNED iYOffsetBitmap = 1, iYOffsetText = 1;
    if(pStart->GetMap() && pStart->DataGet())
    {
        SIGNED iMapHeight = pStart->GetMap()->wHeight;
        SIGNED iTextHeight = TextHeight(lsTEST, pStart->GetFont());

        if((iNodeHeight - iMapHeight) >= 2)
        {
            iYOffsetBitmap = (iNodeHeight - iMapHeight) / 2;
        }

        if((iNodeHeight - iTextHeight) >= 2)
        {
            iYOffsetText = (iNodeHeight - iTextHeight) / 2;
        }
    }
    
    PegPoint Current;

    // draw the bitmap:

    SIGNED iThisWidth = 0;
    if (pStart->GetMap())
    {
        Current = UL;
        iThisWidth = pStart->GetMap()->wWidth;
        Current.x += (iMapWidth - iThisWidth) / 2;
        Current.y += iYOffsetBitmap;
        Bitmap(Current, pStart->GetMap());
    }

    // draw the text:

    if (pStart->DataGet())
    {
        Current = UL;
        Current.x += iMapWidth + 2;
        Current.y += iYOffsetText;
        if (pStart->IsSelected())
        {
            Color.Set(muColors[PCI_STEXT], muColors[PCI_SELECTED], CF_FILL);
        }
        else
        {
            if (pStart == mpSelected)
            {
                Color.Set(muColors[PCI_STEXT], PCLR_LOWLIGHT, CF_FILL);
            }
            else
            {
                Color.Set(muColors[PCI_NTEXT], muColors[PCI_NORMAL], CF_NONE);
            }
        }
        DrawText(Current, pStart->DataGet(), Color, pStart->GetFont());
    }

    //pStart->mPos.Set(UL.x, UL.y, UL.x + pStart->NodeWidth(this),
    //    UL.y + pStart->NodeHeight(this));

    SIGNED iNodeWidth = pStart->NodeWidth(this);
    if(iMapWidth > iThisWidth)
    {
        iNodeWidth += (iMapWidth - iThisWidth);
    }
    pStart->mPos.Set(UL.x, UL.y, UL.x + iNodeWidth,
            UL.y + pStart->NodeHeight(this));

    if (!pStart->IsOpen())
    {
        return;
    }

    PegPoint LinePoint = UL;
    LinePoint.y += pStart->NodeHeight(this);
    LinePoint.x += 5;

  #ifdef PEG_RUNTIME_COLOR_CHECK
    if (NumColors() >= 4)
    {
        Color.Set(LIGHTGRAY, LIGHTGRAY, CF_NONE);
    }
    else
    {
        Color.Set(WHITE, WHITE, CF_NONE);
    }
  #else
   #if (PEG_NUM_COLORS >= 4)
    Color.Set(LIGHTGRAY, LIGHTGRAY, CF_NONE);
   #else
    Color.Set(WHITE, WHITE, CF_NONE);
   #endif
  #endif

    PegTreeNode *pTest = pStart->First();
    WORD iMaxMapWidth = 0;

    while(pTest)
    {
        PegBitmap *pMap = pTest->GetMap();
        if (pMap)
        {
            if (pMap->wWidth > iMaxMapWidth)
            {
                iMaxMapWidth = pMap->wWidth;
            }
        }
        pTest = pTest->Next();
    }
    iMaxMapWidth += 2;
                
    pTest = pStart->First();
    while(pTest)
    {
        SIGNED iNodeHeight = pTest->NodeHeight(this);
        SIGNED iBranchHeight = pTest->BranchHeight(this);
      #ifdef PEG_RUNTIME_COLOR_CHECK
        if (NumColors() >= 4)
        {
            Color.uForeground = LIGHTGRAY;
        }
        else
        {
            Color.uForeground = WHITE;
        }
      #else
       #if (PEG_NUM_COLORS >= 4)
        Color.uForeground = LIGHTGRAY;
       #else
        Color.uForeground = BLACK;
       #endif
      #endif

        if (pTest->First())
        {
            PegRect BoxRect;
            BoxRect.wLeft = LinePoint.x - 4;
            BoxRect.wRight = LinePoint.x + 4;
            BoxRect.wTop = LinePoint.y + (iNodeHeight / 2) - 4;
            BoxRect.wBottom = BoxRect.wTop + 8;
            Line(LinePoint.x, LinePoint.y, LinePoint.x, BoxRect.wTop - 1, Color);
            Rectangle(BoxRect, Color);

            Line(BoxRect.wRight + 1, BoxRect.wTop + 5, BoxRect.wRight + miIndent - 4,
                 BoxRect.wTop + 5, Color);

            if (pTest->Next())
            {
                Line(LinePoint.x, BoxRect.wBottom + 1, LinePoint.x,
                    LinePoint.y + iBranchHeight, Color);
            }

          #ifdef PEG_RUNTIME_COLOR_CHECK
            if (NumColors() >= 16)
            {
                Color.uForeground = RED;
            }
            else
            {
                Color.uForeground = BLACK;
            }
          #else
           #if (PEG_NUM_COLORS >= 16)
            Color.uForeground = RED;
           #else
            Color.uForeground = BLACK;
           #endif
          #endif
           
            Line(BoxRect.wLeft + 2, (BoxRect.wTop + BoxRect.wBottom) / 2,
                 BoxRect.wRight - 2, (BoxRect.wTop + BoxRect.wBottom) / 2, Color);
            
            if (!pTest->IsOpen())
            {
                Line((BoxRect.wLeft + BoxRect.wRight) / 2, BoxRect.wTop + 2,
                    (BoxRect.wLeft + BoxRect.wRight) / 2, BoxRect.wBottom - 2, Color);
            }
        }
        else
        {
            if (pTest->Next())
            {
                Line(LinePoint.x, LinePoint.y, LinePoint.x,
                    LinePoint.y + iBranchHeight, Color);
            }
            else
            {
                Line(LinePoint.x, LinePoint.y, LinePoint.x,
                    LinePoint.y + (iNodeHeight / 2), Color);
            }
            Line(LinePoint.x, LinePoint.y + iNodeHeight / 2,
                LinePoint.x + miIndent,
                LinePoint.y + iNodeHeight / 2, Color);
        }

        PegPoint PutChild = LinePoint;
        PutChild.x += miIndent;
        DrawNode(pTest, PutChild, iMaxMapWidth);
        LinePoint.y += iBranchHeight;
        pTest = pTest->Next();
    }
}
    

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTreeView::ToggleBranch(PegTreeNode *pBranch)
{
    if (pBranch->IsOpen())
    {
        pBranch->Close();

        if (mpHScroll)
        {
            if (mpTopNode->BranchWidth(this) <= mClient.Width() - PEG_SCROLL_WIDTH)
            {
                CheckAutoScroll();
                miLeftOffset = 0;
            }
            else
            {
                mpHScroll->Reset();
            }
        }
        if (mpVScroll)
        {
            if (mpTopNode->BranchHeight(this) <= mClient.Height() - PEG_SCROLL_WIDTH)
            {
                CheckAutoScroll();
                miTopOffset = 0;
            }
            else
            {
                mpVScroll->Reset();
            }
        }
    }
    else
    {
        pBranch->Open();

        if (!mpHScroll)
        {
            if (mpTopNode->BranchWidth(this) > mClient.Width() - PEG_SCROLL_WIDTH)
            {
                CheckAutoScroll();
            }
        }
        else
        {
            mpHScroll->Reset();
        }

        if (!mpVScroll)
        {
            if (mpTopNode->BranchHeight(this) > mClient.Height() - PEG_SCROLL_WIDTH)
            {
                CheckAutoScroll();
            }
        }
        else
        {
            mpVScroll->Reset();
        }
    }
    
    Invalidate();
    Draw();

    if (Id() && Parent())
    {
        PegMessage NewMessage;
        NewMessage.pTarget = Parent();
        NewMessage.pSource = this;
        NewMessage.iData = Id();
        NewMessage.pData = pBranch;

        if (pBranch->IsOpen())
        {
            NewMessage.wType = PEG_SIGNAL(Id(), PSF_NODE_OPEN);
        }
        else
        {
            NewMessage.wType = PEG_SIGNAL(Id(), PSF_NODE_CLOSE);
        }
        MessageQueue()->Push(NewMessage);
    }
}

/*--------------------------------------------------------------------------*/
void PegTreeView::GetVScrollInfo(PegScrollInfo *Put)
{
    Put->wVisible = mClient.Height();
    Put->wMin = 0;
    Put->wCurrent = miTopOffset;
    Put->wMax = mpTopNode->BranchHeight(this) + 4;
    Put->wStep = mClient.Height() / 10;
}

/*--------------------------------------------------------------------------*/
void PegTreeView::GetHScrollInfo(PegScrollInfo *Put)
{
    Put->wVisible = mClient.Width();
    Put->wMin = 0;
    Put->wCurrent = miLeftOffset;
    Put->wMax = mpTopNode->BranchWidth(this);
    Put->wStep = mClient.Width() / 10;
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTreeView::Select(PegTreeNode *pFocus, BOOL bDraw)
{
    // kill focus on other nodes:
    if (mpSelected)
    {
        mpSelected->SetSelected(FALSE);
        Invalidate(mpSelected->mPos);
    }
    pFocus->SetSelected(TRUE);
    mpSelected = pFocus;
    Invalidate(pFocus->mPos);

    if (mpVScroll)
    {
        if (pFocus->mPos.wTop < mClient.wTop && miTopOffset)
        {
            miTopOffset -= mClient.wTop - pFocus->mPos.wTop;
            mpVScroll->Reset();
            Invalidate();
        }
        else
        {
            if (pFocus->mPos.wBottom > mClient.wBottom)
            {
                miTopOffset += pFocus->mPos.wBottom - mClient.wBottom;
                mpVScroll->Reset();
                Invalidate();
            }
        }
    }

    if (bDraw && StatusIs(PSF_VISIBLE))
    {
        Draw();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegTreeNode *PegTreeView::RemoveNode(PegTreeNode *pWho)
{
    PegTreeNode *pMoveFocusTo = NULL;

    if (pWho->Parent())
    {
        if (pWho->IsSelected())
        {
            pMoveFocusTo = pWho->NodeBelow();
            if (!pMoveFocusTo)
            {
                pMoveFocusTo = pWho->NodeAbove();
            }
            mpSelected = NULL;
        }
        pWho->Parent()->Remove(pWho);
        if (pMoveFocusTo)
        {
            Select(pMoveFocusTo, FALSE);
        }
        Invalidate();
        return (pWho);
    }
    return NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTreeView::DestroyNode(PegTreeNode *pWho)
{
    pWho = RemoveNode(pWho);

    if (pWho)
    {
        delete pWho;
    }
    if (StatusIs(PSF_VISIBLE))
    {
        Invalidate();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL PegTreeView::CheckNodeClick(PegPoint Click, PegTreeNode *pStart)
{
    PegTreeNode *pNode = pStart;

    while(pNode)
    {
        if (pNode->mPos.Contains(Click))
        {
            Select(pNode, TRUE);
            mpSelected = pNode;
            return TRUE;
        }
        else
        {
            if (pNode->First())
            {
                // check for click on the box:

                PegRect BoxRect;
                BoxRect.wLeft = pNode->mPos.wLeft - miIndent - 4;
                BoxRect.wTop = ((pNode->mPos.wTop + pNode->mPos.wBottom) / 2) - 4;
                BoxRect.wBottom = BoxRect.wTop + 8;
                BoxRect.wRight = BoxRect.wLeft + 8;

                if (BoxRect.Contains(Click))
                {
                    ToggleBranch(pNode);
                    //mpSelected = NULL;
                    return FALSE;
                }
                if (pNode->IsOpen())
                {
                    if (CheckNodeClick(Click, pNode->First()))
                    {
                        return TRUE;
                    }
                }
            }
        }
        pNode = pNode->Next();
    }
    return FALSE;
}


/*--------------------------------------------------------------------------*/
PegTreeNode *PegTreeView::FindNode(SIGNED iLevel, PEGCHAR *pText)
{
	return FindSubNode(mpTopNode, 1, iLevel, pText);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegTreeNode *PegTreeView::FindSubNode(PegTreeNode *pStart, SIGNED iCurLevel,
    SIGNED iTargetLevel, PEGCHAR *pText)
{
    PegTreeNode *pCurrent = pStart->First();
    PegTreeNode *pReturn;
    PEGCHAR *pCurText;

    while(pCurrent)
    {
        if (!iTargetLevel || iCurLevel == iTargetLevel)
        {
            pCurText = pCurrent->DataGet();
            if (pText && pCurText)
            {
                if (!strcmp(pCurText, pText))
                {
                    return (pCurrent);
                }
            }
        }

        if (!iTargetLevel || iCurLevel < iTargetLevel)
        {
            pReturn = FindSubNode(pCurrent, iCurLevel + 1, iTargetLevel, pText);
            if (pReturn)
            {
                return pReturn;
            }
        }
        pCurrent = pCurrent->Next();
    }
    return NULL;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegTreeNode::PegTreeNode(const PEGCHAR *Text, PegBitmap *map) :
    PegTextThing(Text, TT_COPY, PEG_TREEVIEW_FONT)
{
    mpMap = map;
    mpNext = NULL;
    mpFirst = NULL;
    mpParent = NULL;
    mbOpen = FALSE;
    mbSelected = FALSE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegTreeNode::~PegTreeNode()
{
    // delete any children:
    PegTreeNode *pDel = First();
    PegTreeNode *pNext;

    while(pDel)
    {
        pNext = pDel->Next();
        delete pDel;
        pDel = pNext;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTreeNode::Add(PegTreeNode *Child)
{
    // If I allready have this child, unlink it:

    PegTreeNode *pTest = First();
    PegTreeNode *pLast = NULL;

    while(pTest)
    {
        if (pTest == Child)
        {
            if (pLast)
            {
                pLast->SetNext(pTest->Next());
            }
            else
            {
                SetFirst(pTest->Next());
            }
            break;
        }
        pLast = pTest;
        pTest = pTest->Next();
    }

    if (First())
    {
        pTest = First();
        while(pTest->Next())
        {
            pTest = pTest->Next();
        }
        pTest->SetNext(Child);
    }
    else
    {
        SetFirst(Child);
    }
    Child->SetParent(this);
    Child->SetNext(NULL);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTreeNode::MoveToTop(PegTreeNode *pChild)
{
    // find this child:
    if (pChild == First())
    {
        return;
    }
    PegTreeNode *pTest = First();

    while(pTest)
    {
        if (pTest->Next() == pChild)
        {
            pTest->SetNext(pChild->Next());
            pChild->SetNext(First());
            SetFirst(pChild);
            return;
        }
        pTest = pTest->Next();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTreeNode::Insert(PegTreeNode *Sib)
{
    PegTreeNode *pTest = Parent()->First();

    if (pTest == this)
    {
        Parent()->SetFirst(Sib);
    }
    else
    {
        while(pTest->Next() != this)
        {
            pTest = pTest->Next();
        }
        pTest->SetNext(Sib);
    }
    Sib->SetNext(this);
    Sib->SetParent(Parent());
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegTreeNode *PegTreeNode::Remove(PegTreeNode *Child)
{
    PegTreeNode *pTest = First();
    PegTreeNode *pLast = pTest;

    while(pTest)
    {
        if (pTest == Child)
        {
            if (pTest == First())
            {
                SetFirst(pTest->Next());
            }
            else
            {
                pLast->SetNext(pTest->Next());
            }
            return (pTest);
        }   
        pLast = pTest;
        pTest = pTest->Next();
    }
    return (pTest);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void PegTreeNode::Destroy(PegTreeNode *Child)
{
    PegTreeNode *pTest = Remove(Child);

    if (pTest)
    {
        delete(pTest);
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegTreeNode *PegTreeNode::NodeAbove(void)
{
    if (mpParent)
    {
        if (mpParent->First() != this)
        {
            PegTreeNode *pTest = mpParent->First();
        
            while(pTest->Next() != this)
            {
                pTest = pTest->Next();
            }
            return (pTest->NodeBottom());
        }
        else
        {
            return(mpParent);
        }
    }
    else
    {
        return mpParent;
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegTreeNode *PegTreeNode::NodeBelow(void)
{
    if (mbOpen && mpFirst)
    {
        return mpFirst;
    }
    if (mpNext)
    {
        return mpNext;
    }
    else
    {
        PegTreeNode *pTest = mpParent;

        while(pTest)
        {
            if (pTest->Next())
            {
                return (pTest->Next());
            }
            pTest = pTest->mpParent;
        }
    }
    return NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegTreeNode *PegTreeNode::NodeBottom()
{
    if (!mbOpen || !mpFirst)
    {
        return this;
    }
    PegTreeNode *pTest = mpFirst;

    while(pTest->Next())
    {
        pTest = pTest->Next();
    }
    return(pTest->NodeBottom());
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegTreeNode::BranchHeight(PegTreeView *Parent)
{
    SIGNED iHeight = NodeHeight(Parent);
    if (mbOpen)
    {
        PegTreeNode *pTest = First();

        while(pTest)
        {
            iHeight += pTest->BranchHeight(Parent);
            pTest = pTest->Next();
        }
    }
    return(iHeight);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegTreeNode::NodeHeight(PegTreeView *Parent)
{
    SIGNED iHeight = Parent->TextHeight(lsTEST, mpFont);

    // my height:

    if (mpMap)
    {
        if (mpMap->wHeight > (WORD) iHeight)
        {
            iHeight = mpMap->wHeight;
        }
    }
    return(iHeight + 2);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegTreeNode::BranchWidth(PegTreeView *Parent)
{
	 SIGNED iWidth1 = NodeWidth(Parent) + Parent->GetIndent();
	 SIGNED iWidth2;

	 if (mbOpen)
    {
        PegTreeNode *pTest = First();

        while(pTest)
        {
            iWidth2 = Parent->GetIndent() + pTest->BranchWidth(Parent);
            if (iWidth2 > iWidth1)
            {
                iWidth1 = iWidth2;
            }
            pTest = pTest->Next();
        }
    }
    return(iWidth1);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegTreeNode::NodeWidth(PegTreeView *Parent)
{
    SIGNED iWidth = 0;

    if (mpFont)
    {
        iWidth = Parent->TextWidth(mpText, mpFont) + 4;
    }
    if (mpMap)
    {
        iWidth += mpMap->wWidth + 2;
    }
    return iWidth;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED PegTreeNode::Count(void)
{
    PegTreeNode *pTest = First();
    SIGNED iCount = 0;

    while(pTest)
    {
        iCount++;
        pTest = pTest->Next();
    }
    return(iCount);
}

/*--------------------------------------------------------------------------*/
/*
$Workfile: ptree.cpp $
$Author: Ken $
$Date: 9/04/:2 2:27p $
$Revision: 8 $
$Log: /peg/source/ptree.cpp $
// 
// 8     9/04/:2 2:27p Ken
// 
// 7     8/01/:2 5:53p Jim
// Added keyword expansion
*/
/*--------------------------------------------------------------------------*/

// End of file

