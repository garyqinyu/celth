/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Source file originally created by PegWindowBuilder
//  
//  
//  Class Name: TVHelpWnd
//  
//  Notes:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"
#include "wbstring.hpp"
#include "helpwnd.hpp"
#include "usertype.hpp"

#include "anim.hpp"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//  Constructor
//  Create top level object and add all children
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
TVHelpWnd::TVHelpWnd(SIGNED iLeft, SIGNED iTop) : 
    PegWindow(FF_NONE)
{
    PegRect ChildRect;
    PegThing *pChild1;

    mReal.Set(iLeft, iTop, iLeft + 235, iTop + 124);
    InitClient();
    SetColor(PCI_NORMAL, LIGHTBLUE);
    RemoveStatus(PSF_MOVEABLE|PSF_SIZEABLE);

    ChildRect.Set(iLeft + 3, iTop + 6, iLeft + 20, iTop + 21);
    pChild1 = new PegIcon(ChildRect, &gbhelpBitmap);
    Add(pChild1);

    ChildRect.Set(iLeft + 29, iTop + 2, iLeft + 105, iTop + 23);
    m_pPromptTitle = new PegPrompt(ChildRect, LS(SID_Help_Prompt_Help), 0, FF_NONE|AF_TRANSPARENT|TJ_CENTER);
    ((PegPrompt *) m_pPromptTitle)->SetColor(PCI_NTEXT, YELLOW);
    Add(m_pPromptTitle);

    ChildRect.Set(iLeft + 0, iTop + 26, iLeft + 233, iTop + 125);
    m_pTextHelpString = new PegTextBox(ChildRect, ID_Text_HelpString, FF_NONE|TT_COPY|EF_WRAP, NULL);
    m_pTextHelpString->SetColor(PCI_NORMAL, LIGHTBLUE);
    //m_pTextHelpString->SetScrollMode(WSM_AUTOVSCROLL|WSM_AUTOHSCROLL);
    Add(m_pTextHelpString);

    /* WB End Construction */

	SetFont(PegTextThing::GetDefaultFont(PEG_TBUTTON_FONT));
	m_pTextHelpString->SetColor(PCI_NTEXT, YELLOW);

	Id(HELP_ID);
	m_pcText = new PEGCHAR[NEWMENU_HELPWND_MAXPEGCHAR];
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIGNED TVHelpWnd::Message(const PegMessage &Mesg)
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
void TVHelpWnd::Draw(void)
{
    BeginDraw();
	PegColor color(LIGHTBLUE, LIGHTBLUE, CF_FILL);
	Rectangle(mReal, color, 1);
    DrawChildren();
    EndDraw();
}

void TVHelpWnd::ShowHelpString(const PEGCHAR * pcHelpString, int iType)
{
	const PEGCHAR * pcTemp = NULL;

	switch(iType)
	{
	case 0:
		{			
			pcTemp = NULL;
			break;
		}
	case NEWMENU_BUTTON_TYPE:
		{
			pcTemp = LS(SID_Help_TextBox_ButtonComment);
			break;
		}
	case NEWMENU_BAR_TYPE:
		{
			pcTemp = LS(SID_Help_TextBox_BarComment);
			break;
		}
	case NEWMENU_Select_TYPE:
		{
			pcTemp = LS(SID_Help_TextButton_SelectComment);
	/*		m_pTextHelpString->DataSet(pcTemp);
			m_pTextHelpString->Invalidate();
			Draw();
			return;*/
			break;
		}
	default:
		{
			//assert(FALSE);
		}
	}
	
//	PegStrCpy(m_pcText, pcHelpString);
	if(NULL != pcTemp)
	{
//		PegStrCat(m_pcText, pcTemp);
	}
	m_pTextHelpString->DataSet(m_pcText);
	m_pTextHelpString->Invalidate();
	Draw();
	return;	
}

TVHelpWnd::~TVHelpWnd()
{
	if(NULL != m_pcText)
	{
		delete []m_pcText;
		m_pcText = NULL;
	}
}

void TVHelpWnd::SetFont(PegFont * pFont)
{
	m_pTextHelpString->SetFont(pFont);
	m_pPromptTitle->SetFont(pFont);
	m_pPromptTitle->DataSet(LS(SID_Help_Prompt_Help));
}

