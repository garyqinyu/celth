/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// plist.cpp - PegList, PegVertList, and PegHorzList class implementations. 
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

#include "peg.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// PegList class implementation.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegList::PegList(const PegRect &Rect, WORD wId, WORD wStyle) :
    PegWindow(Rect, wStyle)
{
    Id(wId);
    Type(TYPE_LIST);
    SetSignals(SIGMASK(PSF_LIST_SELECT));
    AddStatus(PSF_TAB_STOP|PSF_KEEPS_CHILD_FOCUS);
    mpLastSelected = NULL;
    miChildSeperation = 1;

    mbNewLast = FALSE;
}


/*--------------------------------------------------------------------------*/
// Destructor- Remove and delete any children.
/*--------------------------------------------------------------------------*/
PegList::~PegList()
{
}

/*--------------------------------------------------------------------------*/
SIGNED PegList::Message(const PegMessage &Mesg)
{
    PegThing *pChild;

    if (Mesg.wType >= FIRST_SIGNAL &&
        Mesg.wType < FIRST_USER_MESSAGE)
    {
        // check for a 'clicked' signal from one of my children,
        // if it is one then send a list select message to 
        // my parent:

        if ((Mesg.wType & ALL_SIGNALS_MASK) == PSF_FOCUS_RECEIVED)
        {
            if (mpLastSelected != Mesg.pSource)
            {
                if (mpLastSelected)
                {
                    if (mpLastSelected->StatusIs(PSF_CURRENT))
                    {
                        mpLastSelected->Message(PegMessage(PM_NONCURRENT));
                    }
                }

                mpLastSelected = First();

                while(mpLastSelected)
                {
                    if (mpLastSelected == Mesg.pSource)
                    {
                        break;
                    }
                    mpLastSelected = mpLastSelected->Next();
                }

                if (mpLastSelected)
                {
                    if (Type() != TYPE_COMBO)
                    {
                        ScrollCurrentIntoView();
                    }
           
                    if (Id() && (GetSignals() & SIGMASK(PSF_LIST_SELECT)))
                    {
                        PegMessage NewMessage(PEG_SIGNAL(Id(), PSF_LIST_SELECT));
                        NewMessage.pSource = this;
                        NewMessage.iData = mpLastSelected->Id();
                        NewMessage.pTarget = Parent();
                        MessageQueue()->Push(NewMessage);
                    }
                }
            }
            else if(mbNewLast)
            {
                mbNewLast = FALSE;
       
                if (Id() && (GetSignals() & SIGMASK(PSF_LIST_SELECT)))
                {
                    PegMessage NewMessage(PEG_SIGNAL(Id(), PSF_LIST_SELECT));
                    NewMessage.pSource = this;
                    NewMessage.iData = mpLastSelected->Id();
                    NewMessage.pTarget = Parent();
                    MessageQueue()->Push(NewMessage);
                }
            }

            if (Type() == TYPE_COMBO)
            {
                return PSF_FOCUS_RECEIVED;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (Parent())
            {
                Parent()->Message(Mesg);
                return 0;
            }
        }
    }
    
    switch (Mesg.wType)
    {
    case PM_SHOW:
        PositionChildren();
        if (!mpLastSelected)
        {
            SetSelected((SIGNED) 0);
        }
        if (mpLastSelected)
        {
            ScrollCurrentIntoView();

            if (StatusIs(PSF_CURRENT) && !mpLastSelected->StatusIs(PSF_CURRENT))
            {
                Presentation()->MoveFocusTree(mpLastSelected);
            }
        }
        PegWindow::Message(Mesg);
        break;

    case PM_CURRENT:
        PegThing::Message(Mesg);

        if (mpLastSelected)
        {
            if (!mpLastSelected->StatusIs(PSF_CURRENT))
            {
                // See if one of my children is current:

                pChild = First();

                while(pChild)
                {
                    if (pChild->StatusIs(PSF_CURRENT) && 
                        !pChild->StatusIs(PSF_NONCLIENT))
                    {
                        mpLastSelected = pChild;
                        mbNewLast = TRUE;
                        break;
                    }
                    pChild = pChild->Next();
                }
                
                if (!pChild)
                {
                    Presentation()->MoveFocusTree(mpLastSelected);
                }
            }
        }
        break;

   #ifdef PEG_KEYBOARD_SUPPORT

    case PM_KEY:
        if (!mpLastSelected)
        {
            return PegWindow::Message(Mesg);
        }
        
        switch(Mesg.iData)
        {
        case PK_LNUP:
            if (Type() != TYPE_HLIST)
            {
                SelectPrevious();
            }
            else
            {
                return (PegWindow::Message(Mesg));
            }
            break;

        case PK_LEFT:
            if (Type() == TYPE_HLIST)
            {
                SelectPrevious();
            }
            else
            {
                return (PegWindow::Message(Mesg));
            }
            break;

        case PK_LNDN:
            if (Type() != TYPE_HLIST)
            {
                SelectNext();
            }
            else
            {
                return (PegWindow::Message(Mesg));
            }
            break;

        case PK_RIGHT:
            if (Type() == TYPE_HLIST)
            {
                SelectNext();
            }
            else
            {
                return (PegWindow::Message(Mesg));
            }
            break;

        case PK_PGUP:
            PageUp();
            break;

        case PK_PGDN:
            PageDown();       
            break;

        default:
            return (PegWindow::Message(Mesg));
        }
        break;

   #endif

    default:
        return(PegWindow::Message(Mesg));
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
void PegList::Add(PegThing *What, BOOL bDraw)
{
    if (What->StatusIs(PSF_NONCLIENT))
    {
        PegWindow::Add(What, bDraw);
        return;
    }

    if (StatusIs(PSF_VISIBLE))
    {
        if (What->StatusIs(PSF_VISIBLE))
        {
            if (What->Parent() == this)
            {
                return;
            }
        }

        PegThing::Add(What, FALSE);
        PositionChildren();
        PegWindow::CheckAutoScroll();
        if (bDraw)
        {
            Draw();
        }
    }
    else
    {
        PegThing::Add(What, bDraw);
    }
}

/*--------------------------------------------------------------------------*/
void PegList::AddToEnd(PegThing *What, BOOL bDraw)
{
    if (What->StatusIs(PSF_NONCLIENT))
    {
        PegWindow::AddToEnd(What, bDraw);
        return;
    }

    if (StatusIs(PSF_VISIBLE))
    {
        if (What->StatusIs(PSF_VISIBLE))
        {
            if (What->Parent() == this)
            {
                return;
            }
        }

        PegThing::AddToEnd(What, FALSE);
        PositionChildren();
        PegWindow::CheckAutoScroll();
        if (bDraw)
        {
            Draw();
        }
    }
    else
    {
        PegThing::AddToEnd(What, bDraw);
    }
}

void PegList::Insert(PegThing *What, SIGNED Where, BOOL bSelect, BOOL bDraw)
{
	if(What->StatusIs(PSF_NONCLIENT))
	{
        PegWindow::AddToEnd(What, bDraw);
		return;
	}
	if(Where > GetNumItems())
	{
		AddToEnd(What, bDraw);
		if(StatusIs(PSF_VISIBLE) && bSelect)
		{
			SetSelected(What);
		}
		return;
	}
	if(Where < 0)
	{
		Add(What, bDraw);
		if(StatusIs(PSF_VISIBLE) && bSelect)
		{
			SetSelected(What);
		}
		return;
	}

	BOOL bVisible = StatusIs(PSF_VISIBLE);
	PegThing *pParent = Parent();
    if (bVisible)
    {
        if (What->StatusIs(PSF_VISIBLE))
        {
            if (What->Parent() == this)
            {
                return;
            }
        }
		pParent->Remove(this, FALSE);
	}

	PegThing *pt = First();
	PegThing *pt2;
	PegThing **temp = new PegThing*[GetNumItems() - Where];
	int index = 0, count = 0;
		
	while(pt)
	{
		pt2 = pt->Next();
		if(!(pt->StatusIs(PSF_NONCLIENT)))
		{
			if(index >= Where)
			{
				Remove(pt, FALSE);
				temp[count++] = pt;
			}
			index++;
		}
		pt = pt2;
	}

    PegThing::AddToEnd(What, FALSE);
	for(int i = 0; i < count; i++)
	{
		PegThing::AddToEnd(temp[i], FALSE);
	}

	delete [] temp;
		
	if(bVisible)
	{
		if(bSelect)
		{
			SetSelected(What);
		}
		pParent->Add(this, TRUE);
		PositionChildren();
        PegWindow::CheckAutoScroll();
        if (bDraw)
        {
            Draw();
        }
    }
}

/*--------------------------------------------------------------------------*/
PegThing *PegList::Remove(PegThing *What, BOOL bDraw)
{
    if (What->StatusIs(PSF_NONCLIENT))
    {
        return (PegWindow::Remove(What, bDraw));
    }
    if (What == mpLastSelected)
    {
        mpLastSelected = NULL;
    }
    if (StatusIs(PSF_VISIBLE))
    {
        PegThing *pReturn = PegWindow::Remove(What, FALSE);
        PositionChildren();
        PegWindow::CheckAutoScroll();
        if (bDraw)
        {
            Draw();
        }
        return pReturn;
    }
    else
    {
        return (PegWindow::Remove(What, bDraw));
    }
}

/*--------------------------------------------------------------------------*/
void PegList::ScrollCurrentIntoView(BOOL bDraw)
{
    if (!mpLastSelected)
    {
        return;
    }

    if (Type() == TYPE_HLIST)
    {
        if (mpLastSelected->mReal.wLeft < mClient.wLeft)
        {
            MoveClientObjects(mClient.wLeft - mpLastSelected->mReal.wLeft, 0, bDraw);

            if (mpHScroll)
            {
                mpHScroll->Reset();

                if (bDraw)
                {
                    mpHScroll->Draw();
                }
            }
        }
        if (mpLastSelected->mReal.wRight > mClient.wRight)
        {   
            MoveClientObjects(mClient.wRight - mpLastSelected->mReal.wRight, 0, bDraw);
            if (mpHScroll)
            {   
                mpHScroll->Reset();

                if (bDraw)
                {
                    mpHScroll->Draw();
                }
            }
        }
        return;
    }

    if (Type() != TYPE_COMBO || mClient.wTop < mReal.wBottom)
    {
        if (mpLastSelected->mReal.wTop < mClient.wTop)
        {
            MoveClientObjects(0, mClient.wTop - mpLastSelected->mReal.wTop, bDraw);
            if (mpVScroll)
            {
                mpVScroll->Reset();

                if (bDraw)
                {
                    mpVScroll->Draw();
                }
            }
        }
        if (mpLastSelected->mReal.wBottom > mClient.wBottom)
        {   
            MoveClientObjects(0, mClient.wBottom - mpLastSelected->mReal.wBottom, bDraw);
            if (mpVScroll)
            {   
                mpVScroll->Reset();

                if (bDraw)
                {
                    mpVScroll->Draw();
                }
            }
        }
    }
}


/*--------------------------------------------------------------------------*/
SIGNED PegList::GetIndex(PegThing *Who)
{
    PegThing *pTest = First();
    SIGNED iIndex = 0;

    if (!Who)
    {
        return -1;
    }

    while(pTest)
    {
        if (pTest == Who)
        {
            return iIndex;
        }
        iIndex++;
        pTest = pTest->Next();
    }
    return -1;
}


/*--------------------------------------------------------------------------*/
SIGNED PegList::GetSelectedIndex(void)
{
    PegThing *pTest = First();
    SIGNED iIndex = 0;

    if (!mpLastSelected)
    {
        return -1;
    }

    while (pTest)
    {
        if (!(pTest->StatusIs(PSF_NONCLIENT)))
        {
            if (pTest == mpLastSelected)
            {
                return iIndex;
            }
            iIndex++;
        }
        pTest = pTest->Next();
    }
    return -1;    
}


/*--------------------------------------------------------------------------*/
PegThing *PegList::SelectNext(void)
{
    if (!mpLastSelected)
    {
        return NULL;
    }
    PegThing *pTest = mpLastSelected;

    while(pTest->Next())
    {
        pTest = pTest->Next();
        if (!pTest->StatusIs(PSF_NONCLIENT))
        {
            SetSelected(pTest);
            return pTest;
        }
    }
    if (Style() & LS_WRAP_SELECT)
    {
        if (First() != pTest)
        {
            pTest = First();

            while(pTest)
            {
                if (!pTest->StatusIs(PSF_NONCLIENT))
                {
                    SetSelected(pTest);
                    return pTest;
                }
                pTest = pTest->Next();
            }
        }
    }

	if(!mpLastSelected->StatusIs(PSF_CURRENT))
	{
		Presentation()->MoveFocusTree(mpLastSelected);
	}

    return NULL;
}


/*--------------------------------------------------------------------------*/
PegThing *PegList::SelectPrevious(void)
{
    if (!mpLastSelected)
    {
        return NULL;
    }

    PegThing *pTest = mpLastSelected;

    while(pTest->Previous())
    {
        pTest = pTest->Previous();
        if (!pTest->StatusIs(PSF_NONCLIENT))
        {
            SetSelected(pTest);
            return pTest;
        }
    }
    if (Style() & LS_WRAP_SELECT)
    {
        if (pTest->Next())
        {
            // go to the end:
            while(pTest->Next())
            {
               pTest = pTest->Next();
            }

            while(pTest)
            {
                if (!pTest->StatusIs(PSF_NONCLIENT))
                {
                    SetSelected(pTest);
                    return pTest;
                }
                pTest = pTest->Previous();
            }
        }
    }

	if(!mpLastSelected->StatusIs(PSF_CURRENT))
	{
		Presentation()->MoveFocusTree(mpLastSelected);
	}

    return NULL;
}

/*--------------------------------------------------------------------------*/
PegThing *PegList::PageDown(void)
{
    PegThing *pTest = mpLastSelected;

    if (!pTest)
    {
        return NULL;
    }

    WORD wMeasure = 0;

    if (pTest)
    {
        if (Type() == TYPE_HLIST)
        {
            wMeasure = pTest->mReal.Width();

            while(pTest->Next() && wMeasure < mClient.Width())
            {
                pTest = pTest->Next();
                if (pTest->StatusIs(PSF_NONCLIENT))
                {
                    if (pTest->Next())
                    {
                        pTest = pTest->Next();
                    }
                    else
                    {
                        pTest = pTest->Previous();
                        break;
                    }
                }
                wMeasure += pTest->mReal.Width() + miChildSeperation;
            }
        }
        else
        {
            wMeasure = pTest->mReal.Height();
            while(pTest->Next() && wMeasure < mClient.Height())
            {
                pTest = pTest->Next();
                if (pTest->StatusIs(PSF_NONCLIENT))
                {
                    if (pTest->Next())
                    {
                        pTest = pTest->Next();
                    }
                    else
                    {
                        pTest = pTest->Previous();
                        break;
                    }
                }
                wMeasure += pTest->mReal.Height() + miChildSeperation;
            }
        }
    }

    SetSelected(pTest);
    return pTest;
}

/*--------------------------------------------------------------------------*/
PegThing *PegList::PageUp(void)
{
    PegThing *pTest = mpLastSelected;

    if (!pTest)
    {
        return NULL;
    }

    WORD wMeasure = 0;

    if (pTest)
    {
        if (Type() == TYPE_HLIST)
        {
            wMeasure = pTest->mReal.Width();
            // figure out which item to select

            while(pTest->Previous() && wMeasure < mClient.Width())
            {
                pTest = pTest->Previous();
                if (pTest->StatusIs(PSF_NONCLIENT))
                {
                    if (pTest->Previous())
                    {
                        pTest = pTest->Previous();
                    }
                    else
                    {
                        pTest = pTest->Next();
                        break;
                    }
                }
                wMeasure += pTest->mReal.Width() + miChildSeperation;
            }
        }
        else
        {
            wMeasure = pTest->mReal.Height();
            while(pTest->Previous() && wMeasure < mClient.Height())
            {
                pTest = pTest->Previous();
                if (pTest->StatusIs(PSF_NONCLIENT))
                {
                    if (pTest->Previous())
                    {
                        pTest = pTest->Previous();
                    }
                    else
                    {
                        pTest = pTest->Next();
                        break;
                    }
                }
                wMeasure += pTest->mReal.Height() + miChildSeperation;
            }
        }
    }
    SetSelected(pTest);
    return pTest;
}

/*--------------------------------------------------------------------------*/
SIGNED PegList::GetNumItems(void)
{
    SIGNED iRetVal = 0;

    PegThing* pThing = First();

    while(pThing)
    {
        if(!(pThing->StatusIs(PSF_NONCLIENT)))
        {
            iRetVal++;
        }
        pThing = pThing->Next();
    }

    return(iRetVal);
}

/*--------------------------------------------------------------------------*/
void PegList::SetSelected(PegThing *Who)
{
    if (!Who)
    {
        mpLastSelected = NULL;
        return;
    }
    PegThing *pTest = First();

    while(pTest)
    {
        if (pTest == Who)
        {
            mpLastSelected = Who;
            if (StatusIs(PSF_VISIBLE))
            {
                ScrollCurrentIntoView();
                Presentation()->MoveFocusTree(Who);
            }
            return;
        }
        pTest = pTest->Next();
    }
    mpLastSelected = NULL;
}

/*--------------------------------------------------------------------------*/
PegThing *PegList::SetSelected(SIGNED iIndex)
{
    if (iIndex < 0)
    {
        mpLastSelected = NULL;
        return NULL;
    }

    PegThing *Current = First();

    while(Current)
    {
        if (!(Current->StatusIs(PSF_NONCLIENT)))
        {
            if (!iIndex)
            {
                mpLastSelected = Current;
                if (StatusIs(PSF_VISIBLE))
                {
                    ScrollCurrentIntoView();
                    Presentation()->MoveFocusTree(Current);
                }
                return Current;
            }
            iIndex--;
        }
        Current = Current->Next();
    }
    return NULL;
}

/*--------------------------------------------------------------------------*/
SIGNED PegList::Clear()
{
    SIGNED iNumDel = 0;
    PegThing* pDel = First();

    if(!pDel)
    {
        return(iNumDel);
    }

    PegThing* pNext;

    while(pDel)
    {
        pNext = pDel->Next();

        if(!(pDel->StatusIs(PSF_NONCLIENT)))
        {
            Remove(pDel, FALSE);
            Destroy(pDel);
        }
                
        pDel = pNext;
        iNumDel++;
    }

    PegWindow::CheckAutoScroll();

    return(iNumDel);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// PegVertList class implentation
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegVertList::PegVertList(const PegRect &Rect, WORD wId, WORD wStyle) : 
    PegList(Rect, wId, wStyle)
{
    Type(TYPE_VLIST);
}

/*--------------------------------------------------------------------------*/
void PegVertList::PositionChildren(void)
{
    PegRect NewPos = mClient;
    PegThing *pChild = First();
    
    while(pChild)
    {
        if (!(pChild->StatusIs(PSF_NONCLIENT)))
        {
            NewPos.wBottom = NewPos.wTop + pChild->mReal.Height() - 1;
            pChild->Resize(NewPos);
            pChild->SetSignals((WORD) (pChild->GetSignals()|SIGMASK(PSF_FOCUS_RECEIVED)));
            NewPos.wTop = NewPos.wBottom + miChildSeperation;
        }
        pChild = pChild->Next();
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// PegHorzList class implentation
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PegHorzList::PegHorzList(const PegRect &Rect, WORD wId, WORD wStyle) : 
    PegList(Rect, wId, wStyle)
{
    Type(TYPE_HLIST);
}

/*--------------------------------------------------------------------------*/
void PegHorzList::PositionChildren(void)
{
    PegRect NewPos = mClient;
    PegThing *pChild = First();
    
    while(pChild)
    {
        if (!(pChild->StatusIs(PSF_NONCLIENT)))
        {
            NewPos.wRight = NewPos.wLeft + pChild->mReal.Width() - 1;
            pChild->Resize(NewPos);
            pChild->SetSignals((WORD) (pChild->GetSignals()|SIGMASK(PSF_FOCUS_RECEIVED)));
            NewPos.wLeft = NewPos.wRight + miChildSeperation;
        }
        pChild = pChild->Next();
    }
}


// End of file
