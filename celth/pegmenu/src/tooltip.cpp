/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: ToolTip
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "wbstring.hpp"
#include "tooltip.hpp"

#include "tvprompt.hpp"
#include "usertype.hpp"


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ToolTip::ToolTip(SIGNED iLeft, SIGNED iTop, const PEGCHAR *Text) : 
    PegWindow(FF_NONE)
{
	PegRect ChildRect;

    mReal.Set(iLeft, iTop, iLeft + 249, iTop + 49);
	m_pFont = PegTextThing::GetDefaultFont(PEG_PROMPT_FONT);
	mReal.wRight = mReal.wLeft + Screen()->TextWidth(Text, m_pFont);
    InitClient();
    SetColor(PCI_NORMAL, LIGHTGRAY);
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);
	
    ChildRect.Set(mReal.wLeft, mReal.wTop, mReal.wRight, mReal.wBottom);
	//you can set font here, but font must be same with parent's.
    m_pPromptTip = new TVPrompt(ChildRect, Text, 0, FF_NONE|AF_TRANSPARENT|TJ_CENTER, m_pFont);
    m_pPromptTip->SetColor(PCI_NTEXT, BLUE);
    Add(m_pPromptTip);
    /* WB End Construction */
	Id(PROMPT_ID);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED ToolTip::Message(const PegMessage &Mesg)
{
    switch (Mesg.wType)
    {
    default:
        return PegWindow::Message(Mesg);
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void ToolTip::Draw(void)
{
    BeginDraw();	
    DrawChildren();
    EndDraw();
}


void ToolTip::SetTipString(const PEGCHAR * pcTip)
{
	//font has been set to be the same with child's
	PegRect rectPrompt;
    rectPrompt.Set(mReal.wLeft, mReal.wTop, mReal.wRight, mReal.wBottom);
	m_pPromptTip->Resize(rectPrompt);
	m_pPromptTip->DataSet(pcTip);

	//caputure previous graphics
	Screen()->Capture(&m_Capture, mReal);

	Draw();
}

SIGNED ToolTip::GetTextWidth(const PEGCHAR * pcTip)
{
	return Screen()->TextWidth(pcTip, m_pFont);
}

void ToolTip::DeleteTipGraphics()
{
	Invalidate();
	BeginDraw();
	Screen()->Restore(this, &m_Capture);
	EndDraw();
}

void ToolTip::SetFont(PegFont * pFont)
{
	m_pFont = pFont;
	m_pPromptTip->SetFont(m_pFont);
}
